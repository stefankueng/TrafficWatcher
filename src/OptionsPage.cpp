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
}


void COptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsPage)
	DDX_Control(pDX, IDC_ADAPTER, m_cAdapter);
	DDX_Check(pDX, IDC_AUTOSTART_CHECK, m_autostart);
	DDX_Radio(pDX, IDC_DL_R256, m_dlSpeed);
	DDX_Radio(pDX, IDC_UL_R256, m_ulSpeed);
	DDX_Check(pDX, IDC_SHOWBITS, m_showbits);
	DDX_Check(pDX, IDC_SHOWIEC, m_showiec);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(COptionsPage)
	ON_BN_CLICKED(IDC_AUTOSTART_CHECK, OnAutostartCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsPage message handlers


void COptionsPage::OnAutostartCheck() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
}

BOOL COptionsPage::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class

	//check if an autostart entry is in the registry
	HKEY hKey;
	DWORD type;
	LONG lnRes = RegOpenKeyEx( 
			   HKEY_CURRENT_USER,  // handle of open key
				 // The following is the address of name of subkey to open
			   "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 
			   0L,KEY_READ,
			   &hKey            // address of handle of open key 
				); 
	if (lnRes == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hKey, LPCTSTR ( M_APPNAME ), NULL, &type, NULL, 0) == ERROR_SUCCESS)
			m_autostart = TRUE;
		else
			m_autostart = FALSE;
		RegCloseKey(hKey);
	}

	m_dlSpeed = GetRegValue("dlSpeed");
	m_ulSpeed = GetRegValue("ulSpeed");

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
	// TODO: Add your specialized code here and/or call the base class
	UpdateData(TRUE);
	HKEY hKey;
	LONG lnRes;
	if (m_autostart)
	{
		CString sKeyName;
		char szFilePath[512];
		lnRes = RegOpenKeyEx( 
				   HKEY_CURRENT_USER,  // handle of open key
					 // The following is the address of name of subkey to open
				   "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 
				   0L,KEY_WRITE,
				   &hKey            // address of handle of open key 
			   ); 

		// now add program path to the RUN key
		int len = GetModuleFileName( AfxGetInstanceHandle( ), szFilePath, sizeof( szFilePath ) );

		if( ERROR_SUCCESS == lnRes )
		{
		   lnRes = RegSetValueEx(hKey,
								 LPCTSTR( M_APPNAME ),  // handle of the opened 
													   // key to set value for 
								 0,      
								 REG_SZ,    
								 (const BYTE *)szFilePath,   //value data
								 len+1 ); 
			RegCloseKey(hKey);
		}

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
		lnRes = RegOpenKeyEx( 
				   HKEY_CURRENT_USER,  // handle of open key
					 // The following is the address of name of subkey to open
				   "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 
				   0L,KEY_WRITE,
				   &hKey            // address of handle of open key 
			   ); 

		// now delete the registry key
		RegDeleteValue(hKey, LPCTSTR( M_APPNAME ));
	}


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
		case 1:
			return 32;
		case 2:
			return 64;
		case 3:
			return 96;
		case 4:
			return 128;
		case 5:
			return 256;
		case 6:
			return 640;
		case 7:
			return 1280;
		default:
			return 32;
	}
	return 32;
}

DWORD	COptionsPage::GetDownloadSpeed()
{
	DWORD temp;
	temp = GetRegValue("dlSpeed");
	switch ((int)temp)
	{
		case 1:
			return 32;
		case 2:
			return 64;
		case 3:
			return 96;
		case 4:
			return 128;
		case 5:
			return 256;
		case 6:
			return 640;
		case 7:
			return 1280;
		default:
			return 32;
	}
	return 32;
}
