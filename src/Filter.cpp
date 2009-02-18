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

void CFilter::Clear()
{
	for (int i=0; i<FILTER_MEAN_DEPTH; ++i)
		diffs[i] = 0;
	dwTotalOld = 0;
	dwPrevTicks = 0;
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

	if (diffTicks)
		return meanDiff * 1000 / diffTicks;
	return 0;
}

void CFilter::Init(DWORD ticks, DWORD64 value)
{
	Clear();
	dwPrevTicks = ticks;
	dwTotalOld = value;
}