/**
 *  @file
 *  @brief
 *  CDlgGxTest
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021-8-17
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2021 miragekiller                             
 *  All rights reserved  
 */

#include "stdafx.h"
#include "DlgGxTest.h"

using namespace TM;

// static CDlgGxTest* s_pDlg;

CDlgGxTest::CDlgGxTest()
{
//     s_pDlg = this;
}

CDlgGxTest::~CDlgGxTest()
{
}

// TMResult CDlgGxTest::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
// //     m_edit.Create(GetHWND(), 0, 0, WS_POPUP | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_HSCROLL | WS_VSCROLL, WS_EX_CLIENTEDGE);
// //     CTuiDummy(Item(IDC_DUMMY_1043)).SetWindow(m_edit);
// //     OnRenderChanged(0,0,0,bHandled);
//     return 0;
// }
// 
// TMResult CDlgGxTest::OnRenderChanged(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     bHandled = false;
// 
// //     _tuiApp.AddMessageFilter(this);
//     return 0;
// }
// 
// TMResult CDlgGxTest::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     bHandled = false;
// //     _tuiApp.RemoveMessageFilter(this);
//     return 0;
// }
// 
// TMResult CDlgGxTest::OnMouseMove(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     GXPTexture tex = m_tuiWindow.GetBkTexture();
//     if (tex)
//     {
//         CPoint pt(lParam);
//         CAutoTextureLockBits32 dib32(tex, 0, GX_LOCK_RECT_READ);
//         TMColor clr = dib32.GetPixel(pt.x, pt.y).ToColor();
//         CString str;
//         str.Format(L"point:%d,%d; color:%d,%d,%d,%d(0x%x)",
//             pt, ColorR(clr), ColorG(clr), ColorB(clr), ColorA(clr), clr);
//         Item(IDC_EDIT_1012).SetWindowText(str);
//         DbgTrace(str);
//     }
//     bHandled = false;
//     return 0;
// }


TMResult CDlgGxTest::OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgGxTest::OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgGxTest::OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    m_tuiWindow.DefRouteProc(pMsg);

    CTuiRect rc;
    CTuiWidget(pMsg->hWnd).GetClientRect(&rc);
    rc.InflateRect(-20, -20, -20, -20);
    
    switch (pMsg->uObjectID)
    {
    case IDC_GROUPBOX_1002:
        _gxApp->FillRect(rc, TMRGBA(128,64,0,128)); // the color = rgb(255,128,0)
        break;

    case IDC_WINDOW_TEMP2:
        _gxApp->FillRect(rc, TMRGBA(0,0,10,128));
        break;
//     case IDC_WINDOW_TEMP1:
//         rc.SetWidth(100);
//         _gxApp->FillRect(rc, TMRGBA(0,255,0,255));
//         rc.KeepWOffset(120);
//         _gxApp->FillRect(rc, TMRGBA(255,255,0,255));
//         break;

    case IDC_GROUPBOX_1003:
        _gxApp->FrameRect(rc, 40, TMRGBA(128,64,0,128));
        break;

    case IDC_GROUPBOX_1004:
        _gxApp->InvertRect(rc);
        break;

    case IDC_GROUPBOX_1005:
        _gxApp->DrawFocusRect(rc);
        break;

    case IDC_GROUPBOX_1006:
        {
            TMColor clr[4] = { TMRGBA(255,0,0,255), TMRGBA(0,128,0,128), TMRGBA(0,0,255,255), TMRGBA(0,0,0,0) };
            _gxApp->DrawRectEdge(rc, 60, 40, clr);
        }
        break;

    case IDC_GROUPBOX_1008:
        _gxApp->GrandientFill(rc, TMRGBA(255,0,0,255), TMRGBA(0,128,0,128), e_gfCenter);
        break;
    }
    return 0;
}

//CTuiMsgFilter
// BOOL CDlgGxTest::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }


// void _GX_TRACE(LPCWSTR p, ...)
// {
//     CStringW str;
//     va_list va;
//     va_start(va, p);
//     str.FormatV (p, va);
//     va_end(va);
// 
//     s_pDlg->Append(str);
// }
// 
// void _GX_TRACE(LPCSTR p, ...)
// {
//     CStringA str;
//     va_list va;
//     va_start(va, p);
//     str.FormatV(p, va);
//     va_end(va);
//     
//     s_pDlg->Append(TM_A2T(str));
// }
