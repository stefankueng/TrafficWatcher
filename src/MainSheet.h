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

#include "TrafficPage.h"
#include "DetailPage.h"
#include "OptionsPage.h"
#include "AboutPage.h"



/**
 * CMainSheet.
 * this class represents the propertysheet which holds all tabs.
 */
class CMainSheet : public CPropertySheet
{
    DECLARE_DYNAMIC(CMainSheet)

public:
    CMainSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    CMainSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

public:
    CTrafficPage*       m_pTrafficPage;
    CDetailPage*        m_pDetailPage;
    COptionsPage*       m_pOptionsPage;
    CAboutPage*         m_pAboutPage;


public:
    void  AddPropPages( );

public:
    virtual BOOL OnInitDialog();
    virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
protected:
    virtual void PostNcDestroy();
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
    virtual ~CMainSheet();

protected:
    afx_msg void OnDestroy();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnPaint();
    afx_msg void OnClose();
    DECLARE_MESSAGE_MAP()
private:
    void    LoadWindowPosition(CRect *rc);
    void    SaveWindowPosition(CRect *rc);
private:
    CFont   m_fontLogo;

};

