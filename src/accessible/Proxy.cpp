/**
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.08.25
 *
 *  Copyright (C) 2009-2018 miragekiller
 */

#include "stdafx.h"
#include "Proxy.h"
#include "accPushButton.h"
#include "accStatic.h"
#include "accEdit.h"
#include "accRadioButton.h"
#include "accListBox.h"
#include "accCheckBox.h"
#include "accMgr.h"
#include "accTreeView.h"
#include "accComboBox.h"
#include "accWidget.h"
#include "accTabButton.h"
#include "accMenuBar.h"
#include "accProgressBar.h"
#include "accDummy.h"
#include "accMenuItem.h"
#include "accSplitter.h"
//#include "Caret.h"
#include "accScrollBar.h"
#include "Speak.h"
#include "Hash.h"
#include "JawsHook.h"

//#define m_accSpeaker g_accSpeak

#pragma comment(lib, "Oleacc.lib")
// #pragma comment(lib, "ole32.lib")

using namespace TM;
using namespace TM::PRI;

enum EChange
{
    eChangeState    = 0x01,
    eChangeVal      = 0x02,
    eChangeName     = 0x04,
    eChangeText     = 0x08,
//    eChangeTxtSel   = 0x10,
};
TM_ENUM_2_FLAG(EChange);

CString _acc_dinfo3(CAccessibleObj& wnd)
{
    CString str;
    if (wnd)
    {
        CString strc;
        wnd.GetClassName(strc);
        if (wnd.m_ptr == &g_accCombListBox || wnd.m_ptr == &g_accCombTreeView)
            str.Format(L"this:%p, HTUI:%p(%d-cb_%s-%X), HWND:%p", &wnd, wnd.m_hWnd, wnd.GetDlgCtrlObjectID(), strc.c_str(), wnd.m_nHash, wnd.GetHWND());
        else
            str.Format(L"this:%p, HTUI:%p(%d-%s-%X), HWND:%p", &wnd, wnd.m_hWnd, wnd.GetDlgCtrlObjectID(), strc.c_str(), wnd.m_nHash, wnd.GetHWND());
    }
    else
        str.Format(L"this:%p, HTUI:0(%?-%?-%X)", &wnd, wnd.m_nHash);
    return str;
}

CString _acc_dinfo5(HTUI hWnd)
{
    CString str;
    if (!hWnd)
        return L"0";
    CTuiWidget wnd(hWnd);
    _HashObject hp = CHash::Find(wnd);
    if (hp)
        return _acc_dinfo3(*hp);
    else
    {
        CString strc;
        wnd.GetClassName(strc);
        str.Format(L"%p(%d-%s)", wnd.m_hWnd, wnd.GetDlgCtrlObjectID(), strc.c_str());
    }
    return str;
}

CString _acc_dinfo(CAccessibleWnd& hWnd)
{
    return _acc_dinfo3(static_cast<CAccessibleObj&>(hWnd));
}

CString _acc_dinfo2(HTUI hWnd)
{
    if (!hWnd)
        return L"0";
    CTuiWidget wnd(hWnd);
    _HashObject hp = CHash::Find(wnd);
    if (hp)
        return _acc_dinfo3(*hp);

    CString str, strc;
    wnd.GetClassName(strc);
    str.Format(L"HWND:%p, HTUI:%p(%d-%s)", wnd.GetHWND(), wnd.m_hWnd, wnd.GetDlgCtrlObjectID(), strc.c_str());
    return str;
}



//LPCWSTR g_SliderValueMap =
//L"A:0" // always 0 for slider maps 
//L":0:Cold"
//L":1:Cool"
//L":2:Warm"
//L":3:Toasty"
//L":4:Hot"
//L":";
//
//
//class CAccPropServer : public IAccPropServer
//{
//private:
//    CComPtr<IAccPropServices> m_accPropServices;
//    //HWND m_hWndTrackBar;
//    //CWindow m_hWndTrackBar;
//    long m_hashTrakBar;
//
//public:
//    TM_UNUSED_ADDREF_RELEASE();
//    TM_BGN_INTERFACE_MAP(IAccPropServer)
//        ACC_TRACE(L"CAccPropServer::QueryInterface(%s)", DbgFormatGuid(iid).c_str());
//        TM_INTERFACE_HANDLER(IAccPropServer)
//    TM_END_INTERFACE_MAP()
//
//    virtual HRESULT STDMETHODCALLTYPE GetPropValue(
//        const BYTE* pIDString,
//        DWORD dwIDStringLen,
//        MSAAPROPID idProp,
//        VARIANT* pvarValue,
//        BOOL* pfHasProp)
//    {
//        //HWND hWnd = 0;
//        //DWORD pidObject = 0;
//        //DWORD pidChild = 0;
//        //DbgVerifyHResult(m_accPropServices->DecomposeHwndIdentityString(pIDString, dwIDStringLen, &hWnd, &pidObject, &pidChild));
//        CAccessibleObj* obj = CHash::Find(m_hashTrakBar);
//        if (!obj)
//        {
//            ACC_TRACE(L"CAccPropServer::GetPropValue() RPC_E_DISCONNECTED");
//            return RPC_E_DISCONNECTED;
//        }
//        HRESULT hr = 0;
//        if (idProp == PROPID_ACC_STATE)
//        {
//            hr = obj->m_ptr->_get_accState(*obj, 0, pvarValue->
//            pvarValue->vt = VT_I4;
//            ACC_TRACE(L"CAccPropServer::GetPropValue_State(%s) %s", DbgFormatState(pvarValue->lVal), _acc_dinfo(*obj).c_str());
//        }
//        //else if (idProp == PROPID_ACC_VALUE)
//        //{
//        //    static int n = 0;
//        //    ++n;
//        //    pvarValue->lVal = n;
//        //    pvarValue->vt = VT_I4;
//        //    ACC_TRACE(L"CAccPropServer::GetPropValue_Value(%d) %s", n, _acc_dinfo(*obj).c_str());
//        //}
//        else
//        {
//            CComBSTR bstr;
//            if (idProp == PROPID_ACC_NAME)
//            {
//                hr = obj->m_ptr->_get_accName(*obj, 0, bstr);
//                ACC_TRACE(L"CAccPropServer::GetPropValue_Name(%s) %s", (BSTR)bstr, _acc_dinfo(*obj).c_str());
//            }
//            else if (idProp == PROPID_ACC_VALUE)
//            {
//                //static wchar_t n[2] = L"0";
//                //++n[0];
//                //bstr = n;
//                hr = obj->m_ptr->_get_accValue(*obj, 0, bstr);
//                ACC_TRACE(L"CAccPropServer::GetPropValue_Value(%s) %s", (BSTR)bstr, _acc_dinfo(*obj).c_str());
//            }
//            else if (idProp == PROPID_ACC_VALUEMAP)
//            {
//                bstr = g_SliderValueMap;
//                ACC_TRACE(L"CAccPropServer::GetPropValue_ValueMap(%s) %s", (BSTR)bstr, _acc_dinfo(*obj).c_str());
//            }
//            else
//                return E_FAIL;
//            pvarValue->vt = VT_BSTR;
//            pvarValue->bstrVal = bstr.Detach();
//        }
//        *pfHasProp = true;
//        return hr;
//    }
//
//    CAccPropServer()
//    {
//    }
//
//    void RegisterT(HWND hWnd, DWORD idObject)
//    {
//        BYTE* p = 0;
//        DWORD n;
//        DbgVerifyHResult(m_accPropServices->ComposeHwndIdentityString(hWnd, idObject, 0, &p, &n));
//        MSAAPROPID mid[] = { PROPID_ACC_NAME, PROPID_ACC_VALUE, PROPID_ACC_STATE, PROPID_ACC_VALUEMAP };
//        DbgVerifyHResult(m_accPropServices->SetPropServer(p, n, mid, tm_countof(mid), this, ANNO_CONTAINER));
//        CoTaskMemFree(p);
//    }
//
//    HRESULT MakeID(CAccessibleObj* obj, BYTE** ppIDString, DWORD* pdwIDStringLen)
//    {
//        if (!m_accPropServices)
//            return E_NOTIMPL;
//        return m_accPropServices->ComposeHwndIdentityString(obj->GetHWND(), obj->m_nHash, 0, ppIDString, pdwIDStringLen);
//    }
//
////    void NotifyEvent_SliderValueChange(HWND hWnd, DWORD nHash, DWORD event)
//    void Register(HWND hWnd, DWORD nHash)
//    {
//        if (!m_accPropServices)
//        {
//            MIDL_INTERFACE("b5f8350b-0548-48b1-a6ee-88bd00b4a5e7") _CLSID_AccPropServices;
//            m_accPropServices.CoCreateInstance(_uuidof(_CLSID_AccPropServices), NULL, CLSCTX_SERVER);
//
//            //WNDCLASSEX wcs = { sizeof(WNDCLASSEX) };
//            //GetClassInfoEx(0, L"msctls_trackbar32", &wcs);
//            //wcs.lpszClassName = L"xxxxxxx";
//            //RegisterClassEx(&wcs);
//            //m_hWndTrackBar = CreateWindow(wcs.lpszClassName, L"", WS_POPUPWINDOW | WS_VISIBLE, 0, 0, 100, 100, 0, 0, 0, 0);
//            //RegisterT(m_hWndTrackBar, OBJID_CLIENT);
//            //RegisterT(m_hWndTrackBar, 0);
//        }
//
//        m_hashTrakBar = nHash;
//        RegisterT(hWnd, nHash);
//    //AccNotifyWinEvent(event, hWnd, OBJID_CLIENT, 0);
//    }
//
//    static void AccNotifyWinEvent(DWORD event, HWND hWnd, LONG idObject, LONG idChild)
//    {
//        ::NotifyWinEvent(event, hWnd, idObject, idChild);
//    }
//};
//static CAccPropServer s_servers;

///////////////////////////////////////////////////////////////////////////////
// CAccNotify
class CAccNotify
{
private:
    CRefPtr<CAccessibleObj> m_obj;
    long m_idChild;

    static void AccNotifyWinEvent(HWND hWnd, HTUI wnd, DWORD event, LONG idObject, LONG idChild)
    {
        DbgTrace(L"AccNotifyEvent begin(event:%s, type:%x idchild:%x, wnd:%s)", DbgFormatAccEvent(event), idObject, idChild, _acc_dinfo2(wnd).c_str());
        ::NotifyWinEvent(event, hWnd, idObject, idChild);
        DbgTrace(L"AccNotifyEvent end(event:%s, type:%x, idchild:%x, wnd:%s)", DbgFormatAccEvent(event), idObject, idChild, _acc_dinfo2(wnd).c_str());
    }

