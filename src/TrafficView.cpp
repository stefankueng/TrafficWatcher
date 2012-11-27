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
#include "stdafx.h"
#include "trafficwatch.h"
#include "TrafficView.h"
#include "optionspage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrafficView dialog


CTrafficView::CTrafficView(CWnd* pParent /*=NULL*/)
    : CDialog(CTrafficView::IDD, pParent)
{
}


void CTrafficView::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_ULBAR, m_ulbar);
    DDX_Control(pDX, IDC_DLBAR, m_dlbar);
    DDX_Control(pDX, IDC_ULBARLAN, m_ulbarlan);
    DDX_Control(pDX, IDC_DLBARLAN, m_dlbarlan);
}


BEGIN_MESSAGE_MAP(CTrafficView, CDialog)
    ON_WM_CLOSE()
    ON_WM_TIMER()
    ON_WM_MOUSEMOVE()
    ON_WM_WINDOWPOSCHANGING()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrafficView message handlers

BOOL CTrafficView::OnInitDialog()
{
    CDialog::OnInitDialog();
    m_nYOffset = 50;
    m_nXOffset = 100;

    // we make sure the Window has no Border,Title
    ModifyStyle( WS_CAPTION | WS_BORDER,0);

    // remove the dialog from the taskbar
    ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

    SetWindowPos( &wndTopMost,0,0,0,0,SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE );

    m_dlbar.SetAnimated();
    m_dlbar.SetMaximumTime();
    m_dlbar.SetShowMaximum();
    m_dlbar.SetMaximumTime(50);
    m_dlbar.SetFontColor(RGB(255, 0, 0));
    m_dlbar.SetColor(COLORDOWN);
    m_ulbar.SetAnimated();
    m_ulbar.SetMaximumTime();
    m_ulbar.SetShowMaximum();
    m_ulbar.SetMaximumTime(50);
    m_ulbar.SetFontColor(RGB(255, 0, 0));
    m_ulbar.SetColor(COLORUP);
    m_dlbar.SetRange(0, (short)(COptionsPage::GetDownloadSpeed()));
    m_ulbar.SetRange(0, (short)(COptionsPage::GetUploadSpeed()));

    m_dlbarlan.SetAnimated();
    m_dlbarlan.SetMaximumTime();
    m_dlbarlan.SetShowMaximum();
    m_dlbarlan.SetMaximumTime(50);
    m_dlbarlan.SetFontColor(RGB(255, 0, 0));
    m_dlbarlan.SetColor(COLORDOWN);
    m_ulbarlan.SetAnimated();
    m_ulbarlan.SetMaximumTime();
    m_ulbarlan.SetShowMaximum();
    m_ulbarlan.SetMaximumTime(50);
    m_ulbarlan.SetFontColor(RGB(255, 0, 0));
    m_ulbarlan.SetColor(COLORUP);
    m_dlbarlan.SetRange(0, (short)(COptionsPage::GetDownloadSpeedLAN()));
    m_ulbarlan.SetRange(0, (short)(COptionsPage::GetUploadSpeedLAN()));


    DWORD ticks = GetTickCount();
    m_sentFilter.Filter(ticks, m_pTheApp->m_wnd.m_ipStat.GetTotalSent());
    m_receivedFilter.Filter(ticks, m_pTheApp->m_wnd.m_ipStat.GetTotalReceived());
    m_sentLANFilter.Filter(ticks, m_pTheApp->m_wnd.m_ipStat.GetTotalSentLAN());
    m_receivedLANFilter.Filter(ticks, m_pTheApp->m_wnd.m_ipStat.GetTotalReceivedLAN());

    SetTimer( IDT_TRAFFIC, 1000, NULL );

    UpdateData(FALSE);


    //position the dialog
    CRect rc;
    GetClientRect( rc );
    LoadWindowPosition( &rc );
    ClipOrCenterRectToMonitor(&rc, MONITOR_WORKAREA|MONITOR_DEFAULTTONEAREST);
    BOOL bCenter = FALSE;
    if( rc.left < 0 || ( rc.right ) > GetSystemMetrics(SM_CXSCREEN) )
        bCenter = TRUE;

    if( rc.top < 0 || ( rc.bottom ) > GetSystemMetrics(SM_CYSCREEN) )
        bCenter = TRUE;

    if( bCenter )
    {
        GetClientRect( rc );
        GetWindowRect( rc );
        rc.OffsetRect(-rc.left, -rc.top);
        MoveWindow( ((GetSystemMetrics(SM_CXSCREEN) - rc.right ) / 2 + 4) & ~7,
                     (GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2, rc.right, rc.bottom, 0);
    }
    else
    {
        SetWindowPos( NULL,rc.left,rc.top,0,0,SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOSIZE );
    }




    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CTrafficView::ClipOrCenterRectToMonitor(LPRECT prc, UINT flags)
{
    HMONITOR hMonitor;
    MONITORINFO mi;
    RECT        rc;
    int         w = prc->right  - prc->left;
    int         h = prc->bottom - prc->top;

    //
    // get the nearest monitor to the passed rect.
    //
    hMonitor = MonitorFromRect(prc, MONITOR_DEFAULTTONEAREST);

    //
    // get the work area or entire monitor rect.
    //
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(hMonitor, &mi);

    if (flags & MONITOR_WORKAREA)
        rc = mi.rcWork;
    else
        rc = mi.rcMonitor;

    //
    // center or clip the passed rect to the monitor rect
    //
    if (flags & MONITOR_CENTER)
    {
        prc->left   = rc.left + (rc.right  - rc.left - w) / 2;
        prc->top    = rc.top  + (rc.bottom - rc.top  - h) / 2;
        prc->right  = prc->left + w;
        prc->bottom = prc->top  + h;
    }
    else
    {
        prc->left   = max(rc.left, min(rc.right-w,  prc->left));
        prc->top    = max(rc.top,  min(rc.bottom-h, prc->top));
        prc->right  = prc->left + w;
        prc->bottom = prc->top  + h;
    }
}

void CTrafficView::LoadWindowPosition(CRect *rc)
{
    HKEY hKey;
    LONG lnRes;
    CString key;
    key = "SOFTWARE\\";
    key += LPCTSTR( M_APPNAME );
    lnRes = RegOpenKeyEx(HKEY_CURRENT_USER, key, 0, KEY_READ, &hKey);
    if (lnRes == ERROR_SUCCESS)
    {
        DWORD valuesize;
        ULONG type;
        valuesize = sizeof(CRect);
        RegQueryValueEx(hKey, "viewpos", 0, &type, (unsigned char *)rc, &valuesize);
    }
    RegCloseKey(hKey);
}

void CTrafficView::SaveWindowPosition(CRect *rc)
{
    HKEY hKey;
    CString key;
    key = "SOFTWARE\\";
    key += LPCTSTR( M_APPNAME );
    DWORD value, valuesize;
    valuesize = sizeof(value);
    LONG lnRes = RegCreateKeyEx(HKEY_CURRENT_USER, key, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &value);
    if (lnRes == ERROR_SUCCESS)
    {
        valuesize = sizeof(CRect);
        RegSetValueEx(hKey, "viewpos", 0, REG_BINARY, (unsigned char *)rc, valuesize);
    }
    RegCloseKey(hKey);
}

void CTrafficView::PostNcDestroy()
{
    m_pTheApp->m_pMainWnd = &m_pTheApp->m_wnd;
    CDialog::PostNcDestroy();
}

void CTrafficView::OnClose()
{
    CRect rc;
    GetWindowRect( rc );
    SaveWindowPosition( &rc );
    m_pTheApp->m_pMainWnd = &m_pTheApp->m_wnd;
    CDialog::OnClose();
}

void CTrafficView::OnTimer(UINT nIDEvent)
{
    if (nIDEvent == IDT_TRAFFIC)
    {
        KillTimer(IDT_TRAFFIC);
        DWORD dwTicks = GetTickCount();

        DWORD ulSpeed = (DWORD)m_sentFilter.Filter(dwTicks, m_pTheApp->m_wnd.m_ipStat.GetTotalSent());
        DWORD dlSpeed = (DWORD)m_receivedFilter.Filter(dwTicks, m_pTheApp->m_wnd.m_ipStat.GetTotalReceived());
        DWORD ulLANSpeed = (DWORD)m_sentLANFilter.Filter(dwTicks, m_pTheApp->m_wnd.m_ipStat.GetTotalSentLAN());
        DWORD dlLANSpeed = (DWORD)m_receivedLANFilter.Filter(dwTicks, m_pTheApp->m_wnd.m_ipStat.GetTotalReceivedLAN());


        m_dlbar.SetText(CUtil::GetNumberString(dlSpeed)+"/s");
        m_ulbar.SetText(CUtil::GetNumberString(ulSpeed)+"/s");

        m_dlbar.SetPos(dlSpeed/1000);       //range is in kbytes/s, not bytes/s!
        m_ulbar.SetPos(ulSpeed/1000);


        m_dlbarlan.SetText(CUtil::GetNumberString(dlLANSpeed)+"/s");
        m_ulbarlan.SetText(CUtil::GetNumberString(ulLANSpeed)+"/s");

        m_dlbarlan.SetPos(dlLANSpeed/1000);
        m_ulbarlan.SetPos(ulLANSpeed/1000);


        UpdateData(FALSE);
        SetTimer(IDT_TRAFFIC, 1000, NULL);
    }
}



void CTrafficView::OnMouseMove(UINT nFlags, CPoint point)
{
    if(nFlags == MK_LBUTTON)
    {
        ::ReleaseCapture();
        ::SendMessage(m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
    }

    CDialog::OnMouseMove(nFlags, point);
}

void CTrafficView::OnWindowPosChanging( WINDOWPOS* lpwndpos )
{
    CRect   wndRect;

    GetWindowRect(&wndRect);

    HMONITOR hMonitor = MonitorFromRect(&wndRect, MONITOR_DEFAULTTONEAREST);
    MONITORINFOEX mi;
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(hMonitor, &mi);

    int wndWidth = wndRect.right - wndRect.left;
    int wndHeight = wndRect.bottom - wndRect.top;

    // Snap to screen border
    // Left border
    if(lpwndpos->x >= -m_nXOffset + mi.rcWork.left && lpwndpos->x <= mi.rcWork.left + m_nXOffset) {
        lpwndpos->x = mi.rcWork.left;
    }

    // Top border
    if(lpwndpos->y >= -m_nYOffset && lpwndpos->y <= mi.rcWork.top + m_nYOffset) {
        lpwndpos->y = mi.rcWork.top;
    }

    // Right border
    if(lpwndpos->x + wndWidth <= mi.rcWork.right + m_nXOffset && lpwndpos->x + wndWidth >= mi.rcWork.right - m_nXOffset) {
        lpwndpos->x = mi.rcWork.right - wndWidth;
    }

    // Bottom border
    if( lpwndpos->y + wndHeight <= mi.rcWork.bottom + m_nYOffset && lpwndpos->y + wndHeight >= mi.rcWork.bottom - m_nYOffset) {
        lpwndpos->y = mi.rcWork.bottom - wndHeight;
    }
    CDialog::OnWindowPosChanging(lpwndpos);
}



void CTrafficView::OnDestroy()
{
    CRect rc;
    GetWindowRect( rc );
    SaveWindowPosition( &rc );
    m_pTheApp->m_pMainWnd = &m_pTheApp->m_wnd;
    CDialog::OnDestroy();
}
