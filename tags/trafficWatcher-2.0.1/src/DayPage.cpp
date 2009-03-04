// DayPage.cpp : implementation file
//

#include "stdafx.h"
#include "trafficwatch.h"
#include "DayPage.h"

#include "util.h"
#include "globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHistoryPage property page

IMPLEMENT_DYNCREATE(CHistoryPage, CPropertyPage)

CHistoryPage::CHistoryPage() : CPropertyPage(CHistoryPage::IDD)
{
	//{{AFX_DATA_INIT(CHistoryPage)
	//}}AFX_DATA_INIT

	for (int i=0; i<LASTSERVICE; i++)
	{
		m_ul[i] = _T("");
		m_dl[i] = _T("");
	}
	m_ultot = _T("");
	m_dltot = _T("");
	m_tot = _T("");
	m_date = _T("");

}

CHistoryPage::~CHistoryPage()
{
}


void CHistoryPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHistoryPage)
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_DL_NEWS, m_dl[S_NEWS]);
	DDX_Text(pDX, IDC_DL_MAIL, m_dl[S_MAIL]);
	DDX_Text(pDX, IDC_DL_OTHER, m_dl[S_OTHER]);
	DDX_Text(pDX, IDC_DL_SHARE, m_dl[S_SHARE]);
	DDX_Text(pDX, IDC_DL_TOT, m_dltot);
	DDX_Text(pDX, IDC_DL_UDP, m_dl[S_UDP]);
	DDX_Text(pDX, IDC_DL_WEB, m_dl[S_WEB]);
	DDX_Text(pDX, IDC_TOTAL, m_tot);
	DDX_Text(pDX, IDC_UL_NEWS, m_ul[S_NEWS]);
	DDX_Text(pDX, IDC_UL_MAIL, m_ul[S_MAIL]);
	DDX_Text(pDX, IDC_UL_OTHER, m_ul[S_OTHER]);
	DDX_Text(pDX, IDC_UL_SHARE, m_ul[S_SHARE]);
	DDX_Text(pDX, IDC_UL_TOT, m_ultot);
	DDX_Text(pDX, IDC_UL_UDP, m_ul[S_UDP]);
	DDX_Text(pDX, IDC_UL_WEB, m_ul[S_WEB]);
	DDX_Text(pDX, IDC_DATE, m_date);
}


BEGIN_MESSAGE_MAP(CHistoryPage, CPropertyPage)
	//{{AFX_MSG_MAP(CHistoryPage)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CHistoryPage::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	CTime today;
	today = CTime::GetCurrentTime();
	day = today.GetDay();
	month = 0;
	FillData(day, month);
	return CPropertyPage::OnSetActive();
}

BOOL CHistoryPage::OnKillActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	return CPropertyPage::OnKillActive();
}
void CHistoryPage::OnPaint()
{
	CPropertyPage::OnPaint();
	DrawDayDiagram(day);
	DrawMonthDiagram(month);
}

void CHistoryPage::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect;
	//determine the position of the diagram
	GetDlgItem(IDC_DAYDIAGRAM)->GetWindowRect(rect);
	//convert diagram coordinates relative to window
	ScreenToClient(&rect);
	//check if point lies inside diagram
	if (rect.PtInRect(point))
	{
		month = 0;									//a specific day was selected, not a month
		int x = point.x - rect.left;
		float dx = max(((float)rect.Width() / (float)CalcDays()),1);
		x = rect.Width() - x;
		int dayindex = (int)max(((float)x / dx), 0);
		CTime t = CTime::GetCurrentTime();
		CTimeSpan dt = CTimeSpan(1,0,0,0);
		for (int i = 0; i < dayindex; i++)
		{
			t -= dt;
		}
		day = t.GetDay();
	}
	GetDlgItem(IDC_MONTHDIAGRAM)->GetWindowRect(rect);
	ScreenToClient(&rect);
	if (rect.PtInRect(point))
	{
		day = 0;									//a specific month was selected, not a day
		int x = point.x - rect.left;
		float dx = max((float)rect.Width() / (float)12, 1);
		x = rect.Width() - x;
		int monthindex = (int)max(((float)x / dx),0);
		CTime t = CTime::GetCurrentTime();
		month = t.GetMonth();
		month -= monthindex;
		if (month <= 0)
			month = 12 + month;
	}
	FillData(day, month);
	RedrawWindow();
}

