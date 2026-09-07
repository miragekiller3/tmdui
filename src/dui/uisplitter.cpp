/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.02.13
 *
 *  Copyright (C) 2006 miragekiller
 */

#include "uisplitter.h"
#include "view.h"
#include "tmkeyboard.h"


TM_BGN_NAMESPACE


////////////////////////////////////////////////////////////////////////////////
// CTuiSplitter


//TM_REGISTER_CLASS2 (CTuiSplitter, _CS("splitter"));
TM_REGISTER_CLASS1(CTuiSplitter)

CTuiSplitterImpl::CTuiSplitterImpl()
    : m_lDown(0)
    , m_lPrev(0)
    , m_nMin(0)
    , m_nMax(0)
{
    m_dwStyle |= WS_TABSTOP;
}

TMResult CTuiSplitterImpl::OnLButtonDown(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    SetFocus(eFocusReasonMouse);
    SetCapture();
    m_lDown = m_lPrev = lParam;
    DrawGhostBar(lParam);
    Invalidate();
    return 0;
}

TMResult CTuiSplitterImpl::OnLButtonUp(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (IsCapture())
    {
        DrawGhostBar(m_lPrev);    
        MoveTo(lParam); 
        GetView()->ReleaseCapture();
    }

    return 0;
}

TMResult CTuiSplitterImpl::OnMouseMove(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (IsCapture())
    {
        if (m_dwStyle & SS_IMMEDIATELY)
        {
            MoveTo(lParam);
            m_lDown = lParam;
        }
        else
        {
            DrawGhostBar(m_lPrev);
            m_lPrev = lParam;
            DrawGhostBar(m_lPrev);
        }
    }
    return 0;
}

TMResult CTuiSplitterImpl::OnSetCursor(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    OSSetCursor((m_dwStyle & SS_HORIZONTAL) ? CCursorAT::cursor_SPLITH() : CCursorAT::cursor_SPLITV());
    return true;
}

TMResult CTuiSplitterImpl::OnKeyDown(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    int d = 0;
    wParam = GetModule()->GetLocal().ConvertKey(wParam);

    switch (wParam)
    {
    case VK_UP:     tm_fallthrough;
    case VK_LEFT:   d = -1; break;
        
    case VK_DOWN:   tm_fallthrough;
    case VK_RIGHT:  d = 1; break;

    default:
        return bHandled = false;
    }

    CTuiPoint pt(m_lDown);
    int shift = CKeyInput(wParam, lParam).IsModifiersShift() ? 10 : 1;
    d *= shift;
    pt.Offset(d, d);
    //pt.x += d;
    //pt.y += d;
    lParam = pt.ToLPARAM();// MAKELPARAM(pt.x, pt.y);
    MoveTo(lParam);

    return true;
}

// CTuiWgt
void CTuiSplitterImpl::DoPrintClient(IGXApp* gapp, TMLParam lParam)
{
    if (m_pBk)
    {
        ITuiPic* p = IsHighContrast() ? (ITuiPic*)&GetModule()->m_aPrePic.m_picDft_BUTTON : m_pBk;
        p->Render(GetButtonState(), 0, m_rcClient, 0, gapp);
    }
}

TMResult CTuiSplitterImpl::OnSetRange(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_nMin = TM_LOWORD(lParam);
    m_nMax = TM_HIWORD(lParam);
    return 0;
}

TMResult CTuiSplitterImpl::OnMouseEntry(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    Invalidate();
    bHandled = FALSE;
    return 0;
}

TMResult CTuiSplitterImpl::OnMouseLeave(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    Invalidate();
    bHandled = FALSE;
    return 0;
}

TMResult CTuiSplitterImpl::OnMoved(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (!m_lDown)
    {  
        GetParent()->ChangeFrame(TMTrue);
        CTuiRect rect;
        GetClientRect(&rect);
        m_lDown = rect.TopLeft().ToLPARAM();// MAKELPARAM(rect.Left, rect.top);
    }
    int nPos = lParam;
    if (m_dwStyle & SS_HORIZONTAL)
    {
        lParam = MAKELPARAM(0, nPos);

    }
    else
    {
        lParam = MAKELPARAM(nPos, 0);
    }
    MoveTo(lParam);
    return 0;
}

