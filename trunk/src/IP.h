// TrafficWatcher - a network speed monitor

// Copyright (C) 2008, 2012 - Stefan Kueng

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

#include <windows.h>
#include <conio.h>


#define Max_Num_Adapter 10


class CIP
{
public:
    CIP();
    virtual ~CIP();

    BOOL    CreateAdapterList();

public:
    LPADAPTER   lpAdapter;              //define a pointer to an ADAPTER structure
    LPPACKET    lpPacket;               //define a pointer to a PACKET structure
    WCHAR       AdapterName[512];       //string that contains a list of the network adapters


private:
    char        AdapterList[Max_Num_Adapter][1024];

};
