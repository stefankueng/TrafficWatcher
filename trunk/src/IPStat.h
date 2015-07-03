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

#pragma once

#include "PortStat.h"
#include "Packet.h"


#define ETHER_ADDR_LEN      6


/**
 * CIPStat.
 * holds all statistical data of the IP stack. the statistics are separated by main services
 * and received and sent bytes.
 */
class CIPStat : public CPacket
{
public:
    CIPStat();
    virtual ~CIPStat();
    DWORD64 GetTotal();
    DWORD64 GetTotalReceived() {return m_received.GetTotal();}
    DWORD64 GetTotalSent() {return m_sent.GetTotal();}

    DWORD64 GetTotalReceivedLAN() {return m_receivedLAN.GetTotal();}
    DWORD64 GetTotalSentLAN() {return m_sentLAN.GetTotal();}


    DWORD64 GetIndexReceived(int i) {return m_received.GetIndexRec(i);}

    DWORD64 GetIndexSent(int i) {return m_sent.GetIndexSent(i);};

    BOOL    init(int ind = -1);
    BOOL    close();

private: //methods

    /**
     * this method is called from the thread in the base class CPacket whenever a packet
     * was received from the network adapter.
     * this methods analyzes the packet and gathers information about type, port and size
     * of the ip packets sent or received by the adapter
     *
     * \param *buffer : see CPacket::AnalyzePackets()
     * \param *packet : see CPacket::AnalyzePackets()
     */
    void        AnalyzePackets(const UCHAR *buffer, const UCHAR *packet);


private: //members
    CPortStat   m_sent;
    CPortStat   m_received;

    CPortStat   m_sentLAN;
    CPortStat   m_receivedLAN;

#pragma pack(1)

    /// structure of the ethernet packet header
    struct sniff_ethernet
    {
        u_char ether_dhost[ETHER_ADDR_LEN]; ///<Destination host address
        u_char ether_shost[ETHER_ADDR_LEN]; ///<Source host address
        u_short ether_type;                 ///<IP? ARP? RARP? etc
    };
    /// structure of the ip packet header
    struct sniff_ip
    {
        u_char ip_hl:4,                     ///<header length
        ip_v:4;                             ///<version
        u_char ip_tos;                      ///<type of service
        u_short ip_len;                     ///<total length
        u_short ip_id;                      ///<identification
        u_short ip_off;                     ///<fragment offset field
        #define IP_RF 0x8000                ///<reserved fragment flag
        #define IP_DF 0x4000                ///<dont fragment flag
        #define IP_MF 0x2000                ///<more fragments flag
        #define IP_OFFMASK 0x1fff           ///<mask for fragmenting bits
        u_char ip_ttl;                      ///<time to live
        u_char ip_p;                        ///<protocol
        u_short ip_sum;                     ///<checksum
        struct in_addr ip_src,ip_dst;       ///<source and dest address
    };

    struct sniff_ip6
    {
        union
        {
            struct ip6_hdrctl
            {
                u_int32_t ip6_un1_flow; // 20 bits of flow-ID
                u_int16_t ip6_un1_plen; // payload length
                u_int8_t  ip6_un1_nxt;  // next header
                u_int8_t  ip6_un1_hlim; // hop limit
            } ip6_un1;
            u_int8_t ip6_un2_vfc;   // 4 bits version, top 4 bits class
        } ip6_ctlun;
        struct in6_addr ip6_src;    // source address
        struct in6_addr ip6_dst;    // destination address
    };

    /// structure of the tcp packet header
    struct sniff_tcp
    {
        u_short th_sport;                   ///<source port
        u_short th_dport;                   ///<destination port
        u_short th_seq;                     ///<sequence number
        u_short th_ack;                     ///<acknowledgment number
        u_int th_x2:4,                      ///<(unused)
        th_off:4;                           ///<data offset
        u_char th_flags;
        #define TH_FIN 0x01
        #define TH_SYN 0x02
        #define TH_RST 0x04
        #define TH_PUSH 0x08
        #define TH_ACK 0x10
        #define TH_URG 0x20
        #define TH_ECE 0x40
        #define TH_CWR 0x80
        #define TH_FLAGS (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
        u_short th_win;                     ///<window
        u_short th_sum;                     ///<checksum
        u_short th_urp;                     ///<urgent pointer
    };

    struct SOCKADDRIPV6
    {
        __int64 sa_family;      // address family
        unsigned char sa_data[16];
    };


    const sniff_ethernet *ethernet;             ///< The ethernet header
    const sniff_ip *ip;                         ///< The IP header
    const sniff_tcp *tcp;                       ///< The TCP header
    const sniff_ip6 *ip6;                       ///< The IPv6 header
    const sniff_tcp *tcp6;                      ///< The TCP header
#pragma pack()

};
