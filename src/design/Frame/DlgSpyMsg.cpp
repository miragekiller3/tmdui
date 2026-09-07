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

#include "stdafx.h"
#include "DlgSpyMsg.h"
#include "ProjMgr.h"
#include "tmformatacc.h"
#include "tmclipboard.h"
#include "tmformatmsg.h"
#include "../design.h"
#include "DlgSpyMsg_Event.h"

using namespace TM;

static LPCTSTR const strFmt_CALLWNDPROC       = L"(%p)WH_CALLWNDPROC ";
static LPCTSTR const strFmt_CALLWNDPROCRET    = L"(%p)WH_CALLWNDPROCRET ";
static LPCTSTR const strFmt_CBT               = L"(%p)WH_CBT  ";
static LPCTSTR const strFmt_FOREGROUNDIDLE    = L"(%p)WH_FOREGROUNDIDLE ";
static LPCTSTR const strFmt_GETMESSAGE        = L"(%p)WH_GETMESSAGE ";
static LPCTSTR const strFmt_KEYBOARD          = L"(%p)WH_KEYBOARD ";
static LPCTSTR const strFmt_KEYBOARD_LL       = L"(%p)WH_KEYBOARD_LL ";
static LPCTSTR const strFmt_MOUSE             = L"(%p)WH_MOUSE ";
static LPCTSTR const strFmt_MOUSE_LL          = L"(%p)WH_MOUSE_LL ";
static LPCTSTR const strFmt_MSGFILTER         = L"(%p)WH_MSGFILTER ";
static LPCTSTR const strFmt_SHELL             = L"(%p)WH_SHELL ";
static LPCTSTR const strFmt_SYSMSGFILTER      = L"(%p)WH_SYSMSGFILTER ";
static LPCTSTR const strFmtHookShellUser32    = L"(%p)HookShellUser32 ";
static LPCTSTR const strFmtHookEvent          = L"(%p)HookEvent ";


const int nPosHandle = 10;
const int nPosID = 0;
const int nLenID = 7;

#define PRECHAR L'.'


HTUI   g_hInvalidate = 0;
static bool s_bEntry = false;

void SpyMessage(HWND hWnd, TUIMSG* pMsg, TMResult* lr, int& step, const double* cycle)
{
    if (s_bEntry)
        return;
    if (CDlgSpyMsg::s_pMSg)
        CDlgSpyMsg::s_pMSg->OnSpyDone(CDlgSpyMsg::eTypeWin, 0, hWnd, pMsg, lr, step, cycle);
}

void SpyFunction(LPCTSTR pStr, TUIMSG* pMsg, TMResult* lr, int& step, double cycle)
{
    if (CDlgSpyMsg::s_pMSg)
        CDlgSpyMsg::s_pMSg->OnSpyDone(CDlgSpyMsg::eTypeFunction, pStr, 0, pMsg, lr, step, &cycle);
}

void SpyMsgFunc(CSpyMsg& sm, CSpyMsg::EMode eMode, CSpyMsg::EType eType, int tid)
{
    if (CDlgSpyMsg::s_pMSg)
        CDlgSpyMsg::s_pMSg->OnSpyDone(sm, eMode, eType, tid);
}


TM_BGN_NAMESPACE

TMBool g_bInSpyMessageOrFunc = false;

void Edit_SpyInvalidate(HWND hWnd, HTUI h, int nObjID, const CString& cname, const TMRect* rc, const TMRectF* rcDirty)
{
    if (!CDlgSpyMsg::s_pMSg)
        return;

    CString str2;
    TUIMSG msg = {0};
    msg.hWnd = h;
    msg.uObjectID = nObjID;
    
    if (hWnd) // invalidate
    {
        RECT rc2;
        DbgVerify(::GetUpdateRect(hWnd, &rc2, false));

#       define FMT1 L"Invalidate(class:%s, visible:%d, rcAdd:" TM_L(TM_FMT_RCI_STR) L", rcResult:" TM_L(TM_FMT_RCI_STR)
#       define FMT2 L"Invalidate(class:%s, visible:%d, rcAdd:all, rcResult:" TM_L(TM_FMT_RCI_STR)
        TMBool bVisible = ::IsWindowVisible(hWnd);
        if (rc)
            str2.Format(FMT1, cname.c_str(), bVisible, TM_FMT_RCI_VAL(*rc), TM_FMT_RCI_VAL(rc2));
        else
            str2.Format(FMT2, cname.c_str(), bVisible, TM_FMT_RCI_VAL(rc2));
    }
    else // update dirty
    {
        DbgAssert(rc || rcDirty);
#       define FMT3 L"MakeUpdateRect(class:%s, rc:" TM_L(TM_FMT_RCI_STR)
#       define FMT4 L"MakeDirtyRect(class:%s, rc:" TM_L(TM_FMT_RCF_STR)
        if (rc)
            str2.Format(FMT3, cname.c_str(), TM_FMT_RCI_VAL(*rc));
        else
            str2.Format(FMT4, cname.c_str(), TM_FMT_RCF_VAL(*rcDirty));
    }

    int step;
    CDlgSpyMsg::s_pMSg->OnSpyDone(CDlgSpyMsg::eTypeInvalidate, str2, 0, &msg, 0, step, 0);
}

TM_END_NAMESPACE


static HWINEVENTHOOK s_hook;

static void CALLBACK NotifyProc(HWINEVENTHOOK hEvent, DWORD event, HWND hwndMsg, LONG idObject,
                                LONG idChild, DWORD idThread, DWORD dwmsEventTime) 
{
    if (CDlgSpyMsg::s_pMSg && (event != EVENT_OBJECT_LOCATIONCHANGE && idObject != OBJID_CARET))
        CDlgSpyMsg::s_pMSg->OnEvent(event, hwndMsg, idObject, idChild, idThread, dwmsEventTime);
}

