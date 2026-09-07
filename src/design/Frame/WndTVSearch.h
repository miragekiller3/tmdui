/**
 *    @file
 *    @brief
 *    CWndTVSearch
 *
 *
 *    @author 
 *    @date    2014-5-1
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2014 miragekiller                               
 *    All rights reserved  
 */

#ifndef WNDTVSEARCH_H
#define WNDTVSEARCH_H

#include "tuires.h"


class CWndTVSearch
     : public TM::CTuiWidgetImpl<CWndTVSearch>
//     : public TM::CTuiDialogImpl<CWndTVSearch>
{
private:
    TM::CTuiTree m_wndTree;
    TM::CStrPtr  m_txtKey;
//     int       m_nTxtLen;

public:
    CWndTVSearch();
    ~CWndTVSearch();
    
public:
    enum { IDD = IDO_TUI_OBJ };
    
    BEGIN_TUIMSG_MAP (CWndTVSearch)
//       BEGIN_MSG_MAP (CWndTVSearch)
//         MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//           ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
//         COMMAND_ID_HANDLER  (IDOK,                      OnOk)
//         COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
        COMMAND_HANDLER     (IDC_FILTER, EN_CHANGE,        OnEnChange)
        COMMAND_ID_HANDLER  (IDB_CLEAR_FILTER,            OnEditClear)
        COMMAND_ID_HANDLER  (IDB_GOTO_SEL,                OnGotoSel)
        
    END_MSG_MAP()
    
//     TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnEnChange(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnEditClear(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnGotoSel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

//       TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
//     TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    void SetTarget(TM::HTUI hWgt) { m_wndTree = hWgt; }
};

// extern TM::CStaticWindowT<CWndTVSearch> g_wndSearchProp;
// extern TM::CStaticWindowT<CWndTVSearch> g_wndSearchObj;


#endif // WNDTVSEARCH_H