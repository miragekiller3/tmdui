/**
 *    @file
 *    @brief
 *    CDlgTestcase1
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2017-12-8
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2017 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "DlgTestcase1.h"

using namespace TM;


static CTuiRect s_rcTest(300, 300, 400, 400);


CDlgTestcase1::CDlgTestcase1()
{
}

CDlgTestcase1::~CDlgTestcase1()
{
}

TMResult CDlgTestcase1::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
//     _tuiApp.AddMessageFilter(this);
//     OnSetSize(0, 0, Item())
    Item(IDC_FLOAT_2010).MoveWindow(0, 0, 0, 0);

    RegisterHotKey(GetHWND(), 0Xa001, 0, VK_F2);

    return 0;
}

TMResult CDlgTestcase1::OnHotKey (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    CTuiWidget wnd(Item(IDC_WINDOW_1004));//IDC_PUSHBUTTON_2009));
    wnd.ShowWindow(!wnd.IsWindowVisible());
    return 0;
}


TMResult CDlgTestcase1::OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
//     _tuiApp.RemoveMessageFilter(this);
    return 0;
}

TMResult CDlgTestcase1::OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgTestcase1::OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgTestcase1::OnEnable (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    Item(wID).EnableWindow(!CTuiButton((HTUI)hWndCtl).GetCheck());
    return 0;
}

TMResult CDlgTestcase1::OnShowHide (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//     static int x = 0;
//     if (x = !x)
//         Item(IDC_WINDOW_1004).MoveWindow(0,0,200,200);
//     else
//         Item(IDC_WINDOW_1004).MoveWindow(0,0,0,0);

    int nCheck = CTuiButton((HTUI)hWndCtl).GetCheck();
    Item(wID).ShowWindow(!nCheck);
    if (wID == IDC_WINDOW_1011 && nCheck)
    {
        Item(IDC_PUSHBUTTON_1013).SetWindowPos(0, 0, 0, 30, 40, SWP_NOMOVE | SWP_NOZORDER);
    }
    return 0;
}

TMResult CDlgTestcase1::OnBring (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TMBool bTop = CTuiButton((HTUI)hWndCtl).GetCheck();
    if (bTop)
        Item(IDC_WINDOW_1010).BringWindowToTop();
    else
        Item(IDC_WINDOW_1010).SendWindowToBack();
    return 0;
}

TMResult CDlgTestcase1::OnSetSize (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TMBool b = CTuiButton((HTUI)hWndCtl).GetCheck();
    if (b)
    {
        Item(IDC_FLOAT_2010).MoveWindow(&s_rcTest);
    }
    else
    {
        CTuiWidget wnd(Item(IDC_FLOAT_2010));
        wnd.GetWindowRect(&s_rcTest);
        wnd.MoveWindow(0, 0, 0, 0);
    }
    bHandled = false;
    return 0;
}

TMResult CDlgTestcase1::OnChangeTab (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    static int n = 0;
    n = !n;
    Item(n ? IDC_EDIT_1020 : IDC_EDIT_1022).SetFocus();
    return 0;
}

TMResult CDlgTestcase1::OnHideBAfter5S (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    KillTimer();
    SetTimer(5000);
    return 0;
}

void CDlgTestcase1::OnTimer(...)
{
    KillTimer();
    Item(IDC_WINDOW_1004).ShowWindow(false);
};


//CTuiMsgFilter
// BOOL CDlgTestcase1::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }
