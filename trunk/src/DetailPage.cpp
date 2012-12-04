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
#include "stdafx.h"
#include "trafficwatch.h"
#include "DetailPage.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDetailPage property page

IMPLEMENT_DYNCREATE(CDetailPage, CPropertyPage)

CDetailPage::CDetailPage() : CPropertyPage(CDetailPage::IDD)
{
    for (int i=0; i<LASTSERVICE; i++)
    {
        m_dlData[i] = _T("");
        m_ulData[i] = _T("");
        m_dlSpeed[i] = _T("");
        m_ulSpeed[i] = _T("");
        m_totalData[i] = _T("");
    }
}

CDetailPage::~CDetailPage()
{
}

void CDetailPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS_WEB_UL, m_ulCtrl[S_WEB]);
    DDX_Control(pDX, IDC_PROGRESS_WEB_DL, m_dlCtrl[S_WEB]);
    DDX_Control(pDX, IDC_PROGRESS_UDP_UL, m_ulCtrl[S_UDP]);
    DDX_Control(pDX, IDC_PROGRESS_UDP_DL, m_dlCtrl[S_UDP]);
    DDX_Control(pDX, IDC_PROGRESS_SHARE_UL, m_ulCtrl[S_SHARE]);
    DDX_Control(pDX, IDC_PROGRESS_SHARE_DL, m_dlCtrl[S_SHARE]);
    DDX_Control(pDX, IDC_PROGRESS_OTHER_UL, m_ulCtrl[S_OTHER]);
    DDX_Control(pDX, IDC_PROGRESS_OTHER_DL, m_dlCtrl[S_OTHER]);
    DDX_Control(pDX, IDC_PROGRESS_MAIL_UL, m_ulCtrl[S_MAIL]);
    DDX_Control(pDX, IDC_PROGRESS_MAIL_DL, m_dlCtrl[S_MAIL]);
    DDX_Control(pDX, IDC_PROGRESS_NEWS_UL, m_ulCtrl[S_NEWS]);
    DDX_Control(pDX, IDC_PROGRESS_NEWS_DL, m_dlCtrl[S_NEWS]);
    DDX_Text(pDX, IDC_NEWS_DL_DATA, m_dlData[S_NEWS]);
    DDX_Text(pDX, IDC_NEWS_DL_SPEED, m_dlSpeed[S_NEWS]);
    DDX_Text(pDX, IDC_NEWS_TOTAL_DATA, m_totalData[S_NEWS]);
    DDX_Text(pDX, IDC_NEWS_UL_DATA, m_ulData[S_NEWS]);
    DDX_Text(pDX, IDC_NEWS_UL_SPEED, m_ulSpeed[S_NEWS]);
    DDX_Text(pDX, IDC_MAIL_DL_DATA, m_dlData[S_MAIL]);
    DDX_Text(pDX, IDC_MAIL_DL_SPEED, m_dlSpeed[S_MAIL]);
    DDX_Text(pDX, IDC_MAIL_TOTAL_DATA, m_totalData[S_MAIL]);
    DDX_Text(pDX, IDC_MAIL_UL_DATA, m_ulData[S_MAIL]);
    DDX_Text(pDX, IDC_MAIL_UL_SPEED, m_ulSpeed[S_MAIL]);
    DDX_Text(pDX, IDC_OTHER_DL_DATA, m_dlData[S_OTHER]);
    DDX_Text(pDX, IDC_OTHER_DL_SPEED, m_dlSpeed[S_OTHER]);
    DDX_Text(pDX, IDC_OTHER_TOTAL_DATA, m_totalData[S_OTHER]);
    DDX_Text(pDX, IDC_OTHER_UL_DATA, m_ulData[S_OTHER]);
    DDX_Text(pDX, IDC_OTHER_UL_SPEED, m_ulSpeed[S_OTHER]);
    DDX_Text(pDX, IDC_SHARE_DL_DATA, m_dlData[S_SHARE]);
    DDX_Text(pDX, IDC_SHARE_DL_SPEED, m_dlSpeed[S_SHARE]);
    DDX_Text(pDX, IDC_SHARE_TOTAL_DATA, m_totalData[S_SHARE]);
    DDX_Text(pDX, IDC_SHARE_UL_DATA, m_ulData[S_SHARE]);
    DDX_Text(pDX, IDC_SHARE_UL_SPEED, m_ulSpeed[S_SHARE]);
    DDX_Text(pDX, IDC_UDP_DL_DATA, m_dlData[S_UDP]);
    DDX_Text(pDX, IDC_UDP_DL_SPEED, m_dlSpeed[S_UDP]);
    DDX_Text(pDX, IDC_UDP_TOTAL_DATA, m_totalData[S_UDP]);
    DDX_Text(pDX, IDC_UDP_UL_DATA, m_ulData[S_UDP]);
    DDX_Text(pDX, IDC_UDP_UL_SPEED, m_ulSpeed[S_UDP]);
    DDX_Text(pDX, IDC_WEB_DL_DATA, m_dlData[S_WEB]);
    DDX_Text(pDX, IDC_WEB_DL_SPEED, m_dlSpeed[S_WEB]);
    DDX_Text(pDX, IDC_WEB_TOTAL_DATA, m_totalData[S_WEB]);
    DDX_Text(pDX, IDC_WEB_UL_DATA, m_ulData[S_WEB]);
    DDX_Text(pDX, IDC_WEB_UL_SPEED, m_ulSpeed[S_WEB]);
}


