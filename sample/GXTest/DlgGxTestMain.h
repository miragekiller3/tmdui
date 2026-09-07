/**
 *  @file
 *  @brief
 *  CDlgGxTestMain
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2022-6-28
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2022 miragekiller                             
 *  All rights reserved  
 */

#ifndef DLGGXTESTMAIN_H
#define DLGGXTESTMAIN_H

#include "tuires.h"
#include "Resource.h"


class CDlgGxTestMain
//    : public TM::CTuiWidgetImpl<CDlgGxTestMain> //, TM::CTuiWindow>
//     : public CDialogImpl<CDlgGxTestMain>
    : public CDialogImpl<CDlgGxTestMain>
    , public TM::CTuiMsgFilter
    , public IGXAppNotify
{
private:
    WTL::CEdit m_edit;
    TM::CTuiPtr<class CDlgGxTest>        m_ptrShade;
    TM::CTuiPtr<class CDlgGxTestBlt>     m_ptrBlt;
    TM::CTuiPtr<class CDlgGxTestBltDir>  m_ptrBltDir;
    
public:
    CDlgGxTestMain();
    ~CDlgGxTestMain();
    
public:
//     enum { IDD = IDC_MANAGE_GX_TEST_MAIN };
    enum { IDD = IDD_DIALOG1 };
    
    BEGIN_MSG_MAP (CDlgGxTestMain)
//     BEGIN_MSG_MAP (CDlgGxTestMain)
        MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
        MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT,     OnPrintClient)
//         MESSAGE_HANDLER     (CM_RENDERCHANGED,          OnRenderChanged)

//         COMMAND_HANDLER     (IDC_DEV_GDI,   BN_CLICKED, OnDevice)
//         COMMAND_HANDLER     (IDC_DEV_D3D9,  BN_CLICKED, OnDevice)
//         COMMAND_HANDLER     (IDC_DEV_D3D11, BN_CLICKED, OnDevice)
//         COMMAND_HANDLER     (IDC_DEV_GL,    BN_CLICKED, OnDevice)
        COMMAND_HANDLER     (IDC_RADIO1,    BN_CLICKED, OnDevice)
        COMMAND_HANDLER     (IDC_RADIO2,    BN_CLICKED, OnDevice)
        COMMAND_HANDLER     (IDC_RADIO3,    BN_CLICKED, OnDevice)
        COMMAND_HANDLER     (IDC_RADIO4,    BN_CLICKED, OnDevice)

//         COMMAND_HANDLER     (IDC_PUSHBUTTON_1071,    BN_CLICKED, OnTest)
//         COMMAND_HANDLER     (IDC_PUSHBUTTON_1072,    BN_CLICKED, OnTest)
        COMMAND_HANDLER     (IDC_BUTTON1,   BN_CLICKED, OnTest)
        COMMAND_HANDLER     (IDC_BUTTON2,   BN_CLICKED, OnTest)
        COMMAND_HANDLER     (IDC_BUTTON3,   BN_CLICKED, OnTest)

        COMMAND_HANDLER     (IDC_CHECK1,    BN_CLICKED, OnLayered)
        
        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
    END_MSG_MAP()
    
    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnRenderChanged(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);


//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnDevice(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnTest(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnLayered(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);

    //CTuiMsgFilter
    virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult) tm_override; 

    //IGXAppNotify
    virtual void OnGXDeviceChanged(IGXApp* pNew) tm_override; // pNew == 0 means termed

public:
    void Append(LPCWSTR str)
    {
// //         TM::CTuiMemo memo(Item(IDC_MEMO_1092));
// //         if (memo)
// //         {
//             m_edit.AppendText(str);
//             m_edit.AppendText(L"\r\n");
// //         }
    }

};


#endif // DLGGXTESTMAIN_H