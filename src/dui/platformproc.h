/*
 *  @file
 *  @brief  WNDPROC support for manager
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2020.8.15
 *
 *  Copyright (C) 2020 miragekiller
 */

#pragma once

#ifndef PLATFORMPROC_H
#define PLATFORMPROC_H

#include "uiwindow_win_theme.h"
#include "menupop.h"


TM_BGN_NAMESPACE


#ifdef TM_WIN

struct CRemoveAeroSnap : public CHookKeyboardLLImpl<CRemoveAeroSnap>
{
public:
    ~CRemoveAeroSnap()
    {
        RemoveHook();
    }
    
    void Set()
    {
        RemoveHook();
        SetHook();
    }

    // CHookKeyboardLLImpl
    TMBool OnKeyboardLL(UINT uMsg, KBDLLHOOKSTRUCT* ps)
    {
        if (uMsg == WM_KEYDOWN)
        {
            if (ps->vkCode == VK_LEFT || ps->vkCode == VK_UP || ps->vkCode == VK_DOWN || ps->vkCode == VK_RIGHT)
                return true;
        }
        else if (uMsg == WM_KEYUP)
        {
            if (ps->vkCode == VK_LWIN || ps->vkCode == VK_RWIN)
                RemoveHook();
        }

        return false;
    }
};

#endif // TM_WIN

inline TMBool _ProcessMgrNativeMessage(CTuiWnd_Platform& mgr, TMHWnd hWnd, TMUInt uMsg, TMWParam wParam, 
                                       TMLParam lParam, TMResult& lResult,
                                       TMULong dwMsgMapID)
{
    if (dwMsgMapID != 0)
        return false;

    DbgAssert(hWnd == mgr.GetHWND());
    DbgAssert(hWnd);

    if (uMsg >= WM_USER && uMsg < WM_APP)
    {
        lResult = mgr.SendMessage(uMsg, wParam, lParam, CTuiWgt::eSendMsgProc);
        return true;
    }

    if (g_menuPop.ProcessMessageWin(&mgr, uMsg, wParam, lParam))
        return true;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        return lResult = 1;

    case WM_SETCURSOR:
        if (TM_LOWORD(lParam) == HTCLIENT)
            return lResult = 1;
        break;

    case WM_NCDESTROY:
        mgr.m_dwPriStyle |= MS_PRI_IS_DESTROYWIN;
        mgr.CTuiWgt::DestroyWindow();
//         mgr.m_pTheme = ISysFrame::GetSysFrame();
//        CallFlush();
#ifdef TM_WIN
        ::PostMessage(0, WM_NULL, 0, 0); // let GetMessage continue;
#endif
        break;

    case CM_SETALPHA:
        {
            TMBool b;
            mgr.OnSetAlpha(uMsg, wParam, lParam, b);
        }
        return true;

    case WM_MOUSEACTIVATE:
        if (WS_ADV_NOACTIVATE & mgr.m_dwAdvStyle)
            return lResult = MA_NOACTIVATE, true; //WIN::OnMouseActivate(m_hWnd, ::GetAncestor(hWnd, GA_ROOTOWNER), uMsg, wParam, lParam);
        break;

    case WM_NCACTIVATE:
    case WM_ACTIVATE:
    case WM_ACTIVATEAPP:
    case WM_HOTKEY:
    case WM_DPICHANGED:
    case WM_GETDPISCALEDSIZE:
    case WM_POINTERDOWN:
    case WM_ENTERMENULOOP:
    case WM_EXITMENULOOP:
    case WM_INITMENU:
    case WM_INITMENUPOPUP:
    case WM_UNINITMENUPOPUP:
    case WM_SETTINGCHANGE:
    case WM_DISPLAYCHANGE:
        // let window receive this message
        mgr.SendMessage(uMsg, wParam, lParam);
        break;

    case WM_CLOSE:
    case WM_GETMINMAXINFO:
        return mgr.SendMessage(uMsg, wParam, lParam);

#ifdef TM_QT
    case WM_SYSCOMMAND:
        return mgr.DefWclProc(uMsg, wParam, lParam, lResult);

#else
    case WM_GETDLGCODE:
//         if (lParam && ((TMMSG*)lParam)->message == WM_KEYDOWN && ((TMMSG*)lParam)->wParam == VK_TAB)
//             lResult = mgr.GetView()->GetNextFocusDlgCode(CKeyboard::IsKeyDownShift());
//         else
//             lResult = DLGC_WANTALLKEYS | DLGC_WANTMESSAGE;
        lResult = DLGC_WANTALLKEYS | DLGC_WANTTAB | DLGC_WANTARROWS;
        return true;

    case WM_KEYDOWN:
        if ((wParam == VK_LWIN || wParam == VK_RWIN) && !(mgr.m_dwStyle & WS_MAXIMIZEBOX))
        {
            static CRemoveAeroSnap s_ras;
            s_ras.Set();
        }
        break;

    case WM_SYSCOMMAND:
        if (wParam == SC_MOUSEMENU || wParam == SC_KEYMENU)
            return mgr.TrackSysMenu(wParam), true;
        break;

    case WM_POPUPSYSTEMMENU:
        mgr.TrackSysMenu(lParam, tm_addressof(CPoint(lParam)));
        return true;
#endif

    case WM_NEXTMENU:
        return true;

    case WM_GETOBJECT:
        if (Edit_IsObjectInSpyMode((&mgr)) && WIN::OnGetObject(&mgr, wParam, lParam, lResult))
            return true;
        break;
            
    case CM_GET_WIDGET:
        lResult = (TMResult)(HTUI)(&mgr);
        return true;

    case WM_DWMCOMPOSITIONCHANGED:
        mgr.SetTheme(eThemeTypeAuto, true);
        mgr.SendMessage(uMsg, wParam, lParam);
        break;

    case CM_RENDERCHANGED:
        struct _SetThememFrame
        {
            TM_DECLARE_ASYNC_REF_CALL(CTuiWnd_Platform*);
            static void Done(CTuiWnd_Platform* mgr)
            {
                if (!mgr->IsLifeInvalid())
                    mgr->SetTheme(eThemeTypeAuto, true);
            }
        };
        _SetThememFrame::Call(&mgr);
        break;

//     case WM_CLOSE:
//         mgr.PostMessage(WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), 0);
//         return true;

//     case WM_INPUTLANGCHANGEREQUEST:
//         ActivateKeyboardLayout ((HKL)lParam, KLF_SETFORPROCESS);
//         return true;

//     case WM_ERASEBKGND:
// //         _DBG_CopyDC((HDC)wParam, m_hWnd, GetObjectID());
// //         _DBG_CopyDC(CWindowDC(0), m_hWnd, GetObjectID());
// //        ::FillRect((HDC)wParam, &CRect(0,0,1024,800), (HBRUSH)GetStockObject(WHITE_BRUSH));
//         return true;
                
    case WM_PAINT:
        if (CWindowAT(hWnd).IsIconic())
            break;
//         {
//             CPaintDC dc(hWnd);
//             mgr.MergeNativeInvalidateRect(hWnd, dc.m_ps.rcPaint);
//         }
        mgr.RealInvalidate(true, true);
        return lResult;
        
    case CM_MSGBUBBLE:
        {
            TUIMSG* pMsg = (TUIMSG*)lParam;
            if (g_menuPop.IsMenuPopMessage(&mgr, pMsg))
                return true;
            if (mgr.GetView()->IsDialogMessage(&mgr, pMsg))
                return true;

            TMHWnd hReal = mgr.GetHWND();
            TMMSG msg;
            msg.hwnd = hReal;
            msg.wParam = pMsg->wParam;
            msg.lParam = pMsg->lParam;
            msg.message = pMsg->message;
            if (mgr.TranslateAcceleratorKey(&msg))
                return lResult = true;
        
            // not child window or tab group window
            TMBool bChild = (CWindowAT(hWnd).GetStyle() & WS_CHILD);
            if (bChild && !mgr.IsTabGroup() && !mgr._parent)
            {
                for (; hReal = CWindowAT(hReal).GetParent();)
                {
                    HTUI hWidget = (HTUI)CWindowAT(hReal).SendMessage(CM_GET_WIDGET, 0, 0);
                    if (hWidget)
                        return lResult = hWidget->SendMessage(uMsg, wParam, lParam, CTuiWgt::eSendMsgProc);
                }
            }
        }
        return mgr.CTuiWgt::ProcessWindowMessage(&mgr, uMsg, wParam, lParam, lResult, dwMsgMapID);

    default:
        break;
    }

    if (mgr.GetView()->DispatchAppMessage(&mgr, hWnd, uMsg, wParam, lParam, lResult))
        return true;

    if (mgr.GetView()->IsBasicMouseMessage(uMsg))
    {
        mgr.GetView()->DispatchBasicMouseMessage (&mgr, uMsg, wParam, lParam);
        if (uMsg != WM_RBUTTONUP) // for WM_CONTEXTMENU
            return true;
    }

