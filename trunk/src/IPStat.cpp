// IPStat.cpp: implementation of the CIPStat class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "trafficwatch.h"
#include "IPStat.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define		STAT_FILE_VER		1

CIPStat::CIPStat()
{
	int i;
	//first clear all objects
	for (i=0; i<31; i++)
	{
		m_sent[i].Clear();
		m_received[i].Clear();
	}
	for (i=0; i<12; i++)
	{
		m_sentmonth[i].Clear();
		m_recmonth[i].Clear();
	}

	m_local = TRUE;						//only watch local traffic

	now = CTime::GetCurrentTime();		//get the current time
	thisday = now.GetDay();				//set the current day
	thismonth = now.GetMonth();			//set the current month
	
}

CIPStat::~CIPStat()
{
}

DWORD64	CIPStat::GetTotal()
{
	return (m_received[thisday - 1].GetTotal() + m_sent[thisday - 1].GetTotal());
}

DWORD64 CIPStat::GetTotal(int day)
{
	return (m_received[day-1].GetTotal() + m_sent[day-1].GetTotal());
}

DWORD64 CIPStat::GetTotalMonth(int month)
{
	return (m_recmonth[month-1].GetTotal() + m_sentmonth[month-1].GetTotal());
}


void	CIPStat::Clear(int today)
{
	m_received[today].Clear();
	m_sent[today].Clear();
}

BOOL	CIPStat::init(int ind)
{
	CTime lasttime = CTime(2002,1,1,0,0,0);

	//first clear all data and statistics
	for (int i=0; i<31; i++)
	{
		Clear(i);
	}
	for (int i=0; i<12; i++)
	{
		m_recmonth[i].Clear();
		m_sentmonth[i].Clear();
	}

	CString appPath;
	unsigned char szFilePath[512];
	int len = GetModuleFileName( AfxGetInstanceHandle( ), (char *)szFilePath, sizeof( szFilePath ) );
	if (len > 0)
	{
		appPath = CString(szFilePath);
		appPath = appPath.Left(appPath.ReverseFind('\\'));
		CFile theFile;
		if (theFile.Open(appPath+"\\statistics.dat",CFile::modeRead) != 0)
		{
			theFile.Read(&m_ver, sizeof(m_ver));
			theFile.Read(m_received, sizeof(m_received));
			theFile.Read(m_sent, sizeof(m_sent));
			theFile.Read(m_recmonth, sizeof(m_recmonth));
			theFile.Read(m_sentmonth, sizeof(m_sentmonth));
			theFile.Read(&lasttime, sizeof(lasttime));
		}
		else
		{
			//statistics file failed, try the backup copy!
			if (theFile.Open(appPath+"\\statistics1.dat",CFile::modeRead) != 0)
			{
				theFile.Read(&m_ver, sizeof(m_ver));
				theFile.Read(m_received, sizeof(m_received));
				theFile.Read(m_sent, sizeof(m_sent));
				theFile.Read(m_recmonth, sizeof(m_recmonth));
				theFile.Read(m_sentmonth, sizeof(m_sentmonth));
				theFile.Read(&lasttime, sizeof(lasttime));
			} // if (theFile.Open(appPath+"\\statistics1.dat",CFile::modeRead) != 0)
		}
	}

	thismonth = lasttime.GetMonth();
	CTime now = CTime::GetCurrentTime();
	CTimeSpan timediff;
	timediff = now - lasttime;
	int td;		//thisday
	td = now.GetDay();
	if (td != lasttime.GetDay())
	{
		//its not the same day, so we have to clear the days (and months)
		//where the program was not active
		Clear(td-1);
		CTimeSpan oneday = CTimeSpan(1,0,0,0);
		if (now.GetMonth() != thismonth)
		{
			thismonth = now.GetMonth();
			m_recmonth[thismonth-1].Clear();
			m_sentmonth[thismonth-1].Clear();
		}
		for (int i=0; i<timediff.GetDays(); i++)
		{
			now -= oneday;
			Clear(now.GetDay());
			if (now.GetMonth() != thismonth)
			{
				thismonth = now.GetMonth();
				m_recmonth[thismonth-1].Clear();
				m_sentmonth[thismonth-1].Clear();
			}
			thismonth = now.GetMonth();
		}
	}
	if (CPacket::init() == TRUE)
		return CPacket::Open(ind);
	
	return FALSE;
}

