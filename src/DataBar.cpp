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
#include "stdafx.h"
#include "DataBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_ANIMATE     100

// Function prototypes.
COLORREF LightenColor(const COLORREF crColor, BYTE byIncreaseVal);
COLORREF DarkenColor(const COLORREF crColor, BYTE byReduceVal);

COLORREF LightenColor(const COLORREF crColor, BYTE byIncreaseVal)
{
    BYTE byRed = GetRValue(crColor);
    BYTE byGreen = GetGValue(crColor);
    BYTE byBlue = GetBValue(crColor);

    if ((byRed + byIncreaseVal) <= 255)
        byRed = BYTE(byRed + byIncreaseVal);
    if ((byGreen + byIncreaseVal)   <= 255)
        byGreen = BYTE(byGreen + byIncreaseVal);
    if ((byBlue + byIncreaseVal) <= 255)
        byBlue = BYTE(byBlue + byIncreaseVal);

    return RGB(byRed, byGreen, byBlue);
}   // LightenColorref

COLORREF DarkenColor(const COLORREF crColor, BYTE byReduceVal)
{
    BYTE byRed = GetRValue(crColor);
    BYTE byGreen = GetGValue(crColor);
    BYTE byBlue = GetBValue(crColor);

    if (byRed >= byReduceVal)
        byRed = BYTE(byRed - byReduceVal);
    if (byGreen >= byReduceVal)
        byGreen = BYTE(byGreen - byReduceVal);
    if (byBlue >= byReduceVal)
        byBlue = BYTE(byBlue - byReduceVal);

    return RGB(byRed, byGreen, byBlue);
}

CDataBar::CDataBar()
{
    m_bAnimated = TRUE;
    m_nAnimOffset = 0;
    m_crColor = ::GetSysColor(COLOR_HIGHLIGHT);
    m_fontColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
    m_nPos = 0;
    m_nMaximum = 0;
    m_nMaximumTime = 0;
    m_nMaximumTimeTemp = 0;
    GetColors();
    CreatePens();
}

CDataBar::~CDataBar()
{
    DeletePens();
}


BEGIN_MESSAGE_MAP(CDataBar, CProgressCtrl)
    ON_WM_PAINT()
    ON_WM_TIMER()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CDataBar::OnPaint()
{
    CPaintDC dcPaint(this); // device context for painting
    CRect rect, rectClient;
    GetClientRect(rectClient);
    rect = rectClient;
    BOOL bVertical = GetStyle() & PBS_VERTICAL;

    // Create a memory DC for drawing.
    CDC dc;
    dc.CreateCompatibleDC(&dcPaint);
    int nSavedDC = dc.SaveDC();
    CBitmap bmp;
    bmp.CreateCompatibleBitmap(&dcPaint, rect.Width(), rect.Height());
    CBitmap *pOldBmp = dc.SelectObject(&bmp);

    CBrush br1(m_crColorLightest);
    CBrush br2(::GetSysColor(COLOR_3DFACE));
    dc.FillRect(rect, &br2);

    int nLower, nUpper;
    GetRange(nLower, nUpper);

    // Determine the size of the bar and draw it.
    if (bVertical)
    {
        rect.top = rect.bottom - int(((float)rect.Height() * float(m_nPos - nLower)) / float(nUpper - nLower));
        dc.FillRect(rect, &br1);
        DrawVerticalBar(&dc, rect);
        if (m_bShowMaximum)
        {
            rect = rectClient;
            rect.top = rect.bottom - int(((float)rect.Height() * float(m_nMaximum - nLower)) / float(nUpper - nLower)) - 2;
            rect.bottom = rect.top + 2;
            DrawVerticalBar(&dc, rect);
        }
    }
    else
    {
        rect.right = int(((float)rect.Width() * float(m_nPos - nLower)) / float(nUpper - nLower));
        dc.FillRect(rect, &br1);
        DrawHorizontalBar(&dc, rect);
        if (m_bShowMaximum)
        {
            rect = rectClient;
            rect.right = int(((float)rect.Width() * float(m_nMaximum - nLower)) / float(nUpper - nLower)) + 2;
            rect.left = rect.right - 2;
            DrawHorizontalBar(&dc, rect);
        }
    }

    DrawText(&dc, rectClient);
    dcPaint.BitBlt(rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(),
                        &dc, rectClient.left, rectClient.top, SRCCOPY);

    dc.SelectObject(pOldBmp);
    dc.RestoreDC(nSavedDC);
    dc.DeleteDC();
}

