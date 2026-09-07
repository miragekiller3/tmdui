/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2020.10.23
 *
 *  Copyright (C) 2020 miragekiller
 */

#pragma once

#ifndef UIWEBVIEW_H
#define UIWEBVIEW_H

#include "tmcom_win.h"
#include "uiactivex.h"
#include "tmformatguid.h"


#pragma warning (push)
#pragma warning (disable : 4192) //warning C4192: automatically excluding 'IUnknown' while importing type library
#import "../3rd/Microsoft.Web.WebView2/WebView2.tlb" no_implementation, raw_interfaces_only
#import "../3rd/win/Mshtml.tlb" no_implementation, raw_interfaces_only
#pragma warning (pop)

#ifndef CORE_WEBVIEW_TARGET_PRODUCT_VERSION
#   define CORE_WEBVIEW_TARGET_PRODUCT_VERSION L"87.0.664.37"
#endif

//#define WEBVIEW_TRACE DbgTrace
#define WEBVIEW_TRACE DbgTraceNull

TM_BGN_NAMESPACE
class CTuiWebViewImpl;
TM_END_NAMESPACE



///////////////////////////////////////////////////////////////////////////////
// WebView2
TM_BGN_NAMESPACE_X(WebView2)

class CChromeCont;

#define Chrome_EVENT_IMPL(func) \
    template <class T> void Add_##func(T* p) { DbgVerifyHResult(p->add_##func (this, &this->m_token)); } \
    template <class T> void Remove_##func(T* p) { DbgVerifyHResult(p->remove_##func (this->m_token)); this->m_token.value = 0; }

template <class T, class T_Base> class TM_NO_VTABLE CChromeEventImpl : public T_Base
{
public:
    EventRegistrationToken m_token;
    CChromeEventImpl() { m_token.value = 0; }
    T* GetT() { return static_cast<T*>(this); }
};

template <class T>
struct TM_NO_VTABLE CCoreWebView2NavigationStartingEventHandler
    : public CChromeEventImpl<T, ICoreWebView2NavigationStartingEventHandler>
{
    Chrome_EVENT_IMPL(NavigationStarting);
    STDMETHODIMP Invoke(ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args) tm_override;
};

template <class T>
struct TM_NO_VTABLE CCoreWebView2DocumentTitleChangedEventHandler
    : public CChromeEventImpl<T, ICoreWebView2DocumentTitleChangedEventHandler>
{
    Chrome_EVENT_IMPL(DocumentTitleChanged);
    STDMETHODIMP Invoke(struct ICoreWebView2* sender, IUnknown* args) tm_override;
};

template <class T>
struct TM_NO_VTABLE CCoreWebView2HistoryChangedEventHandler
    : public CChromeEventImpl<T, ICoreWebView2HistoryChangedEventHandler>
{
    Chrome_EVENT_IMPL(HistoryChanged);
    STDMETHODIMP Invoke(struct ICoreWebView2* sender, IUnknown* args) tm_override;
};

template <class T>
struct TM_NO_VTABLE CCoreWebView2NavigationCompletedEventHandler
    : public CChromeEventImpl<T, ICoreWebView2NavigationCompletedEventHandler>
{
    Chrome_EVENT_IMPL(NavigationCompleted);
    STDMETHODIMP Invoke(ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args) tm_override;
};

template <class T>
struct TM_NO_VTABLE CCoreWebView2WebMessageReceivedEventHandler
    : public CChromeEventImpl<T, ICoreWebView2WebMessageReceivedEventHandler>
{
    Chrome_EVENT_IMPL(WebMessageReceived);
    STDMETHODIMP Invoke(ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args) tm_override;
};

template <class T>
struct TM_NO_VTABLE CCoreWebView2ContainsFullScreenElementChangedEventHandler
    : public CChromeEventImpl<T, ICoreWebView2ContainsFullScreenElementChangedEventHandler>
{
    Chrome_EVENT_IMPL(ContainsFullScreenElementChanged);
    STDMETHODIMP Invoke(ICoreWebView2* sender, IUnknown* args) tm_override;
};

template <class T>
struct TM_NO_VTABLE CCoreWebView2HeaderRequestedEventHandler
    : public CChromeEventImpl<T, ICoreWebView2WebResourceRequestedEventHandler>
{
protected:
    TM::CString m_strHead;
    Chrome_EVENT_IMPL(WebResourceRequested);
    STDMETHODIMP Invoke(ICoreWebView2* sender, ICoreWebView2WebResourceRequestedEventArgs* args) tm_override;

public:
    void WriteHead(ICoreWebView2HttpRequestHeaders* pHead);
};

