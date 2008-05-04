// AboutPage.cpp : implementation file
//

#include "stdafx.h"
#include "trafficwatch.h"
#include "AboutPage.h"
#include "version.h"
// compiler error because of missing version.h file?
// run the NAnt build script to generate the version.h file from the version.in
// file. Or at least copy the file version.in to version.h


IMPLEMENT_DYNAMIC(CAboutPage, CPropertyPage)
CAboutPage::CAboutPage()
	: CPropertyPage(CAboutPage::IDD)
	, m_VersionString(_T(""))
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
	DDX_Text(pDX, IDC_VERSIONINFO, m_VersionString);
}


BEGIN_MESSAGE_MAP(CAboutPage, CPropertyPage)
END_MESSAGE_MAP()



BOOL CAboutPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_VersionString.Format(_T("Version %d.%d.%d.%d"), VER_MAJOR, VER_MINOR, VER_MICRO, VER_REVISION);
	m_webLink.SetURL(_T("http://tools.tortoisesvn.net"));
	m_sourceLink.SetURL(_T("http://code.google.com/p/trafficwatcher/source/browse"));

	UpdateData(FALSE);

	return TRUE;  
}
