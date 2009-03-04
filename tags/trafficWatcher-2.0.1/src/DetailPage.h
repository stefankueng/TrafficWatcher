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



#define IDT_DETAIL 105

/**
 * CDetailPage.
 * a CPropertyPage which holds detailed information about the ip traffic.
 */
class CDetailPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDetailPage)

public:
	CDetailPage();
	~CDetailPage();
	enum { IDD = IDD_DETAILPAGE };

	CDataBar	m_ulCtrl[LASTSERVICE];
	CDataBar	m_dlCtrl[LASTSERVICE];
	CString		m_dlData[LASTSERVICE];
	CString		m_ulData[LASTSERVICE];
	CString		m_dlSpeed[LASTSERVICE];
	CString		m_ulSpeed[LASTSERVICE];
	CString		m_totalData[LASTSERVICE];

	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
protected:
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
private:
	DWORD64 m_lwReceived[LASTSERVICE];
	DWORD64 m_lwSent[LASTSERVICE];
	DWORD64 m_lwReceivedOld[LASTSERVICE];
	DWORD64 m_lwSentOld[LASTSERVICE];

	DWORD64	dwTime;
	DWORD64	dwElapsed;
	DWORD64	dwOldTime;

};