BOOL	CIPStat::close()
{
	Save();
	Stop((WORD)1000);
	return TRUE;
}

void	CIPStat::Save()
{
	CTime thistime = CTime::GetCurrentTime();

	CString appPath;
	unsigned char szFilePath[512];
	int len = GetModuleFileName( AfxGetInstanceHandle( ), (char *)szFilePath, sizeof( szFilePath ) );
	if (len > 0)
	{
		appPath = CString(szFilePath);
		appPath = appPath.Left(appPath.ReverseFind('\\'));
	}
	else
	{
		appPath = ".";
	}
	//first make a backup copy of the old statistics file
	TRY	
	{
		CFile::Remove(appPath+"\\statistics1.dat");
	}
	CATCH( CFileException, e )
	{
	}
	END_CATCH
	TRY
	{
		CFile::Rename(appPath+"\\statistics.dat", appPath+"\\statistics1.dat");
	}
	CATCH( CFileException, e )
	{
	}
	END_CATCH


	CFile theFile;
	if (theFile.Open(appPath+"\\statistics.dat",CFile::modeCreate | CFile::modeWrite) != 0)
	{
		m_ver = STAT_FILE_VER;
		theFile.Write(&m_ver, sizeof(m_ver));
		theFile.Write(m_received, sizeof(m_received));
		theFile.Write(m_sent, sizeof(m_sent));
		theFile.Write(m_recmonth, sizeof(m_recmonth));
		theFile.Write(m_sentmonth, sizeof(m_sentmonth));
		theFile.Write(&thistime, sizeof(thistime));
	}
}

