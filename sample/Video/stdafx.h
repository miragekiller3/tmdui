/**
 *    @file
 *
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2012-8-19
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2012 miragekiller                               
 *    All rights reserved  
 */

#ifndef STDAFX_H
#define STDAFX_H

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
//#include "atlctrls.h"

#include "tmstring.h"
#include "tmduiatl.h"
#include "tmdshow.h" 


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // STDAFX_H
