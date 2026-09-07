/**
 *    @file
 *    @brief
 *    CWndPad
 *
 *
 *    @author 
 *    @date    2014-11-13
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2014 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "WndPad.h"
#include "FactoryData.h"
#include "ResourceID.h"
#include "FileOpt.h"
#include "ResFile.h"
#include "resource.h"
#include "tmformatacc.h"
#include "../frame/DlgSpyFocus.h"

using namespace TM;

CPanelApp*   s_pView;



// static CPanelApp s_view;

CPanelApp* GetPanelApp()
{
    return s_pView;
}

TM::CView* AppGetDoc()
{
    return GetPanelApp();
}

TM_BGN_NAMESPACE

// for editor
class CPadMgr : public TM::CTuiWnd
{
public:
    CPadMgr() { }
    
    // CTuiWgt
    virtual CFrame* GetFrame() tm_final { return CTuiLayerWnd::GetFrame(); }

    // CTuiLayerImpl
    void CreateBkBuffer(IGXApp* gapp, const TM::CSize& sz) tm_final {}
    
    // MgrXXX
    virtual TMHWnd MgrGetHWND() const tm_final { return (static_cast<CPanelApp*>(GetView()))->m_hRealWnd; }
    virtual TMBool NativeInvalidateRect(SPY_INVALIDATE_PARAM(const TM::TUIRect& rc, TM::HTUI hSender)) tm_final;
    virtual TMHWnd GetOverlayHWND() tm_final { return MgrGetHWND(); }
    //     virtual TMBool ProcessWindowMessage(TM::HTUI hWnd, TMUInt uMsg, TMWParam wParam,
    //         TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID);
};

TMBool CPadMgr::NativeInvalidateRect(SPY_INVALIDATE_PARAM(const TUIRect& rc, TM::HTUI hSender))
{
    (static_cast<CPanelApp*>(GetView()))->GetProjMgr()->Invalidate();
    return true;
}

static CFactoryImpl<CPadMgr>        s_fctMgrPad(TM_CS(TUI_CLASS_WINDOW), false, true);

TM_END_NAMESPACE


CPanelApp::CPanelApp(PANELINFO& info)
    : m_hRealWnd (info.hWnd)
    , m_hResWin (info.hResWin)
    , m_pEditor (info.pMainWnd)
    , m_strXmlFileName(TM_CS(TUI_RES_XML_FILE))
    , m_pmCurrent(0)
{
    s_pView = this;
    CreateEditorModule();
    m_idMgr.Reset();
    m_fileRC.LoadDftResFile(m_idMgr, info.hResWin);
    m_fileQRC.LoadDftResFile(m_idMgr);
}

IObjEdit* CPanelApp::GetDataByObjID(TMUInt32 id)
{
    CModule::CObjList::recorder rec(GetEditorModule()->_FindObj(id, 0));
    if (!rec)
        return 0;

    CTuiObject* p = static_cast<CTuiObject*>(rec.get_data());
    ++rec;
    DbgAssert(!rec || rec.get_data()->m_dwObjID != id); // make sure the object only create 1 instance.
    return p->m_pEditData;
}

TM::CString CPanelApp::FormatAcc(IAccessible* pacc, CComVariant var)
{
    // window
    TM::CString str(DbgFormatAcc(pacc, &var));
    
    // selection
    HRESULT hr = pacc->get_accSelection(&var);
    if (SUCCEEDED(hr)) //0 != var.lVal)
    {
        {
            TM::CStdStr strTmp;
            strTmp.Format(L"\r\ncurrent selection:%d\r\n", var.lVal);
            str += strTmp;
        }
        str += DbgFormatAcc(pacc, &var);
        
        // bind window
        CComPtr<IDispatch> ptrDispBind;
        pacc->get_accChild(var, &ptrDispBind);
        if (ptrDispBind)
        {
            CComQIPtr<IAccessible, &IID_IAccessible> ptrBind(ptrDispBind);
            str += L"\r\nbind window:\r\n";
            CComVariant varb(0);
            str += DbgFormatAcc(ptrBind, &varb);
        }
    }
    
    const GUID CLSID_CUIAutomation = {0xff48dba4, 0x60ef, 0x4201, 0xaa, 0x87, 0x54, 0x10, 0x3e, 0xef, 0x59, 0x4e};
    
    //         class DECLSPEC_UUID("ff48dba4-60ef-4201-aa87-54103eef594e")
    
    // uiautomation
    //     CComPtr<IUIAutomation> ptrUIA;
    //     ptrUIA.CoCreateInstance(CLSID_CUIAutomation, 0, CLSCTX_INPROC_SERVER);
    //     DbgAssert(ptrUIA);
    // 
    //     CComPtr<IUIAutomationElement> ptrEle;
    //     ptrUIA->ElementFromIAccessible(ptr, &ptrEle);
    // 
    //     UiaReturnRawElementProvider
    //     IRawElementProviderSimple
    
    return std::move(str);
}

HRESULT CPanelApp::AccFromPoint(IAccessible* acc, POINT pt, bool bClientPT, IAccessible** ppAcc, VARIANT* pvarChild)
{
    HRESULT hr = S_OK;
    if (bClientPT)
    {
        HWND hWnd = 0;
        WindowFromAccessibleObject(acc, &hWnd);
        if (!hWnd)
            return E_FAIL;
        ::ClientToScreen(hWnd, &pt);
    }
    CComPtr<IAccessible> pAcc(acc);
    while (SUCCEEDED(hr))
    {
        VARIANT varChild;
        VariantInit(&varChild);
        hr = pAcc->accHitTest(pt.x, pt.y, &varChild);
        if (!SUCCEEDED(hr))
        {
            return(hr);
        }
        
        if (varChild.vt == VT_DISPATCH)
        {
            if (!varChild.pdispVal)
                return(E_POINTER);
            
            pAcc = NULL;
            hr = varChild.pdispVal->QueryInterface(IID_IAccessible, (void**)&pAcc);
            varChild.pdispVal->Release();
        }
        else if ((varChild.vt == VT_I4) || (varChild.vt == VT_EMPTY))
        {
            *ppAcc = pAcc.Detach();
            VariantCopy(pvarChild, &varChild);
            return(S_OK);
        }
        else
        {
            VariantClear(&varChild);
            hr = E_INVALIDARG;
        }
    }
    return(hr);
}

HRESULT CPanelApp::AccFromPoint(HWND hwndPoint, POINT pt, bool bClientPT, IAccessible** ppAcc, VARIANT* pvarChild)
{
    if (IsBadWritePtr(ppAcc,sizeof(void*)) || IsBadWritePtr(pvarChild,sizeof(VARIANT)))
        return (E_INVALIDARG);
    
    if (bClientPT)
        ClientToScreen(hwndPoint, &pt);
    
    *ppAcc = NULL;
    pvarChild->vt = VT_EMPTY;
    
    hwndPoint = GetAncestor(hwndPoint, GA_ROOT);
    if (!hwndPoint)
        return(E_FAIL);
    
    IAccessible* pAcc = 0;
    HRESULT hr = AccessibleObjectFromWindow(hwndPoint, OBJID_WINDOW, IID_IAccessible, (void **)&pAcc);
    if (pAcc)
    {
        hr = AccFromPoint(pAcc, pt, false, ppAcc, pvarChild);
        pAcc->Release();
    }
    return hr;
}

void CPanelApp::CreateEditorModule()
{
    DbgAssert(!GetEditorModule());

    CModule* mdl = CreateModule(HEDITOR);
    
    // replace mgr impl
    CModule::CFactoryList::recorder rec(mdl->m_aFactory);
    rec.find(TM_CS(TUI_CLASS_WINDOW));
    DbgAssert(rec);
    rec.get_data().m_pFactory = &TM::s_fctMgrPad;

    DbgAssert(GetEditorModule());
}

void CPanelApp::InternalSetCapture(const CTuiWgt* hWgt)
{
    //GetAPP()->SetCapture(m_hWnd);
}

void CPanelApp::InternalReleaseCapture()
{
    //GetAPP()->SetCapture(0);
}

TMBool CPanelApp::InternalIsCapture(const CTuiWgt* hWgt)
{
    return false; //GetAPP()->GetCapture() == m_hWnd;
}

void CPanelApp::InternalSetFocus(const CTuiWgt* hWgt)
{
//     if (hWgt)
//         GetAPP()->SetFocus(m_hWnd);
//     else
//         GetAPP()->KillFocus();
}

TMBool CPanelApp::InternalIsWindowActive(const CTuiWgt* hWgt)
{
    return true;
//    return (static_cast<CView*>(GetAPP()))->InternalIsWindowActive(m_hWnd); 
}

CPoint CPanelApp::InternalGetScreenOrg(HTUI hWgt)
{
    TM::CTuiPoint pt; int dpi;
    GetProjMgr()->GetPadInfo(pt, dpi);
    CPoint ptd;
    CTuiZoom(dpi).LPtoDP(pt, ptd);
    ::ClientToScreen(m_hRealWnd, &ptd);
    return ptd;

//     CPoint pt = (static_cast<CView*>(GetAPP()))->InternalGetScreenOrg(m_hWnd);
//     pt.x += m_hWnd->GetLeft();
//     pt.y += m_hWnd->GetTop();
//     return pt; 
}




///////////////////////////////////////////////////////////////////////////
// CPanelApp

TM_BGN_NAMESPACE

TMBool s_bPrinting;


TM_END_NAMESPACE

static EGXDevice s_devDisp[] = {         eGXGraDevGDI, eGXGraDevD3D9, eGXGraDevD3D11, eGXGraDevGL };
static LPCTSTR   s_strDisp[] = { L"DFT", L"GDI",       L"DX9",        L"D11",         L"GL",      0 };
static EGXDevice s_devText[] = {         eGXTxtDevGDI, eGXTxtDevDWrite };
static LPCTSTR   s_strText[] = { L"DFT", L"SYS",       L"DW",         0 };


void CPanelApp::Release()
{
//     DbgAssert(!m_pCurObj);
//     DbgAssert(!m_wndFrame.m_hWnd->m_pFrame.p);
//     m_wndFrame.DestroyWindow();
//     m_wndPic.DestroyWindow();
    Term();
    delete this;
}

TMCStr* CPanelApp::GetObjRootNames()
{
    static TMCStr c[] =  { L"Dialog", L"Frame", L"Pic", 0 };
    return c;
}

static TM::CString CheckPopupMenu(IProj* pProj, TMPoint pt, HWND hWnd, TM::EObjType eType, CMenuHandle muMain)
{
    CMenuHandle muPop = muMain.GetSubMenu(0);
    int n = 0;
    muPop.CreatePopupMenu();
    TMCStr strClass;
    for (n = 0; strClass = pProj->GetClassByIndex(eType, n); ++n)
    {
        int nNewMenu = 30000;
        nNewMenu += n;
        muPop.AppendMenu (MF_STRING,nNewMenu,TM_W2T(strClass));
    }
    
    n =  muPop.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN, pt.x, pt.y, hWnd);
    if (!n)
        return TM::CString();
    TMCharT strTemp[CHAR_MAX + 1] = {0};
    int nTmp = muPop.GetMenuString(n,strTemp,CHAR_MAX,MF_BYCOMMAND);
    return strTemp;
}

static int TrackDlgPopupMenu(CPanelApp& app, TMPoint pt, TMHWnd hWnd, TM::EObjType eObjType, CMenuHandle muMain)
{
    TM::CString str(CheckPopupMenu(&app, pt, hWnd, eObjType, muMain));
    if (!str.IsEmpty())
    {
        app.GetProjMgr()->ItemCreate(0, str);
        //        CreateNewObject((IObjEdit*)eObjType, str);
        return true;
    }
    return false;
}


TMResult CPanelApp::TrackMenuPop(HWND hWnd, TMPoint pt, HMENU hMenuRoot, IObjEdit* pSel)
{
    if (pSel < objEditMax)
        return TrackDlgPopupMenu(*this, pt, hWnd, (TM::EObjType)((TMLParam)pSel-1), hMenuRoot);

    CMenuHandle muMain = hMenuRoot;
    CMenuHandle muPop = muMain.GetSubMenu(1);
    TM::EObjType eType = pSel->GetType();
    if (eType != TM::e_otWgt) // EM_WGT)
    {
        muPop.DeleteMenu(IDM_MOVE_UP, MF_BYCOMMAND);
        muPop.DeleteMenu(IDM_MOVE_DOWN, MF_BYCOMMAND);
    }
    
    muPop.DeleteMenu(IDM_P_EMBED_WGT, MF_BYCOMMAND);
    muPop.DeleteMenu(IDM_WGT_EMBED_P, MF_BYCOMMAND);
    muPop.DeleteMenu(IDM_ITEM_DETACH, MF_BYCOMMAND);
    muPop.DeleteMenu(IDM_ITEM_DETACH_ALL, MF_BYCOMMAND);
    muPop.DeleteMenu(IDM_DEL_HWND, MF_BYCOMMAND);
//     muPop.DeleteMenu(IDM_SPY_MSG_OBJ, MF_BYCOMMAND);
    
    
//     if (eType != TM::e_otWnd) // EM_DIALOG)
//         muPop.DeleteMenu(IDM_TOOL_TEST, MF_BYCOMMAND);
//     
//     if (eType != TM::e_otWnd && eType != TM::e_otWgt)// EM_DIALOG && eMode != EM_WGT)
//     {
//         muPop.DeleteMenu(IDM_TOOL_ADDFILE, MF_BYCOMMAND);
//         muPop.DeleteMenu(IDM_TOOL_DELFILE, MF_BYCOMMAND);
//     }
    
    muPop.TrackPopupMenu(/*TPM_RETURNCMD |*/ TPM_LEFTALIGN, pt.x, pt.y, hWnd);
    return 0;
} 


