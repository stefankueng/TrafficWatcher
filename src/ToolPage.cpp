// ToolPage.cpp : implementation file
//

#include "stdafx.h"
#include "trafficwatch.h"
#include "ToolPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolPage property page

IMPLEMENT_DYNCREATE(CToolPage, CPropertyPage)

CToolPage::CToolPage() : CPropertyPage(CToolPage::IDD)
{
	//{{AFX_DATA_INIT(CToolPage)
	m_AdrEdit = _T("");
	//}}AFX_DATA_INIT
}

CToolPage::~CToolPage()
{
}

void CToolPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolPage)
	DDX_Control(pDX, IDC_PINGBUTTON, m_pingbutton);
	DDX_Control(pDX, IDC_LIST, m_cList);
	DDX_Text(pDX, IDC_IADR_EDIT, m_AdrEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolPage, CPropertyPage)
	//{{AFX_MSG_MAP(CToolPage)
	ON_BN_CLICKED(IDC_GETTCP, OnTCPbutton)
	ON_BN_CLICKED(IDC_GETUDP, OnUDPbutton)
	ON_BN_CLICKED(IDC_WHOISBUTTON, OnWhoisbutton)
	ON_BN_CLICKED(IDC_TRACEBUTTON, OnTracebutton)
	ON_BN_CLICKED(IDC_PINGBUTTON, OnPingbutton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolPage message handlers

BOOL CToolPage::LoadExIpHelperProcedures(void)
{
	HMODULE	hModule;

	hModule = LoadLibrary(_T("iphlpapi.dll"));
	if (hModule == NULL)
		return FALSE;

	// XP and later
	lpfnAllocateAndGetTcpExTableFromStack = (PROCALLOCATEANDGETTCPEXTABLEFROMSTACK)GetProcAddress(hModule,"AllocateAndGetTcpExTableFromStack");
	if (lpfnAllocateAndGetTcpExTableFromStack == NULL)
		return FALSE;

	// XP and later
	lpfnAllocateAndGetUdpExTableFromStack = (PROCALLOCATEANDGETUDPEXTABLEFROMSTACK)GetProcAddress(hModule,"AllocateAndGetUdpExTableFromStack");
	if (lpfnAllocateAndGetUdpExTableFromStack == NULL)
		return FALSE;

	return TRUE;
}

CString CToolPage::GetProcessNameByID(DWORD pid)
{
	HANDLE hProc;
	HMODULE hModules;
	DWORD dwModSize = 256 * sizeof( DWORD );

	hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid); 
	if (hProc == NULL)
	{
		CString temp;
		temp.Format("pid = %d",pid);
		return temp;
	}
	else
	{
		if (EnumProcessModules(hProc, &hModules, sizeof(hModules), &dwModSize))
		{
			char procName[1024];

			GetModuleFileNameEx(hProc, hModules, procName, 1024);
			return CString(procName);
		}
		else
		{
			CString temp;
			temp.Format("pid = %d",pid);
			return temp;
		}
	}
	CloseHandle(hProc);
}

