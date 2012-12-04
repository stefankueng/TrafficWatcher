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

/**
 * COptionsPage.
 * a CPropertyPage in which the user can specify different options for the app.
 */
class COptionsPage : public CPropertyPage
{
    DECLARE_DYNCREATE(COptionsPage)
public:
    COptionsPage();

    DWORD   static GetRegValue(CString entry);
    DWORD   static GetDownloadSpeed();
    DWORD   static GetUploadSpeed();
    DWORD   static GetDownloadSpeedLAN();
    DWORD   static GetUploadSpeedLAN();

    enum { IDD = IDD_OPTIONS };
    CComboBox   m_cAdapter;
    BOOL    m_autostart;
    int     m_dlSpeed;
    int     m_ulSpeed;
    int     m_dlSpeedLAN;
    int     m_ulSpeedLAN;
    BOOL    m_showbits;
    BOOL    m_showiec;


public:
    virtual BOOL OnSetActive();
    virtual BOOL OnKillActive();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
protected:

    afx_msg void OnAutostartCheck();
    DECLARE_MESSAGE_MAP()
};