void CHookEventBase::_SetHook(DWORD dwFlags)
{
    DbgAssert(!s_hook);
    s_hook = SetWinEventHook(EVENT_MIN, EVENT_MAX, _Module.GetModuleInstance(), NotifyProc, 0, 0, dwFlags);
    if (dwFlags != WINEVENT_INCONTEXT)
        CTuiButton(CDlgSpyMsg::s_pMSg->Item(IDC_CHECKBOX_31256)).SetCheck(0);
    else
        CTuiButton(CDlgSpyMsg::s_pMSg->Item(IDC_CHECKBOX_33093)).SetCheck(0);
}

void CHookEventBase::RemoveHook()
{
    UnhookWinEvent(s_hook);
    s_hook = 0;
}


CDlgSpyMsg::CPtr CDlgSpyMsg::s_pMSg;

CDlgSpyMsg::CDlgSpyMsg()
    : m_bPasue      (false)
    , m_bBreaked    (false)
    , m_bTypeBlcok  (false)
    , m_bAutoScroll (true)
    , m_hRemote     (0)
    , m_tid         (ThreadSelfId())
    , m_nInd        (0)
{
}

CDlgSpyMsg::~CDlgSpyMsg()
{
}

TMResult CDlgSpyMsg::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    OnClear(0,0,0,bHandled);
    OnLogMsg(0,0,0,bHandled);
//    _tuiApp.AddMessageFilter(this);
    _dbgApp.TraceAddHook(this);

    RegisterHotKey(m_hWnd, 0Xa001, 0, VK_PAUSE);
    ModifyStyleEx(0, WS_EX_APPWINDOW);
    return 0;
}

TMResult CDlgSpyMsg::OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
    _dbgApp.TraceDelHook(this);

    CHookCallWnd::RemoveHook();
    CHookCallWndRet::RemoveHook();
    CHookCbt::RemoveHook();
    CHookForegroundIdle::RemoveHook();
    CHookGetMessage::RemoveHook();
    CHookKeyboard::RemoveHook();
    CHookKeyboardLL::RemoveHook();
    CHookMouse::RemoveHook();
    CHookMouseLL::RemoveHook();
    CHookMsgFilter::RemoveHook();
    CHookShell::RemoveHook();
    CHookSysMsgFilter::RemoveHook();
    CHookShellUser32::RemoveHook();

    CHookEventSelf::RemoveHook();
    CHookEventGlobal::RemoveHook();

    m_remote.Set(0); 

    m_wndPanel->GetProj()->EnableSpyMessage(CSpyMsg::eWnd, false);
    m_wndPanel->GetProj()->EnableSpyMessage(CSpyMsg::eWidget, false);
    m_wndPanel->GetProj()->EnableSpyMessage(CSpyMsg::eLog, false);
    m_wndPanel->GetProj()->EnableSpyMessage(CSpyMsg::eFunc, false);

    //  _tuiApp.RemoveMessageFilter(this);
    return 0;
}

TMResult CDlgSpyMsg::OnHotKey (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (HIWORD(lParam) == VK_PAUSE)
        CTuiButton(Item(IDC_PUSHBUTTON_PAUSE_BREAK)).Click();
    else
        bHandled = false;
    return 0;
}

TMResult CDlgSpyMsg::OnDrawItem(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    TUIDRAWITEM* p = (TUIDRAWITEM*)pMsg->lParam;
    IGXApp* gapp = p->gxApp;
    if (p->dwDrawStage == CDDS_ITEMPREPAINT)
    {
        CGXStrView strTxt(CTextHandle(*p->gxText).GetString());
        strTxt = strTxt.TrimLeft(PRECHAR);
        if (!m_strSelID.IsEmpty() && strTxt.Find(m_strSelID) == nPosID)
            gapp->FillRect(p->rcItem, TMRGB(128,255,128));
        else if (IsBreak(strTxt))
            gapp->FillRect(p->rcItem, TMRGB(255,128,0));

        if (p->hItem == m_selParent)
        {
            CTuiRect rc(p->rcItem);
            rc.SetWidth(10);
            gapp->FillRect(rc, TMRGB(0,255,128));
        }
    }
    CTuiWidget::DefRouteProc(pMsg);
    return 0;
}

TMResult CDlgSpyMsg::OnEndMenuPop(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    CTuiWidget(pMsg->hWnd).ShowWindow(SW_HIDE);
    return 0;
}

static void _CDlgSpyMsg_CheckFlag(CDlgSpyMsg& dlg, int oid, CSpyMsg::EType eSpy)
{
    BOOL b = CTuiButton(dlg.Item(oid)).GetCheck();
    if (b == dlg.m_bShowLog[eSpy])
        return;
    dlg.m_bShowLog[eSpy] = b;
    dlg.m_wndPanel->GetProj()->EnableSpyMessage(eSpy, b);
}

