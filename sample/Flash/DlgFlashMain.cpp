/**
 *    @file
 *    @brief
 *    CDlgFlashMain
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2018-7-8
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2018 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "DlgFlashMain.h"
#import "Flash.ocx"

using namespace TM;
using namespace ShockwaveFlashObjects;


CDlgFlashMain::CDlgFlashMain()
{
}

CDlgFlashMain::~CDlgFlashMain()
{
}

TMResult CDlgFlashMain::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
//     CTuiWidget wnd = _tuiApp.CreateDlg(_Module.GetResourceInstance(), IDC_MANAGE_TIPS, GetHWND());
//     CTuiArrow wndArrow = wnd.Item(IDC_SIGHT_1004);
//     wndArrow.SetWindow(Item(IDC_PUSHBUTTON_1006));
// 
//     wndArrow = this->Item(IDC_ARROW_2007);
//     wndArrow.SetWindow(Item(IDC_PUSHBUTTON_1006));
    CTuiActiveX wnd(Item(IDC_ACTIVEX_2009));
    CComPtr<IShockwaveFlash> ptrFlash;
    wnd.QueryControl(&ptrFlash);
//     ptrFlash->PutWMode(L"transparent");
    ptrFlash->put_Movie((BSTR)L"c:\\1.swf");
//    ptrFlash->PutQuality2(L"high");
//    ptrFlash->PutLoop(VARIANT_TRUE);
//    ptrFlash->PutPlaying(VARIANT_TRUE);
//    ptrFlash->StopPlay();
//    DbgVerifyHResult(ptrFlash->Play());
//    long x = ptrFlash->GetFrameNum();
//    _bstr_t y = ptrFlash->GetMovie();


//     _tuiApp.AddMessageFilter(this);
    return 0;
}



TMResult CDlgFlashMain::OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
//     _tuiApp.RemoveMessageFilter(this);
    return 0;
}

TMResult CDlgFlashMain::OnObjInit(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiActiveX wnd(Item(IDC_ACTIVEX_2009));
    CComPtr<IShockwaveFlash> ptrFlash;
    wnd.QueryControl(&ptrFlash);
    ptrFlash->PutWMode(L"transparent");
//    ptrFlash->put_Movie(L"c:\\2.swf");
    bHandled = false;
    return 0;
}


TMResult CDlgFlashMain::OnPlay (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiActiveX wnd(Item(IDC_ACTIVEX_2009));
    CComPtr<IShockwaveFlash> ptrFlash;
    wnd.QueryControl(&ptrFlash);
    long x = ptrFlash->GetFrameNum();
    DbgVerifyHResult(ptrFlash->Play());
    return 0;
}


TMResult CDlgFlashMain::OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgFlashMain::OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

//CTuiMsgFilter
// BOOL CDlgFlashMain::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }
