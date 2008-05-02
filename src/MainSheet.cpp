#include "stdafx.h"
#include "trafficwatch.h"
#include "winproc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int m_nLastTab = 0;    

/////////////////////////////////////////////////////////////////////////////
// CMainSheet

IMPLEMENT_DYNAMIC(CMainSheet, CPropertySheet)


CMainSheet::CMainSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage) : CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
    AddPropPages( );
}

CMainSheet::CMainSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage) : CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
    AddPropPages( );
}

CMainSheet::~CMainSheet()
{
	delete m_pTrafficPage;
	delete m_pDetailPage;
	delete m_pHistoryPage;
	delete m_pToolPage;
	delete m_pOptionsPage;
	delete m_pAboutPage;
}


BEGIN_MESSAGE_MAP(CMainSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CMainSheet)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainSheet message handlers

void CMainSheet::AddPropPages( )
{
	m_pTrafficPage = new CTrafficPage;
	m_pDetailPage = new CDetailPage;
	m_pHistoryPage = new CHistoryPage;
	m_pToolPage = new CToolPage;
	m_pOptionsPage = new COptionsPage;
	m_pAboutPage = new CAboutPage;

	AddPage( m_pTrafficPage );
	AddPage( m_pDetailPage );
	AddPage( m_pHistoryPage );
	AddPage( m_pToolPage );
	AddPage( m_pOptionsPage );
	AddPage( m_pAboutPage );

	SetActivePage(m_nLastTab);

}


///////////////////////////////////////////////////////////////////////////////////////////
//
BOOL CMainSheet::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
    //save last tab
    m_nLastTab = GetActiveIndex( );    	
	return CPropertySheet::OnChildNotify(message, wParam, lParam, pLResult);
}


///////////////////////////////////////////////////////////////////////////////////////////
//
BOOL CMainSheet::OnInitDialog() 
{
    BOOL bResult = CPropertySheet::OnInitDialog();

	GetDlgItem( IDOK )->ShowWindow( FALSE ); 
	GetDlgItem( IDCANCEL )->ShowWindow( FALSE ); 


	m_fontLogo.CreateFont(24, 0, 0, 0, FW_BOLD, true, false,0,0,0,0,0,0, "Arial");

  
   	HICON m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(m_hIcon, TRUE);         // Set big icon
	SetIcon(m_hIcon, FALSE);        // Set small icon

    UpdateData(FALSE);
    //position the dialog
    CRect rc;
    GetClientRect( rc );
    LoadWindowPosition( &rc );
    BOOL bCenter = FALSE;
    if( rc.left < 0 || ( rc.right ) >= GetSystemMetrics(SM_CXSCREEN) )
        bCenter = TRUE;

    if( rc.top < 0 || ( rc.bottom ) >= GetSystemMetrics(SM_CYSCREEN) )
        bCenter = TRUE;

    if( bCenter ) 
    {
        GetWindowRect( rc );
        rc.OffsetRect(-rc.left, -rc.top);
        MoveWindow( ((GetSystemMetrics(SM_CXSCREEN) - rc.right ) / 2 + 4) & ~7,
			         (GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2, rc.right, rc.bottom, 0);
    } 
	else 
	{
        SetWindowPos( NULL,rc.left,rc.top,0,0,SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOSIZE );
	}

	SetActivePage(m_pTrafficPage);

	return bResult;
}

void CMainSheet::LoadWindowPosition(CRect *rc)
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
		RegQueryValueEx(hKey, "windowpos", 0, &type, (unsigned char *)rc, &valuesize);
	}
	RegCloseKey(hKey);
}

void CMainSheet::SaveWindowPosition(CRect *rc)
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
		RegSetValueEx(hKey, "windowpos", 0, REG_BINARY, (unsigned char *)rc, valuesize);
	}
	RegCloseKey(hKey);
}
///////////////////////////////////////////////////////////////////////////////////////////
//
void CMainSheet::OnDestroy() 
{
    CRect rc;
    GetWindowRect( rc );
    SaveWindowPosition( &rc );
    m_pTheApp->m_pMainWnd = &m_pTheApp->m_wnd;
	CPropertySheet::OnDestroy();
}