void CHistoryPage::DrawDayDiagram(int day)
{
	CTime today;
	CTimeSpan oneday = CTimeSpan(1,0,0,0);
	DWORD64 maxval;
	int dayrange;

	CDC	*pDC;
	CRect rect, rectClient;


	maxval = GetMaxDay();
	dayrange = CalcDays();

	today = CTime::GetCurrentTime();

	GetDlgItem(IDC_DAYDIAGRAM)->GetClientRect(rect);
	rectClient = rect;
	pDC = GetDlgItem(IDC_DAYDIAGRAM)->GetDC();

	CDC dc;
	dc.CreateCompatibleDC(pDC);
	int nSavedDC = dc.SaveDC();
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	CBitmap *pOldBmp = dc.SelectObject(&bmp);

	CBrush br(::GetSysColor(COLOR_3DFACE));
	dc.FillRect(rect, &br);

	DWORD64 temp;
	for (int i=0; i<dayrange; i++)
	{
		temp = m_pTheApp->m_wnd.m_ipStat.GetTotal(today.GetDay());
		rect.top = rectClient.top + int( ( (float)rectClient.Height() * (CUtil::int64_to_float(maxval) - CUtil::int64_to_float(temp))) / CUtil::int64_to_float(maxval) );
		rect.left = rectClient.right - (((i+1)*(rectClient.Width()) / dayrange));
		rect.right = rectClient.right - (((i)*(rectClient.Width()) / dayrange));
		if (day == today.GetDay())
		{
			CUtil::DrawColorCube(&dc, rect, COLORREF(RGB(0,127,0)), COLORREF(RGB(255,255,255)),false);
		}
		else
		{
			CUtil::DrawColorCube(&dc, rect, COLORREF(RGB(0,0,127)), COLORREF(RGB(255,255,255)),false);
		}
		today -= oneday;
	}
	pDC->BitBlt(rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), 
						&dc, rectClient.left, rectClient.top, SRCCOPY);

	dc.SelectObject(pOldBmp);
	dc.RestoreDC(nSavedDC);
	dc.DeleteDC();
}

void CHistoryPage::DrawMonthDiagram(int month)
{
	CTime today;
	int monthnum;
	DWORD64 maxval;

	CDC	*pDC;
	CRect rect, rectClient;


	maxval = GetMaxMonth();

	today = CTime::GetCurrentTime();
	monthnum = today.GetMonth();

	GetDlgItem(IDC_MONTHDIAGRAM)->GetClientRect(rect);
	rectClient = rect;
	pDC = GetDlgItem(IDC_MONTHDIAGRAM)->GetDC();

	CDC dc;
	dc.CreateCompatibleDC(pDC);
	int nSavedDC = dc.SaveDC();
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	CBitmap *pOldBmp = dc.SelectObject(&bmp);

	CBrush br(::GetSysColor(COLOR_3DFACE));
	dc.FillRect(rect, &br);

	DWORD64 temp;
	for (int i=0; i<12; i++)
	{
		temp = m_pTheApp->m_wnd.m_ipStat.GetTotalMonth(monthnum);
		rect.top = rectClient.top + int(((float)rectClient.Height() * (CUtil::int64_to_float(maxval) - CUtil::int64_to_float(temp))) / CUtil::int64_to_float(maxval));
		rect.left = rectClient.right - (((i+1)*(rectClient.Width()) / 12));
		rect.right = rectClient.right - (((i)*(rectClient.Width()) / 12));
		if (month == monthnum)
		{
			CUtil::DrawColorCube(&dc, rect, COLORREF(RGB(0,127,0)), COLORREF(RGB(255,255,255)),false);
		}
		else
		{
			CUtil::DrawColorCube(&dc, rect, COLORREF(RGB(0,0,127)), COLORREF(RGB(255,255,255)),false);
		}
		monthnum--;
		if (monthnum==0)	monthnum=12;
	}
	pDC->BitBlt(rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), 
						&dc, rectClient.left, rectClient.top, SRCCOPY);

	dc.SelectObject(pOldBmp);
	dc.RestoreDC(nSavedDC);
	dc.DeleteDC();
}