void CDataBar::DrawHorizontalBar(CDC *pDC, const CRect rect)
{
    if (!rect.Width())
        return;

    int nLeft = rect.left;
    int nTop = rect.top;
    int nBottom = rect.bottom;

    CPen *pOldPen = pDC->SelectObject(&m_penColorLight);

    int nRight = rect.right;

    pDC->MoveTo(nLeft + 2, nBottom - 4);
    pDC->LineTo(nRight - 2, nBottom - 4);
    pDC->MoveTo(nLeft + 2, nTop + 2);
    pDC->LineTo(nRight - 2, nTop + 2);
    pDC->SetPixel(nLeft + 1, nBottom - 3, m_crColorLight);
    pDC->SetPixel(nLeft + 1, nTop + 1, m_crColorLight);

    pDC->SelectObject(&m_penColorLighter);
    pDC->MoveTo(nLeft + 2, nBottom - 5);
    pDC->LineTo(nRight - 3, nBottom - 5);
    pDC->LineTo(nRight - 3, nTop + 3);
    pDC->LineTo(nLeft + 1, nTop + 3);
    pDC->SetPixel(nLeft + 1, nBottom - 4, m_crColorLighter);
    pDC->SetPixel(nLeft + 1, nTop + 2, m_crColorLighter);

    pDC->SelectObject(&m_penColor);
    pDC->MoveTo(nLeft, nBottom - 1);
    pDC->LineTo(nLeft, nTop);
    pDC->LineTo(nLeft + 2, nTop);
    pDC->SetPixel(nLeft + 1, nBottom - 2, m_crColor);
    pDC->MoveTo(nLeft + 2, nBottom - 3);
    pDC->LineTo(nRight - 2, nBottom - 3);
    pDC->MoveTo(nLeft + 2, nTop + 1);
    pDC->LineTo(nRight - 1, nTop + 1);

    pDC->SelectObject(&m_penColorDark);
    pDC->MoveTo(nLeft + 2, nBottom - 2);
    pDC->LineTo(nRight - 2, nBottom - 2);
    pDC->LineTo(nRight - 2, nTop + 1);
    pDC->MoveTo(nLeft + 2, nTop);
    pDC->LineTo(nRight, nTop);
    pDC->SetPixel(nLeft + 1, nBottom - 1, m_crColorDark);

    pDC->SelectObject(&m_penColorDarker);
    pDC->MoveTo(nLeft + 2, nBottom - 1);
    pDC->LineTo(nRight - 1, nBottom - 1);
    pDC->LineTo(nRight - 1, nTop);

    pDC->SelectObject(&m_penShadow);
    pDC->MoveTo(nRight, nTop);
    pDC->LineTo(nRight, nBottom);

    pDC->SelectObject(&m_penLiteShadow);
    pDC->MoveTo(nRight + 1, nTop);
    pDC->LineTo(nRight + 1, nBottom);

    pDC->SelectObject(pOldPen);
}

