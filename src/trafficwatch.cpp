// TrafficWatcher - a network speed monitor

// Copyright (C) 2008 - Stefan Kueng

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
#include "trafficwatch.h"
#include "CmdLineParser.h"
#include "Registry.h"

#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

//private message for the taskbar
extern UINT TaskbarCallbackMsg;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTrafficwatchApp

BEGIN_MESSAGE_MAP(CTrafficwatchApp, CWinApp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrafficwatchApp construction

CTrafficwatchApp::CTrafficwatchApp()
{
    m_pTheApp = this;
    m_mutex = NULL;
}

CTrafficwatchApp::~CTrafficwatchApp()
{
   if(m_mutex)
      ::CloseHandle(m_mutex);
   WSACleanup( );

}


CTrafficwatchApp theApp;
CTrafficwatchApp* m_pTheApp;


/**
 * Initializes the application. Also checks if another instance of this application
 * is already running. if so, just bring the window of the running instance to front
 * and don't start another instance.
 *
 * \return BOOL  : TRUE if instance is running, FALSE if Instance was canceled.
 */
BOOL CTrafficwatchApp::InitInstance()
{
    pcap_if_t *alldevs;
    char errbuf[PCAP_ERRBUF_SIZE];

    // check if winsock2.x is available
    WSADATA WinsockData;
    if( WSAStartup(MAKEWORD(2,2), &WinsockData) != 0)
    {
        AfxMessageBox("This program requires Winsock 2.x", MB_ICONHAND );
        return FALSE;   //quit program
    }

    CCmdLineParser parser(AfxGetApp()->m_lpCmdLine);
    if (parser.HasKey(_T("initwpcap")))
    {
        // list all available adapters (this starts the NPF service which is what we want)
        if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
        {
            AfxMessageBox(errbuf);
            return FALSE;
        }
        pcap_freealldevs(alldevs);
        return FALSE;
    }
    if (parser.HasKey(_T("wpcapautostart")))
    {
        // list all available adapters (this starts the NPF service which is what we want)
        if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
        {
            AfxMessageBox(errbuf);
            return FALSE;
        }
        pcap_freealldevs(alldevs);
        // now set the service to autostart
        CRegDWORD regService = CRegDWORD(_T("SYSTEM\\CurrentControlSet\\Services\\NPF\\Start"), 0, 0, HKEY_LOCAL_MACHINE);
        regService = 2; // SERVICE_AUTO_START (for SERVICE_SYSTEM_START, use a value of 1)
        return FALSE;
    }


#ifndef DEBUG

    // only one instance of this application allowed
    m_mutex = ::CreateMutex(NULL, FALSE, M_APPNAME);

    if( m_mutex != NULL )
    {
       if(::GetLastError()==ERROR_ALREADY_EXISTS)
       {
            //an instance of this app is already running
            HWND hWnd = FindWindow(NULL, M_APPNAME);                            //try finding the running instance of this app
            if( hWnd )
            {
                PostMessage( hWnd, TaskbarCallbackMsg ,0 ,WM_LBUTTONDBLCLK );   //open the window of the already running app
                SetForegroundWindow( hWnd );                                    //set the window to front
            }
            return( FALSE );
        }
    }
#endif



    // create a hidden window to receive system tray messages
    LPCTSTR pClass = AfxRegisterWndClass( 0 );
    CRect rc;
    rc.SetRectEmpty( );

    if (m_wnd.CreateEx( WS_EX_TOOLWINDOW, pClass, M_APPNAME, WS_OVERLAPPED, rc, NULL, 0) == 0)
    {
        AfxMessageBox( "could not create window", MB_OK | MB_ICONHAND );
        return FALSE;   //quit program
    }

    m_pMainWnd = &m_wnd;
    m_wnd.StartUp( );

    return TRUE;
}
