// TrafficWatcher - a network speed monitor

// Copyright (C) 2008 - Stefan Kueng

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

#pragma once


/**
 * CPortStat.
 * CPortStat holds data about ip ports and builds summaries of that data.
 */
class CPortStat : CWnd
{
public:
    CPortStat();
    virtual ~CPortStat();

    /**
     * adds new data and builds statistics.
     *
     * \param port : specifies from wich ip-port the data was sent or received
     * \param len : the length of the datapacket
     */
    void    AddDataTCP(USHORT awayport, USHORT localport, ULONG len);

    /**
     * add new data and builds statistics
     *
     * \param port : specifies from wich ip-port the data was sent or received
     * \param len : the length of the datapacket
     */
    void    AddDataUDP(USHORT awayport, USHORT localport, ULONG len);

    /**
     * add new data and builds statistics
     *
     * \param port : specifies from wich ip-port the data was sent or received
     * \param len : the length of the datapacket
     */
    void    AddData(USHORT awayport, USHORT localport, ULONG len);

    /**
     * returns the total amount of data from all ports.
     *
     * \return  : the total data amount
     */
    DWORD64 GetTotal();

    /**
     * clears all statistics. should be called before using this class
     */
    void    Clear();

    /**
     * returns the transferred data for all 'other' services which are not covered by
     * separate methods.
     */

    DWORD64 GetIndexRec(int i) {return m_services[i];};
    DWORD64 GetIndexSent(int i) {return m_services[i];};

public://members
    DWORD64 m_services[LASTSERVICE];                //place for different services
};