template <class T>
struct TM_NO_VTABLE CCoreWebView2ProcessFailedEventHandler
    : public CChromeEventImpl<T, ICoreWebView2ProcessFailedEventHandler>
{
protected:
    Chrome_EVENT_IMPL(ProcessFailed);
    STDMETHODIMP Invoke(ICoreWebView2* sender, ICoreWebView2ProcessFailedEventArgs* args) tm_override;
};

template <class T>
struct TM_NO_VTABLE CCoreWebView2NewWindowRequestedEventHandler
    : public CChromeEventImpl<T, ICoreWebView2NewWindowRequestedEventHandler>
{
protected:
    Chrome_EVENT_IMPL(NewWindowRequested);
    STDMETHODIMP Invoke(ICoreWebView2* sender, ICoreWebView2NewWindowRequestedEventArgs* args) tm_override;
};

template <class T>
struct TM_NO_VTABLE CCoreWebView2WindowCloseRequestedEventHandler
    : public CChromeEventImpl<T, ICoreWebView2WindowCloseRequestedEventHandler>
{
protected:
    Chrome_EVENT_IMPL(WindowCloseRequested);
    STDMETHODIMP Invoke(ICoreWebView2* sender, IUnknown* args) tm_override;
};

template <class T>
struct TM_NO_VTABLE CCoreWebView2AcceleratorKeyPressedEventHandler
    : public CChromeEventImpl<T, ICoreWebView2AcceleratorKeyPressedEventHandler>
{
protected:
    Chrome_EVENT_IMPL(AcceleratorKeyPressed);
    STDMETHODIMP Invoke(ICoreWebView2Controller* sender, ICoreWebView2AcceleratorKeyPressedEventArgs* args) tm_override;
};

#undef Chrome_EVENT_IMPL

struct TM_NO_VTABLE CChromeExt
    : public CCoreWebView2NavigationStartingEventHandler<CChromeExt>
//    , public ICoreWebView2SourceChangedEventHandler
//     , public ICoreWebView2ContentLoadingEventHandler
    , public CCoreWebView2DocumentTitleChangedEventHandler<CChromeExt>
    , public CCoreWebView2HistoryChangedEventHandler<CChromeExt>
    , public CCoreWebView2NavigationCompletedEventHandler<CChromeExt>
    , public CCoreWebView2WebMessageReceivedEventHandler<CChromeExt>
//     , public ICoreWebView2WebResourceRequestedEventHandler
    , public CCoreWebView2HeaderRequestedEventHandler<CChromeExt>
    , public CCoreWebView2ContainsFullScreenElementChangedEventHandler<CChromeExt>
    , public CCoreWebView2ProcessFailedEventHandler<CChromeExt>
    , public CCoreWebView2NewWindowRequestedEventHandler<CChromeExt>
    , public CCoreWebView2WindowCloseRequestedEventHandler<CChromeExt>
//     , public CCoreWebView2AcceleratorKeyPressedEventHandler<CChromeExt>
//     , public CCoreWebView2EnvironmentOptions
{
protected:
    TM_BGN_INTERFACE_MAP        (ICoreWebView2NavigationStartingEventHandler)
        WEBVIEW_TRACE(L"CChromeExt::QueryInterface(%s)", DbgFormatGuid(iid).c_str()); 
    TM_END_INTERFACE_MAP        ()

public:
    CChromeCont* GetThis();
    TM::CTuiWebViewImpl* GetWidget();
    HRESULT SendWidgetMessage(TMUInt msg, TMWParam wp, TMLParam lp);

public:
    void InitExt(ICoreWebView2* p, ICoreWebView2Controller* c);
    void UninitExt(ICoreWebView2* p, ICoreWebView2Controller* c);
};

class TM_NO_VTABLE CChromeCtrl : public IUnknown
{
public:
    CComPtr<ICoreWebView2Environment>   m_ptrEnv;
    CComPtr<ICoreWebView2Controller>    m_ptrController;
    CComPtr<ICoreWebView2>              m_ptrWebView;

    TM_BGN_INTERFACE_MAP        (IUnknown)
        TM_INTERFACE_HANDLER_M  (ICoreWebView2, m_ptrWebView)
        TM_INTERFACE_HANDLER_M  (ICoreWebView2Controller, m_ptrController)
        TM_INTERFACE_HANDLER_M  (ICoreWebView2Environment, m_ptrEnv)
    TM_END_INTERFACE_MAP        ()
};