    void NotifyEvent(DWORD event) const
    {
        DbgAssert(m_obj);
        _HashInt id = m_obj->IsMgr() ? OBJID_CLIENT : m_obj->m_nHash;
        HWND hWnd;
        if (m_obj->m_ptr == &g_accCombListBox || m_obj->m_ptr == &g_accCombTreeView)
        {
            CTuiWidget wnd(CTuiComboUnit(*m_obj).GetEdit());
            if (wnd)
                hWnd = wnd.GetHWND();
            else
                return;
        }
        else
            hWnd = m_obj->GetHWND();
        AccNotifyWinEvent(hWnd, *m_obj, event, id, m_idChild);
    }

//     static CAccNotify Find(HTUI hWgt, LONG idChild = CHILDID_SELF)
//     {
//         return CAccNotify(CHash::Find(hWgt), idChild);
//     }

    CAccNotify(CAccessibleObj* obj, LONG idChild = CHILDID_SELF)
    {
        m_obj = obj;
        m_idChild = idChild;
    }

public:
    CAccNotify(HTUI hWgt, LONG idChild = CHILDID_SELF)
    {
        m_obj.Attach(CAccessibleObj::MakeAccClient(hWgt));
//         int n1 = m_obj->AddRef();
//         m_obj->Release();
//         DbgTrace("CAccNotify:%d", n1);
        m_idChild = idChild;
    }

    CAccessibleObj* operator->() const { return m_obj; }

    bool IsEditOrCombKey(int nKey) const
    {
        CString str;
        CTuiWidget(*m_obj).GetClassName(str);
        if (str == TUI_CLASS_RICH || str == TUI_CLASS_MEMO)
        {
            if ((nKey == VK_UP || nKey == VK_DOWN) &&
                (m_obj->m_ptr->_get_accRole(*m_obj, CHILDID_SELF) != ROLE_SYSTEM_COMBOBOX))
                return true;
        }
        else if (str == TUI_CLASS_EDIT)
            ;
        else
            return false;

        return nKey == VK_BACK || nKey == VK_DELETE || nKey == VK_LEFT || nKey == VK_RIGHT;
    }

    long IsListFocus(HTUI hFocus) // return role
    {
        DbgAssert(m_obj);
        if (m_obj->m_ptr == &g_accCombListBox || m_obj->m_ptr == &g_accCombTreeView)
            return CAccessibleObj::CombUtilIsFocus(*m_obj, eCombList, hFocus) ? ROLE_SYSTEM_COMBOBOX : 0;
        if (m_obj->m_ptr == &g_accListBox || m_obj->m_ptr == &g_accTreeView)
            return m_obj->IsSelfOrChild(hFocus) ? ROLE_SYSTEM_LIST : 0;
        return 0;
    }

    bool CheckComb()
    {
        DbgAssert(m_obj);
        if (m_obj->m_ptr == &g_accCombButton)
        {
            *this = CAccNotify(m_obj->GetParent(), CAccComboBox::eIndexButton);
            return true;
        }
        //else if (m_obj->m_ptr->_get_accRole(*m_obj, m_idChild) == ROLE_SYSTEM_COMBOBOX)
        //{
        //    m_idChild = CAccComboBox::eIndexEdit;
        //    return true;
        //}
        return false;
    }

    CAccNotify& CheckSel()
    {
        DbgAssert(m_obj);
        DbgAssert(m_idChild == 0);
        if (m_obj->m_ptr == &g_accListBox || m_obj->m_ptr == &g_accCombListBox)
        {
            CTuiListBox listbox(*m_obj);
            m_idChild = listbox.GetCurSel() + 1;
            ACC_TRACE(L"LBN_SELCHANGE:%x, index:%d", listbox.GetHWND(), m_idChild);
        }
        else if (m_obj->m_ptr == &g_accTreeView || m_obj->m_ptr == &g_accCombTreeView)
        {
            CTuiTree tree(*m_obj);
            CTVItem Item = tree.GetSelectedItem();
            if (Item)
            {
                ACC_TRACE(L"TVN_SELCHANGE:%x, index:%d", tree.GetHWND(), m_idChild);
                m_idChild = CAccTreeView::GetIndexByItem(Item);
            }
        }
        else if (m_obj->m_ptr == &g_accMenuBar)
        {
            CTuiMenuBar menuBar(*m_obj);
            m_idChild = menuBar.GetCurSel() + 1;
        }
        return *this;
    }

    void NotifyEvent_FOCUS()
    {
        if (!CheckComb())
            CheckSel();
        ACC_TRACE(L"CAccNotify::NotifyEvent_FOCUS:%x, index:%d", m_obj->m_hWnd, m_idChild);
        this->NotifyEvent(EVENT_OBJECT_FOCUS);
        if (m_idChild)
            this->NotifyEvent(EVENT_OBJECT_SELECTION);
    }

    void NotifyEvent_MenuBar(TMUInt nNotifyCode)
    {
        if (m_obj->m_ptr == &g_accMenuBar)
            NotifyEvent(nNotifyCode == BN_SETFOCUS ? EVENT_SYSTEM_MENUSTART : EVENT_SYSTEM_MENUEND);
    }

    void NotifyEvent_EditSelChange()
    {
        if (m_obj->m_ptr->_get_accRole(*m_obj, 0) == ROLE_SYSTEM_COMBOBOX)
            return;
        NotifyEvent(EVENT_OBJECT_TEXTSELECTIONCHANGED);
        NotifyEvent(IA2_EVENT_TEXT_CARET_MOVED);
//            AccNotifyEvent(EVENT_OBJECT_TEXTSELECTIONCHANGED, hWnd);
        AccNotifyWinEvent(m_obj->GetHWND(), *m_obj, EVENT_OBJECT_LOCATIONCHANGE, OBJID_CARET, CHILDID_SELF);
//        AccNotifyWinEvent(m_obj->GetHWND(), *m_obj, EVENT_OBJECT_SHOW, OBJID_CARET, CHILDID_SELF);
        //       AccNotifyEvent(IA2_EVENT_TEXT_CARET_MOVED, hWnd);
    }

    template <class T_Map> static void NotifyEvent_NameValueState(T_Map& aWnd, HTUI hFocus)
    {
        T_Map aChange;
        aChange.swap(aWnd);

        for (typename T_Map::recorder rec(aChange); rec; ++rec)
        {
            EChange flag = rec.get_data();
            CTuiWidget wnd(rec.get_key());
            CAccNotify an(wnd);
            if (!an.m_obj)
                continue;

            if (flag & eChangeState)
            {
                if (an.m_obj->m_ptr == &g_accCombButton)
                {
                    CAccNotify(wnd.GetParent()).NotifyEvent(EVENT_OBJECT_STATECHANGE);
                    return;
                };
                if (wnd.IsSelfOrChild(hFocus))
                    an.CheckSel().NotifyEvent(EVENT_OBJECT_STATECHANGE);
            }
            if (flag & eChangeText)
                flag = (an.m_obj->m_ptr == &g_accEdit) ? eChangeVal : eChangeName;

            if (flag & eChangeName)
                an.NotifyEvent(EVENT_OBJECT_NAMECHANGE);
            if (flag & eChangeVal)
            {
                an.NotifyEvent(EVENT_OBJECT_VALUECHANGE);
                //if (an.m_obj->m_ptr == &g_accTrackBar) // JAWS not read slider.
                //{
                //    CComBSTR bstr;
                //    g_accTrackBar._get_accValue(wnd, 0, bstr);
                //    g_accFilter.SayString(bstr, eSpeakDevJAWS);
                //}
            }

            //if (flag & eChangeTxtSel)
            //    an.NotifyEvent_EditSelChange();
        }
    }

    static TMBool NotifyEvent_Destroy(HTUI wnd)
    {
        _HashObject hp = CHash::Find(wnd);
        if (!hp)
            return false;
        CAccNotify(hp).NotifyEvent(EVENT_OBJECT_DESTROY);
        return true;
    }
};


template <class T>
class CAccTimer : public CAniImpl<T>
{
private:
    enum { eNotifyFocusDelay = 300 };

public:
    void OnAni(TMHAni hAni, ...)
    {
        g_bDisableJawsHook = true;
        this->KillAni();
        (static_cast<T*>(this))->DoNotify();
    }

public:
    void SetNotifyTimer() { if (!*this) this->SetAni(1); }
    void SetNotifyTimer_Delay() { this->KillAni(); this->SetAni(eNotifyFocusDelay); }
};


///////////////////////////////////////////////////////////////////////////////
// CAccFoucsTrack
template <class T_Cont>
class CAccFoucsTrack : public CAccTimer<T_Cont>
{
protected:
    // create dialog:               notify focus after 500ms
    // normal set focus:            notify focus next cycle
    // list,tree::set select:       notify select next cycle
    // list,tree::bind set focus:   notify select next cycle; notify focus after 500ms
    typedef CSet<HTUI>              CWndSet;
    typedef CMap<HTUI, EChange>     CWndMap;
    CWndSet m_aChangeSel;
    CWndMap m_aChangeOther;
    HTUI    m_hFocus;
    UINT    WM_JHOOKLDR;
    bool    m_bLoadJAWS;

public:
    CAccFoucsTrack()
        : WM_JHOOKLDR(RegisterWindowMessage(TM_T("JHOOKLDR")))
        , m_bLoadJAWS(true)
    {
    }

    // message process
    void OnSetChangeSel(HTUI hWnd)
    {
        if (!m_aChangeSel.size())
            this->SetNotifyTimer();
        m_aChangeSel.insert(hWnd);
    }

    void OnSetChangeOther(HTUI hWnd, EChange eChange)
    {
        if (!m_aChangeOther.size())
            this->SetNotifyTimer();
        m_aChangeOther[hWnd] |= eChange;
    }

    void OnSetChangeFocus(HTUI hWnd)
    {
        if (!m_hFocus)
            this->SetNotifyTimer();
        m_hFocus = hWnd;
    }

    void OnSetChangeActivate()
    {
        g_bDisableJawsHook = false;
        m_hFocus = _tuiApp.GetFocus();
        this->SetNotifyTimer_Delay();
    }

    void OnSetLoadJAWS()
    {
        m_bLoadJAWS = true;
    }

public:
    void OnDestroy(HTUI hWnd)
    {
        m_aChangeSel.erase(hWnd);
        m_aChangeOther.erase(hWnd);
        if (CAccNotify::NotifyEvent_Destroy(hWnd))
            CHash::Erase(hWnd);
    }

    void DoJawsHookInstall()
    {
        if (m_bLoadJAWS)
        {
            JawsHookInstall();
            m_bLoadJAWS = false;
        }
    }

