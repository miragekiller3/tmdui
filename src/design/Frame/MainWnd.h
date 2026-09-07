
//-----------------------------------------------------------
//MainWnd.h

//  Copyright (C) 2004 miragekiller                               
//  All rights reserved                                                     
//
//
//  Author: 
//      miragekiller    <3916345933@qq.com>
//
//    Histroy :
//      9/29/2006       create
//-------------------------------------------------------------

#pragma  once 

#include "tuires.h"
#include "resource.h"
#include "ObjectTreeView.h"
#include "GoTo.h"
#include "ProjMgr.h"
#include "DlgSetUser.h"
#include "WndPerformance.h"
#include "WndMemRecorder.h"
#include "WndSelector.h"
#include "WndAcc.h"
#include "WndThumb.h"
#include "tmfilename.h"



#define HOOK_HANDLER_EXP(exp, func) \
    if (uMsg == CM_MSGROUTE && (exp)) \
    { \
        TMBool& bhandle = *(TMBool*)wParam; \
        bhandle = TMTrue; \
        lResult = func(((TM::TUIMSG*)lParam)->message, ((TM::TUIMSG*)lParam)->wParam, ((TM::TUIMSG*)lParam)->lParam, bhandle); \
        if (bhandle) \
        return TMTrue; \
    }

#define HOOK_HANDLER(msg, func) \
    HOOK_HANDLER_EXP (((TM::TUIMSG*)lParam)->message == (msg), func)

#define HOOK_RANGE_HANDLER(msgFirst, msgLast, func) \
    HOOK_HANDLER_EXP (((TM::TUIMSG*)lParam)->message >= (msgFirst) && ((TM::TUIMSG*)lParam)->message <= (msgLast), func)

#define HOOK_RANGE_ID_HANDLER(id, msgFirst, msgLast, func) \
    HOOK_HANDLER_EXP (((TM::TUIMSG*)lParam)->message >= (msgFirst) && ((TM::TUIMSG*)lParam)->message <= (msgLast) && (id) == ((TM::TUIMSG*)lParam)->uID, func)

#define HOOK_ID_HANDLER(id, msg, func) \
    HOOK_HANDLER_EXP (((TM::TUIMSG*)lParam)->message == (msg) && (id) == ((TM::TUIMSG*)lParam)->uID, func)

#define HOOK_OID_HANDLER(oid, msg, func) \
    HOOK_HANDLER_EXP (((TM::TUIMSG*)lParam)->message == (msg) && (oid) == ((TM::TUIMSG*)lParam)->uObjectID, func)

#define HOOK_ID_RANGE_HANDLER(idFirst, idLast, msg, func) \
    HOOK_HANDLER_EXP (((TM::TUIMSG*)lParam)->message == (msg) && ((TM::TUIMSG*)lParam)->uID >= (idFirst) && ((TM::TUIMSG*)lParam)->uID <= (idLast), func)

#define HOOK_TAG_HANDLER(tag, msg, func) \
    HOOK_HANDLER_EXP (((TM::TUIMSG*)lParam)->message == (msg) && (((TM::TUIMSG*)lParam)->uTag) == tag, func)

#define HOOK_TAG_RANGE_HANDLER(TagFirst, TagLast, msg, func) \
    HOOK_HANDLER_EXP (((TM::TUIMSG*)lParam)->message == (msg) && ((TM::TUIMSG*)lParam)->uTag >= (TagFirst) && ((TM::TUIMSG*)lParam)->uTag <= (TagLast), func)

//  TMResult HookHandler(HTUI hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);



#define MAINWNDNAME  _T("TMDui Resource Editor")


struct CCfgDebug
{
    bool            bShowHideWindow;
    bool            bDisableGhost;
    bool            bLogBkBuffer;
    bool            bLogFlip;
    bool            bLogMsg;
    bool            bUIDark;
    bool            bUIHC;
    int             nBk;
    int             nDevGra;
    int             nDevTxt;

