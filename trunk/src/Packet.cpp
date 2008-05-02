#include	"stdafx.h"
#include	"Packet.h"

CPacket::~CPacket()
{
	Close();
}

BOOL CPacket::init()
{
	ULONG	buflen;
	PIP_ADAPTER_INFO	pAdInfo;

	buflen = 0;
	pAdInfo = (struct _IP_ADAPTER_INFO *)new UCHAR[1000];
	GetAdaptersInfo(pAdInfo, &buflen);
	delete pAdInfo;
	pAdInfo = (struct _IP_ADAPTER_INFO *)new UCHAR[buflen];
	nAdapterCount = 0;
	if (GetAdaptersInfo(pAdInfo, &buflen) == ERROR_SUCCESS)
	{
		do
		{
			Adapters[nAdapterCount].AdapterDescription = pAdInfo->Description;
			Adapters[nAdapterCount].AdapterString = pAdInfo->AdapterName;
			Adapters[nAdapterCount].AdapterString = "\\Device\\NPF_" + Adapters[nAdapterCount].AdapterString;
			Adapters[nAdapterCount].Type = pAdInfo->Type;
			if (pAdInfo->AddressLength != 0)
			{
				Adapters[nAdapterCount].AddressLength = pAdInfo->AddressLength;
				Adapters[nAdapterCount].Address = "";
				for (int i = 0; i < (int)Adapters[nAdapterCount].AddressLength; i++)
				{
					CString temp;
					temp.Format(" %02X", pAdInfo->Address[i]);
					Adapters[nAdapterCount].Address += temp;
				}
			}
			Adapters[nAdapterCount].ip_str = pAdInfo->IpAddressList.IpAddress.String;
			Adapters[nAdapterCount].ip = inet_addr(Adapters[nAdapterCount].ip_str);
			Adapters[nAdapterCount].mask_str = pAdInfo->IpAddressList.IpMask.String;
			Adapters[nAdapterCount].mask = inet_addr(Adapters[nAdapterCount].mask_str);
			Adapters[nAdapterCount].gateway_str = pAdInfo->GatewayList.IpAddress.String;
			nAdapterCount++;
		//} while (pAdInfo->Next != NULL);
		} while ((pAdInfo = pAdInfo->Next) != NULL);
	}
	else
	{
		AfxMessageBox("could not obtain info about adapters!");
	}
	delete pAdInfo;


	return TRUE;
}

BOOL CPacket::Close()
{
	int i;
	for (i=0; i<nAdapterCount; i++)
	{
		CloseAdapter(i);
		if (Adapters[i].pBuffer != NULL)
		{
			delete Adapters[i].pBuffer;
			Adapters[i].pBuffer = NULL;
		}
		if (Adapters[i].pPacket != NULL)
		{
			PacketFreePacket(Adapters[i].pPacket);
			Adapters[i].pPacket = NULL;
		}
	}
	return TRUE;
}

