// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__F3A7F3EB_5F22_496E_8BA2_1E1A2CF52B0E__INCLUDED_)
#define AFX_STDAFX_H__F3A7F3EB_5F22_496E_8BA2_1E1A2CF52B0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _ATL_APARTMENT_THREADED


#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers

#if _MSC_VER < 1300 // vc6:120X
#   define _WIN32_WINNT 0x0500
#   define _WIN32_IE    0x0600
#   define WINVER 0x050a
#else
#   define _WIN32_WINNT 0x0501
#   define _WIN32_IE    0x0600
#   define WINVER 0x050a
#endif

// #   define TM_RENDER_GL        1


//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
#include <atlbase.h>
// #include "../../tmduiatl.h"
// class CExeModule : public CComModule
// {
// public:
//     TMULong dwThreadID;
//     HANDLE hEventShutdown;
//     bool bActivity;
// };
extern CComModule _Module;
#include <atlcom.h>
#include <atlwin.h>
#include <atlhost.h>
// Windows Header Files:
#include <atlapp.h>
#include <atlctl.h>
//#include <atlframe.h>
//#include <atlhost.h>
#include <atlctrls.h>
// Windows Header Files:
//#include <windows.h>

// C RunTime Header Files
// #include <stdlib.h>
// #include <malloc.h>
// #include <memory.h>
// #include <tchar.h>
// #include <crtdbg.h>
// 
// #include <mmsystem.h>
// #include <time.h>
// #include <Shellapi.h>
// #include <Shlwapi.h>
// #include <commctrl.h>
// #include <Richedit.h>
// #include <commdlg.h>

#include "tmstring.h"
#include "tmduiatl.h"

#include "resource.h"


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__F3A7F3EB_5F22_496E_8BA2_1E1A2CF52B0E__INCLUDED)
