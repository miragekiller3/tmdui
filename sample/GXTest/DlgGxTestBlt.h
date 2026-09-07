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

#ifndef DLGGXTESTBLT_H
#define DLGGXTESTBLT_H

#include "tuires.h"


class CDlgGxTestBlt
//    : public TM::CTuiWidgetImpl<CDlgGxTestBlt> //, TM::CTuiWindow>
     : public TM::CTuiDialogImpl<CDlgGxTestBlt>
//     , public TM::CTuiMsgFilter
{
public:
    int m_format;
    int m_reset;
    TMUInt8 m_alpha;
    TM::CTexture m_tex;
    TM::CTexture m_tex_a;
//     CEdit m_edit;
    
public:
    CDlgGxTestBlt();
    ~CDlgGxTestBlt();
    
public:
    enum { IDD = IDC_MANAGE_GX_TEST_BLT };
    
    BEGIN_MSG_MAP (CDlgGxTestBlt)
//     BEGIN_MSG_MAP (CDlgGxTestBlt)
        MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
        MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         MESSAGE_HANDLER     (WM_MOUSEMOVE,              OnMouseMove)
        MESSAGE_HANDLER     (CM_RENDERCHANGED,          OnRenderChanged)

        
        ROUTE_OID_HANDLER   (IDC_GROUPBOX_1082, WM_PRINTCLIENT,      OnPrintClient)
        ROUTE_OID_HANDLER   (IDC_GROUPBOX_1084, WM_PRINTCLIENT,      OnPrintClient)
        ROUTE_OID_HANDLER   (IDC_GROUPBOX_1086, WM_PRINTCLIENT,      OnPrintClient)
        ROUTE_OID_HANDLER   (IDC_GROUPBOX_1088, WM_PRINTCLIENT,      OnPrintClient)
        ROUTE_OID_HANDLER   (IDC_GROUPBOX_1090, WM_PRINTCLIENT,      OnPrintClient)
        ROUTE_OID_HANDLER   (IDC_GROUPBOX_1091, WM_PRINTCLIENT,      OnPrintClient)
//         ROUTE_OID_HANDLER    (IDC_GROUPBOX_1008, WM_PRINTCLIENT,      OnPrintClient)


        COMMAND_HANDLER     (IDC_RADIOBUTTON_FMT_32,        BN_CLICKED,    OnFormat)
        COMMAND_HANDLER     (IDC_RADIOBUTTON_FMT_32A,       BN_CLICKED,    OnFormat)
        COMMAND_HANDLER     (IDC_RADIOBUTTON_FMT_24,        BN_CLICKED,    OnFormat)
        COMMAND_HANDLER     (IDC_RADIOBUTTON_FMT_NV12,      BN_CLICKED,    OnFormat)

        COMMAND_HANDLER     (IDC_RADIOBUTTON_RST_NONE,      BN_CLICKED,    OnReset)
        COMMAND_HANDLER     (IDC_RADIOBUTTON_RST_DIB,       BN_CLICKED,    OnReset)
        COMMAND_HANDLER     (IDC_RADIOBUTTON_RST_A,         BN_CLICKED,    OnReset)
        COMMAND_HANDLER     (IDC_RADIOBUTTON_RST_DIB_A,     BN_CLICKED,    OnReset)
        COMMAND_HANDLER     (IDC_RADIOBUTTON_RST_DIB_KA,    BN_CLICKED,    OnReset)

        COMMAND_HANDLER     (IDC_EDIT_ALPHA,                EN_UPDATE,     OnEnUpdate)
        
        

        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)

    END_MSG_MAP()
    
    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnRenderChanged(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnMouseMove(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnFormat(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnReset(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnEnUpdate(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);


    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

//     void Append(LPCWSTR str)
//     {
// //         if (m_edit)
// //         {
// //             m_edit.AppendText(str);
// //             m_edit.AppendText(L"\r\n");
// //         }
//     }

    void Reset();

};


#endif // DLGGXTESTBLT_H