    void DoNotify()
    {
        HTUI hFocus = _tuiApp.GetFocus();
        if (m_hFocus != hFocus)
            m_hFocus = 0;
        bool bDelay = false;
        ACC_TRACE(L"CAccFoucsTrack::DoNotify() m_hFocus:%s", _acc_dinfo2(m_hFocus).c_str());

#ifdef TM_DBG
        tm_for(CWndSet::recorder rec(m_aChangeSel); rec; ++rec)
            ACC_TRACE(L"CAccFoucsTrack::DoNotify() ChangeSel:%s", _acc_dinfo5(rec.get_data()).c_str());

        tm_for(CWndMap::recorder rec(m_aChangeOther); rec; ++rec)
            ACC_TRACE(L"CAccFoucsTrack::DoNotify() ChangeOther:%s; %d", _acc_dinfo5(rec.get_key()).c_str(), rec.get_data());
#endif

        DoJawsHookInstall();

        // selection
        if (m_aChangeSel.size())
        {
            CWndSet aSelChange;
            aSelChange.swap(m_aChangeSel);
            for (CWndSet::recorder rec(aSelChange); rec; ++rec)
            {
                CTuiWidget wnd(rec.get_data());
                if (wnd == m_hFocus) // need set focus, so need't notify selection.
                    break;
                CAccNotify an(wnd);
                long bf = an.IsListFocus(hFocus);
                if (bf) // focus is bind window, so need notify selection and delay notify focus.
                {
                    if (bf == ROLE_SYSTEM_COMBOBOX)
                        m_hFocus = 0; // comb::list need't notify focus.
    //                NotifySel(wnd, LBN_SELCHANGE);
                    an.NotifyEvent_FOCUS();
                    //an.NotifyEvent_SELECTION();
                    bDelay = true;
                    m_aChangeOther.clear();
                    break;
                }
            }
        }

        // focus
        if (m_hFocus)
        {
            DbgAssert(m_hFocus == hFocus);
            m_aChangeOther.clear();
            if (bDelay)
                this->SetNotifyTimer_Delay();
            else
            {
                m_hFocus = 0;
                CAccNotify(hFocus).NotifyEvent_FOCUS();
            }
            //FocusEvent_Focus(hFocus, bDelay);
            return;
        }

        // name, val, state
        CAccNotify::NotifyEvent_NameValueState(m_aChangeOther, hFocus);
    }
};

///////////////////////////////////////////////////////////////////////////////
// CAccMsgFilter
class CAccMsgFilter
    : public CTuiMsgFilter
    , public CAccFoucsTrack<CAccMsgFilter>
    , public CAccSpeak
{
private:
    typedef CMap<CTuiApp*, bool> CAppList;
    CAppList    m_aApp;

    virtual BOOL PreTranslateMessage(TUIMSG* pMsg, TMResult& lResult) tm_final;

    void PrecessKeyDown(HTUI hWnd, int nKey, LPARAM lParam);

public:
    void RegisterMessageFilter(CTuiApp* app)
    {
        bool& b = m_aApp[app];
        if (b)
            return;
        b = true;
        app->AddMessageFilter(this);
    }
};
CAccMsgFilter g_accFilter;



STDAPI DllGetClassObject(
    REFCLSID rclsid,  //CLSID for the class object
    REFIID riid,      //Reference to the identifier of the interface
    // that communicates with the class object
    LPVOID * ppv      //Address of output variable that receives the
    // interface pointer requested in riid
    )
{
    return E_FAIL;
}

STDAPI DllCanUnloadNow()
{
    return CHash::IsClean() ? S_OK : S_FALSE;
}

void GetAccStyleByClassName(LPCTSTR strObjType, IAccessibleStyle** ppStyle)
{
    if (!StrCompare(strObjType, TUI_CLASS_PUSHBUTTON))
        *ppStyle = &g_accPushButton;
    else if(!StrCompare(strObjType, TUI_CLASS_COMBOBUTTON))
        *ppStyle = &g_accCombButton;
    else if (!StrCompare(strObjType, TUI_CLASS_STATIC))
        *ppStyle = &g_accStatic;
    else if (!StrCompare(strObjType, TUI_CLASS_GROUPBOX))
        *ppStyle = &g_accGroup;
    else if (!StrCompare(strObjType, TUI_CLASS_EDIT) || !StrCompare(strObjType, TUI_CLASS_MEMO) || !StrCompare(strObjType, TUI_CLASS_RICH))
        *ppStyle = &g_accEdit;
    else if (!StrCompare(strObjType, TUI_CLASS_RADIOBUTTON))
        *ppStyle = &g_accRadioButton;
    else if (!StrCompare(strObjType, TUI_CLASS_TABBUTTON))
        *ppStyle = &g_accPageTab;
    else if (!StrCompare(strObjType, TUI_CLASS_LISTBOX))
        *ppStyle = &g_accListBox;
    else if (!StrCompare(strObjType, TUI_CLASS_CHECKBOX))
        *ppStyle = &g_accCheckBox;
    else if (!StrCompare(strObjType, TUI_CLASS_WINDOW))
        *ppStyle = &g_accMgr;
    else if (!StrCompare(strObjType, TUI_CLASS_TREE))
        *ppStyle = &g_accTreeView;
    else if (!StrCompare(strObjType, TUI_CLASS_MENUBAR))
        *ppStyle = &g_accMenuBar;
    else if (!StrCompare(strObjType, TUI_CLASS_TRACKBAR))
        *ppStyle = &g_accTrackBar;
    else if (!StrCompare(strObjType, TUI_CLASS_PROGRESS))
        *ppStyle = &g_accProgress;
    else if (!StrCompare(strObjType, TUI_CLASS_DUMMY))
        *ppStyle = &g_accDummy;
    else if (!StrCompare(strObjType, TUI_CLASS_MENUITEM))
        *ppStyle = &g_accMenuItem;
    else if (!StrCompare(strObjType, TUI_CLASS_SPLITTER))
        *ppStyle = &g_accSplitter;
    else if (!StrCompare(strObjType, TUI_CLASS_FLOAT))
        *ppStyle = &g_accWidget;
//     else if (!StrCompare(strObjType, TUI_CLASS_UPDOWN))
//         *ppStyle = &g_accSpin;
    else
        *ppStyle = 0;
}

void CAccMsgFilter::PrecessKeyDown(HTUI hWnd, int wParam, LPARAM lParam)
{
    CTuiEdit wgtWnd(hWnd);
    TCHAR szWindowText[1024] = { 0 };

    CTuiWidget wgtControl(hWnd);
    wgtControl.GetWindowText(szWindowText, 1023);
    CString strText;
    strText = szWindowText;

    TMSel0 nStart = -1, nEnd = -1;
    wgtWnd.GetSel(nStart, nEnd);

//    ACC_TRACE(L"CAccMsgFilter::PreTranslateMessage: nNotifyCode:%d, strText:%s, strText.length():%d, nStart:%d, nEnd:%d, nLine:%d, szLine:%s", wParam, strText.c_str(), strText.length(), nStart, nEnd, nLine, szLine);

    if (nStart > nEnd)
        Swap(nStart, nEnd);

    if (nStart < 0 || nEnd < 0 || strText.length() < nEnd)
        return;

    if (wParam == VK_BACK)
    {
        nStart = nEnd - 1;
        if (nStart < 0)
            return;
    }

    if (nStart != nEnd)
    {
        CString strSelected = strText.Mid(nStart, nEnd - nStart);
        this->SayString(strSelected, eSpeakAllOld);
        //AccNotifyAlert(hWnd, strSelected.c_str(), TRUE);
        DbgTrace(L"CAccMsgFilter::PreTranslateMessage: strSelected:%s", strSelected.c_str());
    }
    else if (wParam == VK_UP || wParam == VK_DOWN)
    {
        int nLine = wgtWnd.LineFromChar(nStart);
        TCHAR szLine[1024] = { 0 };
        wgtWnd.GetLine(nLine, szLine, 1024);
        this->SayString(szLine, eSpeakAllOld);
        //AccNotifyAlert(hWnd, szLine, FALSE);
        DbgTrace(L"CAccMsgFilter::PreTranslateMessage: VK_UP/VK_DOWN, szLine:%s", szLine);
    }
    //else if (wParam == VK_BACK)
    //{
    //    CAccNotify(hWnd).NotifyEvent(IA2_EVENT_TEXT_REMOVED);
    //}
    else
    {
        CString strChar = strText.Mid(nStart, 1);
        if (strChar == _T(" ") || strChar == _T("\t"))
            strChar = "space";
        else
        {
            TMUInt16 mdf = CKeyInput(wParam, lParam).GetModifiersFast();
            if (wParam == VK_DELETE)
            {
                if (mdf)
                    return;
            }
            else if (mdf == MOD_CONTROL)
            {
                int n = strText.FindF(StrIsSpace, nStart);
                if (n == -1)
                    n = strText.length();
                strChar = strText.substr(nStart, n - nStart);
            }
        }
        this->SayString(strChar, eSpeakAllOld);
        //AccNotifyAlert(hWnd, strChar.c_str(), FALSE);
        DbgTrace(L"CAccMsgFilter::PreTranslateMessage: VK_LEFT/VK_RIGHT, strChar:%s", strChar.c_str());
    }
}


