/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2008.08.07
 *
 *  Copyright (C) 2008 miragekiller
 */

#include "uimenubar.h"
#include "pic.h"
#include "../tmhook.h"
#include "platform.h"
#include "uiwindow.h"
#include "view.h"


TM_BGN_NAMESPACE



////////////////////////////////////////////////////////////////////////////////
//CTuiMenuBarImpl

// const int MENU_MARGIN = 8;

struct _TraceMenu : public CHookMsgFilterImpl<_TraceMenu>
{
    enum { eHotCloseMenu = -1, eHotEntryAlt = -2 };
    CTuiMenuBarImpl*    m_pWnd;
    int                 m_nHot;
    TMUInt              m_nLastMsg;
    TMHMenu             m_hPop;
    TMPoint             m_pt;
    TMHMenu             m_hSubPop; //0: alt mode; ==hPop: 1 level; != hPop: sub popup
    
    _TraceMenu      (CTuiMenuBarImpl* pWnd)
        : m_pWnd    (pWnd)
        , m_hSubPop (0)
    {
        DbgAssert(pWnd);
    }

    TMBool OnMsgFilter(int nEvent, MSG* pMsg)
    {
        if (nEvent == MSGF_MENU)
        {
//             if (pMsg->message != WM_TIMER)
//                 DbgTrace("OnMsgFilter : %x, %x", pMsg->message, pMsg->wParam);

            TMBool bCloseMenu = false;
            
            if (pMsg->message == WM_MOUSEMOVE)
            {
                CPoint pt(pMsg->lParam);
                if (pt != m_pt)
                {
                    m_pt = pt;
                    CTuiPoint ptl;
                    m_pWnd->ScreenToClient(&pt, 1, &ptl);
                    TMBool b = true;
                    m_nHot = m_pWnd->HitTest(ptl, b);
                    if (!b)
                        m_nHot = eHotCloseMenu;
                    m_nLastMsg = WM_MOUSEMOVE;
                    bCloseMenu = (m_nHot >= 0 && m_nHot != m_pWnd->m_nPop);
                }
            }
            else if (pMsg->message == WM_KEYDOWN)
            {
                if (pMsg->wParam == VK_ESCAPE)
                    m_nHot = eHotEntryAlt;
                else
                {
                    TMWParam key = m_pWnd->GetModule()->GetLocal().ConvertKey(pMsg->wParam);

                    int nItem = GetMenuItemCount(m_pWnd->m_menu.hMenu);
                    if (nItem <= 1)
                    {
                    }
                    else if ((key == VK_RETURN || key == VK_EXECUTE))
                    {
                        if (!m_hSubPop)
                        {
                            m_nHot = eHotEntryAlt;
                            bCloseMenu = true;
                            m_nLastMsg = WM_KEYDOWN;
                        }
                    }
                    else if (key == VK_LEFT)
                    {
                        //if (GetFocus ())
                        if (m_hSubPop == m_hPop || 0 == m_hSubPop)
                        {
                            m_nHot = m_pWnd->m_nPop - 1;
                            if (m_nHot < 0)
                                m_nHot = nItem - 1;
                            bCloseMenu = true;
                            m_nLastMsg = WM_KEYDOWN;
                        }
                    }
                    else if (key == VK_RIGHT)
                    {
                        // check is selected item have sub menu
                        TMUInt uState = 0;
                        for (int i = 0; ; ++i)
                        {
                            uState = GetMenuState(m_hPop, i, MF_BYPOSITION);
                            if ((uState & MF_HILITE) || (uState == (TMUInt)-1))
                                break;
                        }

                        if ((uState & MF_HILITE) && (uState & MF_POPUP) && uState != (TMUInt)-1)
                        {
                            // do nothing
                        }
                        else
                        {
                            m_nHot = m_pWnd->m_nPop + 1;
                            if (m_nHot >= nItem)
                                m_nHot = 0;
                            bCloseMenu = true;
                            m_nLastMsg = WM_KEYDOWN;
                        }
                    }
                }
            }
            else if (WM_MENUSELECT == pMsg->message)
            {
//                 DbgTrace("WM_INITMENUPOPUP : %d", pMsg->wParam);
                m_hSubPop = (TMHMenu)pMsg->lParam;
            }
            
            //close the current menu
            if (bCloseMenu)
                SendMessage(m_pWnd->GetHWND(), WM_CANCELMODE, 0, 0);

        }
        return 0;
    }
    
