// TrafficWatcher - a network speed monitor

// Copyright (C) 2008-2009, 2012 - Stefan Kueng

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

#include "stdafx.h"
#include "Packet.h"
#include "UnicodeUtils.h"


CPacket::~CPacket()
{
    Close();
}

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))


BOOL CPacket::init()
{
    ULONG buflen;
    PIP_ADAPTER_INFO pAdInfo;

    // we first get the adapters info for ipv4
    buflen = 0;
    pAdInfo = NULL;
    GetAdaptersInfo(pAdInfo, &buflen);
    pAdInfo = (struct _IP_ADAPTER_INFO *)new UCHAR[buflen];
    nAdapterCount = 0;
    if (GetAdaptersInfo(pAdInfo, &buflen) == ERROR_SUCCESS)
    {
        do
        {
            Adapters[nAdapterCount].AdapterDescription = pAdInfo->Description;
            Adapters[nAdapterCount].AdapterString = pAdInfo->AdapterName;
            Adapters[nAdapterCount].AdapterString = L"\\Device\\NPF_" + Adapters[nAdapterCount].AdapterString;
            Adapters[nAdapterCount].Type = pAdInfo->Type;
            if (pAdInfo->AddressLength != 0)
            {
                Adapters[nAdapterCount].AddressLength = pAdInfo->AddressLength;
                Adapters[nAdapterCount].Address = L"";
                for (int i = 0; i < (int)Adapters[nAdapterCount].AddressLength; i++)
                {
                    CString temp;
                    temp.Format(L" %02X", pAdInfo->Address[i]);
                    Adapters[nAdapterCount].Address += temp;
                }
            }
            Adapters[nAdapterCount].ip4_str = pAdInfo->IpAddressList.IpAddress.String;
            Adapters[nAdapterCount].ip4 = inet_addr(CUnicodeUtils::StdGetUTF8((LPCWSTR)Adapters[nAdapterCount].ip4_str).c_str());
            Adapters[nAdapterCount].mask_str = pAdInfo->IpAddressList.IpMask.String;
            Adapters[nAdapterCount].mask = inet_addr(CUnicodeUtils::StdGetUTF8((LPCWSTR)Adapters[nAdapterCount].mask_str).c_str());
            Adapters[nAdapterCount].gateway_str = pAdInfo->GatewayList.IpAddress.String;
            nAdapterCount++;
        } while ((pAdInfo = pAdInfo->Next) != NULL);
    }
    else
    {
        AfxMessageBox(L"could not obtain info about adapters!");
    }
    delete pAdInfo;

    PIP_ADAPTER_ADDRESSES AdapterAddresses = NULL;
    ULONG OutBufferLength = 0;
    ULONG RetVal = 0, i;

    for (i = 0; i < 5; i++)
    {
        RetVal = GetAdaptersAddresses(AF_UNSPEC, 0, NULL, AdapterAddresses, &OutBufferLength);

        if (RetVal != ERROR_BUFFER_OVERFLOW)
            break;

        if (AdapterAddresses != NULL)
            FREE(AdapterAddresses);

        AdapterAddresses = (PIP_ADAPTER_ADDRESSES)MALLOC(OutBufferLength);
        if (AdapterAddresses == NULL)
        {
            RetVal = GetLastError();
            break;
        }
    }

    if (RetVal == NO_ERROR)
    {
        PIP_ADAPTER_ADDRESSES AdapterList = AdapterAddresses;
        while (AdapterList)
        {
            // find the ip addresses
            IP_ADAPTER_UNICAST_ADDRESS * unicast = AdapterList->FirstUnicastAddress;
            int nIp4Adapter = -1;
            while (unicast)
            {
                if (unicast->Address.lpSockaddr->sa_family == AF_INET)
                {
                    sockaddr_in * s_in = (sockaddr_in*)unicast->Address.lpSockaddr;
                    for (int k = 0; k < nAdapterCount; ++k)
                    {
                        if (s_in->sin_addr.S_un.S_addr == Adapters[k].ip4)
                        {
                            nIp4Adapter = k;
                            break;
                        }
                    }
                }
                unicast = unicast->Next;
            }
            unicast = AdapterList->FirstUnicastAddress;
            if (nIp4Adapter >= 0)
            {
                while (unicast)
                {
                    if (unicast->Address.lpSockaddr->sa_family == AF_INET6)
                    {
                        memcpy_s(&Adapters[nIp4Adapter].ip6, sizeof(SOCKADDRIPV6), unicast->Address.lpSockaddr, sizeof(SOCKADDRIPV6));
                    }
                    unicast = unicast->Next;
                }
            }
            AdapterList = AdapterList->Next;
        }
    }
    else
    {
    }

    if (AdapterAddresses != NULL)
        FREE(AdapterAddresses);


    return TRUE;
}

BOOL CPacket::Close()
{
    int i;
    for (i=0; i<nAdapterCount; i++)
    {
        CloseAdapter(i);
    }
    return TRUE;
}

