#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//	Inlcude used based on how code is integrated
#include "stdafx.h"
#include "WhoIs.h"



CWhoIs::CWhoIs()
{
//	Init variables
	m_iWhoIsPort = IPPORT_WHOIS;						//	defined in WINSOCK.H
	m_szWhoIsServer = "whois.internic.net";				//	Default server
	m_szWhoIsServerIP = GetIpFromHost(m_szWhoIsServer);	//	IP for default server
}


CWhoIs::~CWhoIs()
{
}

BOOL CWhoIs::Init()
{
	CString entryline;
	CStdioFile theFile;
	WhoIsServer server;
	if (theFile.Open("whoisservers.txt", CFile::modeRead))
	{
		while (theFile.ReadString(entryline))
		{
			if ((entryline.GetLength() > 2)&&(entryline.Left(2)!="//"))
			{
				server.tld = entryline.Left(entryline.Find(" "));
				server.server = entryline.Right(entryline.GetLength()-entryline.Find(" ")-1);
				m_arWhoIsServer.Add(server);
			}
		}
	}
	else
		return FALSE;

	return TRUE;
}

void CWhoIs::LocateWhoisServer(LPCSTR szAddress)
{
	CString tld;
	//first find the TLD for the requested address

	if (m_arWhoIsServer.GetSize() == 0)
	{
		return;			//the file "whoisservers.txt" was not found, using default whois server.
	}


	tld = CString(szAddress);
	tld = tld.Right(tld.GetLength()-tld.ReverseFind('.'));

	int i=0;

	while ((tld != ((WhoIsServer)m_arWhoIsServer.GetAt(i)).tld)&&((m_arWhoIsServer.GetSize()-1) > i))
		i++;

	if (i >= m_arWhoIsServer.GetSize())
	{
		TRACE(_T("no server found for this domain %s"),tld);
	}

	SetWhoIsServer(((WhoIsServer)m_arWhoIsServer.GetAt(i)).server);
}

void CWhoIs::SetWhoIsServer(LPCSTR szServerName)
{
//	Set new server and associated IP address
	m_szWhoIsServer = szServerName;
	m_szWhoIsServerIP = GetIpFromHost(szServerName);
}


CString CWhoIs::GetWhoIs(LPCSTR szAddress)
{
	char	szQuery[512];
	char	szBuffer[128];
	CString szResult = "";

//	Set query string
	strcpy(szQuery, szAddress);
	strcat(szQuery, " \r\n");

//	Create socket
	CSocket socket;
	socket.Create();

	LocateWhoisServer(szAddress);


//	Connect to server
	int iResult = socket.Connect(m_szWhoIsServerIP, IPPORT_WHOIS);

//	Quit on error
	if (iResult <= 0)
		return szResult;

//	Send whois query
	iResult = socket.Send(szQuery, strlen(szQuery));

//	Quit on error
	if (iResult <= 0)
		return szResult;

//	Get result
	szResult = "";
	while (TRUE)
	{
	//	Clear buffer before each iteration
		memset(szBuffer, 0, 128);

	//	Try to receive some data
		iResult = socket.Receive(szBuffer, 100);

	//	Quit if no more data
		if (iResult <= 0)
			break;

	//	Add this data to the result string
		szResult += szBuffer;
	}

//	Close socket
	socket.Close();

//	Return result
	return szResult;
}


CString CWhoIs::GetIpFromHost(LPCSTR szHostName)
{
	int i,j;
	CString szResult = "";

//	Convert host name to IP address
 	hostent* pHost = gethostbyname(szHostName);

	for(i = 0; pHost!= NULL && pHost->h_addr_list[i]!= NULL; i++)
 	{ 
 		for(j = 0; j < pHost->h_length; j++)
 		{
			CString addr;
 
 			if(j > 0)
 				szResult += ".";
 
 			addr.Format("%u", (unsigned int)((unsigned char*)pHost->h_addr_list[i])[j]);
			szResult += addr;
 		}
 	}

	return szResult;
}
