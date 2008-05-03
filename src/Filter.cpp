#include "stdafx.h"
#include "Filter.h"

CFilter::CFilter(void)
{
	for (int i=0; i<FILTER_MEAN_DEPTH; ++i)
		diffs[i] = 0;
	dwTotalOld = 0;
	dwPrevTicks = 0;
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

	DWORD64 meanDiff = diffValue;
	for (int i=0; i<FILTER_MEAN_DEPTH; ++i)
		meanDiff += diffs[i];
	meanDiff /= (FILTER_MEAN_DEPTH+1);

	for (int i=FILTER_MEAN_DEPTH-1; i>0; --i)
		diffs[i] = diffs[i-1];
	diffs[0] = diffValue;

	return meanDiff * 1000 / diffTicks;
}