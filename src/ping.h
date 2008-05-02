


#ifndef __PING_H__
#define __PING_H__

#include "winsock2.h"
#include "ws2tcpip.h"
#include "ipstructs.h"



/**
 * CPingReply
 * structure to hold the answer to a ping. see CPing for details.
 */
struct CPingReply
{
	in_addr	 Address;		//The IP address of the replier
	unsigned long avgRTT;	//Round Trip time in Milliseconds
	unsigned long maxRTT;	
	unsigned long minRTT;
	int nError;
};

/**
 * CPing.
 * class to perform a ping on other computers with ICMP-messages.
 * requires Win2k or later because it uses raw-sockets.
 */
class CPing
{
public:

    /*!
     * performs a Ping on another host via ICMP protocoll.
     *
     * @param pszHostName : the name of the host. either as a "raw" ip-address or as a dns-name.
     * @param pr : the CPingReply which holds the answer
	 * @param nPings : number of pings to perform on host
     * @param nTTL : the time to live. default value is 50
     * @param dwTimeout : the timeout in milliseconds. default is 5000 (5 seconds)
     * @param nPacketSize : the size of the ping packet. default is 32
     * @param nTOS : type of service. default is 0 for Ping
     * @param bDontFragment : set to TRUE if you want prevent a fragmentation of the ping datapacket. default is FALSE
     *
     * @return int  : returns the type of the ICMP-answer (e.g. 0 for echo-repy, 11 for ttl timeout) or -1 if failed
     */
	static int Ping(LPCTSTR pszHostName, CPingReply& pr, int nPings = 1, UCHAR nTTL = 50, DWORD dwTimeout = 5000, int nPacketSize = 32, UCHAR nTOS = 0, BOOL bDontFragment = FALSE);

private:
	static BOOL IsSocketReadible(SOCKET socket, DWORD dwTimeout, BOOL& bReadible);
	static USHORT	GenerateIPChecksum(USHORT* pBuffer, int nSize);
	static void	FillIcmpData(LPICMP_HEADER pIcmp, int nData);
	static int	DecodeResponse(char* pBuf, int nBytes, sockaddr_in* from) ;
};



#endif //__PING_H__