TMResult CDlgSpyMsg::OnLogMsg (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    _CDlgSpyMsg_CheckFlag(*this, IDC_CHECKBOX_31127, CSpyMsg::eWnd);
    _CDlgSpyMsg_CheckFlag(*this, IDC_CHECKBOX_31128, CSpyMsg::eWidget);
    _CDlgSpyMsg_CheckFlag(*this, IDC_CHECKBOX_31134, CSpyMsg::eLog);
    _CDlgSpyMsg_CheckFlag(*this, IDC_CHECKBOX_31241, CSpyMsg::eFunc);
    m_bShowLog[eTypeFunction] = CTuiButton(Item(IDC_CHECKBOX_31241)).GetCheck();
    m_bShowLog[eTypeInvalidate] = CTuiButton(Item(IDC_CHECKBOX_31247)).GetCheck();
    m_bShowLog[eTypeHookEvent] = true;


#define SPY_CHECK_HOOK(oid,cls,type) \
    if (oid == wID) \
    { \
        m_bShowLog[type] = CTuiButton(Item(oid)).GetCheck(); \
        cls::RemoveHook(); \
        if (m_bShowLog[type]) cls::SetHook(); \
    }

    SPY_CHECK_HOOK(IDC_CHECKBOX_31159, CHookCallWnd,        eType_CALLWNDPROC);
    SPY_CHECK_HOOK(IDC_CHECKBOX_31160, CHookCallWndRet,     eType_CALLWNDPROCRET);
    SPY_CHECK_HOOK(IDC_CHECKBOX_31161, CHookCbt,            eType_CBT);
//    SPY_CHECK_HOOK(IDC_CHECKBOX_31162, CHookCbt         eType_DEBUG);
    SPY_CHECK_HOOK(IDC_CHECKBOX_31163, CHookForegroundIdle, eType_FOREGROUNDIDLE);
    SPY_CHECK_HOOK(IDC_CHECKBOX_31164, CHookGetMessage,     eType_GETMESSAGE);
//     SPY_CHECK_HOOK(IDC_CHECKBOX_31165, CHookGetMessage,     eType_JOURNALPLAYBACK);
//     SPY_CHECK_HOOK(IDC_CHECKBOX_31166, CHookGetMessage,     eType_JOURNALRECORD);
    SPY_CHECK_HOOK(IDC_CHECKBOX_31167, CHookKeyboard,       eType_KEYBOARD);
    SPY_CHECK_HOOK(IDC_CHECKBOX_31168, CHookKeyboardLL,     eType_KEYBOARD_LL);
    SPY_CHECK_HOOK(IDC_CHECKBOX_31169, CHookMouse,          eType_MOUSE);
    SPY_CHECK_HOOK(IDC_CHECKBOX_31170, CHookMouseLL,        eType_MOUSE_LL);
    SPY_CHECK_HOOK(IDC_CHECKBOX_31171, CHookMsgFilter,      eType_MSGFILTER);
    SPY_CHECK_HOOK(IDC_CHECKBOX_31172, CHookShell,          eType_SHELL);
    SPY_CHECK_HOOK(IDC_CHECKBOX_31173, CHookSysMsgFilter,   eType_SYSMSGFILTER);
    SPY_CHECK_HOOK(IDC_CHECKBOX_31174, CHookShellUser32,    eType_HookShellUser32);
    SPY_CHECK_HOOK(IDC_CHECKBOX_31256, CHookEventSelf,      eType_HookEventSelf);
    SPY_CHECK_HOOK(IDC_CHECKBOX_33093, CHookEventGlobal,    eType_HookEventGlobal);
    SPY_CHECK_HOOK(IDC_CHECKBOX_30435, CHookEventFunc,      eType_HookEventSelf);

#undef SPY_CHECK_HOOK

    return 0;
}

TMResult CDlgSpyMsg::OnLogPaster(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TM::CString str;
    CClipboard cb;
    cb.GetText(str);
    CString strLine;
    for (size_t nStart = 0; nStart = str.SplitLine(nStart, strLine); )
    {
        strLine = strLine.Trim();
        if (!strLine.IsEmpty())
            NativeAppendCache(strLine, CString());
    }
    return 0;
}

TMResult CDlgSpyMsg::OnLogFilter (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    m_aFilter[0].clear(); CString str1; Item(IDC_EDIT_SPY_INC).GetWindowText(str1); str1.SplitTo(m_aFilter[0], ';');
    m_aFilter[1].clear(); CString str2; Item(IDC_EDIT_SPY_EXC).GetWindowText(str2); str2.SplitTo(m_aFilter[1], ';');
    for (CTVItem itm(m_itmRoot); itm = itm.GetGlobalNext();)
        CheckTVItem(itm);
    return 0;
}

TMResult CDlgSpyMsg::OnEditBreak (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CString str;
    CTuiWidget((HTUI)hWndCtl).GetWindowText(str);
    m_aBreak.clear();
    str.SplitTo(m_aBreak, ';');
    CTuiButton(Item(IDC_PUSHBUTTON_PAUSE_BREAK)).SetCheck(1);
    m_bBreaked = 1;
    return 0;
}

TMResult CDlgSpyMsg::OnCheckRemote (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    return m_remote.CanRecive() ? (TMResult)m_hRemote : 0;
}

TMResult CDlgSpyMsg::OnRemoteMessage (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_remote.Recive(this, lParam);
    
    MSG msg;
    for (; PeekMessage(&msg, GetHWND(), WM_REMOTE_MESSAGE, WM_REMOTE_MESSAGE, PM_REMOVE);)
    {
        m_remote.Recive(this, msg.lParam);
    }
    return true;
}

TMBool CDlgSpyMsg::AppendRemoteText(REMOTE_MESSAGE* p)
{
    DbgAssert(p);
    if (HWND_RECIVE == m_hRemote)
    {
        if (m_bPasue)
            return true;
        if ((eTypeWin == p->type) && !m_bShowLog[eTypeWin])
            return true;
        if ((eTypeDgt == p->type) && !m_bShowLog[eTypeDgt])
            return true;
//         CheckBreak(p->strText);
        NativeAppendCache(p->strText, p->strHint);
        return true;
    }
    return false;

}