void CDataBar::DrawVerticalBar(CDC *pDC, const CRect rect)
{
    int nHeight = rect.Height();
    if (!nHeight)
        return;

    int nLeft = rect.left;
    int nTop = rect.top;
    int nRight = rect.right;
    int nBottom = rect.bottom;

    CPen *pOldPen = pDC->SelectObject(&m_penColor);

    if (nHeight > 3)
    {
        pDC->MoveTo(nLeft, nTop + 1);
        pDC->LineTo(nLeft, nTop);
        pDC->LineTo(nRight, nTop);
        pDC->MoveTo(nLeft + 1, nBottom - 2);
        pDC->LineTo(nLeft + 1, nTop + 1);
        pDC->MoveTo(nRight - 3, nBottom - 3);
        pDC->LineTo(nRight - 3, nTop + 1);
        pDC->SetPixel(nRight - 2, nTop + 1, m_crColor);

        pDC->SelectObject(&m_penColorLight);
        pDC->MoveTo(nLeft + 2, nBottom - 3);
        pDC->LineTo(nLeft + 2, nTop + 1);
        pDC->MoveTo(nRight - 4, nBottom - 3);
        pDC->LineTo(nRight - 4, nTop + 1);
        pDC->SetPixel(nLeft + 1, nTop + 1, m_crColorLight);
        pDC->SetPixel(nRight - 3, nTop + 1, m_crColorLight);

        pDC->SelectObject(&m_penColorLighter);
        pDC->MoveTo(nLeft + 3, nBottom - 3);
        pDC->LineTo(nLeft + 3, nTop + 1);
        pDC->MoveTo(nRight - 5, nBottom - 3);
        pDC->LineTo(nRight - 5, nTop + 1);
        pDC->SetPixel(nLeft + 2, nTop + 1, m_crColorLighter);
        pDC->SetPixel(nRight - 4, nTop + 1, m_crColorLighter);

        pDC->SelectObject(&m_penColorDark);
        pDC->MoveTo(nLeft, nBottom - 1);
        pDC->LineTo(nLeft, nTop + 1);
        pDC->MoveTo(nLeft + 2, nBottom - 2);
        pDC->LineTo(nRight - 2, nBottom - 2);
        pDC->LineTo(nRight - 2, nTop + 1);
        pDC->SetPixel(nRight - 1, nTop + 1, m_crColorDark);

        pDC->SelectObject(&m_penColorDarker);
        pDC->MoveTo(nLeft + 1, nBottom - 1);
        pDC->LineTo(nRight - 1, nBottom - 1);
        pDC->LineTo(nRight - 1, nTop + 1);
    }
    else
    {
        CBrush br(m_crColor);
        CBrush *pOldBrush = pDC->SelectObject(&br);
        pDC->SelectObject(&m_penColorDark);
        pDC->Rectangle(rect);
        pDC->SelectObject(pOldBrush);
    }

    pDC->SelectObject(pOldPen);
}

BOOL CDataBar::OnEraseBkgnd(CDC* /*pDC*/)
{
    return TRUE;
}

void CDataBar::GetColors()
{
    m_crColorLight = LightenColor(m_crColor, 51);
    m_crColorLighter = LightenColor(m_crColorLight, 51);
    m_crColorLightest = LightenColor(m_crColorLighter, 51);
    m_crColorDark = DarkenColor(m_crColor, 51);
    m_crColorDarker = DarkenColor(m_crColorDark, 51);
    m_crDkShadow = ::GetSysColor(COLOR_3DDKSHADOW);
    m_crLiteShadow = ::GetSysColor(COLOR_3DSHADOW);

    // Get a color halfway between COLOR_3DDKSHADOW and COLOR_3DSHADOW
    BYTE byRed3DDkShadow = GetRValue(m_crDkShadow);
    BYTE byRed3DLiteShadow = GetRValue(m_crLiteShadow);
    BYTE byGreen3DDkShadow = GetGValue(m_crDkShadow);
    BYTE byGreen3DLiteShadow = GetGValue(m_crLiteShadow);
    BYTE byBlue3DDkShadow = GetBValue(m_crDkShadow);
    BYTE byBlue3DLiteShadow = GetBValue(m_crLiteShadow);

    m_crShadow = RGB(byRed3DLiteShadow + ((byRed3DDkShadow - byRed3DLiteShadow) >> 1),
                          byGreen3DLiteShadow + ((byGreen3DDkShadow - byGreen3DLiteShadow) >> 1),
                          byBlue3DLiteShadow + ((byBlue3DDkShadow - byBlue3DLiteShadow) >> 1));
}

void CDataBar::SetColor(COLORREF crColor)
{
    m_crColor = crColor;
    GetColors();
    CreatePens();
    RedrawWindow();
}

COLORREF CDataBar::GetColor()
{
    return m_crColor;
}

void CDataBar::SetShowMaximum(BOOL b)
{
    m_bShowMaximum = b;
}

BOOL CDataBar::GetShowMaximum()
{
    return m_bShowMaximum;
}

int CDataBar::SetPos(int nPos)
{
    if ((m_bAnimated)||((m_bShowMaximum)&&(m_nMaximumTime > 0)))
        SetTimer(IDT_ANIMATE, 100, NULL);
    return CProgressCtrl::SetPos(nPos);
}

