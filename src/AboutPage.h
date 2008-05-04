#pragma once
#include "afxwin.h"
#include "hyperlink.h"


// CAboutPage dialog

class CAboutPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CAboutPage)

public:
	CAboutPage();
	virtual ~CAboutPage();

// Dialog Data
	enum { IDD = IDD_ABOUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	CHyperLink m_sourceLink;
	CHyperLink m_webLink;
};
