/**
 *  @file
 *  @brief
 *  CDlgPlayHz
 *
 *  @author YourUserName <name@Email.com>
 *  @date   2026-4-3
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2026 miragekiller                             
 *  All rights reserved  
 */

#ifndef DLGPLAYHZ_H
#define DLGPLAYHZ_H

#include "tuires.h"
#include "tmtimer.h"


class CDlgPlayHz
    : public TM::CTuiWidgetImpl<CDlgPlayHz> //, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CDlgPlayHz>
//     , public TM::CTuiMsgFilter
{
private:
    TM::CTimerID1 m_ani1;
    DWORD m_tick;

public:
    CDlgPlayHz();
    ~CDlgPlayHz();
    
public:
    enum { IDD = IDC_MANAGE_PLAY_HZ };
    
    BEGIN_TUIMSG_MAP (CDlgPlayHz)
//     BEGIN_MSG_MAP (CDlgPlayHz)
//         MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT,     OnPrintClient)
//         CMNOTIFY_HANDLER    (IDC_HD, HDN_ITEMCLICK,     OnHeaderClicked)
        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)

        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_PLAY,       OnPlay)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_STOP,       OnStop)
        COMMAND_HANDLER     (IDC_EDIT_FREQ, EN_UPDATE,  OnEnChange)

    END_MSG_MAP()
    
    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
//     TMResult OnHeaderClicked(int idCtrl, TM::NMBASE* pnmh, TMBool& bHandled);
    TMResult OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);

    TMResult OnPlay(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnStop(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnEnChange(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

    void OnTimer(TMHTimer hTimer, TMClock tmNow);
};


#endif // DLGPLAYHZ_H