void CDataBar::SetMaximumTime(int time)
{
    m_nMaximumTime = time;
    m_nMaximumTimeTemp = time + 1;
}
int CDataBar::GetMaximumTime()
{
    return m_nMaximumTime;
}
void CDataBar::CreatePens()
{
    DeletePens();

    m_penColorLight.CreatePen(PS_SOLID, 1, m_crColorLight);
    m_penColorLighter.CreatePen(PS_SOLID, 1, m_crColorLighter);
    m_penColor.CreatePen(PS_SOLID, 1, m_crColor);
    m_penColorDark.CreatePen(PS_SOLID, 1, m_crColorDark);
    m_penColorDarker.CreatePen(PS_SOLID, 1, m_crColorDarker);
    m_penDkShadow.CreatePen(PS_SOLID, 1, m_crDkShadow);
    m_penShadow.CreatePen(PS_SOLID, 1, m_crShadow);
    m_penLiteShadow.CreatePen(PS_SOLID, 1, m_crLiteShadow);
}

void CDataBar::DeletePens()
{
    if (m_penColorLight.m_hObject)
        m_penColorLight.DeleteObject();
    if (m_penColorLighter.m_hObject)
        m_penColorLighter.DeleteObject();
    if (m_penColor.m_hObject)
        m_penColor.DeleteObject();
    if (m_penColorDark.m_hObject)
        m_penColorDark.DeleteObject();
    if (m_penColorDarker.m_hObject)
        m_penColorDarker.DeleteObject();
    if (m_penDkShadow.m_hObject)
        m_penDkShadow.DeleteObject();
    if (m_penShadow.m_hObject)
        m_penShadow.DeleteObject();
    if (m_penLiteShadow.m_hObject)
        m_penLiteShadow.DeleteObject();
}

void CDataBar::SetAnimated(BOOL bAnimated)
{
    m_bAnimated = bAnimated;

    if (m_bAnimated)
    {
        CRect rect;
        GetClientRect(rect);
        m_nAnimOffset = 0;

        RedrawWindow();
        SetTimer(IDT_ANIMATE, 100, NULL);
    }
    else
    {
        KillTimer(IDT_ANIMATE);
        RedrawWindow();
    }
}

BOOL CDataBar::GetAnimated()
{
    return m_bAnimated;
}

void CDataBar::OnTimer(UINT_PTR nIDEvent)
{
    // Increment the Animated bar offset and redraw the window.
    if (nIDEvent == IDT_ANIMATE)
    {
        KillTimer(nIDEvent);
        if (m_bAnimated)
        {
            if ( m_nPos > GetPos() )
            {
                m_nPos -= max(((m_nPos - GetPos())/5),1);
                if ( m_nPos < GetPos() )
                    m_nPos = GetPos();

            }
            else if ( m_nPos < GetPos() )
            {
                m_nPos += max(((GetPos() - m_nPos)/5),1);
                if ( m_nPos > GetPos() )
                    m_nPos = GetPos();

            }
        }
        if (m_bShowMaximum)
        {
            if (m_nPos > m_nMaximum)
            {
                m_nMaximum = m_nPos;
                m_nMaximumTimeTemp = m_nMaximumTime + 1;
            }
            if (m_nMaximumTimeTemp > 1)
            {
                m_nMaximumTimeTemp--;
            }
            else
            {
                if (m_nMaximumTime != 0)
                    m_nMaximum = m_nPos;
                m_nMaximumTimeTemp = m_nMaximumTime + 1;
            }
        }
        SetTimer(IDT_ANIMATE, 100, NULL);
        RedrawWindow();
    }

}

void CDataBar::DrawText(CDC *pDC, const CRect rect)
{
    if (m_text.IsEmpty())
        return;

    BOOL bVertical = GetStyle() & PBS_VERTICAL;

    CFont m_fontText;
    int x,y;


    if (bVertical)
    {
        m_fontText.CreateFont(rect.Width(), 0, 900, 0, FW_DONTCARE, false, false,0,0,0,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH, L"Courier New");
        x = rect.left;
        y = (rect.Height()-(pDC->GetTextExtent(m_text)).cy)/2 + (pDC->GetTextExtent(m_text)).cy;
    }
    else
    {
        m_fontText.CreateFont(rect.Width(), 0, 0, 0, FW_DONTCARE, false, false,0,0,0,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH, L"Courier New");
        x = (rect.Width()-(pDC->GetTextExtent(m_text)).cx)/2 + (pDC->GetTextExtent(m_text)).cx;
        y = rect.top;
    }


    CFont * OldFont = pDC->SelectObject(&m_fontText);


    pDC->SetBkMode(TRANSPARENT);
    pDC->SetTextColor(m_fontColor);
    // draw text in DC
    pDC->TextOut(x,y,m_text);

    // restore old font
    pDC->SelectObject(OldFont);
}
