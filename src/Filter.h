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
#pragma once

#define FILTER_MEAN_DEPTH 3

class CFilter
{
public:
    CFilter(void);
    ~CFilter(void);
    DWORD64     Filter(DWORD ticks, DWORD64 value);
    void        Clear();
    void        Init(DWORD ticks, DWORD64 value);
private:
    DWORD64     dwTotalOld;

    DWORD       dwPrevTicks;

    DWORD64     diffs[FILTER_MEAN_DEPTH];
};
