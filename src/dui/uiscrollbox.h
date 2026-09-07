/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.03.26
 *
 *  Copyright (C) 2006 miragekiller
 */

#pragma once

#ifndef UISCROLLBOX_H
#define UISCROLLBOX_H

#include "uiwidget.h"


TM_BGN_NAMESPACE


////////////////////////////////////////////////////////////////////////////////
// CTuiScrollBoxImpl
class TM_NO_VTABLE CTuiScrollBoxImpl
    : public CTuiWgt // ScrollWin <CTuiScrollBoxImpl>
{
protected:
    int   m_nLineV;
    int   m_nLineH;
    int   m_nPageV;
    int   m_nPageH;

public:
    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_INT    ("VScrollLine", m_nLineV)
        OBJ_PROP_INT    ("HScrollLine", m_nLineH)
        OBJ_PROP_INT    ("VScrollPage", m_nPageV)
        OBJ_PROP_INT    ("HScrollPage", m_nPageH)
        CHAIN_OBJ_PROP  (CTuiWgt)
    END_OBJ_PROP_MAP    ()

    BEGIN_TUIMSG_MAP    (CTuiScrollBoxImpl)
        MESSAGE_HANDLER (WM_CREATE, OnCreate)
        MESSAGE_HANDLER (CM_SCROLLTO, OnScrollTo)
        MESSAGE_HANDLER (CM_MSGBUBBLE, OnMsgBubble)
        ROUTE_HANDLER   (WM_SETFOCUS, OnChildSetFocus)
        CHAIN_MSG_MAP   (CTuiWgt)
    END_TUIMSG_MAP      ()

    TMResult OnCreate(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnScrollTo(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnMsgBubble(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnChildSetFocus(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);

protected:
    // CScrollImpl
    virtual GXDips GetVScrollLine () const { return m_nLineV; }
    virtual GXDips GetHScrollLine () const { return m_nLineH; }
    virtual GXDips GetVScrollPage () const { return m_nPageV ? m_nPageV : m_rcClient.Bottom() - m_rcClient.Top(); }
    virtual GXDips GetHScrollPage () const { return m_nPageH ? m_nPageH : m_rcClient.Right() - m_rcClient.Left(); }

//     GXDips GetScrollWidth ()
//     {
//         CTuiWgt* pChild = GetChild();
//         if (pChild && !pChild->IsAlignClient() && !pChild->IsAnchorFrame())
//             return pChild->GetWidth();
//         else
//             return 0;
//     }
//     
//     GXDips GetScrollHeight ()
//     {
//         CTuiWgt* pChild = GetChild();
//         if (pChild && !pChild->IsAlignClient() && !pChild->IsAnchorVFrame())
//             return pChild->GetHeight();
//         else
//             return 0;
//     }
// 
//     TMBool _GetScrollInfo(SCROLLINFO& ih, SCROLLINFO& iv)
//     {
//     }

    TMBool OnKeyDown(int key);


public:
    CTuiScrollBoxImpl ();
    void Update();
};

TM_END_NAMESPACE


#endif // UISCROLLBOX_H