///////////////////////////////////////////////////////////////////////////////////////////
//
void CMainSheet::PostNcDestroy() 
{
    //restore mfc's main window handle
    m_pTheApp->m_pMainWnd = &m_pTheApp->m_wnd;
	CPropertySheet::PostNcDestroy();
}




///////////////////////////////////////////////////////////////////////////////////////////
//
LRESULT CMainSheet::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
    if( message == WM_SIZE && wParam == SIZE_MINIMIZED )
    {
		CRect rc;
		GetWindowRect( rc );
		SaveWindowPosition( &rc );
        PostMessage( WM_CLOSE,0,0);
    }          

	return CPropertySheet::DefWindowProc(message, wParam, lParam);
}


///////////////////////////////////////////////////////////////////////////////////////////
//
int CMainSheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;
	
//    ModifyStyle( DS_CONTEXTHELP | DS_MODALFRAME, WS_MINIMIZEBOX );    
//    ModifyStyle( DS_CONTEXTHELP | DS_MODALFRAME, 0 );    

	ModifyStyleEx(WS_EX_TOOLWINDOW, WS_EX_APPWINDOW);

	
	// Configure our system menu
	GetSystemMenu( TRUE ); //revert the menu
	CMenu* pSysMenu = GetSystemMenu( FALSE ); //and grab the handle
    pSysMenu->EnableMenuItem( SC_MINIMIZE, MF_ENABLED );
	pSysMenu->DeleteMenu( SC_MAXIMIZE, MF_BYCOMMAND );	  
	pSysMenu->DeleteMenu( SC_SIZE, MF_BYCOMMAND );
	pSysMenu->EnableMenuItem( SC_RESTORE, MF_DISABLED | MF_GRAYED );
    	
	return 0;
}

void CMainSheet::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CString m_LogoText = "Kings";

        

	// So Get the Dialog's Window Rect  
	CRect rectTabCtrl;
	GetTabControl()->GetWindowRect(rectTabCtrl);
	ScreenToClient(rectTabCtrl);


	CRect rectOk;
	GetDlgItem(IDOK)->GetWindowRect(rectOk);
	ScreenToClient(rectOk);

	dc.SetBkMode(TRANSPARENT);

	CRect rectText;
	rectText.left = rectTabCtrl.left;
	rectText.top = rectOk.top;
	rectText.bottom = rectOk.bottom;
	rectText.right = rectOk.left;

	CFont * OldFont = dc.SelectObject(&m_fontLogo);

	// draw text in DC
	COLORREF OldColor = dc.SetTextColor( ::GetSysColor( COLOR_3DHILIGHT));


	dc.DrawText(m_LogoText, rectText + CPoint(2,2), DT_SINGLELINE | DT_LEFT | DT_VCENTER);
	dc.SetTextColor( ::GetSysColor( COLOR_3DSHADOW));
	dc.DrawText( m_LogoText, rectText, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	dc.SetTextColor( ::GetSysColor( COLOR_3DDKSHADOW ));
	dc.DrawText( m_LogoText, rectText - CPoint(2,2),  DT_SINGLELINE | DT_LEFT | DT_VCENTER);

	// restore old text color
	dc.SetTextColor( OldColor);

	// restore old font
	dc.SelectObject(OldFont);
}



void CMainSheet::OnClose() 
{
    CRect rc;
    GetWindowRect( rc );
    SaveWindowPosition( &rc );
    m_pTheApp->m_pMainWnd = &m_pTheApp->m_wnd;

	
	CPropertySheet::OnClose();
	delete this;
	m_pTheApp->m_wnd.m_pDialog = 0;
	delete m_pTheApp->m_wnd.m_pWorldview;
	m_pTheApp->m_wnd.m_pWorldview = 0;
}