#if defined(TM_WIN) && !defined(TM_QT)

    DbgAssert(mgr.m_pTheme);
    if (mgr.m_pTheme->DefProc(&mgr, hWnd, uMsg, wParam, lParam, lResult))
        return true;

    // WM_WINDOWPOSCHANGING will send WM_GETMINMAXINFO.
    if (uMsg == CM_MSGROUTE || uMsg == WM_WINDOWPOSCHANGING)
        return true;
    
//     if (uMsg == WM_SETCURSOR)
//         DbgTrace("WM_SETCURSOR(%x, %x)", wParam, lParam);
//     if (uMsg == WM_SETCURSOR && LOWORD(lParam) == HTCLIENT)
//         DbgAssert(0);

    if (uMsg != WM_ACTIVATE) // WM_ACTIVATE will set default focus.
    {
        LOG_FUNC_FOR_SPY(&mgr, L"mgr.m_procOld()");
        lResult = mgr.m_procOld(hWnd, uMsg, wParam, lParam);
    }
//     lResult = DefSubclassProc(hWnd, uMsg, wParam, lParam);

    if (uMsg == WM_UPDATEUISTATE)
    {
        DWORD dwUiState = ::DefWindowProc(hWnd, WM_QUERYUISTATE, 0, 0);
        mgr.Setting_MS_PRI_HIDEFOCUS(dwUiState & UISF_HIDEFOCUS);
        HTUI hFocus = mgr.GetView()->GetFocus();
        if (hFocus)
            hFocus->Invalidate();
    }
#endif

    return true;
}


TM_END_NAMESPACE



#endif //PLATFORMPROC_H
