#if !defined(AFX_WINPROC_H__6564E3DE_EB5F_4094_9AEC_CF329AE35837__INCLUDED_)
#define AFX_WINPROC_H__6564E3DE_EB5F_4094_9AEC_CF329AE35837__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "globals.h"
#include "MainSheet.h"
#include "trafficview.h"
#include "ipstat.h"
#include "worldview.h"

#define IDT_STATISTICS 103

/**
 * CWinproc.
 * this class represents the main application. it creates a hidden window to
 * receive window messages and creates an icon in the toolbar. it handles all
 * window messages and events, starts and initializes the underlying services
 * like the sniffer, shows up the dialogbox if requested.
 */
class CWinproc : public CWnd
{
// Construction
public:
	CWinproc();
	void StartUp( );
    LRESULT OnTaskbarNotify( WPARAM wParam, LPARAM lParam );
    void UpdateTrayIcon( HICON hIcon );
    void ShowDialog( );
    void ShowView( );

// Attributes
public:
    CMainSheet*	m_pDialog;
	CTrafficView* m_pTView;
	CWorldView* m_pWorldview;

	CIPStat		m_ipStat;

    NOTIFYICONDATA	m_SystemTray;

	DWORD		dwUlMax, dwDlMax;
	BOOL		m_bView;

protected://methods
	HICON		GetTaskBarIcon();
protected://members
	DWORD64		dwRecDiff, dwSentDiff, dwTotalRec, dwTotalRecOld, dwTotalSent, dwTotalSentOld;		
	DWORD		dwTime, dwOldTime;
	DWORD		ulSpeed, dlSpeed, dwElapsed;
	int			internaltimer;
	HICON		m_AppIcon;
	HICON		m_TaskBarIcon;
    ICONINFO	m_TaskBarIconInfo;
    CBitmap		m_bmpIcon;
// Operations
public:

    

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Cwinproc)
	public:
	//}}AFX_VIRTUAL
    

// Implementation
public:
	virtual ~CWinproc();

	// Generated message map functions
protected:
	//{{AFX_MSG(Cwinproc)
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINPROC_H__6564E3DE_EB5F_4094_9AEC_CF329AE35837__INCLUDED_)
