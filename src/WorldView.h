#pragma once

#include "picture.h"

// CWorldView dialog

class CWorldView : public CDialog
{
	DECLARE_DYNAMIC(CWorldView)

public:
	CWorldView(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWorldView();

// Dialog Data
	enum { IDD = IDD_WORLDVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:	//members
	CPicture m_worldpic;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

private: //members
	CArray<double, double&>		m_longitude;
	CArray<double, double&>		m_lattitude;
public:
	void ClearCoordinates();
	void AddCoordinate(double lattitude, double longitude);
};
