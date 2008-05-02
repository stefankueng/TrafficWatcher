// IPStat.h: interface for the CIPStat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPSTAT_H__510D8599_5B76_4495_828E_53852C6FED4B__INCLUDED_)
#define AFX_IPSTAT_H__510D8599_5B76_4495_828E_53852C6FED4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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
	DWORD64 GetTotal(int day);
	DWORD64 GetTotalMonth(int month);
	DWORD64	GetTotalReceived() {return m_received[thisday - 1].GetTotal();};
	DWORD64	GetTotalReceived(int day) {day--; return m_received[day].GetTotal();};
	DWORD64	GetTotalReceivedMonth(int month) {month--; return m_recmonth[month].GetTotal();};

	DWORD64	GetTotalSent() {return m_sent[thisday - 1].GetTotal();};
	DWORD64	GetTotalSent(int day) {day--; return m_sent[day].GetTotal();};
	DWORD64	GetTotalSentMonth(int month) {month--; return m_sentmonth[month].GetTotal();};

	DWORD64	GetIndexReceived(int i) {return m_received[thisday - 1].GetIndexRec(i);};
	DWORD64	GetIndexReceived(int i, int day) {day--; return m_received[day].GetIndexRec(i);};
	DWORD64	GetIndexReceivedMonth(int i, int month) {month--; return m_recmonth[month].GetIndexRec(i);};

	DWORD64	GetIndexSent(int i) {return m_sent[thisday - 1].GetIndexSent(i);};
	DWORD64	GetIndexSent(int i, int day) {day--; return m_sent[day].GetIndexSent(i);};
	DWORD64	GetIndexSentMonth(int i, int month) {month--; return m_sentmonth[month].GetIndexSent(i);};

	void	Clear(int today);
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

	void	Save();

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
	void		AnalyzePackets(UCHAR *buffer, UCHAR *packet);


private://members
	CPortStat	m_sent[31];
	CPortStat	m_received[31];

	CPortStat	m_sentmonth[12];
	CPortStat	m_recmonth[12];

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

    //! structure of the tcp packet header
	typedef	struct sniff_tcp
	{
		u_short th_sport;					///<source port
		u_short th_dport;					///<destination port
		u_short th_seq;						///<sequence number
		u_short th_ack;						///<acknowledgement number
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


	const sniff_ethernet *ethernet;				///<The ethernet header
	const sniff_ip *ip;							///<The IP header
	const sniff_tcp *tcp;						///<the TCP header

private:
	CTime	now;								///<holds the actual time and date.
	int		thisday;
	int		thismonth;



};

#endif // !defined(AFX_IPSTAT_H__510D8599_5B76_4495_828E_53852C6FED4B__INCLUDED_)
