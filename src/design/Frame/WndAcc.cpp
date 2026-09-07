/**
 *  @file
 *  @brief
 *  CWndAcc
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2022-7-11
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2022 miragekiller                             
 *  All rights reserved  
 */

#include "stdafx.h"
#include "WndAcc.h"
#include "MainWnd.h"

using namespace TM;

CWndAcc::CWndAcc()
{
}

CWndAcc::~CWndAcc()
{
}

// TMResult CWndAcc::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
// //     _tuiApp.AddMessageFilter(this);
//     return 0;
// }
// 
// TMResult CWndAcc::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     bHandled = false;
// //     _tuiApp.RemoveMessageFilter(this);
//     return 0;
// }
// 
// TMResult CWndAcc::OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
// {
// //    EndDialog(wID);
//     DestroyWindow();
//     return 0;
// }
// 
// TMResult CWndAcc::OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
// {
// //    EndDialog(wID);
//     DestroyWindow();
//     return 0;
// }

//CTuiMsgFilter
// BOOL CWndAcc::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }

//  CAniProc
void CWndAcc::OnTimer(TMHTimer, ...)
{
    IObjEdit* pCur = GetPanel().ItemGetSelectedData(false);
    if (!pCur)
        return;

    TM::CString str(pCur->GetAccInfo());
    SetWindowText(str);
}

CWndPanel& CWndAcc::GetPanel()
{
    return TM_OTHIS(CMainWnd, m_wndAcc)->GetPanel();
}