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
}


BEGIN_MESSAGE_MAP(CAboutPage, CPropertyPage)
END_MESSAGE_MAP()


