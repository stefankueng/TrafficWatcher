// TrafficWatcher - a network speed monitor

// Copyright (C) 2008-2009 - Stefan Kueng

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

#include "databar.h"
#include "util.h"
#include "Filter.h"

#define MONITOR_CENTER   0x0001			// center rect to monitor
#define MONITOR_CLIP     0x0000			// clip rect to monitor
#define MONITOR_WORKAREA 0x0002			// use monitor work area
#define MONITOR_AREA     0x0000			// use monitor entire area

/////////////////////////////////////////////////////////////////////////////
// CTrafficView dialog
class CTrafficView : public CDialog
{
public:
	CTrafficView(CWnd* pParent = NULL);

	enum { IDD = IDD_VIEW };
	CDataBar	m_ulbar;
	CDataBar	m_dlbar;
	CDataBar	m_ulbarlan;
	CDataBar	m_dlbarlan;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void PostNcDestroy();

public:
	int m_nYOffset;
	int m_nXOffset;

protected:

	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnWindowPosChanging( WINDOWPOS* lpwndpos );
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
protected:
	CFilter		m_sentFilter;
	CFilter		m_receivedFilter;
	CFilter		m_sentLANFilter;
	CFilter		m_receivedLANFilter;

private:
	void		LoadWindowPosition(CRect *rc);
	void		SaveWindowPosition(CRect *rc);
	void		ClipOrCenterRectToMonitor(LPRECT prc, UINT flags);

};

