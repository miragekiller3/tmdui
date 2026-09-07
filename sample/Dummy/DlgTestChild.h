/**
 *    @file
 *    @brief
 *    CDlgTestChild
 *
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2017-2-28
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2017 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGTESTCHILD_H
#define DLGTESTCHILD_H

#include "tuires.h"



class CDlgTestChild
    : public TM::CTuiWidgetImpl<CDlgTestChild> //, TM::CTuiWindow>
#ifdef __cpp_lib_enable_shared_from_this
    , public std::enable_shared_from_this<CDlgTestChild>
#endif
//     : public TM::CTuiDialogImpl<CDlgTestChild>
{
public:
    int m_mode;
    TMBool m_bLayeredChild;
    TM::CString m_strTitle;
    HWND m_hWin;
    
public:
    CDlgTestChild();
    ~CDlgTestChild();
    
public:
    enum { IDD = IDC_MANAGE_TEST_CHILD };
    
    BEGIN_TUIMSG_MAP (CDlgTestChild)
//       BEGIN_MSG_MAP (CDlgTestChild)
        MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//           ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
        COMMAND_ID_HANDLER  (IDC_SHOW1,                 OnShow1)
        COMMAND_ID_HANDLER  (IDC_ENABLE1,               OnEnable1)
        COMMAND_ID_HANDLER  (IDC_ATTACH,                OnAttach)

        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
    END_MSG_MAP()
    
    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//       TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    TM::HTUI CreateWCL(int id);
    HWND CreateWin(TMUInt uID, TMUInt32 dwStyle = WS_CHILD);
    HWND CreateDlg(TMUInt uID, TMUInt uDlgID);

    TMResult OnShow1 (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnEnable1 (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnAttach (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

};


#endif // DLGTESTCHILD_H