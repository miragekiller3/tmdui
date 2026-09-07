/**
 *    @file
 *    @brief
 *    CDlgCheckKeyword
 *
 *
 *    @author miragekiller <3916345933@qq.com>
  *    @date    2017-6-13
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2017 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGCHECKKEYWORD_H
#define DLGCHECKKEYWORD_H

#include "tuires.h"
#include "Tools.h"

class CDlgCheckKeyword
    : public TM::CTuiWidgetImpl<CDlgCheckKeyword> //, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CDlgCheckKeyword>
{
public:
    CLineMap m_aLines;
    CPreSearchID m_aListParent;
//    TM::HTVITEM m_objCur;
    class CWndPanel* m_panel;

public:
    CDlgCheckKeyword();
    ~CDlgCheckKeyword();
    
public:
    enum { IDD = IDC_MANAGE_CHECK_KEYWORD };
    
    BEGIN_TUIMSG_MAP (CDlgCheckKeyword)
//       BEGIN_MSG_MAP (CDlgCheckKeyword)
        MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//           ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)


        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_30268,      OnImport)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_30261,      OnSelDir)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_30254,      OnStart)

        COMMAND_ID_HANDLER  (IDC_RADIOBUTTON_31082,     OnSort)
        COMMAND_ID_HANDLER  (IDC_CHECKBOX_31085,        OnSort)

        COMMAND_HANDLER     (IDC_LISTBOX_30252, TVN_SELCHANGE,  OnSelChange)
        COMMAND_HANDLER     (IDC_MEMO_30269, EN_CHANGE,         OnEnChange)
        COMMAND_HANDLER     (IDC_MEMO_30270, EN_CHANGE,         OnEnChange)
    END_MSG_MAP()
    
    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//       TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnImport (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnSelDir (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnStart (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    TMResult OnSort (...);

    TMResult OnSelChange (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnEnChange (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
};


#endif // DLGCHECKKEYWORD_H