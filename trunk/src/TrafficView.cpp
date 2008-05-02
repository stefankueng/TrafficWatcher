// TrafficView.cpp : implementation file
//

#include "stdafx.h"
#include "trafficwatch.h"
#include "TrafficView.h"
#include "optionspage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrafficView dialog


CTrafficView::CTrafficView(CWnd* pParent /*=NULL*/)
	: CDialog(CTrafficView::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTrafficView)
	//}}AFX_DATA_INIT
}


void CTrafficView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrafficView)
	DDX_Control(pDX, IDC_ULBAR, m_ulbar);
	DDX_Control(pDX, IDC_DLBAR, m_dlbar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTrafficView, CDialog)
	//{{AFX_MSG_MAP(CTrafficView)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrafficView message handlers

BOOL CTrafficView::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_nYOffset = 50;
	m_nXOffset = 100;

	// we make sure the Window has no Border,Title 
	ModifyStyle( WS_CAPTION | WS_BORDER,0);
	
	// remove the dialog from the taskbar
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	SetWindowPos( &wndTopMost,0,0,0,0,SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE );
	
	m_dlbar.SetAnimated();
	m_dlbar.SetMaximumTime();
	m_dlbar.SetShowMaximum();
	m_dlbar.SetMaximumTime(50);
	m_dlbar.SetFontColor(RGB(255, 0, 0));
	m_dlbar.SetColor(COLORDOWN);
	m_ulbar.SetAnimated();
	m_ulbar.SetMaximumTime();
	m_ulbar.SetShowMaximum();
	m_ulbar.SetMaximumTime(50);
	m_ulbar.SetFontColor(RGB(255, 0, 0));
	m_ulbar.SetColor(COLORUP);
	m_dlbar.SetRange(0, (short)(COptionsPage::GetDownloadSpeed()));
	m_ulbar.SetRange(0, (short)(COptionsPage::GetUploadSpeed()));

	dwTotalRec = m_pTheApp->m_wnd.m_ipStat.GetTotalReceived();
	dwTotalSent = m_pTheApp->m_wnd.m_ipStat.GetTotalSent();
	dwTotalRecOld = dwTotalRec;
	dwTotalSentOld = dwTotalSent;
	SetTimer( IDT_TRAFFIC, 1000, NULL );
	
	UpdateData(FALSE);

	
    //position the dialog
    CRect rc;
    GetClientRect( rc );
    LoadWindowPosition( &rc );
    BOOL bCenter = FALSE;
    if( rc.left < 0 || ( rc.right ) > GetSystemMetrics(SM_CXSCREEN) )
        bCenter = TRUE;

    if( rc.top < 0 || ( rc.bottom ) > GetSystemMetrics(SM_CYSCREEN) )
        bCenter = TRUE;

	int temp = GetSystemMetrics(SM_CYFULLSCREEN);

    if( bCenter ) 
    {
		GetClientRect( rc );
        GetWindowRect( rc );
        rc.OffsetRect(-rc.left, -rc.top);
        MoveWindow( ((GetSystemMetrics(SM_CXSCREEN) - rc.right ) / 2 + 4) & ~7,
			         (GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2, rc.right, rc.bottom, 0);
    }
	else 
	{
		SetWindowPos( NULL,rc.left,rc.top,0,0,SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOSIZE );
	}
	
	
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CTrafficView::LoadWindowPosition(CRect *rc)
{
	HKEY hKey;
	LONG lnRes;
	CString key;
	key = "SOFTWARE\\";
	key += LPCTSTR( M_APPNAME );
	lnRes = RegOpenKeyEx(HKEY_CURRENT_USER, key, 0, KEY_READ, &hKey);
	if (lnRes == ERROR_SUCCESS)
	{
		DWORD valuesize;
		ULONG type;
		valuesize = sizeof(CRect);
		RegQueryValueEx(hKey, "viewpos", 0, &type, (unsigned char *)rc, &valuesize);
	}
	RegCloseKey(hKey);
}

void CTrafficView::SaveWindowPosition(CRect *rc)
{
	HKEY hKey;
	CString key;
	key = "SOFTWARE\\";
	key += LPCTSTR( M_APPNAME );
	DWORD value, valuesize;
	valuesize = sizeof(value);
	LONG lnRes = RegCreateKeyEx(HKEY_CURRENT_USER, key, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &value);
	if (lnRes == ERROR_SUCCESS)
	{
		valuesize = sizeof(CRect);
		RegSetValueEx(hKey, "viewpos", 0, REG_BINARY, (unsigned char *)rc, valuesize);
	}
	RegCloseKey(hKey);
}

void CTrafficView::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
    m_pTheApp->m_pMainWnd = &m_pTheApp->m_wnd;
	CDialog::PostNcDestroy();
}

