/**
 *  @file
 *  @brief
 *  CDlgGxTestBlt
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
#include "DlgGxTestBlt.h"

using namespace TM;

// static CDlgGxTestBlt* s_pDlg;

CDlgGxTestBlt::CDlgGxTestBlt()
    : m_alpha(196)
    , m_format(IDC_RADIOBUTTON_FMT_32)
    , m_reset(IDC_RADIOBUTTON_RST_NONE)
{
//     s_pDlg = this;
}

CDlgGxTestBlt::~CDlgGxTestBlt()
{
}

TMResult CDlgGxTestBlt::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
//     m_edit.Create(GetHWND(), 0, 0, WS_POPUP | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_HSCROLL | WS_VSCROLL, WS_EX_CLIENTEDGE);
//     CTuiDummy(Item(IDC_DUMMY_1043)).SetWindow(m_edit);
//     OnFormat(0, IDC_RADIOBUTTON_FMT_32, 0, bHandled);
//     OnRenderChanged(0,0,0,bHandled);
    Reset();
    return 0;
}

TMResult CDlgGxTestBlt::OnRenderChanged(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
    Reset();
//    OnFormat(0, m_id, 0, bHandled);

//     _tuiApp.AddMessageFilter(this);
    return 0;
}

TMResult CDlgGxTestBlt::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
    m_tex.Release();
    m_tex_a.Release();
//     _tuiApp.RemoveMessageFilter(this);
    return 0;
}

// TMResult CDlgGxTestBlt::OnMouseMove(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
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

TMResult CDlgGxTestBlt::OnFormat(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    m_format = wID;
    Reset();
    return 0;
}

TMResult CDlgGxTestBlt::OnReset(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    m_reset = wID;
    Reset();
    return 0;
}

TMResult CDlgGxTestBlt::OnEnUpdate(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    m_alpha = m_tuiWindow.GetDlgItemInt(IDC_EDIT_ALPHA);
    m_tuiWindow.Invalidate();
    return 0;
}

TMResult CDlgGxTestBlt::OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgGxTestBlt::OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgGxTestBlt::OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    m_tuiWindow.DefRouteProc(pMsg);

    CTuiRect rc;
    CTuiWidget(pMsg->hWnd).GetClientRect(&rc);
    rc.InflateRect(-20, -20, -20, -20);
    
    switch (pMsg->uObjectID)
    {
    case IDC_GROUPBOX_1082:
        if (m_tex)
            m_tex->Blt(rc, CRect(0,0,0,0), GX_BLT_DFTSRCWH | GX_BLT_SRCCOPY, m_alpha);
        break;
    case IDC_GROUPBOX_1084:
        if (m_tex)
            m_tex->Blt(rc, CRect(0,0,0,0), GX_BLT_DFTSRCWH, m_alpha);
        break;

    case IDC_GROUPBOX_1086:
        if (m_tex_a)
            m_tex_a->Blt(rc, CRect(0,0,0,0), GX_BLT_DFTSRCWH | GX_BLT_SRCCOPY, m_alpha);
        break;
    case IDC_GROUPBOX_1088:
        if (m_tex_a)
            m_tex_a->Blt(rc, CRect(0,0,0,0), GX_BLT_DFTSRCWH, m_alpha);
        break;

//     case IDC_GROUPBOX_1090:
//         if (m_tex_ka)
//             m_tex_ka.Blt(rc, CRect(0,0,0,0), GX_BLT_DFTSRCWH | GX_BLT_SRCCOPY, m_alpha);
//         break;
//     case IDC_GROUPBOX_1091:
//         if (m_tex_ka)
//             m_tex_ka.Blt(rc, CRect(0,0,0,0), GX_BLT_DFTSRCWH, m_alpha);
//         break;

    }
    return 0;
}

//CTuiMsgFilter
// BOOL CDlgGxTestBlt::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }

void _CreateTex(TM::CTexture& tex, EGXFormat fmt)
{
    CImage img;
    img.CreateFromResource(0, TM_T("24.JPG"), TM_T("TUIPIC"));
    img.ConvertBitCount(fmt);
    tex.CreateFromImage(img);
}

void CDlgGxTestBlt::Reset()
{
    m_tuiWindow.Invalidate();
    m_tex.Release();
    m_tex_a.Release();

    char c[1024] = { 0 };
    GXDibData dd;
    dd.bits = c;
    dd.wb = 7 * 4;
    dd.flag = 0;
    int wbKA = 7 * 4;

    switch (m_format)
    {
    case IDC_RADIOBUTTON_FMT_32:
        _CreateTex(m_tex, eGXFormat32X);
        _CreateTex(m_tex_a, eGXFormat32X);
        break;
        
    case IDC_RADIOBUTTON_FMT_32A:
        m_tex.CreateFromResource(0, TM_T("32.GIF"), TM_T("TUIPIC"));
        m_tex_a.CreateFromResource(0, TM_T("32.GIF"), TM_T("TUIPIC"));
        wbKA = 7 * 3;
        break;
        
    case IDC_RADIOBUTTON_FMT_24:
        _CreateTex(m_tex, eGXFormat24);
        _CreateTex(m_tex_a, eGXFormat24);
        dd.wb = wbKA = 7 * 3;
        break;
        
    case IDC_RADIOBUTTON_FMT_NV12:
        {
            m_tex.CreateIndirect(7, 7, eGXFormatNV12);
            m_tex_a.CreateIndirect(7, 7, eGXFormatNV12);
            dd.wb = 7;
            wbKA = 7;
            if (!m_tex || !m_tex_a)
                return;
            m_tex->ResetAsDib(dd);
            m_tex_a->ResetAsDib(dd);
        }
        break;
    }

    if (!m_tex_a)
        return;

    switch (m_reset)
    {
    case IDC_RADIOBUTTON_RST_NONE: return;
    case IDC_RADIOBUTTON_RST_DIB: m_tex_a->ResetAsDib(dd); return;
    case IDC_RADIOBUTTON_RST_DIB_KA: dd.flag = dd.eKeepAlpha; dd.wb = wbKA; m_tex_a->ResetAsDib(dd); return;
    }
    
#   pragma warning(disable:4305)
#   pragma warning(disable:4309)
    char a[256] = {
        196, 196, 196, 196, 196, 196,
        196, 196, 196, 196, 196, 196,
        196, 196,   0,   0, 196, 196,
        196, 196,   0,   0, 196, 196,
        196, 196, 196, 196, 196, 196,
        196, 196, 196, 196, 196, 196,
    };
    dd.alpha.Set(a, 6, 6);
    dd.alpha.m_stride = 6;

    if (m_reset == IDC_RADIOBUTTON_RST_DIB_A)
    {
        dd.wb = wbKA;
        m_tex_a->ResetAsDib(dd);
    }
    else if (m_reset == IDC_RADIOBUTTON_RST_A)
    {
        dd.bits = 0;
        dd.wb = 0;
        m_tex_a->ResetAsDib(dd);
    }
}
