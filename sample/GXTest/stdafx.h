/**
 *  @file
 *
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021-8-17
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2021 miragekiller                             
 *  All rights reserved  
 */

#ifndef STDAFX_H
#define STDAFX_H

#include "tmdef.h"


#ifdef TM_WIN

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers

#if _MSC_VER < 1300 // vc6:120X
#   define WINVER       0x050a
#   define _WIN32_WINNT 0x0500
#   define _WIN32_IE    0x0600
#else
#   define WINVER       0x050a
#   define _WIN32_WINNT 0x0501 // 0x0501:xp; 0x0600:vista; 0x0601:win7; 0x0602:win8; 0x0603:win8.1; 0x0A00:win10
#   define _WIN32_IE    0x0600
#endif

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <atlwin.h>
#include "atlapp.h"
#include "atlgdi.h"
#include "atlctrls.h"

#endif // TM_WIN



#include "tmatl.h"
#include "tmstring.h"
#include "tmduiatl.h"
#include "tmgdix.h"

#endif // STDAFX_H
