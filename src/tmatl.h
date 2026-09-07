/*
 *  @file
 *  @brief  ATL for all platform support
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2010.4.28
 *
 *  Copyright (C) 2010 miragekiller
 */

#pragma once

#ifndef TMATL_H
#define TMATL_H

#include "tmdef.h"

#if defined(TM_WIN_DESKTOP) // && !defined(TM_QT)

#   define _CRT_SECURE_NO_WARNINGS
#   define WIN32_LEAN_AND_MEAN      // Exclude rarely-used stuff from Windows headers

#   if _MSC_VER < 1300 // vc6:120X
#       define __WINVER         0x050a
#       define __WIN32_WINNT    0x0500
#       define __WIN32_IE       0x0600
#   else
#       define __WINVER         0x050a
#       define __WIN32_WINNT    0x0501 // 0x0501:xp; 0x0600:vista; 0x601:win7;
#       define __WIN32_IE       0x0600
#   endif // _MSC_VER

#   ifndef WINVER
#       define WINVER       __WINVER
#   endif

#   ifndef _WIN32_WINNT
#       define _WIN32_WINNT __WIN32_WINNT
#   endif

#   ifndef _WIN32_IE
#       define _WIN32_IE    __WIN32_IE
#   endif
#endif // TM_WIN_DESKTOP


#if TM_USE_MS_ATL

// don't use _Module; _Module replace by _ModuleATL
#   pragma warning(push)
#   define _Module (*ATL::_pModule)
#   pragma warning(disable : 4996) // warning C4996: 'GetVersionExW': was declared deprecated

#   ifndef __ATLBASE_H__
#       include <atlbase.h>
#   endif // __ATLBASE_H__

#   ifndef __ATLWIN_H__
#       include <atlwin.h>
#   endif // __ATLWIN_H__

#   ifndef __ATLAPP_H__
#       include <atlapp.h>
#   endif // __ATLAPP_H__

#   ifndef __ATLGDI_H__
#       include <atlgdi.h>
#   endif // __ATLGDI_H__

#   ifndef __ATLCOM_H__
#       include <atlcom.h>
#   endif


#   undef _Module
#   pragma warning(pop)

#   if (_ATL_VER >= 0x0700)
#       define _ModuleATL ATL::_AtlBaseModule
#   else // !(_ATL_VER >= 0x0700)
#       define _ModuleATL (*ATL::_pModule)
#   endif // !(_ATL_VER >= 0x0700)

TM_BGN_NAMESPACE_X(std)

inline TMCStrW data(const ATL::CComBSTR& c) tm_noexcept { return c; }
inline size_t size(const ATL::CComBSTR& c) { return c.Length(); }
inline void swap(ATL::CComBSTR& c1, ATL::CComBSTR& c2, ...) tm_noexcept { tm_swap(c1.m_str, c2.m_str); }

TM_END_NAMESPACE_X(std)


#else // TM_USE_MS_ATL

//#ifndef TMATOMIC_H
//#   include "tmatomic.h"
//#endif

//#define __ATLBASE_H__
//#define __ATLWIN_H__
//#define __ATLAPP_H__
//#define __ATLGDI_H__
//#define __ATLCOM_H__

#include "tmcom.h"

#endif // TM_USE_MS_ATL


#define CHAIN_MSG_MAP_POINTER(theChainPointer) \
    { \
        if (theChainPointer && theChainPointer->ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult)) \
            return TMTrue; \
    }

#ifndef BEGIN_MSG_MAP

#define BEGIN_MSG_MAP(theClass) \
public: \
    TMBool ProcessWindowMessage(TMHWnd hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID = 0) \
    { \
        TMBool bHandled = TMTrue; \
        hWnd; \
        uMsg; \
        wParam; \
        lParam; \
        lResult; \
        bHandled; \
        switch (dwMsgMapID) \
        { \
        case 0:

#define MESSAGE_HANDLER(msg, func) \
    if (uMsg == msg) \
    { \
        bHandled = TMTrue; \
        lResult = func(uMsg, wParam, lParam, bHandled); \
        if (bHandled) \
            return TMTrue; \
    }

#define MESSAGE_RANGE_HANDLER(msgFirst, msgLast, func) \
    if (uMsg >= msgFirst && uMsg <= msgLast) \
    { \
        bHandled = TMTrue; \
        lResult = func(uMsg, wParam, lParam, bHandled); \
        if (bHandled) \
            return TMTrue; \
    }

#define COMMAND_HANDLER(id, code, func) \
    if (uMsg == WM_COMMAND && id == LOWORD(wParam) && code == HIWORD(wParam)) \
    { \
        bHandled = TMTrue; \
        lResult = func(HIWORD(wParam), LOWORD(wParam), (TMHWnd)lParam, bHandled); \
        if (bHandled) \
            return TMTrue; \
    }

#define COMMAND_ID_HANDLER(id, func) \
    if (uMsg == WM_COMMAND && id == LOWORD(wParam)) \
    { \
        bHandled = TMTrue; \
        lResult = func(HIWORD(wParam), LOWORD(wParam), (TMHWnd)lParam, bHandled); \
        if (bHandled) \
            return TMTrue; \
    }

#define COMMAND_CODE_HANDLER(code, func) \
    if (uMsg == WM_COMMAND && code == HIWORD(wParam)) \
    { \
        bHandled = TMTrue; \
        lResult = func(HIWORD(wParam), LOWORD(wParam), (TMHWnd)lParam, bHandled); \
        if (bHandled) \
            return TMTrue; \
    }

#define COMMAND_RANGE_HANDLER(idFirst, idLast, func) \
    if (uMsg == WM_COMMAND && LOWORD(wParam) >= idFirst  && LOWORD(wParam) <= idLast) \
    { \
        bHandled = TMTrue; \
        lResult = func(HIWORD(wParam), LOWORD(wParam), (TMHWnd)lParam, bHandled); \
        if (bHandled) \
            return TMTrue; \
    }

#define CHAIN_MSG_MAP(theChainClass) \
    { \
        if (theChainClass::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult)) \
            return TMTrue; \
    }

#define CHAIN_MSG_MAP_MEMBER(theChainMember) \
    { \
        if (theChainMember.ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult)) \
            return TMTrue; \
    }

