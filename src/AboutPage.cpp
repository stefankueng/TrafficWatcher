// AboutPage.cpp : implementation file
//

#include "stdafx.h"
#include "trafficwatch.h"
#include "AboutPage.h"


// CAboutPage dialog

IMPLEMENT_DYNAMIC(CAboutPage, CPropertyPage)
CAboutPage::CAboutPage()
	: CPropertyPage(CAboutPage::IDD)
{
}

CAboutPage::~CAboutPage()
{
}

void CAboutPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SOURCECODELINK, m_sourceLink);
	DDX_Control(pDX, IDC_WEBLINK, m_webLink);
}


BEGIN_MESSAGE_MAP(CAboutPage, CPropertyPage)
END_MESSAGE_MAP()



BOOL CAboutPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_webLink.SetURL(_T("http://tools.tortoisesvn.net"));
	m_sourceLink.SetURL(_T("http://code.google.com/p/trafficwatcher/source/browse"));

	return TRUE;  
}
