#pragma once

#define FILTER_MEAN_DEPTH 3
class CFilter
{
public:
	CFilter(void);
	~CFilter(void);
	DWORD64		Filter(DWORD ticks, DWORD64 value);
private:
	DWORD64		dwTotalOld;

	DWORD		dwPrevTicks;

	DWORD64		diffs[FILTER_MEAN_DEPTH];
};
