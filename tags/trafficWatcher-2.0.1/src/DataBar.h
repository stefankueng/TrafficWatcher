// TrafficWatcher - a network speed monitor

// Copyright (C) 2008 - Stefan Kueng

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
#pragma once

/** 
 * CDataBar.
 * control for a single bar which can change the size animated. Additionally a maximum bar
 * can be drawn which either stays at the maximum position or stays there just for a certain
 * amount of time.
 * this control can be compared with the spectrum analyzer bars of hifi devices.
 */
class CDataBar : public CProgressCtrl
{
public:
	CDataBar();

public:

    // checks if the bar is animated or not.
    // <returns>TRUE if the bar is drawn animated</returns>
	BOOL GetAnimated();

	// sets the animated state.
	// <param name='bAnimated'>TRUE if animated, FALSE if static</param>
	void SetAnimated(BOOL bAnimated = TRUE);

	// returns the main color the bar is drawn.
	// <returns>the color the bar is drawn with</returns>
	COLORREF GetColor();

	// sets the color the bar will be drawn.
	// <param name='crColor'>the color.</param>
	void SetColor(COLORREF crColor);

	// sets the color for the info text of the bar.
	// <param name='fColor'>the color for the text</param>
	void SetFontColor(COLORREF fColor) {m_fontColor = fColor;};
	
	// returns the color of the info text of the bar.
	// <returns>the text color</returns>
	COLORREF GetFontColor() {return m_fontColor;};

	// if set to TRUE, then a small bar will be drawn at the top/right of the mainbar
	// which indicates the maximum value reached during the last seconds. The number of
	// seconds for which the maximum bar is shown can be set with SetMaximumTime().
	// see <see cref="SetMaximumTime(int time = 0)"/> for more information.
	// <param name='b'>TRUE if the maximum bar should be drawn</param>
	void SetShowMaximum(BOOL b = TRUE);

	// returns if the maximum bar is drawn or not.
	// <returns>TRUE if the maximum bar is drawn</returns>
	BOOL GetShowMaximum();

	// overridden from CProgressCtrl to start the timer needed for
	// animation and the maximum bar (when set).
	// this method calls the method SetPos() in the base class.
	int SetPos(int nPos);

	// sets the time for which a maximum bar will stay until it "falls down" back
	// to the actual position. if set to zero, then the maximum bar will never
	// fall down and always show the maximum value of all time.
	// <param name='time'>the time to keep the maximum in 1/10 seconds</param>
	void SetMaximumTime(int time = 0);

	// returns the maximum time. see SetMaximumTime() for details.
	// <returns>the time in 1/10 seconds</returns>
	int GetMaximumTime();

	// sets the text to show inside the bar. e.g. a value in percent or a more
	// detailed short description.
	// <param name='text'>the text to show</param>
	void	SetText(CString text) {m_text = text;};
	
	// returns the text which is actually shown on the bar
	// <returns>the text</returns>
	CString GetText() {return m_text;};

	virtual ~CDataBar();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP()
private:
	int m_nAnimOffset;
	int m_nPos;
	BOOL m_bAnimated;
	BOOL m_bShowMaximum;
	int m_nMaximum;
	int m_nMaximumTime;
	int m_nMaximumTimeTemp;
	void DrawVerticalBar(CDC *pDC, const CRect rect);
	void DrawHorizontalBar(CDC *pDC, const CRect rect);
	void DeletePens();
	void CreatePens();
	void DrawText(CDC *pDC, const CRect rect);

	CPen m_penColor;
	CPen m_penColorLight;
	CPen m_penColorLighter;
	CPen m_penColorDark;
	CPen m_penColorDarker;
	CPen m_penDkShadow;
	CPen m_penShadow;
	CPen m_penLiteShadow;
	void GetColors();
	CString m_text;
	COLORREF m_fontColor;
	COLORREF m_crColor;
	COLORREF m_crColorLight;
	COLORREF m_crColorLighter;
	COLORREF m_crColorLightest;
	COLORREF m_crColorDark;
	COLORREF m_crColorDarker;
	COLORREF m_crDkShadow;
	COLORREF m_crShadow;
	COLORREF m_crLiteShadow;
};