TMResult CDlgSpyMsg::OnRemote (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiButton but((HTUI)hWndCtl);
    m_hRemote = (HWND)but.GetCheck();

    TMBool bEnableBreak = (m_hRemote != HWND_SEND);
    Item(IDC_PUSHBUTTON_30272).EnableWindow(bEnableBreak); 
    Item(IDC_PUSHBUTTON_PAUSE_BREAK).EnableWindow(bEnableBreak); 

    m_remote.Set(0); 
    if (m_hRemote == HWND_SEND)
    {
        HWND hFind = 0;
        for (; hFind = FindWindowEx(0, hFind, TUI_CLASS_WIN_MAIN, L"Log Message");)
        {
            HWND ret = (HWND)::SendMessage(hFind, WM_CHECK_REMOTE, 0, 0);
            if (ret == HWND_RECIVE)
            {
                SendSysText(L"set Log message mode to remote send mode");
                m_remote.Set(hFind); 
                m_hRemote = hFind;
                return 0;
            }
        }

        // not find receive window
        SendSysText(L"not find remote window, please open remote 'Log Message' and reclick button");
    }
    else if (m_hRemote == HWND_RECIVE)
    {
        SendSysText(L"set Log message mode to remote receive mode");
    }
    else if (m_hRemote == HWND_SELF)
        SendSysText(L"set Log message mode to self mode");

    TMBool bEnable = (m_hRemote != HWND_RECIVE);
    Item(IDC_PUSHBUTTON_OTHER).EnableWindow(bEnable); 
    Item(IDC_PUSHBUTTON_30272).EnableWindow(bEnable); 
    Item(IDC_PUSHBUTTON_PAUSE_BREAK).EnableWindow(bEnable); 

    return 0;
}

TMResult CDlgSpyMsg::OnFlush(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    CAutoLock lock(m_lock);
    CStringCache aTmp;
    aTmp.swap(m_cache);

    TMLParam lOld = _dbgApp.TraceSetToSys(false);

    CTVItem itm;
    for (CStringCache::recorder rec(aTmp); rec; ++rec)
    {
        CStringCache::value_type& v = rec.get_data();
        itm = _NativeAppendText(v);
    }

    if (m_bAutoScroll && itm)
        itm.EnsureVisible();

    _dbgApp.TraceSetToSys(lOld);

    return 0;
}


TMResult CDlgSpyMsg::OnClear(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiTree tree(Item(IDC_TREE_31124));
    tree.ResetContent();
    m_itmRoot = tree.GetRootItem();
    m_selParent = 0;
    m_strSel.clear();
    m_strSelID.clear();
    m_nCurLineID = 0;
    m_nInd = 0;
    return 0;
}

TMResult CDlgSpyMsg::OnDelete(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiTree tree(Item(IDC_TREE_31124));
    for (CTVItem itm(tree.GetRootItem()); itm;)
    {
        CTVItem itm2 = itm.GetGlobalNext();
        if (itm2 && itm2.IsSelected())
            itm2.Remove();
        else
            itm = itm2;
    }
    m_selParent = 0;
    m_strSel.clear();
    m_strSelID.clear();
    m_nCurLineID = 0;
    return 0;
}


TMResult CDlgSpyMsg::OnPause (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    m_bPasue = CTuiButton((HTUI)(hWndCtl)).GetCheck();
    return 0;
}

TMResult CDlgSpyMsg::OnCopy (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiTree tree(Item(IDC_TREE_31124));
    
    CString strTxt;
    for (CTVItem itm = tree.GetRootItem(); itm = itm.GetGlobalNext();)
    {
        if (itm.IsSelected())
        {
            if (itm.GetHeight() > 2)
            {
                CString str;
                itm.GetText(str);
                strTxt += str;
                strTxt += L"\r\n";

                CString strHint;
                itm.GetHint(strHint);
                if (!strHint.IsEmpty())
                {
                    strTxt += strHint;
                    strTxt += L"\r\n";
                }
            }
        }
    }
    
    CClipboard().AddString(strTxt);
    return 0;
}

TMResult CDlgSpyMsg::OnOther (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiWidget(Item(IDC_WINDOW_31156)).DoMenuPop((HTUI)hWndCtl);
    return 0;
}

TMResult CDlgSpyMsg::OnBreak (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTVItem itm = CTuiTree(m_itmRoot.GetWindow()).GetSelectedItem();
    if (!itm)
        return 0;

    {
        CString str;
        itm.GetText(str);
        CStringList::recorder rec = IsBreak(str);
        if (rec)
            rec.erase(); // find, remove it;
        else
        {
            // add
            str = str.TrimLeft(PRECHAR);
            m_aBreak.insert(str.substr(nPosHandle));
        }
    }

    CString str;
    for (CStringList::recorder rec(m_aBreak); rec; ++rec)
    {
        str += rec.get_data();
        str += ';';
    }
    Item(IDC_EDIT_BREAK).SetWindowText(str);
    m_tuiWindow.Invalidate();
    return 0;
}

TMResult CDlgSpyMsg::OnPauseBreak (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    m_bBreaked = CTuiButton((HTUI)hWndCtl).GetCheck();
    return 0;
}

TMResult CDlgSpyMsg::OnTypeBlock (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    m_bTypeBlcok = CTuiButton((HTUI)hWndCtl).GetCheck();
    return 0;
}

TMResult CDlgSpyMsg::OnSelChange (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTVItem itm = CTuiTree(m_itmRoot.GetWindow()).GetSelectedItem();
    if (!itm)
    {
        m_bAutoScroll = true;
        return 0;
    }

    // if select on last itm, the tree will auto scroll.
    m_bAutoScroll = !itm.GetGlobalNext();

    m_selParent = itm.GetParent();
    itm.GetText(m_strSel);
    m_strSel = m_strSel.TrimLeft(PRECHAR);
    if (m_strSel.GetLength() < 30)
    {
        m_strSelID.clear();
        return 0;
    }

    m_strSelID = m_strSel.substr(nPosID, nLenID);
    int nRight = m_strSel.Find(')', nPosHandle);
    if (nRight < 0)
        return 0;
    CString str2(m_strSel.substr(nPosHandle, nRight - nPosHandle));
    m_wndPanel->SelectByString(str2);
    return 0;
}

TMResult CDlgSpyMsg::OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

void CDlgSpyMsg::AddSpyID(IObjEdit* pSel)
{
    if (!pSel)
        return;
    CString strID;
    pSel->GetIdStr(strID);
    CTuiEdit(Item(IDC_EDIT_SPY_INC)).AppendText(CString(L";") + strID);
}


struct CAutoInd
{
    CAutoInd()
    {
        ++CDlgSpyMsg::s_pMSg->m_nInd;
    }

    ~CAutoInd()
    {
        if (CDlgSpyMsg::s_pMSg->m_nInd > 0)
            --CDlgSpyMsg::s_pMSg->m_nInd;
    }
};

struct CMsgLog
{
    CDlgSpyMsg* m_pDlg;
    void* m_lr;

    CMsgLog(CDlgSpyMsg* pDlg, TMResult* lr)
        : m_lr(lr)
        , m_pDlg(pDlg)
    {
        if (m_pDlg && m_lr && m_pDlg->m_nInd > 0) // m_pDlg->m_strInd.size()) // message end
        {
            --m_pDlg->m_nInd; // m_pDlg->m_strInd.erase(m_pDlg->m_strInd.begin());
        }
    }

    ~CMsgLog()
    {
        if (m_pDlg && !m_lr) // message begin
        {
            ++m_pDlg->m_nInd; //            m_pDlg->m_strInd += PRECHAR;
        }
    }
};

struct _CDlgSpyMsgInd
{
    CSpyMsg::EMode eMode;
    _CDlgSpyMsgInd(CSpyMsg::EMode e) : eMode(e)
    {
        if (eMode == CSpyMsg::eOut && CDlgSpyMsg::s_pMSg->m_nInd > 0) // s_pMSg->m_strInd.size())
            --CDlgSpyMsg::s_pMSg->m_nInd; // s_pMSg->m_strInd.erase(s_pMSg->m_strInd.begin());
    }
    ~_CDlgSpyMsgInd()
    {
        if (eMode == CSpyMsg::eIn)
            ++CDlgSpyMsg::s_pMSg->m_nInd; // s_pMSg->m_strInd += PRECHAR;
    }
};

void CDlgSpyMsg::OnSpyDone(EType eType, LPCTSTR pStr, HWND hWnd, TUIMSG* pMsg, TMResult* lr, int& step, const double* cycle)
{
    if (m_bPasue)
        return;

    int oid = /*m_nCurOID =*/ pMsg->uObjectID;// CTuiWidget(pMsg->hWnd).GetDlgCtrlObjectID();

    CString strCycle(L" ");
    if (lr && cycle)
        strCycle.Format(L"(%g us)", cycle);
    if (pStr) // SpyFunction or invalidate
    {
        if (!m_bShowLog[eType])
            return;
        CString str;
        CMsgLog log(eType == eTypeFunction ? this : 0 , lr);
        str.Format(strFmtFUN, pMsg->hWnd, oid);
        str += pStr;
        str += strCycle;
        AppendText(!lr, eTypeWin, str);
    }
    DbgAssert(pStr);
}

void CDlgSpyMsg::OnSpyDone(CSpyMsg& sm, CSpyMsg::EMode eMode, CSpyMsg::EType eType, int tid)
{
    DbgAssert(eType >= 0 && eType < CSpyMsg::eEnd);
    if (m_bPasue || !m_bShowLog[eType])
        return;

    CAutoLock lock(m_lock);

    _CDlgSpyMsgInd smi(eMode);
    if (sm.bBlock)
        return;

    CString str;
    CString strHint;
    sm.ToItemString(str, strHint, eMode);
    if (!str.IsEmpty())
        sm.bBlock = AppendText(eMode != CSpyMsg::eOut, (CDlgSpyMsg::EType)eType, str, strHint);
    return;
}

void CDlgSpyMsg::OnTrace(int tp, LPCTSTR pStr)
{
    if (m_bPasue)
        return;

//     TMBool b = (g_bInSpyMessageOrFunc || (s_pMSg && s_pMSg->m_strInd.IsEmpty())) && m_bShowLog[eTypeDBWin];
    TMBool b = (g_bInSpyMessageOrFunc || m_nInd == 0) && m_bShowLog[eTypeDBWin];
    if (!b)
        return;

    AppendText(true, eTypeDBWin, pStr);
}

TMBool CDlgSpyMsg::OnCallWndProc(CWPSTRUCT* msg)
{
    FormatAndAppendText(true, eType_CALLWNDPROC, strFmt_CALLWNDPROC, msg->hwnd, DbgFormatMessage(msg->message, msg->wParam, msg->lParam, 0));
    return 0;
}

TMBool CDlgSpyMsg::OnCallWndRetProc(CWPRETSTRUCT* msg)
{
    FormatAndAppendText(true, eType_CALLWNDPROCRET, strFmt_CALLWNDPROCRET, msg->hwnd, DbgFormatMessage(msg->message, msg->wParam, msg->lParam, &msg->lResult));
    return 0;
}

#define PITM(x)   {x, L#x}

static CString _CDlgSpyMsg_GetCbtString(int nCode)
{
    CStaticMapPair<int, LPCTSTR> map[] =
    {
        PITM(HCBT_MOVESIZE),                                                
        PITM(HCBT_MINMAX),
        PITM(HCBT_QS),
        PITM(HCBT_CREATEWND),
        PITM(HCBT_DESTROYWND),
        PITM(HCBT_ACTIVATE),
        PITM(HCBT_CLICKSKIPPED),
        PITM(HCBT_KEYSKIPPED),
        PITM(HCBT_SYSCOMMAND),
        PITM(HCBT_SETFOCUS),
    };
    CString p;
    p.Format(L"%d", nCode);
    StaticMapFind(map, nCode, p);
    CString str(L" nCode:");
    str += p;
    return str;
}

