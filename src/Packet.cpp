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
	}
	return TRUE;
}

BOOL CPacket::Open(int i, DWORD bufsize, DWORD kernelbuf, BOOL promiscuous)
{
	pcap_if_t *alldevs;
	pcap_if_t *d;
	char errbuf[PCAP_ERRBUF_SIZE];

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
		AfxMessageBox(_T("no valid adapter found!"));
		return FALSE;						//invalid adapter number or not initialized yet
	}
	nActiveAdapter = i;						//save the number of the selected adapter

	// list all available adapters
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
	{
		AfxMessageBox(errbuf);
		return FALSE;
	}

	for(d=alldevs; d; d=d->next)
	{
		if (d->description)
			TRACE(" (%s)\n", d->description);
		else
			TRACE(" (No description available)\n");
	}

	// Jump to the selected adapter
	int j = 0;
	for(d=alldevs, j=0; j<i;d=d->next, j++);

	// Open the device
	if ( (Adapters[nActiveAdapter].pAdapter = pcap_open(d->name,          // name of the device
														65536,            // portion of the packet to capture
														// 65536 guarantees that the whole packet will be captured on all the link layers
														PCAP_OPENFLAG_PROMISCUOUS,    // promiscuous mode
														1000,             // read timeout
														NULL,             // authentication on the remote machine
														errbuf            // error buffer
														) ) == NULL)
	{
		AfxMessageBox(_T("Unable to open the adapter."));
		// Free the device list
		pcap_freealldevs(alldevs);
		return FALSE;
	}

	// At this point, we don't need any more the device list. Free it
	pcap_freealldevs(alldevs);

	Start();								//method of the base class CThread


	return TRUE;
}

void CPacket::CloseAdapter(int i)
{
	if (Adapters[i].pAdapter != NULL)
	{
		//PacketCloseAdapter(Adapters[i].pAdapter);
		Adapters[i].pAdapter = NULL;
	}
}


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
	pcap_loop(Adapters[nActiveAdapter].pAdapter, 0, packet_handler, (u_char *)this);

	return 0;
}

void CPacket::packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
	CPacket * pThis = (CPacket*)param;
	pThis->AnalyzePackets(pkt_data, pkt_data + header->len);
}













