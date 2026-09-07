#ifndef MAIN_H
#define MAIN_H

// #include "Resource.h"
#include "tuires.h"

class CMain : public TM::CTuiWidgetImpl<CMain, TM::CTuiWindow>
{
public:
    enum { IDD = IDD_TUI_MANAGE_10002 };
    
public:
    BEGIN_TUIMSG_MAP           (CMain)
        COMMAND_ID_HANDLER  (IDOK,                      OnCancel)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
        ROUTE_TAG_HANDLER   (99, WM_MOUSEMOVE,          OnMouseMove)
        ROUTE_ID_HANDLER    (IDC_ACCEPT1, CM_DRAGENTRY, OnDragEntry)
        ROUTE_ID_HANDLER    (IDC_ACCEPT2, CM_DRAGENTRY, OnDragEntry)
        ROUTE_ID_HANDLER    (IDC_ACCEPT1, CM_DRAGOVER,  OnDragOver)
        ROUTE_ID_HANDLER    (IDC_ACCEPT2, CM_DRAGOVER,  OnDragOver)
        ROUTE_ID_HANDLER    (IDC_ACCEPT1, CM_DRAGDROP,  OnDragDrop)
        ROUTE_ID_HANDLER    (IDC_ACCEPT2, CM_DRAGDROP,  OnDragDrop)
//         CHAIN_MSG_MAP       (root_class)
    END_TUIMSG_MAP()
    
    TMResult OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnMouseMove(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnDragEntry(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnDragOver(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnDragDrop(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);

//     class AAA : public TM::CWndMsgMap
//     {
//         virtual TMBool ProcessWindowMessage(TMHWnd hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)
//         {
//             TMBool bHandled;
//             TM_OTHIS(CMain, aaa)->OnCancel(0,0,0,bHandled);
//             return true;
//         }
//     } aaa;

};    


#endif // MAIN_H