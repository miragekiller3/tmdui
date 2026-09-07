/*
 *  @file
 *  @brief  input: focus, caret, gesture
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2019.7.4
 *
 *  Copyright (C) 2019 miragekiller
 */

#include "tmgdi.h"
#include "input.h"
#include "uiwindow.h"
#include "platform.h"
#include "view.h"
#include "tooltip.h"


TM_BGN_NAMESPACE


//#define INPUT_TRACE DbgTrace
#define INPUT_TRACE DbgTraceNull

CInput g_input;


///////////////////////////////////////////////////////////////////////////////
// CGesture
CGesture::CGesture()
{
    m_htGesture.hWnd = 0;
}

void CGesture::OnWMGestureNotify(HTUI hMgr, TMLParam lParam)
{
#ifndef TM_QT
    // init
    GESTURENOTIFYSTRUCT* pg = (GESTURENOTIFYSTRUCT*)lParam;
    TMHWnd hWnd = hMgr->GetHWND();
    MemZeroT(m_htGesture);
    MemZeroT(m_hGesture);
    if (pg->hwndTarget == hWnd)
    {
        // hit test
        CPoint pt(pg->ptsLocation.x, pg->ptsLocation.y);
        CPointF ptNew;
        hMgr->ScreenToClient(&pt, 1, &ptNew);
        ptNew.CopyTo(m_htGesture.pt);
        hMgr->GetAndHitMouseTarget(m_htGesture, WM_GESTURENOTIFY, CWP_SKIPBINDWINDOW);

        // notify gesture use bubble
        if (m_htGesture.hWnd)
        {
            const DWORD dwPanMask = 0x1f;
            TMUInt32 dwFlag = 0;
            GESTURECONFIG gc[] =
            {
                { GID_ZOOM,         0, GC_ZOOM},
                { GID_PAN,          0, dwPanMask},
                { GID_ROTATE,       0, GC_ROTATE},
                { GID_TWOFINGERTAP, 0, GC_TWOFINGERTAP},
                { GID_PRESSANDTAP,  0, GC_PRESSANDTAP},
            };
            for (HTUI pNew = m_htGesture.hWnd; pNew ; pNew = pNew->GetParent())
            {
                TMUInt32 nMask = pNew->SendMessage(CM_GESTURENOTIFY, 0, (TMLParam)&m_htGesture);
                if (!nMask)
                    continue;
                dwFlag |= nMask;
                for (int i = 0; i < GESTURE_ID_COUNT; ++i)
                {
                    if (!m_hGesture[i] && ((0x01 << i) & nMask))
                    {
                        m_hGesture[i] = pNew;
                        DWORD dwMask = gc[i].dwBlock;
                        gc[i].dwWant = ((nMask >> GESTURE_DETAIL_SHIFT) & gc[i].dwBlock) | GC_ALLGESTURES;
                        gc[i].dwBlock = gc[i].dwWant ^ dwMask;
                    }
                }
            }

            if (dwFlag)
            {
                DbgVerify(_SetGestureConfig(hWnd, 0, 5, gc, sizeof(GESTURECONFIG)));
                return;
            }
        }
    }
    else
        INPUT_TRACE("CGesture::OnWMGestureNotify() hWnd:%p not self(%d)", hWnd, hMgr->GetObjectID());


    // needn't gesture.
    GESTURECONFIG gc[] = {0, 0, GC_ALLGESTURES};
    DbgVerify(_SetGestureConfig(hWnd, 0, 1, gc, sizeof(GESTURECONFIG)));
#endif
}

TMBool CGesture::OnWMGesture(HTUI hMgr, TMLParam lParam)
{
#ifndef TM_QT

    GESTUREINFO gi = { sizeof(gi) };
    HGESTUREINFO hGI = (HGESTUREINFO)lParam;
    _GetGestureInfo(hGI, &gi);
    INPUT_TRACE("CGesture::OnWMGesture(type:%d, flag:%d, point:%d,%d, hwnd:%p, self:%d)",
        gi.dwID, gi.dwFlags, gi.ptsLocation.x, gi.ptsLocation.y, gi.hwndTarget, hMgr->GetObjectID());
    if (gi.hwndTarget != hMgr->GetHWND())
        return false;

//     DbgAssert(gi.hwndTarget == hMgr->GetHWND());

    int nid = gi.dwID - eGIDBgn;
    if (nid < 0 || nid >= tm_countof(m_hGesture) || !m_hGesture[nid])
        return false;

    m_hGesture[nid]->SendMessage(CM_GESTURE, (TMWParam)&gi, (TMLParam)&m_htGesture);
    _CloseGestureInfoHandle(hGI);
#endif
    return true;
}

