/*
 *  @file
 *  @brief  window layer base class.
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2017.11.16
 *
 *  Copyright (C) 2017 miragekiller
 */

#pragma once

#ifndef UILAYERWND_H
#define UILAYERWND_H

#include "uilayer.h"
#include "tmani.h"

TM_BGN_NAMESPACE


class CTuiLayerWnd;
class IRealWindow
{
public:
    virtual TMHWnd GetReal() const = 0;
    virtual CTuiLayerWnd* GetLayer() = 0;

    virtual TMBool IsAllStyle(TMUInt32 dwStyle, TMUInt32 dwMask) = 0;
    virtual CFrame* GetFrame() = 0;
    
    virtual void RealUpdateAlpha(TMBool bUpdateNow) = 0;
    virtual void RealChangeFrame(TMBool bPost) = 0;
    virtual void RealInvalidate(TMBool bDrawHide = false, TMBool bUpdate = false) = 0;
};


class TM_NO_VTABLE CTuiLayerWnd
    : public CTuiLayerBase
{
public:
    TMUInt          m_interval;

public:
    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_INT    ("Interval", m_interval)
        CHAIN_OBJ_PROP  (CTuiLayerBase)
    END_OBJ_PROP_MAP    ()

    TM_DECLARE_FLAG     (_MS_PRI_NEEDERASEBKGND,    m_dwPriStyle, MS_PRI_NEEDERASEBKGND);
    TM_DECLARE_FLAG     (_MS_PRI_NEEDRESETSIZEOBX,  m_dwPriStyle, MS_PRI_NEEDRESETSIZEOBX);

    BEGIN_TUIMSG_MAP    (CTuiLayerWnd)
        MESSAGE_HANDLER (CM_SETINTERVAL,            OnSetInterval)
        MESSAGE_HANDLER (CM_SETALPHA,               OnSetAlpha)
        CHAIN_MSG_MAP   (CTuiLayerBase)
    END_TUIMSG_MAP()

    TMResult OnSetInterval(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { m_interval = lParam; return 0; }
    TMResult OnSetAlpha(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

public:
    CTuiLayerWnd() : m_interval(0) {}
    ~CTuiLayerWnd();

    void MgrUpdateWindow() { GetReal()->RealInvalidate(false, true); }
    TMUInt GetInterval() { return (Edit_IsObjectInDocMode(this)) ? 0 : m_interval; }
    
public:
    // CTuiWgt
    virtual TMBool NativeInvalidateRect(SPY_INVALIDATE_PARAM(const TUIRect& rc, TM::HTUI hSender)) tm_override;

    //CTuiLayerWnd
    virtual IRealWindow* GetReal() { return 0; }
    virtual TMUInt32 GetThemeStyle() { return m_dwStyle; }
    virtual TMUInt32 GetThemeExStyle() { return m_dwAdvStyle; }
};



TM_END_NAMESPACE



#endif //UILAYERWND_H
