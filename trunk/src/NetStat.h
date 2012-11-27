// NetStat.h: interface for the CNetStat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETSTAT_H__00C4B51E_F411_4FFF_B359_8808E6AFF267__INCLUDED_)
#define AFX_NETSTAT_H__00C4B51E_F411_4FFF_B359_8808E6AFF267__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsock2.h>
#include <iphlpapi.h>

#define SIO_RCVALL  0x98000001
#define MAX_IP_SIZE 65535

class CNetStat
{
public:
    CNetStat();
    virtual ~CNetStat();
    BOOL    initialize();
    BOOL    GetPacket(WSABUF *wbuf);
    BOOL    AnalyzePacket(WSABUF *wbuf);
    BOOL    GetAdapter(SOCKET s, SOCKADDR_IN *ifx, int num);
    BOOL    GetAdapterList(SOCKET_ADDRESS_LIST *slist);


public:
    IP_ADAPTER_INFO     HostInfo;
    BOOL    threadactive;
    DWORD64 sentbytes;
    DWORD64 receivedbytes;
private:
    SOCKET  sock;
    ULONG   ip;
    DWORD   ThreadID;

};


//forward declaration
UINT    sniff(LPVOID pParam);

#endif // !defined(AFX_NETSTAT_H__00C4B51E_F411_4FFF_B359_8808E6AFF267__INCLUDED_)
