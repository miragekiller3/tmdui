// TestDll.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f TestDllps.mk in the project directory.

#include "stdafx.h"
#include "panel.h"
#include "dui/uiwindow_win.h"


TM_BGN_NAMESPACE

struct CTuiAppDebug : public CTuiAppTester
{
    CView& _app;
    CTuiAppDebug(CView& app) : _app(app) {}
    
    virtual TM::CView& GetView()
    {
        return _app;
    }

    virtual void Term() tm_override
    {
        LOG_FUNC_FOR_SPY(0, L"_tuiApp.Term()");
        _app.Term();
    }

    virtual TMBool LoadXml(TMHModule hRes, TMCStr pBase, TMCStr pLang = 0) tm_override
    {
        LOG_FUNC_FOR_SPY(0, L"_tuiApp.LoadXml(hRes:0x%X, pBase:%s, pLang:%s)", hRes, pBase, pLang);
        return _app.LoadXml(hRes, pBase, pLang);
    }

    virtual void UnLoadXml(TMHModule hRes) tm_override
    {
        LOG_FUNC_FOR_SPY(0, L"_tuiApp.UnLoadXml(hRes:0x%X)", hRes);
        _app.UnLoadXml(hRes);
    }

    virtual void SetColorTransform(TMHModule hRes, IColorTransform* pct) tm_override
    {
        LOG_FUNC_FOR_SPY(0, L"_tuiApp.SetColorTransform(hRes:0x%X, pct:0x%X)", hRes, pct);
        _app.SetColorTransform(hRes, pct);
    }

    virtual TMBool SetProperty(EAppProperty idProp, const void* pVol, int nVolSize) tm_override
    {
        LOG_FUNC_FOR_SPY(0, L"_tuiApp.SetProperty(idProp:0x%X)", idProp);
        return _app.SetProperty(idProp, pVol, nVolSize);
    }

    virtual TMBool GetProperty(EAppProperty idProp, void* pVol, int nVolSize) tm_override
    {
        LOG_FUNC_FOR_SPY(0, L"_tuiApp.GetProperty(idProp:0x%X)", idProp);
        return _app.GetProperty(idProp, pVol, nVolSize);
    }

    virtual CStrPtr LoadString(TMHModule hRes, int nID, const CStrView* strXmlProperty) tm_override
    {
        LOG_FUNC_FOR_SPY(0, L"_tuiApp._LoadString(hRes:0x%X, nID:%d, strXmlProperty:%s)", hRes, nID, strXmlProperty ? TM_T2W(*strXmlProperty) : L"");
        return _app.LoadString(hRes, nID, strXmlProperty);
    }

    virtual TMHMenu LoadMenu(TMHModule hRes, TMCStr pMenu) tm_override
    {
        LOG_FUNC_FOR_SPY(0, L"_tuiApp.LoadMenu(hRes:0x%X, pMenu:%d)", hRes, pMenu);
        return _app.LoadMenu(hRes, pMenu);
    }

    virtual TMResult GetResourceStream(TMHModule hRes, TMCStr lpszName, TMCStr uType, struct IStream** ppStream) tm_override
    {
        LOG_FUNC_FOR_SPY(0, L"_tuiApp.GetResourceStream(hRes:0x%X, lpszName:%s, uType:%s)", hRes, lpszName, uType);
        return _app.GetResourceStream(hRes, lpszName, uType, ppStream);
    }

    virtual CComPtr<ITuiPic> GetPic(TMHModule hRes, int nIDObj, int dpi = TUI_DPI_SYSTEM_DEFAULT) tm_override
    {
        LOG_FUNC_FOR_SPY(0, L"_tuiApp.GetPic(hRes:0x%X, nIDObj:%d, dpi:%d)", hRes, nIDObj, dpi);
        return _app.GetPic(hRes, nIDObj, dpi);
    }

    virtual CComPtr<ITuiPic> CreateCustomPic(TMHModule hRes, int nIDObj, IStream* stream) tm_override
    {
        LOG_FUNC_FOR_SPY(0, L"_tuiApp.CreateCustomPic(hRes:0x%X, nIDObj:%d, stream:0x%X)", hRes, nIDObj, stream);
        return _app.CreateCustomPic(hRes, nIDObj, stream);
    }

    virtual void RegisterCallbackFactory(TMHModule hRes, int nObjectID, const TUIFACTORY& factory) tm_override
    {
        LOG_FUNC_FOR_SPY(0, L"_tuiApp.RegisterCallbackFactory(hRes:0x%X, nIDObj:%d)", hRes, nObjectID);
        _app.RegisterCallbackFactory(hRes, nObjectID, factory);
    }

    virtual TMBool AddMessageFilter(CTuiMsgFilter* pMessageFilter) tm_override
    {
        LOG_FUNC_FOR_SPY(0, L"_tuiApp.AddMessageFilter(pMessageFilter:0x%p)", pMessageFilter);
        return _app.AddMessageFilter(pMessageFilter);
    }

    virtual TMBool RemoveMessageFilter(CTuiMsgFilter* pMessageFilter) tm_override
    {
        LOG_FUNC_FOR_SPY(0, L"_tuiApp.RemoveMessageFilter(pMessageFilter:0x%p)", pMessageFilter);
        return _app.RemoveMessageFilter(pMessageFilter);
    }

