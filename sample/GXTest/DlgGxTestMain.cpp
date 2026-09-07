/**
 *  @file
 *  @brief
 *  CDlgGxTestMain
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2022-6-28
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2022 miragekiller                             
 *  All rights reserved  
 */

#include "stdafx.h"
#include "DlgGxTestMain.h"
#include "DlgGxTest.h"
#include "DlgGxTestBlt.h"
#include "DlgGxTestBltDir.h"

using namespace TM;

static CDlgGxTestMain* s_pDlg;

CDlgGxTestMain::CDlgGxTestMain()
{
    s_pDlg = this;
}

CDlgGxTestMain::~CDlgGxTestMain()
{
}

TMResult CDlgGxTestMain::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    _tuiApp.AddMessageFilter(this);
    gxSetNotify(this, true);
//     m_edit.Create(0, 0, L"log", WS_VISIBLE | WS_OVERLAPPEDWINDOW | ES_MULTILINE);
    return 0;
}

TMResult CDlgGxTestMain::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
    _tuiApp.RemoveMessageFilter(this);
    return 0;
}

// TMResult CDlgGxTestMain::OnRenderChanged(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     bHandled = false;
//     
//     EGXDevice dev = _gxApp->GetDeviceType();
//     //CTuiRadioButton(Item(IDC_DEV_GDI + dev - 1)).SetCheck(true);
//     CButton(GetDlgItem(IDC_RADIO1 + dev - 1)).SetCheck(true);
//     
//     return 0;
// }

void CDlgGxTestMain::OnGXDeviceChanged(IGXApp* pNew)
{
    EGXDevice dev = (eGXGraDevMask & pNew->GetDeviceType());
    //CTuiRadioButton(Item(IDC_DEV_GDI + dev - 1)).SetCheck(true);
    CButton(GetDlgItem(IDC_RADIO1 + dev - 1)).SetCheck(1);
}


TMResult CDlgGxTestMain::OnDevice(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    EGXDevice dev[] = { eGXGraDevGDI, eGXGraDevD3D9, eGXGraDevD3D11, eGXGraDevGL };
//     gxSetDefaultDevices(&(dev[wID - IDC_DEV_GDI]), 1, true);
    gxSetDefaultGraDevices(true, dev[wID - IDC_RADIO1]);
    return 0;
}

TMResult CDlgGxTestMain::OnTest(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    if (wID == IDC_BUTTON1) //IDC_PUSHBUTTON_1071)
    {
        if (!m_ptrShade.IsWindow())
            m_ptrShade.CreateInstance()->Create(0);
    }
    else if (wID == IDC_BUTTON2)
    {
        if (!m_ptrBlt.IsWindow())
            m_ptrBlt.CreateInstance()->Create(0);
    }
    else
    {
        if (!m_ptrBltDir.IsWindow())
            m_ptrBltDir.CreateInstance()->Create(0);
    }
    OnLayered(0,0,0,bHandled);
    return 0;
}

TMResult CDlgGxTestMain::OnLayered(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    DWORD flagAdd = CButton(GetDlgItem(IDC_CHECK1)).GetCheck() ? WS_ADV_LAYERED : 0;
    DWORD flagRemove = flagAdd ? 0 : WS_ADV_LAYERED;
    if (m_ptrShade.IsWindow())
        m_ptrShade->m_tuiWindow.ModifyStyleEx(flagRemove, flagAdd);
    if (m_ptrBlt.IsWindow())
        m_ptrBlt->m_tuiWindow.ModifyStyleEx(flagRemove, flagAdd);
    return 0;
}


TMResult CDlgGxTestMain::OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgGxTestMain::OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    EndDialog(wID);
//    DestroyWindow();
    return 0;
}

//CTuiMsgFilter
BOOL CDlgGxTestMain::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
{
    if (pMsg->message != WM_MOUSEMOVE)
        return false;

    GXPTexture tex = CTuiWindow(CTuiWidget(pMsg->hWnd).GetWindow(GW_OWNER)).GetBkTexture();
    if (tex)
    {
        CPoint pt(pMsg->lParam);
        CAutoTextureLockBits32 dib32(tex, 0, GX_LOCK_RECT_READ);
        TMColor clr = dib32.GetPixel(pt.x, pt.y).ToColor();
        CString str;
        str.Format(L"point:%d,%d; color:%d,%d,%d,%d(0x%x)",
            pt, ColorR(clr), ColorG(clr), ColorB(clr), ColorA(clr), clr);
//        Item(IDC_EDIT_1012).SetWindowText(str);
        CWindow(GetDlgItem(IDC_EDIT1)).SetWindowText(str);
        DbgTrace(str);
    }
    return false;
}



void _GX_TRACE(LPCWSTR p, ...)
{
    CStringW str;
    va_list va;
    va_start(va, p);
    str.FormatV (p, va);
    va_end(va);

    s_pDlg->Append(str);
}

void _GX_TRACE(LPCSTR p, ...)
{
    CStringA str;
    va_list va;
    va_start(va, p);
    str.FormatV(p, va);
    va_end(va);
    
    s_pDlg->Append(TM_A2T(str));
}
