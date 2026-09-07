#include "stdafx.h"
#include "main.h"


using namespace TM;



// static CMain::Register<0> s_callback_CMain;


TMResult CMain::OnUpdate1(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CString str1, str2;
    Item(IDC_EDIT_20024).GetWindowText(str1);
    Item(IDC_EDIT_20025).GetWindowText(str2);
    _tuiApp.SetHintColorBk(str1.ToULong(0, 16));
    _tuiApp.SetHintColorText(str2.ToULong(0, 16));
    return 0;
}


TMResult CMain::OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    EndDialog(wID);
    return 0;
}

// TMResult CMain::OnHintInit(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
// {
//     CTuiButton but(Item(IDC_CHECKBOX_21001));
//     if (but.GetCheck())
//     {
//         HWND hWnd = (HWND)pMsg->wParam;
//         TOOLINFO* pt = (TOOLINFO*)pMsg->lParam;
//         ::SendMessage(hWnd, TTM_TRACKACTIVATE, true, pMsg->lParam);
// 
//         //::ShowWindow(hWnd, SW_SHOWNOACTIVATE);
//     }
// 
//     bHandled = false;
//     return 0;
// }

TMResult CMain::OnSetting(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiButton wnd((HTUI)hWndCtl);
    int b = wnd.GetCheck();
    if (wID == IDC_CHECKBOX_1000)
        _tuiApp.SetShowHintOnEllipsis(b);
    else if (wID == IDC_CHECKBOX_1001)
        _tuiApp.SetShowHintOnTabKey(b);
    
    return 0;
}

TMResult CMain::OnKeyDown(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    if (pMsg->wParam == 'T')
    {
        CTuiWidget wnd(pMsg->hWnd);
        wnd.ActiveHint(true);
    }
    else
        bHandled = false;
    return 0;
}

TMResult CMain::OnHintAddTool(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    DbgTrace("CDlgTooltip1::OnAddTool");
    TOOLINFO* p = (TOOLINFO*)pMsg->lParam;
    CTuiWidget((TM::HTUI)pMsg->wParam).Item(IDC_PUSHBUTTON_20019).SetWindowText(p->lpszText);
    return 0;
}

TMResult CMain::OnHintShow(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    WINDOWPOS* wp = (WINDOWPOS*)pMsg->lParam;
    ResetPos(wp);

//     TM::CRect rc;
//     ::GetWindowRect(wp->hwnd, &rc);
//     POINT pt;
//     GetCursorPos(&pt);
//     rc.MoveToX(pt.x - rc.Width() / 2);
//     rc.MoveToY(pt.y + 20);
//     wp->x = rc.Left();
//     wp->y = rc.Top();
//     wp->flags &= ~SWP_NOMOVE;
    return 0;
}

TMResult CMain::OnHideC(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    KillTimer();
    SetTimer(3000);
    Item(IDC_WINDOW_21002).ShowWindow(SW_SHOW);
    return 0;
}

TMResult CMain::OnResetHInt(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiWidget wnd((HTUI)hWndCtl);
    LPCTSTR p[2] = { L"test1", L"test 2 xxxxxxxxxxxxxxxx" };
    static int n = 0;
    n = !n;
    wnd.SetHint(p[n]);
    return 0;
}

void CMain::OnTimer(...)
{
    Item(IDC_WINDOW_21002).ShowWindow(SW_HIDE);
    KillTimer();
}

void CMain::ResetPos(WINDOWPOS* wp)
{
    TM::CRect rc;
    ::GetWindowRect(wp->hwnd, &rc);
    POINT pt;
    GetCursorPos(&pt);
    rc.MoveToX(pt.x - rc.Width() / 2);
    rc.MoveToY(pt.y + 20);
    wp->x = rc.Left();
    wp->y = rc.Top();
    wp->flags &= ~SWP_NOMOVE;

    DbgTrace("CDlgTooltip1::MoveTTWindow(%d, %d, %d, %d)", rc);
}


//static s_zoom = 100;

// TMResult CMain::OnZoomIn(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
// {
//     s_zoom -= 10;
// //     _tuiApp.SetZoom(s_zoom);
//     return 0;
// }
// 
// TMResult CMain::OnZoomOut(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
// {
//     s_zoom += 10;
// //     _tuiApp.SetZoom(s_zoom);
//     return 0;
// }

