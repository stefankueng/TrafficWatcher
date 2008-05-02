#ifndef __WHOIS_H__
#define __WHOIS_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "winsock2.h"
#include "ws2tcpip.h"
#include <afxsock.h>
#include <afxtempl.h>


/**
 * CWhoIs.
 * implements the WhoIs service. the WhoIs service is used to obtain
 * information about who registered an internet domain directly from
 * the registrars. the service is performed on ip port 43.
 * the class uses a file named whoisservers.txt which contains information
 * about which server to use for specific top level domains.
 * the format of the file is:
 * tld server
 * e.g
 * @source
 * .com whois.internic.net
 * .ch whois.nic.ch
 * .de whois.nic.de
 * @endsource
 * you can modify that file to fill in new whois servers.
 */
class CWhoIs
{
public://Constructors / Destructors

    /*!
     * initialises the class. also sets a default whois host (whois.internic.net)
     */
	CWhoIs();
	~CWhoIs();

public://methods

    /*!
     * retrieves the information from a registrar.
     *
     * @param szAddress : the domain to get information about
     *
     * @return  : a CString containing all the received information
     */
	CString		GetWhoIs(LPCSTR szAddress);

    /*!
     * sets a specific whois server. use this only if no server is specified in the file
	 * whoisservers.txt.
     *
     * @param szServerName : the servername
     */
	void		SetWhoIsServer(LPCSTR szServerName);

    /*!
     * reads the file whoisservers.txt to obtain specific whois servers from tld's.
     */
	BOOL		Init();

protected:
	/**
	 * internal structure. contains one line from the file whoisservers.txt.
	 */
	struct WhoIsServer
	{
		CString tld;			//top level domain
		CString server;			//the corresponding whois server
	};



    /*!
     * retrieves the ip address from a hostname.
     *
     * @param szHostName : the hostname (e.g. www.microsoft.com)
     *
     * @return  : the ip address
     */
	CString		GetIpFromHost(LPCSTR szHostName);

    /*!
     * finds a whois server for the tld of szAddress.
     *
     * @param szAddress : the domain or hostname
     */
	void		LocateWhoisServer(LPCSTR szAddress);

protected://members
	
	int			m_iWhoIsPort;
	CString		m_szWhoIsServer;
	CString		m_szWhoIsServerIP;
	CArray<WhoIsServer, WhoIsServer&>		m_arWhoIsServer;
};

#endif