void CToolPage::OnTCPbutton() 
{
	// TODO: Add your control notification handler code here
	if (LoadExIpHelperProcedures())
	{
		m_cList.DeleteAllItems();
		int c = ((CHeaderCtrl*)(m_cList.GetDlgItem(0)))->GetItemCount()-1;
		while (c>=0)
			m_cList.DeleteColumn(c--);
		m_cList.InsertColumn(0,	"local address");
		m_cList.InsertColumn(1, "local port");
		m_cList.InsertColumn(2, "remote address");
		m_cList.InsertColumn(3, "remote port");
		m_cList.InsertColumn(4, "state");
		m_cList.InsertColumn(5, "process");

		PMIB_TCPTABLE_EX pTcpTable = NULL;
		DWORD dwSize;
		dwSize = 0;
		//first just get the required space
		lpfnAllocateAndGetTcpExTableFromStack(&pTcpTable, TRUE, GetProcessHeap(), 0, 2);
		for (UINT i=0; i<pTcpTable->dwNumEntries; i++)
		{
			CString temp;
			DWORD dwIP = pTcpTable->table[i].dwLocalAddr;
			temp.Format("%d.%d.%d.%d", dwIP&0xFF, (dwIP>>8)&0xFF, (dwIP>>16)&0xFF, (dwIP >> 24)&0xFF);
			m_cList.InsertItem(i,temp);
			temp.Format("%d",htons((USHORT)pTcpTable->table[i].dwLocalPort));
			m_cList.SetItemText(i,1,temp);
			dwIP = pTcpTable->table[i].dwRemoteAddr;
			temp.Format("%d.%d.%d.%d", dwIP&0xFF, (dwIP>>8)&0xFF, (dwIP>>16)&0xFF, (dwIP >> 24)&0xFF);
			m_cList.SetItemText(i,2,temp);
			temp.Format("%d",htons((USHORT)pTcpTable->table[i].dwRemotePort));
			m_cList.SetItemText(i,3,temp);
			
			temp = GetProcessNameByID(pTcpTable->table[i].dwProcessId);
			m_cList.SetItemText(i,5,temp);

			switch (pTcpTable->table[i].dwState)
			{
				case MIB_TCP_STATE_CLOSED:
					temp = "closed";
					break;
				case MIB_TCP_STATE_LISTEN:
					temp = "listening";
					break;
				case MIB_TCP_STATE_SYN_SENT:
					temp = "syn sent";
					break;
				case MIB_TCP_STATE_SYN_RCVD:
					temp = "syn received";
					break;
				case MIB_TCP_STATE_ESTAB:
					temp = "established";
					break;
				case MIB_TCP_STATE_FIN_WAIT1:
					temp = "wait for finish 1";
					break;
				case MIB_TCP_STATE_FIN_WAIT2:
					temp = "wait for finish 2";
					break;
				case MIB_TCP_STATE_CLOSE_WAIT:
					temp = "wait for close";
					break;
				case MIB_TCP_STATE_CLOSING:
					temp = "closing";
					break;
				case MIB_TCP_STATE_LAST_ACK:
					temp = "last acknowledge";
					break;
				case MIB_TCP_STATE_TIME_WAIT:
					temp = "time wait";
					break;
				case MIB_TCP_STATE_DELETE_TCB:
					temp = "deleting";
					break;
				default:
					temp = "unknown";
					break;
			}
			m_cList.SetItemText(i,4,temp);
			
		}
		AutoSizeColumns();
		if (pTcpTable)
			HeapFree(GetProcessHeap(),0,pTcpTable);
	}
	else
	{
		m_cList.DeleteAllItems();
		int c = ((CHeaderCtrl*)(m_cList.GetDlgItem(0)))->GetItemCount()-1;
		while (c>=0)
			m_cList.DeleteColumn(c--);
		m_cList.InsertColumn(0,	"local address");
		m_cList.InsertColumn(1, "local port");
		m_cList.InsertColumn(2, "remote address");
		m_cList.InsertColumn(3, "remote port");
		m_cList.InsertColumn(4, "state");

		PMIB_TCPTABLE pTcpTable = NULL;
		DWORD dwSize;
		dwSize = 0;
		//first just get the required space
		GetTcpTable(pTcpTable, &dwSize, 2);
		pTcpTable = (PMIB_TCPTABLE)new UCHAR[dwSize];
		GetTcpTable(pTcpTable, &dwSize, 2);
		for (UINT i=0; i<pTcpTable->dwNumEntries; i++)
		{
			CString temp;
			DWORD dwIP = pTcpTable->table[i].dwLocalAddr;
			temp.Format("%d.%d.%d.%d", dwIP&0xFF, (dwIP>>8)&0xFF, (dwIP>>16)&0xFF, (dwIP >> 24)&0xFF);
			m_cList.InsertItem(i,temp);
			temp.Format("%d",htons((USHORT)pTcpTable->table[i].dwLocalPort));
			m_cList.SetItemText(i,1,temp);
			dwIP = pTcpTable->table[i].dwRemoteAddr;
			temp.Format("%d.%d.%d.%d", dwIP&0xFF, (dwIP>>8)&0xFF, (dwIP>>16)&0xFF, (dwIP >> 24)&0xFF);
			m_cList.SetItemText(i,2,temp);
			temp.Format("%d",htons((USHORT)pTcpTable->table[i].dwRemotePort));
			m_cList.SetItemText(i,3,temp);
			switch (pTcpTable->table[i].dwState)
			{
				case MIB_TCP_STATE_CLOSED:
					temp = "closed";
					break;
				case MIB_TCP_STATE_LISTEN:
					temp = "listening";
					break;
				case MIB_TCP_STATE_SYN_SENT:
					temp = "syn sent";
					break;
				case MIB_TCP_STATE_SYN_RCVD:
					temp = "syn received";
					break;
				case MIB_TCP_STATE_ESTAB:
					temp = "established";
					break;
				case MIB_TCP_STATE_FIN_WAIT1:
					temp = "wait for finish 1";
					break;
				case MIB_TCP_STATE_FIN_WAIT2:
					temp = "wait for finish 2";
					break;
				case MIB_TCP_STATE_CLOSE_WAIT:
					temp = "wait for close";
					break;
				case MIB_TCP_STATE_CLOSING:
					temp = "closing";
					break;
				case MIB_TCP_STATE_LAST_ACK:
					temp = "last acknowledge";
					break;
				case MIB_TCP_STATE_TIME_WAIT:
					temp = "time wait";
					break;
				case MIB_TCP_STATE_DELETE_TCB:
					temp = "deleting";
					break;
				default:
					temp = "unknown";
					break;
			}
			m_cList.SetItemText(i,4,temp);
			
		}
		AutoSizeColumns();
	}
}

