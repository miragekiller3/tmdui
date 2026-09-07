//-----------------------------------------------------------
//main.h
//
//  Copyright (C) 2009 miragekiller                               
//  All rights reserved                                                     
//
//
//  Author: 
//      miragekiller    <3916345933@qq.com>
//
//    Histroy :
//      2009.02.04      create
//-------------------------------------------------------------

#ifndef MAIN_H
#define MAIN_H

#include "Resource.h"
#include "tuires.h"
#include "tv.h"



class CMain
    : public TM::CTuiDialogImpl<CMain>
    , public TM::ITVEvent
{
public:
    enum { IDD = IDD_TUI_MANAGE_10002 };

public:
    CTVCtrl m_tv;
    TM::CTuiMemo m_tvLog;
    
public:
    BEGIN_MSG_MAP           (CMain)
        MESSAGE_HANDLER     (WM_INITDIALOG,         OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,            OnDestroy)
        COMMAND_ID_HANDLER  (IDOK,                  OnCancel)
        COMMAND_ID_HANDLER  (IDCANCEL,              OnCancel)
        COMMAND_ID_HANDLER  (IDB_ADDITEM,           OnAddItem)
        COMMAND_ID_HANDLER  (IDB_ADDHTML,           OnAddHtml)
        
        COMMAND_ID_HANDLER  (IDB_DELSEL,            OnDelItem)
        COMMAND_HANDLER     (IDE_FILTER, EN_CHANGE, OnChange)
        COMMAND_ID_HANDLER  (IDB_DEC,               OnDec)
        COMMAND_ID_HANDLER  (IDB_ADD,               OnAdd)

//         ROUTE_ID_HANDLER(IDB_ADD, WM_PRINTCLIENT, OnPrintClient);

        CHAIN_MSG_MAP       (root_class)
    END_MSG_MAP()

//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
//         {
//             bHandled = false;
//             TM::CTuiRect rc;
//             Item(IDB_ADD).GetClientRect(rc);
//             rc.SetWidth(20);
//             gxFillRect(rc, TMRGB(255,0,0));
//             return 0;
//     }

    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

//     TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
//     {
//         bHandled = false;
//         TM::CTuiWidget wnd(Item(IDE_FILTER));
//         return 0;
//     }
    TMResult OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        m_tuiWindow.DestroyWindow();
        return 0;
    }



//     TMResult OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
//     {
//         EndDialog(wID);
//         return 0;
//     }

    TMResult OnAddItem(...) { return AddItem(false), 0; }
    TMResult OnAddHtml(...) { return AddItem(true), 0; }
    TMResult OnDelItem(...);
    TMResult OnChange(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnDec(...);
    TMResult OnAdd(...);

    void AddItem(TMBool bHTML);
    void AddLog (TMCStr p);

    //ITVEvent
    virtual void OnDeleteItem (TM::HTVITEM hItem);
    
    virtual void OnStateChanged (TM::HTVITEM hItem, TMUInt uOld, TMUInt uNew);
    
    virtual void OnSelChanged (TM::HTVITEM hOld, TM::HTVITEM hNew,TMUInt uKey);

};    




#endif // MAIN_H