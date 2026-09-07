/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.07.08
 *
 *  Copyright (C) 2018 miragekiller
 */

#pragma once

#ifndef UIARROW_H
#define UIARROW_H

#include "uiwidget.h"
#include "../tmhook.h"


TM_BGN_NAMESPACE


/////////////////////////////////////////////////////////////////////////////
// class CTuiArrowImpl
class TM_NO_VTABLE CTuiArrowImpl
    : public CTuiWgt
    , public CHookCallWndRetImpl<CTuiArrowImpl>
{
private:
    TMUInt32 m_nIDSource;
    Ref m_wndTarget;
    Ref m_wndCont;

public:
    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_MASK   ("Style", m_dwStyle, CTuiStaticImpl::CSymbol_Style)
        OBJ_PROP_WIN    ("BindWnd", m_nIDSource)
        CHAIN_OBJ_PROP  (CTuiWgt)
    END_OBJ_PROP_MAP    ()

    BEGIN_TUIMSG_MAP    (CTuiArrowImpl)
        MESSAGE_HANDLER (CM_SETBUDDY,                   OnSetWindow)
        MESSAGE_HANDLER (CM_GETBUDDY,                   OnGetWindow)
        MESSAGE_HANDLER (WM_DESTROY,                    OnDestroy)
        MESSAGE_HANDLER (CM_SHOWWINDOW_P,               OnRefresh)
        MESSAGE_HANDLER (WM_SHOWWINDOW,                 OnRefresh)
        MESSAGE_HANDLER (CM_SET_PADING,                 OnRefresh)
        MESSAGE_HANDLER (WM_MOVE,                       OnRefresh)
        MESSAGE_HANDLER (WM_SIZE,                       OnRefresh)
        CHAIN_MSG_MAP   (CTuiWgt)
    END_TUIMSG_MAP      ()

    // target subclass proc
    DECALRE_MSG_MAP_WGT (CTuiArrowImpl, target);

    // cont subclass proc
    DECALRE_MSG_MAP_WGT (CTuiArrowImpl, cont)  { p->ContMessage(uMsg, wParam, lParam, false); return false; }
    DECALRE_MSG_MAP_WIN (CTuiArrowImpl, contW) { p->ContMessage(uMsg, wParam, lParam, true); return false; }


    TMResult OnSetWindow (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    
    TMResult OnGetWindow (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        return (TMResult)((m_wndTarget && m_wndTarget->IsLifeInvalid()) ? (HTUI)m_wndTarget : 0);
    }

    TMResult OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnSetDirty (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnRefresh (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnSetPadding (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

    CTuiArrowImpl() : m_nIDSource(0) { m_dwStyle |= SS_DLGBKONHC; }
    void SetWin(HTUI hWnd);
    void DirtySelf();
    CTuiPoint ContCalcNewPointer(TMBool bWnd);
    void ContMessage(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool bWnd);

    // CHookCallWndRet
    TMBool OnCallWndRetProc(CWPRETSTRUCT* ps);

    // CTuiWgt
    virtual void DoPrintClient(IGXApp* gapp, TMLParam lParam) { RenderStaticBk(gapp, 0); }

    // CTuiObject
#ifdef TUI_EDIT_MODE

    CString GetDebugInfo() tm_override;

    TMBool EditorMatchID(TMUInt32 n) tm_override
    {
        if (n == m_nIDSource)
            return true;
        return CTuiWgt::EditorMatchID(n);
    }
#endif // TUI_EDIT_MODE
};


TM_END_NAMESPACE

#endif // UIARROW_H
