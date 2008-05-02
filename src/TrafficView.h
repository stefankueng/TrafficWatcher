#pragma once

#include "databar.h"
#include "util.h"

/////////////////////////////////////////////////////////////////////////////
// CTrafficView dialog
class CTrafficView : public CDialog
{
// Construction
public:
	CTrafficView(CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_VIEW };
	CDataBar	m_ulbar;
	CDataBar	m_dlbar;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();

public://members
	int m_nYOffset;
	int m_nXOffset;

// Implementation
protected:

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnWindowPosChanging( WINDOWPOS* lpwndpos );
	afx_msg void OnDestroy();
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

