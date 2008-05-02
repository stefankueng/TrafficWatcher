// WorldView.cpp : implementation file
//

#include "stdafx.h"
#include "trafficwatch.h"
#include "WorldView.h"


// CWorldView dialog

IMPLEMENT_DYNAMIC(CWorldView, CDialog)
CWorldView::CWorldView(CWnd* pParent /*=NULL*/)
	: CDialog(CWorldView::IDD, pParent)
{
}

CWorldView::~CWorldView()
{
}

void CWorldView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWorldView, CDialog)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CWorldView message handlers

BOOL CWorldView::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_worldpic.Load(IDR_WORLDPIC,"JPG");

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CWorldView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	m_worldpic.UpdateSizeOnDC(&dc);

	CRect rect;
	GetClientRect(rect);
	m_worldpic.Show(&dc, rect);

	CPen pen(PS_SOLID,1,RGB(255,0,0));
	CPen *pOldPen = dc.SelectObject(&pen);

	double lo, la;
	int x,y;

	for (int i=0; i<m_longitude.GetCount(); i++)
	{
		//some cartographers weren't mathematicians :-)
		//UP is apparently negative to them
		la = -m_lattitude.GetAt(i);
		lo = m_longitude.GetAt(i);
		if ((la!=0)||(lo!=0))
		{
			la = la + 90.0;
			lo = lo + 180.0;
			x = (int)(lo * ((double)rect.Width() / 360.0));
			y = (int)(la * ((double)rect.Height() / 180.0));
			if (i==0)
				dc.MoveTo(x,y);
			else
				dc.LineTo(x,y);
			dc.Ellipse(x-3,y-3,x+3,y+3);
		}
	}
	dc.SelectObject(pOldPen);
}

void CWorldView::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	Invalidate(FALSE);
}

BOOL CWorldView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	//return CDialog::OnEraseBkgnd(pDC);
	return TRUE;
}

void CWorldView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(nFlags == MK_LBUTTON)
	{
		::ReleaseCapture();
		::SendMessage(m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CWorldView::ClearCoordinates()
{
	m_longitude.RemoveAll();
	m_lattitude.RemoveAll();
}

void CWorldView::AddCoordinate(double lattitude, double longitude)
{
	m_longitude.Add(lattitude);
	m_lattitude.Add(longitude);
	Invalidate(FALSE);
}
