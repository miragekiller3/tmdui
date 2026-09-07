/**
 *    @file
 *    @brief
 *    CDlgControls
 *
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2012-10-2
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2012 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "WndControlsView.h"
#include "MainWnd.h"
#include "tmdrag.h"

using namespace TM;



CDlgControls::CDlgControls()
{
}

CDlgControls::~CDlgControls()
{
}

TMResult CDlgControls::OnMouseMove(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    if (pMsg->wParam == MK_LBUTTON)
    {
        CTuiWidget wnd(pMsg->hWnd);
        CString str;
        wnd.GetWindowText(str);
        if (!str.IsEmpty())
        {
            CDragData dd;
            dd.AddText(str, DRAG_WND_CLASS);
            dd.DoDrag(wnd.m_hWnd, eDragEffCopy);
        }
    }
    return 0;
}

TMResult CDlgControls::OnLButtonDbClick(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    return 0;
}

void CDlgControls::Init(HTUI hWnd, IProj* proj)
{
    for (int m_nCount = 0; ; ++m_nCount)
    {
        TM::CString lpszName = proj->GetClassByIndex(TM::e_otWgt, m_nCount);
        DbgTrace("%s", TM_T2A(lpszName));
        
        if (lpszName.IsEmpty())
            break;
        
        CTuiWidget wnd;
        wnd.Create(hWnd, IDW_TUI_CONTROL_ITEM);
        wnd.SetWindowText(lpszName);
        wnd.SetWindowPos(HTUI_BOTTOM, 0, 0, CTRL_WIDTH, CTRL_HEIGHT, SWP_NOMOVE | SWP_SHOWWINDOW);
    }
}