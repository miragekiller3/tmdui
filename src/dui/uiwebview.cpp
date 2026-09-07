/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2020.10.16
 *
 *  Copyright (C) 2020 miragekiller
 */

#include "uiwebview.h"
#include "tmtime.h"

// #pragma comment (lib, "WebView2Guid.lib")
// #pragma comment (lib, "WebView2Loader.dll.lib")



TM_BGN_NAMESPACE_X(WebView2)

using namespace TM;

struct dll_WebView2Loader   { DECLARE_DLL_LOAD("WebView2Loader.dll", LOAD_LIBRARY_SEARCH_APPLICATION_DIR); };
DECLARE_DLL_FUNC(dll_WebView2Loader, HRESULT, STDAPICALLTYPE, CreateCoreWebView2EnvironmentWithOptions,     (PCWSTR browserExecutableFolder, PCWSTR userDataFolder, ICoreWebView2EnvironmentOptions* environmentOptions, ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* environment_created_handler));
// DECLARE_DLL_FUNC(dll_WebView2Loader, HRESULT, STDAPICALLTYPE, CreateCoreWebView2Environment,                (ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler* environment_created_handler));
// DECLARE_DLL_FUNC(dll_WebView2Loader, HRESULT, STDAPICALLTYPE, GetAvailableCoreWebView2BrowserVersionString, (PCWSTR browserExecutableFolder, LPWSTR* versionInfo));
// DECLARE_DLL_FUNC(dll_WebView2Loader, HRESULT, STDAPICALLTYPE, CompareBrowserVersions,                       (PCWSTR version1, PCWSTR version2, int* result));


///////////////////////////////////////////////////////////////////////////////
// ICoreWebView2NavigationStartingEventHandler
STDMETHODIMP CCoreWebView2NavigationStartingEventHandler<CChromeExt>::Invoke(
    ICoreWebView2* sender, struct ICoreWebView2NavigationStartingEventArgs* args)
{
    WEBVIEW_TRACE("CCoreWebView2NavigationStartingEventHandler::Invoke");

    CCoSTR _result;
    DbgVerifyHResult(args->get_uri(&_result));
    TMBool b = GetT()->SendWidgetMessage(CWV_ONNAVIGATESTART, 0, (TMLParam)_result.c_str());
    if (b)
        args->put_Cancel(b);
    return S_OK;
}

// CCoreWebView2DocumentTitleChangedEventHandler
STDMETHODIMP CCoreWebView2DocumentTitleChangedEventHandler<CChromeExt>::Invoke(ICoreWebView2* sender, IUnknown* args)
{
    WEBVIEW_TRACE("CCoreWebView2DocumentTitleChangedEventHandler::Invoke");
    CCoSTR _result;
    DbgVerifyHResult(sender->get_DocumentTitle(&_result));
    GetT()->SendWidgetMessage(CWV_ONTITLECHANGE, 0, (TMLParam)_result.c_str());
    return S_OK;
}

// CCoreWebView2HistoryChangedEventHandler
STDMETHODIMP CCoreWebView2HistoryChangedEventHandler<CChromeExt>::Invoke(ICoreWebView2* sender, IUnknown* args)
{
    WEBVIEW_TRACE("CCoreWebView2HistoryChangedEventHandler::Invoke");
    long canGoForward = false, canGoBack = false;
    DbgVerifyHResult(sender->get_CanGoForward(&canGoForward));
    DbgVerifyHResult(sender->get_CanGoBack(&canGoBack));
    HTUI hWnd = GetT()->GetWidget();
    hWnd->PostMessage(CWV_ONCOMMANDSTATE, CSC_NAVIGATEFORWARD, canGoForward);
    hWnd->PostMessage(CWV_ONCOMMANDSTATE, CSC_NAVIGATEBACK, canGoBack);
    return S_OK;
}

// CCoreWebView2NavigationCompletedEventHandler
STDMETHODIMP CCoreWebView2NavigationCompletedEventHandler<CChromeExt>::Invoke(
    ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args)
{
    WEBVIEW_TRACE("CCoreWebView2NavigationCompletedEventHandler::Invoke");
    long IsSuccess = false;
    DbgVerifyHResult(args->get_IsSuccess(&IsSuccess));
    if (IsSuccess)
        GetT()->SendWidgetMessage(CWV_ONNAVIGATECOMPLETE, 0, 0);
    else
    {
        COREWEBVIEW2_WEB_ERROR_STATUS status;
        DbgVerifyHResult(args->get_WebErrorStatus(&status));
        GetT()->SendWidgetMessage(CWV_ONNAVIGATEERROR, 0, status);
    }
    return S_OK;
}

// CCoreWebView2NavigationCompletedEventHandler
STDMETHODIMP CCoreWebView2WebMessageReceivedEventHandler<CChromeExt>::Invoke(
    ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args)
{
    WEBVIEW_TRACE("CCoreWebView2WebMessageReceivedEventHandler::Invoke");
    CCoSTR str;
    if (FAILED(args->TryGetWebMessageAsString(&str)))
        DbgVerifyHResult(args->get_webMessageAsJson(&str));
    if (StrCompare(str, -1, L"{\"Key\":\"contextmenu\"", 20) == 0) // "{"Key":"contextmenu","Value":{"X":1099,"Y":464}}"
    {
        CPoint pt;
        GetCursorPos(&pt);
        GetT()->SendWidgetMessage(WM_CONTEXTMENU, 0, pt.ToLPARAM());
    }
    else
        GetT()->SendWidgetMessage(CWV_ONWEBMESSAGE, 0, (TMLParam)str.c_str());
    return S_OK;
}

// CCoreWebView2ContainsFullScreenElementChangedEventHandler
STDMETHODIMP CCoreWebView2ContainsFullScreenElementChangedEventHandler<CChromeExt>::Invoke(ICoreWebView2 * sender, IUnknown* args)
{
    WEBVIEW_TRACE("CCoreWebView2ContainsFullScreenElementChangedEventHandler::Invoke");
    long containsFullscreenElement = false;
    DbgVerifyHResult(sender->get_ContainsFullScreenElement(&containsFullscreenElement));
    GetT()->SendWidgetMessage(CWV_ONFULLSCREEN, 0, containsFullscreenElement);
    return S_OK;
}

// CCoreWebView2WebResourceRequestedEventHandler
void CCoreWebView2HeaderRequestedEventHandler<CChromeExt>::WriteHead(ICoreWebView2HttpRequestHeaders* pHead)
{
    DbgAssert(pHead);
    TM::CStrViewW str;
    for (size_t pos = 0; pos = m_strHead.SplitLine(pos, str);)
    {
        TM::CStrViewW s0, s1;
        int n = str.Split(0, ':', s0);
        if (n < 1)
            continue;
        s1 = str.substr(n);
        s0 = s0.Trim();
        s1 = s1.Trim();
        TMCharW c1 = s0.end()[0]; (TMCharW&)(s0.end()[0]) = 0;
        TMCharW c2 = s1.end()[0]; (TMCharW&)(s1.end()[0]) = 0;
        pHead->SetHeader((TMCharW*)s0.data(), (TMCharW*)s1.data());
        (TMCharW&)(s0.end()[0]) = c1;
        (TMCharW&)(s1.end()[0]) = c2;
    }
//     TCHAR c[256];
//     GetT()->GetWidget()->GetModule()->GetLocal().ToString(c);
//     pHead->SetHeader(L"Accept-Language", c);
}

STDMETHODIMP CCoreWebView2HeaderRequestedEventHandler<CChromeExt>::Invoke(
    ICoreWebView2* sender, ICoreWebView2WebResourceRequestedEventArgs* args)
{
    WEBVIEW_TRACE("CCoreWebView2ContainsFullScreenElementChangedEventHandler::Invoke");
    
    // get interface
    CComPtr<ICoreWebView2WebResourceRequest> ptrRequest;
    DbgVerifyHResult(args->get_Request(&ptrRequest));
    DbgAssert(ptrRequest);
    CComPtr<ICoreWebView2HttpRequestHeaders> ptrHeader;
    DbgVerifyHResult(ptrRequest->get_Headers(&ptrHeader));
    DbgAssert(ptrRequest);

    // write head table
    DbgAssert(!m_strHead.IsEmpty());
    WriteHead(ptrHeader);

    return S_OK;
}

// CCoreWebView2ProcessFailedEventHandler
STDMETHODIMP CCoreWebView2ProcessFailedEventHandler<CChromeExt>::Invoke(
    ICoreWebView2* sender, ICoreWebView2ProcessFailedEventArgs* args)
{
    WEBVIEW_TRACE("CCoreWebView2ProcessFailedEventHandler::Invoke");
    COREWEBVIEW2_PROCESS_FAILED_KIND failureType;
    DbgVerifyHResult(args->get_ProcessFailedKind(&failureType));
    CTuiWebViewImpl* p = GetT()->GetWidget();
    p->PostMessage(CWV_ONPROCESSFAILED, 0, failureType);
    return S_OK;
}

// CCoreWebView2NewWindowRequestedEventHandler
struct CDeferralNewWindow
    : public CTuiWidgetImpl<CDeferralNewWindow, CTuiWebView>
//     , public CEnableSharedFromThis<CDeferralNewWindow>
{
    CComPtr<ICoreWebView2Deferral> m_deferral;
    CComPtr<ICoreWebView2NewWindowRequestedEventArgs> m_args;

    TMBool ProcessWindowMessage(
        HTUI hWnd, TMUInt uMsg, TMWParam wParam,
        TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID = 0)
    {
        if (uMsg == CWV_ONOBJINIT)
        {
            CComPtr<ICoreWebView2> ptrW2;
            this->QueryControl(&ptrW2);
            if (ptrW2)
                m_args->put_NewWindow(ptrW2);
            m_args->put_Handled(true);
            m_deferral->Complete();
            UnsubclassWindow();
        }
        else if (uMsg == WM_DESTROY)
            m_deferral->Complete();

        return false;
    }
};

// CCoreWebView2NewWindowRequestedEventHandler
STDMETHODIMP CCoreWebView2NewWindowRequestedEventHandler<CChromeExt>::Invoke(
    ICoreWebView2* sender, ICoreWebView2NewWindowRequestedEventArgs* args)
{
    WEBVIEW_TRACE("CCoreWebView2NewWindowRequestedEventHandler::Invoke");
    HTUI hWnd = 0;
    CCoSTR str;
    args->get_uri(&str);
    TMBool bCancel = GetT()->SendWidgetMessage(CWV_ONNEWWINDOW, (TMWParam)str.c_str(), (TMLParam)&hWnd);
    if (bCancel)
        args->put_Handled(true);
    else if (hWnd)
    {
        CTuiWebView wb(hWnd);
        CComPtr<ICoreWebView2> ptrW2;
        wb.QueryControl(&ptrW2);
        if (ptrW2)
        {
            args->put_NewWindow(ptrW2);
            args->put_Handled(true);
        }
        else
        {
            CComPtr<ICoreWebView2Environment> ptrEnv;
            wb.QueryControl(&ptrEnv);
            if (ptrEnv) // this is chrome webview an have not inited
            {
                CDeferralNewWindow::CPtr ptr;
                ptr.CreateInstance(),
                    (ptr->m_args = args),
                    args->GetDeferral(&ptr->m_deferral),
                    ptr->SubclassWindow(hWnd);
            }
        }
    }
    return S_OK;
}

// CCoreWebView2WindowCloseRequestedEventHandler
STDMETHODIMP CCoreWebView2WindowCloseRequestedEventHandler<CChromeExt>::Invoke(ICoreWebView2* sender, IUnknown* args)
{
    WEBVIEW_TRACE("CCoreWebView2WindowCloseRequestedEventHandler::Invoke");
    TMBool b = GetT()->SendWidgetMessage(CWV_ONWINDOWCLOSING, 0, 0);
    if (b)
        GetT()->GetWidget()->PostMessage(WM_CLOSE);
    return S_OK;
}

// CCoreWebView2AcceleratorKeyPressedEventHandler
// STDMETHODIMP CCoreWebView2AcceleratorKeyPressedEventHandler<CChromeExt>::Invoke(ICoreWebView2Controller* sender, ICoreWebView2AcceleratorKeyPressedEventArgs* args)
// {
//     unsigned int vk;
//     args->get_VirtualKey(&vk);
//     WEBVIEW_TRACE("CCoreWebView2AcceleratorKeyPressedEventHandler::Invoke(%d)", vk);
//     return S_OK;
// }


///////////////////////////////////////////////////////////////////////////////
// CChromeExt
CChromeCont* CChromeExt::GetThis()
{
    return static_cast<CChromeCont*>(this);
}

CTuiWebViewImpl* CChromeExt::GetWidget()
{
    return static_cast<CTuiWebViewImpl*>((static_cast<CChromeCont*>(this))->m_pWnd);
}

HRESULT CChromeExt::SendWidgetMessage(TMUInt msg, TMWParam wp, TMLParam lp)
{
    return GetThis()->m_pWnd->SendMessage(msg, wp, lp);
}

void CChromeExt::InitExt(ICoreWebView2* p, ICoreWebView2Controller* c)
{
    this->Add_NavigationStarting(p);
    this->Add_DocumentTitleChanged(p);
    this->Add_HistoryChanged(p);
    this->Add_NavigationCompleted(p);
    this->Add_WebMessageReceived(p);
    this->Add_ContainsFullScreenElementChanged(p);
    this->Add_ProcessFailed(p);
    this->Add_NewWindowRequested(p);
    this->Add_WindowCloseRequested(p);
//     this->Add_AcceleratorKeyPressed(c);
}

void CChromeExt::UninitExt(ICoreWebView2* p, ICoreWebView2Controller* c)
{
    this->Remove_NavigationStarting(p);
    this->Remove_DocumentTitleChanged(p);
    this->Remove_HistoryChanged(p);
    this->Remove_NavigationCompleted(p);
    this->Remove_WebMessageReceived(p);
    this->Remove_WebResourceRequested(p);
    this->Remove_ContainsFullScreenElementChanged(p);
    this->Remove_ProcessFailed(p);
    this->Remove_NewWindowRequested(p);
    this->Remove_WindowCloseRequested(p);
//     this->Remove_AcceleratorKeyPressed(c);
}



///////////////////////////////////////////////////////////////////////////////
// CChromeCont
CComPtr<ICoreWebView2Settings> CChromeCont::GetSetting()
{
    DbgAssert(m_ptrWebView);
    CComPtr<ICoreWebView2Settings> ptr;
    m_ptrWebView->get_Settings(&ptr);
    DbgAssert(ptr);
    return ptr;
}

void CChromeCont::OnStyleChanged(TMUInt32 nOld, TMUInt32 nNew)
{
    nOld ^= nNew;
    if (nOld & WVS_EXTNOSCRIPT)
        GetSetting()->put_IsScriptEnabled(!(nNew & WVS_EXTNOSCRIPT));
}

void CChromeCont::OnInit()
{
    WEBVIEW_TRACE("CChromeCont::OnInit()");
    TM::CTuiWebViewImpl* hWnd = GetWidget();

    OnStyleChanged(0, m_pWnd->m_dwStyle);
    m_pWnd->PostMessage(CWV_ONOBJINIT, 0, 0);
    m_ptrController->put_IsVisible(true);
    if (hWnd->IsLifeInvalid())
        return;
    this->SyncWin(m_pWnd, true);
    //m_pWnd->SetDirtyAndInvalidate();

    if (!hWnd->Is_WVS_EXTENDMODE())
        return;
    if (hWnd->IsLifeInvalid())
        return;

    InitExt(m_ptrWebView, m_ptrController);

    TMBool IsNoMenu = GetWidget()->Is_WVS_EXTNOCONTEXTMENU();
    GetThis()->GetSetting()->put_AreDefaultContextMenusEnabled(!IsNoMenu);
    if (IsNoMenu)
    {
        LPCWSTR pScript = 
            L"document.addEventListener('contextmenu', function(event) {\r\n"
            L"  let jsonObject =\r\n"
            L"  {\r\n"
            L"    Key: 'contextmenu',\r\n"
            L"    Value: { X: event.screenX, Y: event.screenY }\r\n"
            L"  };\r\n"
            L"  window.chrome.webview.postMessage(jsonObject);\r\n"
            L"});\r\n";
        m_ptrWebView->AddScriptToExecuteOnDocumentCreated((LPWSTR)pScript, 0);
    }
}

// IDummy
void CChromeCont::DestroyDummy(EUnbindDummy eReason)
{
    WEBVIEW_TRACE("CChromeCont::Destroy()");

    if (m_ptrWebView)
    {
        UninitExt(m_ptrWebView,  m_ptrController);
        m_ptrWebView.Release();
    }
    if (m_ptrController)
    {
        m_ptrController->Close(); // the function will call PeekMessage().
        m_ptrController.Release();
    }
    m_ptrEnv.Release();
    DetachWidget();

    HWND hWnd = UnbindHWND();
    if (hWnd && eReason != eUnbindDummyDetach) // Detach: call when the window destorying.
        ::DestroyWindow(hWnd);

    GetUnknown()->Release();
};

TMBool CChromeCont::ProcessWindowMessage(HTUI hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)
{
    if (m_ptrWebView)
    {
    switch (uMsg)
    {
    case WM_SETFOCUS:
        SetWndFocus();
        return false; // don't call CDummyChild::WM_SETFOCUS; don't install hook;

    case CWV_GETCONTROL:
        {
            WEBVIEW_TRACE("CChromeCont::CW_GETCONTROL");
            IUnknown* pUnk = GetUnknown();
            lResult = (TMResult)pUnk;
        }
        return true;

    case CWV_SETZOOM:
        {
            WEBVIEW_TRACE("CChromeCont::CW_SETZOOM");
            HRESULT hr;
            DbgVerifyHResult(hr = m_ptrController->put_ZoomFactor(*(double*)lParam));
            return SUCCEEDED(hr);
        }

    case CWV_GETZOOM:
        {
            WEBVIEW_TRACE("CChromeCont::CW_GETZOOM");
            HRESULT hr;
            DbgVerifyHResult(hr = m_ptrController->get_ZoomFactor((double*)lParam));
            return SUCCEEDED(hr);
        }

    case CWV_GETSOUCE:
        {
            WEBVIEW_TRACE("CChromeCont::CW_GETSOUCE");
            DbgAssert(lParam);
            HRESULT hr;
            CCoSTR bstr;
            DbgVerifyHResult(hr = m_ptrWebView->get_Source(&bstr));
            if (SUCCEEDED(hr))
                ((CStrPtr*)lParam)->MoveFromCustomString(bstr), lResult = true;
        }
        return true;

    case CWV_NAVIGATE:
        {
            WEBVIEW_TRACE("CChromeCont::CW_NAVIGATE");
            HRESULT hr;
            m_ptrWebView->RemoveWebResourceRequestedFilter((BSTR)L"*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL);
            Remove_WebResourceRequested(m_ptrWebView.p);
            m_strHead = GetWidget()->MakeHeadStr((TMStrW)lParam);
            m_ptrWebView->AddWebResourceRequestedFilter((BSTR)L"*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL);
            Add_WebResourceRequested(m_ptrWebView.p);
            DbgVerifyHResult(hr = m_ptrWebView->Navigate((TMStrW)wParam));
            lResult = SUCCEEDED(hr);
        }
        return true;

    case CWV_NAVIGATETOSTRING:
        {
            WEBVIEW_TRACE("CChromeCont::CW_NAVIGATE");
            HRESULT hr;
            DbgVerifyHResult(hr = m_ptrWebView->NavigateToString((TMStrW)lParam));
            lResult = SUCCEEDED(hr);
        }
        return true;

    case CWV_ADDDOCSCRIPT:
        {
            WEBVIEW_TRACE("CChromeCont::CW_ADDDOCSCRIPT");
            HRESULT hr;
            DbgVerifyHResult(hr = m_ptrWebView->AddScriptToExecuteOnDocumentCreated((TMStrW)lParam, 0));
            lResult = SUCCEEDED(hr);
        }
        return true;

    case CWV_EXECSCRIPT:
        {
            WEBVIEW_TRACE("CChromeCont::CW_EXECSCRIPT");
            HRESULT hr;
            DbgVerifyHResult(hr = m_ptrWebView->ExecuteScript((TMStrW)lParam, 0));
            lResult = SUCCEEDED(hr);
        }
        return true;

    case CWV_RELOAD:
        WEBVIEW_TRACE("CChromeCont::CW_RELOAD");
        lResult = Reload();
        return true;

    case CWV_GETPID:
        {
            WEBVIEW_TRACE("CChromeCont::CW_GETPID");
            HRESULT hr;
            unsigned int pid = 0;
            DbgVerifyHResult(hr = m_ptrWebView->get_BrowserProcessId(&pid));
            lResult = pid;
        }
        return true;

    case CWV_CANGOBACK:
        {
            WEBVIEW_TRACE("CChromeCont::CW_CANGOBACK");
            HRESULT hr;
            long n = 0;
            DbgVerifyHResult(hr = m_ptrWebView->get_CanGoBack(&n));
            lResult = n;
        }
        return true;

    case CWV_CANGOFORWARD:
        {
            WEBVIEW_TRACE("CChromeCont::CW_CANGOFORWARD");
            HRESULT hr;
            long n = 0;
            DbgVerifyHResult(hr = m_ptrWebView->get_CanGoForward(&n));
            lResult = n;
        }
        return true;

    case CWV_GOBACK:
        {
            WEBVIEW_TRACE("CChromeCont::CW_GOBACK");
            HRESULT hr;
            DbgVerifyHResult(hr = m_ptrWebView->GoBack());
            lResult = SUCCEEDED(hr);
        }
        return true;

    case CWV_GOFORWARD:
        {
            WEBVIEW_TRACE("CChromeCont::CW_GOFORWARD");
            HRESULT hr;
            DbgVerifyHResult(hr = m_ptrWebView->GoForward());
            lResult = SUCCEEDED(hr);
        }
        return true;

    case CWV_STOP:
        WEBVIEW_TRACE("CChromeCont::CW_STOP");
        DbgVerifyHResult(m_ptrWebView->Stop());
        return true;

    case CWV_GETTITLE:
        {
            WEBVIEW_TRACE("CChromeCont::CW_GETTITLE");
            CCoSTR bstr;
            HRESULT hr;
            DbgVerifyHResult(hr = m_ptrWebView->get_DocumentTitle(&bstr));
            if (SUCCEEDED(hr))
                ((CStrPtr*)lParam)->MoveFromCustomString(bstr), lResult = true;
        }
        return true;

    default:
        break;
    }
    }

    return CDummyChild::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
}


struct CChromeCreator
    : public ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler
    , public ICoreWebView2CreateCoreWebView2ControllerCompletedHandler
{
    CComPtr<ICoreWebView2Environment> m_ptrEnv;
    CComPtr<ICoreWebView2Controller> m_ptrController;
    CComPtr<ICoreWebView2> m_ptrWebView;
    CRefPtr<CTuiWebViewImpl> m_pWnd;
    CChromeCont* m_pCont;
    HWND m_hWnd;
    HWND m_hWalker;
    HRESULT m_hr;

    TM_UNUSED_IC_GetDebugInfo();

    void Clean()
    {
        if (m_hWnd)
        {
            DbgAssert(IsWindow(m_hWnd));
            ::DestroyWindow(m_hWnd);
        }
    }

    TM_VIRTUAL_ADDREF_RELEASE();

    TM_BGN_INTERFACE_MAP(ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler)
    TM_END_INTERFACE_MAP()

    // ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler
    HRESULT __stdcall Invoke(HRESULT hr, ICoreWebView2Environment* env) tm_override
    {
        m_hr = hr;
        if (FAILED(hr))
            return hr;
        m_ptrEnv = env;
        CreateBrowserControlsWebView();
        return m_hr;
    }

    // ICoreWebView2CreateCoreWebView2ControllerCompletedHandler
    HRESULT __stdcall Invoke(HRESULT hr, ICoreWebView2Controller* host) tm_override
    {
        m_hr = hr;
        DbgVerifyHResult(hr);
        if (FAILED(hr))
            return hr;
        // WebView created
        m_ptrController = host;
        DbgVerifyHResult(m_hr = m_ptrController->get_CoreWebView2(&m_ptrWebView));
    
        CComPtr<ICoreWebView2Settings> settings;
        DbgVerifyHResult(hr = m_ptrWebView->get_Settings(&settings));
        DbgVerifyHResult(hr = settings->put_AreDevToolsEnabled(FALSE));

        if (m_pWnd->IsLifeInvalid())
            return E_FAIL;
        if (m_pCont != m_pWnd->m_ptrDummyImpl)
            return E_FAIL;
        DbgAssert(m_pCont->m_ptrEnv == m_ptrEnv);
        m_pCont->m_ptrController = m_ptrController;
        m_pCont->m_ptrWebView = m_ptrWebView;

        ::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (TMLParam)m_pCont);
        ::SetWindowLongPtr(m_hWalker, GWLP_USERDATA, (TMLParam)m_pCont);

        m_pCont->OnInit();
        return S_OK;
    }

    static void PutVisible(HWND hWnd, BOOL isVisible)
    {
        CChromeCont* p = (CChromeCont*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
        if (p && p->m_ptrController)
            p->m_ptrController->put_IsVisible(isVisible);
    }

    // other
    static LRESULT WINAPI _WNDPROC(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
    {
        if (message == WM_SIZE)
        {
            CChromeCont* p = (CChromeCont*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
            if (p && p->m_ptrController)
            {
                CRect rc(0, 0, LOWORD(lparam), HIWORD(lparam));
                p->m_ptrController->put_Bounds(rc);
            }
        }
        else if (message == WM_WINDOWPOSCHANGED)
        {
            WINDOWPOS* pw = (WINDOWPOS*)lparam;
            if (pw->flags & SWP_SHOWWINDOW)
                PutVisible(hWnd, true);
            else if (pw->flags & SWP_HIDEWINDOW)
                PutVisible(hWnd, false);
        }
        
        return DefWindowProc(hWnd, message, wparam, lparam);
    }

    static LRESULT WINAPI _WalkPROC(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
    {
        if (message == WM_SETFOCUS)
        {
            DbgTrace("CChromeCreator::_WNDPROC(WM_SETFOCUS, %p)", hWnd);
            CChromeCont* p = (CChromeCont*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
            if (p)
            {
                TMBool bReverse = g_input.m_key.IsKeyTab() < 0; //CKeyboard::IsKeyDownShift();
                p->GetWidget()->PostMessage(WM_NEXTDLGCTL, bReverse, false);
            }
        }

        return DefWindowProc(hWnd, message, wparam, lparam);
    }

    void CreateBrowserControlsWebView()
    {
        if (!m_hWnd)
        {
            LPCWSTR p = TUI_CLASS_WIN_CHROME;
            static ATOM atom = CWindowAT::NativeRegister(p);
            m_hWnd = CWindowAT::NativeCreate1(p, m_pWnd->GetHWND(), &_WNDPROC, 0, WS_CHILD, WS_EX_CONTROLPARENT);
            LPCWSTR p2 = TUI_CLASS_WIN_CHROME_W;
            static ATOM atom2 = CWindowAT::NativeRegister(p2);
            m_hWalker = CWindowAT::NativeCreate1(p2, m_hWnd, &_WalkPROC, 0, WS_CHILD | WS_TABSTOP | WS_VISIBLE);
        }
        DbgAssert(::IsWindow(m_hWnd));
        if (m_pWnd->Is_WVS_PRIVATEMODE())
        {
            CComQIPtr<ICoreWebView2Environment10> env10(m_ptrEnv);
            DbgAssert(env10);
            CComPtr<ICoreWebView2ControllerOptions> ptrOptions;
            DbgVerifyHResult(env10->CreateCoreWebView2ControllerOptions(&ptrOptions));
            DbgAssert(ptrOptions);
            DbgVerifyHResult(ptrOptions->put_IsInPrivateModeEnabled(true));
            m_hr = env10->CreateCoreWebView2ControllerWithOptions((wireHWND)m_hWnd, ptrOptions, this);
        }
        else
            m_hr = m_ptrEnv->CreateCoreWebView2Controller((wireHWND)m_hWnd, this);
    }

    HRESULT _CreateEnv(LPCWSTR pExec, LPCWSTR pUser, ICoreWebView2EnvironmentOptions* pOption)
    {
        tm_try
        {
            // have WebView2Loader.dll but no edge, it will throw exception.
            return _CreateCoreWebView2EnvironmentWithOptions(
                pExec,
                pUser,
                pOption, this);
        }
        tm_catch_all()
        {
        }
        return E_UNEXPECTED;
    }

    TMBool Done(LPCWSTR pExec, LPCWSTR pUser, ICoreWebView2EnvironmentOptions* pOption)
    {
        HRESULT hr;
        DbgVerifyHResult(hr = _CreateEnv(pExec, pUser, pOption));

        if (FAILED(hr))
        {
            WEBVIEW_TRACE(L"CChromeCreator::Done(%s, %s) faile", pExec, pUser);
            return false;
        }

        DbgAssert(::IsWindow(m_hWnd));

        return true;
    }

    static CChromeCreator* Inst(CTuiWebViewImpl* hWnd)
    {
        CChromeCreator* p = new CRefObjectT<CChromeCreator>(0);
        p->m_pWnd = hWnd;
        p->m_hWnd = 0;
        return p;
    }
};

struct CCoreWebView2EnvironmentOptions
    : public TM::CBaseRefT<CCoreWebView2EnvironmentOptions, ICoreWebView2EnvironmentOptions>
    , public TM::CRefObjectRoot<0>
{
private:
    CRefPtr<CTuiWebViewImpl> m_hWnd;

    TM_BGN_INTERFACE_MAP        (CCoreWebView2EnvironmentOptions)
        WEBVIEW_TRACE(TM_T("CCoreWebView2EnvironmentOptions::QueryInterface:%s"),  DbgFormatGuid(iid).c_str());
        TM_INTERFACE_HANDLER    (ICoreWebView2EnvironmentOptions)
    TM_END_INTERFACE_MAP        ()

public:
    CCoreWebView2EnvironmentOptions(CTuiWebViewImpl* h) : m_hWnd(h) {}
        
    // ICoreWebView2EnvironmentOptions
    STDMETHODIMP get_AdditionalBrowserArguments(LPWSTR *value) tm_override
    {
        HRESULT hr;
        CString& strArguments = CView::s_strWebview2Arguments;
        if (!strArguments)
            hr = E_NOTIMPL;
        else
            hr = (*value = TM::CCoSTR::MakeCoMemStringLen(strArguments, strArguments.GetLength()), S_OK);
        WEBVIEW_TRACE(L"CCoreWebView2EnvironmentOptions::get_AdditionalBrowserArguments(%s), return:%x", *value, hr);
        return hr;
    }
    
    STDMETHODIMP put_AdditionalBrowserArguments(LPWSTR value) tm_override
    {
        WEBVIEW_TRACE(L"CCoreWebView2EnvironmentOptions::put_AdditionalBrowserArguments(%s)", value);
        return E_NOTIMPL;
    }
    
    STDMETHODIMP get_Language(LPWSTR *value) tm_override
    {
        HRESULT hr;
        CString& strLanguage = CView::s_strWebview2Language;
        if (!strLanguage)
            hr = E_NOTIMPL;
        else
            hr = (*value = TM::CCoSTR::MakeCoMemStringLen(strLanguage, strLanguage.GetLength()), S_OK);
        WEBVIEW_TRACE(L"CCoreWebView2EnvironmentOptions::get_Language(%s) return:%x", *value, hr);
        return hr;
    }
    
    STDMETHODIMP put_Language(LPWSTR value) tm_override
    {
        WEBVIEW_TRACE(L"CCoreWebView2EnvironmentOptions::put_Language(%s)", value);
        return E_NOTIMPL;
    }
    
    STDMETHODIMP get_TargetCompatibleBrowserVersion(LPWSTR *value) tm_override
    {
        *value = TM::CCoSTR::MakeCoMemString(CORE_WEBVIEW_TARGET_PRODUCT_VERSION);
        WEBVIEW_TRACE(L"CCoreWebView2EnvironmentOptions::get_TargetCompatibleBrowserVersion(%s)", *value);
        return S_OK;
    }
    
    STDMETHODIMP put_TargetCompatibleBrowserVersion(LPWSTR value) tm_override
    {
        WEBVIEW_TRACE(L"CCoreWebView2EnvironmentOptions::put_TargetCompatibleBrowserVersion(%s)", value);
        return E_NOTIMPL;
    }
    
    STDMETHODIMP get_AllowSingleSignOnUsingOSPrimaryAccount(long *allow) tm_override
    {
        *allow = CView::s_bWebview2AllowSingleSignOnUsingOSPrimaryAccount;
        WEBVIEW_TRACE(L"CCoreWebView2EnvironmentOptions::get_AllowSingleSignOnUsingOSPrimaryAccount(%d)", *allow);
        return S_OK;
    }
    
    STDMETHODIMP put_AllowSingleSignOnUsingOSPrimaryAccount(long allow) tm_override
    {
        WEBVIEW_TRACE(L"CCoreWebView2EnvironmentOptions::put_AllowSingleSignOnUsingOSPrimaryAccount(%d)", allow);
        return E_NOTIMPL;
    }
};

CChromeCont* CChromeCont::InitInstance(CTuiWebViewImpl* hWnd, HINSTANCE hInstance, int nCmdShow)
{
    DbgAssert(CTuiWebViewImpl::HaveChrome(hWnd->GetStyle()));

    if (!_CreateCoreWebView2EnvironmentWithOptions)
        return 0;

    CView* view = hWnd->GetView();
    BROWSERFOLDERS executable, userData;
    view->s_strWebview2ExecutableFolder.CopyTo(executable.szFolder[0]);
    view->s_strWebview2UserDataFolder.CopyTo(userData.szFolder[0]);
    executable.szFolder[1][0] = userData.szFolder[1][0] = 0;
    hWnd->SendMessage(CWV_ONCRGETFOLDER, (TMWParam)&executable, (TMLParam)&userData);
    WEBVIEW_TRACE(L"CIEExtend::InitInstance::CWV_ONCRGETFOLDER() E0:%s, U0:%s, E1:%s, U1:%s",
        executable.szFolder[0], userData.szFolder[0],
        executable.szFolder[1], userData.szFolder[1]);

    if (hWnd->IsLifeInvalid())
        return 0;

    CRefPtr<CChromeCreator> creator(CChromeCreator::Inst(hWnd));
    CRefPtr<ICoreWebView2EnvironmentOptions> ptrOption(new CCoreWebView2EnvironmentOptions(hWnd));
    while(1)
    {
        if (creator->Done(
            executable.szFolder[0][0] ? executable.szFolder[0] : 0,
            userData.szFolder[0][0] ? userData.szFolder[0] : 0, ptrOption))
            break;
        
        if (!executable.szFolder[0][0] && !executable.szFolder[1][0])
        {
            creator->Clean();
            return 0;
        }

        if (creator->Done(
            executable.szFolder[1][0] ? executable.szFolder[1] : 0,
            userData.szFolder[1][0] ? userData.szFolder[1] : 0, ptrOption))
            break;

        creator->Clean();
        return 0;
    }

    if (hWnd->IsLifeInvalid())
        return 0;

    WebView2::CChromeCont* window = new CRefObjectT<WebView2::CChromeCont>(); //(hWnd->m_ptrDummyImpl, hWnd);
    window->AttachWidget(hWnd);
    creator->m_pCont = window;
    window->m_ptrEnv = creator->m_ptrEnv;
    window->BindHWND(creator->m_hWnd);
    return window;
}

TMBool CChromeCont::Reload()
{
    HRESULT hr;
    DbgVerifyHResult(hr = m_ptrWebView->Reload());
    return SUCCEEDED(hr);
}

// edge source code:(I guess)
// OnFocus(HWND hPre): if (GetNextDlgTabItem(hPre) == this) { SetHTMLFocus(0); }
void CChromeCont::SetWndFocus()
{
    if (GetTargetFocus())
        return;
    HWND hWnd = m_wnd.GetWindow(GW_CHILD);
    HWND hWnd2 = ::GetWindow(hWnd, GW_HWNDNEXT);
    ::SendMessage(hWnd2, WM_SETFOCUS, (LPARAM)hWnd, 0);
}

TM_END_NAMESPACE_X(WebView2)


TM_BGN_NAMESPACE

///////////////////////////////////////////////////////////////////////////////
// CIEExtend
CIECont* CIEExtend::GetThis()
{
    return static_cast<CIECont*>(this);
}

CTuiWebViewImpl* CIEExtend::GetWidget()
{
    return static_cast<CTuiWebViewImpl*>((static_cast<CIECont*>(this))->m_pWnd);
}

HRESULT CIEExtend::SendWidgetMessage(TMUInt msg, TMWParam wp, TMLParam lp)
{
    return GetWidget()->SendMessage(msg, wp, lp);
}


///////////////////////////////////////////////////////////////////////////////
// IDispatch
STDMETHODIMP CIEExtend::GetIDsOfNames(REFIID riid, OLECHAR** rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
{
    WEBVIEW_TRACE(L"CIEExtend::GetIDsOfNames(cNames:%d, rgszNames:%s)", cNames, rgszNames[0]);
    if (cNames == 1 && 0 == StrCompare(L"postMessage", rgszNames[0]))
         return rgDispId[0] = e_DISPID_postMessage, S_OK;
    return E_NOTIMPL;
}

STDMETHODIMP CIEExtend::Invoke(DISPID dispidMember, REFIID riid,
                               LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult,
                               EXCEPINFO* pexcepinfo, UINT* puArgErr)
{
    WEBVIEW_TRACE(L"CIEExtend::Invoke(dispidMember:%d)", dispidMember);
//     CheckIEBug(); // have bug

    if (dispidMember == e_DISPID_postMessage && pdispparams->cArgs == 1 && pdispparams->rgvarg->vt == VT_BSTR)
        return SendWidgetMessage(CWV_ONWEBMESSAGE, 0, (TMLParam)pdispparams->rgvarg->bstrVal), S_OK;

//     if (this->GetAmbientProp(dispidMember, pvarResult))
//         return S_OK;

    return CWebBrowserEventT<CIEExtend>::Invoke(dispidMember, riid, lcid, wFlags,
        pdispparams, pvarResult, pexcepinfo, puArgErr);
}

// DWebBrowserEvents2
void CIEExtend::DLControl(long *pFlag)
{
    WEBVIEW_TRACE("CIEExtend::DLControl");
//     CheckIEBug(); // have bug
    if (GetWidget()->GetStyle() & WVS_EXTNOSCRIPT)
        *pFlag |= DLCTL_NO_SCRIPTS;
    SendWidgetMessage(CWV_ONIEDLCONTROL, 0, (TMLParam)pFlag);
}

// HRESULT CIEExtend::UserAgent(VARIANT* pvarResult)
// {
//     WEBVIEW_TRACE("CIEExtend::UserAgent");
//     *((CComVariant*)pvarResult) = L"MyWebBrowserHost";
//     return S_OK;
// }

void CIEExtend::NavigateComplete2(IDispatch *pDisp, VARIANT *URL)
{
    WEBVIEW_TRACE("CIEExtend::NavigateComplete2()");
//         CheckIEBug(); // no bug

    CTuiWebViewImpl* p = GetWidget();
    if (!p->Is_WV_PRI_ZOOMED())
    {
        Ref ref(p);
        GetThis()->TrySyncZoom(p);
        if (p->IsLifeInvalid())
            return;
    }
    
    if (m_spNavigateToString && 0 == StrCompareI(URL->bstrVal, L"about:blank"))
    {
        CComQIPtr<IPersistStreamInit> spPSI(GetThis()->GetDocument());
        DbgAssert(spPSI);
//         DbgVerifyHResult(spPSI->InitNew());
        DbgVerifyHResult(spPSI->Load(m_spNavigateToString));
        m_spNavigateToString.Release();
        return;
    }

    p->SendMessage(CWV_ONNAVIGATECOMPLETE, 0, 0);
}

void CIEExtend::NewWindow3(IDispatch** ppDisp, VARIANT_BOOL* Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl)
{
    WEBVIEW_TRACE(L"CIEExtend::NewWindow3(flag:%X, bstrUrlContext:%s, bstrUrl:%s)", dwFlags, bstrUrlContext, bstrUrl);
//         CheckIEBug(); // no bug
    HTUI hWnd = 0;
    TMBool bCancel = SendWidgetMessage(CWV_ONNEWWINDOW, (TMWParam)bstrUrl,  (TMLParam)&hWnd);
    if (bCancel)
        *Cancel = VARIANT_TRUE;
    else if (hWnd == GetWidget())
    {
        *Cancel = VARIANT_TRUE;
        GetThis()->Navigate(bstrUrl);
    }
    else if (hWnd)
    {
        CTuiWebView web(hWnd);
        CComPtr<IWebBrowser2> ptr;
        web.QueryControl(&ptr);
        if (ptr)
            ptr->get_Application(ppDisp);
    }
} 

void CIEExtend::DocumentComplete(IDispatch *pDisp, VARIANT *URL)
{
    WEBVIEW_TRACE("CIEExtend::DocumentComplete()");
//         CheckIEBug(); // no bug

    CIECont* p = GetThis();
    p->ExecScriptC(L"window.chrome = new Object(); window.chrome.webview = window.external;");
    p->ExecScript(p->m_strJavaScript);
    SendWidgetMessage(CWV_ONIEDOCCOMPLETE, 0, (TMLParam)pDisp);
}

void CIEExtend::WindowClosing(VARIANT_BOOL IsChildWindow, VARIANT_BOOL *Cancel)
{
    WEBVIEW_TRACE(L"CIEExtend::WindowClosing(%d)", IsChildWindow);
//     CheckIEBug(); // not check
    TMBool b = SendWidgetMessage(CWV_ONWINDOWCLOSING, 0, IsChildWindow); 
    if (b)
        GetWidget()->PostMessage(WM_CLOSE);
    else
        *Cancel = VARIANT_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// IDocHostUIHandler
STDMETHODIMP CIEExtend::ShowContextMenu(DWORD dwID, TMPoint* pptPosition, IUnknown* pCommandTarget, IDispatch* pDispatchObjectHit)
{
    WEBVIEW_TRACE("CIEExtend::ShowContextMenu()");
//         CheckIEBug(); // no bug
    if (!GetWidget()->Is_WVS_EXTNOCONTEXTMENU())
        return S_FALSE;
    SendWidgetMessage(WM_CONTEXTMENU, 0, ((CPoint*)pptPosition)->ToLPARAM());
    return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
// IOleCommandTarget

#define XOLECMDID_SHOWSCRIPTERROR    40
#define XOLECMDID_SHOWMESSAGE        41
#define XOLECMDID_SHOWFIND            42
#define XOLECMDID_SHOWPAGESETUP        43
#define XOLECMDID_SHOWPRINT            44

STDMETHODIMP CIEExtend::Exec(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT *pvaIn, VARIANT *pvaOut)
{
    WEBVIEW_TRACE(L"CIEExtend::Exec(nCmdID:%d)", nCmdID);
//     CheckIEBug();
    static CLSID const CGID_DocHostCommandHandler
        = { 0xf38bc242, 0xb950, 0x11d1, { 0x89,0x18,0x00,0xc0,0x4f,0xc2,0xc8,0x36 } };
    if (pguidCmdGroup && pvaIn && pvaOut && (*pguidCmdGroup == CGID_DocHostCommandHandler))
    {
#ifdef TM_DBG
        CComPtr<IHTMLWindow2> ptrWin(GetThis()->GetWindow());
        CComPtr<IHTMLEventObj> ptrEvent;
        
        DbgAssert(ptrWin);
        DbgVerifyHResult(ptrWin->get_event(&ptrEvent));
        CComDispatchDriver dispx(ptrEvent);
        if (dispx)
        {
            if (nCmdID == XOLECMDID_SHOWSCRIPTERROR)
            {
                CComVariant varLine, varCharacter, varCode, varMsg, varURL;
                DbgVerifyHResult(dispx.GetPropertyByName(L"errorLine", &varLine));
                DbgVerifyHResult(dispx.GetPropertyByName(L"errorCharacter", &varCharacter));
                DbgVerifyHResult(dispx.GetPropertyByName(L"errorCode", &varCode));
                DbgVerifyHResult(dispx.GetPropertyByName(L"errorMessage", &varMsg));
                DbgVerifyHResult(dispx.GetPropertyByName(L"errorUrl", &varURL));
                DbgTrace(L"CIEExtend::Exec script error 40: %s(line:%d-character:%d-code:%d) %s",
                    varURL.bstrVal, varLine.lVal, varCharacter.lVal, varCode.lVal, varMsg.bstrVal);
            }
            else if (nCmdID == XOLECMDID_SHOWMESSAGE)
            {
                CComVariant varRet;
                DbgVerifyHResult(dispx.GetPropertyByName(L"messageText", &varRet));
                DbgTrace(L"CIEExtend::Exec messageText 41: %s", varRet.bstrVal);
            }
        }
#endif
        pvaOut->vt = VT_BOOL;
        pvaOut->boolVal = SendWidgetMessage(CWV_ONIEEXEC, nCmdID, (TMLParam)pvaIn->punkVal) ? VARIANT_FALSE : VARIANT_TRUE;
        return S_OK;
    }
    return OLECMDERR_E_NOTSUPPORTED;
}

void CIECont::Init(CTuiWebViewImpl* hWnd)
{
    DbgAssert(m_spUnknown);
    if (hWnd->Is_WVS_EXTENDMODE())
        DispEventAdvise(m_spUnknown);
}

HRESULT CIECont::ExecScript(BSTR code)
{
    if (code)
    {
        CComPtr<IHTMLWindow2> ptrWin(GetWindow());
        if (ptrWin)
        {
            C3rdAutoDestroy<CIECont> _auto(this);
            HRESULT hr;
            VARIANT vRet;
            vRet.vt = VT_EMPTY;
            DbgVerifyHResult(hr = ptrWin->execScript(code, 0, &vRet));
            return hr;
        }
    }
    return E_FAIL;
}

static HRESULT _CIECont_SyncZoom(IWebBrowser2* p, CComVariant& varZoom)
{
#ifndef OLECMDID_OPTICAL_ZOOM
#   define OLECMDID_OPTICAL_ZOOM 63
#endif

    TM_TRY_ALL(return p->ExecWB((OLECMDID)OLECMDID_OPTICAL_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER, &varZoom, NULL));
    return E_UNEXPECTED;
}

TMBool CIECont::TrySyncZoom(CTuiWebViewImpl* p)
{
    int nDpi;
    if (CAppAT::IsDPIAware())
        nDpi = CInt::Round(m_zoom * (double)p->GetDPI() * 100.0 / 96.0);
    else
    {
        CComQIPtr<MSHTML::IHTMLScreen3> ptr3(GetScreen());
        if (!ptr3)
        {
            p->Reset_WV_PRI_ZOOMED();
            return false;
        }
        long nZoomSys;
        DbgVerifyHResult(ptr3->get_systemXDPI(&nZoomSys));
        nDpi = CInt::Round(m_zoom * (double)p->GetDPI() * (double)nZoomSys * 100.0 / 96.0 / 96.0);
    }

    CComQIPtr<IWebBrowser2> ptr(m_spUnknown);
    DbgAssert(ptr);
    CComVariant varZoom(nDpi); 
    HRESULT hr;
    DbgVerifyHResult(hr = _CIECont_SyncZoom(ptr, varZoom));
    TMBool bRet = SUCCEEDED(hr);
    p->Setting_WV_PRI_ZOOMED(bRet);
    
#ifdef TM_DBG
    CComQIPtr<MSHTML::IHTMLScreen2> ptr2(GetScreen());
    CComQIPtr<MSHTML::IHTMLScreen3> ptr3(GetScreen());
    if (ptr2 && ptr3)
    {
        long nZoomSys, nZoomLogic;
        long x;
        DbgVerifyHResult(ptr3->get_systemXDPI(&nZoomSys));
        DbgVerifyHResult(ptr2->get_logicalXDPI(&nZoomLogic));
        DbgVerifyHResult(ptr2->get_deviceXDPI(&x));
        WEBVIEW_TRACE("CIECont::SyncZoom(set:%d, dpiDevice:%d, dpiLogic:%d, dpiSystem:%d, dpiWidget:%d)",
            nDpi, x, nZoomLogic, nZoomSys, p->GetDPI());
    }
#endif
    
    return bRet;
}

CComQIPtr<IHTMLDocument2> CIECont::GetDocument()
{
    CComQIPtr<IWebBrowser2> ptr(m_spUnknown);
    DbgAssert(ptr);
    CComPtr<IDispatch> ptrDisp;
    ptr->get_Document(&ptrDisp);
    return CComQIPtr<IHTMLDocument2>(ptrDisp);
}

CComPtr<IHTMLWindow2> CIECont::GetWindow()
{
    CComQIPtr<IHTMLDocument2> ptr(GetDocument());
    if (!ptr)
        return 0;
    CComPtr<IHTMLWindow2> ptrWin;
    ptr->get_parentWindow(&ptrWin);
    return ptrWin;
}

CComPtr<IHTMLScreen> CIECont::GetScreen()
{
    CComPtr<IHTMLScreen> ptr;
    CComPtr<IHTMLWindow2> ptrWin(GetWindow());
    if (ptrWin)
        ptrWin->get_screen(&ptr);
    return ptr;
}

CComPtr<IOmHistory> CIECont::GetHistory()
{
    CComPtr<IOmHistory> ptrHistroy;
    CComPtr<IHTMLWindow2> ptrWin(GetWindow());
    if (ptrWin)
        ptrWin->get_history(&ptrHistroy);
    return ptrHistroy;
}

TMBool CIECont::Navigate(LPCTSTR pUrl)
{
    StopNoRedirect();

    CComQIPtr<IWebBrowser2> ptr(m_spUnknown);
    DbgAssert(ptr);
    HRESULT hr;
    DbgVerifyHResult(hr = ptr->Navigate2(tm_addressof(CComVariant(pUrl)), 0, 0, 0, &m_varHead));
    return SUCCEEDED(hr);
}

void CIECont::Stop()
{
    C3rdAutoDestroy<CIECont> _auto(this);
    CComQIPtr<IWebBrowser2> ptr(m_spUnknown);
    DbgAssert(ptr);
    DbgVerifyHResult(ptr->Stop());
}

// IDummy
TMBool CIECont::ProcessWindowMessage(HTUI hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)
{
    CRefPtr<IOleClientSite> ref(this);

    switch (uMsg)
    {
    case WM_DESTROY:
        if (m_spUnknown)
            DispEventUnadvise(m_spUnknown);
        break;

    case CWV_SETZOOM:
        WEBVIEW_TRACE("CIECont::CW_SETZOOM");
        DbgAssert(!hWnd->IsLifeDestroyed());
        DbgAssert(lParam);
        m_zoom = *(double*)lParam;
        lResult = TrySyncZoom(static_cast<CTuiWebViewImpl*>(hWnd));
        return true;

    case CM_ZOOMED:
        TrySyncZoom(static_cast<CTuiWebViewImpl*>(hWnd));
        break;

    case CWV_GETZOOM:
        WEBVIEW_TRACE("CIECont::CW_GETZOOM");
        DbgAssert(!hWnd->IsLifeDestroyed());
        DbgAssert(lParam);
        *(double*)lParam = m_zoom;//(double)GetNativeZoom() / (double)m_pWnd->GetDPI();
        return true;

    case CWV_GETSOUCE:
        {
            WEBVIEW_TRACE("CIECont::CW_GETSOUCE");
            DbgAssert(!hWnd->IsLifeDestroyed());
            DbgAssert(lParam);
            CComQIPtr<IWebBrowser2> ptr(m_spUnknown);
            DbgAssert(ptr);
            HRESULT hr;
            CComBSTR bstr;
//                DbgVerifyHResult(hr = ptr->get_Path(&bstr));
            DbgVerifyHResult(hr = ptr->get_LocationURL(&bstr));
            if (SUCCEEDED(hr))
            {
                CStrPtr& str = *(CStrPtr*)lParam;
                str.MoveFromCustomString(bstr);
                lResult = true;
            }
        }
        return true;
        
    case CWV_NAVIGATE:
        {
            WEBVIEW_TRACE("CIECont::CW_NAVIGATE");
            DbgAssert(!hWnd->IsLifeDestroyed());
            C3rdAutoDestroy<CIECont> _auto(this);
            m_spNavigateToString.Release();
            m_varHead = GetWidget()->MakeHeadStr((TMCStrW)lParam);
            lResult = Navigate((TMCStrW)wParam);
        }
        return true;
        
    case CWV_NAVIGATETOSTRING:
        {
            WEBVIEW_TRACE("CIECont::CW_NAVIGATE");
            DbgAssert(!hWnd->IsLifeDestroyed());
            C3rdAutoDestroy<CIECont> _auto(this);

            // make stream;
            LPCWSTR p = (LPCWSTR)lParam;
            int nSize = p ? StrLen(p) : 0;
            m_spNavigateToString.Release();
//             TM::W2UTF utf(p);
//            StreamCreateFromMem(utf.c_str(), utf.size(), &m_spNavigateToString);
            StreamCreateFromMem(p, nSize * sizeof(*p), &m_spNavigateToString);

            m_varHead.Clear();
            lResult = Navigate(L"about:blank");
        }
        return true;

    case CWV_ADDDOCSCRIPT:
        {
            WEBVIEW_TRACE("CIECont::CW_ADDDOCSCRIPT");
            DbgAssert(!hWnd->IsLifeDestroyed());
            m_strJavaScript += (TMCStrW)lParam;
        }
        return lResult = true;

    case CWV_EXECSCRIPT:
        WEBVIEW_TRACE("CIECont::CW_EXECSCRIPT");
        DbgAssert(!hWnd->IsLifeDestroyed());
        lResult = SUCCEEDED(ExecScriptC((LPCTSTR)lParam));
        return true;

    case CWV_RELOAD:
        {
            WEBVIEW_TRACE("CIECont::CW_RELOAD");
            DbgAssert(!hWnd->IsLifeDestroyed());
            CComPtr<IHTMLWindow2> ptrWin(GetWindow());
            if (ptrWin)
            {
                CComPtr<IHTMLLocation> ptrLocation;
                ptrWin->get_location(&ptrLocation);
                if (ptrLocation)
                {
                    C3rdAutoDestroy<CIECont> _auto(this);
                    HRESULT hr;
                    DbgVerifyHResult(hr = ptrLocation->reload(true));
                    lResult = SUCCEEDED(hr);
                }
            }
        }
        return true;

    case CWV_GETPID:
        WEBVIEW_TRACE("CIECont::CW_GETPID");
        DbgAssert(!hWnd->IsLifeDestroyed());
        lResult = GetCurrentProcessId();
        return true;

    case CWV_CANGOBACK:
        {
            WEBVIEW_TRACE("CIECont::CW_CANGOBACK");
            DbgAssert(!hWnd->IsLifeDestroyed());
            CComPtr<IOmHistory> ptrHistroy(GetHistory());
            if (ptrHistroy)
            {
                HRESULT hr;
                short s = 0;
                DbgVerifyHResult(hr = ptrHistroy->get_length(&s));
                lResult = s;
            }
        }
        return true;
        
    case CWV_CANGOFORWARD:
        lResult = 0;
        return true;

    case CWV_GOBACK:
        {
            WEBVIEW_TRACE("CIECont::CW_GOBACK");
            DbgAssert(!hWnd->IsLifeDestroyed());
            CComPtr<IOmHistory> ptrHistroy(GetHistory());
            if (ptrHistroy)
            {
                C3rdAutoDestroy<CIECont> _auto(this);
                HRESULT hr;
                DbgVerifyHResult(hr = ptrHistroy->back(0));
                lResult = SUCCEEDED(hr);
            }
        }
        return true;

    case CWV_GOFORWARD:
        {
            WEBVIEW_TRACE("CIECont::CW_GOFORWARD");
            DbgAssert(!hWnd->IsLifeDestroyed());
            CComPtr<IOmHistory> ptrHistroy(GetHistory());
            if (ptrHistroy)
            {
                C3rdAutoDestroy<CIECont> _auto(this);
                HRESULT hr;
                DbgVerifyHResult(hr = ptrHistroy->forward(0));
                lResult = SUCCEEDED(hr);
            }
        }
        return true;

    case CWV_STOP:
        WEBVIEW_TRACE("CIECont::CW_STOP");
        DbgAssert(!hWnd->IsLifeDestroyed());
        Stop();
        return true;

    case CWV_GETTITLE:
        {
            WEBVIEW_TRACE("CIECont::CW_GETTITLE");
            DbgAssert(!hWnd->IsLifeDestroyed());
            CComQIPtr<IHTMLDocument2> ptr(GetDocument());
            if (ptr)
            {
                HRESULT hr;
                CComBSTR bstr;
                DbgVerifyHResult(hr = ptr->get_title(&bstr));
                if (SUCCEEDED(hr))
                    ((CStrPtr*)lParam)->MoveFromCustomString(bstr), lResult = true;
            }
        }
        return true;

    default:
        break;
    }
    
    return CActiveXCont::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
}


///////////////////////////////////////////////////////////////////////////////
// CTuiWebViewImpl
TM_REGISTER_CLASS1(CTuiWebView);

void CTuiWebViewImpl::Recreate()
{
    DbgAssert(!IsLifeInvalid());

    if (HaveChrome(m_dwStyle))
    {
        WebView2::CChromeCont* window = WebView2::CChromeCont::InitInstance(this, 0, 0);
        if (window)
        {
            DbgAssert(window == m_ptrDummyImpl);
            return;
        }
    }
    
    if (!m_ptrDummyImpl && !(m_dwStyle & WVS_NOIE))
    {
//        CIECont* p = new CRefObjectT<C3rdObjectT<CIECont> >();
        CIECont* p = new CRefObjectT<CIECont>();
        if (p->AttachWidget(this, CLSID_WebBrowser)) // L"{8856F961-340A-11D0-A96B-00C04FD705A2}")
            p->Init(this);
    }
}

// IE will overwrite "Accept-Language" to system language
CStringW CTuiWebViewImpl::MakeHeadStr(LPCWSTR pHead)
{
    if (pHead && wcsstr(pHead, L"Accept-Language:"))
        return pHead;

    TCHAR c[256];
    GetModule()->GetLocal().ToString(c);
    CStringW str(L"Accept-Language: ");
    str += c;
    str += L"\r\n";
    str += pHead;
    return str;
}

// CTuiWgt
#ifdef TUI_EDIT_MODE

CString CTuiWebViewImpl::GetDebugInfo()
{
    TM::CString strSrc, strTitle;
    CTuiWebView wb(this);
    wb.GetSource(strSrc);
    int pid = wb.GetBrowserProcessId();
    wb.GetDocumentTitle(strTitle);
    TM::CString str;
    str.Format(L"\r\n\r\npid:0X%X;\r\nsrc:%s;\r\ntitle:%s;", pid, strSrc.c_str(), strTitle.c_str());

    return CTuiDummyBase::GetDebugInfo() +  str;
}

#endif // TUI_EDIT_MODE


TM_END_NAMESPACE