TMBool CDlgSpyMsg::OnCbt(int nCode, WPARAM wParam, LPARAM lParam)
{
    CString str;
    str.Format(L"%s, wParam:%p, lParam:%p", _CDlgSpyMsg_GetCbtString(nCode).c_str(), wParam, lParam);
    FormatAndAppendText(true, eType_CBT, strFmt_CBT, 0, str);
    return 0;
}

TMBool CDlgSpyMsg::OnForegroundIdle()
{
    FormatAndAppendText(true, eType_FOREGROUNDIDLE, strFmt_FOREGROUNDIDLE, 0, L"");
    return 0;
}

TMBool CDlgSpyMsg::OnGetMessage(TMBool bRemove, MSG* msg)
{
    CString str(DbgFormatMessage(msg->message, msg->wParam, msg->lParam) + VA2T(L" bRemove:%d", bRemove));
    FormatAndAppendText(true, eType_GETMESSAGE, strFmt_GETMESSAGE, msg->hwnd, str);
    return 0;
}

TMBool CDlgSpyMsg::OnKeyboard(UINT uVirtKey, UINT uFlags)
{
    FormatAndAppendText(true, eType_KEYBOARD, strFmt_KEYBOARD, 0, VA2T(L"uVirtKey:0X%X, uFlags:0X%X", uVirtKey, uFlags));
    return 0;
}

TMBool CDlgSpyMsg::OnKeyboardLL(UINT uMsg, KBDLLHOOKSTRUCT* ps)
{
    CString str(DbgFormatMessage(uMsg) + VA2T(L" vkCode:0X%X", ps->vkCode));
    FormatAndAppendText(true, eType_KEYBOARD_LL, strFmt_KEYBOARD_LL, 0, str);
    return 0;
}

TMBool CDlgSpyMsg::OnMouse(UINT uMsg, MOUSEHOOKSTRUCT* pm)
{
    CString str(DbgFormatMessage(uMsg) + VA2T(L" pt:%d,%d, wHitTestCode:%d", pm->pt, pm->wHitTestCode));
    FormatAndAppendText(true, eType_MOUSE, strFmt_MOUSE, pm->hwnd, str);
    return 0;
}

TMBool CDlgSpyMsg::OnMouseLL(UINT uMsg, MSLLHOOKSTRUCT* pm)
{
    CString str(DbgFormatMessage(uMsg) + VA2T(L" pt:%d,%d", pm->pt));
    FormatAndAppendText(true, eType_MOUSE_LL, strFmt_MOUSE_LL, 0, str);
    return 0;
}

static CString _CDlgSpyMsg_GetMsgFilterString(int nEvent, MSG* msg)
{
#ifndef MSGF_DDEMGR
#   define MSGF_DDEMGR 0x8001
#endif
    
    CStaticMapPair<int, LPCTSTR> map[] = { PITM(MSGF_DDEMGR), PITM(MSGF_DIALOGBOX), PITM(MSGF_MENU), PITM(MSGF_SCROLLBAR) };
    LPCTSTR p = L"unknown";
    StaticMapFind(map, nEvent, p);
    CString str(DbgFormatMessage(msg->message, msg->wParam, msg->lParam) + L" nEvent:");
    str += p;
    return str;
}

static CString _CDlgSpyMsg_GetShellString(int nCode)
{
#ifndef HSHELL_WINDOWREPLACED
#   define HSHELL_APPCOMMAND        12
#   define HSHELL_WINDOWREPLACED    13
#   define HSHELL_WINDOWREPLACING   14
#   define HSHELL_HIGHBIT           0x8000
#   define HSHELL_FLASH             (HSHELL_REDRAW | HSHELL_HIGHBIT)
#endif

#ifndef SHELL_RUDEAPPACTIVATED
#   define SHELL_RUDEAPPACTIVATED   (HSHELL_WINDOWACTIVATED | HSHELL_HIGHBIT)
#endif

    CStaticMapPair<int, LPCTSTR> map[] =
    {
        PITM(HSHELL_ACCESSIBILITYSTATE), PITM(HSHELL_ACTIVATESHELLWINDOW), 
        PITM(HSHELL_GETMINRECT), PITM(HSHELL_LANGUAGE), PITM(HSHELL_REDRAW), PITM(HSHELL_TASKMAN),
        PITM(HSHELL_WINDOWACTIVATED), PITM(HSHELL_WINDOWCREATED), PITM(HSHELL_WINDOWDESTROYED),
        PITM(HSHELL_APPCOMMAND), PITM(HSHELL_WINDOWREPLACED), PITM(HSHELL_FLASH), PITM(SHELL_RUDEAPPACTIVATED),
    };
    LPCTSTR p = L"unknown";
    StaticMapFind(map, nCode, p);
    CString str(L" nCode:");
    str += p;
    return str;
}


TMBool CDlgSpyMsg::OnMsgFilter(int nEvent, MSG* msg)
{
    FormatAndAppendText(true, eType_MSGFILTER, strFmt_MSGFILTER, msg->hwnd, _CDlgSpyMsg_GetMsgFilterString(nEvent, msg));
    return 0;
}

TMBool CDlgSpyMsg::OnShell(int nCode, WPARAM wParam, LPARAM lParam)
{
    FormatAndAppendText(true, eType_SHELL, strFmt_SHELL, (HWND)wParam, _CDlgSpyMsg_GetShellString(nCode));
    return 0;
}

