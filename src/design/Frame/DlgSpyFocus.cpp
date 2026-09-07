/**
 *    @file
 *    @brief
 *    CDlgSpyFocus
 *
 *  @author miragekiller <3916345933@qq.com>
 *    @date    2017-11-12
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2017 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "DlgSpyFocus.h"
#include "MainWnd.h"

using namespace TM;

void CDlgSpyFocus::OnTrackFocus(HWND hParent, void* pNewObj, const TM::CRect& rc)
{
    static CDlgSpyFocus::CPtr s_ptr;
    if (!pNewObj)
    {
        if (s_ptr.IsWindow())
            s_ptr->DestroyWindow();
        s_ptr.Release();
        return;
    }

    if (!s_ptr.IsWindow())
    {
        TM::CTuiApp* app = AppGetEditor();
        s_ptr.CreateInstance()->Create(*app, hParent);
    }

    ::SetWindowPos(s_ptr->GetHWND(), 0, rc.Left(), rc.Top(), rc.Width(), rc.Height(),
        SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}