void CTrafficView::OnClose() 
{
    CRect rc;
    GetWindowRect( rc );
    SaveWindowPosition( &rc );
    m_pTheApp->m_pMainWnd = &m_pTheApp->m_wnd;
	CDialog::OnClose();
}

void CTrafficView::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == IDT_TRAFFIC)
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

		m_dlbar.SetText(CUtil::GetNumberString(dlSpeed)+"/s");
		m_ulbar.SetText(CUtil::GetNumberString(ulSpeed)+"/s");

		m_dlbar.SetPos(dlSpeed/1000);		//range is in kbytes/s, not bytes/s!
		m_ulbar.SetPos(ulSpeed/1000);


		UpdateData(FALSE);
		SetTimer(IDT_TRAFFIC, 1000, NULL);
	}
}



void CTrafficView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(nFlags == MK_LBUTTON)
	{
		::ReleaseCapture();
		::SendMessage(m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CTrafficView::OnWindowPosChanging( WINDOWPOS* lpwndpos )
{
	CRect	wndRect, trayRect;
	int		leftTaskbar = 0, rightTaskbar = 0, topTaskbar = 0, bottomTaskbar = 0;

	GetWindowRect(&wndRect);

	// Screen resolution
	int screenWidth =	GetSystemMetrics(SM_CXSCREEN); 
	int screenHeight =	GetSystemMetrics(SM_CYSCREEN);
	
	
/*	CRect taskbarrect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &taskbarrect, 0);
	screenHeight = taskbarrect.Height();
*/
	// Find the taskbar
	CWnd* pWnd = FindWindow(_T("Shell_TrayWnd"), _T(""));
	pWnd->GetWindowRect(&trayRect);

	int wndWidth = wndRect.right - wndRect.left;
	int wndHeight = wndRect.bottom - wndRect.top;

	if(trayRect.top <= 0 && trayRect.left <= 0 && trayRect.right >= screenWidth) {
		// top taskbar
		topTaskbar = trayRect.bottom - trayRect.top;
	}
	else if(trayRect.top > 0 && trayRect.left <= 0) {
		// bottom taskbar
		bottomTaskbar = trayRect.bottom - trayRect.top;
	}
	else if(trayRect.top <= 0 && trayRect.left > 0) {
		// right taskbar
		rightTaskbar = trayRect.right - trayRect.left;
	}
	else {
		// left taskbar
		leftTaskbar = trayRect.right - trayRect.left;
	}

	// Snap to screen border
	// Left border
	if(lpwndpos->x >= -m_nXOffset + leftTaskbar && lpwndpos->x <= leftTaskbar + m_nXOffset) {
		lpwndpos->x = leftTaskbar;
	}

	// Top border
	if(lpwndpos->y >= -m_nYOffset && lpwndpos->y <= topTaskbar + m_nYOffset) {
		lpwndpos->y = topTaskbar;
	}

	// Right border
	if(lpwndpos->x + wndWidth <= screenWidth - rightTaskbar + m_nXOffset && lpwndpos->x + wndWidth >= screenWidth - rightTaskbar - m_nXOffset) {
		lpwndpos->x = screenWidth - rightTaskbar - wndWidth;
	}

	// Bottom border
	if( lpwndpos->y + wndHeight <= screenHeight - bottomTaskbar + m_nYOffset && lpwndpos->y + wndHeight >= screenHeight - bottomTaskbar - m_nYOffset) {
		lpwndpos->y = screenHeight - bottomTaskbar - wndHeight;
	}
	CDialog::OnWindowPosChanging(lpwndpos);
}




void CTrafficView::OnDestroy() 
{
    CRect rc;
    GetWindowRect( rc );
    SaveWindowPosition( &rc );
    m_pTheApp->m_pMainWnd = &m_pTheApp->m_wnd;
	CDialog::OnDestroy();
}