    bool            m_bPauseApp;
    bool            m_bEnalbeMemAna;
//    bool            m_bTrackFocus;
    bool            m_bKeepMenuPop;

//    int             m_nTestHC;
//    int             m_nTestColor;

    SERIALIZE_BGN_MAP       (CCfgDebug)
        SERIALIZE_ENTRY     (TM_T("debug"))
        SERIALIZE_VAR       (TM_T("DisableGhost"),  bDisableGhost)
        SERIALIZE_VAR       (TM_T("PauseApp"),      m_bPauseApp)
        SERIALIZE_VAR       (TM_T("EnableMemAna"),  m_bEnalbeMemAna)
        
        SERIALIZE_VAR       (TM_T("LogBkPic"),      bLogBkBuffer)
        SERIALIZE_VAR       (TM_T("LogFlip"),       bLogFlip)
        SERIALIZE_VAR       (TM_T("LogMsg"),        bLogMsg)
        SERIALIZE_LEAVE     ()

        SERIALIZE_ENTRY     (TM_T("device"))
        SERIALIZE_VAR       (TM_T("gra"),           nDevGra)
        SERIALIZE_VAR       (TM_T("txt"),           nDevTxt)
        SERIALIZE_LEAVE     ()

        SERIALIZE_ENTRY     (TM_T("UI Interface"))
        SERIALIZE_VAR       (TM_T("Dark"),          bUIDark)
        SERIALIZE_VAR       (TM_T("High Contrast"), bUIHC)
        SERIALIZE_VAR       (TM_T("bk ground"),     nBk)
        SERIALIZE_LEAVE     ()
    SERIALIZE_END_MAP       ()

    SERIALIZE_BGN_MAP2      (DoSerializeMenu, CCfgDebug)
        SERIALIZE_VAR       (IDM_SHOW_HIDE,         bShowHideWindow)
        SERIALIZE_VAR       (IDM_DISABLEGHOST,      bDisableGhost)
        SERIALIZE_VAR       (IDM_RECORDWIN,         bLogBkBuffer)
        SERIALIZE_VAR       (IDM_RECORDFLIP,        bLogFlip)
        SERIALIZE_VAR       (IDM_RECORDMSG,         bLogMsg)
        SERIALIZE_VAR       (IDM_UI_DARK,           bUIDark)
        SERIALIZE_VAR       (IDM_UI_HIGHCONTRAST,   bUIHC)
    SERIALIZE_END_MAP       ()
        
    CCfgDebug();
    
    void CheckPauseApp();
    void InitMenu(HMENU hMenu);
    void SaveCfg();

    void ExecDark();
    void ExecHC();
    void ExecBk(CMainWnd& wnd);
};

