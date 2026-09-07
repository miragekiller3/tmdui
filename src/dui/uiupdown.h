/*
 *  @file
 *  @brief  updown
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2019.11.26
 *
 *  Copyright (C) 2019 miragekiller
 */

#pragma once

#ifndef UIUPDOWN_H
#define UIUPDOWN_H

#include "uisys.h"


TM_BGN_NAMESPACE


////////////////////////////////////////////////////////////////////////////////
// CTuiUpDownImpl
class TM_NO_VTABLE CTuiUpDownImpl
    : public CTuiScrollObj
{
private:
    struct CSBPic : public ITuiPic
    {
        CPicPtr         m_pUp;
        CPicPtr         m_pDown;
        
        TM_UNUSED_ADDREF_RELEASE();

        TM_BGN_INTERFACE_MAP(ITuiPic)
        TM_END_INTERFACE_MAP()
            
        // ITuiPic
        virtual TMUInt32 GetObjectID() const tm_override { return 0; }
        virtual void Render(int nState, TMHDC, const CTuiRect& rc, WPRENDERFX* fx, IGXApp* gapp) tm_override {}
        virtual void RenderSB(const SCROLLSTATE& state, const CTuiRect& rc, WPRENDERFX_SB* fx, IGXApp* gapp) tm_override;
    };
    friend CSBPic;
    
    CSBPic          m_sbPic;
    
public:

    TM_BGN_MASK_SYMBOL2 (CSymbol_Style, CSymbol_WsStyle)
//         TM_MASK_SYMBOL  (UDS_ALIGNLEFT)
//         TM_MASK_SYMBOL  (UDS_ALIGNRIGHT)
//         TM_MASK_SYMBOL  (UDS_ARROWKEYS) 
        TM_MASK_SYMBOL  (UDS_AUTOBUDDY)
        TM_MASK_SYMBOL  (UDS_HORZ)
//         TM_MASK_SYMBOL  (UDS_NOTHOUSANDS)
        TM_MASK_SYMBOL  (SBS_HTML)
        TM_MASK_SYMBOL  (BS_DRAWFOCURECT)
        TM_MASK_BREAK   ()
    TM_END_MASK_SYMBOL  ()

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_MASK   ("Style",   m_dwStyle, CSymbol_Style)
        OBJ_PROP_PIC    ("UpPic",   m_sbPic.m_pUp)
        OBJ_PROP_PIC    ("DwonPic", m_sbPic.m_pDown)
        CHAIN_OBJ_PROP  (CTuiScrollObj)
    END_OBJ_PROP_MAP    ()

public:
    BEGIN_TUIMSG_MAP    (CTuiUpDownImpl)
        MESSAGE_HANDLER (WM_NCCREATE,                       OnInit)
        MESSAGE_HANDLER (WM_SIZE,                           OnInit)
        MESSAGE_HANDLER (WM_DESTROY,                        OnDestroy)
        MESSAGE_HANDLER (CM_ZOOMED,                         OnZoomed)
        MESSAGE_HANDLER (WM_LBUTTONDOWN,                    OnLButtonDown)
        CHAIN_MSG_MAP   (CTuiScrollObj)
    END_TUIMSG_MAP      ()

    TMResult OnInit     (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy  (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnZoomed   (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

public:
    CTuiUpDownImpl ();
    ~CTuiUpDownImpl ();

private:
    void Init();

    // CTuiScrollObj
    virtual HTUI GetTarget() tm_override;
    virtual int SyncNewPos(int step) tm_override;

    // CTuiWgt
    virtual void DoPrintClient(IGXApp* gapp, TMLParam lParam) tm_override;

    // IFrameSource
    virtual TMUInt32 FSGetLayout() tm_override { return IsStyleHorz() ? LAYOUT_HSCROLL : LAYOUT_VSCROLL; }

    // CTuiObject
#ifdef TUI_EDIT_MODE
    TMBool EditorMatchID(TMUInt32 n) tm_override
    {
        if (n == m_sbPic.m_pUp.GetObjectID() || n == m_sbPic.m_pDown.GetObjectID())
            return true;
        return CTuiScrollObj::EditorMatchID(n);
    }
#endif // TUI_EDIT_MODE
};


TM_END_NAMESPACE


#endif // UIUPDOWN_H
