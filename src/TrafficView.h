#if !defined(AFX_TRAFFICVIEW_H__6313D26F_F36E_4667_9614_D5F3A17443BB__INCLUDED_)
#define AFX_TRAFFICVIEW_H__6313D26F_F36E_4667_9614_D5F3A17443BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrafficView.h : header file
//
#include "databar.h"
#include "util.h"

/////////////////////////////////////////////////////////////////////////////
// CTrafficView dialog
class CTrafficView : public CDialog
{
// Construction
public:
	CTrafficView(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTrafficView)
	enum { IDD = IDD_VIEW };
	CDataBar	m_ulbar;
	CDataBar	m_dlbar;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrafficView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
public://members
	int m_nYOffset;
	int m_nXOffset;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTrafficView)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnWindowPosChanging( WINDOWPOS* lpwndpos );
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected://members
	DWORD64		dwTotalRec;
	DWORD64		dwTotalSent;
	DWORD64		dwRecDiff;
	DWORD64		dwSentDiff;
	DWORD64		dwTotalRecOld;
	DWORD64		dwTotalSentOld;
	DWORD		dwTime;
	DWORD		dwElapsed;
	DWORD		dwOldTime;
private:
	void		LoadWindowPosition(CRect *rc);
	void		SaveWindowPosition(CRect *rc);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRAFFICVIEW_H__6313D26F_F36E_4667_9614_D5F3A17443BB__INCLUDED_)
