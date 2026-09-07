/**
 *  @file
 *  @brief
 *  CDlgTooltip2
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2022-7-4
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2022 miragekiller                             
 *  All rights reserved  
 */

#ifndef DLGTOOLTIP2_H
#define DLGTOOLTIP2_H


class CDlgTooltip2
//     : public TM::CTuiWidgetImpl<CDlgTooltip2> //, TM::CTuiWindow>
    : public TM::CTuiDialogImpl<CDlgTooltip2>
//     , public TM::CTuiMsgFilter
{
public:
    CDlgTooltip2();
    ~CDlgTooltip2();
    
public:
    enum { IDD = IDC_MANAGE_TOOLTIP2 };
    
//    BEGIN_TUIMSG_MAP (CDlgTooltip2)
    BEGIN_MSG_MAP (CDlgTooltip2)
//         MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT,     OnPrintClient)
//         COMMAND_ID_HANDLER  (IDOK,                      OnOk)
//         COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)

        MESSAGE_HANDLER     (TTM_ADDTOOL,             OnAddTool)
        MESSAGE_HANDLER     (WM_WINDOWPOSCHANGING,    OnWindowPosChanging)
    END_MSG_MAP()
    
    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);

    TMResult OnAddTool (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnWindowPosChanging (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);


    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

};


#endif // DLGTOOLTIP2_H