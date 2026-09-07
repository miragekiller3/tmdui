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

#ifndef DLGGXTEST_H
#define DLGGXTEST_H

#include "tuires.h"


class CDlgGxTest
//    : public TM::CTuiWidgetImpl<CDlgGxTest> //, TM::CTuiWindow>
     : public TM::CTuiDialogImpl<CDlgGxTest>
//     , public TM::CTuiMsgFilter
{
public:
    CDlgGxTest();
    ~CDlgGxTest();
    
public:
    enum { IDD = IDC_MANAGE_GX_TEST };
    //enum { IDD = IDC_MANAGE_GX_TEST_TEMP };
    
    BEGIN_MSG_MAP (CDlgGxTest)
//     BEGIN_MSG_MAP (CDlgGxTest)
//         MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         MESSAGE_HANDLER     (WM_MOUSEMOVE,              OnMouseMove)
//         MESSAGE_HANDLER     (CM_RENDERCHANGED,          OnRenderChanged)
        ROUTE_OID_HANDLER   (IDC_WINDOW_TEMP1,  WM_PRINTCLIENT,      OnPrintClient)
        ROUTE_OID_HANDLER   (IDC_WINDOW_TEMP2,  WM_PRINTCLIENT,      OnPrintClient)
        ROUTE_OID_HANDLER   (IDC_GROUPBOX_1002, WM_PRINTCLIENT,      OnPrintClient)
        ROUTE_OID_HANDLER   (IDC_GROUPBOX_1003, WM_PRINTCLIENT,      OnPrintClient)
        ROUTE_OID_HANDLER   (IDC_GROUPBOX_1004, WM_PRINTCLIENT,      OnPrintClient)
        ROUTE_OID_HANDLER   (IDC_GROUPBOX_1005, WM_PRINTCLIENT,      OnPrintClient)
        ROUTE_OID_HANDLER   (IDC_GROUPBOX_1006, WM_PRINTCLIENT,      OnPrintClient)
        ROUTE_OID_HANDLER   (IDC_GROUPBOX_1008, WM_PRINTCLIENT,      OnPrintClient)
//         ROUTE_OID_HANDLER   (IDC_GROUPBOX_1022, WM_PRINTCLIENT,      OnPrintClient)
//         ROUTE_OID_HANDLER   (IDC_GROUPBOX_1024, WM_PRINTCLIENT,      OnPrintClient)
//         ROUTE_OID_HANDLER    (IDC_GROUPBOX_1008, WM_PRINTCLIENT,      OnPrintClient)

        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)

    END_MSG_MAP()
    
    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnRenderChanged(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnMouseMove(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);


    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

    void Append(LPCWSTR str)
    {
//         if (m_edit)
//         {
//             m_edit.AppendText(str);
//             m_edit.AppendText(L"\r\n");
//         }
    }

};


#endif // DLGGXTEST_H