/**
 *    @file
 *    @brief
 *    CDlgWin2pic
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2020-4-28
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2020 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "DlgWin2pic.h"

using namespace TM;


CDlgWin2pic::CDlgWin2pic()
{
}

CDlgWin2pic::~CDlgWin2pic()
{
}

TMResult CDlgWin2pic::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
//     _tuiApp.AddMessageFilter(this);
    return 0;
}

TMResult CDlgWin2pic::OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
//     _tuiApp.RemoveMessageFilter(this);
    return 0;
}

TMResult CDlgWin2pic::OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgWin2pic::OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgWin2pic::OnSet (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiButton wnd((HTUI)hWndCtl);
    int n = wnd.GetCheck();
    if (n)
    {
        CTuiWidget wndc(wnd.GetWindow(GW_CHILD));
        CComPtr<ITuiPic> ptr(wndc.ConvertToPic());
        if (wID == IDC_PUSHBUTTON_1002)
            wnd.SetForePic(ptr);
        else
            wnd.SetBkPic(ptr);
    }
    else
    {
        if (wID == IDC_PUSHBUTTON_1002)
            wnd.SetForePic(0);
        else
            wnd.SetBkPic(0);
    }
    return 0;
}

TMResult CDlgWin2pic::OnSetFrm (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    this->SetFrame(0);
    this->SetFrame(IDC_FORM_1020);

    CTuiWidget wndc(Item(IDC_ANIMATE_1019));
    CComPtr<ITuiPic> ptr(wndc.ConvertToPic());

    CTuiButton wnd((HTUI)hWndCtl);
    int nTag = wnd.GetTag();
    this->SetFramePic((EHitObj)nTag, ptr);

    return 0;
}


//CTuiMsgFilter
// BOOL CDlgWin2pic::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }
