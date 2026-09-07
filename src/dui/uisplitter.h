/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.02.13
 *
 *  Copyright (C) 2006 miragekiller
 */

#pragma once

#ifndef UISPLITTER_H
#define UISPLITTER_H

#include "uiwidget.h"


TM_BGN_NAMESPACE

////////////////////////////////////////////////////////////////////////////////
// CTuiSplitter
class TM_NO_VTABLE CTuiSplitterImpl : public CTuiWgt
   
{
private:
    TMLParam  m_lPrev;
    TMLParam  m_lDown;
    int     m_nMin;     // min left value by manager;
    int     m_nMax;     // max left value by manager;

public:
    enum
    {
        SS_HORIZONTAL       = 0x01,
        SS_LAYOUTSIBLINGS   = 0x02,
        SS_IMMEDIATELY      = 0x04,
    };

    TM_BGN_MASK_SYMBOL2 (CSymbol_Style, CSymbol_WsStyle) //, SS_HORIZONTAL)
        TM_MASK_SYMBOL  (SS_HORIZONTAL)
        TM_MASK_SYMBOL  (SS_IMMEDIATELY)
        TM_MASK_SYMBOL  (SS_LAYOUTSIBLINGS)
        TM_MASK_BREAK   ()
    TM_END_MASK_SYMBOL  ()

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_MASK   ("Style", m_dwStyle, CSymbol_Style)
        OBJ_PROP_INT    ("Min", m_nMin)
        OBJ_PROP_INT    ("Max", m_nMax)
        CHAIN_OBJ_PROP  (CTuiWgt)
    END_OBJ_PROP_MAP    ()

public:
    BEGIN_TUIMSG_MAP    (CTuiSplitterImpl)
        MESSAGE_HANDLER (WM_LBUTTONDOWN,    OnLButtonDown)
        MESSAGE_HANDLER (WM_LBUTTONUP,      OnLButtonUp)
        MESSAGE_HANDLER (WM_MOUSEMOVE,      OnMouseMove)
        MESSAGE_HANDLER (CM_MOUSEENTRY,     OnMouseEntry)
        MESSAGE_HANDLER (WM_MOUSELEAVE,     OnMouseLeave)
        MESSAGE_HANDLER (WM_SETCURSOR,      OnSetCursor)
        MESSAGE_HANDLER (WM_SETFOCUS,       OnInvalidate)
        MESSAGE_HANDLER (WM_KILLFOCUS,      OnInvalidate)
        MESSAGE_HANDLER (WM_KEYDOWN,        OnKeyDown)
        MESSAGE_HANDLER (CM_SETPOS,         OnMoved)
        MESSAGE_HANDLER (CM_SETRANGE,       OnSetRange)
        CHAIN_MSG_MAP   (CTuiWgt)
    END_TUIMSG_MAP      ()

    TMResult OnLButtonDown  (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnLButtonUp    (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnMouseMove    (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnSetCursor    (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnKeyDown      (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnMoved        (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnSetRange     (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnMouseEntry   (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnMouseLeave   (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

public:
    CTuiSplitterImpl();

private:
    void GetNewOffset(TMLParam lParam, int& x, int& y);
    void DrawGhostBar(TMLParam lParam);
    void MoveTo(TMLParam lParam);

    // CTuiWgt
    virtual void DoPrintClient(IGXApp* gapp, TMLParam lParam);
};


TM_END_NAMESPACE


#endif // UISPLITTER_H
