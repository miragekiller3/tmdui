/**
 *    @file
 *    @brief
 *    CDlgTestcase1
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2017-12-8
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2017 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGTESTCASE1_H
#define DLGTESTCASE1_H

#include "tuires.h"
#include "../h/zoom.h"


class CDlgTestcase1
    : public TM::CTuiWidgetImpl<CDlgTestcase1, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CDlgTestcase1>
//     , public TM::CTuiMsgFilter
    , public CZoomImplT<CDlgTestcase1>
    , public TM::CTimerImpl<CDlgTestcase1>
{
public:
    CDlgTestcase1();
    ~CDlgTestcase1();
    
public:
    enum { IDD = IDC_MANAGE_TESTCASE1 };
    
    BEGIN_TUIMSG_MAP (CDlgTestcase1)
//     BEGIN_MSG_MAP (CDlgSpyMsg)
        MESSAGE_HANDLER     (WM_INITDIALOG,                     OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
        MESSAGE_HANDLER     (WM_HOTKEY,                         OnHotKey)


        COMMAND_HANDLER     (IDC_FLOAT_2010, BN_CLICKED,        OnEnable)
        COMMAND_HANDLER     (IDC_WINDOW_2015, BN_CLICKED,       OnEnable)
        COMMAND_HANDLER     (IDC_WINDOW_1004, BN_CLICKED,       OnShowHide)
        COMMAND_HANDLER     (IDC_WINDOW_1010, BN_CLICKED,       OnShowHide)
        COMMAND_HANDLER     (IDC_FLOAT_1012, BN_CLICKED,        OnShowHide)
        COMMAND_HANDLER     (IDC_WINDOW_1011, BN_CLICKED,       OnShowHide)

        COMMAND_HANDLER     (IDC_PUSHBUTTON_2000, BN_CLICKED,   OnBring)
        COMMAND_HANDLER     (IDC_PUSHBUTTON_2012, BN_CLICKED,   OnSetSize)
        COMMAND_HANDLER     (IDC_PUSHBUTTON_1021, BN_CLICKED,   OnChangeTab)
        COMMAND_HANDLER     (IDC_PUSHBUTTON_2017, BN_CLICKED,   OnHideBAfter5S)

        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)

        CHAIN_MSG_MAP       (CZoomImpl)
    END_MSG_MAP()
    
    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnHotKey (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    TMResult OnEnable (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnShowHide (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnBring (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnSetSize (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnChangeTab (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnHideBAfter5S (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    
    // CTimerImpl
    void OnTimer(...);


    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

};


#endif // DLGTESTCASE1_H