class TM_NO_VTABLE CChromeCont
    : public TM::CDummyChild
    , public CChromeExt
    , public CChromeCtrl
{
public:
    friend CChromeExt;
    using CChromeCtrl::GetUnknown;

public:
    CChromeCont() { WEBVIEW_TRACE("CChromeCont::CChromeCont()"); }
    ~CChromeCont() { WEBVIEW_TRACE("CChromeCont::~CChromeCont()"); DbgAssert(!m_ptrEnv); }
    CComPtr<ICoreWebView2Settings> GetSetting();
    void OnStyleChanged(TMUInt32 nOld, TMUInt32 nNew);
    void OnInit();
    static CChromeCont* InitInstance(TM::CTuiWebViewImpl* hWnd, HINSTANCE hInstance, int nCmdShow);
    HRESULT CreateBrowserControlsWebView();
    TMBool Reload();
    void SetWndFocus();

    ///////////////////////////////////////////////////////////////////////////
    // IDummy
    virtual void DestroyDummy(EUnbindDummy eReason) tm_override;
    virtual TMBool SyncIsValid() tm_override { return m_ptrController != 0; }
    virtual TMBool ProcessWindowMessage(TM::HTUI hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID) tm_override;
};


TM_END_NAMESPACE_X(WebView2)


///////////////////////////////////////////////////////////////////////////////
// TM
TM_BGN_NAMESPACE

class CIECont;

