// TrafficWatcher - a network speed monitor

// Copyright (C) 2008, 2012 - Stefan Kueng

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
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
    , m_VersionString(L"")
{
}

CAboutPage::~CAboutPage()
{
}

void CAboutPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_VERSIONINFO, m_VersionString);
}


BEGIN_MESSAGE_MAP(CAboutPage, CPropertyPage)
END_MESSAGE_MAP()



BOOL CAboutPage::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    m_VersionString.Format(L"Version %d.%d.%d.%d", VER_MAJOR, VER_MINOR, VER_MICRO, VER_REVISION);
    m_webLink.ConvertStaticToHyperlink(m_hWnd, IDC_WEBLINK, L"http://tools.tortoisesvn.net");
    m_sourceLink.ConvertStaticToHyperlink(m_hWnd, IDC_SOURCECODELINK, L"http://code.google.com/p/trafficwatcher/source/browse");

    UpdateData(FALSE);

    return TRUE;
}
