/**
 *    @file
 *    @brief
 *    CDlgRotate
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2018-12-4
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2018 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "DlgRotate.h"
#include "tmgdix.h"
#include "tmfile.h"


using namespace TM;


CDlgRotate::CDlgRotate()
    : m_ptrPic (IDC_BMP_1002)
    , m_nAngle (0)
{
//     CComPtr<IStream> stream;
//     StreamCreateFromFile(L"c:\\1.svg", TM::CFile::e_faRead, &stream);
//     m_ptrPic = (ITuiPic*)_tuiApp.CreateCustomPic(_Module.GetModuleInstance(), IDC_BMP_1002, stream);
}

CDlgRotate::~CDlgRotate()
{
}

TMResult CDlgRotate::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
//     _tuiApp.AddMessageFilter(this);
    SetAni(100);
    return 0;
}

TMResult CDlgRotate::OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    KillAni();
    bHandled = false;
//     _tuiApp.RemoveMessageFilter(this);
    return 0;
}

TMResult CDlgRotate::OnZoomed (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_ptrPic.ChangeDPI(wParam);
    bHandled = false;
    return 0;
}


TMResult CDlgRotate::OnOk (TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgRotate::OnCancel (TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgRotate::OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    CTuiRect rc;
    CTuiWidget(pMsg->hWnd).GetClientRect(&rc);
    float cx = (rc.Left() + rc.Right()) / 2;
    float cy = (rc.Top() + rc.Bottom()) / 2;

    _gxApp->Push(eGXPropAffine);
    CAffine aff;
    aff.SetTranslate(-cx, -cy);
    aff.Rotate(m_nAngle);
    aff.Translate(cx, cy);
    _gxApp->SetTransform(&aff, 0, 0, eGXTransformLeftMultiply);
    m_ptrPic->Render(0, 0, rc);
    _gxApp->Pop();

    return 0;
}


//CTuiMsgFilter
// BOOL CDlgRotate::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }

void CDlgRotate::OnAni(...)
{
    m_nAngle += 10;
//    Invalidate();
   Item(IDC_WINDOW_1003).Invalidate();
   Item(IDC_FLOAT_1004).Invalidate();
}
