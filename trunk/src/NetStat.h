// TrafficWatcher - a network speed monitor

// Copyright (C) 2008, 2012 - Stefan Kueng

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

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