    // process:
    // 1. pop menu and mouse select other menu bar item: pop other item.
    // 2. pop menu and key down ESC: close menu, and go to ALT mode.
    // 3. pop menu and key down ALT: close menu, close ALT mode.
    // 4. pop menu and click other window: close menu, close ALT mode.
    // 5. select menu: close menu, close ALT mode.
    void TrackMenu(int nHot, TMUInt msg)
    {
        TMHWnd hWnd = m_pWnd->GetHWND();
        TMHMenu hMenu = m_pWnd->m_menu.hMenu;
        TMUInt uID = GetMenuItemID(hMenu, nHot);
        if (uID != -1)
        {
            HTUI hWgt = m_pWnd;
            DbgVerify(::PostMessage (hWnd, WM_COMMAND, uID, (TMLParam)hWgt));
            return;
        }

        TMPoint ptMsg = {0, 0};

        DbgAssert(nHot >= 0);
        
        m_nHot = nHot;
        m_nLastMsg = msg;
        SetHook();
        m_pWnd->SetPriInPop();// >m_dwPriStyle |= MS_PRI_INPOP;
        
        SendMessage(hWnd, WM_ENTERMENULOOP, false, 0);
        SendMessage(hWnd, WM_INITMENU, (TMWParam)(TMHMenu)m_pWnd->m_menu.hMenu, 0);
        
        while (m_nHot >= 0)
        {
            m_pWnd->InternalSetPop(m_nHot);
            m_pWnd->Notify(BN_PUSHED);
            m_hPop = GetSubMenu(hMenu, m_nHot);
            enum { eFlag = MF_POPUP | MF_HILITE | MF_MOUSESELECT };
            
            SendMessage(hWnd, WM_MENUSELECT, MAKEWPARAM(m_nHot, eFlag), (TMLParam)hMenu);
            
            m_pWnd->RedrawWindow();

            // select first item
            if (m_nLastMsg == WM_KEYDOWN)
            {
                HWND hwndFocus = GetFocus();
                if (hwndFocus)
                {
                    PostMessage(hWnd, WM_KEYDOWN, VK_DOWN, 0x11500001);
                    PostMessage(hWnd, WM_KEYUP, VK_DOWN, 0xD1500001);
                }
            }

            CTuiPoint pt = m_pWnd->GetTrackMenuPos(m_nHot);
            DbgTrace("_TraceMenu::TrackPopupMenu:%d,%d", pt);
            int nid = m_pWnd->GetModule()->TrackPopupMenu(m_hPop, TPM_LEFTALIGN | TPM_RETURNCMD, pt.x, pt.y, hWnd);
            m_hSubPop = 0;
            m_pWnd->Notify(BN_UNPUSHED);
            
            if (nid)
            {
                //if (m_nLastMsg == WM_LBUTTONDOWN)
                //    m_pWnd->m_nPop = eHotCloseMenu;
                m_nHot = eHotCloseMenu;
                PostMessage(hWnd, WM_COMMAND, nid, 0);
//                break;
            }
            
            //ignore WM_NCLBUTTONDOWN if repeat to double-click,or menu will be opened again after be closed  
            if (m_pWnd->m_nPop == m_nHot)
            {
                WIN::RemoveMesssage(hWnd, WM_LBUTTONDOWN, WM_LBUTTONDOWN);
                break;
            }

            if (m_pWnd->m_nPop == eHotCloseMenu)
                break;

            // user click blank area.
            if (m_nHot == eHotCloseMenu)
            {
                m_pWnd->m_nPop = eHotCloseMenu;
                break;
            }
        }
        
        RemoveHook();
        
        SendMessage (hWnd, WM_MENUSELECT, 0xFFFF0000, 0);
        SendMessage (hWnd, WM_EXITMENULOOP, 0, 0);

        m_pWnd->ResetPriInPop();
        if (m_nHot == eHotCloseMenu)
            m_pWnd->LeaveAltMode();
        else // eHotEntryAlt or click current pop item
            m_pWnd->EntryAltMode();

        m_pWnd->Invalidate();
    }
    
