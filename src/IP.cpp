// IP.cpp: implementation of the CIP class.
//
//////////////////////////////////////////////////////////////////////

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

BOOL    CIP::CreateAdapterList()
{
    WCHAR       AdapterName[512]; // string that contains a list of the network adapters
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
