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


#include "trafficwatch.h"
#include "DataBar.h"
#include <iphlpapi.h>
#include "Filter.h"

/////////////////////////////////////////////////////////////////////////////
// CTrafficPage dialog

class CTrafficPage : public CPropertyPage
{
    DECLARE_DYNCREATE(CTrafficPage)

public:
    CTrafficPage();
    ~CTrafficPage();

    enum { IDD = IDD_MAINPAGE };
    CDataBar    m_ul_bar;
    CDataBar    m_dl_bar;
    CDataBar    m_ul_barlan;
    CDataBar    m_dl_barlan;
    CString     m_dlLANSpeed;
    CString     m_ulLANSpeed;
    CString     m_connections;
    CString     m_dlByteSpeed;
    CString     m_gateway;
    CString     m_ip;
    CString     m_mask;
    CString     m_ulByteSpeed;
    CString     m_adaptDesc;
    CString     m_macaddress;


    public:
    virtual BOOL OnSetActive();
    virtual BOOL OnKillActive();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    DECLARE_MESSAGE_MAP()
private:
    CFilter     m_sentFilter;
    CFilter     m_receivedFilter;
    CFilter     m_sentLANFilter;
    CFilter     m_receivedLANFilter;

    MIB_TCPSTATS    m_tcpstats;

};