TMBool CDlgSpyMsg::OnSysMsgFilter(int nEvent, MSG* msg)
{
    FormatAndAppendText(true, eType_SYSMSGFILTER, strFmt_SYSMSGFILTER, msg->hwnd, _CDlgSpyMsg_GetMsgFilterString(nEvent, msg));
    return 0;
}

void CDlgSpyMsg::OnShellUser32(int nCode, WPARAM wParam)
{
    FormatAndAppendText(true, eType_HookShellUser32, strFmtHookShellUser32, (HWND)wParam, _CDlgSpyMsg_GetShellString(nCode));
}

void CDlgSpyMsg::OnEvent(DWORD event, HWND hwndMsg, LONG idObject, LONG idChild, DWORD idThread, DWORD dwmsEventTime)
{
    if (m_bPasue || /*m_bInEvent ||*/ hwndMsg == GetHWND() || hwndMsg == ::GetParent(GetHWND()))
        return;

//    m_bInEvent = false;
    LPCTSTR pEvent = DbgFormatAccEvent(event);
    CString strHInt;
    CString strRolAndID;
    {
        // DbgFormatAccFromEvent maybe reentry
        if (s_bEntry)
            return;
        s_bEntry = true;

        CAutoInd ind;
        TMLParam lOld = _dbgApp.TraceSetToSys(false);
        strHInt = DbgFormatAccFromEvent(hwndMsg, idObject, idChild, &strRolAndID);
         _dbgApp.TraceSetToSys(lOld);
        s_bEntry = false;
    }

    CString str;
    str.Format(L"%s, %s, %X, %s, hWnd:%X idThread:%X", //, dwmsEventTime:%d",
        pEvent, DbgFormatObjId(idObject), idChild, strRolAndID.c_str(), hwndMsg, idThread);//, dwmsEventTime);


    FormatAndAppendText(true, eType_HookEventSelf, strFmtHookEvent, (HWND)hwndMsg, str, strHInt);
//    m_bInEvent = false;
}

static CDlgSpyMsg::CStringList::recorder _CDlgSpyMsg_Filter(CDlgSpyMsg::CStringList& aList, const TM::CString& str)
{
    CDlgSpyMsg::CStringList::recorder rec(aList);
    for (; rec; ++rec)
    {
        if (str.FindI(rec.get_data()) > 0)
            break;
    }
    return rec;
}

TMBool CDlgSpyMsg::Filter(const TM::CString& str, int n)
{
    DbgAssert(n == 0 || n == 1);
    return _CDlgSpyMsg_Filter(m_aFilter[n], str);
}

void CDlgSpyMsg::CheckTVItem(TM::CTVItem itm)
{
    TMBool bShow = true;
    CString str;
    itm.GetText(str);
    if (m_aFilter[1].size() && Filter(str, 1))
        bShow = false;
    if (bShow && m_aFilter[0].size() && !Filter(str, 0))
        bShow = false;
    if (bShow)
        itm.SetHeight(-1);
    else
        itm.SetHeight(0);
}

bool CDlgSpyMsg::CheckBreak(TM::CString& str)
{
    if (m_bTypeBlcok && IsBreak(str))
    {
        str += L" blocked";
        return true;
    }

    if (!m_bBreaked && IsBreak(str))
    {
        CTuiButton(Item(IDC_PUSHBUTTON_PAUSE_BREAK)).SetCheck(1);
        m_bBreaked = 1;
        TMBool bOld = _dbgApp.TraceSetAssert(true);
        DbgAssert(!L"Please pause program by VC");
        _dbgApp.TraceSetAssert(bOld);
    }

    return false;
}

CDlgSpyMsg::CStringList::recorder CDlgSpyMsg::IsBreak(const TM::CString& str)
{
    return _CDlgSpyMsg_Filter(m_aBreak, str);
}

void CDlgSpyMsg::FormatAndAppendText(BOOL bIn, EType type, LPCTSTR strFmt, HWND hWnd,  const CString& pText, const TM::CString& strHint)
{
    if (m_bPasue)
        return;

    if (hWnd == GetHWND())
        return;

    CString str;
    str.Format(strFmt, hWnd);

    str += pText;
    str += ' ';
    CString strH;

    DWORD pid = 0;
    ::GetWindowThreadProcessId(hWnd, &pid);
    if (!pid)
        str += L"unknown ";
    else
    {
        strH += L"hWnd(";
        strH += DbgFormatWindow(hWnd);
        strH += L")\r\n";

        if (pid == GetCurrentProcessId())
        {
            str += L"self ";
        }
        else
        {
            //         Windows 2000 = GetModuleFileName()
            //         Windows XP x32 = GetProcessImageFileName()
            //         Windows XP x64 = GetProcessImageFileName()
            //         Windows Vista = QueryFullProcessImageName()
            //         Windows 7 = QueryFullProcessImageName()


            HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
            wchar_t tsFileDosPath[MAX_PATH + 1] = { 0 };

            //*/
            DECLARE_DLL_FUNC(dll_kernel32, BOOL, WINAPI, QueryFullProcessImageNameW,
            (HANDLE hProcess, DWORD  dwFlags, LPWSTR lpExeName, PDWORD lpdwSize));
            if (_QueryFullProcessImageNameW)
            {
                DWORD dwx = MAX_PATH + 1;
                _QueryFullProcessImageNameW(hProc, 0, tsFileDosPath, &dwx);
            }
            else
            {
                DECLARE_DLL_FUNC(dll_psapi, DWORD, WINAPI, GetProcessImageFileNameW,
                (HANDLE hProcess, LPWSTR  lpImageFileName, DWORD  nSize));
                _GetProcessImageFileNameW(hProc, tsFileDosPath, MAX_PATH + 1);
            }



            /*/
            DECLARE_DLL_FUNC("psapi.dll", BOOL, WINAPI, EnumProcessModules,
            (HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded));
            #   define EnumProcessModules CDllFunc_EnumProcessModules::LoadDirSystem32()

              DECLARE_DLL_FUNC("psapi.dll", DWORD, WINAPI, GetModuleFileNameExW,
              (HANDLE hProcess, HMODULE hModule, LPWSTR lpFilename, DWORD nSize));
              #   define GetModuleFileNameEx CDllFunc_GetModuleFileNameExW::LoadDirSystem32()

                HMODULE hMod;
                DWORD cbNeeded;
                if (EnumProcessModules(hProc, &hMod, sizeof(hMod), &cbNeeded))
                GetModuleFileNameEx(hProc, hMod, tsFileDosPath, MAX_PATH + 1);
                //*/
            str += tsFileDosPath;
            str += ' ';
            CloseHandle(hProc);
        }
    }
    AppendText(bIn, type, str, strH + strHint);
}

