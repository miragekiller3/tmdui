/**
 *  @file
 *  @brief
 *  CDlgTesterSetting
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021-12-9
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2021 miragekiller                             
 *  All rights reserved  
 */

#ifndef DLGTESTERSETTING_H
#define DLGTESTERSETTING_H

#include "tuires.h"

class CWndPanel;

class CDlgTesterSetting
    : public TM::CTuiWidgetImpl<CDlgTesterSetting> //, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CDlgTesterSetting>
//     , public TM::CTuiMsgFilter
{
public:
    CWndPanel* m_panel;

public:
    CDlgTesterSetting();
    ~CDlgTesterSetting();
    
public:
    enum { IDD = IDC_MANAGE_TESTER_SETTING };
    
    BEGIN_TUIMSG_MAP (CDlgTesterSetting)
//     BEGIN_MSG_MAP (CDlgTesterSetting)
        MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT,     OnPrintClient)
        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)

        COMMAND_ID_HANDLER  (IDC_UI,                    OnUI)
        COMMAND_ID_HANDLER  (IDC_APP,                   OnApp)

        COMMAND_HANDLER     (IDC_APPCLR, EN_UPDATE,     OnColor)

    END_MSG_MAP()
    
    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);

    TMResult OnUI(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnApp(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnColor(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

    void InitUI();
    void InitApp();
    void InitColor();
};


#endif // DLGTESTERSETTING_H