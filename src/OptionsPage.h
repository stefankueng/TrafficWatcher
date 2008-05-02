#if !defined(AFX_OPTIONSPAGE_H__77A1BF3B_44EC_47D5_B009_10B4ED26FB64__INCLUDED_)
#define AFX_OPTIONSPAGE_H__77A1BF3B_44EC_47D5_B009_10B4ED26FB64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsPage.h : header file
//

/**
 * COptionsPage.
 * a CPropertyPage in which the user can specifiy different options for the app.
 */
class COptionsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(COptionsPage)
// Construction
public:
	COptionsPage();   // standard constructor

	DWORD	static GetRegValue(CString entry);
	// test comment of method
	DWORD	static GetDownloadSpeed();
	DWORD	static GetUploadSpeed();

// Dialog Data
	//{{AFX_DATA(COptionsPage)
	enum { IDD = IDD_OPTIONS };
	CComboBox	m_cAdapter;
	BOOL	m_autostart;
	int		m_dlSpeed;
	int		m_ulSpeed;
	BOOL	m_showbits;
	BOOL	m_showiec;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsPage)
	afx_msg void OnAutostartCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSPAGE_H__77A1BF3B_44EC_47D5_B009_10B4ED26FB64__INCLUDED_)