TMBool CGesture::ProcessMessage(HTUI hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam)
{
#ifndef TM_QT

    if (uMsg == WM_GESTURENOTIFY)
    {
        OnWMGestureNotify(hWnd, lParam);

#ifdef TM_DBG 
        GESTURECONFIG gcx[5] = {0};
        UINT uiGcs = 5;
        gcx[0].dwID  = GID_PAN; 
        gcx[1].dwID  = GID_ROTATE; 
        gcx[2].dwID  = GID_ZOOM; 
        gcx[3].dwID  = GID_TWOFINGERTAP; 
        gcx[4].dwID  = GID_PRESSANDTAP; 
        DbgVerify(_GetGestureConfig(hWnd->GetHWND(), 0, 0, &uiGcs, gcx, sizeof(GESTURECONFIG))); 
        INPUT_TRACE("CGesture::GestureProcessMessage(WM_GESTURENOTIFY) oid:%d; result:(pan want:%x, block:%x)",
            hWnd->GetObjectID(), gcx[0].dwWant, gcx[0].dwBlock);
#endif
    }
    else if (uMsg == WM_GESTURE)
        return OnWMGesture(hWnd, lParam);
#endif
    return false;
}

void CGesture::Clear(HTUI hWnd)
{
    for (int i = 0; i < tm_countof(m_hGesture); ++i)
    {
        if (hWnd == m_hGesture[i])
            m_hGesture[i] = 0;
    }
    if (hWnd == m_htGesture.hWnd)
        m_htGesture.hWnd = 0;
}

void CGesture::ClearItem(HTUI hWnd, void* pItem)
{
    if (hWnd == m_htGesture.hWnd && pItem == m_htGesture.pTaget)
        m_htGesture.pTaget = 0;
}


TMBool CGesture::GetState(TUIHITINFO& ht, int nID)
{
    DbgAssert(nID >= 0 && nID < tm_countof(m_hGesture));
    ht = m_htGesture;
    ht.hHint = m_hGesture[nID];
    return true;
}


TMResult CGesture::OnCMGestureNotify(HTUI hWnd)
{
    // check flag
    TMResult flag = 0;
    if (hWnd->HasVScroll())
        flag =  GESTURE_PAN_SINGLE_FINGER_VERTICALLY | GESTURE_PAN_GUTTER | GESTURE_PAN_INERTIA;
    if (hWnd->HasHScroll())
        flag |= (GESTURE_PAN_SINGLE_FINGER_HORIZONTALLY | GESTURE_PAN_GUTTER | GESTURE_PAN_INERTIA);
    return flag;
}

TMBool CGesture::OnCMGesture(HTUI hWnd, TMWParam wParam)
{
#if defined(TM_WIN) && !defined(TM_QT)
    GESTUREINFO* pi = (GESTUREINFO*)wParam;
    if (pi->dwID != GID_PAN)
        return false;

    static POINTS ptOld;
    if (pi->dwFlags == GESTURE_FLAG_BEGIN)
        ptOld = pi->ptsLocation;
    else
    {
        CPoint pt(ptOld.x - pi->ptsLocation.x, ptOld.y - pi->ptsLocation.y);
        CTuiPoint ptw;
        CTuiZoom zoom(hWnd->GetDPI());
        zoom.DPtoLP(&pt, 1, &ptw);
        ptOld = pi->ptsLocation;
        CTuiWidget wnd(hWnd);
        wnd.ScrollBy(ptw.X(), ptw.Y());
    }
#endif // #if defined(TM_WIN) && !defined(TM_QT)
    return true;
}


///////////////////////////////////////////////////////////////////////////////
// HotKey
CInputHotKey::CInputHotKey()
    : m_bActivateApp(true)
{
}

