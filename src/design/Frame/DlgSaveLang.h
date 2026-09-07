/**
 *    @file
 *    @brief
 *    CDlgSaveLang
 *
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2016-12-27
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2016 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGSAVELANG_H
#define DLGSAVELANG_H

#include "tuires.h"
#include "../Design.h"

class CMainWnd;

class CDlgSaveLang
//    : public TM::CTuiWidgetImpl<CDlgSaveLang> //  , TM::CTuiWindow>
     : public TM::CTuiDialogImpl<CDlgSaveLang>
{
public:
    CMainWnd& m_wndMain;
    LPCTSTR m_pPath;
    
public:
    CDlgSaveLang(CMainWnd& wnd, LPCTSTR pPath);
    ~CDlgSaveLang();
    
public:
    enum { IDD = IDC_TUI_SAVE_LANG };
    
    BEGIN_MSG_MAP (CDlgSaveLang)
//       BEGIN_MSG_MAP (CDlgSaveLang)
//         MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//           ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
    END_MSG_MAP()
    
    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//       TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnAutoTrans (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    ESaveMode GetSaveMode();
    
};


#endif // DLGSAVELANG_H