BOOL CPacket::Open(int i, DWORD /*bufsize*/, DWORD /*kernelbuf*/, BOOL /*promiscuous*/)
{
    pcap_if_t *alldevs;
    pcap_if_t *d;
    char errbuf[PCAP_ERRBUF_SIZE];
    struct bpf_program fcode;

    Stop(true);
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
        AfxMessageBox(L"no valid adapter found!");
        return FALSE;                       //invalid adapter number or not initialized yet
    }
    nActiveAdapter = i;                     //save the number of the selected adapter

    // list all available adapters
    if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
    {
        if (AfxMessageBox(L"Failed to connect to the NPF service!\nThis may be because the service is not running yet.\nDo you want to start the service?", MB_ICONQUESTION | MB_YESNO) == IDYES)
        {
            // maybe the npf service isn't running yet
            // start our own exe with elevated privileges to get the service started
            TCHAR szFilePath[MAX_PATH*2] = {0};
            int len = GetModuleFileName( AfxGetInstanceHandle( ), szFilePath, sizeof( szFilePath ) );

            int timeout = 0;
            if (len < (MAX_PATH*2))
            {
                SHELLEXECUTEINFO TempInfo = {0};
                TempInfo.cbSize = sizeof(SHELLEXECUTEINFO);
                TempInfo.fMask = 0;
                TempInfo.hwnd = NULL;
                TempInfo.lpVerb = L"runas";
                TempInfo.lpFile = szFilePath;
                TempInfo.lpParameters = L"/initwpcap";
                TempInfo.lpDirectory = NULL;
                TempInfo.nShow = SW_NORMAL;
                ::ShellExecuteEx(&TempInfo);
                // now wait for about 10 seconds to give our elevated process time
                // to start the npf service. If it isn't started after that time, we throw the error anyway.
                while (timeout < 100)
                {
                    if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
                    {
                        Sleep(100);
                    }
                    else
                        timeout = 200;
                    timeout++;
                }
            }

            if (timeout != 201)
            {
                AfxMessageBox(CUnicodeUtils::StdGetUnicode(errbuf).c_str());
                return FALSE;
            }
        }
        else
            return FALSE;
    }

    int pcapAdapterNr = i;
    int count = 0;
    for(d=alldevs; d; d=d->next)
    {
        // +8 to skip the rpcap:// part of the string
        if (Adapters[i].AdapterString.Compare(CUnicodeUtils::StdGetUnicode(d->name+8).c_str()) == 0)
        {
            pcapAdapterNr = count;
        }
        if (d->description)
            TRACE(" (%s)\n", d->description);
        else
            TRACE(" (No description available)\n");
        count++;
    }

    // Jump to the selected adapter
    int j = 0;
    for(d=alldevs, j=0; j<pcapAdapterNr;d=d->next, j++);

    // Open the device
    if ( (Adapters[nActiveAdapter].pAdapter = pcap_open(d->name,          // name of the device
                                                        65536,            // portion of the packet to capture
                                                        // 65536 guarantees that the whole packet will be captured on all the link layers
                                                        0,                // no promiscuous mode
                                                        1000,             // read timeout
                                                        NULL,             // authentication on the remote machine
                                                        errbuf            // error buffer
                                                        ) ) == NULL)
    {
        AfxMessageBox(L"Unable to open the adapter.");
        // Free the device list
        pcap_freealldevs(alldevs);
        return FALSE;
    }

    u_int netmask;
    if (d->addresses != NULL)
        // Retrieve the mask of the first address of the interface
        netmask=((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
    else
        // If the interface is without an address we suppose to be in a C class network
        netmask=0xffffff;

    netmask=0;
    if (pcap_compile(Adapters[nActiveAdapter].pAdapter, &fcode, "ip or ip6", 1, netmask) < 0)
    {
        AfxMessageBox(L"Unable to compile the packet filter. Check the syntax.");
        pcap_freealldevs(alldevs);
        return FALSE;
    }

    if (pcap_setfilter(Adapters[nActiveAdapter].pAdapter, &fcode) < 0)
    {
        AfxMessageBox(L"Error setting the filter.");
        pcap_freealldevs(alldevs);
        return FALSE;
    }

    if (pcap_datalink(Adapters[nActiveAdapter].pAdapter) != DLT_EN10MB)
    {
        AfxMessageBox(L"This program works only on Ethernet networks.");
        pcap_freealldevs(alldevs);
        return FALSE;
    }

    // At this point, we don't need any more the device list. Free it
    pcap_freealldevs(alldevs);

    Start(); //method of the base class CThread


    return TRUE;
}

void CPacket::CloseAdapter(int i)
{
    if (Adapters[i].pAdapter != NULL)
    {
        pcap_breakloop(Adapters[i].pAdapter);
        Adapters[i].pAdapter = NULL;
    }
}


BOOL CPacket::IsValidIPAdapter(int i)
{
    //to be valid, an adapter must have an ip address other than 0.0.0.0
    //and must not be a loopback device (127.0.0.1)
    return ((Adapters[i].ip4 != 0)&&(Adapters[i].ip4 != 0x7f000001));
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
