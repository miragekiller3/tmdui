/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.07.08
 *
 *  Copyright (C) 2018 miragekiller
 */

#include "uiarrow.h"
#include "uidummy.h"


TM_BGN_NAMESPACE

////////////////////////////////////////////////////////////////////////////////
// register
TM_REGISTER_CLASS1 (CTuiArrow);



////////////////////////////////////////////////////////////////////////////////
// class CTuiArrowImpl


// target subclass proc
TMBool CTuiArrowImpl::_Proc_target(
    CTuiArrowImpl* pVoid, HTUI hWnd, TMUInt uMsg, TMWParam wParam,
    TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)
{
    DISABLE_SPY();
    lResult = CTuiWidget(hWnd).DefWindowProc(uMsg, wParam, lParam);
    if (uMsg == WM_DESTROY)
        pVoid->SetWin(0);
    else if (uMsg == WM_SIZE || uMsg == WM_MOVE || uMsg == CM_SET_PADING)
        pVoid->DirtySelf();
    return true;
}

// cont subclass proc
CTuiPoint CTuiArrowImpl::ContCalcNewPointer(TMBool bWnd)
{
    CTuiRect rc;
    m_wndTarget->GetPaddingRect(&rc);
    CTuiPoint ptTarget(rc.CenterPoint());
    this->GetPaddingRect(rc);
    CTuiPoint ptSelf(rc.CenterPoint());
    CTuiPoint ptNew;
    if (bWnd && m_wndCont->GetModule()->GetLocal().IsRTL())
        return CTuiPoint(
        m_wndCont->m_rcWnd.Right() - ptSelf.X() + ptTarget.X(),
        m_wndCont->m_rcWnd.Top()  - ptSelf.Y() + ptTarget.Y());
    else
        return CTuiPoint(
        m_wndCont->m_rcWnd.Left() - ptSelf.X() + ptTarget.X(),
        m_wndCont->m_rcWnd.Top()  - ptSelf.Y() + ptTarget.Y());
}

void CTuiArrowImpl::ContMessage(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool bWnd)
{
    if (uMsg != CM_PRI_POSCHANGING)
        return;
    
    if (!this->Is_WS_VISIBLE())
        return;
    if (::IsIconic(m_wndTarget->GetHWND()))
        return;
    
    if (bWnd)
    {
        WINDOWPOS* pwp = (WINDOWPOS*)lParam;
        if (pwp->flags & SWP_HIDEWINDOW)
            return;
        if (!(pwp->flags & SWP_SHOWWINDOW) && !::IsWindowVisible(pwp->hwnd))
            return;
        CPoint ptTgt;
        CTuiPoint ptNew(ContCalcNewPointer(bWnd));
        m_wndTarget->ClientToScreen(&ptNew, 1, &ptTgt);
        CRect* rcBorder = (CRect*)wParam;
        pwp->x = ptTgt.X() - rcBorder->Left();
        pwp->y = ptTgt.Y() - rcBorder->Top();
    }
    else
    {
        TUIWINDOWPOS* pwp = (TUIWINDOWPOS*)lParam;
        CTuiPoint ptNew(ContCalcNewPointer(bWnd));
        pwp->x = ptNew.X();
        pwp->y = ptNew.Y();
    }
    Reset_WS_PRI_DIRTY();
}

// CHookCallWndRet
TMBool CTuiArrowImpl::OnCallWndRetProc(CWPRETSTRUCT* ps)
{
    if (ps->message == WM_WINDOWPOSCHANGED)
    {
        if (IDummy::SyncIsSelfOrOwneredWindow(m_wndTarget, ps->hwnd)) // IDummy::IsOwneredWindow(this, ps->hwnd)
            DirtySelf();

//             SetDirtyAndNotify();
    }
//     else if (ps->message == WM_SIZE || ps->message == WM_MOVE) // support auto size dialog
//     {
//         if (IDummy::IsOwneredWindow(this, ps->hwnd))
//             DirtySelf();
//     }
    return false;
}

TMResult CTuiArrowImpl::OnSetWindow(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    SetWin((HTUI)wParam);
    return 1;
}

TMResult CTuiArrowImpl::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    SetWin(0);
    bHandled = false;
    return 0;
}

TMResult CTuiArrowImpl::OnRefresh(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    DirtySelf();
    bHandled = false;
    return 0;
}

void CTuiArrowImpl::SetWin(HTUI hWnd)
{
    if (m_wndTarget)
    {
        Unsubclass_target(m_wndTarget); // CTuiWidget(m_wndTarget).UnsubclassWindow(CWidgetMsgDelegate(this, &_TargetProc));
        if (!m_wndCont)
            ;
        else if (m_nIDSource)
            Unsubclass_cont(m_wndCont); // CTuiWidget(m_wndCont).UnsubclassWindow(CWidgetMsgDelegate(this, &_ContProc));
        else
        {
            DbgAssert(m_wndCont->Is_WS_PRI_IS_WINDOW());
            Unsubclass_contW(m_wndCont); // CTuiWindow(m_wndCont).UnsubclassMgr(CWindowMsgDelegate(this, &_ContWndProc));
        }
        m_wndCont.Release();
        RemoveHook();
    }
    m_wndTarget = hWnd;
    if (m_wndTarget)
    {
        DbgAssert(!(hWnd->GetOwner()->GetStyle() & WS_CHILD));
        Subclass_target(m_wndTarget); //CTuiWidget(m_wndTarget).SubclassWindow(CWidgetMsgDelegate(this, &_TargetProc));
        if (m_nIDSource)
        {
            hWnd = _parent;
            for (; ; hWnd = hWnd->_parent)
            {
                if (!hWnd || hWnd->Is_WS_PRI_IS_WINDOW())
                    return;
                if (hWnd->GetObjectID() == m_nIDSource || hWnd->m_dwID == m_nIDSource)
                    break;
            }
            m_wndCont = hWnd;
            Subclass_cont(m_wndCont);// CTuiWidget(m_wndCont).SubclassWindow(CWidgetMsgDelegate(this, &_ContProc));
        }
        else // bind dialog
        {
            SetHook(); // only bind dialog need hook
            m_wndCont = GetOwner();
            Subclass_contW(m_wndCont); // CTuiWindow(m_wndCont).SubclassMgr(CWindowMsgDelegate(this, &_ContWndProc));
        }
        DirtySelf();
    }
}

void CTuiArrowImpl::DirtySelf()
{
    if (!m_wndCont)
        return;
    m_wndCont->UpdateOnArrowDirty();
}

// CTuiWgt
#ifdef TUI_EDIT_MODE

CString CTuiArrowImpl::GetDebugInfo()
{
    TM::CString str;
    if (m_wndTarget)
        str.Format(L"\r\n\r\nbind HTUI:0X%X(%d)", (HTUI)m_wndTarget, m_wndTarget->GetObjectID());
    else
        str = L"\r\n\r\nbind HTUI:0";
    return CTuiWgt::GetDebugInfo() + str;
}

#endif // TUI_EDIT_MODE

TM_END_NAMESPACE
