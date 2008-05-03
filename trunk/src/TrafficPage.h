#pragma once


#include "trafficwatch.h"
#include "DataBar.h"
#include "iphlpapi.h"

/////////////////////////////////////////////////////////////////////////////
// CTrafficPage dialog

class CTrafficPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTrafficPage)

public:
	CTrafficPage();
	~CTrafficPage();

	enum { IDD = IDD_MAINPAGE };
	CDataBar	m_ul_bar;
	CDataBar	m_dl_bar;
	CDataBar	m_ul_barlan;
	CDataBar	m_dl_barlan;
	CString		m_dlLANSpeed;
	CString		m_ulLANSpeed;
	CString		m_connections;
	CString		m_dlByteSpeed;
	CString		m_gateway;
	CString		m_ip;
	CString		m_mask;
	CString		m_ulByteSpeed;
	CString		m_adaptDesc;
	CString		m_macaddress;


	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
private:
	DWORD64	dwTotalRec;
	DWORD64	dwRecDiff;
	DWORD64	dwTotalRecOld;
	DWORD64	dwTotalSent;
	DWORD64	dwSentDiff;
	DWORD64	dwTotalSentOld;

	DWORD64	dwTotalRecLAN;
	DWORD64	dwRecDiffLAN;
	DWORD64	dwTotalRecOldLAN;
	DWORD64	dwTotalSentLAN;
	DWORD64	dwSentDiffLAN;
	DWORD64	dwTotalSentOldLAN;

	DWORD64	dwTime;
	DWORD64	dwElapsed;
	DWORD64	dwOldTime;

	MIB_TCPSTATS	m_tcpstats;


};