#define END_MSG_MAP() \
            break; \
        default: \
            DbgAssert(false); \
            break; \
        } \
        return TMFalse; \
    }

template <int n>
struct CComModuleT
{
public:
    TMHModule m_hInst;
    TMHModule m_hInstResource;
    static CComModuleT* s_pModule;

    CComModuleT() { DbgAssert(!s_pModule); s_pModule = this; }
    TMHModule GetModuleInstance() { return m_hInst; }
    TMHModule GetResourceInstance() { return m_hInstResource; }

    TMResult Init(void* pObjMap, TMHModule hInstance, const void* pLibID = 0)
    {
        m_hInst = m_hInstResource = hInstance;
        return 0;
    }

    void Term() {}
};
template <int n> CComModuleT<n>* CComModuleT<n>::s_pModule;
typedef CComModuleT<0> CComModule;

#   define _ModuleATL   (*CComModule::s_pModule)


class CMessageFilter
{
public:
    virtual TMBool PreTranslateMessage(struct TMMSG* pMsg) = 0;
};


#endif // BEGIN_MSG_MAP


#include "tmnative.h"
#include "tmgdi.h"
#include "tmmonitor.h"

#if defined(TM_WIN) && TM_USE_MS_ATL
#   include "tmatl_win.h"
#endif // TM_WIN

#ifdef TM_QT
#   include "tmatl_qt.h"
#endif // TM_QT

#ifdef TM_GTK
#   include "tmatl_gtk.h"
#endif


#ifdef TM_QT
#   define CAppAT               CAppQt
#   define CWindowAT            CWindowQt
#   define CIconATT             CIconQtT
#   define CIconHandleAT        CIconHandleQt
#   define CIconAT              CIconQt
#   define CCursorATT           CCursorQtT
#   define CCursorHandleAT      CCursorHandleQt
#   define CCursorAT            CCursorQt
#   define CDCATT               CDCQtT
#   define CDCHandleAT          CDCHandleQt
#   define CDCAT                CDCQt
#   define CScreenDCWrite       CScreenDCWriteQt
#   define CRefWindowT          CRefWindowQtT
#elif defined(TM_GTK)
#   define CAppAT               CAppGtk
#   define CWindowAT            CWindowGtk
#   define CIconATT             CIconGtkT
#   define CIconHandleAT        CIconHandleGtk
#   define CIconAT              CIconGtk
#   define CCursorATT           CCursorGtkT
#   define CCursorHandleAT      CCursorHandleGtk
#   define CCursorAT            CCursorGtk
#   define CDCATT               CDCGtkT
#   define CDCHandleAT          CDCHandleGtk
#   define CDCAT                CDCGtk
#   define CScreenDCWrite       CScreenDCWriteGtk
#   define CRefWindowT          CRefWindowGtkT
#elif defined(TM_WIN)
#   define CAppAT               CAppWin
#   define CWindowAT            CWindowWin
#   define CIconATT             CIconWinT
#   define CIconHandleAT        CIconHandleWin
#   define CIconAT              CIconWin
#   define CCursorATT           CCursorWinT
#   define CCursorHandleAT      CCursorHandleWin
#   define CCursorAT            CCursorWin
#   define CDCATT               CDCWinT
#   define CDCHandleAT          CDCHandleWin
#   define CDCAT                CDCWin
#   define CScreenDCWrite       CScreenDCWriteWin
#   define CRefWindowT          CRefWindowWinT
#else
#   error not support
#endif // TM_QT




#undef CWIDGET


#endif // TMATL_H
