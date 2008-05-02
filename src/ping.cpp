#include "stdafx.h"
#include <afxpriv.h>
#include "ping.h"



#define MIN_ICMP_PACKET_SIZE 8    //minimum 8 byte icmp packet (just header)
#define MAX_ICMP_PACKET_SIZE 1024 //Maximum icmp packet size


#ifndef _WINSOCK2API_
#include <winsock2.h>
#include <ws2tcpip.h>
#endif



//Automatically use the Winsock 2 library
#pragma comment(lib, "ws2_32.lib")





#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




int CPing::Ping(LPCTSTR pszHostName, CPingReply& pr, int nPings, UCHAR nTTL, DWORD dwTimeout, int nPacketSize, UCHAR nTOS, BOOL bDontFragment)
{
	pr.nError = -1;
	pr.minRTT = ULONG_MAX;
	pr.maxRTT = 0;
	pr.avgRTT = 0;

	//For correct operation of the T2A macro, see TN059
	USES_CONVERSION;

	//Use the High performace counter to get an accurate RTT
	LARGE_INTEGER Frequency;
	Frequency.QuadPart = 0;
	if (!QueryPerformanceFrequency(&Frequency))
	{
		TRACE(_T("Failed to get the high performance counter frequency\n"));
		return -1;
	}
	__int64 nTimerFrequency = Frequency.QuadPart;

	//Resolve the address of the host to connect to
	sockaddr_in dest;
	memset(&dest,0,sizeof(dest));
	LPSTR lpszAscii = T2A((LPTSTR) pszHostName);
	unsigned long addr = inet_addr(lpszAscii);
	if (addr == INADDR_NONE)
	{
		//Not a dotted address, then do a lookup of the name
		hostent* hp = gethostbyname(lpszAscii);
		if (hp)
		{
			memcpy(&(dest.sin_addr),hp->h_addr,hp->h_length);
			dest.sin_family = hp->h_addrtype;
		}
		else
		{
			TRACE(_T("CPing::PingUsingWinsock, Could not resolve the host name %s\n"), pszHostName);
			CString temp;
			temp.Format("Could not resolve host name \"%s\" ", pszHostName);
			AfxMessageBox(temp);
			return -1;
		}
	}
	else
	{
		dest.sin_addr.s_addr = addr;
		dest.sin_family = AF_INET;
	}

	//Create the raw socket
	SOCKET sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, 0);
	if (sockRaw == INVALID_SOCKET) 
	{
		TRACE(_T("CPing::PingUsingWinsock, Failed to create a raw socket\n"));
		return -1;
	}

	SOCKADDR_IN sockLocalAddress;
	ZeroMemory(&sockLocalAddress, sizeof(sockLocalAddress));
	sockLocalAddress.sin_family = AF_INET;
	sockLocalAddress.sin_port = htons((u_short)0);

	LPHOSTENT lphost;
	lphost = gethostbyname(NULL);
	if (lphost != NULL)
		sockLocalAddress.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
	else
	{
		return -1; 
	}

	if (bind(sockRaw, (sockaddr*) &sockLocalAddress, sizeof(sockLocalAddress)) == SOCKET_ERROR)
    {
		TRACE(_T("CPing::PingUsingWinsock, Failed to bind to specified address\n"));
		return -1;
    }

	//Set the TTL on the socket  
	int nTempTTL = nTTL;
	if (setsockopt(sockRaw, IPPROTO_IP, IP_TTL, (char*) &nTempTTL, sizeof(nTempTTL)) == SOCKET_ERROR)
	{
		TRACE(_T("CPing::PingUsingWinsock, Failed to set the TTL value on the socket\n"));
		return -1;
	}

	//Set the TOS on the socket  
	int nTempTos = nTOS;
	if (setsockopt(sockRaw, IPPROTO_IP, IP_TOS, (char*) &nTempTos, sizeof(nTempTos)) == SOCKET_ERROR)
	{
		TRACE(_T("CPing::PingUsingWinsock, Failed to set the Tos value on the socket\n"));
		return -1;
	}

	//Set the Don't Fragment flag on the socket  
	if (bDontFragment)
	{
		if (setsockopt(sockRaw, IPPROTO_IP, IP_DONTFRAGMENT, (char*) &bDontFragment, sizeof(bDontFragment)) == SOCKET_ERROR)
		{
			TRACE(_T("CPing::PingUsingWinsock, Failed to set the Don't Fragment value on the socket\n"));
			return -1;
		}
	}
 
	//Allocate the ICMP packet
	int nBufSize = nPacketSize + sizeof(ICMP_HEADER);
	char* pICMP = new char[nBufSize];
	FillIcmpData((LPICMP_HEADER) pICMP, nBufSize);

	//Get the tick count prior to sending the packet
	LARGE_INTEGER TimerTick;
	VERIFY(QueryPerformanceCounter(&TimerTick));
	__int64 nStartTick = TimerTick.QuadPart;

	for (int i=0; i<nPings; i++)
	{
		//Send of the packet
		int nWrote = sendto(sockRaw, pICMP, nBufSize, 0, (sockaddr*)&dest, sizeof(dest));
		if (nWrote == SOCKET_ERROR)
		{
			TRACE(_T("CPing::PingUsingWinsock, sendto failed\n"));

			delete [] pICMP;

			DWORD dwError = GetLastError();
			closesocket(sockRaw);
			SetLastError(dwError);

			return -1;
		}

		//allocate the recv buffer
		char* pRecvBuf = new char[MAX_ICMP_PACKET_SIZE];
		BOOL bReadable;
		sockaddr_in from;
		int nFromlen = sizeof(from);
		int nRead = 0;

		//Allow the specified timeout
		if (IsSocketReadible(sockRaw, dwTimeout, bReadable))
		{
			if (bReadable)
			{
				//Receive the response
				nRead = recvfrom(sockRaw, pRecvBuf, MAX_ICMP_PACKET_SIZE, 0, (sockaddr*)&from, &nFromlen);
			}
			else
			{
				TRACE(_T("CPing::PingUsingWinsock, timeout occured while awaiting recvfrom\n"));
				closesocket(sockRaw);

				delete [] pICMP;
				delete [] pRecvBuf;

				//set the error to timed out
				SetLastError(WSAETIMEDOUT);

				return -1;
			}
		}
		else
		{
			TRACE(_T("CPing::PingUsingWinsock, IsReadible call failed\n"));

			delete [] pICMP;
			delete [] pRecvBuf;

			DWORD dwError = GetLastError();
			closesocket(sockRaw);
			SetLastError(dwError);

			return -1;
		}

		//Get the current tick count
		QueryPerformanceCounter(&TimerTick);

		//Now check the return response from recvfrom
		if (nRead == SOCKET_ERROR)
		{
			TRACE(_T("CPing::PingUsingWinsock, recvfrom call failed\n"));

			delete [] pICMP;
			delete [] pRecvBuf;

			DWORD dwError = GetLastError();
			closesocket(sockRaw);
			SetLastError(dwError);

			return -1;
		}

		//Decode the response we got back
		pr.nError = DecodeResponse(pRecvBuf, nRead, &from);

		pr.Address = from.sin_addr;
		ULONG rtt = (ULONG) ((TimerTick.QuadPart - nStartTick) * 1000 / nTimerFrequency);
		pr.avgRTT += rtt;
		if (pr.maxRTT < rtt)
			pr.maxRTT = rtt;
		if (pr.minRTT > rtt)
			pr.minRTT = rtt;
		delete [] pRecvBuf;
	}
	pr.avgRTT /= nPings;
	//Don't forget to release out socket
	closesocket(sockRaw);

	//Free up the memory we allocated
	delete [] pICMP;

	//return the status
	return pr.nError;
}

