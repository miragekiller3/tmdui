/**
 *  @file
 *  @brief
 *  CDlgTooltip2
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2022-7-4
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2022 miragekiller                             
 *  All rights reserved  
 */

#include "stdafx.h"
#include "DlgTooltip2.h"
#include "main.h"

using namespace TM;


CDlgTooltip2::CDlgTooltip2()
{
}

CDlgTooltip2::~CDlgTooltip2()
{
}

TMResult CDlgTooltip2::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
//     _tuiApp.AddMessageFilter(this);
    return 0;
}

TMResult CDlgTooltip2::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
//     _tuiApp.RemoveMessageFilter(this);
    return 0;
}

TMResult CDlgTooltip2::OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgTooltip2::OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgTooltip2::OnAddTool(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    DbgTrace("CDlgTooltip1::OnAddTool");
    bHandled = false;
    TOOLINFO* p = (TOOLINFO*)lParam;
    Item(IDC_PUSHBUTTON_21006).SetWindowText(p->lpszText);
    return 0;
}

TMResult CDlgTooltip2::OnWindowPosChanging(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    WINDOWPOS* p = (WINDOWPOS*)lParam;
    CMain::ResetPos(p);
//     if (wParam)
//     {
//         TM::CRect rc;
//         GetWindowRect(&rc);
//         MoveTTWindow(rc);
//     }
    bHandled = false;
    return 0;
}

//CTuiMsgFilter
// BOOL CDlgTooltip2::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }
