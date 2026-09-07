/*
 *  @file
 *  @brief  progress
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2019.11.26
 *
 *  Copyright (C) 2019 miragekiller
 */

#pragma once

#ifndef UIPROGRESS_H
#define UIPROGRESS_H

#include "uisys.h"


TM_BGN_NAMESPACE

const TMUInt32 PBS_SCALE    = 0x1000;
const TMUInt32 PBS_SLIDE    = 0x2000;

////////////////////////////////////////////////////////////////////////////////
// CTuiProgressImpl
class TM_NO_VTABLE CTuiProgressImpl
    : public CTuiScrollObj
{
private:
    struct CSBPic : public ITuiPic
    {
        CPicPtr         m_pBtn;
        
        TM_UNUSED_ADDREF_RELEASE();
        
        TM_BGN_INTERFACE_MAP(ITuiPic)
        TM_END_INTERFACE_MAP()
        
        // ITuiPic
        virtual TMUInt32 GetObjectID () const tm_override { return 0; }
        virtual void Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp) tm_override {}
        virtual void RenderSB(const SCROLLSTATE& state, const CTuiRect& rc, WPRENDERFX_SB* fx, IGXApp* gapp) tm_override;
    };
    friend CSBPic;
    
    CSBPic          m_sbPic;

public:
    TM_BGN_MASK_SYMBOL2 (CSymbol_Style, CSymbol_WsStyle)
        TM_MASK_SYMBOL  (PBS_SMOOTH)
        TM_MASK_SYMBOL  (PBS_VERTICAL)
        TM_MASK_SYMBOL2 (PBS_SCALE, PBS_SCALE | PBS_SLIDE)
        TM_MASK_SYMBOL2 (PBS_SLIDE, PBS_SCALE | PBS_SLIDE)
        TM_MASK_BREAK   ()
    TM_END_MASK_SYMBOL  ()

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_MASK   ("Style", m_dwStyle, CSymbol_Style)
        OBJ_PROP_PIC    ("BarPic", m_sbPic.m_pBtn)
        CHAIN_OBJ_PROP  (CTuiScrollObj)
    END_OBJ_PROP_MAP    ()

public:
    BEGIN_TUIMSG_MAP    (CTuiProgressImpl)
        MESSAGE_HANDLER (WM_NCCREATE,                       OnInit)
        MESSAGE_HANDLER (WM_SIZE,                           OnInit)
        MESSAGE_HANDLER (WM_DESTROY,                        OnDestroy)
        MESSAGE_HANDLER (CM_ZOOMED,                         OnZoomed)
        if (uMsg != WM_LBUTTONDOWN)
            CHAIN_MSG_MAP(CTuiScrollObj)
    END_TUIMSG_MAP      ()

    TMResult OnInit     (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy  (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnZoomed   (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

public:
    CTuiProgressImpl();
    ~CTuiProgressImpl();

private:
    void Init();
    void RenderFore(IGXApp* gapp, GXDips nLen, CTuiRect& rc);

    // CTuiWgt
    virtual void DoPrintClient(IGXApp* gapp, TMLParam lParam) tm_override;

    // IFrameSource
    virtual TMUInt32 FSGetLayout() tm_override { return IsStyleVertical() ? LAYOUT_VSCROLL : LAYOUT_HSCROLL; }

    // CTuiObject
#ifdef TUI_EDIT_MODE
    TMBool EditorMatchID(TMUInt32 n) tm_override
    {
        if (n == m_sbPic.m_pBtn.GetObjectID())
            return true;
        return CTuiScrollObj::EditorMatchID(n);
    }
#endif // TUI_EDIT_MODE

};


TM_END_NAMESPACE


#endif // UIPROGRESS_H
