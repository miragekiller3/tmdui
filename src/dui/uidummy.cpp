/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.12.29
 *
 *  Copyright (C) 2006 miragekiller
 */

#include "uidummy.h"
#include "view.h"
#include "platform.h"

TM_BGN_NAMESPACE

////////////////////////////////////////////////////////////////////////////////
// register
TM_REGISTER_CLASS1 (CTuiDummy);


#ifdef TM_DBG

typedef CMap<HWND, HTUI> CDummyList;
CDummyList s_aDbgDummys;

TM_INLINE void DummyDebug_Insert(HWND hWnd, HTUI p)
{
    HTUI& dummy = s_aDbgDummys[hWnd];
    DbgAssert(!dummy);
    s_aDbgDummys[hWnd] = p;
}

TM_INLINE void DummyDebug_Erase(HWND hWnd, HTUI p)
{
    if (!hWnd)
        return;
    HTUI& dummy = s_aDbgDummys[hWnd];
    DbgAssert(dummy == p);
    s_aDbgDummys.erase(hWnd);
}

#else

#   define DummyDebug_Insert(x,y)
#   define DummyDebug_Erase(x,y)

#endif // TM_DBG

////////////////////////////////////////////////////////////////////////////////
// IDummy
void IDummy::SyncWin(HTUI hWgt, TMBool bUpdateNow)
{
    DbgAssert(hWgt);
    if (!SyncIsValid())
        return;
    
    // if dummy is hide then hide window immediately.
    TMBool bWgtVisble = hWgt->IsAllVisible(true, false);
    if (!bWgtVisble)
    {
        SyncSetTargetScreenPos(0, SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE);
        return;
    }
    
    if (!bUpdateNow)
    {
//        hWgt->SetDirtyAndInvalidate();
        hWgt->PostDirty();
        return;
    }
    
    hWgt->Reset_WS_PRI_DIRTY();
    // get dummy, window rect and compare
    TMUInt32 dwFlags = SWP_NOZORDER /*| SWP_NOOWNERZORDER*/ | SWP_NOACTIVATE;

    // get screen rect
    CRectI rcWgt;
    dwFlags |= SyncGetScreenPos(hWgt, rcWgt);
    if (dwFlags & SWP_HIDEWINDOW)
    {
        SyncSetTargetScreenPos(0, dwFlags);
        return;
    }
    
    if (!SyncIsTargetVisible())
        dwFlags |= SWP_SHOWWINDOW;
    if ((dwFlags & (SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW)) == (SWP_NOMOVE | SWP_NOSIZE))
        return; // not change
    
    SyncSetTargetScreenPos(&rcWgt, dwFlags);
}

HWND IDummy::GetTargetFocus()
{
    HWND hTarget = GetTargetHWND();
    HWND hWnd = ::GetFocus();
    if (hTarget == hWnd)
        return hWnd;
    return CWindowAT(hTarget).IsChild(hWnd) ? hWnd : 0;
}

TMBool IDummy::ProcessWindowMessage(HTUI hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)
{
    if (uMsg == CM_SHOWWINDOW_P || uMsg == CM_WINDOWPOSCHANGED || uMsg == CM_SET_PADING || uMsg == CM_LANGUAGECHANGED)
        SyncWin(hWnd, SyncIsUpdateNowMode(hWnd));
    else if (uMsg == WM_ENABLE || uMsg == CM_ENABLE_P)
        SyncSetTargetEnable(hWnd->IsAllEnable(true));
//     else if (uMsg == CM_LANGUAGECHANGED)
//     {
//         HWND hWnd = GetTargetHWND();
//         if (hWnd)
//         {
// //            WIN::SetRTLOnLanguageChanged(hWnd, lParam);
//             hWgt->SetDirtyAndInvalidate();
//         }
//     }
    else if ((uMsg == WM_PRINTCLIENT || uMsg == CM_PRI_DIRTY) && hWnd->Is_WS_PRI_DIRTY())
        SyncWin(hWnd, true);
    return false;
}

void IDummy::SyncEnableWindow(HTUI hWgt, HWND hWnd, TMWParam wp, TMBool bFloat)
{
    TMBool b = wp ? hWgt->IsAllEnable(bFloat) : false;
    DbgVerify(::EnableWindow(hWnd, b));
}

TMBool IDummy::SyncIsSelfOrOwneredWindow(HTUI hWgt, HWND hOwner)
{
    HWND hMgr = hWgt->GetHWND();
    return hMgr == hOwner || ::IsChild(hOwner, hMgr);
}


