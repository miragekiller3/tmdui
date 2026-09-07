/**
 *    @file
 *    @brief
 *    CDlgMenubar
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2021-3-29
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2021 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "DlgMenubar.h"

using namespace TM;


CDlgMenubar::CDlgMenubar()
{
}

CDlgMenubar::~CDlgMenubar()
{
}

TMResult CDlgMenubar::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
//     _tuiApp.AddMessageFilter(this);
    return 0;
}

TMResult CDlgMenubar::OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
//     _tuiApp.RemoveMessageFilter(this);
    return 0;
}

TMResult CDlgMenubar::OnOk (TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgMenubar::OnCancel (TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgMenubar::OnShowHide (TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    CTuiButton but((HTUI)hWndCtl);
    CTuiMenuBar mb(Item(IDC_MENUBAR_1002));
    TMBool b = but.GetCheck();
    if (b)
    {
        mb.ShowWindow(SW_SHOW);
        mb.SetAltMode(true);
    }
    else
        mb.ShowWindow(SW_HIDE);

    return 0;
}


//CTuiMsgFilter
// BOOL CDlgMenubar::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }
