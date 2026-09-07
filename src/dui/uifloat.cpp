/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2017.11.30
 *
 *  Copyright (C) 2017 miragekiller
 */

#include "uifloat.h"
#include "uiwindow_win_theme.h"
#include "platform.h"
#include "uidummy.h"
#include "../tmgdi.h"


TM_BGN_NAMESPACE


/////////////////////////////////////////////////////////
// CTuiFloatImpl
TM_REGISTER_CLASS1(CTuiFloat);

CTuiFloatImpl::CTuiFloatImpl()
{
    m_dwStyle |= SS_WINDOWBKONHC;
    m_dwPriStyle |= DM_PRI_USEWINDOWRECT;
}

TMResult CTuiFloatImpl::OnCreate(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
    if (Edit_IsObjectInDocMode(this))
        return 0;
    
    m_pTheme = ITheme::GetThemeCurrent(this);

    // float no wm_paint, so need dirty message.
    Reset_WS_PRI_DIRTY();
    PostDirty();

    return 0;
}

TMResult CTuiFloatImpl::OnEnable(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    IDummy::SyncEnableWindow(this, m_hWnd, wParam, true);
    bHandled = false;
    return 0;
}

TMResult CTuiFloatImpl::OnLanguageChanged(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (m_hWnd)
        WIN::SetRTLOnLanguageChanged(m_hWnd, lParam);
    CTuiWgt::Invalidate();
    bHandled = false;
    return 0;
}

TMResult CTuiFloatImpl::OnWindowPosChanged(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (CheckVisible(false))
    {
        this->PostDirty();// SetDirtyAndInvalidate();
        RealInvalidate(true, false);
    }
    bHandled = false;
    return 0;
}

TMResult CTuiFloatImpl::OnGetThemeType(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    return (TMResult)m_pTheme->GetType();
}

TMResult CTuiFloatImpl::OnDirty(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (!Is_WS_PRI_DIRTY())
        return 0;
    Reset_WS_PRI_DIRTY();

    struct CDummyWindow : public CDummyWindowBase
    {
        CDummyWindow(TMHWnd hWnd) { m_wnd = hWnd; }
        virtual void DestroyDummy(EUnbindDummy eReason) tm_override {}
    };
    CDummyWindow wnd(GetOverlayHWND());
    wnd.SyncWin(this, true);
    //DoPrint(_gxApp, 0);
    ::InvalidateRect(GetOverlayHWND(), 0, 0);
    return 0;
}

// CTuiWgt
TMHWnd CTuiFloatImpl::GetOverlayHWND()
{
    if (m_hWnd)
        return m_hWnd;
    
    if (IsLifeInvalid())
        return 0;
    
    CRefWindow::Recreate(GetHWND(), CRefWindowT<CTuiFloatImpl>::rcDefault, 0, WS_POPUP, GetThemeExStyle());
    DbgAssert(m_hWnd);
    m_pTheme->Enable(this, m_hWnd, true);
    AddFlipChain();

#ifdef TM_DBG
    CString str(CTuiWgt::GetWindowText());
    CString str2;
    str2.Format(L"%d", GetObjectID());
    str += str2;
    ::SetWindowText(GetOverlayHWND(), str);
#endif
    
    WIN::SetWinProp(m_hWnd, WIN_PROP_TYPE_FLOAT);
    
    SetHook();
    return m_hWnd;
}

void CTuiFloatImpl::Reclaim()
{
    CTuiLayerWnd::Reclaim();

    if (Edit_IsObjectInDocMode(this))
        return;

    if (m_hWnd)
    {
        CImage img;
        img.CreateIndirect(1, 1, eGXFormat32A);
        SIZE sz = { 1, 1 };
        POINT pt = { 0, 0 };
        UpdateLayeredWin(m_hWnd, 0, &sz, CImageDC(img), pt, m_alpha);
    }
}

void CTuiFloatImpl::DoPrint(IGXApp* gapp, TMLParam lParam)
{
    if ((lParam & PRF_FLOAT) && (eRenderBkOk == RenderScene(gapp)))
    {
        if (gapp->ClipAndPush(m_rcWnd, false, eGXPropClip))
        {
            m_txBuffer->Blt(m_rcWnd, CRect(0,0,0,0), GX_BLT_DFTDSTSRCWH, GetAlpha());
            gapp->Pop();
        }
    }
    return;
}

void CTuiFloatImpl::OnSetLife(TMUInt32 life)
{
    if (life == WS_PRI_LIFE_DESTROYING)
    {
        if (m_hWnd)
        {
            ::DestroyWindow(m_hWnd);
            m_hWnd = 0;
        }
    }
    CTuiWgt::OnSetLife(life);
}

// CTuiLayerWnd
void CTuiFloatImpl::CreateBkBuffer(IGXApp* gapp, const CSize& sz)
{
    if (Edit_IsObjectInDocMode(this)) // m_pTheme->IsThemeLayered() || 
        m_txBuffer.CreateRenderTarget(sz.cx, sz.cy, eGXFormat32A, gapp);
    else
        m_txBuffer.CreateSwapChain(GetOverlayHWND(), sz.cx, sz.cy, eGXFormat32A, gapp);
}

TMBool CTuiFloatImpl::CheckVisible(TMBool bUpdateNow)
{
    if (Edit_IsObjectInDocMode(this))
        return false;

    TMBool bTarget = IsAllVisible(true, true) && !m_rcWnd.IsRectEmpty();
    if (bUpdateNow || !bTarget)
    {
        CWindowAT wnd(m_hWnd);
        TMBool bSelf = m_hWnd && wnd.IsSelfVisible();
        if (bTarget && !bSelf)
            wnd.ShowWindow(SW_SHOWNOACTIVATE);
        else if (!bTarget && bSelf && m_hWnd)
            wnd.ShowWindow(SW_HIDE);
    }
    return bTarget;
}

// CRefWindowT
LRESULT CTuiFloatImpl::WndProc(TMUInt uMsg, TMWParam wParam, TMLParam lParam)
{
    LOG_MSG_FOR_SPY(m_hWnd, this, uMsg, wParam, lParam);
//     DbgTrace(L"CTuiFloatImpl::_WNDPROC(%s)", DbgFormatMessage(uMsg, wParam, lParam).c_str());

    if (GetView()->DoPreTranslateAppMessage(this, m_hWnd, uMsg, wParam, lParam, __msgLog.lr))
        return __msgLog.lr;

    DbgAssert(uMsg != WM_SETFOCUS);
    DbgAssert(uMsg != WM_ACTIVATE);

    if (WM_MOUSEACTIVATE == uMsg)
        return __msgLog.lr = WIN::OnMouseActivate(m_hWnd, GetHWND(), uMsg, wParam, lParam);
    else if (WM_GETOBJECT == uMsg)
    {
        if (WIN::OnGetObject(this, wParam, lParam, __msgLog.lr))
            return __msgLog.lr;
    }
    else if (WM_PAINT == uMsg)
    {
        CTuiFloatImpl* p = this;
        if (p->PaintToScreen())
        {
            if (Is_WS_PRI_DIRTY())
            {
                Reset_WS_PRI_DIRTY();
                CheckVisible(true);
            }
        }

        return __msgLog.lr;
    }
    else if (WM_SETCURSOR == uMsg)
        return __msgLog.lr = 1;
    else if (WM_MOUSEWHEEL == uMsg || WM_CONTEXTMENU == uMsg // WM_SETCURSOR == uMsg ||
        || (WM_MOUSELEAVE == uMsg && !GetView()->GetCapture()))
    {
        if (GetView()->DispatchAppMessage(this, m_hWnd, uMsg, wParam, lParam, __msgLog.lr))
            return __msgLog.lr;
    }
    else if (m_pTheme->DefProc(this, m_hWnd, uMsg, wParam, lParam, __msgLog.lr))
        return __msgLog.lr;
    else if (uMsg == WM_NCDESTROY)
    {
        RemoveHook();
        ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
        CTuiWgt::DestroyWindow();
        return __msgLog.lr = 0;
    }

    CView* pView = GetView();
    if (pView->IsBasicMouseMessage(uMsg))
    {
        if (WM_MOUSEMOVE == uMsg)
            WIN::TrackMouseLeave(m_hWnd, true);

        CPoint pt(lParam);
        ::MapWindowPoints(m_hWnd, GetHWND(), &pt, 1);
        //         DbgTrace("MapWindowPoints:%d,%d", pt);
        pView->DispatchBasicMouseMessage(this, uMsg, wParam, pt);
    }

    return __msgLog.lr = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

// CHookCallWndRet
TMBool CTuiFloatImpl::OnCallWndRetProc(CWPRETSTRUCT* ps)
{
    if (ps->message == WM_WINDOWPOSCHANGED && CWndWindowPos::IsMoveOrSize(ps->lParam))
    {
        if (IDummy::SyncIsSelfOrOwneredWindow(this, ps->hwnd))
            this->PostDirty();// SetDirtyAndInvalidate();
    }
    else if (WM_ENABLE == ps->message)
    {
        if (IDummy::SyncIsSelfOrOwneredWindow(this, ps->hwnd))
            IDummy::SyncEnableWindow(this, m_hWnd, ps->wParam, true);
    }

    return false;
}



TM_END_NAMESPACE



