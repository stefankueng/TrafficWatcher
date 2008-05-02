#pragma once

#include "winsock2.h"
#include "afxinet.h"
#include <stdlib.h>




class CNetGeoClient
{
#define DEFAULT_SERVER_URL "netgeo.caida.org"


public:
	CNetGeoClient(void);
	CNetGeoClient(CString url);
	~CNetGeoClient(void);
	BOOL	GetLatLong(CString machine, double& lon, double& lan);

private:	//methods
	CString	getIPAddress(CString adr);

private:
   CString netGeoUrl;				// URL of the NetGeo server, will almost always be the default value.
   CString userAgent;				// to indicate the NetGeo server who's calling
};
