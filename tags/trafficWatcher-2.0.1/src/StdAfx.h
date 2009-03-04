#pragma once

#ifndef WINVER
#	define WINVER 0x0600
#endif

#ifndef _WIN32_WINNT
#	define _WIN32_WINNT 0x0600
#endif						

#ifndef _WIN32_WINDOWS
#	define _WIN32_WINDOWS 0x0600
#endif

#ifndef _WIN32_IE
#	define _WIN32_IE 0x0501
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif

#include <commctrl.h>
#include <Shlobj.h>
#include <Shlwapi.h>
#include <tchar.h>
#include <math.h>


