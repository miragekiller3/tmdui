/**
 *  @file
 *  @brief
 *  CWndAcc
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2022-7-11
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2022 miragekiller                             
 *  All rights reserved  
 */

#ifndef WNDACC_H
#define WNDACC_H

#include "WndPerformance.h"

class CWndAcc
//     : public TM::CTuiWidgetImpl<CWndAcc> //, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CWndAcc>
//     , public TM::CTuiMsgFilter
    : public CWndRefeshWindow<CWndAcc>
{
public:
    CWndAcc();
    ~CWndAcc();
    
public:
    enum { IDD = IDC_MEMO_ACC };
    
//     BEGIN_TUIMSG_MAP (CWndAcc)
//     BEGIN_MSG_MAP (CWndAcc)
//         MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT,     OnPrintClient)
//         COMMAND_ID_HANDLER  (IDOK,                      OnOk)
//         COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
//     END_MSG_MAP()
    
//     TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
// //     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
//     TMResult OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
//     TMResult OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

    //  CAniProc
    void OnTimer(TMHTimer, ...);

    CWndPanel& GetPanel();

};


#endif // WNDACC_H