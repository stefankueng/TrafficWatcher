// TrafficWatcher - a network speed monitor

// Copyright (C) 2008-2009, 2012 - Stefan Kueng

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
#include "OptionsPage.h"
#include "Util.h"
#include "Registry.h"

#include <VersionHelpers.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsPage dialog

IMPLEMENT_DYNCREATE(COptionsPage, CPropertyPage)

COptionsPage::COptionsPage() : CPropertyPage(COptionsPage::IDD)
{
    m_autostart = FALSE;
    m_showbits = FALSE;
    m_showiec = FALSE;
    m_dlSpeed = 1;
    m_ulSpeed = 1;
    m_dlSpeedLAN = 1;
    m_ulSpeedLAN = 1;
}


void COptionsPage::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_ADAPTER, m_cAdapter);
    DDX_Check(pDX, IDC_AUTOSTART_CHECK, m_autostart);
    DDX_Radio(pDX, IDC_DL_R256, m_dlSpeed);
    DDX_Radio(pDX, IDC_UL_R256, m_ulSpeed);
    DDX_Radio(pDX, IDC_DLAN_1M, m_dlSpeedLAN);
    DDX_Radio(pDX, IDC_ULAN_1M, m_ulSpeedLAN);
    DDX_Check(pDX, IDC_SHOWBITS, m_showbits);
    DDX_Check(pDX, IDC_SHOWIEC, m_showiec);
}


BEGIN_MESSAGE_MAP(COptionsPage, CPropertyPage)
    ON_BN_CLICKED(IDC_AUTOSTART_CHECK, OnAutostartCheck)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsPage message handlers


void COptionsPage::OnAutostartCheck()
{
    UpdateData(TRUE);
}

BOOL COptionsPage::OnSetActive()
{
    CRegString regRun = CRegString(L"Software\\Microsoft\\Windows\\CurrentVersion\\Run\\TrafficWatch");
    m_autostart = !CString(regRun).IsEmpty();

    m_dlSpeed    = CRegDWORD(L"SOFTWARE\\" M_APPNAME L"\\dlspeed");
    m_ulSpeed    = CRegDWORD(L"SOFTWARE\\" M_APPNAME L"\\ulspeed");
    m_dlSpeedLAN = CRegDWORD(L"SOFTWARE\\" M_APPNAME L"\\dlSpeedlan");
    m_ulSpeedLAN = CRegDWORD(L"SOFTWARE\\" M_APPNAME L"\\ulSpeedlan");

    m_showbits = CUtil::IsBits();
    m_showiec = CUtil::IsIEC();

    for (int j=0; j<m_cAdapter.GetCount(); j++)
    {
        m_cAdapter.DeleteString(j);
    }

    m_cAdapter.ResetContent();
    for (int i=0; i<m_pTheApp->m_wnd.m_ipStat.GetNumberOfAdapters(); i++)
    {
        m_cAdapter.AddString(m_pTheApp->m_wnd.m_ipStat.GetDescription(i));
    }
    m_cAdapter.SelectString(-1, m_pTheApp->m_wnd.m_ipStat.GetDescription());

    UpdateData(FALSE);
    return CPropertyPage::OnSetActive();
}

