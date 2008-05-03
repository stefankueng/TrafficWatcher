// OptionsPage.cpp : implementation file
//

#include "stdafx.h"
#include "trafficwatch.h"
#include "OptionsPage.h"
#include "util.h"
#include "Registry.h"

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
	CRegString regRun = CRegString(_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run\\TrafficWatch"));
	m_autostart = !CString(regRun).IsEmpty();

	m_dlSpeed = GetRegValue("dlSpeed");
	m_ulSpeed = GetRegValue("ulSpeed");
	m_dlSpeedLAN = GetRegValue("dlSpeedlan");
	m_ulSpeedLAN = GetRegValue("ulSpeedlan");

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

	CRegString regRun = CRegString(_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run\\TrafficWatch"));
	if (m_autostart)
	{
		TCHAR szFilePath[512];
		GetModuleFileName( AfxGetInstanceHandle( ), szFilePath, sizeof( szFilePath ) );

		regRun = szFilePath;


		CRegDWORD regService = CRegDWORD(_T("SYSTEM\\CurrentControlSet\\Services\\NPF\\Start"), 0, 0, HKEY_LOCAL_MACHINE);
		if (DWORD(regService) == 3)
		{
			OSVERSIONINFO osinfo = {0};
			osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx(&osinfo);
			if (osinfo.dwMajorVersion >= 6)
			{
				// if Vista
				SHELLEXECUTEINFO TempInfo = {0};
				TempInfo.cbSize = sizeof(SHELLEXECUTEINFO);
				TempInfo.fMask = 0;
				TempInfo.hwnd = NULL;
				TempInfo.lpVerb = _T("runas");
				TempInfo.lpFile = szFilePath;
				TempInfo.lpParameters = _T("/wpcapautostart");
				TempInfo.lpDirectory = NULL;
				TempInfo.nShow = SW_NORMAL;
				::ShellExecuteEx(&TempInfo);
			}
		}
	}
	else
	{
		regRun.removeKey();
	}

	HKEY hKey;
	LONG lnRes;
	CString key;
	key = "SOFTWARE\\";
	key += LPCTSTR( M_APPNAME );
	DWORD value, valuesize;
	valuesize = sizeof(value);
	lnRes = RegCreateKeyEx(HKEY_CURRENT_USER, key, 0L, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &value);
	if (lnRes == ERROR_SUCCESS)
	{
		value = m_dlSpeed;
		valuesize = sizeof(valuesize);
		RegSetValueEx(hKey, "dlSpeed", 0, REG_DWORD, (unsigned char *)&value, valuesize);
		value = m_ulSpeed;
		valuesize = sizeof(valuesize);
		RegSetValueEx(hKey, "ulSpeed", 0, REG_DWORD, (unsigned char *)&value, valuesize);

		value = m_dlSpeedLAN;
		valuesize = sizeof(valuesize);
		RegSetValueEx(hKey, "dlSpeedlan", 0, REG_DWORD, (unsigned char *)&value, valuesize);
		value = m_ulSpeedLAN;
		valuesize = sizeof(valuesize);
		RegSetValueEx(hKey, "ulSpeedlan", 0, REG_DWORD, (unsigned char *)&value, valuesize);

		value = m_cAdapter.GetCurSel();
		valuesize = sizeof(valuesize);
		RegSetValueEx(hKey, "adapter", 0, REG_DWORD, (unsigned char *)&value, valuesize);
		RegCloseKey(hKey);
	}
	if (m_pTheApp->m_wnd.m_ipStat.GetActiveAdapterNumber() != m_cAdapter.GetCurSel())
	{
		m_pTheApp->m_wnd.m_ipStat.Stop((WORD)500);
		m_pTheApp->m_wnd.m_ipStat.Close();
		int cursel = m_cAdapter.GetCurSel();
		if ((cursel!=CB_ERR)&&(m_pTheApp->m_wnd.m_ipStat.init(cursel) == FALSE))
		{
			AfxMessageBox("could not open Adapter!");
		}
	}

	CUtil::SetBits(m_showbits);
	CUtil::SetIEC(m_showiec);

	return CPropertyPage::OnKillActive();
}


DWORD	COptionsPage::GetRegValue(CString entry)
{
	HKEY hKey;
	LONG lnRes;
	CString key;
	key = "SOFTWARE\\";
	key += LPCTSTR( M_APPNAME );
	lnRes = RegOpenKeyEx(HKEY_CURRENT_USER, key, 0, KEY_READ, &hKey);
	if (lnRes == ERROR_SUCCESS)
	{
		DWORD value, valuesize;
		ULONG type;
		valuesize = sizeof(value);
		if (RegQueryValueEx(hKey, entry, 0, &type, (unsigned char *)&value, &valuesize) == ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return value;
		}
	}
	RegCloseKey(hKey);
	return 0;
}

DWORD	COptionsPage::GetUploadSpeed()
{
	DWORD temp;
	temp = GetRegValue("ulSpeed");
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

DWORD	COptionsPage::GetDownloadSpeed()
{
	DWORD temp;
	temp = GetRegValue("dlSpeed");
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

DWORD	COptionsPage::GetUploadSpeedLAN()
{
	DWORD temp;
	temp = GetRegValue("ulSpeedlan");
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

DWORD	COptionsPage::GetDownloadSpeedLAN()
{
	DWORD temp;
	temp = GetRegValue("dlSpeedlan");
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
