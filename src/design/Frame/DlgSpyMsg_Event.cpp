/**
 *  @file
 *  @brief
 *  CDlgBlockEvent
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2026-1-23
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2026 miragekiller                             
 *  All rights reserved  
 */

#include "stdafx.h"

#include "ia2-api\ia2_api_all.h"
#include <UIAutomation.h>

#include "../design.h"
#include "tmformatacc.h"
#include "tmformatmsg.h"
#include "DlgSpyMsg_Event.h"
// #include "DlgSpyMsg.h"
#include "tmminhook.h"
//#include <UIAutomationCore.h>

using namespace TM;

#define EVENT_OFFSET    100



static LPCTSTR const strFmtHookEvent          = L"(%p)HookEvent %s, idObj:%s, idChild:%X";
static LPCTSTR const strFmtHookUIA            = L"(%p)HookUIA %s, rtid:%X, root rtid:%X, focus rtid:%X, host rtid:%X";

struct CSpyMsg_Event : public CSpyMsg
{
    TM::CString m_str;
    CComVariant m_strAccName;
    TM::CString m_strHint;
    int m_idObj;
    int m_idRoot;
    int m_idFocus;
    int m_idHost;

    CSpyMsg_Event() : m_idRoot(-1), m_idFocus(-1), m_idHost(-1) {}

    CSpyMsg_Event(DWORD event, HWND hwnd, LONG idObject, LONG idChild)
    {
        m_idObj = idObject;
        m_str.Format(strFmtHookEvent, hwnd, DbgFormatAccEvent(event), DbgFormatObjId(idObject), idChild);
        SpyMsgFunc(*this, eIn, eHookEvent);
    }

    BOOL CheckUIA(IRawElementProviderSimple* pProvider, DWORD event, TMCStr param)
    {
//         if (CDlgBlockEvent::s_dlg && CDlgBlockEvent::s_dlg->FilterEvent(event)) // AutomationPropertyChangedEventId
//             return true;
// 
        pProvider->GetPropertyValue(UIA_NamePropertyId, &m_strAccName);

        int type = 0;
        DbgIAccessibleGetRuntimeId(pProvider, type, m_idObj);
        CComQIPtr<IRawElementProviderFragment> frag(pProvider);
        if (frag)
        {
            CComQIPtr<IRawElementProviderFragmentRoot> root;
            frag->get_FragmentRoot(&root);
            if (root)
                DbgIAccessibleGetRuntimeId(CComQIPtr<IRawElementProviderFragment>(root), type, m_idRoot);

            CComQIPtr<IRawElementProviderFragment> parent;
            frag->Navigate(NavigateDirection_Parent, &parent);
        }

        CComQIPtr<IRawElementProviderFragmentRoot> root(pProvider);
        if (root)
        {
            CComQIPtr<IRawElementProviderFragment> focus;
            root->GetFocus(&focus);
            if (focus)
                DbgIAccessibleGetRuntimeId(focus, type, m_idFocus);
        }

        CComPtr<IRawElementProviderSimple> host;
        pProvider->get_HostRawElementProvider(&host);
        if (host)
            DbgIAccessibleGetRuntimeId(host, type, m_idHost);

        m_str.Format(strFmtHookUIA, pProvider, DbgFormatAccEvent(event), m_idObj, m_idRoot, m_idFocus, m_idHost);
        if (param)
            m_str += param;
        m_strHint = DbgFormatUIA(pProvider);
        SpyMsgFunc(*this, eIn, eHookEvent);

        return bBlock;
    }

    ~CSpyMsg_Event()
    {
        SpyMsgFunc(*this, eOut, eHookEvent);
    }
    
    virtual void ToItemString(TM::CString& str, TM::CString& strHint, EMode eMode) tm_final
    {
        str = m_str;
        strHint = m_strHint;
    }
};


DECLARE_MINHOOK_FUNC(dll_user32, VOID, WINAPI, NotifyWinEvent, (DWORD event, HWND hwnd, LONG idObject, LONG idChild))
{
    CSpyMsg_Event evt(event, hwnd, idObject, idChild);
    if (evt.bBlock)
        return;
//     if (CDlgBlockEvent::s_dlg && CDlgBlockEvent::s_dlg->FilterEvent(event))
//         return;
    _NotifyWinEvent_Org(event, hwnd, idObject, idChild);
}

DECLARE_MINHOOK_FUNC(
    dll_Uiautomationcore, HRESULT, WINAPI,
    UiaRaiseAutomationPropertyChangedEvent,
    (IRawElementProviderSimple * pProvider, PROPERTYID id, VARIANT oldValue, VARIANT newValue))
{
    CSpyMsg_Event evt;
    if (evt.CheckUIA(pProvider, 20004, VA2T(L", propId:%d", id))) // AutomationPropertyChangedEventId
        return S_OK;
    return _UiaRaiseAutomationPropertyChangedEvent_Org(pProvider, id, oldValue, newValue);
}

DECLARE_MINHOOK_FUNC(
    dll_Uiautomationcore, HRESULT, WINAPI,
    UiaRaiseAutomationEvent,
    (IRawElementProviderSimple * pProvider, EVENTID id))
{
//     if (id == 20005) //UIA_AutomationFocusChangedEventId)
//     {
//         static CSet<DWORD> aSet;
//         int type;
//         int xid = 0;
//         DbgIAccessibleGetRuntimeId(pProvider, type, xid);
//         DbgTrace("UiaRaiseAutomationEvent UIA_AutomationFocusChangedEventId %X", xid);
//         if (aSet.insert(xid).second)
//             return S_OK;
//     }

    CSpyMsg_Event evt;
    if (evt.CheckUIA(pProvider, id, 0))
        return S_OK;
    CSpyMsg_Event evt2;
    return _UiaRaiseAutomationEvent_Org(pProvider, id);
}

DECLARE_MINHOOK_FUNC(
    dll_Uiautomationcore, HRESULT, WINAPI,
    UiaRaiseStructureChangedEvent,
    (IRawElementProviderSimple * pProvider, enum StructureChangeType structureChangeType, int * pRuntimeId, int cRuntimeIdLen))
{
    CSpyMsg_Event evt;
    if (evt.CheckUIA(pProvider, 20002, VA2T(L", structureChangeType:%d", structureChangeType)))  // StructureChangedEventId
        return S_OK;
    return _UiaRaiseStructureChangedEvent_Org(pProvider, structureChangeType, pRuntimeId, cRuntimeIdLen);
};

DECLARE_MINHOOK_FUNC(
    dll_Uiautomationcore, HRESULT, WINAPI,
    UiaRaiseAsyncContentLoadedEvent,
    (IRawElementProviderSimple * pProvider, enum AsyncContentLoadedState asyncContentLoadedState, double percentComplete))
{
    CSpyMsg_Event evt;
    if (evt.CheckUIA(pProvider, 20006, VA2T(L", asyncContentLoadedState:%d", asyncContentLoadedState)))  // AsyncContentLoadedEventId
        return S_OK;
    return _UiaRaiseAsyncContentLoadedEvent_Org(pProvider, asyncContentLoadedState, percentComplete);
};

DECLARE_MINHOOK_FUNC(
    dll_Uiautomationcore, HRESULT, WINAPI,
    UiaRaiseTextEditTextChangedEvent,
    (IRawElementProviderSimple * pProvider, enum TextEditChangeType textEditChangeType, SAFEARRAY *pChangedData))
{
    CSpyMsg_Event evt;
    if (evt.CheckUIA(pProvider, 20015, VA2T(L", textEditChangeType:%d", textEditChangeType)))  // Text_TextChangedEventId
        return S_OK;
    return _UiaRaiseTextEditTextChangedEvent_Org(pProvider, textEditChangeType, pChangedData);
};

