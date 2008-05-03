#include "stdafx.h"
#include "Filter.h"

CFilter::CFilter(void)
{
}

CFilter::~CFilter(void)
{
}

DWORD64 CFilter::Filter(DWORD ticks, DWORD64 value)
{
	DWORD diffTicks = ticks - dwPrevTicks;
	dwPrevTicks = ticks;

	DWORD64 diffValue = value - dwTotalOld;
	dwTotalOld = value;

	return diffValue *1000 / diffTicks;
}