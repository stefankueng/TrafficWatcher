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
#include "trafficwatch.h"
#include "IPStat.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define STAT_FILE_VER       1

CIPStat::CIPStat()
    : ethernet(NULL)
    , ip(NULL)
    , ip6(NULL)
    , tcp(NULL)
    , tcp6(NULL)
{
    m_sent.Clear();
    m_received.Clear();
}

CIPStat::~CIPStat()
{
}

DWORD64 CIPStat::GetTotal()
{
    return (m_received.GetTotal() + m_sent.GetTotal());
}

BOOL CIPStat::init(int ind)
{
    //first clear all data and statistics
    m_sent.Clear();
    m_received.Clear();

    if (CPacket::init() == TRUE)
    {
        if (ind >= 0)
            return CPacket::Open(ind);
        return TRUE;
    }
    return FALSE;
}

BOOL CIPStat::close()
{
    Stop((WORD)1000);
    return TRUE;
}


//this method is called from the thread in the base class (CPacket)
void CIPStat::AnalyzePackets(const UCHAR *buffer, const UCHAR * /*packet*/)
{
    #define PROT_TCP        6
    #define PROT_UDP        17

    if (nActiveAdapter==(-1))
        return;

    ethernet = (struct sniff_ethernet*)(buffer);
    ip = (struct sniff_ip*)(buffer + 14);
    ip6 = (struct sniff_ip6*)(buffer + 14);
    tcp = (struct sniff_tcp*)(buffer + 14 + 20);        //since port numbers are at the same position in tcp and udp header, just use tcp
    tcp6 = (struct sniff_tcp*)(buffer + 14 + 40);       //since port numbers are at the same position in tcp and udp header, just use tcp

    USHORT len;
    len = htons(ip->ip_len);

    if (htons(ethernet->ether_type) == 0x86dd)  // IPv6
    {
        ULONG llen = ip6->ip6_ctlun.ip6_un1.ip6_un1_plen;
        if (llen > 10000 )
            llen = htons((u_short)llen);
        SOCKADDRIPV6 * sock6 = (SOCKADDRIPV6 *)&Adapters[nActiveAdapter].ip6;
        // assume for now that ipv6 is only used inside the LAN
        if (memcmp(ip6->ip6_src.u.Byte, sock6->sa_data, 16) == 0)
        {
            m_sentLAN.AddDataTCP((tcp6->th_dport), (tcp6->th_sport), llen);
        }
        else if (memcmp(ip6->ip6_dst.u.Byte, sock6->sa_data, 16) == 0)
        {
            m_receivedLAN.AddDataTCP((tcp6->th_sport), (tcp6->th_dport), llen);
        }
    }
    else if (htons(ethernet->ether_type) == 0x0800) // IPv4
    {
        len = htons(ip->ip_len);
        //since we're probably (or for sure) in promiscuous mode, check if the packet was meant for us or not
        if ((ip->ip_src.S_un.S_addr)==Adapters[nActiveAdapter].ip4)
        {
            if (((ip->ip_dst.S_un.S_addr)&Adapters[nActiveAdapter].mask) != (Adapters[nActiveAdapter].ip4 & Adapters[nActiveAdapter].mask))
            {
                //the packet was sent from this host/adapter
                if (ip->ip_p == PROT_TCP)
                {
                    //tcp protocol
                    m_sent.AddDataTCP((tcp->th_dport), (tcp->th_sport), len);
                }
                else if (ip->ip_p == PROT_UDP)
                {
                    //udp protocol
                    m_sent.AddDataUDP((tcp->th_dport), (tcp->th_sport), len);
                }
                else
                {
                    //other protocol like ICMP, ...
                    m_sent.AddData(0, 0, len);
                }
            }
            else
            {
                if (ip->ip_p == PROT_TCP)
                {
                    //tcp protocol
                    m_sentLAN.AddDataTCP((tcp->th_dport), (tcp->th_sport), len);
                }
                else if (ip->ip_p == PROT_UDP)
                {
                    //udp protocol
                    m_sentLAN.AddDataUDP((tcp->th_dport), (tcp->th_sport), len);
                }
                else
                {
                    //other protocol like ICMP, ...
                    m_sentLAN.AddData(0, 0, len);
                }
            }
        }
        else if ((ip->ip_dst.S_un.S_addr)==Adapters[nActiveAdapter].ip4)
        {
            //the packet was sent to this host/adapter
            if (((ip->ip_src.S_un.S_addr)&Adapters[nActiveAdapter].mask) != (Adapters[nActiveAdapter].ip4 & Adapters[nActiveAdapter].mask))
            {
                if (ip->ip_p == PROT_TCP)
                {
                    //tcp protocol
                    m_received.AddDataTCP((tcp->th_sport), (tcp->th_dport), len);
                }
                else if (ip->ip_p == PROT_UDP)
                {
                    //udp protocol
                    m_received.AddDataUDP((tcp->th_sport), (tcp->th_dport), len);
                }
                else
                {
                    //other protocol like ICMP, ...
                    m_received.AddData(0, 0, len);
                }
            }
            else
            {
                if (ip->ip_p == PROT_TCP)
                {
                    //tcp protocol
                    m_receivedLAN.AddDataTCP((tcp->th_sport), (tcp->th_dport), len);
                }
                else if (ip->ip_p == PROT_UDP)
                {
                    //udp protocol
                    m_receivedLAN.AddDataUDP((tcp->th_sport), (tcp->th_dport), len);
                }
                else
                {
                    //other protocol like ICMP, ...
                    m_receivedLAN.AddData(0, 0, len);
                }
            }
        }
        else if (((ip->ip_src.S_un.S_addr)&Adapters[nActiveAdapter].mask)==(Adapters[nActiveAdapter].ip4&Adapters[nActiveAdapter].mask))
        {
            //the packet was sent from the local network
            if (((ip->ip_dst.S_un.S_addr)&Adapters[nActiveAdapter].mask) != (Adapters[nActiveAdapter].ip4 & Adapters[nActiveAdapter].mask))
            {
                if (ip->ip_p == PROT_TCP)
                {
                    //tcp protocol
                    m_sentLAN.AddDataTCP((tcp->th_dport), (tcp->th_sport), len);
                }
                else if (ip->ip_p == PROT_UDP)
                {
                    //udp protocol
                    m_sentLAN.AddDataUDP((tcp->th_dport), (tcp->th_sport), len);
                }
                else
                {
                    //other protocol like ICMP, ...
                    m_sentLAN.AddData(0, 0, len);
                }
            }
        }
        else if (((ip->ip_dst.S_un.S_addr)&Adapters[nActiveAdapter].mask)==(Adapters[nActiveAdapter].ip4&Adapters[nActiveAdapter].mask))
        {
            //the packet was sent to the local network
            if (((ip->ip_src.S_un.S_addr)&Adapters[nActiveAdapter].mask) != (Adapters[nActiveAdapter].ip4 & Adapters[nActiveAdapter].mask))
            {
                if (ip->ip_p == PROT_TCP)
                {
                    //tcp protocol
                    m_receivedLAN.AddDataTCP((tcp->th_sport), (tcp->th_dport), len);
                }
                else if (ip->ip_p == PROT_UDP)
                {
                    //udp protocol
                    m_receivedLAN.AddDataUDP((tcp->th_sport), (tcp->th_dport), len);
                }
                else
                {
                    //other protocol like ICMP, ...
                    m_receivedLAN.AddData(0, 0, len);
                }
            }
        }
    }
}
