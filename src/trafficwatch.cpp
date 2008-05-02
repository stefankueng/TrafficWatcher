// trafficwatch.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "trafficwatch.h"

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
	//{{AFX_MSG_MAP(CTrafficwatchApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrafficwatchApp construction

CTrafficwatchApp::CTrafficwatchApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_pTheApp = this;
	m_mutex = NULL;
}

CTrafficwatchApp::~CTrafficwatchApp()
{
   if(m_mutex)
      ::CloseHandle(m_mutex);
   WSACleanup( );

}

/////////////////////////////////////////////////////////////////////////////
// The one and only CNetPerSecApp object

CTrafficwatchApp theApp;
CTrafficwatchApp* m_pTheApp;


/*!
 * Initializes the application. Also checks if another instance of this application
 * is already running. if so, just bring the window of the running instance to front
 * and don't start another instance.
 *
 * @return BOOL  : TRUE if instance is running, FALSE if Instance was cancelled.
 */
BOOL CTrafficwatchApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.



#ifndef DEBUG
	
	//only one instance of this application allowed
	m_mutex = ::CreateMutex(NULL, FALSE, M_APPNAME);
	
	if( m_mutex != NULL )
	{   
       if(::GetLastError()==ERROR_ALREADY_EXISTS)
	   {
			//an instance of this app is already running
			HWND hWnd = FindWindow(NULL, M_APPNAME);							//try finding the running instance of this app
			if( hWnd )
			{
                PostMessage( hWnd, TaskbarCallbackMsg ,0 ,WM_LBUTTONDBLCLK );	//open the window of the already running app
				SetForegroundWindow( hWnd );									//set the window to front
            }
			return( FALSE );
		}		
	}
#endif

	
	//check if winsock2.x is available	
    WSADATA WinsockData;
    if( WSAStartup(MAKEWORD(2,2), &WinsockData) != 0) 
    {
        AfxMessageBox("This program requires Winsock 2.x", MB_ICONHAND );
		return FALSE;	//quit program
    }
	
	
    //create a hidden window to receive system tray messages
    LPCTSTR pClass = AfxRegisterWndClass( 0 ); 
    CRect rc;
   	rc.SetRectEmpty( );

	if( m_wnd.CreateEx( WS_EX_TOOLWINDOW, pClass, M_APPNAME, WS_OVERLAPPED, rc, NULL, 0) == 0 )
	{
		AfxMessageBox( "could not create window", MB_OK | MB_ICONHAND );
		return FALSE;	//quit program
	}
	
    m_pMainWnd = &m_wnd;
    m_wnd.StartUp( );


	return TRUE;
	
}
