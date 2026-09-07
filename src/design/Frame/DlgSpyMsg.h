/**
 *    @file
 *    @brief
 *    CDlgSpyMsg
 *
 *  @author miragekiller <3916345933@qq.com>
 *    @date    2017-11-12
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2017 miragekiller                               
 *    All rights reserved  
 */

#ifndef DLGSPYMSG_H
#define DLGSPYMSG_H

#include "tuires.h"

#include "tmthread.h"
#include "tmhook.h"
#include "TraceMT.h"
#include "../design.h"

class CWndPanel;
struct CSpyMsg;


struct CHookEventBase
{
    void _SetHook(DWORD dwFlags);
    void RemoveHook();
};

struct CHookEventSelf : public CHookEventBase
{
    void SetHook() { _SetHook(WINEVENT_INCONTEXT); }
};

struct CHookEventGlobal: public CHookEventBase
{
    void SetHook() { _SetHook(WINEVENT_OUTOFCONTEXT); }
};


#define WM_CHECK_REMOTE     WM_USER + 203    // return:HWND_SELF/HWND_SEND/HWND_RECIVE
#define WM_REMOTE_MESSAGE   WM_USER + 204    // lp:REMOTE_MESSAGE*
#define WM_FLUSH            WM_USER + 205    //

const HWND HWND_SELF = 0;
const HWND HWND_SEND = (HWND)1;
const HWND HWND_RECIVE = (HWND)2;

struct REMOTE_MESSAGE
{
    TMBool      bUsed;
    TMUInt16    type;
    int         ind;
    wchar_t     strText[20480];
    wchar_t     strHint[20480];
};

class CProcessMemorys
{
public:
    enum { eCacheCount = 1024 };
    HANDLE m_hProcess;
    int m_nCur;

    REMOTE_MESSAGE* FindMem();

    CProcessMemorys() : m_hProcess(0) {}
    void Set(HWND hWnd);
    TMBool Send(HWND hWnd, TMUInt16 type, const TM::CString& pText, const TM::CString& strHint, int ind);
    void Recive(class CDlgSpyMsg* dlg, TMLParam lp);
    TMBool CanRecive() { return true; }
};