/////////////////////////////////////////////////////////
// CDummyWindowBase
void CDummyWindowBase::BindHWND(TMHWnd hWnd)
{
    DbgAssert(!m_wnd);
    DbgAssert(hWnd);

    m_wnd = hWnd;
    DummyDebug_Insert(m_wnd, m_pWnd);
    m_wnd.EnableWindow(m_pWnd->IsAllEnable());
    OnZoomed(m_pWnd->GetDPI());
    SyncWin(m_pWnd, SyncIsUpdateNowMode(m_pWnd));
}

TMHWnd CDummyWindowBase::UnbindHWND()
{
    DummyDebug_Erase(m_wnd, m_pWnd);
    return m_wnd.Detach();
}

void CDummyWindowBase::OnZoomed(int nDpi)
{
    DbgAssert(m_wnd);
//     m_wnd.EnableDPIChangeResize();
//     m_wnd.SendMessage(WM_DPICHANGED, MAKEWPARAM(nDpi, nDpi), 0);
//     m_wnd.DisableDPIChangeResize();

//     CTuiWindow mgr = GetWindowMgr();
//     if (mgr)
//         mgr.SetDPI(nDpi);
}

// TMBool CDummyWindowBase::IsDetached()
// {
//     if (!m_pWnd)
//         return true;
//     CTuiDummyBase* p = static_cast<CTuiDummyBase*>(m_pWnd);
//     return this != p->m_ptrDummyImpl;
// }

void CDummyWindowBase::AttachWidget(HTUI hWnd)
{
    m_pWnd = hWnd;
    CTuiDummyBase* p = static_cast<CTuiDummyBase*>(hWnd);
    IDummy* pd = p->m_ptrDummyImpl;
    p->m_ptrDummyImpl.p = this;
    if (pd)
        pd->DestroyDummy(eUnbindDummyDefault);
}

void CDummyWindowBase::DetachWidget()
{
    if (!m_pWnd)
        return;
    CTuiDummyBase* p = static_cast<CTuiDummyBase*>(m_pWnd);
    if (this != p->m_ptrDummyImpl)
        return;
    p->m_ptrDummyImpl.p = 0;
}


// IDummy
TMBool CDummyWindowBase::SyncIsTargetVisible()
{
    return m_wnd.GetStyle() & WS_VISIBLE;
}

TMUInt32 CDummyWindowBase::SyncGetScreenPos(HTUI hWgt, CRectI& rc)
{
    // get src screen pos
    CTuiRect rcTmp;
    if (hWgt->GetPriStyle() & DM_PRI_USEWINDOWRECT)
        hWgt->GetWindowRect(&rcTmp);
    else
        hWgt->GetPaddingRect(&rcTmp);
    if (hWgt->GetModule()->GetLocal().IsRTL())
        rcTmp.SetLeftRight(rcTmp.Right(), rcTmp.Left());
    hWgt->ClientToScreen(rcTmp, rc);

    // fix bug: layered window set 0, 0 will show window old width and height.
    if (rc.Width() <= 0 || rc.Height() <= 0)
        return SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE;
    
    // get target screen pos
    DbgAssert(m_wnd);
    CRectI rcDst;
    m_wnd.GetWindowRect(&rcDst);

    // calc SetWindowPos flag
    TMUInt dwFlags = 0;
    if (rc.Width() == rcDst.Width() && rc.Height() == rcDst.Height())
    {
        dwFlags |= SWP_NOSIZE;
        if (rc.left == rcDst.left && rc.top == rcDst.top)
            dwFlags |= SWP_NOMOVE;
    }
    return dwFlags;
}

void CDummyWindowBase::SyncSetTargetScreenPos(CRect* rcSreen, TMUInt32 flag)
{
    if (!m_wnd)
        return;
    if (flag & SWP_HIDEWINDOW)
        m_wnd.ShowWindow(SW_HIDE);
    else
    {
        m_wnd.ScreenToParent(*rcSreen);
//         rcSreen->NormalizeRect();
        m_wnd.DisableDPIChangeResize();
        DbgVerify(m_wnd.SetWindowPos(0, rcSreen, flag));
        m_wnd.EnableDPIChangeResize();
        DbgTrace("SyncSetTargetScreenPos(%d,%d,%d,%d)", *rcSreen);

#if defined(TM_DBG) && !defined(TUI_TEST_MODE)
        CRect rcNow;
        m_wnd.GetWindowRectToParent(rcNow);
        DbgAssert(rcNow == *rcSreen);
        if (rcNow != *rcSreen)
            DbgVerify(m_wnd.SetWindowPos(0, rcSreen, flag));
#endif
    }
}

