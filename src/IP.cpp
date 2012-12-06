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

#include "stdafx.h"
#include "trafficwatch.h"
#include "IP.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIP::CIP()
{
}

CIP::~CIP()
{
}

BOOL CIP::CreateAdapterList()
{
    WCHAR       AdapterName[512];   // string that contains a list of the network adapters
    WCHAR       *temp,*temp1;


    ULONG       AdapterLength;
    int         AdapterNum=0;

    PacketGetAdapterNames((char *)AdapterName,&AdapterLength);
    temp=AdapterName;
    temp1=AdapterName;
    int i=0;
    while ((*temp!='\0')||(*(temp-1)!='\0'))
    {
        if (*temp=='\0')
        {
            memcpy(AdapterList[i],temp1,(temp-temp1)*2);
            temp1=temp+1;
            i++;
    }

    temp++;
    }

    AdapterNum=i;
    for (i=0;i<AdapterNum;i++)
        wprintf(L"\n%d- %s\n",i+1,AdapterList[i]);
    printf("\n");

    return TRUE;
}
