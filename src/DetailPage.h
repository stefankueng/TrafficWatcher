#pragma once



#define IDT_DETAIL 105

/**
 * CDetailPage.
 * a CPropertyPage which holds detailed information about the ip traffic.
 */
class CDetailPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDetailPage)

public:
	CDetailPage();
	~CDetailPage();
	enum { IDD = IDD_DETAILPAGE };

	CDataBar	m_ulCtrl[LASTSERVICE];
	CDataBar	m_dlCtrl[LASTSERVICE];
	CString		m_dlData[LASTSERVICE];
	CString		m_ulData[LASTSERVICE];
	CString		m_dlSpeed[LASTSERVICE];
	CString		m_ulSpeed[LASTSERVICE];
	CString		m_totalData[LASTSERVICE];

	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
protected:
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
private:
	DWORD64 m_lwReceived[LASTSERVICE];
	DWORD64 m_lwSent[LASTSERVICE];
	DWORD64 m_lwReceivedOld[LASTSERVICE];
	DWORD64 m_lwSentOld[LASTSERVICE];

	DWORD64	dwTime;
	DWORD64	dwElapsed;
	DWORD64	dwOldTime;

};

