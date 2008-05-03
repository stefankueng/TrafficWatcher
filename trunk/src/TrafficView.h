#pragma once

#include "databar.h"
#include "util.h"
#include "Filter.h"

/////////////////////////////////////////////////////////////////////////////
// CTrafficView dialog
class CTrafficView : public CDialog
{
public:
	CTrafficView(CWnd* pParent = NULL);

	enum { IDD = IDD_VIEW };
	CDataBar	m_ulbar;
	CDataBar	m_dlbar;
	CDataBar	m_ulbarlan;
	CDataBar	m_dlbarlan;

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
	CFilter		m_sentFilter;
	CFilter		m_receivedFilter;
	CFilter		m_sentLANFilter;
	CFilter		m_receivedLANFilter;

private:
	void		LoadWindowPosition(CRect *rc);
	void		SaveWindowPosition(CRect *rc);
};