    TMBool KeyMenu(TMLParam lParam)
    {
        //DbgTrace("keymenu%d", GetTickCount());

        TMHMenu hMenu = m_pWnd->m_menu.hMenu;
        lParam = StrToLower((TMCharT)lParam);
        int nCount = m_pWnd->m_aRight.size();// GetMenuItemCount (hMenu);
        for (int i = 0; i < nCount; i++)
        {
            TMCharT c[256];
            int n = GetMenuString(hMenu, i, c, 255, MF_BYPOSITION);
            const TMCharT* p = StrChr(c, TM_T('&'), n);
            if (p)
            {
                if (StrToLower(p[1]) == lParam)
                {
                    TrackMenu(i, WM_KEYDOWN);
                    return true;
                }
            }
        }
        return false;
    }
    
    void MouseMenu(const TUIPoint& pt)
    {
        TMBool b;
        int n = m_pWnd->HitTest(pt, b);
        if (n >= 0)
            TrackMenu(n, WM_MOUSEMOVE);
    }
    
};


TMBool CTuiMenuBarImpl::MenuBarWindowProc(CTuiMenuBarImpl* pBar, TMHWnd hWnd, TMUInt uMsg, TMWParam wParam,
                                          TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)
{
    if (uMsg == WM_SYSCOMMAND && wParam == SC_KEYMENU && !lParam)
        return pBar->SendMessage(MB_SETALTMODE, !pBar->IsPriInAlt(), 0);//  OnSysKeyMenu();
    else if (uMsg == WM_ACTIVATE) // && m_nPop >= 0)
    {
        if (WA_INACTIVE == LOWORD(wParam))
        {
            Ref ref(pBar);
            pBar->RemoveHook();
            pBar->LeaveAltMode();
        }
        else
            pBar->SetHook();
        return false;
    }
    else if (uMsg == CM_GET_MENUBAR)
    {
        HTUI hWnd2 = pBar;
        lResult = (TMResult)hWnd2;
        return true;
    }

    return false;
}

CTuiMenuBarImpl::CTuiMenuBarImpl()
    : m_nPop (-1)
//     , m_clrNormal(cColorInvalid)    // (TMRGB(0,0,0))
//     , m_clrHilight(cColorInvalid)   // (TMRGB(0,0,0))
    , m_bdItem (6, 0, 6, 0)
{
    m_dwLayout = ALIGN_TOP;
//     m_pFore.GetPre(IDP_PRE_PIC_MENU);
}

TMResult CTuiMenuBarImpl::OnCreate (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_menu.Set(GetModule()->m_hRes, m_menu.nID);
    CTuiWindow(GetOwner()).SubclassMgr(CWindowMsgDelegate(this, &MenuBarWindowProc));
    if (GetActiveWindow() == GetHWND())
        SetHook();
    bHandled = false;
    return 0;
}

TMResult CTuiMenuBarImpl::OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    CTuiWindow mgr(GetOwner());
    if (mgr)
        mgr.UnsubclassMgr(CWindowMsgDelegate(this, MenuBarWindowProc));
    RemoveHook();
    bHandled = false;
    return 0;
}

TMResult CTuiMenuBarImpl::OnEnable(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
    if (!wParam)
        LeaveAltMode();
    return 0;
}