struct TM_NO_VTABLE CIEExtend
    : public CWebBrowserEventT<CIEExtend>
    , public CDocHostUIHandler
    , public CDocHostShowUI
    , public IOleCommandTarget
    , public CAmbientPropImpl<CIEExtend>
{
protected:
    enum { e_DISPID_postMessage = 20 };

    CComPtr<IStream> m_spNavigateToString;

    CIECont* GetThis();
    CTuiWebViewImpl* GetWidget();
    HRESULT SendWidgetMessage(TMUInt msg, TMWParam wp, TMLParam lp);
    struct _CheckIEBug // IE bug: if destroy dialog when every event, IE will crash.
    {
        HWND hWnd;
        _CheckIEBug(CIEExtend* ext) : hWnd(((HTUI)ext->GetWidget())->GetHWND()) {}
        ~_CheckIEBug() { ::DestroyWindow(hWnd); } //DbgAssert(::IsWindow(hWnd)); }
    };
    friend _CheckIEBug;
# define CheckIEBug() TM_SW_DBG(_CheckIEBug _temp(this), ;)

public:
    TM_BGN_INTERFACE_MAP        (IDispatch)
        TM_INTERFACE_HANDLER    (IDispatch)
        TM_INTERFACE_HANDLER    (DWebBrowserEvents2)
        TM_INTERFACE_HANDLER    (IDocHostUIHandler)
        TM_INTERFACE_HANDLER    (IDocHostShowUI)
        TM_INTERFACE_HANDLER    (IOleCommandTarget)
    TM_END_INTERFACE_MAP        ()

    ///////////////////////////////////////////////////////////////////////////
    // IDispatch
    STDMETHODIMP GetIDsOfNames(REFIID riid, OLECHAR** rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) tm_override;

    STDMETHODIMP Invoke(DISPID dispidMember, REFIID riid,
        LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult,
        EXCEPINFO* pexcepinfo, UINT* puArgErr) tm_override;

    // DWebBrowserEvents2
    void DLControl(long *pFlag);

//     HRESULT UserAgent(VARIANT* pvarResult);

    void BeforeNavigate2(
        IDispatch *pDisp, VARIANT *URL, VARIANT *Flags,
        VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers,
        VARIANT_BOOL *Cancel)
    {
        WEBVIEW_TRACE(L"CIEExtend::BeforeNavigate2(%s)", URL->bstrVal);
//         CheckIEBug(); // have bug
        *Cancel = SendWidgetMessage(CWV_ONNAVIGATESTART, 0, (TMLParam)URL->bstrVal);
    }

    void CommandStateChange(long Command, VARIANT_BOOL Enable)
    {
        WEBVIEW_TRACE("CIEExtend::CommandStateChange(command:%d; enable:%d)", Command, Enable);
//         CheckIEBug(); // no bug
        SendWidgetMessage(CWV_ONCOMMANDSTATE, Command, Enable);
    }

    void DocumentComplete(IDispatch *pDisp, VARIANT *URL);

    void DownloadBegin()
    {
        WEBVIEW_TRACE("CIEExtend::DownloadBegin()");
//         CheckIEBug(); // have bug
        SendWidgetMessage(CWV_ONIEDOWNBEGIN, 0, 0);
    }

    void DownloadComplete()
    {
        WEBVIEW_TRACE("CIEExtend::DownloadComplete()");
//         CheckIEBug(); // no bug
        SendWidgetMessage(CWV_ONIEDOWNCOMPLETE, 0, 0);
    }
    
    void FileDownload(VARIANT_BOOL *Cancel)
    {
        WEBVIEW_TRACE("CIEExtend::FileDownload()");
//         CheckIEBug(); // have bug
        *Cancel = SendWidgetMessage(CWV_ONIEFILEDOWNLOAD, 0, 0);
   }

    void NavigateComplete2(IDispatch *pDisp, VARIANT *URL);

    void NavigateError(
        IDispatch *pDisp, VARIANT *URL, VARIANT *TargetFrameName,
        VARIANT *StatusCode, VARIANT_BOOL *Cancel)
    {
        WEBVIEW_TRACE("CIEExtend::NavigateError(code:%d)", StatusCode->lVal);
//         CheckIEBug(); // no bug
        SendWidgetMessage(CWV_ONNAVIGATEERROR, 0, StatusCode->lVal);
    }

    void NewWindow3(IDispatch** ppDisp, VARIANT_BOOL* Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl);

    void OnFullScreen(VARIANT_BOOL FullScreen)
    {
        WEBVIEW_TRACE("CIEExtend::OnFullScreen(%d)", FullScreen);
//         CheckIEBug(); // not check
        SendWidgetMessage(CWV_ONFULLSCREEN, 0, FullScreen);
    }

    //void OnMenuBar (VARIANT_BOOL MenuBar) {}

//     void OnQuit()
//     {
//         WEBVIEW_TRACE("CIEExtend::OnQuit()");
//         SendWidgetMessage(CWV_ONQUIT, 0, 0);
//     }

    void OnStatusBar(VARIANT_BOOL StatusBar)
    {
        WEBVIEW_TRACE("CIEExtend::OnStatusBar(%d)", StatusBar);
//         CheckIEBug(); // not check
        SendWidgetMessage(CWV_ONIESTATUSBAR, 0, StatusBar);
    }

    void OnTheaterMode(VARIANT_BOOL TheaterMode)
    {
        WEBVIEW_TRACE("CIEExtend::OnTheaterMode(%d)", TheaterMode);
//         CheckIEBug(); // not check
        SendWidgetMessage(CWV_ONIETHEATERMODE, 0, TheaterMode);
    }

//     void OnToolBar (VARIANT_BOOL ToolBar) {}
//     void OnVisible (VARIANT_BOOL Visible) {}

    void ProgressChange(long Progress, long ProgressMax)
    {
        WEBVIEW_TRACE("CIEExtend::ProgressChange(%d, %d)", Progress, ProgressMax);
//         CheckIEBug(); // no bug
        SendWidgetMessage(CWV_ONIEPROGRESSCHANGE, Progress, ProgressMax);
    }

    void PropertyChange(BSTR szProperty)
    {
        WEBVIEW_TRACE(L"CIEExtend::PropertyChange(%s)", szProperty);
//         CheckIEBug(); // have bug
        SendWidgetMessage(CWV_ONIEPROPCHANGE, 0, (TMLParam)szProperty);
    }

    void StatusTextChange(BSTR Text)
    {
        WEBVIEW_TRACE(L"CIEExtend::StatusTextChange(%s)", Text);
//         CheckIEBug(); // have bug
        SendWidgetMessage(CWV_ONIESTATUETEXT, 0, (TMLParam)Text);
    }

    void TitleChange(BSTR Text)
    {
        WEBVIEW_TRACE(L"CIEExtend::TitleChange(%s)", Text);
//         CheckIEBug(); // have bug
        SendWidgetMessage(CWV_ONTITLECHANGE, 0, (TMLParam)Text);
    }

    void WindowClosing(VARIANT_BOOL IsChildWindow, VARIANT_BOOL *Cancel);

    ///////////////////////////////////////////////////////////////////////////
    // IDocHostUIHandler
    STDMETHODIMP ShowContextMenu(DWORD dwID, TMPoint* pptPosition, IUnknown* pCommandTarget, IDispatch* pDispatchObjectHit) tm_override;

    STDMETHODIMP GetExternal(IDispatch** ppDispatch) tm_override
    {
        WEBVIEW_TRACE(L"CIEExtend::GetExternal()");
        *ppDispatch = this;
        GetUnknown()->AddRef();
        return S_OK;
    }

    ///////////////////////////////////////////////////////////////////////////
    // IOleCommandTarget
    STDMETHODIMP QueryStatus(const GUID *pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT *pCmdText) tm_override
    {
        WEBVIEW_TRACE("CIEExtend::QueryStatus");
        return OLECMDERR_E_NOTSUPPORTED;
    }

    STDMETHODIMP Exec(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut) tm_override;
};