DECLARE_MINHOOK_FUNC(
    dll_Uiautomationcore, HRESULT, WINAPI, 
    UiaRaiseChangesEvent,
    (IRawElementProviderSimple * pProvider, int eventIdCount, struct UiaChangeInfo * pUiaChanges))
{
    CSpyMsg_Event evt;
    if (evt.CheckUIA(pProvider, 20034, VA2T(L", eventIdCount:%d", eventIdCount)))  // ChangesEventId
        return S_OK;
    return _UiaRaiseChangesEvent_Org(pProvider, eventIdCount, pUiaChanges);
}

DECLARE_MINHOOK_FUNC(
    dll_Uiautomationcore, HRESULT, WINAPI,
    UiaRaiseNotificationEvent,
    (IRawElementProviderSimple* pProvider, enum NotificationKind notificationKind, enum NotificationProcessing notificationProcessing, BSTR displayString, BSTR activityId))
{
    CSpyMsg_Event evt;
    if (evt.CheckUIA(pProvider, 20035, VA2T(L", notificationKind:%d", notificationKind)))  // NotificationEventId
        return S_OK;
    return _UiaRaiseNotificationEvent_Org(pProvider, notificationKind, notificationProcessing, displayString, activityId);
};

DECLARE_MINHOOK_FUNC(
    dll_Uiautomationcore, HRESULT, WINAPI,
    UiaRaiseActiveTextPositionChangedEvent,
    (IRawElementProviderSimple* pProvider, ITextRangeProvider* textRange))
{
    CSpyMsg_Event evt;
    if (evt.CheckUIA(pProvider, 20036, 0))  // ActiveTextPositionChangedEventId
        return S_OK;
    return _UiaRaiseActiveTextPositionChangedEvent_Org(pProvider, textRange);
}


void CHookEventFunc::SetHook()
{
    _NotifyWinEvent_Install();
    _UiaRaiseAutomationPropertyChangedEvent_Install();
    _UiaRaiseAutomationEvent_Install();
    _UiaRaiseStructureChangedEvent_Install();
    _UiaRaiseAsyncContentLoadedEvent_Install();
    _UiaRaiseTextEditTextChangedEvent_Install();
    _UiaRaiseChangesEvent_Install();
    _UiaRaiseNotificationEvent_Install();
    _UiaRaiseActiveTextPositionChangedEvent_Install();
}

void CHookEventFunc::RemoveHook()
{
    _NotifyWinEvent_Uninstall();
    _UiaRaiseAutomationPropertyChangedEvent_Uninstall();
    _UiaRaiseAutomationEvent_Uninstall();
    _UiaRaiseStructureChangedEvent_Uninstall();
    _UiaRaiseAsyncContentLoadedEvent_Uninstall();
    _UiaRaiseTextEditTextChangedEvent_Uninstall();
    _UiaRaiseChangesEvent_Uninstall();
    _UiaRaiseNotificationEvent_Uninstall();
    _UiaRaiseActiveTextPositionChangedEvent_Uninstall();
}


// static HTUI _CDlgBlockEvent_AddItem(CTuiWidget p, DWORD event, TMCStr str)
// {
//     CTuiWidget wc;
//     wc.Create(p, IDC_CHECKBOX_30425, p.GetModule());
//     wc.ShowWindow(true);
//     wc.SetDlgCtrlID(event);
//     wc.SetWindowText(str);
//     return wc;
// }
// 
// BOOL CDlgBlockEvent::FilterEvent(DWORD event)
// {
//     CTuiWidget wndP(Item(IDC_WINDOW_30427));
//     CTuiCheckBox wnd(wndP.Item(event + EVENT_OFFSET));
//     if (!wnd)
//     {
//         CString str;
//         switch(event)
//         {
//     #   define EVENT_DEFW(n,m) case n: str = TM_CS(_T(#m)); break;
//     #   define EVENT_DEF2(n,m) case n: str = TM_CS(_T(#m) _T("2")); break;
//     #   define EVENT_DEFU(n,m) case n: str = TM_CS(_T(#m) _T("U")); break;
// 
//     #   include "ataccevent_i.h"
//         default: str.Format(L"%X", event);
//         }
//         wnd = _CDlgBlockEvent_AddItem(wndP, event + EVENT_OFFSET, str);
//     }
//     return wnd.GetCheck();
// }