BEGIN_MESSAGE_MAP(CDetailPage, CPropertyPage)
    ON_WM_TIMER()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDetailPage message handlers

void CDetailPage::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == IDT_DETAIL)
    {
        DWORD64     lwDiff;

        KillTimer(IDT_DETAIL);

        dwTime = GetTickCount();
        dwElapsed = dwTime - dwOldTime;
        //check for wrap
        if (dwElapsed > 0x8000)
        {
            dwElapsed = dwTime + (0 - dwOldTime);
        }
        dwOldTime = dwTime;

        for (int i=0; i<LASTSERVICE; i++)
        {
            m_lwReceived[i] = m_pTheApp->m_wnd.m_ipStat.GetIndexReceived(i);
            lwDiff = m_lwReceived[i] - m_lwReceivedOld[i];
            m_dlSpeed[i] = CUtil::GetNumberString(lwDiff * 1000 / dwElapsed) + "/s";
            m_dlCtrl[i].SetPos((int)lwDiff / (int)dwElapsed);
            m_lwReceivedOld[i] = m_lwReceived[i];
            m_dlData[i] = CUtil::GetNumberString(m_lwReceived[i]);

            m_lwSent[i] = m_pTheApp->m_wnd.m_ipStat.GetIndexSent(i);
            lwDiff = m_lwSent[i] - m_lwSentOld[i];
            m_ulSpeed[i] = CUtil::GetNumberString(lwDiff * 1000 / dwElapsed) + "/s";
            m_ulCtrl[i].SetPos((int)lwDiff / (int)dwElapsed);
            m_lwSentOld[i] = m_lwSent[i];
            m_ulData[i] = CUtil::GetNumberString(m_lwSent[i]);

            m_totalData[i] = CUtil::GetNumberString(m_lwSent[i] + m_lwReceived[i]);
        }
        UpdateData(FALSE);

        SetTimer(IDT_DETAIL, 1000, NULL);

    }
}

BOOL CDetailPage::OnSetActive()
{
    int dl, ul;
    dl = COptionsPage::GetDownloadSpeed();
    ul = COptionsPage::GetUploadSpeed();
    SetTimer(IDT_DETAIL, 1000, NULL);
    for (int i=0; i<LASTSERVICE; i++)
    {
        m_dlCtrl[i].SetShowMaximum();
        m_dlCtrl[i].SetMaximumTime(50);
        m_dlCtrl[i].SetColor(COLORDOWN);
        m_dlCtrl[i].SetRange(0,(short)dl);
        m_ulCtrl[i].SetShowMaximum();
        m_ulCtrl[i].SetMaximumTime(50);
        m_ulCtrl[i].SetColor(COLORUP);
        m_ulCtrl[i].SetRange(0, (short)ul);
    }
    return CPropertyPage::OnSetActive();
}

BOOL CDetailPage::OnKillActive()
{
    KillTimer(IDT_DETAIL);
    return CPropertyPage::OnKillActive();
}
