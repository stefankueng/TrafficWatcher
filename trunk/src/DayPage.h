#pragma once
// DayPage.h : header file
//

/**
 * CHistoryPage.
 * a CPropertyPage which holds information about the statistics of the past. a graph is
 * shown of each day of the last month and the last 12 months.
 */
class CHistoryPage : public CPropertyPage
{
    DECLARE_DYNCREATE(CHistoryPage)

// Construction
public:
    CHistoryPage();
    ~CHistoryPage();

// Dialog Data
    //{{AFX_DATA(CHistoryPage)
    enum { IDD = IDD_HISTORYPAGE };
    //}}AFX_DATA
    CString m_dl[LASTSERVICE];
    CString m_ul[LASTSERVICE];
    CString m_ultot;
    CString m_dltot;
    CString m_tot;
    CString m_date;


// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CHistoryPage)
    public:
    virtual BOOL OnSetActive();
    virtual BOOL OnKillActive();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    //{{AFX_MSG(CHistoryPage)
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
private:
    void    DrawDayDiagram(int day);
    void    DrawMonthDiagram(int month);
    void    FillData(int day, int month);
    int     CalcDays();
    DWORD64 GetMaxDay();
    DWORD64 GetMaxMonth();
    int     day;
    int     month;
};