void CToolPage::OnUDPbutton() 
{
	// TODO: Add your control notification handler code here
	if (LoadExIpHelperProcedures())
	{
		m_cList.DeleteAllItems();
		int c = ((CHeaderCtrl*)(m_cList.GetDlgItem(0)))->GetItemCount()-1;
		while (c>=0)
			m_cList.DeleteColumn(c--);
		m_cList.InsertColumn(0,	"local address");
		m_cList.InsertColumn(1, "local port");
		m_cList.InsertColumn(2, "process");

		PMIB_UDPTABLE_EX pUdpTable = NULL;
		DWORD dwSize;
		dwSize = 0;
		//first just get the required space
		lpfnAllocateAndGetUdpExTableFromStack(&pUdpTable,TRUE,GetProcessHeap(),0,2);
		for (UINT i=0; i<pUdpTable->dwNumEntries; i++)
		{
			CString temp;
			DWORD dwIP = pUdpTable->table[i].dwLocalAddr;
			temp.Format("%d.%d.%d.%d", dwIP&0xFF, (dwIP>>8)&0xFF, (dwIP>>16)&0xFF, (dwIP >> 24)&0xFF);
			m_cList.InsertItem(i,temp);
			temp.Format("%d",htons((USHORT)pUdpTable->table[i].dwLocalPort));
			m_cList.SetItemText(i,1,temp);
			temp = GetProcessNameByID(pUdpTable->table[i].dwProcessId);
			m_cList.SetItemText(i,2,temp);
		}
		AutoSizeColumns();
		if (pUdpTable)
			HeapFree(GetProcessHeap(),0,pUdpTable);
	}
	else
	{
		m_cList.DeleteAllItems();
		int c = ((CHeaderCtrl*)(m_cList.GetDlgItem(0)))->GetItemCount()-1;
		while (c>=0)
			m_cList.DeleteColumn(c--);
		m_cList.InsertColumn(0,	"local address");
		m_cList.InsertColumn(1, "local port");

		PMIB_UDPTABLE pUdpTable = NULL;
		DWORD dwSize;
		dwSize = 0;
		//first just get the required space
		GetUdpTable(pUdpTable, &dwSize, 2);
		pUdpTable = (PMIB_UDPTABLE)new UCHAR[dwSize];
		GetUdpTable(pUdpTable, &dwSize, 2);
		for (UINT i=0; i<pUdpTable->dwNumEntries; i++)
		{
			CString temp;
			DWORD dwIP = pUdpTable->table[i].dwLocalAddr;
			temp.Format("%d.%d.%d.%d", dwIP&0xFF, (dwIP>>8)&0xFF, (dwIP>>16)&0xFF, (dwIP >> 24)&0xFF);
			m_cList.InsertItem(i,temp);
			temp.Format("%d",htons((USHORT)pUdpTable->table[i].dwLocalPort));
			m_cList.SetItemText(i,1,temp);
		}
		AutoSizeColumns();
	}
}