void CDummyWindowBase::SyncSetTargetEnable(TMBool bEnable)
{
    if (m_wnd)
        m_wnd.EnableWindow(bEnable);
}

TMBool CDummyWindowBase::ProcessWindowMessage(HTUI hWnd, TMUInt uMsg, TMWParam wParam,
    TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)
{
    if (uMsg == CM_ZOOMED)
        OnZoomed(wParam);
    else if (uMsg == CM_GETWINDOW)
        return lResult = (TMResult)m_wnd.m_hWnd, true;

//    else if (uMsg == WM_DESTROY)
//        m_pWnd = 0;
    return IDummy::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
}


/////////////////////////////////////////////////////////
// CDummyChild
CDummyChild::~CDummyChild()
{
    DbgAssert(!m_wnd);
}

void CDummyChild::BindHWND(TMHWnd hWnd)
{
    DbgAssert(!m_wnd);
    DbgAssert(hWnd);
    CDummyWindowBase::BindHWND(hWnd);
    CHookCallWnd::SetHook();
    DbgAssert(m_wnd);
}

TMHWnd CDummyChild::UnbindHWND()
{
    CHookCallWnd::RemoveHook();
    CHookGetMessageEx::RemoveHook();
    return CDummyWindowBase::UnbindHWND();
//     DbgAssert(!m_wnd);
}

struct _CDummyChild_CheckSetFocus : public CWidgetCallT<_CDummyChild_CheckSetFocus, HTUI>
{
    static void Done(HTUI hWnd)
    {
        if (hWnd->IsLifeInvalid())
            return;
        CTuiDummyBase* p = static_cast<CTuiDummyBase*>(hWnd);
        if (p->m_ptrDummyImpl && p->m_ptrDummyImpl->GetTargetFocus())
            hWnd->GetView()->CheckSetFocus(hWnd, false, false, eFocusReasonUnknown);
    }
};

TMBool CDummyChild::OnCallWndProc(CWPSTRUCT* ps)
{
    DbgAssert(m_wnd);
    if (ps->message == WM_DESTROY && ps->hwnd == m_wnd)
        DestroyDummy(eUnbindDummyDetach);
    else if (ps->message == WM_SETFOCUS || ps->message == WM_KILLFOCUS)
    {
        DbgTrace(L"CDummyChild::OnCallWndProc(self:%p, new foucs:%p, target:%p) %s",
            GetTargetHWND(), ::GetFocus(), ps->hwnd,
            DbgFormatMessage(ps->hwnd, ps->message, ps->wParam, ps->lParam).c_str());
        _CDummyChild_CheckSetFocus::Call(m_pWnd);
    }
    else if (ps->message == WM_WINDOWPOSCHANGED && (((LPWINDOWPOS)(ps->lParam))->flags & SWP_SHOWWINDOW))
    {
        if (IDummy::SyncIsSelfOrOwneredWindow(m_pWnd, ps->hwnd))
            SyncWin(m_pWnd, false);
    }
//     else if ((ps->message == WM_PAINT /*|| uMsg == CM_PRI_DIRTY*/) && m_pWnd->Is_WS_PRI_DIRTY() && ps->hwnd == m_pWnd->GetHWND())
//         SyncWin(m_pWnd, true);
    return false;
}

// TMBool CDummyChild::IsValideWindow(HWND hWnd)
// {
//     return hWnd == m_wnd || m_wnd.IsChild(hWnd);
// }

static HWND _CDummyChild_GetNextDlgTabItem(HWND hDlg, HWND hCtl, BOOL bPrevious)
{
    HWND hNext = ::GetNextDlgTabItem(hDlg, hCtl, bPrevious);
    if (!hNext || hNext == hCtl)
        return 0;
    return hNext;
}

