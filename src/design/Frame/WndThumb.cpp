/**
 *  @file
 *  @brief
 *  CWndThumb
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
#include "WndThumb.h"
#include "MainWnd.h"

using namespace TM;


CWndThumb::CWndThumb()
{
}

CWndThumb::~CWndThumb()
{
}

// TMResult CWndThumb::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
// //     _tuiApp.AddMessageFilter(this);
//     return 0;
// }
// 
// TMResult CWndThumb::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     bHandled = false;
// //     _tuiApp.RemoveMessageFilter(this);
//     return 0;
// }
// 

TMResult CWndThumb::OnPrintClient(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    DefWindowProc(uMsg, wParam, lParam); // default render bk;
    
    IObjEdit* pCur = GetPanel().ItemGetSelectedData(false);
    if (!pCur)
        return 0;

    CTuiRect rc;
    GetClientRect(&rc);
    rc.SetWidth(rc.Width() / 2);
    rc.InflateRect(-2, -2);
    pCur->RendSample(_gxApp, 0, rc, pCur->eRenderThumbAll);
    rc.OffsetRectX(rc.Width() + 4);
    pCur->RendSample(_gxApp, 0, rc, pCur->eRenderThumbNoChild);
    return 0;
}


// TMResult CWndThumb::OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
// {
// //    EndDialog(wID);
//     DestroyWindow();
//     return 0;
// }
// 
// TMResult CWndThumb::OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
// {
// //    EndDialog(wID);
//     DestroyWindow();
//     return 0;
// }

//CTuiMsgFilter
// BOOL CWndThumb::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }

//  CAniProc
void CWndThumb::OnTimer(TMHTimer, ...)
{
    Invalidate();
}

CWndPanel& CWndThumb::GetPanel()
{
    return TM_OTHIS(CMainWnd, m_wndThumb)->GetPanel();
}