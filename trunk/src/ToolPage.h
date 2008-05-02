#if !defined(AFX_TOOLPAGE_H__C3F3A059_C1BC_4817_89AA_64D4D49E096E__INCLUDED_)
#define AFX_TOOLPAGE_H__C3F3A059_C1BC_4817_89AA_64D4D49E096E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolPage.h : header file
//

#include "ping.h"
#include "whois.h"
#include "netgeoclient.h"
#include "thread.h"
#include "PSAPI.H"

/**
 * CToolPage.
 * a CPropertyPage which holds several tools like ping. traceroute and whois.
 */
class CToolPage : public CPropertyPage, CThread
{
	DECLARE_DYNCREATE(CToolPage)

// Construction
public:
	CToolPage();
	~CToolPage();

// Dialog Data
	//{{AFX_DATA(CToolPage)
	enum { IDD = IDD_TOOLS };
	CButton	m_pingbutton;
	CListCtrl	m_cList;
	CString	m_AdrEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CToolPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CToolPage)
	afx_msg void OnTCPbutton();
	afx_msg void OnUDPbutton();
	afx_msg void OnWhoisbutton();
	afx_msg void OnTracebutton();
	afx_msg void OnPingbutton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private://methods

    /*!
     * sizes the column width of the listcontrol.
     */
	void	AutoSizeColumns();

    /*!
     * the thread used to perform queries like ping. traceroute, ...
     */
	DWORD	ThreadMethod();
	BOOL	LoadExIpHelperProcedures(void);
	CString GetProcessNameByID(DWORD pid);

	PROCALLOCATEANDGETTCPEXTABLEFROMSTACK lpfnAllocateAndGetTcpExTableFromStack;
	PROCALLOCATEANDGETUDPEXTABLEFROMSTACK lpfnAllocateAndGetUdpExTableFromStack;


private:
	int		m_command;

#define		PING_TOOL			1
#define		TRACEROUTE_TOOL		2
#define		WHOIS_TOOL			3


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLPAGE_H__C3F3A059_C1BC_4817_89AA_64D4D49E096E__INCLUDED_)