void CHistoryPage::FillData(int day, int month)
{
	DWORD64 data;
	if (day == 0)
	{
		CTime date = CTime(2002,month,1,0,0,0);
		m_date = "month:  "+date.Format("%B");
		for (int i=0; i<LASTSERVICE; i++)
		{
			data = m_pTheApp->m_wnd.m_ipStat.GetIndexSentMonth(i, month);
			m_ul[i] = CUtil::GetNumberString(data);
			data = m_pTheApp->m_wnd.m_ipStat.GetIndexReceivedMonth(i, month);
			m_dl[i] = CUtil::GetNumberString(data);
		}
		data = m_pTheApp->m_wnd.m_ipStat.GetTotalReceivedMonth(month);
		m_dltot = CUtil::GetNumberString(data);
		data = m_pTheApp->m_wnd.m_ipStat.GetTotalSentMonth(month);
		m_ultot = CUtil::GetNumberString(data);
		data = m_pTheApp->m_wnd.m_ipStat.GetTotalMonth(month);
		m_tot = CUtil::GetNumberString(data);
	}
	else
	{
		CTime today = CTime::GetCurrentTime();
		CTimeSpan dt = CTimeSpan(1,0,0,0);

		while (today.GetDay() != day)
		{
			today -= dt;
		}

		m_date = today.Format("%A, %B %d, %Y");
		for (int i=0; i<LASTSERVICE; i++)
		{
			data = m_pTheApp->m_wnd.m_ipStat.GetIndexSent(i, day);
			m_ul[i] = CUtil::GetNumberString(data);
			data = m_pTheApp->m_wnd.m_ipStat.GetIndexReceived(i, day);
			m_dl[i] = CUtil::GetNumberString(data);
		}
		data = m_pTheApp->m_wnd.m_ipStat.GetTotalReceived(day);
		m_dltot = CUtil::GetNumberString(data);
		data = m_pTheApp->m_wnd.m_ipStat.GetTotalSent(day);
		m_ultot = CUtil::GetNumberString(data);
		data = m_pTheApp->m_wnd.m_ipStat.GetTotal(day);
		m_tot = CUtil::GetNumberString(data);
	}
	UpdateData(FALSE);
}


int	CHistoryPage::CalcDays()
{
	CTime t;
	CTimeSpan dt = CTimeSpan(1,0,0,0);
	int i, num;

	num = 0;
	t = CTime::GetCurrentTime();
	i = t.GetDay();
	do
	{
		num++;
		t -= dt;
	} while ( i != t.GetDay() );

	return num;
}

DWORD64 CHistoryPage::GetMaxDay()
{
	DWORD64 maxval, temp;

	CTime tim;
	CTimeSpan dt = CTimeSpan(1,0,0,0);
	int i, num;

	num = 0;
	maxval = 0;
	tim = CTime::GetCurrentTime();
	i = tim.GetDay();

	do
	{
		num++;
		temp = m_pTheApp->m_wnd.m_ipStat.GetTotal(tim.GetDay());
		maxval = max(maxval, temp);
		tim -= dt;
	} while ( i != tim.GetDay() );
	maxval = max(maxval, 1);
	return maxval;
}

DWORD64 CHistoryPage::GetMaxMonth()
{
	DWORD64 maxval, temp;

	maxval = 0;

	for (int i=1; i<13; i++)
	{
		temp = m_pTheApp->m_wnd.m_ipStat.GetTotalMonth(i);
		maxval = max(maxval, temp);
	}
	maxval = max(maxval, 1);
	return maxval;
}



/////////////////////////////////////////////////////////////////////////////
// CHistoryPage message handlers
