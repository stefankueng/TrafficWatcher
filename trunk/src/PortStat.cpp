// TrafficWatcher - a network speed monitor

// Copyright (C) 2008-2009 - Stefan Kueng

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#include "stdafx.h"
#include "trafficwatch.h"
#include "PortStat.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPortStat::CPortStat()
{
}

CPortStat::~CPortStat()
{
}

void CPortStat::AddData(USHORT /*awayport*/, USHORT /*localport*/, ULONG len)
{
    AddDataTCP(0, 0, len);
}

void CPortStat::AddDataTCP(USHORT awayport, USHORT localport, ULONG len)
{
    awayport = ntohs(awayport);
    localport = ntohs(localport);
    switch (awayport)
    {
        //we are the client, using the awayport
        case 119:   //nntp
        case 563:   //nntp over ssl
            m_services[S_NEWS] += (DWORD)len;
            break;
        case 80:
        case 443:   //http over ssl
            m_services[S_WEB] += (DWORD)len;
            break;
        case 25:    //smtp
        case 109:   //pop2
        case 110:   //pop3
        case 143:
        case 220:
        case 585:
        case 993:
        case 995:   //pop3 over ssl
            m_services[S_MAIL] += (DWORD)len;
            break;
        case 1214:
        case 6346:
        case 6347:
        case 6699:
        case 8888:
        case 4661:
        case 4662:
        case 6367:
        case 6426:
            m_services[S_SHARE] += (DWORD)len;
            break;
        default:
            //either unknown protocol or we are the server!
            switch (localport)
            {
                case 119:   //nntp
                case 563:   //nntp over ssl
                    m_services[S_NEWS] += (DWORD)len;
                    break;
                case 80:
                case 443:   //http over ssl
                    m_services[S_WEB] += (DWORD)len;
                    break;
                    //even if it's highly unlikely that a home-host is a mail or newsserver, check that too
                case 25:    //smtp
                case 109:   //pop2
                case 110:   //pop3
                case 143:
                case 220:
                case 585:
                case 993:
                case 995:   //pop3 over ssl
                    m_services[S_MAIL] += (DWORD)len;
                    break;
                    //for filesharing, home-hosts are always servers too!
                case 1214:
                case 6346:
                case 6347:
                case 6699:
                case 8888:
                case 4661:
                case 4662:
                case 6367:
                case 6426:
                    m_services[S_SHARE] += (DWORD)len;
                    break;
                default:
                    m_services[S_OTHER] += (DWORD)len;
                    break;
            }
            break;
    }
}

void CPortStat::AddDataUDP(USHORT /*awayport*/, USHORT /*localport*/, ULONG len)
{
    m_services[S_UDP] += (DWORD)len;
}

DWORD64 CPortStat::GetTotal()
{
    int i;
    DWORD64 temp = 0;
    for (i=0; i<LASTSERVICE; i++)
        temp += m_services[i];
    return temp;
}

void CPortStat::Clear()
{
    int i;
    for (i=0; i<LASTSERVICE; i++)
        m_services[i] = 0;
}
