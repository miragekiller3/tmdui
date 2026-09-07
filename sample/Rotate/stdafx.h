/**
 *    @file
 *
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2018-12-4
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2018 miragekiller                               
 *    All rights reserved  
 */

#ifndef STDAFX_H
#define STDAFX_H

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

#endif // STDAFX_H
