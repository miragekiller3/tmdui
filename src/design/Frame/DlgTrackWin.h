/**
 *    @file
 *    @brief
 *    CDlgTrackWin
 *
 *  @author miragekiller <3916345933@qq.com>
 *    @date    2018-6-21
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2018 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGTRACKWIN_H
#define DLGTRACKWIN_H

#include "tuires.h"
#include "tmtimer.h"

class CMainWnd;
class CWndPanel;

class CDlgTrackWin
//     : public TM::CTuiWidgetImpl<CDlgTrackWin> //, TM::CTuiWindow>
    : public TM::CTuiDialogImpl<CDlgTrackWin>
    , public TM::CTimerImpl<CDlgTrackWin>
{
public:
    int m_nMode;
    CMainWnd* m_pMain;

public:
    CDlgTrackWin();
    ~CDlgTrackWin();
    
public:
    enum { IDD = IDC_MANAGE_TARGET_SELECT };
    
    BEGIN_MSG_MAP (CDlgTrackWin)
//     BEGIN_MSG_MAP (CDlgTrackWin)
        MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
        MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
//         COMMAND_ID_HANDLER  (IDOK,                      OnOk)
//         COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
    END_MSG_MAP()
    
    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
//     TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    // TM::CTimerImpl
    virtual void OnTimer(...);

    static void SetTrack(CMainWnd* pMain, int nMode, TMBool bShow);
};


#endif // DLGSPYFOCUS_H