BOOL CPing::IsSocketReadible(SOCKET socket, DWORD dwTimeout, BOOL& bReadible)
{
  timeval timeout = {dwTimeout/1000, dwTimeout % 1000};
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(socket, &fds);
  int nStatus = select(0, &fds, NULL, NULL, &timeout);
  if (nStatus == SOCKET_ERROR)
  {
    return FALSE;
  }
  else
  {
    bReadible = !(nStatus == 0);
    return TRUE;
  }
}



//Decode the raw IP packet we get back
int CPing::DecodeResponse(char* pBuf, int nBytes, sockaddr_in* from) 
{
	//Get the current tick count
	LARGE_INTEGER TimerTick;
	VERIFY(QueryPerformanceCounter(&TimerTick));


	LPIP_HEADER pIpHdr = (LPIP_HEADER) pBuf;
	int nIpHdrlen = pIpHdr->h_len * 4; //Number of 32-bit words*4 = bytes

	//Not enough data recieved
	if (nBytes < nIpHdrlen + MIN_ICMP_PACKET_SIZE)
	{
		TRACE(_T("Received too few bytes from %sin ICMP packet\n"), inet_ntoa(from->sin_addr));
		SetLastError(ERROR_UNEXP_NET_ERR);
		return -1;
	}

	//Check it is an ICMP_ECHOREPLY packet
	LPICMP_HEADER pIcmpHdr = (LPICMP_HEADER) (pBuf + nIpHdrlen);
	if (pIcmpHdr->i_type != 0) //type ICMP_ECHOREPLY is 0
	{
		return ((int)pIcmpHdr->i_type);
	}

	//Check it is the same id as we sent
	if (pIcmpHdr->i_id != (USHORT)GetCurrentProcessId()) 
	{
		TRACE(_T("Received someone else's ICMP packet!\n"));
		SetLastError(ERROR_UNEXP_NET_ERR);
		return -1;
	}
	return pIcmpHdr->i_type;
}

//generate an IP checksum based on a given data buffer
USHORT CPing::GenerateIPChecksum(USHORT* pBuffer, int nSize) 
{
	unsigned long cksum = 0;

	while (nSize > 1) 
	{
		cksum += *pBuffer++;
		nSize -= sizeof(USHORT);
	}

	if (nSize) 
		cksum += *(UCHAR*)pBuffer;

	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >>16);
	return (USHORT)(~cksum);
}

//Fill up the ICMP packet with defined values
void CPing::FillIcmpData(LPICMP_HEADER pIcmp, int nData)
{
	pIcmp->i_type    = 8; //ICMP_ECHO type
	pIcmp->i_code    = 0;
	pIcmp->i_id      = (USHORT) GetCurrentProcessId();
	pIcmp->i_seq     = 0;
	pIcmp->i_cksum   = 0;
	pIcmp->timestamp = GetTickCount();

	//Set up the data which will be sent
	int nHdrSize = sizeof(ICMP_HEADER);
	char* pData = ((char*)pIcmp) + nHdrSize;
	memset(pData, 'E', nData - nHdrSize);

	//Generate the checksum
	pIcmp->i_cksum = GenerateIPChecksum((USHORT*)pIcmp, nData);
}

