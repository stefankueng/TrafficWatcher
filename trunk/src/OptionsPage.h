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

	DWORD	static GetRegValue(CString entry);
	DWORD	static GetDownloadSpeed();
	DWORD	static GetUploadSpeed();

	enum { IDD = IDD_OPTIONS };
	CComboBox	m_cAdapter;
	BOOL	m_autostart;
	int		m_dlSpeed;
	int		m_ulSpeed;
	BOOL	m_showbits;
	BOOL	m_showiec;


	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
protected:

	afx_msg void OnAutostartCheck();
	DECLARE_MESSAGE_MAP()
};

