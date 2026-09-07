/*
 *  @file
 *  @brief  track bar
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2019.11.22
 *
 *  Copyright (C) 2019 miragekiller
 */

#pragma once

#ifndef UITRACKBAR_H
#define UITRACKBAR_H

#include "uisys.h"


TM_BGN_NAMESPACE


////////////////////////////////////////////////////////////////////////////////
// CTuiTrackBarImpl
class TM_NO_VTABLE CTuiTrackBarImpl
    : public CTuiScrollObj
{
private:
    struct CSBPic : public ITuiPic
    {
        CPicPtr         m_pLeft;
        CPicPtr         m_pRight;
        CPicPtr         m_pBtn;

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
    int             m_nBtnPicWidth;

public:
    TM_BGN_MASK_SYMBOL2 (CSymbol_Style, CSymbol_WsStyle)
        TM_MASK_SYMBOL  (PBS_VERTICAL)
        TM_MASK_SYMBOL  (SBS_HTML)
        TM_MASK_SYMBOL  (SBS_SWAPUPDOWNKEY)
        TM_MASK_SYMBOL  (BS_DRAWFOCURECT)
        TM_MASK_BREAK   ()
    TM_END_MASK_SYMBOL  ()

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_MASK   ("Style", m_dwStyle, CSymbol_Style)
        OBJ_PROP_PIC    ("LeftPic", m_sbPic.m_pLeft)
        OBJ_PROP_PIC    ("RightPic", m_sbPic.m_pRight)
        OBJ_PROP_PIC    ("BtnPic", m_sbPic.m_pBtn)
        OBJ_PROP_INT    ("BtnPicWidth", m_nBtnPicWidth)
        CHAIN_OBJ_PROP  (CTuiScrollObj)
    END_OBJ_PROP_MAP    ()

public:
    BEGIN_TUIMSG_MAP    (CTuiTrackBarImpl)
        MESSAGE_HANDLER (WM_NCCREATE,           OnInit)
        MESSAGE_HANDLER (WM_SIZE,               OnInit)
        MESSAGE_HANDLER (WM_DESTROY,            OnDestroy)
        MESSAGE_HANDLER (CM_ZOOMED,             OnZoomed)
        MESSAGE_HANDLER (WM_KEYDOWN,            OnKeyDown)
        MESSAGE_HANDLER (WM_LBUTTONDOWN,        OnLButtonDown)
        MESSAGE_HANDLER (WM_SETFOCUS,           OnInvalidate)
        MESSAGE_HANDLER (WM_KILLFOCUS,          OnInvalidate)
        MESSAGE_HANDLER (TBM_SETLEFTPIC,        OnSetLeftPic)
        MESSAGE_HANDLER (TBM_SETRIGHTPIC,       OnSetRightPic)
        MESSAGE_HANDLER (TBM_SETTRCKBTNPIC,     OnSetTrackBtnPic)
        CHAIN_MSG_MAP   (CTuiScrollObj)
    END_TUIMSG_MAP      ()

    TMResult OnInit     (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy  (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnZoomed   (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnSetLeftPic(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnSetRightPic(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnSetTrackBtnPic(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

public:
    CTuiTrackBarImpl ();
    ~CTuiTrackBarImpl ();

private:
    void Init();

    // CTuiWgt
    virtual void DoPrintClient(IGXApp* gapp, TMLParam lParam) tm_override;

    // IFrameSource
    virtual TMUInt32 FSGetLayout() tm_override { return IsStyleVertical() ? LAYOUT_VSCROLL : LAYOUT_HSCROLL; }

    // CTuiObject
#ifdef TUI_EDIT_MODE
    TMBool EditorMatchID(TMUInt32 n) tm_override
    {
        if (n == m_sbPic.m_pBtn.GetObjectID() || n == m_sbPic.m_pLeft.GetObjectID() || n == m_sbPic.m_pRight.GetObjectID())
            return true;
        return CTuiScrollObj::EditorMatchID(n);
    }
#endif // TUI_EDIT_MODE
};


TM_END_NAMESPACE


#endif // UITRACKBAR_H
