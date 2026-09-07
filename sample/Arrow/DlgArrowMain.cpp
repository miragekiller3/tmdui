/**
 *    @file
 *    @brief
 *    CDlgArrowMain
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
#include "DlgArrowMain.h"

using namespace TM;


CDlgArrowMain::CDlgArrowMain()
{
}

CDlgArrowMain::~CDlgArrowMain()
{
}

TMResult CDlgArrowMain::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
//     CTuiWidget wnd = _tuiApp.CreateDlg(_Module.GetResourceInstance(), IDC_MANAGE_TIPS, GetHWND());
//     wndArrow = wnd.Item(IDC_SIGHT_1004);
//     wndArrow.SetWindow(Item(IDC_PUSHBUTTON_1006));
// //    CWindow(wnd.GetHWND()).ShowWindow(SW_SHOWNOACTIVATE);
// 
//     CTuiWidget wnd2 = _tuiApp.CreateDlg(_Module.GetResourceInstance(), IDC_MANAGE_UP, GetHWND());
//     CTuiArrow wndArrow2 = wnd2.Item(IDC_ARROW_1008);
//     wndArrow2.SetWindow(Item(IDC_PUSHBUTTON_1006));
//     wndTxt = wnd2.Item(IDC_STATIC_2013);
// //    CWindow(wnd2.GetHWND()).ShowWindow(SW_SHOWNOACTIVATE);
// 
//     wndArrow3 = this->Item(IDC_ARROW_2007);
//     wndArrow3.SetWindow(Item(IDC_PUSHBUTTON_1006));

//     _tuiApp.AddMessageFilter(this);
    return 0;
}

TMResult CDlgArrowMain::OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
//     _tuiApp.RemoveMessageFilter(this);
    return 0;
}

TMResult CDlgArrowMain::OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgArrowMain::OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgArrowMain::OnCreatePop (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    int tag = CTuiWidget((HTUI)hWndCtl).GetTag();
    if (wndPop[tag])
    {
        wndPop[tag].DestroyWindow();
        wndArrow[tag] = 0;
    }
    else
    {
        int ida;
        if (tag == 0)
        {
            wndPop[tag].Create(*this, IDC_FLOAT_2006);
            wndPop[tag].ShowWindow(true);
            ida = IDC_ARROW_2007;
        }
        else if (tag == 1)
        {
            wndPop[tag] = _tuiApp.CreateDlg(_Module.GetResourceInstance(), IDC_MANAGE_TIPS, GetHWND());
            ida = IDC_SIGHT_1004;
        }
        else
        {
            wndPop[tag] = _tuiApp.CreateDlg(_Module.GetResourceInstance(), IDC_MANAGE_UP, GetHWND());
            ida = IDC_ARROW_1008;
        }
        wndArrow[tag] = wndPop[tag].Item(ida);
        wndArrow[tag].SetWindow(Item(IDC_PUSHBUTTON_1006));
    }
    return 0;
}


TMResult CDlgArrowMain::OnText (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    if (!wndPop[2])
        return 0;
    CTuiEdit wndTxt(wndPop[2].Item(IDC_STATIC_2013));
    wndTxt.AppendText(L"aaabbbbbb ddd\r\n");
    if (wndTxt.GetWindowTextLength() > 300)
        wndTxt.SetWindowText(L"");
    return 0;
}

TMResult CDlgArrowMain::OnBind (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    switch (wID)
    {
    case IDC_BIND1:     if (wndArrow[0]) wndArrow[0].SetWindow(Item(IDC_PUSHBUTTON_1006)); break;
    case IDC_UNBIND1:   if (wndArrow[0]) wndArrow[0].SetWindow(0); break;
    case IDC_BIND2:     if (wndArrow[1]) wndArrow[1].SetWindow(Item(IDC_PUSHBUTTON_1006)); break;
    case IDC_UNBIND2:   if (wndArrow[1]) wndArrow[1].SetWindow(0); break;
    case IDC_BIND3:     if (wndArrow[2]) wndArrow[2].SetWindow(Item(IDC_PUSHBUTTON_1006)); break;
    case IDC_UNBIND3:   if (wndArrow[2]) wndArrow[2].SetWindow(0); break;
    }
    return 0;
}

TMResult CDlgArrowMain::OnMinCreate (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    ::ShowWindow(GetHWND(), SW_SHOWMINIMIZED);

    CTuiWidget wnd2 = _tuiApp.CreateDlg(_Module.GetResourceInstance(), IDC_MANAGE_UP, GetHWND());
    CTuiArrow wndArrow2(wnd2.Item(IDC_ARROW_1008));
    wndArrow2.SetWindow(Item(IDC_PUSHBUTTON_1006));
    static int x = 20;
    x += 20;
    wndArrow2.SetPading(x+100,x + 160,0,0);
//    CWindow(wnd2.GetHWND()).ShowWindow(SW_SHOWNA);
//    CWindow(wnd2.GetHWND()).ShowWindow(SW_SHOWMINIMIZED);
//    CWindow(wnd2.GetHWND()).ModifyStyle(WS_VISIBLE, 0);
    //CWindow(wnd2.GetHWND()).SetRedraw(false);
//    ::ShowWindow(GetHWND(), SW_MINIMIZE);
    //
    
    CWindow(wnd2.GetHWND()).ShowWindow(SW_MINIMIZE);
    CWindow(wnd2.GetHWND()).SendMessage(WM_SHOWWINDOW, false, SW_PARENTCLOSING);

    return 0;
}



//CTuiMsgFilter
// BOOL CDlgArrowMain::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }
