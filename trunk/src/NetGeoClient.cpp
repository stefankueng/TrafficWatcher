#include "stdafx.h"
#include "netgeoclient.h"

CNetGeoClient::CNetGeoClient(void)
{
	netGeoUrl = DEFAULT_SERVER_URL;
	userAgent = "trafficwatch/NetGeoClient/Win2k/WinXP";
}
CNetGeoClient::CNetGeoClient(CString url)
{
	netGeoUrl = url;
	userAgent = "trafficwatch/NetGeoClient/Win2k/WinXP";
}

CNetGeoClient::~CNetGeoClient(void)
{
}

// returns the dotted ip address
CString CNetGeoClient::getIPAddress(CString adr)
{
	unsigned long ipadr = inet_addr(adr);
	sockaddr_in dest;
	memset(&dest,0,sizeof(dest));
	if (ipadr == INADDR_NONE)
	{
		hostent* hp = gethostbyname(adr);
		if ((hp!=0)&&(hp->h_addrtype == AF_INET))
		{
			memcpy(&(dest.sin_addr),hp->h_addr,hp->h_length);
			dest.sin_family = hp->h_addrtype;
			DWORD dwIP = dest.sin_addr.S_un.S_addr;
			adr.Format("%d.%d.%d.%d", dwIP&0xFF, (dwIP>>8)&0xFF, (dwIP>>16)&0xFF, (dwIP >> 24)&0xFF);
		}
		else
		{
			TRACE(_T("CNetGeoClient::getIPAddress, Could not resolve the host name %s\n"), adr);
			CString temp;
			temp.Format("Could not resolve host name \"%s\" ", adr);
			AfxMessageBox(temp);
			adr = "";
		}
	}
	return adr;
}

BOOL CNetGeoClient::GetLatLong(CString machine, double& lat, double& lon)
{
	machine = getIPAddress(machine);
	if (machine.GetLength()>0)
	{
		CString urlString = "/perl/netgeo.cgi?method=getLatLong&target=" + machine;
		CInternetSession session;
		CHttpConnection* pConnection = session.GetHttpConnection(netGeoUrl);
		CHttpFile * pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, urlString);
		BOOL result = pFile->AddRequestHeaders(userAgent);
		result = pFile->SendRequest();
		CString answer;
		while (pFile->ReadString(answer))
		{
			if (answer.Find("HTTP_ERROR")>=0)
			{
				// an error occured while trying to receive data
				TRACE("NetGeoClient: HTTP_ERROR\n");
				pFile->Close();
				delete pFile;
				pConnection->Close();
				delete pConnection;
				session.Close();
				return FALSE;
			}
			if (answer.Find("NETGEO_LIMIT_EXCEEDED")>=0)
			{
				// we reached the limit of requests. no more request will be served
				// by the netgeo server for a certain amount of time (about 30-60 seconds)
				TRACE("NetGeoClient: NETGEO_LIMIT_EXCEEDED\n");
				pFile->Close();
				delete pFile;
				pConnection->Close();
				delete pConnection;
				session.Close();
				return FALSE;
			}
			int index;
			if ((index = answer.Find("LAT:"))>=0)
			{
				lat = atof(answer.Right(answer.GetLength()-index-4).Trim());
				TRACE("lattitude = %f",lat);
			}
			if ((index = answer.Find("LONG:"))>=0)
			{
				lon = atof(answer.Right(answer.GetLength()-index-5).Trim());
				TRACE("  --  longitude = %f\n",lon);
			}
		}
		pFile->Close();
		delete pFile;
		pConnection->Close();
		delete pConnection;
		session.Close();
		return TRUE;
	}
	return FALSE;
}

