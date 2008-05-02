#if !defined(AFX_DETAILPAGE_H__F81AECD9_938E_4031_9CE5_77FAE26FA7CC__INCLUDED_)
#define AFX_DETAILPAGE_H__F81AECD9_938E_4031_9CE5_77FAE26FA7CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DetailPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDetailPage dialog



#define IDT_DETAIL 105

/**
 * CDetailPage.
 * a CPropertyPage which holds detailed information about the ip traffic.
 */
class CDetailPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDetailPage)

// Construction
public:
	CDetailPage();
	~CDetailPage();

// Dialog Data
	//{{AFX_DATA(CDetailPage)
	enum { IDD = IDD_DETAILPAGE };
	//}}AFX_DATA

	CDataBar	m_ulCtrl[LASTSERVICE];
	CDataBar	m_dlCtrl[LASTSERVICE];
	CString		m_dlData[LASTSERVICE];
	CString		m_ulData[LASTSERVICE];
	CString		m_dlSpeed[LASTSERVICE];
	CString		m_ulSpeed[LASTSERVICE];
	CString		m_totalData[LASTSERVICE];

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDetailPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDetailPage)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	DWORD64 m_lwReceived[LASTSERVICE];
	DWORD64 m_lwSent[LASTSERVICE];
	DWORD64 m_lwReceivedOld[LASTSERVICE];
	DWORD64 m_lwSentOld[LASTSERVICE];

	DWORD64	dwTime;
	DWORD64	dwElapsed;
	DWORD64	dwOldTime;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DETAILPAGE_H__F81AECD9_938E_4031_9CE5_77FAE26FA7CC__INCLUDED_)