class CMainWnd
   : public TM::CTuiWidgetImpl<CMainWnd, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CMainWnd>
{
public:
    enum {IDD = IDD_TUI_MANAGE_10001};
    friend CGoto;
    friend CWndPanel;
    friend CPropertyDlg;
//     friend class CDlgSpyFocus;
    friend CWndPerformance;
    friend CWndObjInfo;
    friend CWndSelector;
    friend CWndAcc;
    friend CWndThumb;

protected:
    CWndPanel                           m_curPanel;
    CPropertyDlg                        m_dlgProperty;
    EPlugInMode                         m_eMode;
//     TM::CTuiMenuBar     m_wndMB;
    CSetUserDlg                         m_dlgSetUser;
    CWndTVSearch                        m_wndSearchObj;
    TMHMenu                             m_hMenu;
    TM::CFileName                       m_strFullName;
    CGoto                               m_goto;

    CWndPerformance                     m_wndPerformance;
    CWndObjInfo                         m_wndObjInfo;
    CWndMemRecorder                     m_wndMemRecorder;
    CWndSelector                        m_wndSelector;
    CWndAcc                             m_wndAcc;
    CWndThumb                           m_wndThumb;

    TMBool                              m_bIsControlsDlgVisibale;
    TMBool                              m_bIsCtrlPropertyDlgVisibale;

public:
    CCfgDebug                           m_cfgDbg;

public:
    CMainWnd();
    ~CMainWnd();

    SERIALIZE_BGN_MAP       (CMainWnd)
        SERIALIZE_ENTRY     (TM_T("mainwnd"))
        SERIALIZE_VAR       (TM_T("rect"), TM::CSerializeWindowPlacement(GetHWND()))
        SERIALIZE_LEAVE     ()
    SERIALIZE_END_MAP       ()


    BEGIN_TUIMSG_MAP (CMainWnd)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MESSAGE_HANDLER(WM_COMMAND, OnCommand)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
//         MESSAGE_HANDLER(CM_RENDERCHANGED, OnRenderChanged)
        MESSAGE_HANDLER(WM_GETOBJECT, OnGetObject)

        ROUTE_ID_HANDLER(IDC_TREE, WM_CONTEXTMENU, OnContextMenu)
        HOOK_ID_HANDLER(IDC_TREE, WM_KEYDOWN, OnKeyDown)

        COMMAND_ID_HANDLER(IDB_SHOWPROP, OnShowProp)
        COMMAND_ID_HANDLER(IDB_SHOWTREE, OnShowTree)

        COMMAND_HANDLER(IDC_SELBK, CBN_SELENDOK, OnSelEndOK)
        COMMAND_HANDLER(IDC_COMBOBUTTON_SPY, CBN_DROPDOWN, OnSpyDropDown)
        
        COMMAND_ID_HANDLER  (IDCANCEL,            OnCancel)
   END_MSG_MAP ()
    
protected:
    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//    TMResult OnClose(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnKeyDown(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnSize(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnResChanged(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnCommand(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnRenderChanged (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnGetObject (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    
//     TMResult OnCloseDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnCopy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPaste(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnEnChange(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnEditClear(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnShowProp(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnShowTree(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnSelEndOK(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnSpyDropDown(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    
    TMResult OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    TMResult OnContextMenu(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);

public:
    TM::CString GetUser() const { return m_dlgSetUser.GetUser(); }
    void AddGoto(TMUInt64 nID) { m_goto.Add(nID); }

    void SetWndTitle();

    const TM::CFileName& GetRCName() { return m_strFullName; }
//     TMBool IsParentSelected(TM::CTVItem ti); // if parent have selected state then return true; else return 0

    void SyncTxtEdit(TMBool bSyncText);
//     TMResult OnChangeCurCtrl(TM::EObjType eType/* TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled */);

protected:
    VOID ShowControlsDlg(TMBool bShow);
    VOID ShowCtrlPropertyDlg(TMBool bShow);
    //---------------------------------------------------
    TMBool SaveResource(ESaveMode eMode, int nCmd);
    void OpenDlgXml(TMBool bMerge);
    void OpenDlgLang(TMBool bTest);
    void SaveDlgSkin(TMBool bLang);
    void TestCurEditWindow();
//     TMBool SaveFile(TMCStr szFile);
    void SetMenuBarState(TMBool bLoadLang);

private:
//     void    OpenFile(TMCStr strFile,TMCStr strFileName);
    
//     void CreateFactory(TMWParam wParam, TMLParam lParam);
    TMBool    LoadFile(const TM::CString& strXML, TMBool bMerge);
    
//     void    SelectLanguage(TMHMenu hMenu,TMUInt uIdItem);
//     TMBool  AddNewLanguage();
//     void    BindFile();
//     void    UnbindFile();

    
public:
    CWndPanel& GetPanel() { return m_curPanel; }
    CWndAcc& GetWndAcc() { return  m_wndAcc; }
    CPropertyDlg& GetProperty() { return m_dlgProperty; }
    IProj* GetProj();
    void SetMode(EPlugInMode eMode);
    EPlugInMode GetMode() { return m_eMode; }
    void Init();
};


