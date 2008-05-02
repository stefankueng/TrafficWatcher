#pragma once


#include	"trafficpage.h"
#include	"detailpage.h"
#include	"historypage.h"
#include	"toolpage.h"
#include	"optionspage.h"
#include	"aboutpage.h"



/**
 * CMainSheet.
 * this class represents the propertysheet which holds all tabs.
 */
class CMainSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CMainSheet)

// Construction
public:
	CMainSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CMainSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	CTrafficPage*		m_pTrafficPage;
	CDetailPage*		m_pDetailPage;
	CHistoryPage*		m_pHistoryPage;
	CToolPage*			m_pToolPage;
	COptionsPage*		m_pOptionsPage;
	CAboutPage*			m_pAboutPage;

  
// Operations
public:
void  AddPropPages( );

// Overrides
	public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	protected:
	virtual void PostNcDestroy();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

// Implementation
public:
	virtual ~CMainSheet();

protected:
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
private:
	void	LoadWindowPosition(CRect *rc);
	void	SaveWindowPosition(CRect *rc);
private:
	CFont	m_fontLogo;

};

