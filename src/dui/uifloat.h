/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2017.11.30
 *
 *  Copyright (C) 2017 miragekiller
 */

#pragma once

#ifndef UIFLOAT_H
#define UIFLOAT_H

#include "uilayerwnd.h"
#include "../tmwnd.h"
#include "uiwindow_win.h"


TM_BGN_NAMESPACE


class TM_NO_VTABLE CTuiFloatImpl
    : public CTuiLayerWnd
    , public CHookCallWndRetImpl<CTuiFloatImpl>
    , public CRefWindowT<CTuiFloatImpl, CRealWindow>
{
public:
    typedef CHookCallWndRetImpl<CTuiFloatImpl>          CHookCallWndRet;
    typedef CRefWindowT<CTuiFloatImpl, CRealWindow>     CRefWindow;

    TM_BGN_MASK_SYMBOL2 (CSymbol_FStyle, CTuiStaticImpl::CSymbol_Style)
        TM_MASK_SYMBOL  (DS_NODWMSHADOW)
        TM_MASK_SYMBOL  (DS_DWMBLUR)
        TM_MASK_SYMBOL  (DS_NODWMTRANSPARENT)
        TM_MASK_BREAK   ()
    TM_END_MASK_SYMBOL  ()

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_MASK   ("Style", m_dwStyle, CSymbol_FStyle)
        CHAIN_OBJ_PROP  (CTuiLayerWnd)
    END_OBJ_PROP_MAP    ()
        
    BEGIN_TUIMSG_MAP(CTuiFloatImpl)
        //DbgTrace(TM_T("oid:%d; msg:%s"), GetObjectID(), DbgFormatMessage(uMsg).c_str());
        MESSAGE_HANDLER (WM_NCCREATE,               OnCreate)
        MESSAGE_HANDLER (CM_GETWINDOW,              OnGetWindow)
        MESSAGE_HANDLER (CM_GET_THEME_TYPE,         OnGetThemeType)

        ENTRY_LIFE_MAP  (this)
        MESSAGE_HANDLER (WM_ENABLE,                 OnEnable)
        MESSAGE_HANDLER (CM_ENABLE_P,               OnEnable)
        MESSAGE_HANDLER (CM_LANGUAGECHANGED,        OnLanguageChanged)
        MESSAGE_HANDLER (CM_WINDOWPOSCHANGED,       OnWindowPosChanged)
        MESSAGE_HANDLER (CM_SHOWWINDOW_P,           OnWindowPosChanged)
        MESSAGE_HANDLER (CM_SETPARENT_P,            OnWindowPosChanged)
        MESSAGE_HANDLER (CM_PRI_DIRTY,              OnDirty)
        LEAVE_LIFE_MAP  ()

        CHAIN_MSG_MAP   (CTuiLayerWnd)
    END_TUIMSG_MAP      ()

    TMResult OnCreate(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnEnable(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnLanguageChanged(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnWindowPosChanged(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnGetWindow(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { return (TMResult)GetOverlayHWND(); }
    TMResult OnGetThemeType(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDirty(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    
public:
    CTuiFloatImpl();
    TMBool CheckVisible(TMBool bUpdateNow);

public:
    // CTuiWgt
    virtual TMHWnd GetOverlayHWND() tm_override;
    virtual void Reclaim() tm_override;
    virtual void DoPrint(IGXApp* gapp, TMLParam lParam) tm_override;
    virtual void OnSetLife(TMUInt32 life) tm_override;

    // CTuiLayerWnd
    virtual IRealWindow* GetReal() tm_final { return this; }
    virtual void CreateBkBuffer(IGXApp* gapp, const CSize& sz) tm_override;
    virtual TMUInt32 GetThemeStyle() tm_override { return m_dwStyle; }
    virtual TMUInt32 GetThemeExStyle() tm_override { return WS_ADV_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | (WS_EX_TRANSPARENT & m_dwAdvStyle); }

    // IRealWindow
    CTuiLayerWnd* GetLayer() tm_final { return this; }

    // CRefWindowT
    static LPCTSTR GetWndClassName() { return TUI_CLASS_WIN_FLOAT; }
    LRESULT WndProc(TMUInt uMsg, TMWParam wParam, TMLParam lParam);
    
    // CHookCallWndRet
    TMBool OnCallWndRetProc(CWPRETSTRUCT* ps);
};


TM_END_NAMESPACE



#endif //UIFLOAT_H