// CTuiWgt
// TMResult CTuiMenuBarImpl::OnPrintClient(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
void CTuiMenuBarImpl::DoPrintClient(IGXApp* gapp, TMLParam lParam)
{
    CTuiRect rc;
    GetClientRect(&rc);
    TMBool bHighContrast = IsHighContrast();

    if (m_pBk)
    {
        if (bHighContrast)
            gapp->FillRect(rc, CAppColor::GetColorMenuBk(0));
        else
        {
            CWinState ws(this);
            if (m_nPop >= 0)
                ws.m_nFrame = e_bsFocus;
            ws.RenderBk(gapp, rc);
        }
    }

    if (!m_menu.hMenu)
        return;

    CView* ps = GetView();
//    TMBool bDown = false;
    int nHitID = m_nPop;//-1;
    if (m_nPop < 0 && ps->GetOver() == this)
    {
        const TUIHITINFO& ht = ps->GetHitInfo();
        nHitID = ht.bNC ? -1 : ht.dwCode;
    }

    // make menu data
    if (m_aRight.empty())
    {
        GXDips nRight = 0, nHeight = 0;
        TMCharT c[256];
        MENUITEMINFO mi = { sizeof(MENUITEMINFO), MIIM_TYPE };
        for (int i = 0; ; ++i)
        {
            // get the menu item information
            mi.cch = 255;
            mi.dwTypeData = c;
            if (0 == GetMenuItemInfo(m_menu.hMenu, i, true, &mi))
                break;

            // make item data.
            ITEM itm;
            m_aRight.push_back(itm);
            m_aRight[i].gxText.Create(c);
            m_aRight[i].gxText->SetFormat(DT_SINGLELINE | DT_VCENTER | DT_CENTER);
            GXSize sz(0,0);
            m_aRight[i].gxText->CalcSize(sz, gapp);
            tm_setmax(nHeight, sz.Height());
            nRight += (sz.cx + m_bdItem.left + m_bdItem.right);
            m_aRight[i].nRight = nRight;
        }

        // make scroll range.
        SendScrollRangeMessage(&nRight, &nHeight);
    }
    
    // render
    DbgAssert(m_aRight.size());
    gapp->Push(eGXPropTextColor);
    CPropColor clrs[] = { m_clrNormal, m_clrHilight, m_clrHilight, GetView()->m_clrDisableText, m_clrHilight };
    CPropColor* clrss[] = { clrs, 0 };
    for (int i = 0; i < m_aRight.size(); ++i)
    {
        DbgAssert(m_aRight[i].gxText);
        int nFrame = 0;
        if (nHitID == i)
            nFrame = IsPriInPop() ? e_bsDown : e_bsHigh; // (m_dwPriStyle & MS_PRI_INPOP) ? e_bsDown : e_bsHigh;

        // text color
        RenderSetTextColor(gapp, nFrame, clrss, CAppColor::GetColorMenuItemText);
//         if ((eVisual & eColorVisualHighContrast))
//             clr = CAppColor::GetColorMenuItemText(eVisual, nFrame);// HC_GetColorT(g_clrHCMenuItemText, nFrame);
//         else
        {
//         TMColor clr = CalcTextColor(eVisual, nFrame, clrss, CAppColor::GetColorMenuItemText);
// //             clr = GetModule()->ConvertColor(GetObjectID(), nHitID == i ? m_clrHilight : m_clrNormal);
//         gxSetTextColor(clr);
        }

        // draw button background 
        rc.right = m_rcClient.Left() + m_aRight[i].nRight;
        if (m_pFore)
        {
            ITuiPic* pFore = bHighContrast ? GetPrePic(MENU) : m_pFore;
            pFore->Render(nFrame, 0, rc, 0, gapp);
        }

        // draw text
        CTuiRect rcx(rc);
        rcx.InflateRect(-m_bdItem.left, -m_bdItem.top, -m_bdItem.right, -m_bdItem.bottom);
        m_aRight[i].gxText->Paint(rcx, 0, 0, gapp);
        rc.left = rc.right;
    }
    gapp->Pop();
}

void CTuiMenuBarImpl::Reclaim()
{
    m_aRight.clear();
    CForePicWin<CTuiWgt>::Reclaim();
}

