#include "stdafx.h"
#include "trafficwatch.h"
#include "TrafficPage.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrafficPage property page

IMPLEMENT_DYNCREATE(CTrafficPage, CPropertyPage)

CTrafficPage::CTrafficPage() : CPropertyPage(CTrafficPage::IDD)
{
	m_dlLANSpeed = _T("");
	m_ulLANSpeed = _T("");
	m_connections = _T("");
	m_dlByteSpeed = _T("");
	m_gateway = _T("");
	m_ip = _T("");
	m_mask = _T("");
	m_ulByteSpeed = _T("");
	m_adaptDesc = _T("");
	m_macaddress = _T("");
}

CTrafficPage::~CTrafficPage()
{
}

void CTrafficPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_UL_BAR, m_ul_bar);
	DDX_Control(pDX, IDC_DL_BAR, m_dl_bar);
	DDX_Control(pDX, IDC_UL_BARLAN, m_ul_barlan);
	DDX_Control(pDX, IDC_DL_BARLAN, m_dl_barlan);
	DDX_Text(pDX, IDC_DL_LANSPEED, m_dlLANSpeed);
	DDX_Text(pDX, IDC_UL_LANSPEED, m_ulLANSpeed);
	DDX_Text(pDX, IDC_CONNECTIONS, m_connections);
	DDX_Text(pDX, IDC_DL_BYTESPEED, m_dlByteSpeed);
	DDX_Text(pDX, IDC_GATEWAY, m_gateway);
	DDX_Text(pDX, IDC_IP, m_ip);
	DDX_Text(pDX, IDC_MASK, m_mask);
	DDX_Text(pDX, IDC_UL_BYTESPEED, m_ulByteSpeed);
	DDX_Text(pDX, IDC_ADAPT_DESC, m_adaptDesc);
	DDX_Text(pDX, IDC_MACADDR, m_macaddress);
}


BEGIN_MESSAGE_MAP(CTrafficPage, CPropertyPage)
	ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrafficPage message handlers

BOOL CTrafficPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	SetTimer( IDT_TRAFFIC, 1000, NULL );

	m_dl_bar.SetAnimated();
	m_dl_bar.SetMaximumTime();
	m_dl_bar.SetShowMaximum();
	m_dl_bar.SetMaximumTime(50);
	m_dl_bar.SetColor(COLORDOWN);
	m_ul_bar.SetAnimated();
	m_ul_bar.SetMaximumTime();
	m_ul_bar.SetShowMaximum();
	m_ul_bar.SetMaximumTime(50);
	m_ul_bar.SetColor(COLORUP);

	m_dl_barlan.SetAnimated();
	m_dl_barlan.SetMaximumTime();
	m_dl_barlan.SetShowMaximum();
	m_dl_barlan.SetMaximumTime(50);
	m_dl_barlan.SetColor(COLORDOWN);
	m_ul_barlan.SetAnimated();
	m_ul_barlan.SetMaximumTime();
	m_ul_barlan.SetShowMaximum();
	m_ul_barlan.SetMaximumTime(50);
	m_ul_barlan.SetColor(COLORUP);


	if (m_pTheApp->m_wnd.m_ipStat.GetActiveAdapterNumber() > (-1))
	{
		m_ip = m_pTheApp->m_wnd.m_ipStat.GetIP();
		m_mask = m_pTheApp->m_wnd.m_ipStat.GetMask();
		m_gateway = m_pTheApp->m_wnd.m_ipStat.GetGateway();
		m_macaddress = m_pTheApp->m_wnd.m_ipStat.GetMacAddr();
		m_adaptDesc = m_pTheApp->m_wnd.m_ipStat.GetDescription();
	}

	
	DWORD dwTicks = GetTickCount();

	m_sentFilter.Filter(dwTicks, m_pTheApp->m_wnd.m_ipStat.GetTotalSent());
	m_receivedFilter.Filter(dwTicks, m_pTheApp->m_wnd.m_ipStat.GetTotalReceived());
	m_sentLANFilter.Filter(dwTicks, m_pTheApp->m_wnd.m_ipStat.GetTotalSentLAN());
	m_receivedLANFilter.Filter(dwTicks, m_pTheApp->m_wnd.m_ipStat.GetTotalReceivedLAN());

	UpdateData(FALSE);
	
	return TRUE;
}

void CTrafficPage::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == IDT_TRAFFIC)
	{
		if (m_pTheApp->m_wnd.m_ipStat.GetActiveAdapterNumber() > -1)
		{
			KillTimer(IDT_TRAFFIC);

			DWORD dwTicks = GetTickCount();

			DWORD ulSpeed = (DWORD)m_sentFilter.Filter(dwTicks, m_pTheApp->m_wnd.m_ipStat.GetTotalSent());
			DWORD dlSpeed = (DWORD)m_receivedFilter.Filter(dwTicks, m_pTheApp->m_wnd.m_ipStat.GetTotalReceived());
			DWORD ulLANSpeed = (DWORD)m_sentLANFilter.Filter(dwTicks, m_pTheApp->m_wnd.m_ipStat.GetTotalSentLAN());
			DWORD dlLANSpeed = (DWORD)m_receivedLANFilter.Filter(dwTicks, m_pTheApp->m_wnd.m_ipStat.GetTotalReceivedLAN());


			m_dl_bar.SetText(CUtil::GetNumberString(dlSpeed)+"/s");
			m_ul_bar.SetText(CUtil::GetNumberString(ulSpeed)+"/s");

			m_dl_bar.SetPos(dlSpeed/1000);		//range is in kbytes/s, not bytes/s!
			m_ul_bar.SetPos(ulSpeed/1000);


			m_dl_barlan.SetText(CUtil::GetNumberString(dlLANSpeed)+"/s");
			m_ul_barlan.SetText(CUtil::GetNumberString(ulLANSpeed)+"/s");

			m_dl_barlan.SetPos(dlLANSpeed/1000);
			m_ul_barlan.SetPos(ulLANSpeed/1000);


			m_dlByteSpeed = CUtil::GetNumberString(dlSpeed)+"/s";
			m_ulByteSpeed = CUtil::GetNumberString(ulSpeed)+"/s";
			m_dlLANSpeed = CUtil::GetNumberString(dlLANSpeed)+"/s";
			m_ulLANSpeed = CUtil::GetNumberString(ulLANSpeed)+"/s";


			GetTcpStatistics(&m_tcpstats);

			m_connections.Format("%d", m_tcpstats.dwCurrEstab);

			UpdateData(FALSE);
		} 
		SetTimer(IDT_TRAFFIC, 1000, NULL);
	}
}

BOOL CTrafficPage::OnSetActive() 
{
	m_dl_bar.SetRange(0, (short)(COptionsPage::GetDownloadSpeed()));
	m_ul_bar.SetRange(0, (short)(COptionsPage::GetUploadSpeed()));
	SetTimer( IDT_TRAFFIC, 10, NULL );
	return CPropertyPage::OnSetActive();
}

BOOL CTrafficPage::OnKillActive() 
{
	KillTimer( IDT_TRAFFIC );
	return CPropertyPage::OnKillActive();
}

