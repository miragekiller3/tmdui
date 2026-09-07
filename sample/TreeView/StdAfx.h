// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__F3A7F3EB_5F22_496E_8BA2_1E1A2CF52B0E__INCLUDED_)
#define AFX_STDAFX_H__F3A7F3EB_5F22_496E_8BA2_1E1A2CF52B0E__INCLUDED_

#include "tmdef.h"


#ifdef TM_WIN

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers

#if _MSC_VER < 1300 // vc6:120X
#   define WINVER       0x050a
#   define _WIN32_WINNT 0x0500
#   define _WIN32_IE    0x0600
#else
#   define WINVER       0x050a
#   define _WIN32_WINNT 0x0501 // 0x0501:xp; 0x0600:vista; 0x601:win7;
#   define _WIN32_IE    0x0600
#endif

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <atlapp.h>

#include "atlwin.h"
#include <atlgdi.h>

#endif // TM_WIN



#include "tmatl.h"
#include "tmstring.h"
#include "tmduiatl.h"

#endif // !defined(AFX_STDAFX_H__F3A7F3EB_5F22_496E_8BA2_1E1A2CF52B0E__INCLUDED)
