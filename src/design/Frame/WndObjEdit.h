/**
 *    @file
 *    @brief
 *    CWndPicEdit
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2019-6-21
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2019 miragekiller                               
 *    All rights reserved  
 */

#ifndef WNDOBJEDIT_H
#define WNDOBJEDIT_H

#include "tuires.h"
#include "tmani.h"

class CWndPanel;

template <class T>
class CObjEditWindowImpl : public TM::CTuiWidgetImpl<T>
{
public:
    CWndPanel* m_pPanel;

public:
    CObjEditWindowImpl* Init(CWndPanel* p) { m_pPanel = p; return this; }

};

///////////////////////////////////////////////////////////////////////////////
// CWndPicEdit
class CWndPicEdit
    : public CObjEditWindowImpl<CWndPicEdit> //, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CWndPicEdit>
//     , public TM::CTuiMsgFilter
    , public TM::CAniImpl<CWndPicEdit>
//     , public TM::CEnableSharedFromThis<CWndPicEdit>
{
private:
    int m_nCurrentFrame;

public:
    CWndPicEdit();
    ~CWndPicEdit();
    
public:
    enum { IDD = IDD_EDIT_PIC };
    
    BEGIN_TUIMSG_MAP (CWndPicEdit)
//     BEGIN_MSG_MAP (CWndPicEdit)
//         MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
        ROUTE_ID_HANDLER (ID_PIC_EDIT, WM_PRINTCLIENT, OnPrintEdit)
        ROUTE_ID_HANDLER (ID_PIC_TEST, WM_PRINTCLIENT, OnPrintTest)
        
        COMMAND_HANDLER(IDC_PUSHBUTTON_31242,  BN_CLICKED,      OnStep)
        COMMAND_HANDLER(IDC_PUSHBUTTON_31243,  BN_CLICKED,      OnStep)
        COMMAND_HANDLER(IDC_PUSHBUTTON_30342,  BN_CLICKED,      OnStep)
        COMMAND_HANDLER(IDC_PUSHBUTTON_30343,  BN_CLICKED,      OnStep)
        COMMAND_HANDLER(IDC_PUSHBUTTON_30339,  BN_CLICKED,      OnPlay)
        COMMAND_HANDLER(IDC_PUSHBUTTON_33021,  BN_CLICKED,      OnEdit)

//         COMMAND_ID_HANDLER  (IDOK,                      OnOk)
//         COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
    END_MSG_MAP()
    
//     TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
//     TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    TMResult OnPrintEdit(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnPrintTest(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnPrintText(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);

    TMResult OnStep (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnPlay (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnEdit (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

    // TM::CAni
    void OnAni(TMHAni tm, ...) { Invalidate(); }
};


///////////////////////////////////////////////////////////////////////////////
// CWndFrmEdit
class CWndFrmEdit
    : public CObjEditWindowImpl<CWndFrmEdit> //, TM::CTuiWindow>
//     , public TM::CEnableSharedFromThis<CWndFrmEdit>
{
public:
    CWndFrmEdit() {}
    ~CWndFrmEdit() {}
    
public:
    enum { IDD = IDD_TUI_FRAME_EDIT };
    
    BEGIN_TUIMSG_MAP (CWndFrmEdit)
//     BEGIN_MSG_MAP (CWndFrmEdit)
//         MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT       OnPrintClient)
        MESSAGE_HANDLER     (WM_PRINTCLIENT,            OnPrintClient)
        
//         COMMAND_ID_HANDLER  (IDOK,                      OnOk)
//         COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
    END_MSG_MAP()
    
    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnPrintClient (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    
    
    //CTuiMsgFilter
    //     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);
};


///////////////////////////////////////////////////////////////////////////////
// CWndWinEdit
class CWndWinEdit
    : public CObjEditWindowImpl<CWndWinEdit> //, TM::CTuiWindow>
//     , public TM::CEnableSharedFromThis<CWndWinEdit>
{
public:
    TM::CTuiPicPtr  m_rpic;
//     CObjData*       m_pCurObj;

public:
    CWndWinEdit() {}
    ~CWndWinEdit() {}

    CWndWinEdit* Init(CWndPanel* p);
    
public:
    enum { IDD = IDC_EDIT_WCL };

    BEGIN_TUIMSG_MAP    (CWndWinEdit)
//         MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MBUTTONDBLCLK, OnMouse)

//         MESSAGE_HANDLER (WM_CREATE,         OnCreate)
//         MESSAGE_HANDLER (WM_SETCURSOR,      OnSetCursor)
        MESSAGE_HANDLER (WM_MOUSEMOVE,      OnMouseMove)
        MESSAGE_HANDLER (WM_LBUTTONDOWN,    OnLButtonDown)
        MESSAGE_HANDLER (WM_LBUTTONUP,      OnLButtonUp)
        MESSAGE_HANDLER (WM_KEYDOWN,        OnKeyDown)
//         MESSAGE_HANDLER (WM_CAPTURECHANGED, OnCaptureChanged)
        MESSAGE_HANDLER (WM_PRINTCLIENT,    OnPrintClient)

        MESSAGE_HANDLER (CM_DRAGENTRY,      OnDragEntry)
        MESSAGE_HANDLER (CM_DRAGOVER,       OnDragEntry)
        MESSAGE_HANDLER (CM_DRAGLEAVE,      OnDragLeave)
        MESSAGE_HANDLER (CM_DRAGDROP,       OnDragDrop)


        

//        MESSAGE_HANDLER (WM_CREATE, OnCreate)
//         MESSAGE_HANDLER (WM_SIZE, OnSize)
//         MESSAGE_HANDLER (WM_MOVE, OnMove)
//         MESSAGE_HANDLER (uMsg, OnGeneralMessage)
//         MESSAGE_HANDLER (CM_ZOOMED, OnSize)
    END_TUIMSG_MAP      ()
    
//     TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnCreate (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnKeyDown (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnCaptureChanged (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnSetCursor (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnMouseMove (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnLButtonDown (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnLButtonUp (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

    TMResult OnDragLeave (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDragEntry (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDragDrop (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

    TMResult OnPrintClient (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnMouse (TMUInt uMsg, TMWParam wParam, TMLParam& lParam, TMBool& bHandled);

//     void OnPrint();
    void DoSelect();
    TM::CTuiPoint ToPos(const TM::CTuiPoint& lp);
//     void CheckDelSelMgr(CObjData* p);
//     void SwitchSelMgr(IObjEdit* p);


    
    //CTuiMsgFilter
    //     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);
};


#endif // WNDFRAMEEDIT_H