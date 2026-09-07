/**
 *  @file
 *  @brief
 *  CDlgSvgEdit
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021-10-20
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2021 miragekiller                             
 *  All rights reserved  
 */

#ifndef DLGSVGEDIT_H
#define DLGSVGEDIT_H

#include "tuires.h"
#include "tmtimer.h"

class CDlgSvgEdit
    : public TM::CTuiWidgetImpl<CDlgSvgEdit> //, TM::CTuiWindow>
    , public TM::ITVEvent
    , public TM::CTimerImpl<CDlgSvgEdit>
//     : public TM::CTuiDialogImpl<CDlgSvgEdit>
//     , public TM::CTuiMsgFilter
{
public:
    typedef TM::CVector<TM::CTexture> CTexList;
    TM::CDomDocument m_doc;
    TM::CDomDocument m_docTemp;
    TM::CString m_strFile;
    TM::CTVItem m_tvObj;
    TM::CTVItem m_tvAni;
    TM::CDomElementPtr  m_nodeObj;
    TM::CDomTextPtr     m_nodeAni;
    TM::CImage m_img;
    CTexList m_tex;
    int m_nCur;


public:
    CDlgSvgEdit();
    ~CDlgSvgEdit();

public:
    enum { IDD = IDC_MANAGE_SVG_VIEW };
    
    BEGIN_TUIMSG_MAP (CDlgSvgEdit)
//     BEGIN_MSG_MAP (CDlgSvgEdit)
        MESSAGE_HANDLER     (WM_INITDIALOG,                     OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT,     OnPrintClient)
        ROUTE_OID_HANDLER   (IDC_WINDOW_33040, WM_PRINTCLIENT,  OnPrintClient)

        COMMAND_ID_HANDLER  (IDOK,                              OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                          OnCancel)
    END_MSG_MAP()
    
    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnObjStateChange(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

    // ITVEvent
    virtual void OnStateChanged (TM::HTVITEM hItem, TMUInt uOld, TMUInt uNew) tm_override;

    // CTimerImpl
    void OnTimer(...);

private:
    void InitNode(TM::IDomNode* node, TM::CTVItem itmParent);
    void SyncTreeObj(TM::IDomElement* domParent, TM::CTVItem itmParent);
    void SyncTreeAni();
    void ReloadPic();

public:


};


#endif // DLGSVGEDIT_H