#pragma once


#include "trafficwatch.h"
#include "DataBar.h"
#include "iphlpapi.h"

/////////////////////////////////////////////////////////////////////////////
// CTrafficPage dialog

class CTrafficPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTrafficPage)

// Construction
public:
	CTrafficPage();
	~CTrafficPage();

// Dialog Data
	enum { IDD = IDD_MAINPAGE };
	CDataBar	m_ul_bar;
	CDataBar	m_dl_bar;
	CString		m_dlMaxSpeed;
	CString		m_ulMaxSpeed;
	CString		m_connections;
	CString		m_dlByteSpeed;
	CString		m_dlData;
	CString		m_gateway;
	CString		m_ip;
	CString		m_mask;
	CString		m_totMaxSpeed;
	CString		m_totByteSpeed;
	CString		m_totData;
	CString		m_ulByteSpeed;
	CString		m_ulData;
	CString		m_adaptDesc;
	CString		m_macaddress;


	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
private://members
	DWORD64	dwTotalRec;
	DWORD64	dwRecDiff;
	DWORD64	dwTotalRecOld;
	DWORD64	dwTotalSent;
	DWORD64	dwSentDiff;
	DWORD64	dwTotalSentOld;

	DWORD64	dwTime;
	DWORD64	dwElapsed;
	DWORD64	dwOldTime;

	MIB_TCPSTATS	m_tcpstats;


};