BOOL CAccMsgFilter::PreTranslateMessage(TUIMSG* pMsg, TMResult& lResult)
{
    HTUI hWnd = pMsg->hWnd;
    TMUInt uMsg = pMsg->message;
    TMWParam wParam = pMsg->wParam;
    TMLParam lParam = pMsg->lParam;

    switch (uMsg)
    {
    case CM_APPMESSAGE:
    {
        TUIMSG* msg = (TUIMSG*)lParam;
        if (msg->message == WM_JHOOKLDR && msg->lParam)
            OnSetLoadJAWS();
    }
    break;

    case CM_ACC_POST_KEYDOWN:
        PrecessKeyDown(hWnd, wParam, lParam);
        break;

    case WM_KEYDOWN:
        if (CAccNotify(hWnd).IsEditOrCombKey(wParam))
        {
            if (wParam == VK_BACK)
                PrecessKeyDown(hWnd, wParam, lParam);
            else
                CTuiWidget(hWnd).PostMessage(CM_ACC_POST_KEYDOWN, wParam, lParam);
        }
        break;
   
    case WM_SETFOCUS:
        this->OnSetChangeFocus(hWnd);
        break;

    case WM_NCACTIVATE:
        if (wParam)
            this->OnSetChangeActivate();
        break;

    case CM_SETACCTEXT:
    case CM_SETHINT:
        this->OnSetChangeOther(hWnd, eChangeName);
        break;

    case WM_SETTEXT:
    case CM_SETTEXT:
        this->OnSetChangeOther(hWnd, eChangeText);
        break;

    case CM_ACC_ALERT_EVENT:
        {
            CString strAlert;
            CTuiWidget(hWnd).GetAccText(strAlert);
            if (!strAlert.IsEmpty())
                this->SayString(strAlert);
        }
        break;

//     case CM_SUBCLASS:
//         if (!lParam)
    case WM_NCDESTROY:
        DbgTrace(L"CAccMsgFilter::PreTranslateMessage(WM_NCDESTROY), wnd:%s", _acc_dinfo2(hWnd).c_str());
        this->OnDestroy(hWnd);
        break;

    case CBM_STATECHANGED: // static,button->listbox
        OnSetChangeOther(hWnd, eChangeState);
        break;

    case CM_PRECOMMAND:
//      Message Source  wParam(high word)                   wParam(low word)                    lParam
//      Menu            0                                   Menu identifier(IDM_*)              0
//      Accelerator     1                                   Accelerator identifier(IDM_*)       0
//      Control         Control defined notification code   Control identifier                  Handle to the control window
        if (!lParam)
            break;

        {
            TMUInt nNotifyCode = HIWORD(wParam);
            ACC_TRACE(L"CM_PRECOMMAND:%x, %x", nNotifyCode, lParam);

            switch (nNotifyCode)
            {
//             case BN_CLICKED:

            case SB_ENDSCROLL:      // track bar, up down, progress
//             case EN_CHANGE:         // edit
            case CN_VALUECHANGE:    // track bar, up down, progress, edit
                this->OnSetChangeOther(hWnd, eChangeVal);
//                CAccNotify::AccNotifyEvent(EVENT_OBJECT_VALUECHANGE, hWnd);
                break;

            //case CBN_SELENDOK:
            //    {
            //        CTuiComboUnit wnd(hWnd);
            //        hWnd = wnd.GetEdit();
            //        if (hWnd)
            //            CAccNotify::AccNotifyEvent(EVENT_OBJECT_FOCUS, hWnd);
            //    }
            //    break;

            //case EN_SELCHANGE: // edit
            //    this->OnSetChangeOther(hWnd, eChangeTxtSel);
            //    break;

            case LBN_SELCHANGE: // TVN_SELCHANGE, CBN_SELCHANGE
                this->OnSetChangeSel(hWnd);
                break;

            case TVN_STATECHANGE: // BN_HILITE
            //case CBN_DROPDOWN:
            //case CBN_CLOSEUP:
                this->OnSetChangeOther(hWnd, eChangeState);
                break;

            case BN_SETFOCUS:
            case BN_KILLFOCUS:
                CAccNotify(hWnd).NotifyEvent_MenuBar(nNotifyCode);
                break;
            }
        }
        break;

    case WM_SYSCOMMAND:
        if (wParam == SC_MOUSEMENU)
            CAccNotify(hWnd, OBJID_MENU).NotifyEvent_MenuBar(BN_SETFOCUS);
        break;
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////
// CAccessibleStyle
long CAccessibleStyle::_get_accCtrlCount(CAccessibleWnd& sender)
{
    long nCur = 0;
    CTuiWidget hWin = sender.GetWindow(GW_CHILD);
    while (hWin)
    {
        if (!(hWin.GetBindState(sender) & eBindStateSelfMask))
            nCur++;
        hWin = hWin.GetWindow(GW_HWNDNEXT);
    }
    return nCur;
//    return CAccessibleObj::GetChildCount(sender);
}

long CAccessibleStyle::_get_accItem(CAccessibleWnd& sender, long nIndex, IDispatch** ppRet)
{
    DbgAssert(nIndex != CHILDID_SELF);
    int nCount =  _get_accItemCount(sender);
    if (nIndex > nCount)
        return nCount;
    *ppRet = 0;
    return -1;
};

// HTUI CAccessibleStyle::_get_accParent(CAccessibleWnd& sender)
// {
//     return sender.GetParent();
// }

HRESULT CAccessibleStyle::_get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name)
{
    return CAccessibleObj::GetAccNameOrValue(sender, name, eGetCustom | eGetAccText | eGetHint | eGetWindowText);
}

HRESULT CAccessibleStyle::_get_accHelp(CAccessibleWnd& sender, long nIndex, CComBSTR& helpString)
{
    return S_FALSE;
//     if (nIndex != CHILDID_SELF)
//         return S_FALSE;
// 
//     CString strID;
// 
//     UINT nID = sender.GetDlgCtrlID();
//     if (nID)
//         strID.Format(_T("c%d"), nID);
//     else
//     {
//         nID = sender.GetDlgCtrlObjectID();
//         strID.Format(_T("o%d"), nID);
//     }
//     helpString = strID;
//     return S_OK;
}

HRESULT CAccessibleStyle::_get_accState(CAccessibleWnd& sender, long nIndex, long& state)
{
    if (nIndex != CHILDID_SELF)
        return S_FALSE;
    state = CAccessibleObj::GetState(sender);
    return S_OK;
}

HRESULT CAccessibleStyle::_get_accFocus(CAccessibleWnd& sender, VARIANT* pvarFocusChild)
{
    if (sender.IsFocus())
    {
        pvarFocusChild->vt = VT_I4;
        pvarFocusChild->lVal = CHILDID_SELF;
        return S_OK;
    }
    return S_FALSE;
}

HRESULT CAccessibleStyle::_accSelect(CAccessibleWnd& sender, long nIndex, long flagsSelect)
{
    if (flagsSelect != SELFLAG_TAKEFOCUS)
        return DISP_E_MEMBERNOTFOUND;

    sender.SetFocus();
    return S_OK;
}

HRESULT CAccessibleStyle::_accNavigate(CAccessibleWnd& sender, long navDir, long varStart, VARIANT* pvarEndUpAt)
{
    if (varStart != CHILDID_SELF)
        return S_FALSE;

    HTUI hTarget;
    switch (navDir)
    {
    case NAVDIR_FIRSTCHILD:
        hTarget = sender.GetWindow(GW_CHILD);
        break;

    case NAVDIR_LASTCHILD:
        hTarget = sender.GetWindow(GW_CHILD);
        if (hTarget)
            hTarget = sender.GetWindow(GW_HWNDLAST);
        break;

    case NAVDIR_NEXT:
    case NAVDIR_DOWN:
        hTarget = sender.GetWindow(GW_HWNDNEXT);
        break;

    case NAVDIR_PREVIOUS:
    case NAVDIR_UP:
        hTarget = sender.GetWindow(GW_HWNDPREV);
        break;

    default:
        return S_FALSE;
    }

    if (!hTarget)
        return S_FALSE;

    IDispatch* pChildAcc = CAccessibleObj::MakeAccClient(hTarget);
    DbgAssert(pChildAcc);
    pvarEndUpAt->vt = VT_DISPATCH;
    pvarEndUpAt->pdispVal = pChildAcc;
    return S_OK;
}

class CAccessible2TextNormal : public CAccessible2AggBaseT<CAccessible2TextNormal, CAccessible2TextT<CAccessible2TextNormal> >
{
};

HRESULT CAccessibleStyle::_QueryService(CAccessibleWnd& sender, REFIID riid, void** ppvObject)
{
    if (riid == TM_UUID_OF(IAccessibleText))
    {
        *ppvObject = static_cast<IAccessibleText*>(CAccessible2TextNormal::_Inst(sender));
        return S_OK;
    }
    return E_NOINTERFACE;
}


///////////////////////////////////////////////////////////////////////////////
// CAccessibleStyleButtonBase
HRESULT CAccessibleStyleButtonBase::_get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name)
{
    CString strText;
    //1.Get AccText first
    sender.GetAccText(strText);
    //2.Get hint text if acc text is empty
    if (strText.IsEmpty())
    {
        sender.GetHint(strText);
    }
    //3.Get window test if acc text and hint are empty
    if (strText.IsEmpty())
    {
        sender.GetWindowText(strText);
        strText.Replace(_T("&"), _T(""));
    }
    
    if (!strText.IsEmpty())
    {
        name = strText;
        return S_OK;
    }
    return S_FALSE;
}

HRESULT CAccessibleStyleButtonBase::_get_accState(CAccessibleWnd& sender, long nIndex, long& state)
{
    HRESULT hr = CAccessibleStyle::_get_accState(sender, nIndex, state);
    if (FAILED(hr))
        return hr;
    
    CTuiButton winCkBtn(sender);
    long role = sender.AccGetRole();
    int nCheck = winCkBtn.GetCheck();
    if (role == ROLE_SYSTEM_BUTTONMENU) // || this == &g_accCombButton)
        state |= (nCheck ? STATE_SYSTEM_EXPANDED : STATE_SYSTEM_COLLAPSED);
    else if (nCheck)
        state |= STATE_SYSTEM_CHECKED;

    if (!sender)
        return RPC_E_DISCONNECTED;

    if (!sender)
        return RPC_E_DISCONNECTED;
    EButStates nStates = winCkBtn.GetState();
    if (nStates & e_bssDown)
        state |= STATE_SYSTEM_PRESSED;
    if (nStates & e_bssDefault)
        state |= STATE_SYSTEM_DEFAULT;

//     TMBool bPressed = (nState & e_bssDown);
//     if (bPressed)
//         state |= STATE_SYSTEM_PRESSED;
// 
//     TMUInt32 dwStyle = sender.GetStyle();
//     TMBool bDefault  = nState (dwStyle & BS_DEFPUSHBUTTON);
//     if (bDefault)
//         state |= STATE_SYSTEM_DEFAULT;
    
    return S_OK;
}

// HRESULT CAccessibleStyleButtonBase::_get_accKeyboardShortcut(CAccessibleWnd& sender, long nIndex, CComBSTR& szKeyboardShortcut)
// {
//     return sender.GetAccKeyboardShortcut(szKeyboardShortcut);
// }

