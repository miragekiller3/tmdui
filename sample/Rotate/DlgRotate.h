/**
 *    @file
 *    @brief
 *    CDlgRotate
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2018-12-4
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2018 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGROTATE_H
#define DLGROTATE_H

#include "tuires.h"
#include "tmani.h"


class CDlgRotate
    : public TM::CTuiWidgetImpl<CDlgRotate, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CDlgRotate>
//     , public TM::CTuiMsgFilter
    , public TM::CAniImpl<CDlgRotate>
{
protected:
    TM::CTuiPicPtr m_ptrPic;
    int m_nAngle;

public:
    CDlgRotate();
    ~CDlgRotate();
    
public:
    enum { IDD = IDC_MANAGE_ROTATE };
    
    BEGIN_TUIMSG_MAP (CDlgRotate)
//     BEGIN_MSG_MAP (CDlgSpyMsg)
        MESSAGE_HANDLER     (WM_INITDIALOG,                     OnInitDialog)
        MESSAGE_HANDLER     (WM_DESTROY,                        OnDestroy)
        MESSAGE_HANDLER     (CM_ZOOMED,                         OnZoomed)
        ROUTE_OID_HANDLER   (IDC_WINDOW_1003, WM_PRINTCLIENT,   OnPrintClient)
        ROUTE_OID_HANDLER   (IDC_FLOAT_1004, WM_PRINTCLIENT,    OnPrintClient)
        COMMAND_ID_HANDLER  (IDOK,                              OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                          OnCancel)
    END_MSG_MAP()
    
    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnZoomed (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk (TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnCancel (TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

    void OnAni(...);

};


#endif // DLGROTATE_H