void CToolPage::AutoSizeColumns()
{
	m_cList.SetRedraw(false);
	int mincol = 0;
	int maxcol = ((CHeaderCtrl*)(m_cList.GetDlgItem(0)))->GetItemCount()-1;
	int col;
	for (col = mincol; col <= maxcol; col++)
	{
		m_cList.SetColumnWidth(col,LVSCW_AUTOSIZE);
		int wc1 = m_cList.GetColumnWidth(col);
		m_cList.SetColumnWidth(col,LVSCW_AUTOSIZE_USEHEADER);
		int wc2 = m_cList.GetColumnWidth(col);
		int wc = max(wc1,wc2);
		m_cList.SetColumnWidth(col,wc);
	}
	//m_cList.SetRedraw(true);
	//m_cList.Invalidate(TRUE);
	m_cList.SetRedraw(true);
	if (IsWindowVisible())
	{
		Invalidate(TRUE);
	}
}


void CToolPage::OnWhoisbutton() 
{
	UpdateData(TRUE);
	if (m_command != WHOIS_TOOL)
	{
		SetDlgItemText(IDC_WHOISBUTTON, "Stop");
		GetDlgItem(IDC_PINGBUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_TRACEBUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_GETTCP)->EnableWindow(FALSE);
		GetDlgItem(IDC_GETUDP)->EnableWindow(FALSE);

		UpdateData(FALSE);


		m_command = WHOIS_TOOL;
		Start();
	}
	else
	{
		Stop((WORD)1000);
		SetDlgItemText(IDC_WHOISBUTTON, "TraceRoute");
		GetDlgItem(IDC_PINGBUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_TRACEBUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_GETTCP)->EnableWindow(TRUE);
		GetDlgItem(IDC_GETUDP)->EnableWindow(TRUE);

		m_command = 0;
		UpdateData(FALSE);
	}
}

void CToolPage::OnTracebutton() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_command != TRACEROUTE_TOOL)
	{
		SetDlgItemText(IDC_TRACEBUTTON, "Stop");
		GetDlgItem(IDC_PINGBUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_WHOISBUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_GETTCP)->EnableWindow(FALSE);
		GetDlgItem(IDC_GETUDP)->EnableWindow(FALSE);

		UpdateData(FALSE);


		m_command = TRACEROUTE_TOOL;
		m_pTheApp->m_wnd.m_pWorldview->ShowWindow(SW_SHOW);
		Start();
	}
	else
	{
		Stop((WORD)5000);
		SetDlgItemText(IDC_TRACEBUTTON, "TraceRoute");
		GetDlgItem(IDC_PINGBUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_WHOISBUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_GETTCP)->EnableWindow(TRUE);
		GetDlgItem(IDC_GETUDP)->EnableWindow(TRUE);
		m_command = 0;
		UpdateData(FALSE);
	}
}

void CToolPage::OnPingbutton() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_command != PING_TOOL)
	{
		SetDlgItemText(IDC_PINGBUTTON, "Stop");
		GetDlgItem(IDC_TRACEBUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_WHOISBUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_GETTCP)->EnableWindow(FALSE);
		GetDlgItem(IDC_GETUDP)->EnableWindow(FALSE);

		UpdateData(FALSE);


		m_command = PING_TOOL;
		Start();
	}
	else
	{
		Stop((WORD)1000);
		SetDlgItemText(IDC_PINGBUTTON, "Ping");
		GetDlgItem(IDC_TRACEBUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_WHOISBUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_GETTCP)->EnableWindow(TRUE);
		GetDlgItem(IDC_GETUDP)->EnableWindow(TRUE);
		m_command = 0;
		UpdateData(FALSE);
	}
}