    virtual void GetMouseHotState(TUIHITINFO& ht) tm_override
    {
        LOG_FUNC_FOR_SPY(0, L"_tuiApp.GetMouseHotState()");
        _app.GetMouseHotState(ht);
    }

    virtual TMBool GetMouseLDownState(TUIHITINFO& ht) tm_override
    {
        LOG_FUNC_FOR_SPY(0, L"_tuiApp.GetMouseLDownState()");
        return _app.GetMouseLDownState(ht);
    }

    virtual TMBool GetMouseRDownState(TUIHITINFO& ht) tm_override
    {
        LOG_FUNC_FOR_SPY(0, L"_tuiApp.GetMouseRDownState()");
        return _app.GetMouseRDownState(ht);
    }

    virtual TMBool GetMouseMDownState(TUIHITINFO& ht) tm_override
    {
        LOG_FUNC_FOR_SPY(0, L"_tuiApp.GetMouseMDownState()");
        return _app.GetMouseMDownState(ht);
    }

    virtual TMBool GetGestureState(TUIHITINFO& ht, int nID) tm_override
    {
        LOG_FUNC_FOR_SPY(0, L"_tuiApp.GetGestureState(nID:%d)", nID);
        return _app.GetGestureState(ht, nID);
    }

    virtual CKeyInput GetKeyInput() tm_override
    {
        LOG_FUNC_FOR_SPY(0, L"_tuiApp.GetKeyInput()");
        return _app.GetKeyInput();
    }

    virtual TMBool SetCapture(HTUI hWnd) tm_override
    {
        LOG_FUNC_FOR_SPY(hWnd, L"_tuiApp.SetCapture()");
        return _app.SetCapture(hWnd);
    }

    virtual HTUI GetCapture() tm_override { return _app.GetCapture(); }
    virtual TMBool SetFocus(HTUI hWnd, EFocusReason reason = eFocusReasonUnknown) tm_override { return _app.SetFocus(hWnd, reason); }
    virtual HTUI GetFocus() tm_override { return _app.GetFocus(); }
    virtual void KillFocus() tm_override { _app.KillFocus(); }
    virtual void SetFocusChain(HTUI hFirst, HTUI hSecond) { _app.SetFocusChain(hFirst, hSecond); }

    virtual int GetMenuPopLevel(HTUI hWnd, TMBool bOwner) tm_override { return _app.GetMenuPopLevel(hWnd, bOwner); }
    virtual TMBool EndMenuPopLevel(int nLevel, int nResult, TMBool bRestoreFocus = true, TUIMSG* p = 0) tm_override { return _app.EndMenuPopLevel(nLevel, nResult, bRestoreFocus, p); }
    virtual TMBool GetMenuPopInfo(int nLevel, HTUI* pOwner, HTUI* pPop) tm_override { return _app.GetMenuPopInfo(nLevel, pOwner, pPop); }
    virtual HTUI CreateDlgEx(TMHModule hInst, int nID, TMHWnd hWndParent, TUIFACTORY* pFactory, void* param = 0) tm_override { return _app.CreateDlgEx(hInst, nID, hWndParent, pFactory, param); }
    virtual HTUI CreateDlgByHWND(TMHModule hInst, int nID, TMHWnd hWnd, TUIFACTORY* pFactory, void* param = 0) tm_override { return _app.CreateDlgByHWND(hInst, nID, hWnd, pFactory, param); }
    virtual void EnumDialogs(PFNWWENUMPROC lpEnumFunc, TMLParam lParam) tm_override { _app.EnumDialogs(lpEnumFunc, lParam); }
};

struct _EditorApp : public TM::CViewWin {};
struct _TesterApp : public TM::CViewWin {};

class CTuiAppEditor
    : public _EditorApp
    , public _TesterApp
{
    virtual void Term() tm_override
    {
        _EditorApp::Term();
        _TesterApp::Term();
    }

public:
    ~CTuiAppEditor()
    {
        Term();
    }
};

static CTuiAppEditor _app;
static CTuiAppDebug _appDbg(static_cast<_TesterApp&>(_app));

#ifdef TUI_EDIT_MODE

TM_BGN_PRIVATE_NAMESPACE

CTuiApp* tuiGetAPP(HTUI hWnd)
{
    if (hWnd)
    {
        ASSERT_CLASS(hWnd);
        CView* p = hWnd->GetView();
        if (p == &AppGetTester()->GetView())
            return AppGetTester();
        return p;
    }
    else
        return AppGetTester();
}

TM_END_NAMESPACE // TM_BGN_PRIVATE_NAMESPACE

#endif // TUI_EDIT_MODE

TM_END_NAMESPACE // TM_BGN_NAMESPACE

TM::CTuiApp* AppGetEditor()
{
    TM::_EditorApp* p = &TM::_app;
    return p;
}

CTuiAppTester* AppGetTester()
{
    return &TM::_appDbg;
}


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()


/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

//#include "../../atmemcheck.h"

extern "C"
TMBool WINAPI DllMain(TMHModule hInstance, TMULong dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        OleInitialize(0);

        _Module.Init(ObjectMap, hInstance, 0);
        DisableThreadLibraryCalls(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        DbgTrace("DllMain(DLL_PROCESS_DETACH)");
        _Module.Term();
    }
    return TMTrue;    // ok
}


/*
STDAPI DllCanUnloadNow(void)
{
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
}
*/