TMBool CDummyChild::SetNextFocus(MSG* msg)
{
    DbgAssert(msg->message == WM_KEYDOWN && msg->wParam == VK_TAB);

    int nCode = ::SendMessage(msg->hwnd, WM_GETDLGCODE, msg->wParam, (LPARAM)msg);
    if (nCode & (DLGC_WANTMESSAGE | DLGC_WANTTAB))
        return false;

    //xinpa: when tab focus to next control, need to know if inner control this wnd need tab. Mainly for tab in Browser.
    nCode = ::SendMessage(m_wnd, WM_GETDLGCODE, msg->wParam, (LPARAM)msg);
    if (nCode & (DLGC_WANTMESSAGE | DLGC_WANTTAB))
        return false;

    TMBool bReverse = CKeyInput(msg->wParam, msg->lParam).IsModifiersShift();
    HWND hNext = _CDummyChild_GetNextDlgTabItem(m_pWnd->GetHWND(), msg->hwnd, bReverse);//m_pWnd->GetHWND(), msg->hwnd, bReverse);
    if (m_wnd.IsChild(hNext) || (m_wnd == hNext && m_pWnd->IsTabGroup())) // IsValideWindow(hNext))
    {
        ::SetFocus(hNext);
        return true;
    }
    //m_pWnd->GetView()->SetNextFocus(m_hWnd, bReverse);
    if (m_pWnd->IsTabGroup())
        SetDefaultWndFocus(bReverse);
    else
        m_pWnd->SetNextFocus(bReverse, eFocusReasonTab);
    return true;
}

// CHookGetMessageEx
TMBool CDummyChild::PreTranslateMessage(MSG* msg)
{
    if (msg->message >= WM_KEYFIRST &&
        msg->message <= WM_KEYLAST &&
        m_pWnd->GetOwner()->TranslateAcceleratorKey(msg))
        return true;
    if (msg->message == WM_KEYDOWN && msg->wParam == VK_TAB)
        return SetNextFocus(msg);
    return 0;
}

void CDummyChild::SetDefaultWndFocus(TMBool bReverse)
{
    // GetNextDlgTabItem have a bug:
    // when GetNextDlgTabItem(hWnd, 0, true); it always return 0;
    //    so I get first tab, then get first's prev tab stop window.
    HWND hNext = _CDummyChild_GetNextDlgTabItem(m_wnd, 0, 0);
    if (hNext == m_wnd) // not find
    {
        if (!(m_wnd.GetStyle() & WS_TABSTOP))
            hNext = 0;
    }
    else if (bReverse && hNext)
    {
        HWND hTmp = _CDummyChild_GetNextDlgTabItem(m_wnd, hNext, bReverse);
        if (hTmp)
            hNext = hTmp;
    }

    if (hNext)
        ::SetFocus(hNext);
    else
        m_pWnd->SetNextFocus(bReverse, eFocusReasonActiveWindow);
}

TMBool CDummyChild::ProcessWindowMessage(HTUI hWnd, TMUInt uMsg, TMWParam wParam,
    TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)
{
    if (uMsg == WM_SETFOCUS)
    {
        DbgTrace(L"CDummyChild::SetFocus(HWND:%p, %d)", GetTargetHWND(), m_pWnd->GetObjectID());
        CHookGetMessageEx::SetHook();
        if (!GetTargetFocus())
        {
            TMBool bReverse = (g_input.m_key.IsKeyTab() < 0); // CKeyboard::IsKeyDownShift();
            SetDefaultWndFocus(bReverse);
        }
    }
    else if (uMsg == WM_KILLFOCUS)
    {
        DbgTrace(L"CDummyChild::KillFocus(HWND:%p, %d)", GetTargetHWND(), m_pWnd->GetObjectID());
        CHookGetMessageEx::RemoveHook();
    }
    else if (uMsg == CM_SETPARENT_P && m_wnd)
    {
        HWND hOld = ((HTUI)wParam)->GetHWND();
        HWND hNew = ((HTUI)lParam)->GetHWND();
        if (hOld != hNew)
        {
            ::ShowWindow(m_wnd, SW_HIDE);
            ::SetParent(m_wnd, hNew);
        }
        hWnd->SetDirtyAndInvalidate();
    }

    return CDummyWindowBase::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
}

/////////////////////////////////////////////////////////
// CDummyPop
TMBool CDummyPop::OnCallWndRetProc(CWPRETSTRUCT* ps)
{
    DbgAssert(m_wnd);
    if (ps->message == WM_DESTROY && ps->hwnd == m_wnd)
        DestroyDummy(eUnbindDummyDetach);
    else if (ps->message == WM_WINDOWPOSCHANGED)
    {
        if (IDummy::SyncIsSelfOrOwneredWindow(m_pWnd, ps->hwnd))
            SyncWin(m_pWnd, SyncIsUpdateNowMode(m_pWnd));
    }
    return false;
}

