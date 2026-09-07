/**
 *  @file
 *  @brief
 *  CDlgTesterSetting
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021-12-9
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2021 miragekiller                             
 *  All rights reserved  
 */

#include "stdafx.h"
#include "DlgTesterSetting.h"
#include "MainWnd.h"
#include "Tester.h"

using namespace TM;


CDlgTesterSetting::CDlgTesterSetting()
{
}

CDlgTesterSetting::~CDlgTesterSetting()
{
}

TMResult CDlgTesterSetting::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
//     _tuiApp.AddMessageFilter(this);
    InitUI();
    InitApp();
    InitColor();

    return 0;
}

TMResult CDlgTesterSetting::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
//     _tuiApp.RemoveMessageFilter(this);
    return 0;
}

TMResult CDlgTesterSetting::OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgTesterSetting::OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgTesterSetting::OnUI(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    TesterSetHighContrast(m_panel->GetProj(), CTuiWidget((HTUI)hWndCtl).GetDlgCtrlObjectID());
    return 0;
}

void CDlgTesterSetting::InitUI()
{
    int nCmd = TesterGetHighContrast(m_panel->GetProj());
    if (nCmd)
        CTuiButton(Item(nCmd)).SetCheck(true);
}

TMResult CDlgTesterSetting::OnApp(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    CTuiButton but((HTUI)hWndCtl);
    int id = but.GetDlgCtrlObjectID();
    TMBool bCheck = but.GetCheck();
    switch (id)
    {
    case IDC_CHECKBOX_33069: AppGetTester()->SetAnimationButton(bCheck); break;
    case IDC_CHECKBOX_33070: AppGetTester()->SetAnimationScroll(bCheck); break;
    case IDC_CHECKBOX_33071: AppGetTester()->SetAnimationSysBut(bCheck); break;

    case IDC_CHECKBOX_33073: AppGetTester()->SetShowHintOnEllipsis(bCheck); break;
    case IDC_CHECKBOX_33074: AppGetTester()->SetShowHintOnTabKey(bCheck); break;

    case IDC_CHECKBOX_33085: AppGetTester()->SetUseSystemFrame(bCheck); break;
    case IDC_CHECKBOX_33086: AppGetTester()->SetWebview2SSOUsingOSA(bCheck); break;
    }
    return 0;
}

void CDlgTesterSetting::InitApp()
{
    CTuiButton(Item(IDC_CHECKBOX_33069)).SetCheck(AppGetTester()->IsAnimationButton());
    CTuiButton(Item(IDC_CHECKBOX_33070)).SetCheck(AppGetTester()->IsAnimationScroll());
    CTuiButton(Item(IDC_CHECKBOX_33071)).SetCheck(AppGetTester()->IsAnimationSysBut());
        
    CTuiButton(Item(IDC_CHECKBOX_33073)).SetCheck(AppGetTester()->IsShowHintOnEllipsis());
    CTuiButton(Item(IDC_CHECKBOX_33074)).SetCheck(AppGetTester()->IsShowHintOnTabKey());
        
    CTuiButton(Item(IDC_CHECKBOX_33085)).SetCheck(AppGetTester()->IsUseSystemFrame());
    CTuiButton(Item(IDC_CHECKBOX_33086)).SetCheck(AppGetTester()->IsWebview2SSOUsingOSA());
}

TMResult CDlgTesterSetting::OnColor(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    CTuiEdit edit((HTUI)hWndCtl);
    int id = edit.GetDlgCtrlObjectID();
    CString str;
    edit.GetWindowText(str);
    TMColor clr = str.ToULong(0, 16);

    switch (id)
    {
    case IDC_EDIT_33046: AppGetTester()->SetHintColorBk(clr); break;
    case IDC_EDIT_33077: AppGetTester()->SetHintColorText(clr); break;

    case IDC_EDIT_33047: AppGetTester()->SetColorEditSelBackground(clr); break;
    case IDC_EDIT_33078: AppGetTester()->SetColorEditSelText(clr); break;
    case IDC_EDIT_33080: AppGetTester()->SetColorEditPrompt(clr); break;
    case IDC_EDIT_33082: AppGetTester()->SetDisabledColorText(clr); break;
    }

    return 0;
}

static void _SetC(CTuiWidget wnd, TMColor clr)
{
    CString str;
    str.Format(L"%08X", clr);
    wnd.SetWindowText(str);
}

void CDlgTesterSetting::InitColor()
{
    _SetC(Item(IDC_EDIT_33046), AppGetTester()->GetHintColorBk());
    _SetC(Item(IDC_EDIT_33077), AppGetTester()->GetHintColorText());
        
    _SetC(Item(IDC_EDIT_33047), AppGetTester()->GetColorEditSelBackground());
    _SetC(Item(IDC_EDIT_33078), AppGetTester()->GetColorEditSelText());
    _SetC(Item(IDC_EDIT_33080), AppGetTester()->GetColorEditPrompt());
    _SetC(Item(IDC_EDIT_33082), AppGetTester()->GetDisabledColorText());
}

//CTuiMsgFilter
// BOOL CDlgTesterSetting::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }
