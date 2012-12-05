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
#include "stdafx.h"
#include "Trafficwatch.h"
#include "winproc.h"
#include "resource.h"
#include "Util.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


UINT TaskbarCallbackMsg = RegisterWindowMessage(L"NPSTaskbarMsg");


/////////////////////////////////////////////////////////////////////////////
// CWinproc

CWinproc::CWinproc()
    : m_TaskBarIcon(0)
    , m_pDialog(NULL)
    , m_pTView(NULL)
{
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
    CRegString regAdapter(L"SOFTWARE\\" M_APPNAME L"\\adapter");
    CString sAdapter = regAdapter;
    if (!sAdapter.IsEmpty())
    {
        DWORD value = (DWORD)-1;
        m_ipStat.init(value);
        for (int i=0; i<m_ipStat.GetNumberOfAdapters(); i++)
        {
            if (m_ipStat.GetDescription(i).Compare(sAdapter) == 0)
            {
                value = i;
                break;
            }
        }
        if (m_ipStat.init(value) == FALSE)
        {
            AfxMessageBox(L"failed to initialize network adapter!", MB_ICONHAND );
        }
    }

    DWORD dwTicks = GetTickCount();

    m_sentFilter.Filter(dwTicks, m_ipStat.GetTotalSent());
    m_receivedFilter.Filter(dwTicks, m_ipStat.GetTotalReceived());
    m_sentLANFilter.Filter(dwTicks, m_ipStat.GetTotalSentLAN());
    m_receivedLANFilter.Filter(dwTicks, m_ipStat.GetTotalReceivedLAN());

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
        AfxMessageBox(L"System tray error.");
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

            s = L"Internet-Upload: " + CUtil::GetNumberString(ulSpeed) + L"/s\nInternet-Download: " + CUtil::GetNumberString(dlSpeed) + L"/s\n"
                + L"LAN-Upload: " + CUtil::GetNumberString(ulSpeedLAN) + L"/s\nLAN-Download: " + CUtil::GetNumberString(dlSpeedLAN) + L"/s";

            m_SystemTray.cbSize = sizeof(NOTIFYICONDATA);
            m_SystemTray.hWnd   = GetSafeHwnd( );
            m_SystemTray.uID    = 1;
            m_SystemTray.uFlags = NIF_TIP;
            wcsncpy_s( m_SystemTray.szTip, sizeof( m_SystemTray.szTip ), s, sizeof( m_SystemTray.szTip ) );
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

            CMenu menu;
            menu.CreatePopupMenu();
            if (m_pTView)
                menu.AppendMenu(MF_STRING | MF_CHECKED, ID_SHOWONTOP, L"View");
            else
                menu.AppendMenu(MF_STRING, ID_SHOWONTOP, L"View");
            menu.AppendMenu(MF_STRING,ID_SHOWDIALOG, L"Open");
            menu.AppendMenu(MF_STRING,ID_CLOSE, L"Exit");
            menu.SetDefaultItem(ID_SHOWDIALOG, FALSE);

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


void CWinproc::OnTimer(UINT_PTR nIDEvent)
{
    // Increment the Animated bar offset and redraw the window.
    if (nIDEvent == IDT_STATISTICS)
    {
        KillTimer(nIDEvent);

        DWORD dwTicks = GetTickCount();

        ulSpeed = (DWORD)m_sentFilter.Filter(dwTicks, m_ipStat.GetTotalSent());
        dlSpeed = (DWORD)m_receivedFilter.Filter(dwTicks, m_ipStat.GetTotalReceived());
        ulSpeedLAN = (DWORD)m_sentLANFilter.Filter(dwTicks, m_ipStat.GetTotalSentLAN());
        dlSpeedLAN = (DWORD)m_receivedLANFilter.Filter(dwTicks, m_ipStat.GetTotalReceivedLAN());

        HICON hIcon = GetTaskBarIcon();
        UpdateTrayIcon( hIcon );
        DestroyIcon( hIcon );

        internaltimer++;
        if (internaltimer > 120)
        {
            internaltimer = 0;
        }

    }

    SetTimer(IDT_STATISTICS, 1000, NULL);
}


HICON   CWinproc::GetTaskBarIcon()
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
    CRect rcRecv(1,1,4,15);
    CRect rcSent(4,1,8,15);
    CRect rcRecvLAN(8,1,12,15);
    CRect rcSentLAN(12,1,15,15);

    CBrush back( RGB(0,0,0) );
    dcMem.FillRect( CRect(0,0,16,16), &back );

    int nIcon = 0;
    nIcon = (int)((16.0 * sqrt(dlSpeed/1000.0)) / sqrt((double)COptionsPage::GetDownloadSpeed()));
    if (dlSpeed == 0)
        nIcon = 1;
    if (nIcon == 0)
        nIcon = 1;
    rcRecv.top =  rcRecv.bottom - nIcon;
    CBrush brush( COLORDOWN );
    dcMem.FillRect( rcRecv, &brush );

    nIcon = (int)((16.0 * sqrt(ulSpeed/1000.0)) / sqrt((double)COptionsPage::GetUploadSpeed()));
    if (ulSpeed == 0)
        nIcon = 1;
    if (nIcon == 0)
        nIcon = 1;
    rcSent.top =  rcSent.bottom - nIcon;
    CBrush brush1( COLORUP );
    dcMem.FillRect( rcSent, &brush1 );

    nIcon = (int)((16.0 * sqrt(ulSpeedLAN/1000.0)) / sqrt((double)COptionsPage::GetDownloadSpeedLAN()));
    if (ulSpeedLAN == 0)
        nIcon = 1;
    if (nIcon == 0)
        nIcon = 1;
    rcSentLAN.top =  rcSentLAN.bottom - nIcon;
    CBrush brush2( COLORUPLAN );
    dcMem.FillRect( rcSentLAN, &brush2 );

    nIcon = (int)((16.0 * sqrt(dlSpeedLAN/1000.0)) / sqrt((double)COptionsPage::GetUploadSpeedLAN()));
    if (dlSpeedLAN == 0)
        nIcon = 1;
    if (nIcon == 0)
        nIcon = 1;
    rcRecvLAN.top =  rcRecvLAN.bottom - nIcon;
    CBrush brush3( COLORDOWNLAN );
    dcMem.FillRect( rcRecvLAN, &brush3 );

    dcMem.SelectObject( pOld );

    HICON hIcon = CreateIconIndirect( &m_TaskBarIconInfo );
    ASSERT(hIcon != NULL);
    dcMem.DeleteDC( );
    return( hIcon );  //calling function must delete this icon handle
}
