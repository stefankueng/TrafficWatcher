#pragma once

#include "portstat.h"
#include "packet.h"


#define ETHER_ADDR_LEN 6


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
	DWORD64	GetTotal();
	DWORD64	GetTotalReceived() {return m_received.GetTotal();};

	DWORD64	GetTotalSent() {return m_sent.GetTotal();};

	DWORD64	GetIndexReceived(int i) {return m_received.GetIndexRec(i);};

	DWORD64	GetIndexSent(int i) {return m_sent.GetIndexSent(i);};

	BOOL	init(int ind = -1);
	BOOL	close();
    /*!
     * set this to TRUE if you want watch only traffic from and to the local host/adapter. if set to FALSE, all
	 * traffic on the LAN will be watched and used for the statistics
     *
     * @param b : TRUE to watch only local traffic, FALSE to watch all LAN traffic
     */
	void	SetLocal(BOOL b = TRUE) {m_local = b;};

    /*!
     * checks if the statistics is built only for local traffic or for the whole LAN traffic.
     *
     * @return  : TRUE for loacal, FALSE for LAN traffic
     */
	BOOL	GetLocal() {return m_local;};

private://methods

    /*!
     * this method is called from the thread in the base class CPacket whenever a packet
	 * was received from the network adapter.
	 * this methods analyzes the packet and gathers information about type, port and size
	 * of the ip packets sent or received by the adapter
     *
     * @param *buffer : see CPacket::AnalyzePackets()
     * @param *packet : see CPacket::AnalyzePackets()
     */
	void		AnalyzePackets(const UCHAR *buffer, const UCHAR *packet);


private://members
	CPortStat	m_sent;
	CPortStat	m_received;

	int			m_ver;

	BOOL		m_local;


    //! structure of the ethernet packet header
	typedef struct sniff_ethernet
	{
		u_char ether_dhost[ETHER_ADDR_LEN];	///<Destination host address
		u_char ether_shost[ETHER_ADDR_LEN];	///<Source host address
		u_short ether_type;					///<IP? ARP? RARP? etc
	};

    //! structure of the ip packet header
	typedef	struct sniff_ip
	{
		u_char ip_hl:4,						///<header length
		ip_v:4;								///<version
		u_char ip_tos;						///<type of service
		u_short ip_len;						///<total length
		u_short ip_id;						///<identification
		u_short ip_off;						///<fragment offset field
		#define IP_RF 0x8000				///<reserved fragment flag
		#define IP_DF 0x4000				///<dont fragment flag
		#define IP_MF 0x2000				///<more fragments flag
		#define IP_OFFMASK 0x1fff			///<mask for fragmenting bits
		u_char ip_ttl;						///<time to live
		u_char ip_p;						///<protocol
		u_short ip_sum;						///<checksum
		struct in_addr ip_src,ip_dst;		///<source and dest address
	};

	typedef struct sniff_ip6
	{
		union 
		{
			struct ip6_hdrctl 
			{
				u_int32_t ip6_un1_flow;	// 20 bits of flow-ID 
				u_int16_t ip6_un1_plen;	// payload length
				u_int8_t  ip6_un1_nxt;	// next header 
				u_int8_t  ip6_un1_hlim;	// hop limit 
			} ip6_un1;
			u_int8_t ip6_un2_vfc;	// 4 bits version, top 4 bits class 
		} ip6_ctlun;
		struct in6_addr ip6_src;	// source address
		struct in6_addr ip6_dst;	// destination address
	};

    //! structure of the tcp packet header
	typedef	struct sniff_tcp
	{
		u_short th_sport;					///<source port
		u_short th_dport;					///<destination port
		u_short th_seq;						///<sequence number
		u_short th_ack;						///<acknowledgment number
		u_int th_x2:4,						///<(unused)
		th_off:4;							///<data offset
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
		u_short th_win;						///<window
		u_short th_sum;						///<checksum
		u_short th_urp;						///<urgent pointer
	};


	const sniff_ethernet *ethernet;				///< The ethernet header
	const sniff_ip *ip;							///< The IP header
	const sniff_tcp *tcp;						///< The TCP header
	const sniff_ip6 *ip6;						///< The IPv6 header
	const sniff_tcp *tcp6;						///< The TCP header
private:


};