class TM_NO_VTABLE CIECont
    : public CActiveXCont
    , public CIEExtend
{
private:
    friend CIEExtend;
    CComBSTR m_strJavaScript;
    CComVariant m_varHead;
    double m_zoom;

private:
    TM_BGN_INTERFACE_MAP        (IOleClientSite)
        WEBVIEW_TRACE(TM_T("CIECont::QueryInterface:%s"),  DbgFormatGuid(iid).c_str());
        TM_INTERFACE_MAP        (CActiveXCont)
        if (m_pWnd->GetStyle() & WVS_EXTENDMODE)
            TM_INTERFACE_MAP    (CIEExtend)
    TM_END_INTERFACE_MAP        ()

public:
    CIECont() : m_zoom(1.0) {}
    ~CIECont() { WEBVIEW_TRACE("CIECont::~CIECont()"); }
    void Init(CTuiWebViewImpl* hWnd);

protected:
    HRESULT ExecScriptC(LPCTSTR code) { return ExecScript(CComBSTR(code).Detach()); } // execScript only support BSTR, not support const char;
    HRESULT ExecScript(BSTR code);
    TMBool TrySyncZoom(CTuiWebViewImpl* p);
    CComQIPtr<IHTMLDocument2> GetDocument();
    CComPtr<IHTMLWindow2> GetWindow();
    CComPtr<IHTMLScreen> GetScreen();
    CComPtr<IOmHistory> GetHistory();
    TMBool Navigate(LPCTSTR pUrl);
    void Stop();

    // IDummyImpl
    virtual TMBool ProcessWindowMessage(HTUI hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID) tm_override;
};


///////////////////////////////////////////////////////////////////////////////
// CTuiHotKeyImpl
class TM_NO_VTABLE CTuiWebViewImpl
    : public CTuiDelayDummy
{
public:
    TM_BGN_MASK_SYMBOL2 (CSymbol_Style,     CSymbol_DStyle)
        TM_MASK_SYMBOL  (WVS_NOCHROME)
        TM_MASK_SYMBOL  (WVS_NOIE)
        TM_MASK_SYMBOL  (WVS_EXTENDMODE)
        TM_MASK_SYMBOL  (WVS_EXTNOSCRIPT)
        TM_MASK_SYMBOL  (WVS_EXTNOCONTEXTMENU)
        TM_MASK_SYMBOL  (WVS_PRIVATEMODE)
        TM_MASK_BREAK   ()
    TM_END_MASK_SYMBOL  ()

    TM_DECLARE_FLAG_GET (_WVS_EXTENDMODE,       m_dwStyle,      WVS_EXTENDMODE);
    TM_DECLARE_FLAG_GET (_WVS_EXTNOCONTEXTMENU, m_dwStyle,      WVS_EXTNOCONTEXTMENU);
    TM_DECLARE_FLAG_GET (_WVS_PRIVATEMODE,      m_dwStyle,      WVS_PRIVATEMODE);
    TM_DECLARE_FLAG     (_WV_PRI_ZOOMED,        m_dwPriStyle,   WV_PRI_ZOOMED);

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_MASK   ("Style", m_dwStyle, CSymbol_Style)
        CHAIN_OBJ_PROP  (CTuiDummyBase)
    END_OBJ_PROP_MAP    ()
    
//     BEGIN_TUIMSG_MAP    (CTuiWebViewImpl)
//         MESSAGE_HANDLER (WM_NCCREATE,                     OnNcCreate)
//         CHAIN_MSG_MAP   (CTuiDummyBase)
//     END_TUIMSG_MAP      ()
// 
//     TMResult OnNcCreate(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

public:
    static TMBool HaveChrome(TMUInt32 dwStyle) { return !(dwStyle & WVS_NOCHROME); }
    CStringW MakeHeadStr(LPCWSTR str);
    virtual void Recreate() tm_override;

#ifdef TUI_EDIT_MODE
    CString GetDebugInfo() tm_override;
#endif // TUI_EDIT_MODE
};


TM_END_NAMESPACE

#endif // UIWEBVIEW_H
