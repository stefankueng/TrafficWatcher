// TrafficWatcher - a network speed monitor

// Copyright (C) 2008, 2012 - Stefan Kueng

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
 * CHistoryPage.
 * a CPropertyPage which holds information about the statistics of the past. a graph is
 * shown of each day of the last month and the last 12 months.
 */
class CHistoryPage : public CPropertyPage
{
    DECLARE_DYNCREATE(CHistoryPage)

// Construction
public:
    CHistoryPage();
    ~CHistoryPage();

// Dialog Data
    //{{AFX_DATA(CHistoryPage)
    enum { IDD = IDD_HISTORYPAGE };
    //}}AFX_DATA
    CString m_dl[LASTSERVICE];
    CString m_ul[LASTSERVICE];
    CString m_ultot;
    CString m_dltot;
    CString m_tot;
    CString m_date;


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CHistoryPage)
    public:
    virtual BOOL OnSetActive();
    virtual BOOL OnKillActive();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    //{{AFX_MSG(CHistoryPage)
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
private:
    void    DrawDayDiagram(int day);
    void    DrawMonthDiagram(int month);
    void    FillData(int day, int month);
    int     CalcDays();
    DWORD64 GetMaxDay();
    DWORD64 GetMaxMonth();
    int     day;
    int     month;
};
