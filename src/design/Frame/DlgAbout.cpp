/**
 *    @file
 *    @brief
 *    CDlgAbout
 *
 *
 *    @author 
 *    @date    2015-2-8
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2015 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "DlgAbout.h"
#include <shellapi.h>
#include "tmani.h"

using namespace TM;

CDlgAbout::CDlgAbout()
{
}

CDlgAbout::~CDlgAbout()
{
}

TMResult CDlgAbout::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    char cmonth[5];
    int day, year;
    const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    sscanf(__DATE__, "%s %d %d", cmonth, &day, &year);
    int month = 1 + (strstr(month_names, cmonth)-month_names) / 3;

    CTuiWidget wnd(Item(IDC_STATIC_30241));
    CString strFmt;
    wnd.GetWindowText(strFmt);
    TM::CString str;
    str.Format(strFmt, TUI_FILE_VER_CURRENT, year * 10000 + month * 100 + day);
    wnd.SetWindowText(str);

    OnCompositionChanged(0, 0, 0, bHandled);

    bHandled = false;
    return 0;
}

TMResult CDlgAbout::OnCompositionChanged (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    EThemeType eType = this->GetThemeType();
    if (eType == eThemeTypeDWMBlur)
        this->SetBkPic(IDC_CLR_BLUR);
    else
        this->SetBkPic(IDP_CLR_0xFFFFFF);
    return 0;
}


// TMResult CDlgAbout::OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     bHandled = false;
//     KillAni();
//     return 0;
// }
// 
// TMResult CDlgAbout::OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
// {
// //    EndDialog(wID);
//     DestroyWindow();
//     return 0;
// }

TMResult CDlgAbout::OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgAbout::OnURL (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CString str;
    Item(wID).GetWindowText(str);
    ShellExecute(NULL, TM_T("open"), str, NULL, NULL, SW_SHOWNORMAL);
    return 0;
}

// class CAniMove
//     : public CAniValWindowT<CAniMove>
//     , public CStaticInstanceCheckBaseT<CAniMove>
// {
// public:
//     CTuiWidget m_wnd1;
//     CTuiWidget m_wnd2;
//     CTuiRect m_rc1, m_rc2;
//     
//     void Start(CTuiWidget wnd1, CTuiWidget wnd2)
//     {
//         m_wnd1 = wnd1;
//         m_wnd2 = wnd2;
// 
//         SetAni(10, 30);
//         
//         CTuiRect rc1, rc2;
//         wnd1.GetWindowRect(rc1);
//         wnd2.GetWindowRect(rc2);
// 
//         AddRect(m_rc1, rc1, rc2);
//         AddRect(m_rc2, rc2, rc1);
//     }
// 
//     virtual void OnAniValFrame(TMHAniVal hAni, TMUInt nFrame, void* data)
//     {
//         m_wnd1.MoveWindow(m_rc1);
//         m_wnd2.MoveWindow(m_rc2);
//         m_wnd1.GetAPP()->RecheckHot();
//         UpdateWindow();
//     }
// };

TMResult CDlgAbout::OnMouseEntry(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    CTuiWidget wnd1(Item(IDOK)), wnd2(pMsg->hWnd);
    wnd1.StopAnimateWindow();
    wnd2.StopAnimateWindow();

    CTuiRect rc1, rc2;
    wnd1.GetWindowRect(rc1);
    wnd2.GetWindowRect(rc2);
    CRefPtr<IEase> ptrEase;
    EaseCreateElastic(&ptrEase, 3, 10);
    wnd1.AnimateWindow(3000, 0, &rc1, &rc2, ptrEase);
    wnd2.AnimateWindow(3000, 0, &rc2, &rc1, ptrEase);

//     if (!CAniMove::GetInstance())
//         CAniMove::CreateInstance(m_hWnd)->Start(Item(IDOK), pMsg->hWnd);
    bHandled = false;
    return 0;
}

// class CAniMove2
//     : public CAniValWindowT<CAniMove2>
//     , public CStaticInstanceCheckBaseT<CAniMove2>
// {
// public:
//     CTuiWidget m_wnd1;
//     CTuiRect m_rc1;
//     
//     void Start(CTuiWidget wnd1)
//     {
//         SetAni(10, 30);
//         m_wnd1 = wnd1;
//         CTuiRect rc1;
//         wnd1.GetWindowRect(rc1);
//         
//         CTuiRect rc2;
//         GetWindowRect(&rc2);
// //        srand (GetTickCount());
//         GXDips l = rc2.left + MulDiv (rc2.Width() - rc1.Width(), rand(), RAND_MAX);
//         GXDips t = rc2.top + MulDiv (rc2.Height() - rc1.Height(), rand(), RAND_MAX);
//         rc2 = rc1;
//         rc2.MoveToXY(l,t);
// 
//         AddRect(m_rc1, rc1, rc2);
//     }
//     
//     virtual void OnAniValFrame(TMHAniVal hAni, TMUInt nFrame, void* data)
//     {
//         m_wnd1.MoveWindow(m_rc1);
//         m_wnd1.GetAPP()->RecheckHot();
//     }
//     
//     // don't close instance
//     virtual void OnAniValStop(TMHAniVal hAni, TMUInt nFrame, void* data)
//     {
//     }
// };

TMResult CDlgAbout::OnMouseCloseEntry(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    CTuiWidget wnd(pMsg->hWnd);
    CTuiRect rc1, rc2, rcc;
    wnd.GetWindowRect(rc1);
    GetClientRect(&rcc);
    rc2 = rc1;
    rc2.MoveToY(rcc.bottom - rc1.Height());

    CRefPtr<IEase> ptrEase;
    EaseCreateBounce(&ptrEase, 6, 2);
    wnd.AnimateWindow(2000, 0, &rc1, &rc2, ptrEase);


//     if (!CAniMove2::GetInstance())
//         CAniMove2::CreateInstance(Item(IDC_WINDOW_31076))->Start(pMsg->hWnd);
    bHandled = false;
    return 0;
}


TMResult CDlgAbout::OnSetCursor(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    SetCursor(LoadCursor(0, IDC_HAND));
    return 0;
}

