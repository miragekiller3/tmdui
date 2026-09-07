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

#ifndef DLGGXTESTBLTDIR_H
#define DLGGXTESTBLTDIR_H

#include "tuires.h"


class CDlgGxTestBltDir
//    : public TM::CTuiWidgetImpl<CDlgGxTestBltDir> //, TM::CTuiWindow>
     : public TM::CTuiDialogImpl<CDlgGxTestBltDir>
//     , public TM::CTuiMsgFilter
{
public:
//     int m_format;
//     int m_reset;
//     TMUInt8 m_alpha;
    TM::CTexture m_tex;
//     TM::CTexture m_tex_a;
//     CEdit m_edit;
    
public:
    CDlgGxTestBltDir();
    ~CDlgGxTestBltDir();
    
public:
    enum { IDD = IDC_MANAGE_GX_TEST_DIRECTION };
    
    BEGIN_MSG_MAP (CDlgGxTestBltDir)
//     BEGIN_MSG_MAP (CDlgGxTestBltDir)
        MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
        MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         MESSAGE_HANDLER     (WM_MOUSEMOVE,              OnMouseMove)
        MESSAGE_HANDLER     (CM_RENDERCHANGED,          OnRenderChanged)

        
        ROUTE_OID_HANDLER   (IDC_GROUPBOX_1216, WM_PRINTCLIENT,     OnPrintClient)
        ROUTE_OID_HANDLER   (IDC_GROUPBOX_1217, WM_PRINTCLIENT,     OnPrintClient)

        COMMAND_HANDLER     (IDC_DIR_MIRROR,    BN_CLICKED,         OnEnUpdate)
        COMMAND_HANDLER     (IDC_DIR_B2T,       BN_CLICKED,         OnEnUpdate)
        COMMAND_HANDLER     (IDC_DIR_ROTATE0,   BN_CLICKED,         OnEnUpdate)

        COMMAND_HANDLER     (IDC_EDIT_ALPHA,    EN_UPDATE,          OnEnUpdate)
        
        

        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)

    END_MSG_MAP()
    
    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnRenderChanged(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnMouseMove(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
//     TMResult OnFormat(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
//     TMResult OnReset(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
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


#endif // DLGGXTESTBLTDIR_H