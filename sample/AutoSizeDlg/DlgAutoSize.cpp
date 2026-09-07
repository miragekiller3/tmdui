/**
 *    @file
 *    @brief
 *    CDlgAutoSize
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2018-9-27
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2018 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "DlgAutoSize.h"

using namespace TM;


CDlgAutoSize::CDlgAutoSize()
{
}

CDlgAutoSize::~CDlgAutoSize()
{
}

TMResult CDlgAutoSize::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
//     _tuiApp.AddMessageFilter(this);
    return 0;
}

TMResult CDlgAutoSize::OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
//     _tuiApp.RemoveMessageFilter(this);
    return 0;
}

TMResult CDlgAutoSize::OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgAutoSize::OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgAutoSize::OnText1 (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiRich rich(Item(IDC_RICH_1002));
    rich.SetWindowText(L"aaaa<br>bbbb");
    return 0;
}

TMResult CDlgAutoSize::OnLayout (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    int nTag = CTuiWidget((HTUI)hWndCtl).GetTag();
    int nLayout[4] = {
        LAYOUT_TOP | LAYOUT_CENTER,
        LAYOUT_LEFT | LAYOUT_VCENTER,
        LAYOUT_RIGHT | LAYOUT_VCENTER,
        LAYOUT_BOTTOM | LAYOUT_CENTER };
    this->SetLayOut(nLayout[nTag] | LAYOUT_AUTOHEIGHT);
    return 0;
}


TMResult CDlgAutoSize::OnText2 (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiRich rich(Item(IDC_RICH_1002));
    rich.SetWindowText(L"cccc<br>dddd<br>bbbb<br>aaaa");
    return 0;
}


//CTuiMsgFilter
// BOOL CDlgAutoSize::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }
