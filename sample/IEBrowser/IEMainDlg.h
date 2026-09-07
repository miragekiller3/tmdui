#ifndef MAIN_H
#define MAIN_H

#include "Resource.h"
#include "tuires.h"
//#include "IEContainerWnd.h"
#include "../h/zoom.h"

#import "Microsoft.Web.WebView2/WebView2.tlb" no_implementation, raw_interfaces_only



TUI_BGN_NAMESPACE


////////////////////////////////////////////////////////////////////////////////
// class CWebBrowser
// support DISPID_AMBIENT_DLCONTROL:


#ifndef DOCHOSTUIFLAG_DPI_AWARE
#   define DOCHOSTUIFLAG_DPI_AWARE                         0x40000000
#endif

template <class T_Wnd>
class CDocHostInterfaceT
    : public IDocHostUIHandler
    , public IDocHostShowUI
{
public:
    T_Wnd* m_wnd;
    TM_UNUSED_IC_GetDebugInfo();

    TM_BGN_INTERFACE_MAP(IDocHostUIHandler)
        if (m_wnd && *m_wnd && m_wnd->m_bSupportDocHostShowUI)
            TM_INTERFACE_HANDLER(IDocHostUIHandler)
            if (m_wnd && *m_wnd && m_wnd->m_bSupportDocHostShowUI)
                TM_INTERFACE_HANDLER(IDocHostShowUI)
                TM_END_INTERFACE_MAP()

#define DHCALL  if (!m_wnd || !*m_wnd) return S_FALSE; return m_wnd->

                void Set(T_Wnd* p)
            {
                m_wnd = p;
            }

    // IDocHostUIHandler
    // MSHTML requests to display its context menu
    STDMETHOD(ShowContextMenu)(DWORD dwID, TMPointD* pptPosition, IUnknown* pCommandTarget, IDispatch* pDispatchObjectHit)
    {
        DHCALL ShowContextMenu(dwID, pptPosition, pCommandTarget, pDispatchObjectHit);
    }

    // Called at initialisation to find UI styles from container
    STDMETHOD(GetHostInfo)(DOCHOSTUIINFO* pInfo)
    {
        DHCALL GetHostInfo(pInfo);
    }

    // Allows the host to replace the IE4/MSHTML menus and toolbars. 
    STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject* pActiveObject, IOleCommandTarget* pCommandTarget, IOleInPlaceFrame* pFrame, IOleInPlaceUIWindow* pDoc)
    {
        DHCALL ShowUI(dwID, pActiveObject, pCommandTarget, pFrame, pDoc);
    }

    // Called when IE4/MSHTML removes its menus and toolbars. 
    STDMETHOD(HideUI)()
    {
        DHCALL HideUI();
    }

    // Notifies the host that the command state has changed. 
    STDMETHOD(UpdateUI)()
    {
        DHCALL UpdateUI();
    }

    // Called from the IE4/MSHTML implementation of IOleInPlaceActiveObject::EnableModeless
    STDMETHOD(EnableModeless)(TMBool fEnable)
    {
        DHCALL EnableModeless(fEnable);
    }

    // Called from the IE4/MSHTML implementation of IOleInPlaceActiveObject::OnDocWindowActivate
    STDMETHOD(OnDocWindowActivate)(TMBool fActivate)
    {
        DHCALL OnDocWindowActivate(fActivate);
    }

    // Called from the IE4/MSHTML implementation of IOleInPlaceActiveObject::OnFrameWindowActivate. 
    STDMETHOD(OnFrameWindowActivate)(TMBool fActivate)
    {
        DHCALL OnFrameWindowActivate(fActivate);
    }

    // Called from the IE4/MSHTML implementation of IOleInPlaceActiveObject::ResizeBorder.
    STDMETHOD(ResizeBorder)(const TMRect* prcBorder, IOleInPlaceUIWindow* pUIWindow, TMBool fFrameWindow)
    {
        DHCALL ResizeBorder(prcBorder, pUIWindow, fFrameWindow);
    }

    // Called by IE4/MSHTML when IOleInPlaceActiveObject::TranslateAccelerator or IOleControlSite::TranslateAccelerator is called. 
    STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID* pguidCmdGroup, DWORD nCmdID)
    {
        DHCALL TranslateAccelerator(lpMsg, pguidCmdGroup, nCmdID);
    }

    // Returns the registry key under which IE4/MSHTML stores user preferences. 
    // Returns S_OK if successful, or S_FALSE otherwise. If S_FALSE, IE4/MSHTML will default to its own user options.
    STDMETHOD(GetOptionKeyPath)(BSTR* pbstrKey, DWORD dwReserved)
    {
        DHCALL GetOptionKeyPath(pbstrKey, dwReserved);
    }

    // Called by IE4/MSHTML when it is being used as a drop target to allow the host to supply an alternative IDropTarget
    STDMETHOD(GetDropTarget)(IDropTarget* pDropTarget, IDropTarget** ppDropTarget)
    {
        DHCALL GetDropTarget(pDropTarget, ppDropTarget);
    }

    // Called by IE4/MSHTML to obtain the host's IDispatch interface
    STDMETHOD(GetExternal)(IDispatch** ppDispatch)
    {
        DHCALL GetExternal(ppDispatch);
    }

    // Called by IE4/MSHTML to allow the host an opportunity to modify the URL to be loaded
    STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR* pchURLIn, OLECHAR** ppchURLOut)
    {
        DHCALL TranslateUrl(dwTranslate, pchURLIn, ppchURLOut);
    }

    // Called on the host by IE4/MSHTML to allow the host to replace IE4/MSHTML's data object.
    // This allows the host to block certain clipboard formats or support additional clipboard formats. 
    STDMETHOD(FilterDataObject)(IDataObject* pDO, IDataObject** ppDORet)
    {
        DHCALL FilterDataObject(pDO, ppDORet);
    }

    // IDocHostShowUI
    STDMETHOD(ShowMessage)(TMHWnd hwnd, LPOLESTR lpstrText, LPOLESTR lpstrCaption,
        DWORD dwType, LPOLESTR lpstrHelpFile, DWORD dwHelpContext,
        TMResult __RPC_FAR* plResult)
    {
        DHCALL ShowMessage(hwnd, lpstrText, lpstrCaption, dwType, lpstrHelpFile, dwHelpContext, plResult);
    }

    STDMETHOD(ShowHelp)(TMHWnd hwnd, LPOLESTR pszHelpFile, TMUInt uCommand,
        DWORD dwData, TMPointD ptMouse, IDispatch __RPC_FAR* pDispatchObjectHit)
    {
        DHCALL ShowHelp(hwnd, pszHelpFile, uCommand,
            dwData, ptMouse, pDispatchObjectHit);
    }
};


template < class T >
class  CWebBrowserT
    : public CTuiActiveXT<CAutoTuiWidget>
    , public IDocHostUIHandler
    , public IDocHostShowUI
{
public:
    typedef CRefObjectT<CDocHostInterfaceT<CWebBrowserT> > dochost;
    typedef CTuiActiveXT<CAutoTuiWidget> CTuiActiveX;

    bool m_bSupportDocHostUIHandler;
    bool m_bSupportDocHostShowUI;
    CRefPtr<dochost> m_ptrHost;

public:
    // IUnknown
    TM_UNUSED_ADDREF_RELEASE();

    TM_BGN_INTERFACE_MAP(IDocHostUIHandler)
        TM_END_INTERFACE_MAP()


        //     STDMETHOD(QueryInterface) (REFIID riid,
        //         void __RPC_FAR *__RPC_FAR *ppvObject)
        //     {
        //         if (m_bSupportDocHostUIHandler && riid == IID_IDocHostUIHandler)
        //         {
        //             IDocHostUIHandler* p = this;
        //             *ppvObject = p;
        //         }
        //         else if (m_bSupportDocHostShowUI && riid == IID_IDocHostShowUI)
        //         {
        //             IDocHostShowUI* p = this;
        //             *ppvObject = p;
        //         }
        //         else
        //             return E_NOTIMPL;
        //         
        //         return S_OK;
        //     }

public:
    CWebBrowserT()
        : m_bSupportDocHostUIHandler(true)
        , m_bSupportDocHostShowUI(true)
    {
        m_ptrHost = new CRefObjectT<CDocHostInterfaceT<CWebBrowserT> >(0);
        m_ptrHost->Set(this);
    }

    ~CWebBrowserT()
    {
        m_ptrHost->Set(0);
    }

    void Attach(HTUI hWnd)
    {
        CTuiActiveX::SubclassWindow(hWnd);
        //         IDocHostUIHandler* p = this;
        CTuiActiveX::SetExtUnkown(m_ptrHost->GetUnknown());
    }

    void operator = (HTUI hWnd)
    {
        Attach(hWnd);
    }

    void SupportDocHostUIHandler(TMBool bSupport)
    {
        m_bSupportDocHostUIHandler = (0 != bSupport);
    }

    void SupportDocHostShowUI(TMBool bSupport)
    {
        m_bSupportDocHostShowUI = (0 != bSupport);
    }


    // IDocHostUIHandler
    // MSHTML requests to display its context menu
    STDMETHOD(ShowContextMenu)(DWORD dwID, TMPointD* pptPosition, IUnknown* pCommandTarget, IDispatch* pDispatchObjectHit)
    {
        return S_FALSE;
    }

    // Called at initialisation to find UI styles from container
    STDMETHOD(GetHostInfo)(DOCHOSTUIINFO* pInfo)
    {
        pInfo->cbSize = sizeof(DOCHOSTUIINFO);
        pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
        pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_DISABLE_SCRIPT_INACTIVE | DOCHOSTUIFLAG_DPI_AWARE;
        return S_OK;
    }

    // Allows the host to replace the IE4/MSHTML menus and toolbars. 
    STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject* pActiveObject, IOleCommandTarget* pCommandTarget, IOleInPlaceFrame* pFrame, IOleInPlaceUIWindow* pDoc)
    {
        return S_FALSE;
    }

    // Called when IE4/MSHTML removes its menus and toolbars. 
    STDMETHOD(HideUI)()
    {
        return S_FALSE;
    }

    // Notifies the host that the command state has changed. 
    STDMETHOD(UpdateUI)()
    {
        return S_FALSE;
    }

    // Called from the IE4/MSHTML implementation of IOleInPlaceActiveObject::EnableModeless
    STDMETHOD(EnableModeless)(TMBool fEnable)
    {
        return S_FALSE;
    }

    // Called from the IE4/MSHTML implementation of IOleInPlaceActiveObject::OnDocWindowActivate
    STDMETHOD(OnDocWindowActivate)(TMBool fActivate)
    {
        return S_FALSE;
    }

    // Called from the IE4/MSHTML implementation of IOleInPlaceActiveObject::OnFrameWindowActivate. 
    STDMETHOD(OnFrameWindowActivate)(TMBool fActivate)
    {
        return S_FALSE;
    }

    // Called from the IE4/MSHTML implementation of IOleInPlaceActiveObject::ResizeBorder.
    STDMETHOD(ResizeBorder)(const TMRect* prcBorder, IOleInPlaceUIWindow* pUIWindow, TMBool fFrameWindow)
    {
        return S_FALSE;
    }

    // Called by IE4/MSHTML when IOleInPlaceActiveObject::TranslateAccelerator or IOleControlSite::TranslateAccelerator is called. 
    STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID* pguidCmdGroup, DWORD nCmdID)
    {
        return S_FALSE;
    }

    // Returns the registry key under which IE4/MSHTML stores user preferences. 
    // Returns S_OK if successful, or S_FALSE otherwise. If S_FALSE, IE4/MSHTML will default to its own user options.
    STDMETHOD(GetOptionKeyPath)(BSTR* pbstrKey, DWORD dwReserved)
    {
        return S_FALSE;
    }

    // Called by IE4/MSHTML when it is being used as a drop target to allow the host to supply an alternative IDropTarget
    STDMETHOD(GetDropTarget)(IDropTarget* pDropTarget, IDropTarget** ppDropTarget)
    {
        return S_FALSE;
    }

    // Called by IE4/MSHTML to obtain the host's IDispatch interface
    STDMETHOD(GetExternal)(IDispatch** ppDispatch)
    {
        return S_FALSE;
    }

    // Called by IE4/MSHTML to allow the host an opportunity to modify the URL to be loaded
    STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR* pchURLIn, OLECHAR** ppchURLOut)
    {
        return S_FALSE;
    }

    // Called on the host by IE4/MSHTML to allow the host to replace IE4/MSHTML's data object.
    // This allows the host to block certain clipboard formats or support additional clipboard formats. 
    STDMETHOD(FilterDataObject)(IDataObject* pDO, IDataObject** ppDORet)
    {
        return S_FALSE;
    }

    // IDocHostShowUI
    STDMETHOD(ShowMessage)(TMHWnd hwnd, LPOLESTR lpstrText, LPOLESTR lpstrCaption,
        DWORD dwType, LPOLESTR lpstrHelpFile, DWORD dwHelpContext,
        TMResult __RPC_FAR* plResult)
    {
        return S_FALSE;
    }

    STDMETHOD(ShowHelp)(TMHWnd hwnd, LPOLESTR pszHelpFile, TMUInt uCommand,
        DWORD dwData, TMPointD ptMouse, IDispatch __RPC_FAR* pDispatchObjectHit)
    {
        return S_FALSE;
    }

};