BOOL CPacket::Open(int i, DWORD bufsize, DWORD kernelbuf, BOOL promiscuous)
{
	if (i<0)
	{
		//search for a adapter and use the first valid found
		i=0;
		while (!IsValidIPAdapter(i)&&(i<=nAdapterCount))
			i++;
	}
	//check if given Adapter is available
	if (i>=nAdapterCount)
	{
		AfxMessageBox("no valid adapter found!");
		return FALSE;						//invalid adapter number or not initialized yet
	}
	nActiveAdapter = i;						//save the number of the selected adapter

	//try to open the adapter
	Adapters[nActiveAdapter].pAdapter = OpenAdapter(nActiveAdapter);			//open the adapter
	if (Adapters[nActiveAdapter].pAdapter == NULL)
	{
		AfxMessageBox("could not open Adapter in CPacket::Open()");
		return FALSE;						//could not open adapter
	}
	
	//set the size of the kernel buffer
	if ( PacketSetBuff(Adapters[nActiveAdapter].pAdapter, kernelbuf)==FALSE )
	{
		AfxMessageBox("not enough memory for kernel buffer!");
		return FALSE;						//not enough memory to allocate kernel buffer
	}

	if (promiscuous)
		PacketSetHwFilter(Adapters[nActiveAdapter].pAdapter, NDIS_PACKET_TYPE_PROMISCUOUS);
	else
		PacketSetHwFilter(Adapters[nActiveAdapter].pAdapter, NDIS_PACKET_TYPE_DIRECTED);
	PacketSetReadTimeout(Adapters[nActiveAdapter].pAdapter, 500);

	//allocate and initialize the application buffer (packet buffer)
	Adapters[nActiveAdapter].pPacket = PacketAllocatePacket();
	if (Adapters[nActiveAdapter].pPacket == NULL)
	{
		AfxMessageBox("not enough memory for packet structure");
		return FALSE;						//could not allocate packet structure
	}

	Adapters[nActiveAdapter].pBuffer = new CHAR[bufsize];
	if (Adapters[nActiveAdapter].pBuffer == NULL)
	{
		AfxMessageBox("not enough memory for packet buffer");
		return FALSE;						//could not allocate enough memory
	}

	PacketInitPacket(Adapters[nActiveAdapter].pPacket, Adapters[nActiveAdapter].pBuffer, bufsize);

	//start the thread which read data from the adapter
	Start();								//method of the base class CThread


	return TRUE;
}

LPADAPTER CPacket::OpenAdapter(int i)
{
	return PacketOpenAdapter((char*)(LPCTSTR)Adapters[i].AdapterString);
}

void CPacket::CloseAdapter(int i)
{
	if (Adapters[i].pAdapter != NULL)
	{
		PacketCloseAdapter(Adapters[i].pAdapter);
		Adapters[i].pAdapter = NULL;
	}
}


//BOOL CPacket::GetNetInfo(int i)
//{
//	BOOL s;
//	s = PacketGetNetInfo((char*)(LPCTSTR)Adapters[i].AdapterString, &Adapters[i].ip, &Adapters[i].mask);
//
//	return s;
//}
//
BOOL CPacket::IsValidIPAdapter(int i)
{
	//to be valid, an adapter must have an ip address other than 0.0.0.0
	//and must not be a loopback device (127.0.0.1)
	return ((Adapters[i].ip != 0)&&(Adapters[i].ip != 0x7f000001));
}

/**
 * reads the packets from the driver. calls AnalyzePackets() for every
 * packet received.
 */
DWORD CPacket::ThreadMethod()
{
#define BPF_ALIGNMENT sizeof(long)
#define BPF_WORDALIGN(x) (((x)+(BPF_ALIGNMENT-1))&~(BPF_ALIGNMENT-1))

	while ((m_runthread)&&(nActiveAdapter>=0))				//member of the base class!
	{
		WaitForSingleObject(Adapters[nActiveAdapter].pAdapter->ReadEvent,500);
		if (PacketReceivePacket(Adapters[nActiveAdapter].pAdapter,
								Adapters[nActiveAdapter].pPacket, TRUE) )
		{
			if (Adapters[nActiveAdapter].pPacket->ulBytesReceived != 0)
			{
				int packetlength;
				packetbegin = (unsigned char *)Adapters[nActiveAdapter].pPacket->Buffer;
				packetlength = Adapters[nActiveAdapter].pPacket->ulBytesReceived;

				packetend = packetbegin + packetlength;
				while ( packetbegin < packetend )
				{
					int caplen, hdrlen;
					caplen = ((struct bpf_hdr *)packetbegin)->bh_caplen;
					hdrlen = ((struct bpf_hdr *)packetbegin)->bh_hdrlen;
					AnalyzePackets(packetbegin, packetbegin + hdrlen);		//call the virtual method
					packetbegin += BPF_WORDALIGN(caplen + hdrlen);
				}
			}
		}
	}
	return 0;
}













