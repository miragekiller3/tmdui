/**
 *    @file
 *    @brief
 *    CDlgMenubar
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2021-3-29
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2021 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGMENUBAR_H
#define DLGMENUBAR_H

#include "tuires.h"
#include "resource.h"


class CDlgMenubar
    : public TM::CTuiWidgetImpl<CDlgMenubar> //, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CDlgMenubar>
//     , public TM::CTuiMsgFilter
{
public:
    CDlgMenubar();
    ~CDlgMenubar();
    
public:
    enum { IDD = IDC_MANAGE_SAMPLE_MENUBAR };
    
    BEGIN_TUIMSG_MAP (CDlgMenubar)
//     BEGIN_MSG_MAP (CDlgMenubar)
//         MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
        COMMAND_ID_HANDLER  (IDC_CHECKBOX_1003,         OnShowHide)
        COMMAND_ID_HANDLER  (IDC_COPY,                  OnCopy)
        
    END_MSG_MAP()
    
    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk (TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnCancel (TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnShowHide (TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnCopy (TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
    {
        ::MessageBox(0, L"copy", L"copy", MB_OK);
        return 0;
    }

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

};


#endif // DLGMENUBAR_H