typedef CWebBrowserT<CAutoTuiWidget>                        CWclWebBrowser;

TM_END_NAMESPACE


class CIEMainDlg
    : public TM::CTuiWidgetImpl<CIEMainDlg, TM::CTuiWindow>
    , public TM::CWebBrowserEventT<CIEMainDlg>
    , public CZoomImplT<CIEMainDlg>

//    , public TM::CHookMouse
{
public:
    virtual TMULong GetIDD() {return IDD;}
    enum { IDD = IDD_TUI_MANAGE_10016 };

    int s_bTestDestroy;

    TM::CTuiWebView m_IEWnd;
    TM::CTuiMemo m_listBox;
    CComPtr<IUnknown> m_pEventAdvise;
    int m_nOpenMode;

    TM::CWclWebBrowser m_wb;

    CIEMainDlg() : s_bTestDestroy(0) {}

public:
    BEGIN_TUIMSG_MAP(CMain)
        if (s_bTestDestroy > 0 && --s_bTestDestroy == 0)
        {
            //m_IEWnd.DestroyWindow();
            ::DestroyWindow(GetHWND());
            return 0;
        }

        MESSAGE_HANDLER(WM_INITDIALOG,         OnInitDialog)
        MESSAGE_HANDLER(WM_DESTROY,            OnDestroy)
        ROUTE_ID_HANDLER   (IDC_IECONTROL, WM_DESTROY,             OnIEDestroy)

//         MESSAGE_HANDLER(CM_ZOOMED,             OnZoomed)

        COMMAND_ID_HANDLER  (IDCANCEL,         OnCancel)
        COMMAND_ID_HANDLER  (IDOK,             OnOk)
        COMMAND_ID_HANDLER  (IDC_DESTROY,      OnDestroyCmd)
        
//          ROUTE_ID_HANDLER    (IDOK, WM_LBUTTONDOWN,             OnRouteCancel)
//          ROUTE_ID_HANDLER    (IDC_MOUSE, WM_LBUTTONDOWN,           OnRouteMouseLButtonDown)
//          ROUTE_ID_HANDLER    (IDC_MOUSE, WM_LBUTTONUP,             OnRouteMouseLButtonUp)
//          COMMAND_ID_HANDLER  (IDC_MOUSE,        OnMouse)
        COMMAND_ID_HANDLER  (IDC_HIDE,         OnHide)

        
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_RECREATE,       OnRecreate)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_CLEAREVENT,     OnClearEvent)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_GETSRC,         OnGetSrc)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_NSHEADER,       OnNavigateHead)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_NSTRING,        OnNavigateSting)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_EXECSCRPIT,     OnExecScript)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_RELOAD,         OnReload)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_GETPID,         OnGetPID)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_GETHWND,        OnGetHWND)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_CANBACK,        OnCanBack)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_BACK,           OnBack)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_CANFORWARD,     OnCanForward)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_FORWARD,        OnForward)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_GETTITLE,       OnGetTitle)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_ZOOMIN,         OnZoomIn)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_ZOOMOUT,        OnZoomOut)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_GETZOOM,        OnGetZoom)

        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_NV_IEX,         OnNavigateIEX)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_SETPINK,        OnSetPink)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_SETWHITE,       OnSetWhite)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_GETINFO,        OnGetInfo)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_OPEN1,          OnOpen)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_OPEN2,          OnOpen)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_OPEN3,          OnOpen)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_OPEN4,          OnOpen)
        COMMAND_ID_HANDLER  (IDC_PUSHBUTTON_STOP,           OnStop)
        

        ROUTE_RANGE_ID_HANDLER(IDC_IECONTROL, CWV_ONWEBMESSAGE, CWV_END, OnWebview)
        ROUTE_ID_HANDLER(IDC_IECONTROL, WM_CONTEXTMENU,     OnWebview)
        ROUTE_ID_HANDLER(IDC_IECONTROL, CWV_ONOBJINIT,      OnWebview)
        ROUTE_ID_HANDLER(IDC_IECONTROL, CWV_ONWEBMESSAGE,   OnWebMessage)
        ROUTE_ID_HANDLER(IDC_IECONTROL, CWV_ONNEWWINDOW,    OnNewWidnow)
        ROUTE_ID_HANDLER(IDC_IECONTROL, CWV_ONCRGETFOLDER,  OnGetFold)
        
         
//         COMMAND_ID_HANDLER  (IDGO,              OnGo)
        CHAIN_MSG_MAP       (CZoomImpl)
    END_MSG_MAP()

    TMResult OnRouteCancel(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
    {
        //EndDialog(0);
        DestroyWindow();
        bHandled = false;
        return 0;
    }

//     TMResult OnRouteMouseLButtonDown(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
//     {
//         TM::CTuiCheckBox wnd(pMsg->hWnd);
//         if (!wnd.GetCheck())
//             wnd.SetCapture();
// 
//         bHandled = false;
//         return 0;
//     }

//     TMResult OnRouteMouseLButtonUp(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
//     {
//         TM::CTuiCheckBox wnd(pMsg->hWnd);
//         if (wnd.IsCapture())
//         {
//             m_ptTarget = pMsg->lParam;
//             wnd.ClientToScreen(&m_ptTarget, 1);
//             m_hTarget = WindowFromPoint(m_ptTarget);
//             ::ScreenToClient(m_hTarget, &m_ptTarget);
//             KillAni();
//             SetAni(3000);
//             _tuiApp.ReleaseCapture();
//         }
// 
//         bHandled = false;
//         return 0;
//     }

//     TMResult OnMouse(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
//     {
//         TM::CTuiCheckBox wnd((TM::HTUI)hWndCtl);
//         if (!wnd.GetCheck())
//             KillAni();
//         return 0;
//     }

    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

    TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        m_listBox.Detach();
        bHandled = false;
        return 0;

    }

    TMResult OnIEDestroy(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
    {
        if (m_pEventAdvise)
        {
            DispEventUnadvise(m_pEventAdvise);
            m_pEventAdvise.Release();
        }
        m_IEWnd.Detach();

        bHandled = false;
        return 0;
    }


//     TMResult OnZoomed(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

    TMResult OnOk(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    TMResult OnDestroyCmd(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        HWND hWnd = CWindow(GetHWND()).GetWindow(GW_CHILD);
        ::DestroyWindow(hWnd);
        return 0;
    }

    
    TMResult OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        ::DestroyWindow(GetHWND());
        return 0;
    }

    TMResult OnHide(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        TM::CTuiButton but(Item(IDC_HIDE));
        TM::CTuiWidget par(Item(IDC_WINDOW_21028));
        if (but.GetCheck())
            par.ShowWindow(SW_HIDE);
        else
            par.ShowWindow(SW_SHOW);
        return 0;
    }

    TMResult OnClearEvent(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        m_listBox.SetSelAll();
        m_listBox.Clear();
        return 0;
    }

    TMResult OnRecreate(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

    TMResult OnGetSrc(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        TM::CString str;
        m_IEWnd.GetSource(str);
        Log(_T("GetSouce:%s"), str.c_str());
        return 0;
    }

    TMResult OnNavigateHead(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        TCHAR c[512];
        GetDlgItemText(IDW_EDIT_20017, c, 511);
        Log(_T("OnNavigateHead"));

        LPCWSTR pHead = 
            L"Accept: text/html,application/xhtml+xml,application/xml,tui/xml\r\n"
            L"User-Agent: Mozilla/5.0 (Windows NT 5.1; rv:52.0; TUI webview:20201111)\r\n"
            L"Accept-Language: ja-JP\r\n";
        TMBool b = m_IEWnd.Navigate(c, pHead);
        if (!b)
            Log(_T("- NavigateHead fail"));
        return 0;
    }

    TMResult OnNavigateSting(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        TCHAR c[512];
        GetDlgItemText(IDW_EDIT_20017, c, 511);
        Log(_T("NavigateToString"));

        TMBool b = m_IEWnd.NavigateToString(c);
        if (!b)
            Log(_T("- NavigateToString fail"));
        return 0;
    }

    TMResult OnExecScript(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        TCHAR c[512];
        GetDlgItemText(IDW_EDIT_20017, c, 511);
        Log(_T("ExecuteScript"));
        TMBool b = m_IEWnd.ExecuteScript(c);
        if (!b)
            Log(_T("- ExecuteScript fail"));
        return 0;
    }
    
    TMResult OnReload(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        m_IEWnd.Reload();
        Log(_T("Reload"));
        return 0;
    }
    
    TMResult OnGetPID(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        int n = m_IEWnd.GetBrowserProcessId();
        Log(_T("GetBrowserProcessId:%d; self:%d"), n, GetCurrentProcessId());
        return 0;
    }

    TMResult OnGetHWND(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        HWND n = m_IEWnd.GetWindow();
        Log(_T("GetWindow:0X%p"), n);
        return 0;
    }

    TMResult OnCanBack(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        int n = m_IEWnd.CanGoBack();
        Log(_T("CanGoBack:%d"), n);
        return 0;
    }

    TMResult OnBack(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        m_IEWnd.GoBack();
        Log(_T("GoBack"));
        return 0;
    }

    TMResult OnCanForward(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        int n = m_IEWnd.CanGoForward();
        Log(_T("CanGoForward:%d"), n);
        return 0;
    }

    TMResult OnForward(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        m_IEWnd.GoForward();
        Log(_T("GoForward"));
        return 0;
    }

    TMResult OnGetTitle(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        TM::CString str;
        m_IEWnd.GetDocumentTitle(str);
        Log(_T("GetTitle:%s"), str.c_str());
        return 0;
    }

    void SetZoom(int dx)
    {
        static double s_nZoom = 100;
        s_nZoom += dx;
        m_IEWnd.SetZoomFactor(s_nZoom / 100.0);
        Log(_T("SetZoom:%g"), s_nZoom / 100.0f);
    }

    TMResult OnZoomIn(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        SetZoom(-10);
        return 0;
    }
    
    TMResult OnZoomOut(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        SetZoom(10);
        return 0;
    }

    TMResult OnGetZoom(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        double n = m_IEWnd.GetZoomFactor();
        Log(_T("GetZoom:%g"), n);
        return 0;
    }

    TMResult OnNavigateIEX(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        Log(_T("OnNavigateIEX"));
        LPCTSTR pf[] =
        {
            L"5",
            L"6",
            L"7",
            L"8",
            L"9",
            L"10",
            L"11",
            L"edge",
            L"edge,chrome=1",
        };
        static int i = 0;
        if (i >= tm_countof(pf))
            i = 0;
        TM::CResource res;
        res.Load(_ModuleATL.GetResourceInstance(), (LPCTSTR)IDR_HTML_TEST1, (LPCTSTR)RT_HTML);
        TM::CString str;
        str.Format((LPCWSTR)res.GetData(), pf[i], pf[i]);
        ++i;

        TMBool b = m_IEWnd.NavigateToString(str);
        if (!b)
            Log(_T("- OnNavigateIEX fail"));
        return 0;
    }

    TMResult OnSetPink(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        Log(_T("OnSetPink"));
        LPCTSTR p = L"document.body.bgColor='#ff00ff';";
        TMBool b = m_IEWnd.ExecuteScript(p);
        if (!b)
            Log(_T("- OnSetPink fail"));
        return 0;
    }

    void ExecuteScript(LPCTSTR pScript)
    {
        TMBool b = m_IEWnd.ExecuteScript(pScript);
        if (!b)
            Log(_T("ExecuteScript() fail"));
    }

    TMResult OnSetWhite(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        Log(_T("OnSetWhite"));
        LPCTSTR p = L"document.body.bgColor='white';";
        ExecuteScript(p);
        return 0;
    }

    TMResult OnGetInfo(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        Log(_T("OnGetInfo"));
        ExecuteScript(L"window.chrome.webview.postMessage('\tnavigator.appCodeName = ' + navigator.appCodeName)");
        ExecuteScript(L"window.chrome.webview.postMessage('\tnavigator.appMinorVersion = ' + navigator.appMinorVersion)");
        ExecuteScript(L"window.chrome.webview.postMessage('\tnavigator.appName = ' + navigator.appName)");
        ExecuteScript(L"window.chrome.webview.postMessage('\tnavigator.appVersion = ' + navigator.appVersion)");
        ExecuteScript(L"window.chrome.webview.postMessage('\tnavigator.connectionSpeed = ' + navigator.connectionSpeed)");
        ExecuteScript(L"window.chrome.webview.postMessage('\tnavigator.cookieEnabled = ' + navigator.cookieEnabled)");
        ExecuteScript(L"window.chrome.webview.postMessage('\tnavigator.cpuClass = ' + navigator.cpuClass)");
        ExecuteScript(L"window.chrome.webview.postMessage('\tnavigator.javaEnabled = ' + navigator.javaEnabled())");
        ExecuteScript(L"window.chrome.webview.postMessage('\tnavigator.mimeTypes = ' + navigator.mimeTypes)");
        ExecuteScript(L"window.chrome.webview.postMessage('\tnavigator.onLine = ' + navigator.onLine)");
        ExecuteScript(L"window.chrome.webview.postMessage('\tnavigator.platform = ' + navigator.platform)");
        ExecuteScript(L"window.chrome.webview.postMessage('\tnavigator.taintEnabled = ' + navigator.taintEnabled())");
        ExecuteScript(L"window.chrome.webview.postMessage('\tnavigator.userAgent = ' + navigator.userAgent)");

        ExecuteScript(L"window.chrome.webview.postMessage('\tnavigator.browserLanguage = ' + navigator.browserLanguage)");
        ExecuteScript(L"window.chrome.webview.postMessage('\tnavigator.systemLanguage = ' + navigator.systemLanguage)");
        ExecuteScript(L"window.chrome.webview.postMessage('\tnavigator.userLanguage = ' + navigator.userLanguage)");
        ExecuteScript(L"window.chrome.webview.postMessage('\tnavigator.language = ' + navigator.language)");
        ExecuteScript(L"window.chrome.webview.postMessage('\tnavigator.languages = ' + navigator.languages)");
        return 0;
    }
    

    TMResult OnOpen(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        Log(_T("OnOpen"));
        m_nOpenMode = wID;

        TCHAR c[512];
        GetDlgItemText(IDW_EDIT_20017, c, 511);
        TM::CString str;
        str.Format(L"window.open(\"%s\");", c);
        TMBool b = m_IEWnd.ExecuteScript(str);
        if (!b)
            Log(_T("- OnOpen fail"));
        return 0;
    }

    TMResult OnStop(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        Log(_T("OnStop"));
        m_IEWnd.Stop();
        return 0;
    }
    
    TMResult OnWebview(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
    {
        if (m_listBox)
        {
            TM::CString str(DbgFormatMessage(pMsg->message, pMsg->wParam, pMsg->lParam));
            Log2(str);
            if (pMsg->message == CWV_ONOBJINIT)
            {
                CComPtr<WebView2::ICoreWebView2Environment> ptr1;
                CComPtr<WebView2::ICoreWebView2Controller> ptr2;
                CComPtr<WebView2::ICoreWebView2> ptr3;
                CComPtr<IWebBrowser2> ptr4;
                TM::CTuiWebView v1(pMsg->hWnd);
                v1.QueryControl(&ptr1);
                v1.QueryControl(&ptr2);
                v1.QueryControl(&ptr3);
                v1.QueryControl(&ptr4);
                Log(L"Environment:0x%p, Controller:0x%p, WebView2:0x%p, WebBrowser:0x%p", ptr1.p, ptr2.p, ptr3.p, ptr4.p);
            }
        }
        bHandled = false;
        return 0;
    }

    TMResult OnWebMessage(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
    {
        LPCTSTR p = (LPCTSTR)pMsg->lParam;
        if (p[0] != '\t')
            Log(L"receive web message:%s", p);

//        ::MessageBox(GetHWND(), , L"receive web message", MB_OK);
        bHandled = false;
        return 0;
    }

    TMResult OnNewWidnow(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
    {
        TMCStrW strUrl = (TMCStrW)pMsg->wParam;
        if (m_nOpenMode == IDC_PUSHBUTTON_OPEN3) // cancel it
            return true;
        TM::HTUI* hWnd = (TM::HTUI*)pMsg->lParam;
        if (m_nOpenMode == IDC_PUSHBUTTON_OPEN2) // open in self
            *hWnd = m_IEWnd;
        else if (m_nOpenMode == IDC_PUSHBUTTON_OPEN4) // open in new webview
        {
            CIEMainDlg::CPtr ptr;
            ptr.CreateInstance()->Create((HWND)0);
            *hWnd = ptr->m_IEWnd;
        }
        return false;
    }

    TMResult OnGetFold(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
    {
        TMStrW p = (TMStrW)pMsg->wParam;
        TCHAR c[512];
        GetDlgItemText(IDW_EDIT_20017, c, 511);
        StrCopy(p, 2048, c);
        return true;
    }

    

//    TMResult OnGo(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
//    {
//        if (m_wclEdit.IsWindow())
//        {
//            TMCharT buf[MAX_PATH] = {0};
//            int len = m_wclEdit.GetWindowTextLength();
//            m_wclEdit.GetWindowText(buf,len-1);
//            TM::CString strEdit = buf;
//            strEdit.Trim();
//            TMBool bEnable = TMTrue;
//            if (strEdit.IsEmpty())
//            {    
//                bEnable = TMFalse;
//            }
//            else
//            {
//                m_IEWnd.Navigate(strEdit);    
//            }
//        }
//        
//        return 0;
//    }
    
    // Web browser event handlers
    void DLControl (long *pFalg);
    
    void BeforeNavigate2 (
        IDispatch *pDisp, VARIANT *URL, VARIANT *Flags,
        VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers,
        VARIANT_BOOL *Cancel);
    
    void ClientToHostWindow (long *CX, long *CY); 

    void CommandStateChange (long Command, VARIANT_BOOL Enable);

    void DocumentComplete (IDispatch *pDisp, VARIANT *URL);

    void DownloadBegin (VOID);

    void DownloadComplete (VOID);

    void FileDownload (VARIANT_BOOL *Cancel);

    void NavigateComplete2 (IDispatch *pDisp, VARIANT *URL);

    void NavigateError (
        IDispatch *pDisp, VARIANT *URL, VARIANT *TargetFrameName,
        VARIANT *StatusCode, VARIANT_BOOL *Cancel);
    
    void NewWindow2 (IDispatch **ppDisp, VARIANT_BOOL *Cancel);

    void OnFullScreen (VARIANT_BOOL FullScreen);

    void OnMenuBar (VARIANT_BOOL MenuBar);

    void OnQuit(VOID);

    void OnStatusBar (VARIANT_BOOL StatusBar);

    void OnTheaterMode (VARIANT_BOOL TheaterMode);

    void OnToolBar (VARIANT_BOOL ToolBar);

    void OnVisible (VARIANT_BOOL Visible);

    void ProgressChange (long Progress, long ProgressMax);

    void PropertyChange (BSTR szProperty);

    void StatusTextChange (BSTR Text);

    void TitleChange (BSTR Text);

    void WindowClosing (VARIANT_BOOL IsChildWindow, VARIANT_BOOL *Cancel);

    void WindowSetHeight (long Height);

    void WindowSetLeft (long Left);

    void WindowSetResizable (VARIANT_BOOL Resizable);

    void WindowSetTop (long Top);

    void WindowSetWidth (long Width);

    void ShowStatic();

    void HidStatic();

    void KillTimer();

    void AppendText(TMCStr lpstrText, TMBool bNoScroll = TMFalse, TMBool bCanUndo = TMFalse)
    {
        int n = 0;
        if (m_listBox)
            m_listBox.GetWindowTextLength();
        if (m_listBox)
            m_listBox.SetSel(n, n, bNoScroll);
        if (m_listBox)
            m_listBox.ReplaceSel(lpstrText, bCanUndo);
//             m_listBox.InsertText(n, lpstrText, bNoScroll, bCanUndo);
    }

    void Log2(TMCStr str)
    {
        AppendText(str);
        AppendText(L"\r\n");
    }

    void Log(TMCStr fmt, ...)
    {
        TM::CString str;
        va_list va;
        va_start(va, fmt);
        str.FormatV(fmt, va);
        va_end(va);
        Log2(str);
    }
};    





#endif // MAIN_H