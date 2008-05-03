#pragma once


#include "trafficwatch.h"
#include "DataBar.h"
#include "iphlpapi.h"
#include "Filter.h"

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
	CFilter		m_sentFilter;
	CFilter		m_receivedFilter;
	CFilter		m_sentLANFilter;
	CFilter		m_receivedLANFilter;

	MIB_TCPSTATS	m_tcpstats;


};

