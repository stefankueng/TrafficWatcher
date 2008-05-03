#pragma once


#include "globals.h"
#include "MainSheet.h"
#include "trafficview.h"
#include "ipstat.h"
#include "Filter.h"


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

	CIPStat		m_ipStat;

    NOTIFYICONDATA	m_SystemTray;

	DWORD		dwUlMax, dwDlMax;
	BOOL		m_bView;

protected:
	HICON		GetTaskBarIcon();
protected:
	DWORD		ulSpeed, dlSpeed;
	DWORD		ulSpeedLAN, dlSpeedLAN;

	CFilter		m_sentFilter;
	CFilter		m_receivedFilter;
	CFilter		m_sentLANFilter;
	CFilter		m_receivedLANFilter;

	int			internaltimer;
	HICON		m_AppIcon;
	HICON		m_TaskBarIcon;
    ICONINFO	m_TaskBarIconInfo;
    CBitmap		m_bmpIcon;

public:
	virtual ~CWinproc();

protected:
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
};

