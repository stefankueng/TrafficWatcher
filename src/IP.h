// IP.h: interface for the CIP class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>
#include <conio.h>
//#include "packet32.h"


#define SIMULTANEOU_READS 10
#define MAX_ETHERNET_FRAME_SIZE 1514

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