//this method is called from the thread in the base class (CPacket)
void CIPStat::AnalyzePackets(UCHAR *buffer, UCHAR *packet)
{
	#define PROT_TCP		6
	#define	PROT_UDP		17

	//since this program will only work with ethernet network adapters, we don't
	//check the network type...

	if (nActiveAdapter==(-1))
		return;

	ethernet = (struct sniff_ethernet*)(packet);
	ip = (struct sniff_ip*)(packet + 14);
	tcp = (struct sniff_tcp*)(packet + 14 + 20);		//since port numbers are at the same position in tcp and udp header, just use tcp

	now = CTime::GetCurrentTime();

	if (thisday != now.GetDay())
	{
		thisday = now.GetDay();
		m_received[thisday - 1].Clear();
		m_sent[thisday - 1].Clear();
	}
	if (thismonth != now.GetMonth())
	{
		thismonth = now.GetMonth();
		m_recmonth[thismonth - 1].Clear();
		m_sentmonth[thismonth - 1].Clear();
	}

	//0x0800 is IP
	if (htons(ethernet->ether_type) == 0x0800)
	{
		//YES! this is an ip packet
		USHORT len;

		len = htons(ip->ip_len);
		
		//since we're probably (or for sure) in promiscuous mode, check if the packet was meant for us or not
		if ((ip->ip_src.S_un.S_addr)==Adapters[nActiveAdapter].ip)
		{
			if (((ip->ip_dst.S_un.S_addr)&Adapters[nActiveAdapter].mask) != (Adapters[nActiveAdapter].ip & Adapters[nActiveAdapter].mask))
			{
				//the packet was sent from this host/adapter
				if (ip->ip_p == PROT_TCP)
				{
					//tcp protocol
					m_sent[thisday - 1].AddDataTCP((tcp->th_dport), (tcp->th_sport), len);
					m_sentmonth[thismonth - 1].AddDataTCP((tcp->th_dport), (tcp->th_sport), len);
				}
				else if (ip->ip_p == PROT_UDP)
				{
					//udp protocol
					m_sent[thisday - 1].AddDataUDP((tcp->th_dport), (tcp->th_sport), len);
					m_sentmonth[thismonth - 1].AddDataUDP((tcp->th_dport), (tcp->th_sport), len);
				}
				else
				{
					//other protocol like ICMP, ...
					m_sent[thisday - 1].AddData(0, 0, len);
					m_sentmonth[thismonth - 1].AddData(0,0,len);
				}
			}
		}
		else if ((ip->ip_dst.S_un.S_addr)==Adapters[nActiveAdapter].ip)
		{
			//the packet was sent to this host/adapter
			if (((ip->ip_src.S_un.S_addr)&Adapters[nActiveAdapter].mask) != (Adapters[nActiveAdapter].ip & Adapters[nActiveAdapter].mask))
			{
				if (ip->ip_p == PROT_TCP)
				{
					//tcp protocol
					m_received[thisday - 1].AddDataTCP((tcp->th_sport), (tcp->th_dport), len);
					m_recmonth[thismonth - 1].AddDataTCP((tcp->th_sport), (tcp->th_dport), len);
				}
				else if (ip->ip_p == PROT_UDP)
				{
					//udp protocol
					m_received[thisday - 1].AddDataUDP((tcp->th_sport), (tcp->th_dport), len);
					m_recmonth[thismonth - 1].AddDataUDP((tcp->th_sport), (tcp->th_dport), len);
				}
				else
				{
					//other protocol like ICMP, ...
					m_received[thisday - 1].AddData(0, 0, len);
					m_recmonth[thismonth - 1].AddData(0, 0, len);
				}
			}
		}
		else if (((ip->ip_src.S_un.S_addr)&Adapters[nActiveAdapter].mask)==(Adapters[nActiveAdapter].ip&Adapters[nActiveAdapter].mask))
		{
			//the packet was sent from the local network
			if (((ip->ip_dst.S_un.S_addr)&Adapters[nActiveAdapter].mask) != (Adapters[nActiveAdapter].ip & Adapters[nActiveAdapter].mask))
			{
				if (ip->ip_p == PROT_TCP)
				{
					//tcp protocol
					if (m_local == FALSE) m_sent[thisday - 1].AddDataTCP((tcp->th_dport), (tcp->th_sport), len);
					if (m_local == FALSE) m_sentmonth[thismonth - 1].AddDataTCP((tcp->th_dport), (tcp->th_sport), len);
				}
				else if (ip->ip_p == PROT_UDP)
				{
					//udp protocol
					if (m_local == FALSE) m_sent[thisday - 1].AddDataUDP((tcp->th_dport), (tcp->th_sport), len);
					if (m_local == FALSE) m_sentmonth[thismonth - 1].AddDataUDP((tcp->th_dport), (tcp->th_sport), len);
				}
				else
				{
					//other protocol like ICMP, ...
					if (m_local == FALSE) m_sent[thisday - 1].AddData(0, 0, len);
					if (m_local == FALSE) m_sentmonth[thismonth - 1].AddData(0, 0, len);
				}
			}
		}
		else if (((ip->ip_dst.S_un.S_addr)&Adapters[nActiveAdapter].mask)==(Adapters[nActiveAdapter].ip&Adapters[nActiveAdapter].mask))
		{
			//the packet was sent to the local network
			if (((ip->ip_src.S_un.S_addr)&Adapters[nActiveAdapter].mask) != (Adapters[nActiveAdapter].ip & Adapters[nActiveAdapter].mask))
			{
				if (ip->ip_p == PROT_TCP)
				{
					//tcp protocol
					if (m_local == FALSE) m_received[thisday - 1].AddDataTCP((tcp->th_sport), (tcp->th_dport), len);
					if (m_local == FALSE) m_recmonth[thismonth - 1].AddDataTCP((tcp->th_sport), (tcp->th_dport), len);
				}
				else if (ip->ip_p == PROT_UDP)
				{
					//udp protocol
					if (m_local == FALSE) m_received[thisday - 1].AddDataUDP((tcp->th_sport), (tcp->th_dport), len);
					if (m_local == FALSE) m_recmonth[thismonth - 1].AddDataUDP((tcp->th_sport), (tcp->th_dport), len);
				}
				else
				{
					//other protocol like ICMP, ...
					if (m_local == FALSE) m_received[thisday - 1].AddData(0, 0, len);
					if (m_local == FALSE) m_recmonth[thismonth - 1].AddData(0, 0, len);
				}
			}
		}
		else
		{
			//if here, then something went terribly wrong!
		}
	}
}


