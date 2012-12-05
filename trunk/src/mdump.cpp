// TrafficWatcher - a network speed monitor

// Copyright (C) 2012 - Stefan Kueng

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

LPCSTR MiniDumper::m_szAppName;

MiniDumper::MiniDumper( LPCSTR szAppName )
{
    // if this assert fires then you have two instances of MiniDumper
    // which is not allowed
    assert( m_szAppName==NULL );

    m_szAppName = szAppName ? strdup(szAppName) : "Application";

    ::SetUnhandledExceptionFilter( TopLevelFilter );
}

LONG MiniDumper::TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo )
{
    LONG retval = EXCEPTION_CONTINUE_SEARCH;

    // firstly see if dbghelp.dll is around and has the function we need
    // look next to the EXE first, as the one in System32 might be old
    // (e.g. Windows 2000)
    HMODULE hDll = NULL;
    char szDbgHelpPath[MAX_PATH];

    if (GetModuleFileName( NULL, szDbgHelpPath, MAX_PATH ))
    {
        char *pSlash = _tcsrchr( szDbgHelpPath, '\\' );
        if (pSlash)
        {
            _tcscpy( pSlash+1, "dbghelp.dll" );
            hDll = ::LoadLibrary( szDbgHelpPath );
        }
    }

    if (hDll==NULL)
    {
        // load any version we can
        hDll = ::LoadLibrary( "dbghelp.dll" );
    }

    LPCTSTR szResult = NULL;

    if (hDll)
    {
        MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );
        if (pDump)
        {
            char szDumpPath[MAX_PATH];
            char szScratch [MAX_PATH];

            // work out a good place for the dump file
            if (!GetTempPath( MAX_PATH, szDumpPath ))
                _tcscpy( szDumpPath, "c:\\temp\\" );

            _tcscat( szDumpPath, m_szAppName );
            _tcscat( szDumpPath, ".dmp" );

            // ask the user if they want to save a dump file
            if (::MessageBox( NULL, "Something bad happened in your program, would you like to save a diagnostic file?", m_szAppName, MB_YESNO )==IDYES)
            {
                // create the file
                HANDLE hFile = ::CreateFile( szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
                                            FILE_ATTRIBUTE_NORMAL, NULL );

                if (hFile!=INVALID_HANDLE_VALUE)
                {
                    _MINIDUMP_EXCEPTION_INFORMATION ExInfo;

                    ExInfo.ThreadId = ::GetCurrentThreadId();
                    ExInfo.ExceptionPointers = pExceptionInfo;
                    ExInfo.ClientPointers = NULL;

                    // write the dump
                    BOOL bOK = pDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL );
                    if (bOK)
                    {
                        sprintf( szScratch, "Saved dump file to '%s'", szDumpPath );
                        szResult = szScratch;
                        retval = EXCEPTION_EXECUTE_HANDLER;
                    }
                    else
                    {
                        sprintf( szScratch, "Failed to save dump file to '%s' (error %d)", szDumpPath, GetLastError() );
                        szResult = szScratch;
                    }
                    ::CloseHandle(hFile);
                }
                else
                {
                    sprintf( szScratch, "Failed to create dump file '%s' (error %d)", szDumpPath, GetLastError() );
                    szResult = szScratch;
                }
            }
        }
        else
        {
            szResult = "dbghelp.dll too old";
        }
    }
    else
    {
        szResult = "dbghelp.dll not found";
    }

    if (szResult)
        ::MessageBox( NULL, szResult, m_szAppName, MB_OK );

    return retval;
}
