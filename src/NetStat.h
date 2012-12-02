// NetStat.h: interface for the CNetStat class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

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