DWORD CToolPage::ThreadMethod()
{
	m_cList.DeleteAllItems();
	int c = ((CHeaderCtrl*)(m_cList.GetDlgItem(0)))->GetItemCount()-1;
	while (c>=0)
		m_cList.DeleteColumn(c--);

	if (m_command == PING_TOOL)
	{
		m_cList.InsertColumn(0,	"host");
		m_cList.InsertColumn(1, "RTT");

		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 0), &wsa) != 0)
		{
			_tprintf(_T("Failed to initialize Winsock 2 Stack\n"));
			return -1;
		}
		CPingReply reply;
		CString temp;
		for (int i=0; (i<5)&&(m_runthread); i++)
		{
			if (CPing::Ping(m_AdrEdit, reply)!=-1)
			{
				DWORD dwIP = reply.Address.S_un.S_addr;
				temp.Format("%d.%d.%d.%d", dwIP&0xFF, (dwIP>>8)&0xFF, (dwIP>>16)&0xFF, (dwIP >> 24)&0xFF);
				m_cList.InsertItem(i, temp);
				temp.Format("%d ms", reply.avgRTT);
				m_cList.SetItemText(i,1,temp);
			}
			else
			{
				m_cList.InsertItem(i, "error");
			}
			AutoSizeColumns();
			Sleep(1000);
		}
		WSACleanup();
		SetDlgItemText(IDC_PINGBUTTON, "Ping");
		GetDlgItem(IDC_TRACEBUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_WHOISBUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_GETTCP)->EnableWindow(TRUE);
		GetDlgItem(IDC_GETUDP)->EnableWindow(TRUE);
		m_command = 0;
	}

	if (m_command == TRACEROUTE_TOOL)
	{
		m_cList.InsertColumn(0, "#");
		m_cList.InsertColumn(1, "host");
		m_cList.InsertColumn(2, "hostname");
		m_cList.InsertColumn(3, "avgRTT");
		m_cList.InsertColumn(4, "maxRTT");
		m_cList.InsertColumn(5, "minRTT");
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2,0), &wsa) != 0)
		{
			TRACE(_T("Failed to initialize Winsock 2 Stack\n"));
			return -1;
		}
		CPingReply reply;
		reply.nError = -1;
		CString temp;
		CNetGeoClient geopos;
		m_pTheApp->m_wnd.m_pWorldview->ClearCoordinates();

		for (int i=1; (i<50) && (reply.nError!=0) && (m_runthread); i++)
		{
			CPing::Ping(m_AdrEdit, reply, 5, i);
			DWORD dwIP = reply.Address.S_un.S_addr;
			temp.Format("%d",i);
			m_cList.InsertItem(i-1, temp);
			temp.Format("%d.%d.%d.%d", dwIP&0xFF, (dwIP>>8)&0xFF, (dwIP>>16)&0xFF, (dwIP >> 24)&0xFF);
			m_cList.SetItemText(i-1, 1, temp);

			//now ask the geopos server for the exact location of the host
			double lon=0;
			double lan=0;
			if (reply.nError == 11)
			{
				geopos.GetLatLong(temp, lon, lan);
				m_pTheApp->m_wnd.m_pWorldview->AddCoordinate(lan, lon);
			}
			hostent* hp = gethostbyaddr((const char *)&dwIP, 4, AF_INET);
			if (!hp)
				temp = "unavailable";
			else
			{
				temp = hp->h_name;
			}

			m_cList.SetItemText(i-1,2,temp);
			temp.Format("%d ms", reply.avgRTT);
			m_cList.SetItemText(i-1,3,temp);
			temp.Format("%d ms", reply.maxRTT);
			m_cList.SetItemText(i-1,4,temp);
			temp.Format("%d ms", reply.minRTT);
			m_cList.SetItemText(i-1,5,temp);
			AutoSizeColumns();
		}
		WSACleanup();
		SetDlgItemText(IDC_TRACEBUTTON, "TraceRoute");
		GetDlgItem(IDC_PINGBUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_WHOISBUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_GETTCP)->EnableWindow(TRUE);
		GetDlgItem(IDC_GETUDP)->EnableWindow(TRUE);
		m_command = 0;
	}

	if (m_command == WHOIS_TOOL)
	{
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2,0), &wsa) != 0)
		{
			TRACE(_T("Failed to initialize Winsock 2 Stack\n"));
			return -1;
		}
		m_cList.InsertColumn(0, "Answer");
		CWhoIs whois;
		whois.Init();
		CString answer = whois.GetWhoIs(m_AdrEdit);
		int nStart = 0;
		int line = 0;
		while (answer.Find('\n', nStart) != -1)
		{
			m_cList.InsertItem(line, answer.Mid(nStart, answer.Find('\n', nStart) - nStart));
			nStart = answer.Find('\n', nStart)+1;
		}
		AutoSizeColumns();

		WSACleanup();
		SetDlgItemText(IDC_WHOISBUTTON, "WhoIs");
		GetDlgItem(IDC_TRACEBUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_PINGBUTTON)->EnableWindow(TRUE);
		GetDlgItem(IDC_GETTCP)->EnableWindow(TRUE);
		GetDlgItem(IDC_GETUDP)->EnableWindow(TRUE);
		m_command = 0;
	}


	return 0;
}