HRESULT CAccessibleStyleButtonBase::_accDoDefaultAction(CAccessibleWnd& sender, long nIndex)
{
    if (nIndex != CHILDID_SELF)
        return DISP_E_MEMBERNOTFOUND;
    
    if (!sender.IsWindowEnabled())
        return S_FALSE;
    
    CTuiButton(sender).Click();
    return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
// CAccessibleEnum
class CAccessibleEnum : public CEnumImplNBaseT<CAccessibleEnum, VARIANT, IEnumVARIANT>
{
public:
    CRefPtr<CAccessibleObj> m_ptrObj;

    CAccessibleObj& GetOwner() { return *m_ptrObj; }
    
    ~CAccessibleEnum()
    {
        ACC_TRACE(L"CAccessibleEnum::~CAccessibleEnum(), %s", _DINFO());
    }

    HRESULT CopyTo(int nPos, VARIANT& var)
    {
        DbgAssert(m_ptrObj);

        IDispatch* pDisp = 0;
        HRESULT hr = m_ptrObj->get_accChild(CComVariant(nPos + 1), &pDisp);
        if (pDisp)
        {
            var.pdispVal = pDisp;
            var.vt = VT_DISPATCH; 
            ACC_TRACE(L"CAccessibleEnum::CopyTo(nPos:%d, disp:%p), %s", nPos, pDisp, _DINFO());
        }
        else if (hr == S_FALSE)
        {
            var.lVal = nPos + 1;
            var.vt = VT_I4;
            ACC_TRACE(L"CAccessibleEnum::CopyTo(nPos:%d, index:%d), %s", nPos, var.lVal, _DINFO());
        }
        else
            return E_FAIL;

        return S_OK;
    }

    int GetCount()
    {
        DbgAssert(m_ptrObj);
        long n = 0;
        m_ptrObj->get_accChildCount(&n);
        return n;
    }
};


///////////////////////////////////////////////////////////////////////////////
// CAccessibleObj
CAccessibleObj::~CAccessibleObj()
{
    ACC_TRACE(L"CAccessibleObj::~CAccessibleObj(%p, %X), %s", this, m_nHash, _DINFO());
}

// HRESULT CAccessibleObj::QueryEnum(void** ppvObject)
// {
//     CAccessibleEnum* p = new CAccessibleEnum();
//     p->m_ptrObj = this;
//     *ppvObject = static_cast<IEnumVARIANT*>(p);
//     return S_OK;
// }

#define WRAPPER_CHECK_POINT(func,point) \
    if (!point) \
    { \
        ACC_TRACE(L"CAccessibleObj::" L#func L"() return E_POINTER; %s", _DINFO()); \
        return E_POINTER; \
    }

#define WRAPPER_CHECK_MHTUI(func) \
    if (!GetOwner()) \
    { \
        ACC_TRACE(L"CAccessibleObj::" L#func L"() return RPC_E_DISCONNECTED; %s", _DINFO()); \
        return RPC_E_DISCONNECTED; \
    }

TMBool CheckVar(VARIANT& var)
{
    for (;;)
    {
        switch (var.vt)
        {
        case VT_ERROR:
            if (var.scode != DISP_E_PARAMNOTFOUND)
                return false;
            tm_fallthrough;
        case VT_EMPTY:
            var.vt = VT_I4;
            var.lVal = 0;
            tm_fallthrough;
        case VT_I4:
            return true;
            
        case VT_VARIANT | VT_BYREF:
            VariantCopy(&var, var.pvarVal);
            continue;

        default:
            break;
        }
    }
    return false;
}

#define WRAPPER_CHECK_CHILD(func) \
    if (!CheckVar(varChild)) \
    { \
        ACC_TRACE(L"CAccessibleObj::" L#func L"(varChild.vt:%d) return E_INVALIDARG; %s", varChild.vt, _DINFO()); \
        return E_INVALIDARG; \
    }


// IAccessible
STDMETHODIMP CAccessibleObj::get_accParent(IDispatch ** ppdispParent)
{
    DbgAssert(GetStyle());
    
    WRAPPER_CHECK_POINT(get_accParent, ppdispParent);
    WRAPPER_CHECK_MHTUI(get_accParent);
    
    HTUI hWnd = GetStyle()->_get_accParent(GetOwner());
    if (!hWnd)
    {
        HRESULT hr = AccessibleObjectFromWindow(GetOwner().GetHWND(), OBJID_WINDOW, IID_IDispatch, (void **)ppdispParent);
        ACC_HRTRACE(hr)(L"CAccessibleObj::get_accParent(*ppdispParent:%p)(WIN) return %X; %s",
            *ppdispParent, hr, _DINFO());
        return S_OK;
    }

    *ppdispParent = MakeAccClient(hWnd);
    ACC_HRTRACE(S_OK)(L"CAccessibleObj::get_accParent(*ppdispParent:%p); harent:%p %s", *ppdispParent, hWnd, _DINFO());
    return S_OK;
}

STDMETHODIMP CAccessibleObj::get_accChildCount(long* pChildCount)
{
    DbgAssert(GetStyle());
    WRAPPER_CHECK_POINT(get_accChildCount, pChildCount);
    WRAPPER_CHECK_MHTUI(get_accChildCount);

    *pChildCount =  GetStyle()->_get_accCtrlCount(GetOwner()) + GetStyle()->_get_accItemCount(GetOwner());
    ACC_HRTRACE(S_OK)(L"CAccessibleObj::get_accChildCount(*pChildCount:%d); %s", *pChildCount, _DINFO());
    return S_OK;
}

STDMETHODIMP CAccessibleObj::get_accChild(VARIANT varChild, IDispatch** ppdispChild)
{
    DbgAssert(GetStyle());
    WRAPPER_CHECK_POINT(get_accChild, ppdispChild);
    WRAPPER_CHECK_MHTUI(get_accChild);
    WRAPPER_CHECK_CHILD(get_accChild);
    
    if (varChild.lVal == CHILDID_SELF)
    {
        *ppdispChild = this;
        ((IAccessible*)this)->AddRef();
        ACC_HRTRACE(S_OK)(L"CAccessibleObj::get_accChild(CHILDID_SELF); %s", _DINFO());
        return S_OK;
    }
//     else if (varChild.lVal == OBJID_CARET)
//     {
//         *ppdispChild = GetOwner().GetObjCaret();
//         ACC_HRTRACE(S_OK)(L"CAccessibleObj::get_accChild(OBJID_CARET)); %s, varChild.lVal", _DINFO());
//         return S_OK;
//     }
//     else if (varChild.lVal == OBJID_HSCROLL)
//     {
//         *ppdispChild = GetOwner().GetObjHScrollBar();
//         ACC_HRTRACE(S_OK)(L"CAccessibleObj::get_accChild(OBJID_HSCROLL); %s, varChild.lVal", _DINFO());
//         return S_OK;
//     }
//     else if (varChild.lVal == OBJID_VSCROLL)
//     {
//         *ppdispChild = GetOwner().GetObjVScrollBar();
//         ACC_HRTRACE(S_OK)(L"CAccessibleObj::get_accChild(OBJID_VSCROLL); %s, varChild.lVal", _DINFO());
//         return S_OK;
//     }
    else if (varChild.lVal >= (LONG)0xFFFFFFF0 &&  varChild.lVal <= (LONG)0xFFFFFFFF)
    {
        ACC_HRTRACE(S_OK)(L"CAccessibleObj::get_accChild(%X) return S_FALSE; %s", varChild.lVal, _DINFO());
        return E_FAIL;
    }
    else if (CHash::IsID(varChild.lVal))
    {
        *ppdispChild = GetAccProxyBy(varChild.lVal);
        return S_OK;
    }

    long n = GetStyle()->_get_accItem(GetOwner(), varChild.lVal, ppdispChild);
    if (n == -1)
    {
        HRESULT hr = *ppdispChild ? S_OK : S_FALSE;
        ACC_HRTRACE(hr)(L"CAccessibleObj::get_accChild(varChild:%d, *ppdispChild:%p) return %X; %s",
            varChild.lVal, *ppdispChild, hr, _DINFO());
        return hr;
    }

    CTuiWidget hWin = GetOwner().GetWindow(GW_CHILD);
    for (long i = 1 + n; hWin; )
    {
        if (hWin.GetBindState(GetOwner()) & eBindStateSelfMask)
        {
            hWin = hWin.GetWindow(GW_HWNDNEXT);
            continue;
        }

        if (i == varChild.lVal)
        {
            *ppdispChild = MakeAccClient(hWin);
            ACC_HRTRACE(S_OK)(L"CAccessibleObj::get_accChild(varChild:%d, *ppdispChild:%p(%s); %s",
                varChild.lVal, *ppdispChild, _acc_dinfo2(hWin).c_str(), _DINFO());
            return S_OK;
        }
        
        hWin = hWin.GetWindow(GW_HWNDNEXT);
        ++i;
    }

    ACC_TRACE(L"CAccessibleObj::get_accChild(varChild:%d) return E_INVALIDARG; %s",
        varChild.lVal, _DINFO());
    return E_INVALIDARG;
}

STDMETHODIMP CAccessibleObj::get_accName(VARIANT varChild, BSTR* pszName)
{
    DbgAssert(GetStyle());
    WRAPPER_CHECK_POINT(get_accName, pszName);
    WRAPPER_CHECK_MHTUI(get_accName);
    WRAPPER_CHECK_CHILD(get_accName);
    
    CComBSTR bstr;
    HRESULT hr = GetStyle()->_get_accName(GetOwner(), varChild.lVal, bstr);
    if (SUCCEEDED(hr) && bstr)
        *pszName = bstr.Detach();
    
    ACC_HRTRACE(S_OK)(L"CAccessibleObj::get_accName(varChild:%d, *pszName:%s) return %X; %s",
        varChild.lVal, *pszName, hr, _DINFO());
    return hr;
}

STDMETHODIMP CAccessibleObj::get_accValue(VARIANT varChild, BSTR* pszValue)
{
    DbgAssert(GetStyle());
    WRAPPER_CHECK_POINT(get_accValue, pszValue);
    WRAPPER_CHECK_MHTUI(get_accValue);
    WRAPPER_CHECK_CHILD(get_accValue);
    
    CComBSTR bstr;
    HRESULT hr = GetStyle()->_get_accValue(GetOwner(), varChild.lVal, bstr);
    if (SUCCEEDED(hr) && bstr)
        *pszValue = bstr.Detach();
    
    ACC_HRTRACE(hr)(L"CAccessibleObj::get_accValue(varChild:%d, *pszValue:%s) return %X; %s",
        varChild.lVal, *pszValue, hr, _DINFO());
    return hr;
}

STDMETHODIMP CAccessibleObj::get_accDescription(VARIANT varChild, BSTR* pszDescription)
{
    DbgAssert(GetStyle());
    WRAPPER_CHECK_POINT(get_accDescription, pszDescription);
    WRAPPER_CHECK_MHTUI(get_accDescription);
    WRAPPER_CHECK_CHILD(get_accDescription);
    
    CComBSTR bstr;
    CString strID;
    HRESULT hr = S_OK;

#ifdef ACC_EXPORT_ID_DESC_TA
    if ((varChild.lVal != CHILDID_SELF))
        return S_FALSE;

    UINT nID = GetOwner().GetDlgCtrlID();
    if (nID)
        strID.Format(_T("c%d"), nID);
    else
    {
        nID = GetOwner().GetDlgCtrlObjectID();
        strID.Format(_T("o%d"), nID);
    }
    bstr = strID;
    if (SUCCEEDED(hr))
        *pszDescription = bstr.Detach();
    return hr;
#endif

    if (varChild.lVal == CHILDID_GETOBJECTID)
    {
        UINT nID = GetOwner().GetDlgCtrlID();
        if (nID)
            strID.Format(_T("c%d"), nID);
        else
        {
            nID = GetOwner().GetDlgCtrlObjectID();
            strID.Format(_T("o%d"), nID);
        }
        bstr = strID;
    }
    else if (varChild.lVal == CHILDID_GETOID)
        bstr = VA2W(L"%d", GetOwner().GetDlgCtrlObjectID());
    else if (varChild.lVal == CHILDID_GETCID)
        bstr = VA2W(L"%d", GetOwner().GetDlgCtrlID());
    else if (varChild.lVal == CHILDID_GETHTUI)
        bstr = VA2W(L"%p", GetOwner().m_hWnd);
    else
        if (GetOwner().SendMessage(CM_ACC_GET_DESCRIPTION, varChild.lVal, (TMLParam)&strID))
    {
        bstr = strID;
    }
    else
        hr = GetStyle()->_get_accDescription(GetOwner(), varChild.lVal, bstr);

    if (SUCCEEDED(hr))
        *pszDescription = bstr.Detach();
    
    ACC_HRTRACE(hr)(L"CAccessibleObj::get_accDescription(varChild:%d, *pszDescription:%s) return %X; %s",
        varChild.lVal, *pszDescription, hr, _DINFO());
    return hr;
}

STDMETHODIMP CAccessibleObj::get_accHelp(VARIANT varChild, BSTR* pszHelp)
{
    DbgAssert(GetStyle());
    WRAPPER_CHECK_POINT(get_accHelp, pszHelp);
    WRAPPER_CHECK_MHTUI(get_accHelp);
    WRAPPER_CHECK_CHILD(get_accHelp);

    CComBSTR bstr;
    HRESULT hr = GetStyle()->_get_accHelp(GetOwner(), varChild.lVal, bstr);
    if (SUCCEEDED(hr) && bstr)
        *pszHelp = bstr.Detach();

    ACC_HRTRACE(hr)(L"CAccessibleObj::get_accHelp(varChild:%d, *pszHelp:%s) return %X; %s",
        varChild.lVal, *pszHelp, hr, _DINFO());
    return hr;
}

STDMETHODIMP CAccessibleObj::get_accRole(VARIANT varChild, VARIANT *pvarRole)
{
    DbgAssert(GetStyle());
    WRAPPER_CHECK_POINT(get_accRole, pvarRole);
    WRAPPER_CHECK_MHTUI(get_accRole);
    WRAPPER_CHECK_CHILD(get_accRole);

    long x = GetRealRole(*this, GetStyle(), varChild.lVal);
    if (x == 0)
    {
        ACC_HRTRACE(E_FAIL)(L"CAccessibleObj::get_accRole() return E_FAIL; %s", _DINFO());
        return S_FALSE;
    }
    
    pvarRole->lVal = x;
    pvarRole->vt   = VT_I4;
    ACC_HRTRACE(S_OK)(L"CAccessibleObj::get_accRole(varChild:%d, *pvarRole:%s); %s",
        varChild.lVal, DbgFormatRole(x), _DINFO());
    return S_OK;
}

STDMETHODIMP CAccessibleObj::get_accState(VARIANT varChild, VARIANT* pvarState)
{
    DbgAssert(GetStyle());
    WRAPPER_CHECK_POINT(get_accState, pvarState);
    WRAPPER_CHECK_MHTUI(get_accState);
    WRAPPER_CHECK_CHILD(get_accState);
    
    long x = 0;
    HRESULT hr = GetStyle()->_get_accState(GetOwner(), varChild.lVal, x);
    if (FAILED(hr))
    {
       ACC_HRTRACE(hr)(L"CAccessibleObj::get_accState() return %X; %s", hr, _DINFO());
       return hr;
    }
    
//    x &= ~STATE_SYSTEM_FOCUSABLE;
    pvarState->lVal = x;
    pvarState->vt   = VT_I4;
    ACC_HRTRACE(S_OK)(L"CAccessibleObj::get_accState(child:%d, *state:%s); %s",
        varChild.lVal, DbgFormatState(x), _DINFO());
    return S_OK;
}

STDMETHODIMP CAccessibleObj::get_accKeyboardShortcut(VARIANT varChild, BSTR* pszKeyboardShortcut)
{
    DbgAssert(GetStyle());
    WRAPPER_CHECK_POINT(get_accKeyboardShortcut, pszKeyboardShortcut);
    WRAPPER_CHECK_MHTUI(get_accKeyboardShortcut);
    WRAPPER_CHECK_CHILD(get_accKeyboardShortcut);
    
    CComBSTR x;
    HRESULT hr = GetStyle()->_get_accKeyboardShortcut(GetOwner(), varChild.lVal, x);
    if (SUCCEEDED(hr) && x)
        *pszKeyboardShortcut = x.Detach();

    ACC_HRTRACE(hr)(L"CAccessibleObj::get_accKeyboardShortcut(varChild:%d, *pszKeyboardShortcut:%s)) return:%X; %s",
        varChild.lVal, *pszKeyboardShortcut, hr, _DINFO());
    return hr;
}

STDMETHODIMP CAccessibleObj::get_accFocus(VARIANT* pvarFocusChild)
{
    DbgAssert(GetStyle());
    WRAPPER_CHECK_POINT(get_accFocus, pvarFocusChild);
    WRAPPER_CHECK_MHTUI(get_accFocus);
    
    HRESULT hr = GetStyle()->_get_accFocus(GetOwner(), pvarFocusChild);
    ACC_HRTRACE(hr)(L"CAccessibleObj::get_accFocus(*pvarFocusChild:%p)) return:%X; %s",
        pvarFocusChild->pdispVal, hr, _DINFO());
    return hr;
}

STDMETHODIMP CAccessibleObj::get_accSelection(VARIANT* pvarSelectedChildren)
{
    DbgAssert(GetStyle());
    WRAPPER_CHECK_POINT(get_accSelection, pvarSelectedChildren);
    WRAPPER_CHECK_MHTUI(get_accSelection);
    
    HRESULT hr =  GetStyle()->_get_accSelection(GetOwner(), pvarSelectedChildren->lVal);
    if (SUCCEEDED(hr))
        pvarSelectedChildren->vt = VT_I4;

    ACC_HRTRACE(hr)(L"CAccessibleObj::get_accSelection(*pvarSelectedChildren:%d)) return:%X; %s",
        pvarSelectedChildren->lVal, hr, _DINFO());
    return hr;
}

STDMETHODIMP CAccessibleObj::get_accDefaultAction(VARIANT varChild, BSTR* pszDefaultAction)
{
    DbgAssert(GetStyle());
    WRAPPER_CHECK_POINT(get_accDefaultAction, pszDefaultAction);
    WRAPPER_CHECK_MHTUI(get_accDefaultAction);
    WRAPPER_CHECK_CHILD(get_accDefaultAction);
    
    CComBSTR x;
    HRESULT hr = GetStyle()->_get_accDefaultAction(GetOwner(), varChild.lVal, x);
    if (SUCCEEDED(hr) && x)
        *pszDefaultAction = x.Detach();

    ACC_HRTRACE(hr)(L"CAccessibleObj::get_accDefaultAction(varChild:%d, *pszDefaultAction:%s) return:%X; %s",
        varChild.lVal, *pszDefaultAction, hr, _DINFO());
    return hr;
}

STDMETHODIMP CAccessibleObj::accSelect(long flagsSelect, VARIANT varChild)
{
    DbgAssert(GetStyle());
    WRAPPER_CHECK_MHTUI(accSelect);
    WRAPPER_CHECK_CHILD(accSelect);
    
    HRESULT hr = GetStyle()->_accSelect(GetOwner(), varChild.lVal, flagsSelect);
    ACC_HRTRACE(hr)(L"CAccessibleObj::accSelect(flagsSelect:%d, varChild:%d) return:%X; %s",
        flagsSelect, varChild.lVal, hr, _DINFO());
    return hr;
}

STDMETHODIMP CAccessibleObj::accLocation(long* pxLeft, long* pyTop, long* pcxWidth, long* pcyHeight, VARIANT varChild)
{
    DbgAssert(GetStyle());
    WRAPPER_CHECK_MHTUI(accLocation);
    WRAPPER_CHECK_CHILD(accLocation);
    
    CTuiRect rcl;
    GetOwner().GetWindowRect(&rcl);
    HRESULT hr = GetStyle()->_accItemLocation(GetOwner(), varChild.lVal, rcl);
    if (FAILED(hr))
    {
        ACC_HRTRACE(hr)(L"CAccessibleObj::accLocation(varChild:%d) return %X; %s", varChild.lVal, hr, _DINFO());
        return hr;
    }

    CalcRect(GetOwner(), rcl, true, pxLeft, pyTop, pcxWidth, pcyHeight);
    ACC_HRTRACE(S_OK)(L"CAccessibleObj::accLocation(varChild:%d, rect:%d,%d,%d,%d); %s",
        varChild.lVal, pxLeft?*pxLeft:-1, pyTop?*pyTop:-1, pcxWidth?*pcxWidth:-1, pcyHeight?*pcyHeight:-1, _DINFO());
    return S_OK;
}

STDMETHODIMP CAccessibleObj::accNavigate(long navDir, VARIANT varStart, VARIANT* pvarEndUpAt)
{
    DbgAssert(GetStyle());
    WRAPPER_CHECK_POINT(accNavigate, pvarEndUpAt);
    WRAPPER_CHECK_MHTUI(accNavigate);
    
    HRESULT hr = GetStyle()->_accNavigate(GetOwner(), navDir, varStart.lVal, pvarEndUpAt);
    ACC_HRTRACE(hr)(L"CAccessibleObj::accNavigate(navDir:%d, varStart:%d, *pvarEndUpAt:%d) return:%X; %s",
        navDir, varStart.lVal, pvarEndUpAt->lVal, hr, _DINFO());
    return hr;
}

STDMETHODIMP CAccessibleObj::accHitTest(long xLeft, long yTop, VARIANT* pvarChildAtPoint)
{
    DbgAssert(GetStyle());
    WRAPPER_CHECK_POINT(accHitTest, pvarChildAtPoint);
    WRAPPER_CHECK_MHTUI(accHitTest);
    
    TMPoint pt = { xLeft, yTop };
    CTuiPoint ptl;
    GetOwner().ScreenToClient(&pt, 1, &ptl); // left,top is screen coordinate.

    HTUI hChild = GetOwner().ChildWindowFromPoint(ptl);
    if (!hChild)
    {
        ACC_HRTRACE(S_FALSE)(L"CAccessibleObj::accHitTest(xLeft:%d, yTop:%d) not find; %s",
            xLeft, yTop, _DINFO());
//         return S_FALSE; // return fail will let AccExplorer32.exe crash.
        pvarChildAtPoint->vt = VT_EMPTY;
        return S_OK;
    }

    if (hChild == GetOwner())
    {
        pvarChildAtPoint->vt = VT_I4;
        pvarChildAtPoint->lVal = CHILDID_SELF;
        HRESULT hr = GetStyle()->_accItemHitTest(GetOwner(), ptl, pvarChildAtPoint->lVal);
        ACC_HRTRACE(hr)(L"CAccessibleObj::accHitTest(xLeft:%d, yTop:%d, pvarChildAtPoint:CHILDID_SELF); %s",
            xLeft, yTop, _DINFO());
        return hr;
    }

    pvarChildAtPoint->vt = VT_DISPATCH;
    pvarChildAtPoint->pdispVal = MakeAccClient(hChild);
    ACC_HRTRACE(S_OK)(L"CAccessibleObj::accHitTest(xLeft:%d, yTop:%d, pvarChildAtPoint:%p); %s",
        xLeft, yTop, pvarChildAtPoint->pdispVal, _DINFO());
    return S_OK;
}

STDMETHODIMP CAccessibleObj::accDoDefaultAction(VARIANT varChild)
{
    DbgAssert(GetStyle());
    WRAPPER_CHECK_MHTUI(accDoDefaultAction);
    WRAPPER_CHECK_CHILD(accDoDefaultAction);
    
    HRESULT hr = GetStyle()->_accDoDefaultAction(GetOwner(), varChild.lVal);
    ACC_HRTRACE(hr)(L"CAccessibleObj::accDoDefaultAction(varChild:%d) return:%X; %s",
        varChild.lVal, hr, _DINFO());
    return hr;
}

STDMETHODIMP CAccessibleObj::put_accName(VARIANT varChild, BSTR szName)
{
    WRAPPER_CHECK_CHILD(put_accName);

    ACC_HRTRACE(S_OK)(L"CAccessibleObj::put_accName(varChild:%d, szName:%s); %s", varChild.lVal, szName, _DINFO());

    if (varChild.lVal != CHILDID_SELF)
        return S_FALSE;

    GetOwner().SetAccText(szName);
//    GetOwner().m_strAccName = szName;
    return S_OK;
}

STDMETHODIMP CAccessibleObj::put_accValue(VARIANT varChild, BSTR pszValue)
{
    ACC_HRTRACE(S_OK)(L"CAccessibleObj::put_accValue(varChild:%d, pszValue:%s); %s", varChild.lVal, pszValue, _DINFO());
    return E_NOTIMPL;
}

// IOleWindow
STDMETHODIMP CAccessibleObj::GetWindow(HWND* phwnd)
{
    DbgAssert(GetStyle());
    WRAPPER_CHECK_MHTUI(GetWindow);
    WRAPPER_CHECK_POINT(GetWindow, phwnd);

    *phwnd = GetOwner().GetHWND();
    return S_OK;
}

STDMETHODIMP CAccessibleObj::QueryService(REFGUID guidService, REFIID riid, void **ppvObject)
{
//     ACC_TRACE(L"CAccessibleObj::QueryService(%s, %s); %s",
//         DbgFormatGuid(guidService).c_str(), DbgFormatGuid(riid).c_str(), _DINFO()); 
    if (guidService == TM_UUID_OF(IAccessible) || guidService == TM_UUID_OF(IAccessible2))
        return _InternalQueryInterface(riid, ppvObject);
    return E_NOINTERFACE;
}
//
//// IAccIdentity
//STDMETHODIMP CAccessibleObj::GetIdentityString(DWORD dwIDChild, BYTE** ppIDString, DWORD* pdwIDStringLen)
//{
//    HRESULT hr = s_servers.MakeID(this, ppIDString, pdwIDStringLen);
//    ACC_TRACE(L"CAccessibleObj::GetIdentityString(); %s", _DINFO());
//    return hr;
//}
//
//// ISupportErrorInfo
//STDMETHODIMP CAccessibleObj::InterfaceSupportsErrorInfo(REFIID riid)
//{
//    ACC_TRACE(L"CAccessibleObj::InterfaceSupportsErrorInfo(%s), %s", DbgFormatGuid(riid).c_str(), _DINFO());
//    return S_FALSE;
//}


// IAccessible2
// STDMETHODIMP CAccessibleObj::get_states(AccessibleStates *states)
// {
//     WRAPPER_CHECK_MHTUI(IAccessible2::get_states);
//     
//     *states = (AccessibleStates)GetStyle()->_get_acc2States(GetOwner());
//     ACC_TRACE(L"CAccessibleObj::IAccessible2::get_states() return:%X; %s", *states, _DINFO());
//     return S_OK;
// }


// public functions
HRESULT CAccessibleObj::_InternalQueryInterface(REFIID iid, void** ppvObject)
{
//     WRAPPER_CHECK_MHTUI(_InternalQueryInterface);
    TM_INTERFACE_HANDLER(IDispatch)
    TM_INTERFACE_HANDLER(IOleWindow)
    TM_INTERFACE_HANDLER(IServiceProvider)
    TM_INTERFACE_HANDLER(IAccessible)
//    TM_INTERFACE_HANDLER(IAccessible2_2)
    //TM_INTERFACE_HANDLER(IAccIdentity)
    //TM_INTERFACE_HANDLER(ISupportErrorInfo)

    IAccessibleStyle* style = GetStyle();
    if (S_OK == style->_QueryService(*this, iid, ppvObject)) // if query IAccessibleText fail, win10 accessible2UIA proxy will crash.
        return S_OK;
		
     //if (iid == IID_IEnumVARIANT)
     //{
     //    CAccessibleEnum* p = new CAccessibleEnum();
     // 
     //    p->m_ptrObj = this;
     //    *ppvObject = static_cast<IEnumVARIANT*>(p);
     //    return S_OK;
     //}

//    TM_INTERFACE_MEMBER             (IAccessibleText, (&s_accText))
//    if (!bDisableHook && style == &g_accMgr)//  || style == &g_accTrackBar)
  //  if ((!g_bDisableJawsHook && style == &g_accMgr) || style == &g_accTrackBar)
    //if (g_accFilter.IsFirstActive())
//    if (this->IsFocus())
    if (style == &g_accTrackBar) // || style == &g_accEdit)
    {
        TM_INTERFACE_HANDLER        (IAccessibleApplication)
    }
    
    TM_INTERFACE_HANDLER            (IAccessible2)
//        if (iid == TM_UUID_OF(IAccessible2))
//    {
////        long role = GetRealRole(*this, m_ptr, 0);
//   //     if (role == ROLE_SYSTEM_LIST || role == ROLE_SYSTEM_OUTLINE || role == ROLE_SYSTEM_LISTITEM)
//   //         return E_NOINTERFACE;
//
//        IAccessible2* p = this;
//        *ppvObject = p;
//        p->AddRef();
//        return S_OK;
//    }
//     switch (GetStyle()->_get_accRole(GetOwner(), CHILDID_SELF))
//     {
//     case ROLE_SYSTEM_TEXT:
//         TM_INTERFACE_HANDLER        (IAccessibleText)
//         break;
//     }
    TM_INTERFACE_HANDLER2           (IAccessible, IUnknown)

    if (!s_bInLresultFromObject)
        ACC_TRACE(L"CAccessibleObj::QueryInterface(%s), %s", DbgFormatGuid(iid).c_str(), _DINFO()); 

    *ppvObject = NULL;
    return E_NOINTERFACE;
}

static CAccessibleObj* _CAccessibleObj_InitStyle(CAccessibleObj* wnd)
{
    if (!wnd)
        return 0;

    DbgAssert(*wnd);
    wnd->GetUnknown()->AddRef();

    if (wnd->m_ptr)
        return wnd;
    
    CString strObjType;
    wnd->GetClassName(strObjType);
    
    IAccessibleStyle* style = 0;
//     if (wnd->AccGetRole() == ROLE_CUSTOM)
//         wnd->AccGetObject(&style);
    if (style)
        wnd->m_ptr = style;
    else
    {
        GetAccStyleByClassName(strObjType, &wnd->m_ptr);
        DbgAssert(*wnd);
        if (!wnd->m_ptr)
        {
            wnd->m_ptr = &g_accWidget;
        }
        else if (wnd->m_ptr == &g_accMgr)
        {
            g_accFilter.DoJawsHookInstall();

            // may be have some app in one process.
            CTuiApp* app = wnd->GetAPP();
            g_accFilter.RegisterMessageFilter(app);
            //            g_accFilter.FocusEvent_Create(wnd->GetHWND());
        }
        //else if (wnd->m_ptr == &g_accTrackBar)
        //    s_servers.Register(wnd->GetHWND(), wnd->m_nHash);
    }
    
    // combobox
    if (CAccessibleObj::IsContainCombobboxBtn(*wnd))
    {
        CAccComboBox* p = new CRefObjectT<CAccComboBox>(0);//  &g_accComboBox;
        p->Init(wnd->m_ptr);
        DbgAssert(*wnd);
        wnd->m_ptr = p;
        ACC_TRACE(L"CAccessibleWrapper::GetObjClient(%s) comb", _acc_dinfo(*wnd).c_str());
    }
    else if(wnd->m_ptr == &g_accListBox)
    {
        if (CTuiComboUnit(*wnd).GetButton())
            wnd->m_ptr = &g_accCombListBox;
    }
    else if (wnd->m_ptr == &g_accTreeView)
    {
        if (CTuiComboUnit(*wnd).GetButton())
            wnd->m_ptr = &g_accCombTreeView;
    }

    
    DbgTrace(L"CAccessibleWrapper::GetObjClient(%s)", _acc_dinfo(*wnd).c_str());
    return wnd;
}

CAccessibleObj* CAccessibleObj::MakeAccClient(HTUI hWgt)
{
    CAccessibleObj* p = CHash::Make(hWgt);
    return _CAccessibleObj_InitStyle(p);
}

CAccessibleObj* CAccessibleObj::GetAccProxyBy(AccHash32 hash)
{
    CAccessibleObj* p = CHash::Find(hash);
    if (!p)
        return 0;
    DbgAssert(!p->IsMgr());
    p->GetUnknown()->AddRef();
    return p;// _CAccessibleObj_InitStyle(p);
}

HRESULT CAccessibleObj::MakeAccKeyboardShortcut(const CString& str, CComBSTR& szKeyboardShortcut)
{
    int nPos = str.Find(_T("&"));
    if ((-1 == nPos) || (str.GetLength() - 1 == nPos))
        return S_FALSE;
    
    CString str2(_T("Alt+"));
    str2 += str.GetAt(nPos + 1);
    szKeyboardShortcut = str2;
    return S_OK;
}

EGetNOV CAccessibleObj::GetAccNameOrValue(HTUI hWgt, CString& str, EGetNOV nMask)
{
    CTuiWidget wnd(hWgt);

//     if (nMask & eGetCustom)
//     {
//         str = m_strAccName;
//         if (!str.IsEmpty())
//             return eGetCustom;
//     }
    if (nMask & eGetAccText)
    {
        wnd.GetAccText(str);
        if (!str.IsEmpty())
            return eGetAccText;
    }

    //1.Get prev static text
    if (nMask & eGetPrevStatic) // && ((strObjType == TUI_CLASS_EDIT) || (strObjType == TUI_CLASS_MEMO) || (strObjType == TUI_CLASS_RICH) || (strObjType == TUI_CLASS_TRACKBAR)))
    {
        CTuiWidget hPreWin = wnd.GetWindow(GW_HWNDPREV);
        if (hPreWin)
        {
            CAccNotify an(hPreWin);
            if (GetRealRole(hPreWin, an->m_ptr, 0) == ROLE_SYSTEM_STATICTEXT)
            {
                hPreWin.GetWindowText(str);
                str.Replace(_T("&"), _T(""));
                if (!str.IsEmpty())
                    return eGetPrevStatic;
            }
        }
    }

    //3.Get hint text if acc text is empty
//     CString strObjType;
//     wnd.GetClassName(strObjType);
    if (nMask & eGetHint)
    {
//         TMBool bReadHint = TMTrue;
//         if (strObjType == TUI_CLASS_STATIC)
//         {
//             CString strWindowText;
//             wnd.GetWindowText(strWindowText);
//             if (!strWindowText.IsEmpty()) {
//                 bReadHint = TMFalse;
//             }
//         }
//         if (bReadHint)
        {
            wnd.GetHint(str);
            if (!str.IsEmpty())
                return eGetHint;
        }
    }
    
    //4.Get Windows text if acc text and hint are empty both
    if (nMask & eGetWindowText)
    {
        wnd.GetWindowText(str);
        if (!str.IsEmpty())
            return eGetWindowText;
    }

    return eGetNull;
}

HRESULT CAccessibleObj::GetAccNameOrValue(HTUI hWgt, CComBSTR& str, EGetNOV nMask)
{
    CString strx;
    EGetNOV ret = GetAccNameOrValue(hWgt, strx, nMask);
    if (!ret)
        return S_FALSE;
    str = strx;
    return S_OK;
}

bool CAccessibleObj::IsContainCombobboxBtn(CAccessibleObj& obj)
{
    CTuiWidget wnd(obj);
    if (obj.m_ptr == &g_accCombButton)
    {
        if (wnd.GetStyle() & CBS_NOEDIT)
            return true;
    }

    for (wnd = wnd.GetWindow(GW_CHILD); wnd; wnd = wnd.GetWindow(GW_HWNDNEXT))
    {
        CString strObjType;
        wnd.GetClassName(strObjType);
        if (strObjType == _T("combobutton"))
        {
            if (wnd.GetStyle() & CBS_NOEDIT)
                return false;
            return true;
        }
    }
    
    return false;
}

bool CAccessibleObj::IsComboList(HTUI hWgt)
{
    HTUI hList = CTuiComboUnit(hWgt).GetPopList();
    return hList == hWgt;
}

void CAccessibleObj::VirtualMouseClick(HTUI hWgt, CTuiPoint pt, TMBool bDoubleClick)
{
    TMPoint ptOld, ptNew;
    GetCursorPos(&ptOld);
    CTuiWidget(hWgt).ClientToScreen(&pt, 1, &ptNew);
    SetCursorPos(ptNew.x, ptNew.y);
    mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0); // MOUSEEVENTF_ABSOLUTE
    if (bDoubleClick)
        mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0); // MOUSEEVENTF_ABSOLUTE
    SetCursorPos(ptOld.x, ptOld.y);
}

void CAccessibleObj::GetText(HTUI hWgt, long startOffset, long endOffset, BSTR *text)
{
    CString str;
    CTuiWidget(hWgt).GetWindowText(str);
    int nLen = endOffset - startOffset;
    CStrPtr nstr(str.Mid(startOffset, nLen));
    *text = SysAllocStringLen(nstr.data(), nstr.GetLength());
}

void CAccessibleObj::CalcRect(HTUI hWgt, const CTuiRect& rc, TMBool bScreen, long *x, long *y, long *width, long *height)
{
    CRect rcDevice;
    if (bScreen)
        CTuiWidget(hWgt).ClientToScreen(rc, rcDevice);
    else
        CTuiWidget(hWgt).GetDPI().LPtoDP(rc, rcDevice);
    if (x)
        *x = rcDevice.Left();
    if (y)
        *y = rcDevice.Top();
    if (width)
        *width = rcDevice.Width();
    if (height)
        *height = rcDevice.Height();
}

long CAccessibleObj::GetRealRole(HTUI hWgt, IAccessibleStyle* style, long idChild)
{
    long idTemp = idChild ? idChild : style->_get_accIndex();
    long x = idTemp ? ROLE_DEFAULT : CTuiWidget(hWgt).AccGetRole();
    if (x == (long)ROLE_DEFAULT) // || x == (long)ROLE_CUSTOM)
        x = style->_get_accRole(hWgt, idChild);
    return x;
}

long CAccessibleObj::GetState(CAccessibleWnd& wnd)
{
    long state = 0;
    TMBool bVisible  = wnd.IsWindowVisible();
    TMBool bEnabled  = wnd.IsWindowEnabled();
    TMBool bTabstop  = wnd.GetStyle() & WS_TABSTOP;
    TMBool bFocus    = wnd.IsFocus();
    
    if (!bVisible)
        state |= STATE_SYSTEM_INVISIBLE;
    if (!bEnabled)
        state |= STATE_SYSTEM_UNAVAILABLE;
    else if (bTabstop)
        state |= STATE_SYSTEM_FOCUSABLE;
    if (bFocus)
        state |= STATE_SYSTEM_FOCUSED;
    
//     CTuiRect rc;
//     sender.GetWindowRect(&rc);
//     for (CTuiWidget wndp(sender); wndp = wndp.GetParent();)
//     {
//         CTuiRect rc2;
//         wndp.GetWindowRect(&rc2);
//         rc &= rc2;
//     }
//     if (rc.IsRectEmpty())
//         state |= STATE_SYSTEM_OFFSCREEN;

    return state;
}

static void _CAccessibleObj_GetGroupPosition(HTUI hWgt, long role, long* similarItemsInGroup, long* positionInGroup)
{
    long pig = 1;
    long sig = 1;
    tm_for(CTuiWidget w(hWgt); w = w.GetWindow(GW_HWNDPREV); )
    {
        if (w.IsWindowVisible())
        {
            CAccessibleObj* p = CAccessibleObj::MakeAccClient(w);
            DbgAssert(p);
            if (role == CAccessibleObj::GetRealRole(*p, p->m_ptr, 0))
            {
                ++pig; ++sig;
            }
            p->Release();
        }
    }
    tm_for(CTuiWidget w(hWgt); w = w.GetWindow(GW_HWNDNEXT); )
    {
        if (w.IsWindowVisible())
        {
            CAccessibleObj* p = CAccessibleObj::MakeAccClient(w);
            DbgAssert(p);
            if (role == CAccessibleObj::GetRealRole(*p, p->m_ptr, 0))
                ++sig;
            p->Release();
        }
    }
    *similarItemsInGroup = sig;
    *positionInGroup     = pig;
}

static void _CAccessibleObj_GetGroupLevel(HTUI hWgt, long role, long* groupLevel)
{
    tm_for(CTuiWidget w(hWgt); w = w.GetParent(); )
    {
        CAccessibleObj* p = CAccessibleObj::MakeAccClient(w);
        DbgAssert(p);
        long r = CAccessibleObj::GetRealRole(*p, p->m_ptr, 0);
        p->Release();
        if (role != r)
            return;
        ++*groupLevel;
    }
}

HRESULT CAccessibleObj::GetGroupPosition(CAccessibleWnd& wnd, IAccessibleStyle* style, long* groupLevel, long* similarItemsInGroup, long* positionInGroup)
{
    // list
    if (style == &g_accListSelItem)
        return g_accListSelItem.GetGroupPosition(wnd, groupLevel, similarItemsInGroup, positionInGroup);

    // tree
    if (style == &g_accTreeSelItem)
        return g_accTreeSelItem.GetGroupPosition(wnd, groupLevel, similarItemsInGroup, positionInGroup);

    //if (style == &g_accTreeView)
    //{
    //    _HashObject p = CHash::GetFocusItem();
    //    if (p->m_hWnd == wnd && p->m_ptr == &g_accTreeSelItem)
    //        return g_accTreeSelItem.GetGroupPosition(wnd, groupLevel, similarItemsInGroup, positionInGroup);
    //}

    *groupLevel = 0;
    long role = CAccessibleObj::GetRealRole(wnd, style, 0);

    // tree
    if (role == ROLE_SYSTEM_OUTLINEITEM)
        _CAccessibleObj_GetGroupLevel(wnd, role, groupLevel);

    if (role == ROLE_SYSTEM_COMBOBOX)
    {
        CTuiWidget btn(CTuiComboUnit(wnd).GetButton());
        DWORD dwStyle = btn.GetStyle();
        if ((dwStyle & CBS_NOIMEDIALTY) && !CTuiComboUnit(wnd).IsDropDown())
            return E_NOTIMPL;
        CTuiWidget lst(CTuiComboUnit(wnd).GetPopList());
        if (!lst)
            return E_NOTIMPL;
        HRESULT hr = E_NOTIMPL;
        CAccessibleObj* obj = CAccessibleObj::MakeAccClient(lst);
        if (obj->m_ptr == &g_accCombListBox)
            hr = g_accCombListBox.GetGroupPosition(*obj, groupLevel, similarItemsInGroup, positionInGroup);
        else if (obj->m_ptr == &g_accCombTreeView)
            hr = g_accCombTreeView.GetGroupPosition(*obj, groupLevel, similarItemsInGroup, positionInGroup);
        obj->Release();
        return hr;
    }

    // items
    if (role == ROLE_SYSTEM_LISTITEM ||
        role == ROLE_SYSTEM_RADIOBUTTON ||
        role == ROLE_SYSTEM_PAGETAB ||
        role == ROLE_SYSTEM_OUTLINEITEM)
    {
        _CAccessibleObj_GetGroupPosition(wnd, role, similarItemsInGroup, positionInGroup);
        return S_OK;
    }

    return E_NOTIMPL;
}

bool CAccessibleObj::CombUtilIsFocus(HTUI hCombUtil, ECombTest eSrc, HTUI hFocus, ECombTest eTest)
{
    if (!hFocus)
        return false;
    if (hCombUtil == hFocus)
        return true;
    CTuiWidget wnd(hCombUtil);
    if (eSrc == eCombList)
    {
        if (wnd.IsChild(hFocus))
            return true;
        if (eTest & eCombButton)
            return CombUtilIsFocus(CTuiComboUnit(wnd).GetButton(), eCombButton, hFocus, eTest & (eCombButton | eCombEdit));
        if (eTest & eCombEdit)
            return CombUtilIsFocus(CTuiComboUnit(wnd).GetEdit(), eCombEdit, hFocus, eTest & eCombNull);
        return false;
    }
    else if (eSrc == eCombButton)
    {
        if (eTest & eCombEdit)
            return CombUtilIsFocus(CTuiComboUnit(wnd).GetEdit(), eCombEdit, hFocus, eTest & eCombList);
        if (eTest & eCombList)
            return CombUtilIsFocus(CTuiComboUnit(wnd).GetPopList(), eCombList, hFocus, eCombNull);
        return false;
    }
    else
    {
        DbgAssert(eSrc == eCombEdit);
        if (wnd.IsChild(hFocus))
            return true;
        if (eTest & eCombList)
            return CombUtilIsFocus(CTuiComboUnit(wnd).GetPopList(), eCombList, hFocus, eCombNull);
        return false;
    }
}


#undef _DINFO
#undef WRAPPER_CHECK_POINT
#undef WRAPPER_CHECK_MHTUI
#undef WRAPPER_CHECK_CHILD