void CPanelApp::ModifyUser(LPCTSTR pNameOld, const TM::CStrView& pNameNew)
{
    m_idMgr.SetUser(pNameNew);
}

TMCStr CPanelApp::GetClassByIndex (TM::EObjType eType, int n)
{
    DbgAssert(eType >= 0 && eType < e_otMax);
    
    for (CModule::CFactoryList::recorder rec(GetEditorModule()->m_aFactory); rec; ++rec)
    {
        const CFactoryInfo::_Class* p = rec.get_data().m_pFactory;
        if (p->eType != eType)
            continue;
        if (--n < 0)
            return p->strClass;
    }
    return 0;
}

void CPanelApp::GetDeviceNames(TMCStr** ppDisplay, TMCStr** ppText)
{
    *ppDisplay = s_strDisp;
//    *pnDisplay = nID;
    *ppText    = s_strText;
}

TMBool CPanelApp::SaveFile(LPCTSTR pFileName, ESaveMode eMode, IDomDocument** xmlOut)
{
    switch (eMode & eSaveModeMask)
    {
    case eSaveModeNormal:
    case eSaveModeSel:
        {
            CAutoSaveAnalysis _ama("GetXmlFileName");
            
            // Get xml file name
            TM::CFileName fx(pFileName);
            TM::CString strb(fx.GetPath(true));

            // save xml
            TM::CString str;
            CIDUsedList aID;

            CAutoSaveAnalysis _ama2("SaveXml");
            SaveXml(strb, str, &aID, eMode);

            CAutoSaveAnalysis _ama4("SaveResFile");
            TMBool bQRC = (0 == StrCompareI(fx.GetExt(false), TM_T("qrc")));
            if (bQRC)
            {
                m_strXmlFileName = TM_CS(TUI_RES_XML_FILE);
                m_fileQRC.SaveResFile(pFileName, m_strXmlFileName);
            }
            else
                m_fileRC.SaveResFile(pFileName, m_strXmlFileName);

            CAutoSaveAnalysis _ama3("DomSave");
            TM::CFileName fnXml(strb + m_strXmlFileName);
            DomSave(fnXml, str);

            CAutoSaveAnalysis _ama5("SaveHFile");
            {
                TM::CFileName fnResH(fnXml.GetPath(true));
                fnResH += _T("resource.h");
                TM::CFile f;
                f.CreateNew(fnResH, f.e_faWrite);
            }
            
            TM::CFileName fnH(fnXml.GetPathBody(false));
            fnH += _T(".h");
            m_idMgr.SaveHFile (fnH, aID);

            if (eMode == eSaveModeNormal)
                ::SetCurrentDirectory(strb);

        }
        return true;
    
    case eSaveModeTest:
        {
            TM::CStringW xml;
            CIDUsedList aID;
            SaveXml(xml, xml, &aID, eSaveModeSel);
            //TM::CDomDocument::SaveXmlFile(strPath, xml);
            DomSave(pFileName, xml);
        }
        return true;

    case eSaveModeSkin:
    case eSaveModeLang:
        SaveSkin(pFileName, eMode, xmlOut);
        return true;
    }

    return false;
}

