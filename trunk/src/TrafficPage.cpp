// TrafficPage.cpp : implementation file
//

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
	//{{AFX_DATA_INIT(CTrafficPage)
	m_dlMaxSpeed = _T("");
	m_ulMaxSpeed = _T("");
	m_connections = _T("");
	m_dlByteSpeed = _T("");
	m_dlData = _T("");
	m_gateway = _T("");
	m_ip = _T("");
	m_mask = _T("");
	m_totMaxSpeed = _T("");
	m_totByteSpeed = _T("");
	m_totData = _T("");
	m_ulByteSpeed = _T("");
	m_ulData = _T("");
	m_adaptDesc = _T("");
	m_macaddress = _T("");
	//}}AFX_DATA_INIT
}

CTrafficPage::~CTrafficPage()
{
}

void CTrafficPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrafficPage)
	DDX_Control(pDX, IDC_UL_BAR, m_ul_bar);
	DDX_Control(pDX, IDC_DL_BAR, m_dl_bar);
	DDX_Text(pDX, IDC_DL_MAXSPEED, m_dlMaxSpeed);
	DDX_Text(pDX, IDC_UL_MAXSPEED, m_ulMaxSpeed);
	DDX_Text(pDX, IDC_CONNECTIONS, m_connections);
	DDX_Text(pDX, IDC_DL_BYTESPEED, m_dlByteSpeed);
	DDX_Text(pDX, IDC_DL_DATA, m_dlData);
	DDX_Text(pDX, IDC_GATEWAY, m_gateway);
	DDX_Text(pDX, IDC_IP, m_ip);
	DDX_Text(pDX, IDC_MASK, m_mask);
	DDX_Text(pDX, IDC_TOT_MAXSPEED, m_totMaxSpeed);
	DDX_Text(pDX, IDC_TOT_BYTESPEED, m_totByteSpeed);
	DDX_Text(pDX, IDC_TOT_DATA, m_totData);
	DDX_Text(pDX, IDC_UL_BYTESPEED, m_ulByteSpeed);
	DDX_Text(pDX, IDC_UL_DATA, m_ulData);
	DDX_Text(pDX, IDC_ADAPT_DESC, m_adaptDesc);
	DDX_Text(pDX, IDC_MACADDR, m_macaddress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTrafficPage, CPropertyPage)
	//{{AFX_MSG_MAP(CTrafficPage)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
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


	if (m_pTheApp->m_wnd.m_ipStat.GetActiveAdapterNumber() > (-1))
	{
		m_ip = m_pTheApp->m_wnd.m_ipStat.GetIP();
		m_mask = m_pTheApp->m_wnd.m_ipStat.GetMask();
		m_gateway = m_pTheApp->m_wnd.m_ipStat.GetGateway();
		m_macaddress = m_pTheApp->m_wnd.m_ipStat.GetMacAddr();
		m_adaptDesc = m_pTheApp->m_wnd.m_ipStat.GetDescription();
	}

	
	
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
			DWORD ulSpeed, dlSpeed;
			
			dwTotalRec = m_pTheApp->m_wnd.m_ipStat.GetTotalReceived();
			dwTotalSent = m_pTheApp->m_wnd.m_ipStat.GetTotalSent();
			dwRecDiff = dwTotalRec - dwTotalRecOld;
			dwSentDiff = dwTotalSent - dwTotalSentOld;
			dwTotalRecOld = dwTotalRec;
			dwTotalSentOld = dwTotalSent;
			dwTime = GetTickCount();
			dwElapsed = dwTime - dwOldTime;
			//check for wrap
			if (dwElapsed > 0x8000)
			{
				dwElapsed = dwTime + (0 - dwOldTime);
			}
			dwOldTime = dwTime;
			
			dlSpeed = DWORD(dwRecDiff * (DWORD64)1000 / dwElapsed);
			ulSpeed = DWORD(dwSentDiff * (DWORD64)1000 / dwElapsed);

			m_dlByteSpeed = CUtil::GetNumberString(dlSpeed)+"/s";
			m_ulByteSpeed = CUtil::GetNumberString(ulSpeed)+"/s";
			m_totByteSpeed = CUtil::GetNumberString(dlSpeed+ulSpeed)+"/s";

			m_dlData = CUtil::GetNumberString(dwTotalRec);
			m_ulData = CUtil::GetNumberString(dwTotalSent);
			m_totData = CUtil::GetNumberString(dwTotalRec + dwTotalSent);

			m_dlMaxSpeed = CUtil::GetNumberString(m_pTheApp->m_wnd.dwDlMax);
			m_ulMaxSpeed = CUtil::GetNumberString(m_pTheApp->m_wnd.dwUlMax);
			m_totMaxSpeed = CUtil::GetNumberString(m_pTheApp->m_wnd.dwDlMax+m_pTheApp->m_wnd.dwUlMax);

			m_dl_bar.SetPos(dlSpeed/1000);		//range is in kbytes/s, not bit/s!
			m_ul_bar.SetPos(ulSpeed/1000);

			GetTcpStatistics(&m_tcpstats);

			m_connections.Format("%d", m_tcpstats.dwCurrEstab);

			UpdateData(FALSE);
		} // if (m_pTheApp->m_wnd.m_ipStat.GetActiveAdapterNumber() > -1){ 
		SetTimer(IDT_TRAFFIC, 1000, NULL);
	} // if (nIDEvent == IDT_TRAFFIC){ 		
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

