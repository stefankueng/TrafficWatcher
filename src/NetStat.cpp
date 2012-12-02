// NetStat.cpp: implementation of the CNetStat class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "trafficwatch.h"
#include "NetStat.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetStat::CNetStat()
{
}

CNetStat::~CNetStat()
{
}

BOOL    CNetStat::initialize()
{
    unsigned int        rbuf;           //just a dummy
    DWORD               dwBytesRet;
    SOCKET_ADDRESS_LIST slist;
    SOCKADDR_IN         sadr;
    ULONG               adaptSize;


    //create a WSASocket for listening (receiving) ip data
    sock=WSASocket(AF_INET,SOCK_RAW,IPPROTO_IP,NULL,0,WSA_FLAG_OVERLAPPED);
    if(sock==INVALID_SOCKET)
    {
        MessageBox(NULL,"Creation of Socket(SOCK_RAW/IPPROTO_IP) failed","Alert!",MB_OK);
        return FALSE;
    }

    GetAdapterList(&slist);
    //if(GetAdapter(sock,&sadr,AdapterNr)!=0)
    if(GetAdapter(sock,&sadr,0)!=TRUE)
    {
        MessageBox(NULL,"Unable to obtain selected network adapter!","Alert!",MB_OK);
        return FALSE;
    }
    //get some information about this host
    adaptSize=sizeof(HostInfo);
    if (GetAdaptersInfo(&HostInfo, &adaptSize)!=ERROR_SUCCESS)
    {
        AfxMessageBox("no info on host!");
        return FALSE;
    }


    //now bind the socket to the system
    sadr.sin_family = AF_INET;                  //IP-protocoll
    sadr.sin_port = htons(0);                   //port 0
    if(bind(sock,(SOCKADDR *)&sadr,sizeof(sadr))==SOCKET_ERROR)
    {
        MessageBox(NULL,"Bind call failed!","Alert!",MB_OK);
        return FALSE;
    }

    rbuf=1;
    //set the mode of the socket
    if(WSAIoctl(sock,SIO_RCVALL,&rbuf,sizeof(rbuf),NULL,0,&dwBytesRet,NULL,NULL)==SOCKET_ERROR)
    {
        MessageBox(NULL,"WSAIoCtl(SIO_RCVALL) failed","Alert!",MB_OK);
        return FALSE;
    }
    //the ip-address of this host as a ulong
    ip = sadr.sin_addr.S_un.S_addr;

    //create the packet capturing thread
    threadactive = TRUE;
    AfxBeginThread(sniff, this);




    return TRUE;

}


BOOL CNetStat::GetPacket(WSABUF *wbuf)
{
    DWORD         dwBytesRet=0,dwFlags=0;


    if(SOCKET_ERROR==WSARecv(sock,wbuf,1,&dwBytesRet,&dwFlags,NULL,NULL))
    {
/*      char buf[200];
        sprintf(buf,"WSARecv failed. Code %d",WSAGetLastError(),sock);
        MessageBox(NULL,buf,"Alert!",MB_OK);
*/      return FALSE;
    }
    wbuf->len=dwBytesRet;

    return TRUE;
}

BOOL CNetStat::AnalyzePacket(WSABUF *wbuf)
{
    //source port at index 20, 21
    //destination port at index 22, 23
    //source ip at 12,13,14,15
    //destination ip at 16,17,18,19
    ULONG   sip;        //source ip
    ULONG   dip;        //dest ip

    sip = ((BYTE)wbuf->buf[15]<<24) + ((BYTE)wbuf->buf[14]<<16) + ((BYTE)wbuf->buf[13]<<8) + (BYTE)wbuf->buf[12];
    dip = ((BYTE)wbuf->buf[19]<<24) + ((BYTE)wbuf->buf[18]<<16) + ((BYTE)wbuf->buf[17]<<8) + (BYTE)wbuf->buf[16];


    if (sip==ip)
    {
        sentbytes += wbuf->len;
    }
    if (dip==ip)
    {
        receivedbytes += wbuf->len;
    }


    return TRUE;
}


BOOL CNetStat::GetAdapter(SOCKET s, SOCKADDR_IN *sadr, int num)
{
    SOCKET_ADDRESS_LIST *slist=NULL;
    char                 buf[2048];
    DWORD                dwBytesRet;
    if(SOCKET_ERROR==WSAIoctl(s, SIO_ADDRESS_LIST_QUERY,NULL,0,buf,2048,&dwBytesRet,NULL,NULL))
    {
        MessageBox(NULL,"WSAIoctl(SIO_ADDRESS_LIST_QUERY) failed","Alert!",MB_OK);
        return FALSE;
    }
    slist=(SOCKET_ADDRESS_LIST *)buf;
    if(num>=slist->iAddressCount)
        return FALSE;
    sadr->sin_addr.s_addr=((SOCKADDR_IN *)slist->Address[num].lpSockaddr)->sin_addr.s_addr;

    return TRUE;
}

BOOL CNetStat::GetAdapterList(SOCKET_ADDRESS_LIST *slist)
{
    SOCKET               sock;
    char                 buf[4096];
    DWORD                dwBytesRet;
    int                  ret;

    sock=socket(AF_INET,SOCK_STREAM,IPPROTO_IP);
    if(sock==SOCKET_ERROR)
        return 0;
    ret=WSAIoctl(sock,SIO_ADDRESS_LIST_QUERY,NULL,0,buf,4096,&dwBytesRet,NULL,NULL);
    if(ret==SOCKET_ERROR)
        return 0;
    memcpy(slist,(SOCKET_ADDRESS_LIST *)&buf,sizeof(SOCKET_ADDRESS_LIST));
    closesocket(sock);
    return 1;
}

UINT    sniff(LPVOID pParam)
{
    CNetStat* pNetStat = (CNetStat*)pParam;

    WSABUF mybuf;
    char rcvbuf[MAX_IP_SIZE];

    mybuf.buf=rcvbuf;

    while (pNetStat->threadactive)
    {
        mybuf.len=MAX_IP_SIZE;
        //memset(mybuf.buf,0x0,MAX_IP_SIZE);
        pNetStat->GetPacket(&mybuf);
        pNetStat->AnalyzePacket(&mybuf);
    }

    return 0;
}