void CInputHotKey::AddHotkey(CPropHotKey hk, HTUI hWnd, TMUInt nCmd)
{
    DbgAssert(hWnd);
    if (!hk.m_key || (hk.m_modifiers & MOD_DISABLE))
        return;

    hk.Normalize();
    if (hk.m_modifiers & MOD_SYS)
    {
        int n = m_hotKeySysMT.AddMT(hk, hWnd, nCmd);
        INPUT_TRACE("CInput::AddHotkey_sys(hWgt:%p(%d), hk:(%d,%x), nCmd:%d) size:%d", hWnd, hWnd->GetObjectID(), hk.m_vk, hk.m_modifiers, nCmd, n);
        if (1 == n)
            m_hotKeySysMT.SetHook();
    }
    else if (hk.m_modifiers & MOD_APP)
    {
        int n = m_hotKeyAppMT.AddMT(hk, hWnd, nCmd);
        if ((1 == n) && m_bActivateApp)
        {
            m_hotKeyAppMT.SetHook();
            INPUT_TRACE("CInput::AddHotkey_app(hWgt:%p(%d), hk:(%d,%x), nCmd:%d) size:%d", hWnd, hWnd->GetObjectID(), hk.m_vk, hk.m_modifiers, nCmd, n);
        }
    }
    else
    {
        int n = hWnd->GetOwner()->m_aHotKeyMgr.Add(hk, hWnd, nCmd);
        INPUT_TRACE("CInput::AddHotkey_dlg(hWgt:%p(%d), hk:(%d,%x), nCmd:%d) size:%d", hWnd, hWnd->GetObjectID(), hk.m_vk, hk.m_modifiers, nCmd, n);
    }
}

void CInputHotKey::RemoveHotkey(CPropHotKey hk, HTUI hWnd)
{
    if (!hk.m_key)
        return;

    hk.Normalize();
    if (hk.m_modifiers & MOD_SYS)
    {
        int n = m_hotKeySysMT.EraseMT(hk, hWnd);
        if (0 == n)
            m_hotKeySysMT.RemoveHook();
        INPUT_TRACE("CInput::RemoveHotkey_sys(hWgt:%p, hk:(%d,%x)) size:%d", hWnd, hk.m_vk, hk.m_modifiers, n);
    }
    else if (hk.m_modifiers & MOD_APP)
    {
        int n = m_hotKeyAppMT.EraseMT(hk, hWnd);
        if (0 == n)
            m_hotKeyAppMT.RemoveHook();
        INPUT_TRACE("CInput::RemoveHotkey_app(hWgt:%p, hk:(%d,%x)) size:%d", hWnd, hk.m_vk, hk.m_modifiers, n);
    }
    else
    {
        int n = hWnd->GetOwner()->m_aHotKeyMgr.Erase(hk, hWnd);
        INPUT_TRACE("CInput::RemoveHotkey_dlg(hWgt:%p, hk:(%d,%x)) size:%d", hWnd, hk.m_vk, hk.m_modifiers, n);
    }
}

