#pragma once

class CFilter
{
public:
	CFilter(void);
	~CFilter(void);
	DWORD64		Filter(DWORD ticks, DWORD64 value);
private:
	DWORD64		dwTotalOld;

	DWORD		dwPrevTicks;

};
