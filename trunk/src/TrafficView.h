#pragma once

#include "databar.h"
#include "util.h"

/////////////////////////////////////////////////////////////////////////////
// CTrafficView dialog
class CTrafficView : public CDialog
{
public:
	CTrafficView(CWnd* pParent = NULL);

	enum { IDD = IDD_VIEW };
	CDataBar	m_ulbar;
	CDataBar	m_dlbar;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void PostNcDestroy();

public:
	int m_nYOffset;
	int m_nXOffset;

protected:

	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnWindowPosChanging( WINDOWPOS* lpwndpos );
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
protected:
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

