/**
 *    @file
 *    @brief
 *    CDlg1000
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2018-5-22
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2018 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLG1000_H
#define DLG1000_H

#include "tuires.h"


class CDlg1000
    : public TM::CTuiWidgetImpl<CDlg1000> //, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CDlg1000>
//     , public TM::CTuiMsgFilter
{
private:
    DWORD m_dwStyle;

public:
    CDlg1000();
    ~CDlg1000();
    
public:
    enum { IDD = IDC_MANAGE_1000 };
    
    BEGIN_TUIMSG_MAP (CDlg1000)
//     BEGIN_MSG_MAP (CDlgSpyMsg)
        MESSAGE_HANDLER     (WM_INITDIALOG,                         OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)

        ROUTE_ID_HANDLER    (IDC_COMBOBUTTON_1003, WM_NCCREATE,     OnNcCreate)

        MESSAGE_HANDLER     (WM_COMMAND,                            OnCommand)

        COMMAND_ID_HANDLER  (IDC_SETTYPE,                           OnSetType)
        COMMAND_HANDLER     (IDC_COMBOBUTTON_1003, CBN_DROPDOWN,    OnDropDown)
        COMMAND_ID_HANDLER  (IDOK,                                  OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                              OnCancel)
    END_MSG_MAP()
    
    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnNcCreate(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnCommand (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnSetType (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnDropDown(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

};


#endif // DLG1000_H