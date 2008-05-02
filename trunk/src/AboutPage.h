#pragma once


//#include "texture.h"
#include "picture.h"

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

	DECLARE_MESSAGE_MAP()
public:

public:
};
