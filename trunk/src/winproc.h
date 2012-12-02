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
    CMainSheet* m_pDialog;
    CTrafficView* m_pTView;

    CIPStat     m_ipStat;

    NOTIFYICONDATA  m_SystemTray;

    DWORD       dwUlMax, dwDlMax;
    BOOL        m_bView;

protected:
    HICON       GetTaskBarIcon();
protected:
    DWORD       ulSpeed, dlSpeed;
    DWORD       ulSpeedLAN, dlSpeedLAN;

    CFilter     m_sentFilter;
    CFilter     m_receivedFilter;
    CFilter     m_sentLANFilter;
    CFilter     m_receivedLANFilter;

    int         internaltimer;
    HICON       m_AppIcon;
    HICON       m_TaskBarIcon;
    ICONINFO    m_TaskBarIconInfo;
    CBitmap     m_bmpIcon;

public:
    virtual ~CWinproc();

protected:
    afx_msg void OnClose();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    DECLARE_MESSAGE_MAP()
};
