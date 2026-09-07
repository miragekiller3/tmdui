/**
 *  @file
 *  @brief
 *  CDlgAutoTranslate
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021-10-20
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2021 miragekiller                             
 *  All rights reserved  
 */

#ifndef DLGAUTOTRANSLATE_H
#define DLGAUTOTRANSLATE_H

#include "tuires.h"


class CDlgAutoTranslate
    : public TM::CTuiWidgetImpl<CDlgAutoTranslate> //, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CDlgAutoTranslate>
//     , public TM::CTuiMsgFilter
{
private:
    TM::CString m_StrFile;

public:
    CDlgAutoTranslate();
    ~CDlgAutoTranslate();
    
public:
    enum { IDD = IDC_MANAGE_AUTO_TRANSLATE };
    
    BEGIN_TUIMSG_MAP (CDlgAutoTranslate)
//     BEGIN_MSG_MAP (CDlgAutoTranslate)
//         MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT,     OnPrintClient)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)

        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_LOAD,       OnLoad)
        COMMAND_ID_HANDLER  (IDOK,                      OnSave)
        
    END_MSG_MAP()
    
    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);

    TMResult OnLoad(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnSave(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

};


#endif // DLGAUTOTRANSLATE_H