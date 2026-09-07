/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.09.14
 *
 *  Copyright (C) 2006 miragekiller
 */

#include "uiwindow.h"
#include "uibutton.h"
#include "view.h"



TM_BGN_NAMESPACE



////////////////////////////////////////////////////////////////////////////////
// class CTuiWnd
CTuiWnd::CTuiWnd()
    : m_nDefaultItem(IDOK)
{
    m_rcWnd.SetRect(0, 0, 500, 300);
    m_dwStyle |= (WS_OVERLAPPEDWINDOW | SS_DLGBKONHC);
    m_dwPriStyle |= WS_PRI_IS_WINDOW;
    m_dwLayout = ANCHOR_NONE | ANCHOR_VNONE;
}

CTuiWnd::~CTuiWnd()
{
}

TMBool CTuiWnd::TranslateAcceleratorKey(TMMSG* pMsg)
{
    TMHWnd hWnd = GetHWND();
    
    if (m_aHotKeyMgr.TranslateHotKeyMT(pMsg->message, pMsg->wParam, pMsg->lParam))
        return true;
    
    return false;
}

TMBool CTuiWnd::InitManager(TUIFACTORY* pFactory, void* param)
{
    InitConstruct(pFactory, param);
    
    // set default focus
    if (!(GetStyle() & WS_CHILD) && m_nDefaultItem)
        InitWndFocusBackup();

    // send WM_INITDIALOG message
    SendMessage(WM_INITDIALOG, 0, (TMLParam)param);

    return !IsLifeDestroyed();
}

void CTuiWnd::SetDefaultFocus(TMUInt uShowFlag)
{
    if (uShowFlag & SWP_SHOWWINDOW)
    {
        if (!GetView()->IsFocuing())
        {
            HTUI h = m_wndFocusBackup;
            if (h)
            {
                if (/*h->IsTabGroup() && h->GetParent() &&*/ h->CanInput())
                    GetView()->SetFocus(h, eFocusReasonActiveWindow);
                else
                    h->SetNextFocus(false, eFocusReasonActiveWindow, !(uShowFlag & SWP_NOACTIVATE));
                //                 DbgTrace("CTuiWnd_Win(%d)::restore focus(%d)", p->GetObjectID(), h->GetObjectID());
            }
        }
    }
    else
    {
        if (GetStyle() & WS_CHILD)
            m_wndFocusBackup = 0;
        else
        {
            HTUI wndFocus = GetView()->GetFocus();
            if (wndFocus && (this->IsChild(wndFocus) || CWindowAT(GetHWND()).IsChild(wndFocus->GetHWND())))
                m_wndFocusBackup = wndFocus;
//             else
//                 InitWndFocusBackup();
        }
        
        //         m_wndFocusBackup = (GetStyle() & WS_CHILD) ? 0 : GetView()->GetFocus();
        if (m_wndFocusBackup)
            DbgTrace("CTuiWnd(%d)::backup focus(widget:%d)", GetObjectID(), m_wndFocusBackup.m_hWnd->GetObjectID());
        else
            DbgTrace("CTuiWnd(%d)::backup focus(widget:%d)", GetObjectID(), 0);
    }
}

void CTuiWnd::InitWndFocusBackup()
{
    if (GetObjectID() == m_nDefaultItem)
        m_wndFocusBackup = this;
    else
        m_wndFocusBackup = GetDlgItem(m_nDefaultItem);
}

TMBool CTuiWnd::ProcessWindowMessage(
    CTuiWgt* hWnd, TMUInt uMsg, TMWParam wParam,
    TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)
{
    if (uMsg == CM_SETDEFAULTFOCUS)
        return (m_wndFocusBackup = (HTUI)lParam), true;
    else if (uMsg == CM_SETDEFAULTITEM)
        return m_nDefaultItem = lParam, true;
    else if (uMsg == WM_CLOSE)
        return PostMessage(WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), 0);
    return CTuiLayerWnd::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
}

#ifdef TUI_EDIT_MODE

CString CTuiWnd::GetDebugInfo()
{
    CString str(CTuiLayerWnd::GetDebugInfo());
    str += L"\r\n\r\nMananger sub class functions:";
    CTuiWnd::CWndProcMgr& aProc = m_mgrWndProc;
    str += _Debug_MakeDelegatesInfo(aProc);

    if (m_wndFocusBackup)
    {
        CString strBkup;
        strBkup.Format(L"\r\nwndFocusBackup:%p(%d)", m_wndFocusBackup.m_hWnd, m_wndFocusBackup.m_hWnd->m_dwObjID);
        str += strBkup;
    }
    else
        str += L"\r\nwndFocusBackup:0";

    CString strHotkey(CKeyboardDebugInfo::HotKeys2String(m_aHotKeyMgr, L"\r\n  "));
    if (!strHotkey.IsEmpty())
    {
        str += L"\r\nHotKeys:";
        str += strHotkey;
    }
    return str;
}

#endif // TUI_EDIT_MODE


TM_BGN_PRIVATE_NAMESPACE


TMBool tuiSubclassMgr(HTUI hMgr, const CWindowMsgDelegate& func)
{
    ASSERT_CLASS(hMgr);
    LOG_FUNC_FOR_SPY(hMgr, L"tuiSubclassMgr(func:0x%p)", &func);
    if (hMgr->IsLifeInvalid())
        return false;
    CTuiWnd* pMgr = static_cast<CTuiWnd*>(hMgr);
    pMgr->SubclassMgr(func);
    pMgr->EnumResetRoutePrintClient();
    return true;
}

bool tuiUnsubclassMgr(HTUI hMgr, const CWindowMsgDelegate& func)
{
    ASSERT_CLASS(hMgr);
    LOG_FUNC_FOR_SPY(hMgr, L"tuiUnsubclassMgr(func:0x%p)", &func);
    CTuiWnd* pMgr = static_cast<CTuiWnd*>(hMgr);
    pMgr->EnumResetRoutePrintClient();
    return pMgr->UnsubclassMgr(func);
}

TMResult tuiDefSubclassMgrProc(HTUI hMgr, TMUInt uMsg, TMWParam wp, TMLParam lp)
{
    ASSERT_CLASS(hMgr);
    CTuiWnd* pMgr = static_cast<CTuiWnd*>(hMgr);
    TMResult lr = 0;
    pMgr->m_mgrWndProc.CallNext(pMgr->GetHWND(), uMsg, wp, lp, lr);
    return lr;
}

TM_END_PRIVATE_NAMESPACE


TM_END_NAMESPACE