class CDlgSpyMsg
    : public TM::CTuiDialogImpl<CDlgSpyMsg> //, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CDlgSpyMsg>
    , public TM::CTuiMsgFilter
   , public ITraceHook

    , public TM::CHookCallWndImpl<CDlgSpyMsg>
    , public TM::CHookCallWndRetImpl<CDlgSpyMsg>
    , public TM::CHookCbtImpl<CDlgSpyMsg>
    , public TM::CHookForegroundIdleImpl<CDlgSpyMsg>
    , public TM::CHookGetMessageImpl<CDlgSpyMsg>
    , public TM::CHookKeyboardImpl<CDlgSpyMsg>
    , public TM::CHookKeyboardLLImpl<CDlgSpyMsg>
    , public TM::CHookMouseImpl<CDlgSpyMsg>
    , public TM::CHookMouseLLImpl<CDlgSpyMsg>
    , public TM::CHookMsgFilterImpl<CDlgSpyMsg>
    , public TM::CHookShellImpl<CDlgSpyMsg>
    , public TM::CHookSysMsgFilterImpl<CDlgSpyMsg>
    , public TM::CHookShellUser32Impl<CDlgSpyMsg>
    , public CHookEventSelf
    , public CHookEventGlobal
{
public:
    struct CACHEITEM { int nInd; TM::CString str; TM::CString hint; };
    typedef TM::CSet<TM::CString>                           CStringList;
    typedef TM::CList<CACHEITEM>                            CStringCache;
    typedef TM::CAutoLockT<TM::CCritical>                   CAutoLock;

    enum EType
    {
        eTypeWin = CSpyMsg::eWnd,
        eTypeDgt = CSpyMsg::eWidget,
        eTypeDBWin = CSpyMsg::eLog,
        eTypeFunction = CSpyMsg::eFunc,
        eTypeHookEvent = CSpyMsg::eHookEvent,
        eTypeInvalidate = CSpyMsg::eEnd,
        eType_CALLWNDPROC,
        eType_CALLWNDPROCRET,
        eType_CBT,
        eType_DEBUG,
        eType_FOREGROUNDIDLE,
        eType_GETMESSAGE,
        eType_JOURNALPLAYBACK,
        eType_JOURNALRECORD,
        eType_KEYBOARD,
        eType_KEYBOARD_LL,
        eType_MOUSE,
        eType_MOUSE_LL,
        eType_MSGFILTER,
        eType_SHELL,
        eType_SYSMSGFILTER,
        eType_HookShellUser32,
        eType_HookEventSelf,
        eType_HookEventGlobal,
        eTypeLast,
        eTypeWarn,
        eTypeUnknown = 9999,
    };


    CWndPanel*      m_wndPanel;
    
    TM::CTVItem     m_itmRoot;
    TMBool          m_bAutoScroll;
    int             m_nCurLineID;
    int             m_tid;
    int             m_nInd;
    TM::CCritical   m_lock;

    // break;
    CStringList     m_aBreak;
    TMBool          m_bBreaked;
    TMBool          m_bTypeBlcok;

    TMBool          m_bShowLog[eTypeLast];
    TMBool          m_bPasue;
    CStringList     m_aFilter[2]; // 0: include; 1:exclude
    TM::CString     m_strSel;
    TM::CString     m_strSelID;
    TM::CTVItem     m_selParent;
    CStringCache    m_cache;

    HWND            m_hRemote;
    CProcessMemorys m_remote;

    static CPtr     s_pMSg;

public:
    CDlgSpyMsg();
    ~CDlgSpyMsg();
    
public:
    enum { IDD = IDD_TUI_SPY_MSG };
    
    BEGIN_MSG_MAP (CDlgSpyMsg)
//     BEGIN_MSG_MAP (CDlgSpyMsg)
        MESSAGE_HANDLER     (WM_INITDIALOG,                     OnInitDialog)
        MESSAGE_HANDLER     (WM_DESTROY,                        OnDestroy)
        MESSAGE_HANDLER     (WM_HOTKEY,                         OnHotKey)
        MESSAGE_HANDLER     (WM_GETOBJECT,                      OnGetObject)
        
        MESSAGE_HANDLER     (WM_CHECK_REMOTE,                   OnCheckRemote)
        MESSAGE_HANDLER     (WM_REMOTE_MESSAGE,                 OnRemoteMessage)
        MESSAGE_HANDLER     (WM_FLUSH,                          OnFlush)
        
        
        ROUTE_OID_HANDLER   (IDC_TREE_31124,    CM_DRAWITEM,    OnDrawItem)
        ROUTE_OID_HANDLER   (IDC_WINDOW_31156,  CM_ENDMENUPOP,  OnEndMenuPop)

        COMMAND_ID_HANDLER  (IDC_CHECKBOX_31127,                OnLogMsg)
        COMMAND_ID_HANDLER  (IDC_CHECKBOX_31134,                OnLogMsg)
        COMMAND_ID_HANDLER  (IDC_CHECKBOX_31241,                OnLogMsg)
        COMMAND_ID_HANDLER  (IDC_CHECKBOX_31247,                OnLogMsg)
        COMMAND_RANGE_HANDLER  (IDC_CHECKBOX_31159, IDC_CHECKBOX_31174,        OnLogMsg)
        COMMAND_ID_HANDLER  (IDC_CHECKBOX_31256,                OnLogMsg)
        COMMAND_ID_HANDLER  (IDC_CHECKBOX_33093,                OnLogMsg)
        COMMAND_ID_HANDLER  (IDC_CHECKBOX_30435,                OnLogMsg)
        COMMAND_ID_HANDLER  (IDC_CHECKBOX_30420,                OnLogPaster)
        

        COMMAND_HANDLER     (IDC_EDIT_SPY_INC, EN_UPDATE,       OnLogFilter)
        COMMAND_HANDLER     (IDC_EDIT_SPY_EXC, EN_UPDATE,       OnLogFilter)
        COMMAND_HANDLER     (IDC_EDIT_BREAK,   EN_UPDATE,       OnEditBreak)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_REMOTE,             OnRemote)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_31133,              OnClear)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_31246,              OnDelete)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_PAUSE,              OnPause)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_COPY,               OnCopy)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_OTHER,              OnOther)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_PAUSE_BREAK,        OnPauseBreak)
        COMMAND_ID_HANDLER  (IDC_CHECKBOX_TYPE_BLOCK,           OnTypeBlock)

        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_30272,              OnBreak)

        COMMAND_CODE_HANDLER(TVN_SELCHANGE,                     OnSelChange)

        COMMAND_ID_HANDLER  (IDCANCEL,                          OnCancel)
        
        CHAIN_MSG_MAP(CHookShellUser32)
    END_MSG_MAP()
    
    TMResult OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnHotKey (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnGetObject (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { return 0; }
    
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);

    TMResult OnDrawItem(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnEndMenuPop(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);

    TMResult OnLogMsg (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnLogPaster (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    
//     TMResult OnLogDBWin (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnLogFilter (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnEditBreak (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

//     TMResult OnStopCatch (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCheckRemote (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnRemoteMessage (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnRemote (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnFlush(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

    TMResult OnClear (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnDelete (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnPause (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCopy (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnOther (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnBreak (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnPauseBreak (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnTypeBlock (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    TMResult OnSelChange (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

//     TMResult OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);

    void AddSpyID(IObjEdit* pSel);

    // spy message
    void OnSpyDone(EType eType, LPCTSTR pStrF, HWND hWnd, TM::TUIMSG* pMsg, TMResult* lr, int& step, const double* cycle);
    void OnSpyDone(CSpyMsg& sm, CSpyMsg::EMode eMode, CSpyMsg::EType eType, int tid);

    // ITraceHook, CTraceMT;
    virtual void OnTrace(int tp, LPCTSTR pStr) tm_final;

    // hook
    TMBool OnCallWndProc(CWPSTRUCT* msg); // CHookCallWnd
    TMBool OnCallWndRetProc(CWPRETSTRUCT* msg); // CHookCallWndRet
    TMBool OnCbt(int nCode, WPARAM wParam, LPARAM lParam); // CHookCbt
    TMBool OnForegroundIdle(); // CHookForegroundIdle
    TMBool OnGetMessage(TMBool bRemove, MSG* msg); // CHookGetMessage
    TMBool OnKeyboard(UINT uVirtKey, UINT uFlags); // CHookKeyboard
    TMBool OnKeyboardLL(UINT uMsg, KBDLLHOOKSTRUCT* ps); // CHookKeyboardLL
    TMBool OnMouse(UINT uMsg, MOUSEHOOKSTRUCT* pm); // CHookMouse
    TMBool OnMouseLL(UINT uMsg, MSLLHOOKSTRUCT* pm); // CHookMouseLL
    TMBool OnMsgFilter(int nEvent, MSG* msg); // CHookMsgFilter
    TMBool OnShell(int nCode, WPARAM wParam, LPARAM lParam); // CHookShell
    TMBool OnSysMsgFilter(int nEvent, MSG* msg); // CHookSysMsgFilter
    void OnShellUser32(int nCode, WPARAM wParam); // CHookShellUser32

    void OnEvent(DWORD event, HWND hwndMsg, LONG idObject, LONG idChild, DWORD idThread, DWORD dwmsEventTime); // CHookEvent

    TMBool Filter(const TM::CString& str, int n);
    void CheckTVItem(TM::CTVItem itm);
    bool CheckBreak(TM::CString& str); // return: is blcok;
    CStringList::recorder IsBreak(const TM::CString& str);
    void FormatAndAppendText(BOOL bIn, EType type, LPCTSTR strFmt, HWND hWnd,  const TM::CString& pText, const TM::CString& strHint = TM::CString());
    bool AppendText(BOOL bIn, EType type, const TM::CString& pText, const TM::CString& strHint = TM::CString(), int tid = 0); // return: is blcok;
    TM::CTVItem _NativeAppendText(CACHEITEM& cache);
    bool NativeAppendCache(const TM::CString& text, const TM::CString& strHint); // return: is blcok;
    void SendSysText(LPCWSTR str);
    TMBool AppendRemoteText(REMOTE_MESSAGE* msg);
};


#endif // DLGSPYMSG_H