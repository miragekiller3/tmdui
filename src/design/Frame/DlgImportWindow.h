/**
 *    @file
 *    @brief
 *    CDlgImportWindow
 *
 *
 *    @author 
 *    @date    2016-9-6
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2016 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGIMPORTWINDOW_H
#define DLGIMPORTWINDOW_H

#include "tuires.h"

class CUndo;

class CDlgImportWindow
//     : public TM::CTuiWidgetImpl<CDlgImportWindow>
     : public TM::CTuiDialogImpl<CDlgImportWindow>
{
public:
    HWND m_hTarget;
    HWND m_hSel;
    CUndo* m_pUndo;

public:
    CDlgImportWindow();
    ~CDlgImportWindow();
    
public:
    enum { IDD = IDC_MANAGE_IMPORT_WINDOW };
    
//    BEGIN_TUIMSG_MAP (CDlgImportWindow)
       BEGIN_MSG_MAP (CDlgImportWindow)
        MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//           ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_30224,      OnParent)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_30230,      OnChild)
        ROUTE_OID_HANDLER   (IDC_WINDOW_30223, WM_MOUSEMOVE,    OnDragMove)
    END_MSG_MAP()
    
    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//       TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnDragMove(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnParent (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnChild (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    
    void SetTarget(HWND hWnd);
    void ImportFromWindow(HWND hWnd);
};


#endif // DLGIMPORTWINDOW_H