int CTuiMenuBarImpl::HitTest(const TUIPoint& pt, TMBool& bHandled)
{
    if (m_rcClient.PtInRect(pt.x, pt.y))
    {
        GXDips x = m_rcClient.Left();
        int i = 0;
        for (CWidthRec rec(m_aRight); rec; ++rec, ++i)
        {
            ITEM& itm = *rec;
            if (pt.x < x + itm.nRight)
            {
//                DbgTrace ("hittest:%d", i);
                return i;
            }
        }
        return -1;
    }

    bHandled = false;
    return -1;
}

CPoint CTuiMenuBarImpl::GetTrackMenuPos(int nHot)
{
    DbgAssert(nHot>=0 && nHot<m_aRight.size());

    CTuiPoint pt(m_rcClient.Left(), m_rcClient.bottom);
    GXDips x = nHot > 0 ? m_aRight[nHot - 1].nRight : 0;
    pt.x += x;
    CPoint ptx;
    ClientToScreen(&pt, 1, &ptx);
    return ptx;
}

TMResult CTuiMenuBarImpl::OnHitTest(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    TUIHITINFO* pht = (TUIHITINFO*)lParam;
    pht->dwCode = HitTest(pht->pt, bHandled);
    return 0;
}

TMResult CTuiMenuBarImpl::OnLButtonDown(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    CTuiPoint pt(lParam);
    _TraceMenu(this).MouseMenu(pt);
    RedrawWindow ();
    return 0;
}

TMResult CTuiMenuBarImpl::OnLButtonUp(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    RedrawWindow ();
    return 0;
}

TMResult CTuiMenuBarImpl::OnMouseEntry(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    TUIHITINFO* p = (TUIHITINFO*)lParam;
    if (!p->bNC && m_nPop >= 0)
        m_nPop = p->dwCode;
    Invalidate();
    return 0;
}

TMResult CTuiMenuBarImpl::OnMouseLeave(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    TUIHITINFO* p = (TUIHITINFO*)lParam;
//        DbgTrace ("CTuiMenuBarImpl::OnMouseLeave %d, %d, %x, %x", GetTickCount(), m_bAlt, p->hWnd, this);
    if (!IsPriInPop() && (p->hWnd != this))
        m_nPop = -1;
    Invalidate();
    return 0;
}

TMResult CTuiMenuBarImpl::OnSetMenu(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_menu.Set((TMHModule)wParam, (int)lParam);
    m_aRight.clear();
    Invalidate();
    return 0;
}

TMResult CTuiMenuBarImpl::OnSetHMenu(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_menu.Set((HMENU)lParam);
    m_aRight.clear();
    Invalidate();
    return 0;
}

TMResult CTuiMenuBarImpl::OnGetMenuInfo(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    CTuiMenuBar::ITEMINFO* info = (CTuiMenuBar::ITEMINFO*)lParam;
    if (wParam < 0 || wParam >= m_aRight.size())
        return false;
    else
    {
        GetClientRect(&info->rc);
        int nLeft = info->rc.left;
        if (wParam != 0)
            info->rc.left += m_aRight[wParam - 1].nRight;
        ITEM& itm = m_aRight[wParam];
        info->rc.right = nLeft + itm.nRight;
        itm.gxText.GetString().CopyTo(info->text);
    }
    return true;
}

TMResult CTuiMenuBarImpl::OnSetAltMode(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (IsPriInAlt()) // m_dwPriStyle & MS_PRI_INALT)
        LeaveAltMode();

    if (wParam)
    {
        if (EntryAltMode())
        {
            InternalSetPop(0);
            return true;
        }
    }
    else
        return true;

    return false;
}

TMResult CTuiMenuBarImpl::OnLanguageChanged(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (m_menu.nID)
        m_menu.ReLoad();
    bHandled = false;
    return 0;
}