void CInputHotKey::OnActivateApp(TMBool bActivate)
{
    if (!bActivate == !m_bActivateApp)
        return;
    m_bActivateApp = !m_bActivateApp;
    INPUT_TRACE("CInput::OnActivateApp(%d))  Hotkey::remove app hook", bActivate);
    m_hotKeyAppMT.RemoveHook();
    if (bActivate) // move hook to system front
    {
        if (m_hotKeyAppMT.size())
        {
            INPUT_TRACE("CInput::OnActivateApp(%d))  Hotkey::set app hook", bActivate);
            m_hotKeyAppMT.SetHook();
        }
        if (m_hotKeySysMT.size())
        {
            INPUT_TRACE("CInput::OnActivateApp(%d))  Hotkey::remove and set sys hook", bActivate);
            m_hotKeySysMT.RemoveHook();
            m_hotKeySysMT.SetHook();
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// FocusChain
TMBool CFocusChain::RemoveFC(CTuiWidget wnd, int index)
{
    DbgAssert(index == 0 || index == 1);
    CFCList::recorder rec(m_aChains[index]);
    rec.find(wnd);
    if (!rec)
        return false;

    // erase
    HTUI hSecond = rec.get_data();
    DbgAssert(hSecond);
    rec.erase();
    m_aChains[1 - index].erase(hSecond);

    return true;
}

void CFocusChain::RemoveFocusChain(HTUI hWnd)
{
    if (!RemoveFC(hWnd, 0))
        RemoveFC(hWnd, 1);
}

void CFocusChain::SetFocusChain(HTUI hFirst, HTUI hSecond)
{
    DbgAssert(hFirst != 0);
    DbgAssert(hFirst != hSecond);

    // remove first
    RemoveFocusChain(hFirst);
    if (!hSecond)
        return;

    // remove second
    RemoveFocusChain(hSecond);

    // add chain
    if (hFirst->IsLifeInvalid() || hSecond->IsLifeInvalid())
        return;
    m_aChains[0][hFirst] = hSecond;
    m_aChains[1][hSecond] = hFirst;
}


///////////////////////////////////////////////////////////////////////////////
// CInput
CInput::CInput()
    : m_bShowCaret(false)
    , m_hCaret(0)
    , m_bKorea(0)
{
}

void CInput::Clear(HTUI hWnd)
{
    CGesture::Clear(hWnd);
    if (hWnd == m_hFocusMgr)
        m_hFocusMgr = 0;
    DbgAssert(hWnd != m_hCaret);
}

///////////////////////////////////////////////////////////////////////////////
// caret
void CInput::OnTimer(TMHTimer hTimer, TMClock tmNow)
{
    m_bShowCaret = !m_bShowCaret;
    if (m_hCaret)
        m_hCaret->Invalidate();
}

void CInput::CaretCreate (CTuiWgt* pWin)
{
    INPUT_TRACE("CInput::Create(%x); m_bShowCaret:%d;", pWin, m_bShowCaret);

    DbgVerify(::DestroyCaret());
    if (m_hCaret)
        KillTimer();

    if (pWin && !pWin->CanInput())
        pWin = 0;

    m_hCaret = pWin;
    m_bShowCaret = 1;

    if (pWin)
    {
        pWin->Invalidate();
        SetTimer(600);
        CWindowAT wnd(pWin->GetHWND());
        DbgVerify(wnd.CreateUnunsedCaret(10, 20));
        DbgVerify(wnd.ShowCaret());
    }
}

void CInput::CaretDestroy (CTuiWgt* pWin)
{
    INPUT_TRACE("CInput::Destroy(%x); m_bShowCaret:%d;", pWin, m_bShowCaret);

    DbgAssert(pWin);

    if (pWin == m_hCaret && m_hCaret)
    {
        if (m_hCaret)
            m_hCaret->Invalidate();
        KillTimer();
        m_hCaret = 0;
        DbgVerify(::DestroyCaret());
    }
}

void CInput::CaretReset(CTuiWgt* pWin)
{
    if (pWin == m_hCaret && m_hCaret)
    {
        KillTimer();
        SetTimer(600);
        m_bShowCaret = 1;
    }
}

void CInput::CaretDraw(IGXApp* gapp, CTuiWgt* pWin, const CTuiRect& rc)
{
    if (m_hCaret == pWin && m_bShowCaret && m_hFocusMgr)
    {
//         INPUT_TRACE("DrawCaret:%d,%d,%d,%d", rc);
        DbgAssert(m_hFocusMgr);
        gapp->InvertRect(rc);

        if (m_rcCaret != rc)
        {
            m_rcCaret = rc;
            CRect rcClient;
            m_hCaret->GetDPI().LPtoDP(rc, rcClient);
            WIN::ImeMoveTo(m_hFocusMgr, m_hCaret, rcClient);
            DbgVerify(SetCaretPos(rcClient.Left(), rcClient.Top()));
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// HotKey
#define DBG_HotKeyCheckLocked()    DbgAssert(g_input.m_criticalHotKey.IsLockThread())

void CHotKeyMap::_Done(const CHotKeyStore* p)
{
    DBG_HotKeyCheckLocked();
    DbgAssert(p->m_hOwner);
    TMUInt nid = p->m_id;
    if (!nid)
        nid = p->m_hOwner->GetDlgCtrlID();
    DbgAssert(p->m_hOwner->IsAllEnable(true));
    p->m_hOwner->Notify(nid, 1, true);
}

const CHotKeyMap::CHotKeyStore* CHotKeyMap::_TranslateHotKey(TMUInt uMsg, TMWParam wparam, TMLParam lparam)
{
    TMUInt16 mdf;
    if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN)
        mdf = 0;
    else if (uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP)
        mdf = MOD_ON_KEYUP;
    else
        return 0;

    DBG_HotKeyCheckLocked();
    CHotKeyList::recorder rec(m_aHotKey, m_aHotKey.binary_find(CHotKeyStore(wparam)));
//    rec.find(CHotKeyStore(wparam));
    if (!rec)
        return 0;
    const CHotKeyStore* key = &rec.get_key();
    mdf |= CKeyInput(wparam, lparam).GetModifiers();
    TMUInt16 mdfSide = mdf & CPropHotKey::eMaskSide;
    if (mdfSide == 0)
        mdfSide = CPropHotKey::eMaskSide;
    for (;;)
    {
        key->CheckNormalize();
        if (key->m_modifiers & MOD_IGNORE_ALL_MODIFIER)
            return key;
        if ((mdf & CPropHotKey::eMaskKeysPress) == (key->m_modifiers & CPropHotKey::eMaskKeysPress))
        {
            TMUInt16 mdfSideKey = key->m_modifiers & CPropHotKey::eMaskSide;
            if (mdfSideKey & mdfSide)
                return key;
        }
        if (!++rec)
            break;
        key = &rec.get_key();
        if (key->m_vk != wparam)
            break;
    }
    return 0;
}

TMBool CHotKeyMap::_Check(CPropHotKey hotkey, HTUI hWnd)
{
    for (CHotKeyList::recorder rec(m_aHotKey); rec; ++rec)
    {
        const CHotKeyStore& ks = rec.get_key();
        if (ks.m_hOwner == hWnd && hotkey.m_key == ks.m_key)
            return true;
    }
    return false;
}

TMBool CHotKeyMap::TranslateHotKeyMT(TMUInt uMsg, TMWParam wparam, TMLParam lparam)
{
    CInputHotKey::CLock lock(g_input.m_criticalHotKey);
    const CHotKeyStore* p = _TranslateHotKey(uMsg, wparam, lparam);
    if (!p)
        return false;
    INPUT_TRACE("CHotKeyMap::TranslateHotKeyMT(uMsg:%d, key:%d) success", uMsg, wparam);
    _Done(p);
    return !(p->m_modifiers & MOD_THROUGH);
}

int CHotKeyMap::Add(const CPropHotKey& hotkey, HTUI hWnd, TMUInt cmd)
{
    DbgAssert(!(hotkey.m_modifiers & MOD_DISABLE));
    hotkey.CheckNormalize();
    DbgAssert(!_Check(hotkey, hWnd));
    m_aHotKey.binary_insert(CHotKeyStore(hotkey, hWnd, cmd));
    return m_aHotKey.size();
}

int CHotKeyMap::AddMT(const CPropHotKey& hotkey, HTUI hWnd, TMUInt cmd)
{
    CInputHotKey::CLock lock(g_input.m_criticalHotKey);
    return Add(hotkey, hWnd, cmd);
}

int CHotKeyMap::Erase(const CPropHotKey& hotkey, HTUI hWnd)
{
    hotkey.CheckNormalize();
    for (CHotKeyList::recorder rec(m_aHotKey); rec; ++rec)
    {
        const CHotKeyStore& ks = rec.get_key();
        if (ks.m_hOwner == hWnd && hotkey.m_key == ks.m_key)
        {
            rec.erase();
            break;
        }
    }
    return m_aHotKey.size();
}

int CHotKeyMap::EraseMT(const CPropHotKey& hotkey, HTUI hWnd)
{
    CInputHotKey::CLock lock(g_input.m_criticalHotKey);
    return Erase(hotkey, hWnd);
}


///////////////////////////////////////////////////////////////////////////////
// IME
void CInput::ImeOnStartCompostion()
{
    m_rcCaret.SetLeft(-9999);
}

void CInput::ImeEnable(HTUI hWgt, TMBool bEnable)
{
//     m_hFocusMgr = hWgt ? hWgt->GetHWND() : 0;
    if (!m_hFocusMgr)
        return;
    WIN::ImeEnable(m_hFocusMgr, bEnable);
}

///////////////////////////////////////////////////////////////////////////////
// focus
void CInput::FocusOnSet(CTuiWnd& mgr)
{
    m_hFocusMgr = &mgr;
}

void CInput::FocusOnKill(CTuiWnd& mgr)
{
//     DbgAssert(mgr.m_hWnd == m_hFocusMgr);
    m_hFocusMgr = 0;
}

EButStates CInput::GetScrollStates(const TUIHITINFO& cur, const TUIHITINFO* pHot, TMBool bHotSB)
{
    DbgAssert(cur.hWnd);
    
    HTUI hWnd = cur.hWnd;
    if (hWnd->GetStyle() & WS_DISABLED)
        return e_sssDisable;
    
    CView* pm = hWnd->GetView();
    if (!pHot)
        pHot = &pm->m_htOver;
    TMBool bSame = (hWnd == pHot->hWnd && cur.dwCode == pHot->dwCode && cur.bNC == pHot->bNC && cur.pTaget == pHot->pTaget);
    
    if (bSame)
    {
        TMBool bDown = pm->IsMouseDown();
        return bDown ? e_sssDown : e_sssHover;
    }
    
    return bHotSB ? e_sssActive : e_sssUnactive;
}


namespace PRI
{
    HTUI tuiCaretGetInfo(TUIRect& rc)
    {
        rc = g_input.m_rcCaret;
        return g_input.m_hCaret;
    }
}



HTUI CFocus::GetFocus() // manager always can't focus.
{
    HTUI hWgt = (m_pFocus && m_pFocus->GetParent()) ? m_pFocus : 0;
    if (hWgt)
        CTuiWidget(hWgt)._Debug_WidgetMustValidAndSameThread();
    return hWgt;
}

TMBool CFocus::CheckSetFocus(CTuiWgt* pWin, TMBool bCheck, TMBool bActive, EFocusReason reason)
{
    if (m_pFocus == pWin)
        return true;

    if (pWin && pWin->IsLifeInvalid())
        return false;
    
    if (pWin && bCheck && !pWin->CanInput())
        return false;

    if (pWin)
        pWin->GetOwner()->m_wndFocusBackup = pWin;

    // don't restore default dialog focus.
    m_bFoucing = true;
    Ref pOldFocus(pWin);
    if (pWin && bActive)
    {
        Ref pOldFocus(pWin);
        pWin->GetView()->InternalSetFocus(pWin);
        if (pWin->IsLifeInvalid())
            pWin = 0;
    }
    m_bFoucing = false;
    
    if (m_pFocus == pWin)  // changed by WM_ACTIVATE
        return false;
    
    CTuiWgt* pOld = GetFocus();
//     if (pWin && pWin->IsLifeInvalid())
//         pWin = 0;
    m_pFocus = pWin;
    if (pOld)
    {
        pOld->Invalidate();
        pOld->SendMessage(WM_KILLFOCUS, (TMWParam)pWin, reason);
        if (pWin != m_pFocus)
            return true; // changed by WM_KILLFOCUS
    }
    
    if (pWin)
    {
        pWin->Invalidate();
        pWin->SendMessage(WM_SETFOCUS, (TMWParam)pOld, reason);
    }

    // WS_AUTOTABSTOP
    if (m_pFocus)
        m_pFocus->SetGroupDefaultTabStop();

    // if call SetFocus on WM_SETFOCUS, the code maybe call twice.
    if (reason == eFocusReasonBacktab || reason == eFocusReasonTab)
        g_mgrTooltip.SetTabToolTip(m_pFocus);
    
    return true;
}

static HTUI _CFoucs_GetGroup(HTUI hCur, TMUInt32 dwStyle, TMBool bGetRoot)
{
    for (; ;)
    {
        if (hCur->GetExStyle() & dwStyle)
            return hCur;
        HTUI hp = hCur->GetParent();
        if (!hp)
            return bGetRoot ? hCur : 0;
        hCur = hp;
    }
}

struct _FWalkStopTab
{
    TMBool operator()(HTUI hWnd) const
    {
        return hWnd->IsTabStop() && hWnd->CanInput() && !hWnd->Is_WS_PRI_NOTABSTOP();
    }
};

struct _FWalkStopArrow
{
    TMBool operator()(HTUI hWnd) const
    {
        return hWnd->IsTabStop() && hWnd->CanInput();
    }
};

TMBool CFocus::SetNextArrow(TMBool bReverse)
{
    if (!m_pFocus)
        return false;

    HTUI hGroup = _CFoucs_GetGroup(m_pFocus, WS_ADV_ARROWGROUP, false);
    if (!hGroup)
        return false;

    CTuiWgt::CItem p(m_pFocus);
    p.GlobalCycleWalk(hGroup, bReverse, _FWalkStopArrow());
    if (p && p != m_pFocus)
    {
        DbgAssert(p->CanInput());
        return CheckSetFocus(p, false, true, eFocusReasonTab);
    }
    return false;
}

static void _CFocus_SetFocus(CFocus& f, CTuiWnd* hMgr, HTUI p)
{
    DbgAssert(p->CanInput());
    hMgr->m_wndFocusBackup = p;
    f.CheckSetFocus(p, false, true, eFocusReasonTab);
}

HTUI CFocus::SetNextTab(CTuiWnd* hMgr, TMBool bReverse)
{
    DbgAssert(hMgr);
    DbgAssert(CWindowAT(hMgr->GetHWND()).IsFocus());

    HTUI hNow = hMgr->m_wndFocusBackup;
    return SetNextTab(hMgr, hNow, bReverse, false);
}

HTUI CFocus::SetNextTab(CTuiWnd* hMgr, HTUI hNow, TMBool bReverse, TMBool bTest)
{
    CTuiWgt::CItem p(hNow);
    if (hNow)
    {
        HTUI hGroup = _CFoucs_GetGroup(hNow, WS_ADV_TABGROUP, false);
        if (hGroup)
        {
            p.GlobalCycleWalk(hGroup, bReverse, _FWalkStopTab());
            if (p && !bTest)
                _CFocus_SetFocus(*this, hMgr, p);
            return p;
        }
    }
    
    p.GlobalWalk(hMgr, bReverse, _FWalkStopTab());
    if (p)
    {
        if (!bTest)
            _CFocus_SetFocus(*this, hMgr, p);
        return p;
    }
    
    // start a new search
    if (!bTest)
        hMgr->m_wndFocusBackup = 0;
    CWindowAT wndParent(hMgr->GetHWND());
    wndParent = (TMHWnd)wndParent.GetParent();
    if (wndParent)
    {
        wndParent.SendMessage(WM_NEXTDLGCTL, bReverse, 0);
        if (!CWindowAT(hMgr->GetHWND()).IsFocus())
            return 0; // dialog change focus.
    }
    
    p.GlobalWalk(hMgr, bReverse, _FWalkStopTab());
    if (p && !bTest)
        _CFocus_SetFocus(*this, hMgr, p);
    return p;
}

void CFocus::CleanFocus(HTUI hWgt)
{
    if (hWgt == m_pFocus)
    {
        INPUT_TRACE("CFocus::CleanFocus(%p)", hWgt);
        if (hWgt == GetFocus())
            m_pFocus->SendMessage(WM_KILLFOCUS);
        m_pFocus = 0;
    }
}

static void _SendDefaultComamndMessage(HTUI hMgr, CTuiWgt* pFocus, TMUInt uID)
{
    if (pFocus)
    {
        DbgAssert(!pFocus->Is_WS_PRI_IS_WINDOW());
        pFocus->SendMessage(CM_PRECOMMAND, MAKEWPARAM(uID, BN_CLICKED), (TMLParam)pFocus);
    }
    else
        hMgr->SendMessage(WM_COMMAND, MAKEWPARAM(uID, BN_CLICKED), 0);
}

TMBool CFocus::OnTabKey(CTuiWnd* hMgr, CKeyInput key)
{
    TMUInt16 mdf = key.GetModifiersFast();
    if (mdf == 0 || mdf == MOD_SHIFT)
    {
        if (hMgr->IsHideFocus())
            CWindowAT(hMgr->GetHWND()).ResetHideFocus();
        SetNextTab(hMgr, mdf & MOD_SHIFT);
        return true;
    }
    return false;
}

TMBool CFocus::IsArrowGroupMessage(CTuiWnd* hMgr, TUIMSG* pMsg)
{
    if (pMsg->message != WM_KEYDOWN)
        return false;
    switch (pMsg->wParam)
    {
    case VK_RIGHT:
    case VK_DOWN:
    case VK_LEFT:
    case VK_UP:
        {
            if (hMgr->IsHideFocus())
                CWindowAT(hMgr->GetHWND()).ResetHideFocus();
            TMWParam wParam = hMgr->GetModule()->GetLocal().ConvertKey(pMsg->wParam);
            return SetNextArrow((wParam == VK_LEFT || wParam == VK_UP));
        }
    }
    return false;
}

TMBool CFocus::IsDialogMessage(CTuiWnd* hMgr, TUIMSG* pMsg)
{
//         return false;
    if (pMsg->message == WM_KEYDOWN)
    {
        if (!m_pFocus)
            m_pFocus = hMgr;

        switch (pMsg->wParam)
        {
        case VK_TAB:
            return OnTabKey(hMgr, CKeyInput(VK_TAB, pMsg->lParam));
            
        case VK_CANCEL:
        case VK_ESCAPE:
            if (GetCapture())
            {
                ReleaseCapture();
                return true;
            }
            if (g_mgrTooltip.IsPop()) // exit tooltip
            {
                g_mgrTooltip.Clear();
                return true;
            }
            _SendDefaultComamndMessage(hMgr, GetFocus(), IDCANCEL);
            return true;
            
        case VK_EXECUTE:
        case VK_RETURN:
            _SendDefaultComamndMessage(hMgr, GetFocus(), IDOK);
            return true;
            
        default:
            return false; //IsArrowGroupMessage(hMgr, pMsg);
        }
    }
    
    return false;
}

TMBool CFocus::AppOnKey(HTUI pMgr, TMUInt uMsg, TMWParam wParam, TMLParam lParam)
{
    switch (uMsg)
    {
    case WM_CHAR:
        if (wParam == VK_TAB) return true;
        tm_fallthrough;
    case WM_SYSCHAR:     tm_fallthrough;
    case WM_DEADCHAR:    tm_fallthrough;
    case WM_SYSDEADCHAR: tm_fallthrough;
    case WM_KEYLAST:
        break;

    case WM_KEYDOWN:     tm_fallthrough;
    case WM_SYSKEYDOWN:
        g_input.m_key.Init(wParam, lParam);
        break;

    case WM_KEYUP:       tm_fallthrough;
    case WM_SYSKEYUP:
        g_input.m_key.Reset();
        break;

    case WM_HELP:
        break;
        
    default:
        return false;
    }
    
    TMBool b = false;
    if (GetFocus())
        b = m_pFocus->SendMessage(uMsg, wParam, lParam);
    else
        b = pMgr->SendBubbleMessage(uMsg, wParam, lParam);
    //     if (!b)
    //     {
    //         TUIMSG msg;
    // //         msg.hwnd = 0;//hWnd;
    //         msg.message = uMsg;
    //         msg.lParam = lParam;
    //         msg.wParam = wParam;
    //         b = IsDialogMessage(pMgr, &msg);
    //     }
    
    return b;
}

void CFocus::AppOnSetFocus(HTUI hMgr, TMHWnd hWnd)
{
    if (!IsFocuing() && !m_pFocus)
    {
        m_pFocus = hMgr;
        if (g_input.m_key.IsKey(VK_TAB))
            OnTabKey((CTuiWnd*)hMgr, g_input.m_key);
    }
}

// void CFocus::AppOnKillFocus(HTUI hMgr, TMHWnd hWnd)
// {
// //     if (!IsFocuing())
// //     {
// //         if (GetFocus())
// //             m_pFocus->SendMessage(WM_KILLFOCUS, 0, 0);
// //         m_pFocus = 0;
// //     }
// }



TM_END_NAMESPACE
