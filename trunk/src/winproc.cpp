#include "stdafx.h"
#include "Trafficwatch.h"
#include "winproc.h"
#include "resource.h"
#include "TitleMenu.h"
#include "util.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


UINT TaskbarCallbackMsg = RegisterWindowMessage("NPSTaskbarMsg");	


/////////////////////////////////////////////////////////////////////////////
// CWinproc

CWinproc::CWinproc()
{
    m_TaskBarIcon = 0;
    ZeroMemory( &m_TaskBarIconInfo, sizeof( m_TaskBarIconInfo ) );
	ZeroMemory( &m_SystemTray, sizeof( m_SystemTray ) );
	m_bView = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// 
CWinproc::~CWinproc()
{
}


/////////////////////////////////////////////////////////////////////////////
// CWinproc
void CWinproc::OnClose() 
{
	m_ipStat.close();
	if( m_SystemTray.hWnd )
		Shell_NotifyIcon( NIM_DELETE, &m_SystemTray );
	CWnd::OnClose();
}

/////////////////////////////////////////////////////////////////////////////
// CWinproc
BEGIN_MESSAGE_MAP(CWinproc, CWnd)
	ON_WM_CLOSE()
	ON_WM_TIMER()
    ON_REGISTERED_MESSAGE(TaskbarCallbackMsg, OnTaskbarNotify)	
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWinproc message handlers



/////////////////////////////////////////////////////////////////////////////
// Startup -- called when the invisible window is created in netpersec.cpp
// initializes SNMP and the system tray icon
void CWinproc::StartUp( )
{

	HKEY hKey;
	LONG lnRes;
	CString key;
	key = "SOFTWARE\\";
	key += LPCTSTR( M_APPNAME );
	lnRes = RegOpenKeyEx(HKEY_CURRENT_USER, key, 0, KEY_READ, &hKey);
	if (lnRes == ERROR_SUCCESS)
	{
		DWORD value, valuesize;
		ULONG type;
		valuesize = sizeof(value);
		if (!RegQueryValueEx(hKey, "adapter", 0, &type, (unsigned char *)&value, &valuesize) == ERROR_SUCCESS)
			value = -1;
		if (m_ipStat.init(value) == FALSE)
		{
			AfxMessageBox("failed to initialize network adapter!", MB_ICONHAND );
		}
	}
	RegCloseKey(hKey);


	dwTotalRec = m_ipStat.GetTotalReceived();
	dwTotalSent = m_ipStat.GetTotalSent();
	dwTotalRecOld = dwTotalRec;
	dwTotalSentOld = dwTotalSent;
	dwOldTime = GetTickCount();

	m_AppIcon = (HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE( IDR_MAINFRAME ),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);

	HICON hIcon;
	hIcon = m_AppIcon;



	m_SystemTray.cbSize = sizeof( NOTIFYICONDATA );
	m_SystemTray.hWnd   = GetSafeHwnd( );
	m_SystemTray.uID    = 1;
	m_SystemTray.hIcon  = hIcon;
	m_SystemTray.uFlags = NIF_MESSAGE | NIF_ICON;
	m_SystemTray.uCallbackMessage = TaskbarCallbackMsg;
	if( !Shell_NotifyIcon(NIM_ADD, &m_SystemTray ) )
		AfxMessageBox("System tray error.");
	SetTimer(IDT_STATISTICS, 1000, NULL);
}



/////////////////////////////////////////////////////////////////////////////
// 
void CWinproc::ShowDialog( )
{
	if (m_pDialog)
	{
		m_pDialog->DestroyWindow();
		delete m_pDialog;
		m_pDialog = 0;
	}
	else
	{
		m_pTheApp->m_pMainWnd = NULL;
		m_pDialog = new CMainSheet( M_APPNAME, NULL );
		m_pDialog->Create(NULL);
		m_pDialog->m_psh.dwFlags |= PSH_NOAPPLYNOW;
		m_pDialog->m_psh.dwFlags |= PSH_MODELESS;

		m_pDialog->ShowWindow(SW_SHOW);
	}

}

void CWinproc::ShowView( )
{
	if (m_pTView)
	{
		m_pTView->DestroyWindow();
		delete m_pTView;
		m_pTView = 0;
	}
	else
	{
		m_pTheApp->m_pMainWnd = NULL;

		m_pTView = new CTrafficView();
		m_pTView->Create(IDD_VIEW);
		m_pTView->ShowWindow(SW_SHOW);
	}
}

/////////////////////////////////////////////////////////////////////////////
// 
LRESULT CWinproc::OnTaskbarNotify( WPARAM wParam, LPARAM lParam)	
{
	UNREFERENCED_PARAMETER(wParam);

	switch( lParam )
	{			
        case WM_MOUSEMOVE:
		{
			CString s,sRecvBPS,sRecvAVE;
				
            s = "Upload: "+CUtil::GetNumberString(ulSpeed)+"/s\nDownload: "+CUtil::GetNumberString(dlSpeed)+"/s";

			m_SystemTray.cbSize = sizeof(NOTIFYICONDATA);
			m_SystemTray.hWnd   = GetSafeHwnd( );
			m_SystemTray.uID    = 1;
			m_SystemTray.uFlags = NIF_TIP;
			strncpy( m_SystemTray.szTip, s, sizeof( m_SystemTray.szTip ) );
			Shell_NotifyIcon( NIM_MODIFY, &m_SystemTray );		
		}
		break;

        case WM_LBUTTONDBLCLK:
			ShowDialog( );
            break;

        case WM_RBUTTONUP:
        {
            POINT pt;
			
            GetCursorPos( &pt );

			CTitleMenu menu;
			menu.CreatePopupMenu();
			menu.AddMenuTitle( M_APPNAME );
			if (m_pTView)
				menu.AppendMenu(MF_STRING | MF_CHECKED, ID_SHOWONTOP, "View");
			else
				menu.AppendMenu(MF_STRING, ID_SHOWONTOP, "View");
			menu.AppendMenu(MF_STRING,ID_SHOWDIALOG,"Open");
			menu.AppendMenu(MF_STRING,ID_CLOSE,"Exit");
			menu.SetDefaultItem(ID_SHOWDIALOG, FALSE);
			menu.SetColor(RGB(150,0,0));
			menu.SetGradientColor(RGB(0,0,150));

			SetForegroundWindow( );
			int cmd = menu.TrackPopupMenu( TPM_RETURNCMD | TPM_LEFTALIGN | TPM_NONOTIFY , pt.x, pt.y,this );
			PostMessage( WM_NULL, 0, 0);

            switch( cmd )
            {
                case ID_CLOSE:
                    //save any settings if the user closes the tray icon while the dlg is open                    
                    //TerminateThread(m_ipStats.pThr->m_hThread,0);
					if (m_pDialog)
					{
						m_pDialog->DestroyWindow();
						delete m_pDialog;
						m_pDialog = 0;
					}
					if (m_pTView)
					{
						m_pTView->DestroyWindow();
						delete m_pTView;
						m_pTView = 0;
					}
                    m_pTheApp->m_wnd.PostMessage(WM_CLOSE);
                    break;
                
                case ID_SHOWDIALOG:
                    ShowDialog( );
                    break;

				case ID_SHOWONTOP:
					ShowView( );
					break;

                }
            }
			break;
		
	}
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// 
void CWinproc::UpdateTrayIcon( HICON hIcon )
{
    ASSERT( hIcon != 0 );

	if( m_SystemTray.hWnd && hIcon )
	{
		m_SystemTray.cbSize = sizeof(NOTIFYICONDATA);
		m_SystemTray.hWnd = GetSafeHwnd( );
		m_SystemTray.uID = 1;
		m_SystemTray.hIcon = hIcon;
		m_SystemTray.uFlags = NIF_ICON;
		m_SystemTray.uCallbackMessage = TaskbarCallbackMsg;
		Shell_NotifyIcon( NIM_MODIFY, &m_SystemTray );
	}
}


void CWinproc::OnTimer(UINT nIDEvent)
{
	// Increment the Animated bar offset and redraw the window.
	if (nIDEvent == IDT_STATISTICS)
	{
		KillTimer(nIDEvent);

		dwTotalRec = m_ipStat.GetTotalReceived();
		dwTotalSent = m_ipStat.GetTotalSent();
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
		if (dlSpeed > dwDlMax)
			dwDlMax = dlSpeed;
		if (ulSpeed > dwUlMax)
			dwUlMax = ulSpeed;
		ulSpeed = DWORD(dwSentDiff * (DWORD64)1000 / dwElapsed);

		//get the icon for the system tray
		if (((dlSpeed/1000) == 0)&&((ulSpeed/1000) == 0))
		{
			UpdateTrayIcon( m_AppIcon );
		}
		else
		{
			HICON hIcon = GetTaskBarIcon();
			UpdateTrayIcon( hIcon );    
			DestroyIcon( hIcon );
		}
		internaltimer++;
		if (internaltimer > 120)
		{
			internaltimer = 0;
			m_ipStat.Save();
		}

	}

	SetTimer(IDT_STATISTICS, 1000, NULL);
}


HICON	CWinproc::GetTaskBarIcon()
{
	if (m_TaskBarIcon == 0)
	{
        m_TaskBarIcon = (HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE( IDI_TASKBAR ),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
   		GetIconInfo( m_TaskBarIcon, &m_TaskBarIconInfo ); 
	  	m_bmpIcon.Attach( m_TaskBarIconInfo.hbmColor );
		ASSERT( m_TaskBarIcon != 0 );
	}
	CDC dcMem;
	dcMem.CreateCompatibleDC(NULL);
	
	CBitmap* pOld = dcMem.SelectObject( &m_bmpIcon );

    //offsets for the left and right halves of the icon
    CRect rcRecv(1,1,7,15);
    CRect rcSent(9,1,15,15);

    CBrush back( RGB(255,255,255) );
    dcMem.FillRect( rcSent, &back );
    dcMem.FillRect( rcRecv, &back );

    int nIcon =  MulDiv(16, dlSpeed/1000, COptionsPage::GetDownloadSpeed());

    rcRecv.top =  rcRecv.bottom - nIcon;
    CBrush brush( COLORDOWN );
    dcMem.FillRect( rcRecv, &brush );

    nIcon =  MulDiv(16, ulSpeed/1000, COptionsPage::GetUploadSpeed());

    rcSent.top =  rcSent.bottom - nIcon;
    CBrush brush1( COLORUP );
    dcMem.FillRect( rcSent, &brush1 );


    dcMem.SelectObject( pOld );

    HICON hIcon = CreateIconIndirect( &m_TaskBarIconInfo );  
	ASSERT(hIcon != NULL);
    dcMem.DeleteDC( );
    return( hIcon );  //calling function must delete this icon handle
}