BOOL COptionsPage::OnKillActive()
{
    UpdateData(TRUE);
    TRACE("m_dlSpeed = %d\n", m_dlSpeed);
    TRACE("m_ulSpeed = %d\n", m_ulSpeed);
    TRACE("m_dlSpeedLAN = %d\n", m_dlSpeedLAN);
    TRACE("m_ulSpeedLAN = %d\n", m_ulSpeedLAN);

    CRegString regRun = CRegString(L"Software\\Microsoft\\Windows\\CurrentVersion\\Run\\TrafficWatch");
    if (m_autostart)
    {
        TCHAR szFilePath[512];
        GetModuleFileName( AfxGetInstanceHandle( ), szFilePath, _countof(szFilePath) );

        if (CString(regRun).Compare(szFilePath))
        {
            regRun = szFilePath;

            CRegDWORD regService = CRegDWORD(L"SYSTEM\\CurrentControlSet\\Services\\NPF\\Start", 0, 0, HKEY_LOCAL_MACHINE);
            if (DWORD(regService) == 3)
            {
                int ret = AfxMessageBox(L"To start TrafficWatcher automatically, it is recommended\nto also start the NPF service automatically.\nDo you want to set the NPF service to start automatically?", MB_ICONQUESTION|MB_YESNOCANCEL);
                if (ret == IDYES)
                {
                    if (IsWindowsVistaOrGreater())
                    {
                        // if Vista
                        SHELLEXECUTEINFO TempInfo = {0};
                        TempInfo.cbSize = sizeof(SHELLEXECUTEINFO);
                        TempInfo.fMask = 0;
                        TempInfo.hwnd = NULL;
                        TempInfo.lpVerb = L"runas";
                        TempInfo.lpFile = szFilePath;
                        TempInfo.lpParameters = L"/wpcapautostart";
                        TempInfo.lpDirectory = NULL;
                        TempInfo.nShow = SW_NORMAL;
                        ::ShellExecuteEx(&TempInfo);
                    }
                }
                else if (ret == IDCANCEL)
                {
                    m_autostart = FALSE;
                    regRun.removeValue();
                }
            }
        }
    }
    else
    {
        regRun.removeValue();
    }

    CRegDWORD regDlspeed(L"SOFTWARE\\" M_APPNAME L"\\dlspeed");
    regDlspeed = m_dlSpeed;
    CRegDWORD regUlspeed(L"SOFTWARE\\" M_APPNAME L"\\ulspeed");
    regUlspeed = m_ulSpeed;
    CRegDWORD regDlspeedLAN(L"SOFTWARE\\" M_APPNAME L"\\dlspeedlan");
    regDlspeedLAN = m_dlSpeedLAN;
    CRegDWORD regUlspeedLAN(L"SOFTWARE\\" M_APPNAME L"\\ulspeedlan");
    regUlspeedLAN = m_ulSpeedLAN;

    int value = m_cAdapter.GetCurSel();
    if (value != CB_ERR)
    {
        CString adapterDesc = m_pTheApp->m_wnd.m_ipStat.GetDescription(value);
        CRegString regAdapter(L"SOFTWARE\\" M_APPNAME L"\\adapter");
        regAdapter = adapterDesc;
    }
    if (m_pTheApp->m_wnd.m_ipStat.GetActiveAdapterNumber() != m_cAdapter.GetCurSel())
    {
        m_pTheApp->m_wnd.m_ipStat.Stop((WORD)500);
        m_pTheApp->m_wnd.m_ipStat.Close();
        int cursel = m_cAdapter.GetCurSel();
        if ((cursel!=CB_ERR)&&(m_pTheApp->m_wnd.m_ipStat.init(cursel) == FALSE))
        {
            AfxMessageBox(L"could not open Adapter!");
        }
    }

    CUtil::SetBits(m_showbits);
    CUtil::SetIEC(m_showiec);

    return CPropertyPage::OnKillActive();
}


DWORD COptionsPage::GetUploadSpeed()
{
    DWORD temp;
    temp = CRegDWORD(L"SOFTWARE\\" M_APPNAME L"\\ulspeed");
    switch ((int)temp)
    {
    case 0:
        return 32;
    case 1:
        return 64;
    case 2:
        return 96;
    case 3:
        return 128;
    case 4:
        return 256;
    case 5:
        return 640;
    case 6:
        return 1280;
    default:
        return 32;
    }
    //return 32;
}

DWORD COptionsPage::GetDownloadSpeed()
{
    DWORD temp;
    temp = CRegDWORD(L"SOFTWARE\\" M_APPNAME L"\\dlspeed");
    switch ((int)temp)
    {
    case 0:
        return 32;
    case 1:
        return 64;
    case 2:
        return 96;
    case 3:
        return 128;
    case 4:
        return 256;
    case 5:
        return 640;
    case 6:
        return 1280;
    default:
        return 32;
    }
    //return 32;
}

DWORD COptionsPage::GetUploadSpeedLAN()
{
    DWORD temp;
    temp = CRegDWORD(L"SOFTWARE\\" M_APPNAME L"\\ulspeedlan");
    switch ((int)temp)
    {
    case 0:
        return 128;
    case 1:
        return 640;
    case 2:
        return 1280;
    case 3:
        return 6912;
    case 4:
        return 12800;
    case 5:
        return 128000;
    case 6:
        return 1280000;
    default:
        return 1280;
    }
    //return 1280;
}

DWORD COptionsPage::GetDownloadSpeedLAN()
{
    DWORD temp;
    temp = CRegDWORD(L"SOFTWARE\\" M_APPNAME L"\\dlspeedlan");
    switch ((int)temp)
    {
    case 0:
        return 128;
    case 1:
        return 640;
    case 2:
        return 1280;
    case 3:
        return 6912;
    case 4:
        return 12800;
    case 5:
        return 128000;
    case 6:
        return 1280000;
    default:
        return 1280;
    }
    //return 1280;
}
