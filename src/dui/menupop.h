/**
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.06.19
 *
 *  Copyright (C) 2006 miragekiller
 */

#pragma once

#ifndef MENUPOP_H
#define MENUPOP_H

#include "tmdui.h"


TM_BGN_NAMESPACE

class CTuiWnd;

// #if defined(TM_WIN) && !defined(TM_QT)

struct CMenuPop
//    : public CHookCallWndRet
//     , public CAni
//     , public CTuiWidgetImpl<CMenuPop>
//     : public CHookMouseLL
//     : public CHookMouse
{
private:
    struct POPWND
    {
        HTUI hOwner;
        HTUI hPop;
    };
    typedef CVector<POPWND> CPopList;
    typedef CPopList::recorder CPopRec;
//     typedef CHookMouse hook_class;
    CPopList m_aPops;

    void _OnCancelModeMsg(HTUI hWnd, TUIMSG* pMsg, int idRet);
    void _OnLButtonDownMsg(TUIMSG* pMsg);
    void _OnHideMsg(TUIMSG* pMsg);
    static void _CancelMode(HTUI hOwner, TMBool bRestoreFocus, TUIMSG* pMsg, int idRet);
    void _ResetLast(TUIMSG* pMsg);
    void _ForwardCommandMessage(HTUI hWgt, TMWParam wp, TMLParam lp);

public:
    TMBool HavePop() const { return !m_aPops.empty(); }
    TMBool ProcessMessageWin(HTUI hMgr, TMUInt message, TMWParam wp, TMLParam lp);
    TMBool ProcessMessageWgt(HTUI hWnd, TMUInt message, TMWParam wp, TMLParam lp);
    TMBool IsMenuPopMessage(CTuiWnd* hMgr, TUIMSG* pMsg);

    void DoMenuPop(HTUI hOwner, HTUI hWnd, HTUI hFocus);
    HTUI GetMenuPop(HTUI hWnd);
    int GetMenuPopLevel(HTUI hWnd, TMBool bOwner);
    TMBool EndMenuPopLevel(int nLevel, int nResult, TMBool bRestoreFocus, TUIMSG* p);
    TMBool GetMenuPopInfo(int nLevel, HTUI* pOwner, HTUI* pPop);
    void Clear(TMBool bRestoreFocus, TUIMSG* pMsg);
};

// #else // #if defined(TM_WIN) && !defined(TM_QT)
// 
// 
// 
// #endif // #if defined(TM_WIN) && !defined(TM_QT) #else

extern CMenuPop g_menuPop;


TM_END_NAMESPACE


#endif // MENUPOP_H
