/**
 *    @file
 *
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2017-12-8
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2017 miragekiller                               
 *    All rights reserved  
 */

#ifndef STDAFX_H
#define STDAFX_H

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

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;

#pragma warning(push)
#pragma warning(disable : 4996) // warning C4996: 'GetVersionExW': was declared deprecated
#include <atlapp.h>
#pragma warning(pop)

#include <atlcom.h>
#include <atlwin.h>
#include <commdlg.h>
//#include "atlctrls.h"

#include "tmstring.h"
#include "tmduiatl.h"
#include "tmtimer.h"



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // STDAFX_H
