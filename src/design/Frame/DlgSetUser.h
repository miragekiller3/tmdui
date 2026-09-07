/**
 *    @file
 *    @brief
 *    CDlgSetUser
 *
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2017-1-13
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2017 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGSETUSER_H
#define DLGSETUSER_H

#include "tuires.h"
#include "tmarchiveex.h"


class CSetUserDlg : public TM::CTuiDialogImpl<CSetUserDlg>
{
public:
    TM::CString strUser;
    TM::CString strEmail;
    
public:
    enum { IDD = IDD_TUI_SETUSER };
    
//     BGN_TUI_DDX_MAP(CSetUserDlg, m_tuiWindow)
//         TUI_DDX_TEXT(IDE_USER, strUser)
//         TUI_DDX_TEXT(IDC_EDIT_32000, strEmail)
//     END_TUI_DDX_MAP()
//         
//     BGN_PROFILE_DDX_MAP(CSetUserDlg, _Module.m_strConfFileName)
//         PROFILE_DDX_SECTION (TM_T("Admin"))
//         PROFILE_DDX_TEXT (TM_T("username"), strUser)
//         PROFILE_DDX_TEXT (TM_T("useremail"), strEmail)
//     END_PROFILE_DDX_MAP()

    SERIALIZE_BGN_MAP2      (DoProfile, CSetUserDlg)
        SERIALIZE_ENTRY     (TM_T("Admin"))
            SERIALIZE_VAR   (TM_T("username"), strUser)
            SERIALIZE_VAR   (TM_T("useremail"), strEmail)
        SERIALIZE_LEAVE()
    SERIALIZE_END_MAP()

    SERIALIZE_BGN_MAP       (CSetUserDlg)
        SERIALIZE_ENTRY     (TM_T("Admin"))
            SERIALIZE_VAR   (TM_T("username"), TM::CSerializeWindowTextT<TM::CTuiWidget>(Item(IDE_USER)))
            SERIALIZE_VAR   (TM_T("useremail"), TM::CSerializeWindowTextT<TM::CTuiWidget>(Item(IDC_EDIT_32000)))
        SERIALIZE_LEAVE()
    SERIALIZE_END_MAP()
        
    BEGIN_MSG_MAP(CSetUserDlg)
        MESSAGE_HANDLER     (WM_INITDIALOG,         OnInitDialog)
        COMMAND_ID_HANDLER  (IDOK,                  OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,              OnCancel)
    END_MSG_MAP()
        
    CSetUserDlg();
    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    LRESULT OnOk(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//     void SetUser();
    TM::CString GetUser() const;
};


#endif // DLGSETUSER_H