/**
 *  @file
 *  @brief
 *  CDlgGxTestBltDir
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2025-8-17
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2025 miragekiller                             
 *  All rights reserved  
 */

#include "stdafx.h"
#include "DlgGxTestBltDir.h"

using namespace TM;

// static CDlgGxTestBltDir* s_pDlg;

CDlgGxTestBltDir::CDlgGxTestBltDir()
//     : m_alpha(196)
//     , m_format(IDC_RADIOBUTTON_FMT_32)
//     , m_reset(IDC_RADIOBUTTON_RST_NONE)
{
//     s_pDlg = this;
}

CDlgGxTestBltDir::~CDlgGxTestBltDir()
{
}

TMResult CDlgGxTestBltDir::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
//     m_edit.Create(GetHWND(), 0, 0, WS_POPUP | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_HSCROLL | WS_VSCROLL, WS_EX_CLIENTEDGE);
//     CTuiDummy(Item(IDC_DUMMY_1043)).SetWindow(m_edit);
//     OnFormat(0, IDC_RADIOBUTTON_FMT_32, 0, bHandled);
//     OnRenderChanged(0,0,0,bHandled);
    Reset();
    return 0;
}

TMResult CDlgGxTestBltDir::OnRenderChanged(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
    Reset();
//    OnFormat(0, m_id, 0, bHandled);

//     _tuiApp.AddMessageFilter(this);
    return 0;
}

TMResult CDlgGxTestBltDir::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
    m_tex.Release();
//     m_tex_a.Release();
//     _tuiApp.RemoveMessageFilter(this);
    return 0;
}

TMResult CDlgGxTestBltDir::OnEnUpdate(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    m_tuiWindow.Invalidate();
    return 0;
}

TMResult CDlgGxTestBltDir::OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgGxTestBltDir::OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgGxTestBltDir::OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    m_tuiWindow.DefRouteProc(pMsg);
    TMUInt8 alpha = m_tuiWindow.GetDlgItemInt(IDC_EDIT_ALPHA);
    TMUInt32 nMirror = CTuiButton(Item(IDC_DIR_MIRROR)).GetCheck() ? GX_BLT_R2L : 0;
    TMUInt32 nB2T    = CTuiButton(Item(IDC_DIR_B2T)).GetCheck() ? GX_BLT_B2T : 0;
    TMUInt32 nRotate = CTuiButton(Item(IDC_DIR_ROTATE0)).GetCheck() ? GX_BLT_X2Y : 0;

    CTuiRect rc;
    CTuiWidget(pMsg->hWnd).GetClientRect(&rc);
    rc.InflateRect(-20, -20, -20, -20);
    if (nRotate)
        rc.Rotate();
    
    switch (pMsg->uObjectID)
    {
    case IDC_GROUPBOX_1216:
        if (m_tex)
            m_tex->Blt(rc, CRect(0,0,0,0), GX_BLT_DFTSRCWH | GX_BLT_SRCCOPY | nMirror | nRotate | nB2T, alpha);
        break;
    case IDC_GROUPBOX_1217:
        if (m_tex)
        {
            CBltSharp bs;
            bs.left = 5;
            bs.top = 3;
            bs.right = 11;
            bs.bottom = 11;
            CRectL rcDst[10];
            CRectD rcSrc[9];
            int n = bs.MakeBltsParams(rcDst, rcSrc, rc, CRect(0, 0, m_tex->GetWidth(), m_tex->GetHeight()));
            rcDst[n] = rc;
            m_tex->Blts(rcDst, rcSrc, n, GX_BLT_DFTSRCWH | nMirror | nRotate | nB2T, alpha);
        }
        break;
    }
    return 0;
}

//CTuiMsgFilter
// BOOL CDlgGxTestBltDir::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }

void _CreateTex(TM::CTexture& tex)
{
    CImage img;
    img.CreateFromResource(0, TM_T("1.BMP"), TM_T("TUIPIC"));
    tex.CreateFromImage(img);
}

void CDlgGxTestBltDir::Reset()
{
    m_tuiWindow.Invalidate();
    m_tex.Release();
    _CreateTex(m_tex);
}