void CTuiSplitterImpl::MoveTo(TMLParam lParam)
{
    int x, y;
    GetNewOffset (lParam, x, y);
 
    Ref pPrev(_prev);// GetWindow (GW_HWNDPREV));
    if (pPrev)
    {
        CTuiRect rc = pPrev->GetBoundRect();
        rc.KeepLOffsetR(x);
        rc.KeepTOffsetB(y);
        pPrev->SetWindowPos(0, &rc, SWP_NOZORDER | SWP_NOLAYOUTSIBLING);
        pPrev->GetWindowRect(&rc);
    }
    
    Ref pNext(_next);// GetWindow (GW_HWNDNEXT));
    if (pNext)
    {
        CTuiRect rc = pNext->GetBoundRect();
        rc.KeepROffsetL(x);
        rc.KeepBOffsetT(y);
        pNext->SetWindowPos(0, &rc, SWP_NOZORDER | SWP_NOLAYOUTSIBLING);
    }
 
    {
        CTuiRect rc = GetBoundRect();
        rc.OffsetRect(x, y);
        SetWindowPos(0, &rc, SWP_NOZORDER | SWP_NOLAYOUTSIBLING);
//        MoveWindow (&rc, true);
    }
//     GetParent()->ChangeFrame(false);

    Notify(HDM_SETHOTDIVIDER);
//     if ((m_dwStyle & SS_LAYOUTSIBLINGS) && _parent)
//     {
//         GetParent()->SetWindowPos (
//             0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
// 
//     
//     }
    m_lDown = 0;
}

static void _CTuiSplitterImpl_CalcOffsetMin(int l, int r, int nMin, int& dx)
{
    DbgAssert(dx < 0);
    int rr = r + dx;
    tm_setmax(rr, l);
    if (nMin)
        tm_setmax(rr, nMin);
    dx = rr - r;
    tm_setmin(dx, 0);
}

static void _CTuiSplitterImpl_CalcOffsetMax(int l, int r, int nMax, int& dx)
{
    DbgAssert(dx > 0);
    int ll = l + dx;
    tm_setmin(ll, r);
    if (nMax)
        tm_setmin(ll, nMax);
    dx = ll - l;
    tm_setmax(dx, 0);
}

void CTuiSplitterImpl::GetNewOffset(TMLParam lParam, int& x, int& y)
{
    CTuiWgt* pPrev = GetWindow(GW_HWNDPREV);
    CTuiWgt* pNext = GetWindow(GW_HWNDNEXT);

    if (m_dwStyle & SS_HORIZONTAL)
    {
        x = 0;
        y = (short)HIWORD(lParam) - (short)HIWORD(m_lDown);
        
        if (y < 0)
        {
            int nTop = pPrev ? pPrev->GetTop() + pPrev->m_bdMargin.bottom
                : _parent->m_rcClient.Top() + _parent->m_bdPadding.top;
            int nBottom = GetTop();
            _CTuiSplitterImpl_CalcOffsetMin(nTop, nBottom, m_nMin, y);
        }
        else if (y > 0)
        {
            int nTop =  GetBottom();
            int nBottom = pNext ? pNext->GetBottom() + pNext->m_bdMargin.top
                : _parent->m_rcClient.Bottom() + _parent->m_bdPadding.bottom;
            _CTuiSplitterImpl_CalcOffsetMax(nTop, nBottom, m_nMax, y);
        }
    }
    else
    {
        x = (short)LOWORD(lParam) - (short)LOWORD(m_lDown);
        y = 0;

        if (pPrev && x < 0)
        {
            int nLeft = pPrev ? pPrev->GetLeft() + pPrev->m_bdMargin.right
                : _parent->m_rcClient.Left() + _parent->m_bdPadding.left;
            int nRight = GetLeft();
            _CTuiSplitterImpl_CalcOffsetMin(nLeft, nRight, m_nMin, x);
//             DbgTrace ("pPrev && x < 0:%d", x);
        }
        else if (pNext && x > 0)
        {
            int nLeft =  GetRight();
            int nRight = pNext ? pNext->GetRight() + pNext->m_bdMargin.left
                : _parent->m_rcClient.Right() + _parent->m_bdPadding.right;
            _CTuiSplitterImpl_CalcOffsetMax(nLeft, nRight, m_nMax, x);
//             DbgTrace ("pNext && x > 0:%d", x);
        }
    }
}

void CTuiSplitterImpl::DrawGhostBar(TMLParam lParam)
{
    if (m_dwStyle & SS_IMMEDIATELY)
        return;

    CTuiRect rect = GetBoundRect();
    int x, y;
    GetNewOffset (lParam, x, y);
    rect.OffsetRect(x, y);

    CRect rcScreen;
    this->ClientToScreen(rect, rcScreen);
    CScreenDCWrite dc(rcScreen);
    dc.InvertRect(rcScreen);
// 
//     TMHWnd hWnd = GetHWND();
//     CDCHandle dc(GetDCEx(hWnd, 0, DCX_CACHE));
//     CBrush brush(CDCHandle::GetHalftoneBrush());
//     CBrushHandle brushOld = dc.SelectBrush(brush);
//     dc.PatBlt(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, PATINVERT);
//     dc.SelectBrush(brushOld);
//     ::ReleaseDC(hWnd, dc);
}


TM_END_NAMESPACE
