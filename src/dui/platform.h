/*
 *  @file
 *  @brief  cross platform support
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2011.4.27
 *
 *  Copyright (C) 2011 miragekiller
 */

#pragma once

#ifndef PLATFORM_H
#define PLATFORM_H

#include "../tmdui.h"
#include "../tmlist.h"
#include "../tmcallset.h"

#if TM_WIN
#   include <windows.h>
#   include "../tmplatform_win.h"
#elif TM_QNX
#   include <screen/screen.h>
#elif TM_ANDROID
#   include <jni.h>
#endif

#ifdef MOZILLA_STRICT_API
#   define uintptr_t unsigned long
// #   define uint64_t  unsigned __int64
#   include "base/public/npapi.h"
#endif

#ifdef TM_WIN


#endif // TM_WIN


TM_BGN_NAMESPACE


class CTuiWnd;
class CTuiWgt;


struct WIN
{
    // bNewTrack:
    // true:  call when mousemove
    // false: call when onmouseleave.
    static TMBool TrackMouseLeave(TMHWnd hWnd, TMBool bClient);
    static TMBool RemoveMesssage(TMHWnd hWnd, TMUInt uMsgMin, TMUInt uMsgMax);
    static void InvalidateRect(TMHWnd hWnd, TMBool bLayered, SPY_INVALIDATE_PARAM(const TMRect* rc, TM::HTUI hSender));
//     static void UpdateWin(TMHWnd hWnd);
    static void SetRTLOnLanguageChanged(TMHWnd hWnd, TMLParam lp);
    static TMBool IsAllStyle(TMHWnd hWnd, TMUInt32 dwStyle, TMUInt32 dwMask);
    static TMResult OnMouseActivate(TMHWnd hWnd, TMHWnd hOwner, TMUInt uMsg, TMWParam wp, TMLParam lp);
    static TMBool OnGetObject(TM::HTUI hWgt, TMWParam wp, TMLParam lp, TMResult& lr);
    static void SetWinProp(TMHWnd hWnd, int nType);
    static TMLParam GetWinPropType(TMHWnd hWnd);
    static TMBool HookGlobalMouseDown(HTUI hWnd); // hWnd == 0: unhook;
    static void ImeEnable(CTuiWnd* mgr, TMBool bEnable); 
    static void ImeMoveTo(CTuiWnd* mgr, HTUI hCaret, const CRect& rc);
    static void ImeOnStartCompostion(); 
    static void ImeOnComposition(CTuiWnd* mgr, TMWParam wp, TMLParam& lp); // return: bhandled
};



TM_END_NAMESPACE

#endif // PLATFORM_H