CDummyPop::~CDummyPop()
{
    DbgAssert(!m_wnd);
}

// CDummyWindowBase
void CDummyPop::BindHWND(TMHWnd hWnd)
{
    CDummyWindowBase::BindHWND(hWnd);
    SetHook();
}

TMHWnd CDummyPop::UnbindHWND()
{
    RemoveHook();
    return CDummyWindowBase::UnbindHWND();
//     HWND hWnd = m_hWnd;
// 
//     CDummyWindowBase::UnbindHWND(bDestroy);
}

// TMBool CDummyPop::ProcessWindowMessage(
//     HTUI hWnd, TMUInt uMsg, TMWParam wParam,
//     TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)
// {
//     if ((uMsg == WM_PRINTCLIENT /*|| uMsg == CM_PRI_DIRTY*/) && m_pWnd->Is_WS_PRI_DIRTY())
//         SyncWin(hWnd, true);
//     return CDummyWindowBase::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
// }


/////////////////////////////////////////////////////////
// CTuiDummyBase
void CTuiDummyBase::InitWin(CWindow wnd)
{
    if (wnd == GetWnd())
        return;

    enum { DM_LAYEREDCHILD  = 0x08 };
    DbgAssert(!(m_dwStyle & DM_LAYEREDCHILD));

    
    m_dwPriStyle &= ~DUMMYINFO::FLAG_MASK;
    if (!wnd.IsWindow())
    {
        m_ptrDummyImpl.DestroyDefault();
        return;
    }
    
    TMLParam nType = WIN::GetWinPropType(wnd);
    WIN::SetWinProp(wnd, nType | WIN_PROP_TYPE_DUMMY);

    if (wnd.GetStyle() & WS_CHILD)
    {
        SetFlagChild();
        CDummyChild* p = new CDummyChild();
        p->AttachWidget(this);
        p->BindHWND(wnd);
    }
    else
    {
        SetFlagPop();
        CDummyPop* p = new CDummyPop(); //this, wnd);//, hWgt, IsStyleScreenDPI());
        p->AttachWidget(this);
        p->BindHWND(wnd);
    }

    SetDirtyAndInvalidate();
}

// CTuiWgt
#ifdef TUI_EDIT_MODE

CString CTuiDummyBase::GetDebugInfo()
{
    TM::CString str;

    HWND hBind = m_ptrDummyImpl ? m_ptrDummyImpl->GetTargetHWND() : 0;
    str.Format(L"\r\n\r\nbind HWND:0X%X", hBind);

    if (hBind)
    {
        CRect rc;
        CWindowAT(hBind).GetWindowRect(&rc);
        TM::CString strTmp;
        strTmp.Format(L" rect:" TM_L(TM_FMT_RCI_STR) L";", TM_FMT_RCI_VAL(rc));
        str += strTmp;
    }

    return CTuiWgt::GetDebugInfo() + str;
}

#endif // TUI_EDIT_MODE


////////////////////////////////////////////////////////////////////////////////
// class CTuiDummyImpl
TMResult CTuiDummyImpl::OnCreate (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (!Edit_IsObjectInDocMode(this) && m_nIDDlg)
    {
        CTuiWidget wnd = GetView()->CreateDlg(GetModule()->m_hRes, m_nIDDlg, GetHWND());
        if (wnd)
            InitWin(wnd.GetHWND());
    }
    bHandled = IsLifeDestroyed();
    return 0;
}

TMResult CTuiDummyImpl::OnGetInfo(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    DbgAssert(lParam);
    DUMMYINFO& di = *(DUMMYINFO*)lParam;
    di.hWnd = GetWnd();
    di.nMode = (m_dwPriStyle & DUMMYINFO::FLAG_MASK);
    di.hOverlay = 0;
    return 0;
}


////////////////////////////////////////////////////////////////////////////////
// class CTuiDelayDummy
TMResult CTuiDelayDummy::OnShow(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (!Edit_IsObjectInDocMode(this) &&
        !IsPriLoaded() &&
        !IsLifeInvalid() &&
        ((IsStyleDelayLoad() && IsAllVisible()) || !IsStyleDelayLoad()))
    {
        SetPriLoaded();
        Recreate();
    }
    bHandled = IsLifeDestroyed();
    return 0;
}



TM_END_NAMESPACE
