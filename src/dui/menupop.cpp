/**
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.06.19
 *
 *  Copyright (C) 2018 miragekiller                               
 */

#include "menupop.h"
#include "view.h"
#include "platform.h"
#include "uiwindow.h"
#include "tmformatmsg.h"
#include "tooltip.h"


TM_BGN_NAMESPACE


// #define TRACE_MENUPOP

#ifdef TRACE_MENUPOP
#   define tm_trace_menu DbgTrace
#else
#   define tm_trace_menu DbgTraceNull
#endif

///////////////////////////////////////////////////////////
// CMenuPop

// if hWnd not in one of pop chain, close all pop
void CMenuPop::_OnCancelModeMsg(HTUI hWnd, TUIMSG* pMsg, int idRet)
{
    tm_trace_menu(TM_T("CMenuPop::_OnCancelModeMsg(hWnd:%x, msg:%s)"), hWnd, pMsg ? DbgFormatMessage(pMsg->message).c_str() : TM_T(""));
    DbgAssert(hWnd);

    // cancel next pop
    for (CPopList::reverse_recorder rec(m_aPops); rec; ++rec)
    {
        CMenuPop::POPWND& pw = *rec;
        if (hWnd == pw.hOwner)
        {
            pw.hPop->SendMessage(WM_CANCELMODE, idRet, (TMLParam)pMsg);
            if (HavePop())
            {
                // the hWnd is last pop's owner
                CMenuPop::POPWND pw2 = m_aPops.back();
                if (hWnd == pw2.hOwner)
                    pw2.hPop->SendMessage(CM_ENDMENUPOP, idRet, (TMLParam)pMsg);
                else
                    tm_trace_menu(L"CMenuPop::_OnCancelModeMsg: cancelmode fail(hWnd:%x)", pw.hPop);
            }
            else
                tm_trace_menu(L"CMenuPop::_OnCancelModeMsg: cancelmode fail(hWnd:%x)", pw.hPop);
            return;
        }
        else if (hWnd->IsChild(pw.hOwner))
        {
            pw.hOwner->SendMessage(WM_CANCELMODE, idRet, (TMLParam)pMsg);
            return;
        }
    }
}

void CMenuPop::_OnLButtonDownMsg(TUIMSG* pMsg)
{
    DbgAssert(HavePop());

//     int n = GetMenuPopLevel(pMsg->hWnd, false);
//     EndMenuPopLevel(n + 1, IDCANCEL, false, pMsg);

    int n = m_aPops.size() - 1;
    for (; n >= 0; --n)
    {
        CMenuPop::POPWND& pw = m_aPops[n];
        if (pMsg->hWnd == pw.hOwner || pw.hOwner->IsChild(pMsg->hWnd))
            return;
        if (pMsg->hWnd == pw.hPop || pw.hPop->IsChild(pMsg->hWnd))
        {
            // close all child menu pop window.
            _CancelMode(pw.hPop, false, pMsg, IDCANCEL);
            return;
        }
    }
    
    Clear(true, pMsg);
}

void CMenuPop::_OnHideMsg(TUIMSG* pMsg)
{
    tm_trace_menu(TM_T("CMenuPop::_OnHideMsg(hWnd:%x, msg:%s)"), pMsg->hWnd, pMsg ? DbgFormatMessage(pMsg->message).c_str() : TM_T(""));
    for (CPopRec rec(m_aPops); rec; ++rec)
    {
        CMenuPop::POPWND pw = *rec;
        if (pMsg->hWnd == pw.hOwner)
        {
            pw.hPop->SendMessage(CM_ENDMENUPOP, IDCANCEL, (TMLParam)pMsg);
            return;
        }
        else if (pMsg->hWnd == pw.hPop)
        {
            rec.erase();
            if (rec)
            {
                pw = rec.get_data();
                pw.hPop->SendMessage(CM_ENDMENUPOP, IDCANCEL, (TMLParam)pMsg);
            }
            else if (!HavePop())
            {
                tm_trace_menu(L"CMenuPop::_OnHideMsg remove hook");
//                 hook_class::RemoveHook();
//                LLHook::Close();
                WIN::HookGlobalMouseDown(0);
                pw.hOwner->SendMessage(CM_UNINITMENUPOP, (TMWParam)(HTUI)pw.hOwner, (TMLParam)(HTUI)pw.hPop);
            }
            return;
        }
    }
}

void CMenuPop::_CancelMode(HTUI hOwner, TMBool bRestoreFocus, TUIMSG* pMsg, int idRet)
{
    tm_trace_menu(TM_T("CMenuPop::_CancelMode(hWnd:%x, msg:%s)"), hOwner, pMsg ? DbgFormatMessage(pMsg->message).c_str() : TM_T(""));
    DbgAssert(hOwner);

    Ref ref(hOwner);
    hOwner->SendMessage(WM_CANCELMODE, idRet, (TMLParam)pMsg);
    if (bRestoreFocus && !hOwner->GetView()->GetFocus())
        hOwner->GetView()->CheckSetFocus(hOwner, false, false, eFocusReasonPopup);
}