// void CPanelApp::SetCurrentObject(IObjEdit* pObj)
// {
//     if (pObj)
//         SwitchSelMgr(pObj->GetAncestorItem());
//     else
//         SwitchSelMgr(0);
// }

TM::CString CPanelApp::SetObjectIDMacro(const TM::CStrView& pOld, const TM::CString& strNew)
{
    return m_idMgr.SetObjectIDMacro(pOld, strNew);
}

// void CPanelApp::ID2Text(TMUInt32 dwID, TM::CString& str, TM::CString* pStrUser)
// {
//     m_idMgr.ID2Text(dwID, str, pStrUser);
// }

void CPanelApp::GetModuleList(CModuleListH* aListH, CModuleListS* aListS)
{
    CView& view = AppGetTester()->GetView();
    for (CView::CModuleList::recorder rec(view.m_aModules); rec; ++rec)
    {
        CModule* pm = rec.get_data();
        if (aListH)
            aListH->push_back(pm->m_hRes);
        if (aListS)
        {
            TCHAR c[1024];
            GetModuleFileName(pm->m_hRes, c, 1024);
            aListS->push_back(c);
        }
    }
}

void CPanelApp::AppendSysInfo(TM::CString& str, ESysInfo eType)
{
    if (eType == eSysInfoRender)
    {
        TCHAR cGX[1024];
        GXADAPTERDESC desc = { L"GDI" };
        _gxApp->GetDeviceType(&desc);
        LPCTSTR pGraDev[] = { L"Auto", L"GDI",  L"D3D9",  L"D3D11", L"GL" };
        LPCTSTR pTxtDev[] = { L"Auto", L"GDI",  L"DWrite" };
        EGXDevice eDev = _gxApp->GetDeviceType();
        StrPrintf(cGX, tm_countof(cGX),
            L"vendorID:%x - deviceID:%x - SubSysId:%x - Revision:%x - videomemory:%d\r\n"
            L"                         %s - driverversion:%d.%d.%d.%d\r\n"
            L"graphic device         : %s\r\n"
            L"text render device     : %s",
            desc.VendorId, desc.DeviceId, desc.SubSysId, desc.Revision, desc.DedicatedVideoMemory, desc.Description,
            (int)(0xffff & (desc.DriverVersion >> 48)),
            (int)(0xffff & (desc.DriverVersion >> 32)),
            (int)(0xffff & (desc.DriverVersion >> 16)),
            (int)(0xffff & (desc.DriverVersion >> 0)),
            pGraDev[eDev & eGXGraDevMask],
            pTxtDev[(eDev & eGXTxtDevMask) >> 4]
            );
        str.AppendFormat(L"Current Render Adapter : %s\r\n", &cGX);
    }

};

void CPanelApp::TesterRTL(TMBool bKeep)
{
    static TMBool b = false;
    if (!bKeep)
        b = !b;
    LPCTSTR p[2] =
    {
        _CS("<") TUI_XML_NODE0_TMDUI _CS(" ") TUI_XML_PROP_LCID _CS("=\"1033\" />"),
        _CS("<") TUI_XML_NODE0_TMDUI _CS(" ") TUI_XML_PROP_LCID _CS("=\"1025\" />"),
    };
    TesterLanguage(p[b]);
}

void CPanelApp::TesterLanguage(LPCTSTR p)
{
    IProj::CModuleListH aModule;
    this->GetModuleList(&aModule, 0);
    for (IProj::CModuleListH::recorder rec(aModule); rec; ++rec)
        AppGetTester()->LoadXml(rec.get_data(), 0, p);
}

void CPanelApp::TesterChangeDevice(int bDisplay, int id)
{
    if (id == 0)
        return;
    if (bDisplay)
        gxSetDefaultGraDevices(true, s_devDisp[id - 1]);
    else
        gxSetDefaultTxtDevices(true, s_devText[id - 1]);
}


IProj* CreateProj(PANELINFO& info)
{
    CPanelApp* view = new CPanelApp(info);

//     _Module.m_pIMainWnd = pMainWnd;
//     view->m_hRealWnd = info.hWnd;
//     view->m_pEditor = info.pMainWnd;

    return view;
}