bool CDlgSpyMsg::AppendText(BOOL bIn, EType type, const CString& pText, const TM::CString& strHint, int tid)
{
    CString strx;
    if (bIn)
        strx.Format(L"<%06d> ", ++m_nCurLineID);
    else
        strx.Format(L"<%06d< ", m_nCurLineID);
    strx += pText;

    if (m_hRemote > HWND_RECIVE)
    {
        TMBool b = m_remote.Send(m_hRemote, type, strx, strHint, m_nInd);//m_strInd.GetLength());
        if (!b)
            SendSysText(L"send remote message error");
        return false;
    }
    else if (m_hRemote == HWND_SELF)
    {
        int tNow = ThreadSelfId();
        if (tNow == m_tid)
        {
            if (tid == 0 || tid == tNow)
            {
                return NativeAppendCache(strx, strHint);
            }
            else
            {
                CString str;
                str.Format(L"[%d] ", tid);
                str += strx;
                return NativeAppendCache(str, strHint);
            }
        }
        else
        {
            CString str;
            str.Format(L"[%d] ", tNow);
            str += strx;
            return NativeAppendCache(str, strHint);
        }
    }

    return false;
}

CTVItem CDlgSpyMsg::_NativeAppendText(CACHEITEM& cache)
{
    CTVItem itmLast = m_itmRoot.GetGlobalPrev();
    if (!itmLast)
        itmLast = m_itmRoot;
    int nLevel = itmLast.GetLevel();
    int nTreeLevel = cache.nInd;
    for (; nLevel > nTreeLevel; --nLevel)
    {
        CTVItem itm = itmLast.GetParent();
        if (itm)
            itmLast = itm;
        else
            break;
    }
    
    if (nTreeLevel)
        cache.str.insert((size_t)0, nTreeLevel, PRECHAR);
    CTVItem itm = itmLast.InsertChild(cache.str, -1);
    itm.SetHint(cache.hint);
    CheckTVItem(itm);
    return itm;
}

bool CDlgSpyMsg::NativeAppendCache(const TM::CString& text, const TM::CString& strHint)
{
    CAutoLock lock(m_lock);

    if (!m_cache.size())
        PostMessage(WM_FLUSH, 0, 0);

    CACHEITEM ci;
    ci.nInd = m_nInd; // m_strInd.length();
    ci.str = text;
    ci.hint = strHint;
    bool b = CheckBreak(ci.str);
    m_cache.insert(ci);

    return b;
}

void CProcessMemorys::Set(HWND hWnd)
{
    if (m_hProcess)
    {
        CloseHandle (m_hProcess);
        m_hProcess = 0;
    }

    if (hWnd)
    {
        DWORD dwProcessId;
        if (!GetWindowThreadProcessId(hWnd, &dwProcessId))
            return;
        
        m_hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, dwProcessId);
        m_nCur = 0;
    }
}

TMBool CProcessMemorys::Send(HWND hWnd, TMUInt16 type, const CString& pText, const TM::CString& strHint, int ind)
{
    if (!m_hProcess || !IsWindow(hWnd))
        return false;

    REMOTE_MESSAGE local = { true, type, ind };
    pText.CopyTo(local.strText);
    strHint.CopyTo(local.strHint);
    
    REMOTE_MESSAGE* pMem = (REMOTE_MESSAGE*)VirtualAllocEx(m_hProcess, NULL, sizeof(REMOTE_MESSAGE), MEM_COMMIT, PAGE_READWRITE);
    TMBool bRet = WriteProcessMemory(m_hProcess, pMem, &local, sizeof(REMOTE_MESSAGE), NULL);
    if (bRet)
        DbgVerify(bRet = ::PostMessage(hWnd, WM_REMOTE_MESSAGE, 0, (TMLParam)pMem));

    ++m_nCur;
    if (m_nCur > eCacheCount)
    {
        m_nCur = 0;
        DbgVerify(::SendMessage(hWnd, WM_REMOTE_MESSAGE, 0, 0));
    }

    return bRet;
}

void CProcessMemorys::Recive(CDlgSpyMsg* dlg, TMLParam lp)
{
    if (!lp)
        return;
    REMOTE_MESSAGE* p = (REMOTE_MESSAGE*)lp;
    DbgAssert(p->bUsed);
    dlg->m_nInd = p->ind; // m_strInd.resize(p->ind, PRECHAR);
    dlg->AppendRemoteText(p);
    p->bUsed = false;
    DbgVerify(VirtualFree(p, 0, MEM_RELEASE));
}

void CDlgSpyMsg::SendSysText(LPCWSTR str)
{
    CString strx;
    strx.Format(L"<%06d> (%p)SYS(0) ", ++m_nCurLineID, GetCurrentProcessId());
    strx += str;
    NativeAppendCache(strx, CString());
}
