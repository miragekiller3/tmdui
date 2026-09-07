/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.03.26
 *
 *  Copyright (C) 2006 miragekiller
 */

#include "uiscrollbox.h"

TM_BGN_NAMESPACE

TM_REGISTER_CLASS1 (CTuiScrollBox);

CTuiScrollBoxImpl::CTuiScrollBoxImpl ()
    : m_nLineV (20)
    , m_nLineH (20)
    , m_nPageV (0)
    , m_nPageH (0)
{
    SetAutoHScroll();
    SetAutoVScroll();

//    m_dwAdvStyle |= (WS_ADV_AUTOVSCROLLBAR | WS_ADV_AUTOHSCROLLBAR);
    m_dwPriStyle |= WS_PRI_IS_SCROLLBOX;
}

TMResult CTuiScrollBoxImpl::OnCreate (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    Update();
    bHandled = IsLifeDestroyed();
    return 0;
}

TMResult CTuiScrollBoxImpl::OnScrollTo(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    TMResult lr = 0; 
    CTuiWgt::ProcessWindowMessage(this, uMsg, wParam, lParam, lr, 0);

    CTuiWgt* pChild = GetWindow(GW_CHILD);
    if (lr && pChild)
    {
        GXDips l = GetScrollLeft();
        GXDips t = GetScrollTop();
        pChild->SetWindowPos(0, m_rcClient.Left() - l, m_rcClient.Top() - t, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    }
    return lr;
}

TMResult CTuiScrollBoxImpl::OnMsgBubble(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    TUIMSG* p = (TUIMSG*)(lParam);
    if (p->message != WM_KEYDOWN)
        return bHandled = false;
    if (p->hWnd != this && p->hWnd != GetChild() && !GetChild()->IsChild(p->hWnd))
        return bHandled = false;
    return bHandled = OnKeyDown(p->wParam);
}

TMResult CTuiScrollBoxImpl::OnChildSetFocus(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    bHandled = false;
    if (pMsg->lParam != eFocusReasonTab)
        return 0;
    CTuiWgt* pChild = GetChild();
    if (!pChild || !pChild->IsChild(pMsg->hWnd))
        return 0;
    CTuiRect rc(pChild->GetWindowRect());
    rc &= pMsg->hWnd->GetWindowRect();
    this->ScrollToVisible(rc, true, true);
    return 0;
}

TMBool CTuiScrollBoxImpl::OnKeyDown(int key)
{
    key = GetModule()->GetLocal().ConvertKey(key);

    switch (key)
    {
    case VK_LEFT:
        return SendScrollToMessageX(GetScrollLeft() - GetHScrollLine());

    case VK_RIGHT:
        return SendScrollToMessageX(GetScrollLeft() + GetHScrollLine());

    case VK_UP:
        return SendScrollToMessageY(GetScrollTop() - GetVScrollLine());
        
    case VK_DOWN:
        return SendScrollToMessageY(GetScrollTop() + GetVScrollLine());
        
    case VK_HOME:
        return SendScrollToMessageY(0);
        
    case VK_END:
        return SendScrollToMessageY(GetScrollMaxTop());
        
    case VK_PRIOR: // page up
        return SendScrollToMessageY(GetScrollTop() - GetVScrollPage());
        
    case VK_NEXT: // page down
        return SendScrollToMessageY(GetScrollTop() + GetVScrollPage());

    default:
        return false;
    }
}

void CTuiScrollBoxImpl::Update()
{
    CTuiWgt* pChild = GetChild();
    if (!pChild)
        return;
    
    GXDips w = (pChild->IsAlignClient() || pChild->IsAnchorFrame()) ? 0 : pChild->GetWidth();
    GXDips h = (pChild->IsAlignClient() || pChild->IsAnchorVFrame()) ? 0 : pChild->GetHeight();
    GXDips l = m_rcClient.Left() - pChild->m_rcWnd.Left();
    GXDips t = m_rcClient.Top() - pChild->m_rcWnd.Top();
    Ref _ref(this);
    this->SendScrollRangeMessage(&w, &h);
    this->SendScrollToMessage(&l, &t);
}


TM_END_NAMESPACE

