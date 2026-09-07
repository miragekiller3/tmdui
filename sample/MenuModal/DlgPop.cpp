/**
 *    @file
 *    @brief
 *    CDlgPop
 *
 *
 *    @author 
 *    @date    2014-4-14
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2014 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "DlgPop.h"



CDlgPop::CDlgPop()
{
}

CDlgPop::~CDlgPop()
{
}

TMResult CDlgPop::OnEndMenuPop (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    DestroyWindow();
    return 0;
}

TMResult CDlgPop::OnPop (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TM::CTuiWidget wnd;
    wnd.Create(GetWindow(GW_OWNER), IDW_WINDOW_POP);
    TM::CTuiRect rc;
    GetWindowRect(&rc);
    rc.OffsetRect(40, 40);
    wnd.SetWindowPos(HTUI_TOP, &rc, 0);
    wnd.DoMenuPop((TM::HTUI)hWndCtl);
    
    return 0;
}

TMResult CDlgPop::OnPopDlg (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TM::CTuiWidget wcl(_tuiApp.CreateDlg(_Module.GetResourceInstance(), IDD_MANAGE_POP_DLG, GetHWND()));
    CWindow wnd(wcl.GetHWND());
    TM::CRect rc;
    wnd.GetWindowRect(&rc);
    TM::CPoint pt;
    GetCursorPos(&pt);
    rc.MoveToXY(pt.x + 5, pt.y + 5);

    TM::CRect rcExclude(pt.x, pt.y, pt.x, pt.y);
    TM::CMonitor::HierarchyFromRect(rc, rcExclude, rcExclude);

    wnd.SetWindowPos(HWND_TOP, &rc, SWP_NOACTIVATE | SWP_NOSIZE);
    wcl.DoMenuPop((TM::HTUI)hWndCtl);
    return 0;
}

TMResult CDlgPop::OnClose (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    EndMenuPop();
    return 0;
}

TMResult CDlgPop::OnCloseChild (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    EndOwnedMenuPop();
    return 0;
}


TMResult CDlgPop::OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    return 0;
}