void CMenuPop::Clear(TMBool bRestoreFocus, TUIMSG* pMsg)
{
    tm_trace_menu(TM_T("CMenuPop::Clear(hWnd:%x, msg:%s)"), pMsg->hWnd, pMsg ? DbgFormatMessage(pMsg->message).c_str() : TM_T(""));

    if (HavePop())
        _CancelMode(m_aPops.begin()->hOwner, bRestoreFocus, pMsg, IDCANCEL);
}

void CMenuPop::_ResetLast(TUIMSG* pMsg)
{
    tm_trace_menu(TM_T("CMenuPop::_ResetLast(hWnd:%x, msg:%s)"), pMsg->hWnd, pMsg ? DbgFormatMessage(pMsg->message).c_str() : TM_T(""));
    DbgAssert(HavePop());
    _CancelMode(m_aPops.back().hOwner, true, pMsg, IDCANCEL);
}

void CMenuPop::_ForwardCommandMessage(HTUI hWgt, TMWParam wp, TMLParam lp)
{
    DbgAssert(HavePop());
    HTUI hOwner = m_aPops[0].hOwner;
    if (hOwner->GetOwner() != hWgt)
        hOwner->SendMessage(WM_COMMAND, wp, lp);
}

TMBool CMenuPop::ProcessMessageWin(HTUI hMgr, TMUInt message, TMWParam wp, TMLParam lp)
{
    if (!HavePop())
        return false;

    switch (message)
    {
    case WM_ACTIVATEAPP:
    case WM_CANCELMODE:
        Clear(WM_ACTIVATEAPP != message, tm_addressof(CTuiMsg(hMgr, message, wp, lp)));
        return hMgr ? hMgr->IsLifeDestroyed() : false;

    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONDBLCLK:
    case WM_NCRBUTTONDOWN:
    case WM_NCRBUTTONDBLCLK:
        _OnLButtonDownMsg(tm_addressof(CTuiMsg(hMgr, message, wp, lp)));
        break;
        
    case WM_SYSKEYDOWN:
        if (wp == VK_MENU)
        {
            Clear(true, tm_addressof(CTuiMsg(hMgr, message, wp, lp)));
            return true;
        }
        break;
        
//     case WM_KEYDOWN:
//         if (wp == VK_ESCAPE)
//         {
//             _ResetLast(&CTuiMsg(hMgr, message, wp, lp));
//             return true;
//         }
//         break;
// 
//     case CM_MSGBUBBLE:
//         {
//             TUIMSG* msg = (TUIMSG*)lp;
//             if (msg->message == WM_KEYDOWN && msg->wParam == VK_ESCAPE)
//             {
//                 _ResetLast(msg);
//                 return true;
//             }
//         }
//         break;

    case WM_COMMAND:
        _ForwardCommandMessage(hMgr, wp, lp);
        break;
    }
    return false;
}

TMBool CMenuPop::ProcessMessageWgt(HTUI hWnd, TMUInt message, TMWParam wp, TMLParam lp)
{
    if (!HavePop())
        return false;

    switch (message)
    {
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_NCLBUTTONDOWN:
    case WM_NCRBUTTONDOWN:
    case WM_NCMBUTTONDOWN:
//         case WM_SETFOCUS
        if (!g_mgrTooltip.IsTooltipChild(hWnd))
        {
            _OnLButtonDownMsg(tm_addressof(CTuiMsg(hWnd, message, wp, lp)));
            return hWnd->IsLifeDestroyed();
        }
        break;

    case CM_SHOWWINDOW_P:
    case CM_ENABLE_P:
    case WM_ENABLE:
    case WM_SHOWWINDOW:
        if (!wp)
        {
            _OnHideMsg(tm_addressof(CTuiMsg(hWnd, message, wp, lp)));
            return hWnd->IsLifeDestroyed();
        }
        break;

    case CM_ENDMENUPOP:
    case WM_DESTROY:
        _OnHideMsg(tm_addressof(CTuiMsg(hWnd, message, wp, lp)));
        return hWnd->IsLifeDestroyed();
    }
    
    return false;
}

TMBool CMenuPop::IsMenuPopMessage(CTuiWnd* hMgr, TUIMSG* pMsg)
{
    if (!HavePop())
        return false;

    if (pMsg->message == WM_CANCELMODE)
    {
        _OnCancelModeMsg(pMsg->hWnd, pMsg->lParam ? (TUIMSG*)pMsg->lParam : pMsg, pMsg->wParam);
        return true;
    }
    else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
    {
        _ResetLast(pMsg);
        return true;
    }
    
    return false;
}

void CMenuPop::DoMenuPop(HTUI hOwner, HTUI hWnd, HTUI hFocus)
{
    tm_trace_menu(L"CMenuPop::DoMenuPop(hOwner:%x, hWnd:%x)", hOwner, hWnd);
    if (!hOwner || !hWnd || hOwner->IsLifeInvalid() || hWnd->IsLifeInvalid())
        return;

    // Create Hook
    if (!HavePop())
    {
        tm_trace_menu(L"CMenuPop::DoMenuPop set hook");
        if (!WIN::HookGlobalMouseDown(hOwner))
            return;
    }
    
    CMenuPop::POPWND pw = { hOwner, hWnd };
    m_aPops.push_back(pw);

    Ref ref1(hFocus);
    Ref ref2(hOwner);
    Ref ref3(hWnd);

    if (hWnd->Is_WS_PRI_IS_WINDOW())
        CWindowAT(hWnd->GetHWND()).ShowWindow((hWnd->GetExStyle() & WS_ADV_NOACTIVATE) ? SW_SHOWNOACTIVATE : SW_SHOWNORMAL);
    else
        { DISABLE_SPY(); CTuiWidget(hWnd).ShowWindow(SW_SHOW); }

    if (hFocus)
        hFocus->SetFocus(eFocusReasonPopup);

    if (!hOwner->IsLifeInvalid() && !hWnd->IsLifeInvalid())
        hOwner->SendMessage(CM_INITMENUPOP, (TMWParam)(HTUI)hOwner, (TMLParam)hWnd);
}

HTUI CMenuPop::GetMenuPop(HTUI hWnd)
{
    tm_trace_menu(L"CMenuPop::GetMenuPop(hWnd:%x)", hWnd);
    if (m_aPops.empty())
        return 0;
    
    for (CMenuPop::CPopRec rec(m_aPops); rec; ++rec)
    {
        CMenuPop::POPWND pw = *rec;
        if (hWnd == pw.hOwner || hWnd->IsChild(pw.hOwner))
            return pw.hPop;
    }
    
    return 0;
}

// void CMenuPop::_GetMenuPopLevel(HTUI hWnd, int& nOwner, int& nPop)
// {
//     tm_trace_menu(L"CMenuPop::_GetMenuPopLevel(hWnd:%x)", hWnd);
//     nOwner = nPop = -1;
//     for (int i = m_aPops.size() - 1; i >= 0; --i)
//     {
//         CMenuPop::POPWND& pw = m_aPops[i];
//         if (nOwner < 0 && hWnd == pw.hOwner || hWnd->IsChild(pw.hOwner))
//             nOwner = i;
//         if (nPop < 0 && hWnd == pw.hPop || pw.hPop->IsChild(hWnd))
//             nPop = i;
//         if (nOwner >= 0 || nPop >= 0)
//             return;
//     }
// }

int CMenuPop::GetMenuPopLevel(HTUI hWnd, TMBool bOwner)
{
    tm_trace_menu(L"CMenuPop::GetMenuPopLevel(hWnd:%x)", hWnd);
    int n = m_aPops.size() - 1;
    for (; n >= 0; --n)
    {
        CMenuPop::POPWND& pw = m_aPops[n];
        if (bOwner)
        {
            if (hWnd == pw.hOwner || hWnd->IsChild(pw.hOwner))
                return n;
        }
        else
        {
            if (hWnd == pw.hPop || pw.hPop->IsChild(hWnd))
                return n;
        }
    }
    return -1;
}

TMBool CMenuPop::EndMenuPopLevel(int nLevel, int nResult, TMBool bRestoreFocus, TUIMSG* p)
{
    tm_trace_menu(L"CMenuPop::EndMenuPopLevel()");

    if (nLevel < 0 || nLevel >= m_aPops.size())
        return false;
    
    CMenuPop::POPWND pw = m_aPops[nLevel];
    _CancelMode(pw.hOwner, bRestoreFocus, p, nResult);
    
    return true;
}

TMBool CMenuPop::GetMenuPopInfo(int nLevel, HTUI* pOwner, HTUI* pPop)
{
    tm_trace_menu(L"CMenuPop::GetMenuPopInfo()");
    if (nLevel < 0 || nLevel >= m_aPops.size())
        return false;
    
    CMenuPop::POPWND pw = m_aPops[nLevel];
    if (pOwner)
        *pOwner = pw.hOwner;
    if (pPop)
        *pPop = pw.hPop;
    return true;
}

CMenuPop g_menuPop;



TM_END_NAMESPACE