TMBool CTuiMenuBarImpl::EntryAltMode()
{
    if (IsPriInAlt())
        return true;

    if (!IsWindowEnabled() ||
        !(m_dwStyle & (MS_AUTOHIDE | WS_VISIBLE)) ||
        !m_menu.hMenu ||
        !GetParent()->CanInput())
        return false;

//    _tuiApp.AddMessageFilter(this);
//     SetHook();
    DISABLE_SPY();
    CTuiWidget(this).ShowWindow(SW_SHOW);
    Invalidate();
    SetPriInAlt(); // m_dwPriStyle |= MS_PRI_INALT;
    Notify(BN_SETFOCUS);
    return true;
}

void CTuiMenuBarImpl::LeaveAltMode()
{
    if (!IsPriInAlt())
        return;
    ResetPriInPop();
    ResetPriInAlt();
    InternalSetPop(-1);
    Notify(BN_KILLFOCUS);
    DISABLE_SPY();
    if (m_dwStyle & MS_AUTOHIDE)
        CTuiWidget(this).ShowWindow(SW_HIDE);
    Invalidate();
    return;
}

TMBool CTuiMenuBarImpl::OnSysChar(TMCharT c)
{
    if (!EntryAltMode())
        return false;
    if (_TraceMenu(this).KeyMenu(c))
        return true;
    LeaveAltMode();
    return false;
}

BOOL CTuiMenuBarImpl::PreTranslateMessage(MSG* pMsg)
{
    if (IsPriInPop() || !IsWindowEnabled())
        return false;

    Ref _ref(this);
    
    if (pMsg->message == WM_SYSCHAR)
        return OnSysChar(pMsg->wParam);

    if (!IsPriInAlt())
        return false;

    if (pMsg->message == WM_KEYDOWN)
    {
        TMCharT c = GetModule()->GetLocal().ConvertKey(pMsg->wParam);
        if (c == VK_RIGHT)
            return (ShowMenuItem (m_nPop + 1), true);
        else if (c == VK_LEFT)
            return (ShowMenuItem (m_nPop - 1), true);
        else if (c == VK_DOWN || c == VK_EXECUTE || c == VK_RETURN)
            return (_TraceMenu(this).TrackMenu(m_nPop, WM_KEYDOWN), true);
        else if (c == VK_ESCAPE || c == VK_TAB)
            return (LeaveAltMode(), true);
        return false;
    }
    else if (pMsg->message == WM_CHAR)
    {
        if (!_TraceMenu(this).KeyMenu(pMsg->wParam))
            LeaveAltMode();
        return true;
    }
    else if ((pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_RBUTTONDOWN || pMsg->message == WM_MBUTTONDOWN))
    {
        TMHWnd hWnd = GetHWND();
        if (pMsg->hwnd == hWnd)
        {
            TMBool bHandle = true;
            CPoint pt(pMsg->lParam);
            CTuiPoint ptl;
            GetDPI().DPtoLP(pt, ptl);
            int id = HitTest(ptl, bHandle);
            if (!bHandle)
                LeaveAltMode();
        }
        else
            LeaveAltMode();
    }
    else if ((pMsg->message == WM_NCLBUTTONDOWN || pMsg->message == WM_NCRBUTTONDOWN || pMsg->message == WM_NCMBUTTONDOWN))
    {
        LeaveAltMode();
    }

    return false;
}

void CTuiMenuBarImpl::ShowMenuItem(int nPos)
{
    int nItem = GetMenuItemCount(m_menu.hMenu);
    if (nPos >= nItem)
        InternalSetPop(0);
    else if (nPos < 0)
        InternalSetPop(nItem - 1);
    else
        InternalSetPop(nPos);
    Invalidate();
}

void CTuiMenuBarImpl::InternalSetPop(int nPos)
{
    if (m_nPop == nPos)
        return;
    m_nPop = nPos;
    if (m_nPop >= 0)
        Notify(BN_HILITE);
    else
        Notify(BN_UNHILITE);
}


TM_REGISTER_CLASS1 (CTuiMenuBar);


TM_END_NAMESPACE

