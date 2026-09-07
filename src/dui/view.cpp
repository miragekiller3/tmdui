/**
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2013.12.07
 *
 *  Copyright (C) 2013 miragekiller                               
 */

#include "view.h"
#include "helper.h"
#include "menupop.h"
#include "tooltip.h"
#include "uiwindow.h"
#include "platform.h"
#include "../tmdrag.h"



TM_BGN_NAMESPACE

bool        CView::s_bWebview2AllowSingleSignOnUsingOSPrimaryAccount;
CString     CView::s_strWebview2Arguments;
CString     CView::s_strWebview2Language;
CString     CView::s_strWebview2ExecutableFolder;
CString     CView::s_strWebview2UserDataFolder;
// TMClock     CView::s_nDisplayFrequency;


CView::CView()
    : m_pCapture(0)
    , m_dwState(0)
    , m_bSystemFrame(false)
    , m_bShowHintOnEllipsis(false)
    , m_bShowHintOnTabKey(false)
    , m_bHighContrast(false)
    , m_bAnimationButton(false)
    , m_bAnimationScroll(false)
    , m_bAnimationSysBut(false)
    , m_clrDisableText(TMRGB(128,128,128))
    , m_clrEditSelBackground(TMRGB(0x04, 0x9f, 0xd9))
    , m_clrColorEditSelText(TMRGB(255, 255, 255))
    , m_clrColorEditPrompt(TMRGB(192,192,192))
{
    CallInit();
    gxInit();
    TUIHITINFO hi = {0};
    m_htOver = m_htPrev = m_htDownL = m_htDownR = m_htDownM = hi;
    OnSettingChanged();
//     OnDisplayChanged();
}

CView::~CView()
{
    DbgTrace("~CView(0x%X)", this);
    gxSetNotify(this, false);
    DbgAssert(0 == m_aModules.size());
    gxTerm();
    CallTerm();
}

////////////////////////////////////////////////////////////////////////////////
// class CView
void CView::Term()
{
    CModuleList aModules;
    aModules.swap(m_aModules);
    for (CModuleList::recorder rec(aModules); rec; ++rec)
    {
        CModule* p = rec.get_data();
        p->TermAndRelease();
    }
    g_mgrTooltip.Clear();
    CallFlush();
}

void CView::OnSettingChanged()
{
    SetHighContrast(CAppAT::IsHighContrast());
}

// void CView::OnDisplayChanged()
// {
//     DEVMODE dm = {0};
//     dm.dmSize = sizeof(dm);
//     if (EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &dm))
//         s_nDisplayFrequency = (1000 / dm.dmDisplayFrequency);
//     else
//         s_nDisplayFrequency = 33;
// }
// 
// TMClock CView::GetNextVBlankTicket()
// {
//     // D3DKMTWaitForVerticalBlankEvent(&waitData);
// 
// //     if (DWM.isEnabled()) 
// //     { 
// //         present(); 
// //         DwmFlush(); 
// //     } 
// //     else 
// //     {
// //         waitForVBlank(); 
// //         present(); 
// // }
//     return (1 + TimeClock() / s_nDisplayFrequency) * s_nDisplayFrequency;
// }

TMBool CView::IsDragDistance(const TUIPoint& pt1, const TUIPoint& pt2)
{
#ifdef TM_WIN
    return abs(pt1.x - pt2.x) > GetSystemMetrics(SM_CXDOUBLECLK)
        || abs(pt1.y - pt2.y) > GetSystemMetrics(SM_CYDOUBLECLK);
#else
    return abs(pt1.x - pt2.x) > 2 || abs(pt1.y - pt2.y) > 2;
#endif
}


// CM_RENDERCHANGED, WM_DWMCOMPOSITIONCHANGED
void CView::DispatchModule(TMUInt msg, TMWParam wp, TMLParam lp)
{
    for (CView::CModuleList::recorder rec(m_aModules); rec; ++rec)
    {
        CModule* p = rec.get_data();
        DbgAssert(p);
        p->DispatchObject(msg, wp, lp);
    }
}

void CView::EnumDialogs (PFNWWENUMPROC lpEnumFunc, TMLParam lParam)
{
    for (CModuleList::recorder rec(m_aModules); rec; ++rec)
    {
        CModule* pm = rec.get_data();
        for (CModule::CObjList::recorder reco(pm->m_aObjs); reco; ++reco)
        {
            CTuiObject* po = static_cast<CTuiObject*>(reco.get_data());
            if (po->GetType() != e_otWnd)
                continue;
            if (!lpEnumFunc(static_cast<HTUI>(po), lParam))
                return;
        }
    }
}

TMBool CView::LoadXml(TMHModule hRes, TMCStr pBase, TMCStr pLang)
{
    CAutoMsgAnalysis awm("CView::LoadXml");
    gxSetNotify(this, true);
    CModule* md = CreateModule(hRes);
    md->LoadXml(pBase, pLang);
    return true;
}

void CView::UnLoadXml(TMHModule hRes)
{
    CModuleList::iterator it = m_aModules.find (hRes);
    if (it != m_aModules.end())
    {
        CModule* pm = it->second;
        m_aModules.erase(it);
        pm->TermAndRelease();
    }
}

void CView::SetColorTransform(TMHModule hRes, IColorTransform* pct)
{
    CAutoMsgAnalysis awm("CView::SetColorTransform");
    CModule* md = CreateModule(hRes);
    md->SetColorTransform(pct);
}


// void CView::SetColorMatrix(const CColorMatrix& cm)
// {
//     m_clrMatrix = cm;
//     m_bColorMatrix = (
//         cm.m[0][1] || cm.m[0][2] || cm.m[1][0] ||
//         cm.m[1][2] || cm.m[2][0] || cm.m[2][1] ||
//         cm.m[0][0] != 255 || cm.m[1][1] != 255 || cm.m[2][2] != 255);
// }
// 
// void CView::GetColorMatrix(CColorMatrix& cm)
// {
//     cm = m_clrMatrix;
// }

CStrPtr CView::LoadString(TMHModule hRes,  int nID, const CStrView* strXmlProperty)
{
    return GetModule(hRes)->LoadString(nID, strXmlProperty);
}


TMHMenu CView::LoadMenu(TMHModule hRes, TMCStr pMenu)
{
#ifdef TM_WIN_DESKTOP
    TMHMenu hMenu = ::LoadMenu (hRes, pMenu);
    if (!hMenu)
    {
        return hMenu;
    }

    CModule* pm = GetModule (hRes);
    if (pm)
    {
        pm->SetMenuInfo (hMenu, 0);
    }

    return hMenu;
#else
    return 0;
#endif
}


// GXPTexture CView::LoadTexture(TMHModule hRes, TMCStr lpszName, TMCStr uType)
// {
//     if (!lpszName)
//         return 0;
// 
// #if TM_ANDROID
//     CModule* pm = GetModule (hRes);
//     if (pm)
//         return ImageCreateFromResource(hRes, pm->m_strXmlFilePath + lpszName, uType);
//     else
//         return ImageCreateFromResource(hRes, lpszName, uType);
// #else
//     GXPTexture himg =gxTextureCreateFromFile(lpszName);
//     if (himg)
//         return himg;
// 
//     CModule* pm = GetModule (hRes);
//     if (pm && !pm->m_strXmlFilePath.IsEmpty())
//     {
//         himg = gxTextureCreateFromFile((pm->m_strXmlFilePath + lpszName).c_str());
//         if (himg)
//             return himg;
//     }
// 
//     return gxTextureCreateFromResource(hRes, lpszName, TMCStr(uType)); // MAKEINTRESOURCE(uType));
// #endif
// }

TMResult CView::GetResourceStream(TMHModule hRes, TMCStr lpszName, TMCStr uType, struct IStream** ppStream)
{
    if (!lpszName)
        return 0;

#ifdef TUI_EDIT_MODE
    if (!hRes)
    {
        TMResult hr = StreamCreateFromFile(lpszName, TM::CFile::e_faRead, ppStream);
        if (hr == S_OK)
            return hr;

        CModule* pm = GetModule(hRes);
        if (pm && !pm->m_strXmlFilePath.IsEmpty())
        {
            hr = StreamCreateFromFile((pm->m_strXmlFilePath + lpszName).c_str(), TM::CFile::e_faRead, ppStream);
            if (hr == S_OK)
                return hr;
        }
        if (IS_INTRESOURCE(lpszName))
            return StreamCreateFromResource(hRes, lpszName, uType, ppStream);
        
        TM::CFnView fn(lpszName);
        return StreamCreateFromResource(hRes, fn.GetName().data(), uType, ppStream);
    }
#endif

    return StreamCreateFromResource(hRes, lpszName, uType, ppStream);
}

CComPtr<ITuiPic> CView::GetPic (TMHModule hRes, int nID, int dpi)
{
    CModule* pm = GetModule (hRes);
    if (pm)
    {
        CPicPtr ptr;
        ptr.Create(*pm, nID, dpi);
        return (ITuiPic*)ptr;
    }
    return 0;
}

CComPtr<ITuiPic> CView::CreateCustomPic(TMHModule hRes, int nIDObj, IStream* stream)
{
    CModule* pm = GetModule (hRes);
    if (pm)
    {
        CPic* ppic = CPicPtr::CreateNative(pm, 0, nIDObj, TUI_DPI_SYSTEM_DEFAULT, true);
        if (ppic)
        {
            ppic->SetStreamSource(stream);
            CComPtr<ITuiPic> pic;
            pic.Attach(ppic);
            return ppic;
        }
    }
    return 0;
}

void CView::RegisterCallbackFactory(TMHModule hRes, int nObjectID, const TUIFACTORY& factory)
{
    CModule* md = CreateModule(hRes);
    md->m_aCallback.insert2(nObjectID, factory);
}

struct _CViewProp
{
    inline static TMBool Set(bool& vol, const void* pVol, int nVolSize)
    {
        return (nVolSize == sizeof(TMBool)) ? (vol = 0 != *(TMBool*)pVol, true) : false;
    }

    inline static TMBool Set(TM::CString& vol, const void* pVol, int nVolSize)
    {
        return (nVolSize == sizeof(TMCStr)) ? (vol = *(TMCStr*)pVol, true) : false;
    }

    template <class T> inline static TMBool SetT(T& vol, const void* pVol, int nVolSize)
    {
        DbgAssertS(sizeof(T) != sizeof(bool));
        return (nVolSize == sizeof(T)) ? (vol = *(T*)pVol, true) : false;
    }

    inline static TMBool Get(bool vol, void* pVol, int nVolSize)
    {
        return (nVolSize == sizeof(TMBool)) ? (*(TMBool*)pVol = vol, true) : false;
    }

    template <class T> inline static TMBool GetT(const T& vol, void* pVol, int nVolSize)
    {
        return (nVolSize == sizeof(T)) ? (*(T*)pVol = vol, true) : false;
    }

    static TMBool SetHighContrast(CView* view, const void* pVol, int nVolSize)
    {
        if (nVolSize != sizeof(TMBool))
            return false;
        bool bx = (0 != *(TMBool*)pVol);
        if (view->m_bHighContrast == bx)
            return true;
        view->m_bHighContrast = bx;
        view->DispatchModule(CM_COLORVISUAL, eColorVisualHighContrast, 0);
//         for (CView::CModuleList::recorder rec(view->m_aModules); rec; ++rec)
//         {
//             CModule* p = rec.get_data();
//             DbgAssert(p);
//             p->DispatchUI(eColorVisualHighContrast);
//         }
        return true;
    }

    static TMBool SetUseSystemFrame(CView* view, const void* pVol, int nVolSize)
    {
        if (nVolSize != sizeof(TMBool))
            return false;
        bool bx = (0 != *(TMBool*)pVol);
        if (view->m_bSystemFrame == bx)
            return true;
        view->m_bSystemFrame = bx;
        view->DispatchModule(WM_DWMCOMPOSITIONCHANGED, 0, 0);
        return true;
    }
};

TMBool CView::SetProperty(EAppProperty idProp, const void* pVol, int nVolSize)
{
    if (!pVol || !nVolSize)
        return false;
    switch (idProp)
    {
    case eAppPropAnimationButton:       return _CViewProp::Set(m_bAnimationButton, pVol, nVolSize);
    case eAppPropAnimationScroll:       return _CViewProp::Set(m_bAnimationScroll, pVol, nVolSize);
    case eAppPropAnimationSysBut:       return _CViewProp::Set(m_bAnimationSysBut, pVol, nVolSize);
    case eAppPropHighContrast:          return _CViewProp::SetHighContrast(this, pVol, nVolSize);
    case eAppPropUseSystemFrame:        return _CViewProp::SetUseSystemFrame(this, pVol, nVolSize);
    case eAppPropWebview2SSOUsingOSA:   return _CViewProp::Set(s_bWebview2AllowSingleSignOnUsingOSPrimaryAccount, pVol, nVolSize);
    case eAppPropWebview2Arguments:     return _CViewProp::Set(s_strWebview2Arguments, pVol, nVolSize);
    case eAppPropWebview2Language:      return _CViewProp::Set(s_strWebview2Language, pVol, nVolSize);
    case eAppPropWebview2ExecutableFolder:  return _CViewProp::Set(s_strWebview2ExecutableFolder, pVol, nVolSize);
    case eAppPropWebview2UserDataFolder:    return _CViewProp::Set(s_strWebview2UserDataFolder, pVol, nVolSize);
    case eAppPropShowHintOnEllipsis:    return _CViewProp::Set(m_bShowHintOnEllipsis, pVol, nVolSize);
    case eAppPropShowHintOnTabKey:      return _CViewProp::Set(m_bShowHintOnTabKey, pVol, nVolSize);
    case eAppPropDisabledColorText:     return _CViewProp::SetT(m_clrDisableText, pVol, nVolSize);
    case eAppPropHintColorBk:           return _CViewProp::SetT(g_mgrTooltip.m_clrBk, pVol, nVolSize);
    case eAppPropHintColorText:         return _CViewProp::SetT(g_mgrTooltip.m_clrText, pVol, nVolSize);
    case eAppPropHintLayered:           return _CViewProp::SetT(g_mgrTooltip.m_bLayedTooltip, pVol, nVolSize);
    case eAppPropEditColorBkSel:        return _CViewProp::SetT(m_clrEditSelBackground, pVol, nVolSize);
    case eAppPropEditColorTextSel:      return _CViewProp::SetT(m_clrColorEditSelText, pVol, nVolSize);
    case eAppPropEditColorPrompt:       return _CViewProp::SetT(m_clrColorEditPrompt, pVol, nVolSize);
//     case eAppPropFocusPic:          return CPropT<ITuiPic*>::Put(m_ptrFocus, pVol, nVolSize);
    }

    return false;
}

TMBool CView::GetProperty(EAppProperty idProp, void* pVol, int nVolSize)
{
    if (!pVol || !nVolSize)
        return false;
    switch (idProp)
    {
    case eAppPropAnimationButton:       return _CViewProp::Get(m_bAnimationButton, pVol, nVolSize);
    case eAppPropAnimationScroll:       return _CViewProp::Get(m_bAnimationScroll, pVol, nVolSize);
    case eAppPropAnimationSysBut:       return _CViewProp::Get(m_bAnimationSysBut, pVol, nVolSize);
    case eAppPropHighContrast:          return _CViewProp::Get(m_bHighContrast, pVol, nVolSize);
    case eAppPropUseSystemFrame:        return _CViewProp::Get(m_bSystemFrame, pVol, nVolSize);
    case eAppPropWebview2SSOUsingOSA:   return _CViewProp::Get(s_bWebview2AllowSingleSignOnUsingOSPrimaryAccount, pVol, nVolSize);
    case eAppPropShowHintOnEllipsis:    return _CViewProp::Get(m_bShowHintOnEllipsis, pVol, nVolSize);
    case eAppPropShowHintOnTabKey:      return _CViewProp::Get(m_bShowHintOnTabKey, pVol, nVolSize);
    case eAppPropDisabledColorText:     return _CViewProp::GetT(m_clrDisableText, pVol, nVolSize);
    case eAppPropHintColorBk:           return _CViewProp::GetT(g_mgrTooltip.m_clrBk, pVol, nVolSize);
    case eAppPropHintColorText:         return _CViewProp::GetT(g_mgrTooltip.m_clrText, pVol, nVolSize);
    case eAppPropHintLayered:           return _CViewProp::GetT(g_mgrTooltip.m_bLayedTooltip, pVol, nVolSize);
    case eAppPropEditColorBkSel:        return _CViewProp::GetT(m_clrEditSelBackground, pVol, nVolSize);
    case eAppPropEditColorTextSel:      return _CViewProp::GetT(m_clrColorEditSelText, pVol, nVolSize);
    case eAppPropEditColorPrompt:       return _CViewProp::GetT(m_clrColorEditPrompt, pVol, nVolSize);
//     case eAppPropFocusPic:          return CPropT<ITuiPic*>::Get(m_ptrFocus, pVol, nVolSize);
    }
    return false;
}

TMBool CView::AddMessageFilter(CTuiMsgFilter* pMessageFilter)
{
    return m_aFilter.Append(pMessageFilter);
}

TMBool CView::RemoveMessageFilter(CTuiMsgFilter* pMessageFilter)
{
    return m_aFilter.Erase(pMessageFilter);
}

void CView::GetMouseHotState(TUIHITINFO& ht)
{
    ht = m_htOver;
}

TMBool CView::GetMouseLDownState(TUIHITINFO& ht)
{
    ht = m_htDownL;
    return MK_LBUTTON & m_dwState;
}

TMBool CView::GetMouseRDownState(TUIHITINFO& ht)
{
    ht = m_htDownR;
    return MK_RBUTTON & m_dwState;
}

TMBool CView::GetMouseMDownState(TUIHITINFO& ht)
{
    ht = m_htDownM;
    return MK_MBUTTON & m_dwState;
}

TMBool CView::GetGestureState(TUIHITINFO& ht, int nID)
{
    return g_input.Gesture().GetState(ht, nID);
}

CTuiWnd* CView::CreateMgr(TMHModule hInst, int nID, int dpi)
{
#ifdef TUI_EDIT_MODE
    tuiPlugInCreate(true);
#endif

    CView* p = this;
    bool b = p->m_bSystemFrame;
    p->m_bSystemFrame = false;
    CTuiWnd* pMgr = CMgrPtr::CreateNative(p->GetModule(hInst), 0, nID, dpi); //CFManagerPtr(p->GetModule(hInst)).CreateObject (0, nID);
    p->m_bSystemFrame = b;
    DbgVerify(pMgr);
    DbgAssert(pMgr);
    return pMgr;
}

// this function call only on editor.
HTUI CView::CreateDlgEx(TMHModule hInst, int nID, TMHWnd hWndParent, TUIFACTORY* pFactory, void* param)
{
    DbgAssert(!hWndParent);

    CTuiWnd* pMgr = CreateMgr(hInst, nID, TUI_DPI_SYSTEM_DEFAULT);
    if (!pMgr)
        return 0;
    
    Ref ref (pMgr);
    if (!pMgr->InitManager(pFactory, param))
        return 0;

    DbgAssert(!pMgr->IsLifeInvalid());
    return pMgr;
}

HTUI CView::CreateDlgByHWND(TMHModule hInst, int nID, TMHWnd hWnd, TUIFACTORY* pFactory, void* param)
{
    DbgAssert(0);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// IGXAppNotify
void CView::OnGXDeviceChanged(IGXApp* pNew)
{
    DispatchModule(CM_RENDERCHANGED, 0, 0);
}

TMBool CView::IsHoting(HTUI hWnd) const
{
    HTUI hChild = m_htOver.hWnd;
    if (!hChild)
        return false;
    return hChild == hWnd || hWnd->IsChild(hChild);
}

TMBool CView::DoMenuPop(HTUI hOwner, HTUI hWnd, HTUI hFocus)
{
    if (!hWnd || hOwner == hWnd)
        return false;
    
    // close old pop window
    int nLevel = GetMenuPopLevel(hOwner, false);
    EndMenuPopLevel(nLevel + 1, IDCANCEL, false, 0);
    
    // pop new window
    g_menuPop.DoMenuPop(hOwner, hWnd, hFocus);

    return true;
}

HTUI CView::GetMenuPop(HTUI hWnd)
{
    return g_menuPop.GetMenuPop(hWnd);
}

int CView::GetMenuPopLevel(HTUI hWnd, TMBool bOwner)
{
    return g_menuPop.GetMenuPopLevel(hWnd, bOwner);
}

TMBool CView::EndMenuPopLevel(int nLevel, int nResult, TMBool bRestoreFocus, TUIMSG* p)
{
    return g_menuPop.EndMenuPopLevel(nLevel, nResult, bRestoreFocus, p);
}

TMBool CView::GetMenuPopInfo(int nLevel, HTUI* pOwner, HTUI* pPop)
{
    return g_menuPop.GetMenuPopInfo(nLevel, pOwner, pPop);
}

HTUI CView::GetCapture()
{
    return m_pCapture;
}

HTUI CView::GetFocus() // manager always can't focus.
{
    return CFocus::GetFocus();
}

static void _CView_SendCaptureChangedMessage(HTUI& hCapture, HTUI hNew)
{
    HTUI hOld = hCapture;
    hCapture = hNew;
    hOld->SendMessage(WM_CAPTURECHANGED, 0, 0);
}

void CView::KillFocus()
{
//     DbgTrace("CView::InternalSetFocus");

    InternalSetFocus(0);
}

TMBool CView::DoPreTranslateMessage(TUIMSG* pMsg, TMResult& lResult)
{
    return m_aFilter.PreTranslateMessage(pMsg, lResult);
}

TMBool CView::DoPreTranslateAppMessage(HTUI hMgr, TMHWnd hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult)
{
    TUIMSG msg = { hWnd, uMsg, wParam, lParam };
    CTuiMsg msgTui(hMgr, CM_APPMESSAGE, 0, (TMLParam)&msg);
    return m_aFilter.PreTranslateMessage(&msgTui, lResult);
}

static TMResult _SendMouseMessage(TMBool bNcMsg, CTuiWgt* pWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam)
{
    DbgAssert(pWnd);
    if (pWnd->IsLifeDestroyed())
        return false;

    if (bNcMsg)
    {
        switch (uMsg)
        {
        case WM_MOUSEMOVE: uMsg = WM_NCMOUSEMOVE; break;
            
        case WM_LBUTTONDOWN: uMsg = WM_NCLBUTTONDOWN; break;
        case WM_LBUTTONUP: uMsg = WM_NCLBUTTONUP; break;
        case WM_LBUTTONDBLCLK: uMsg = WM_NCLBUTTONDBLCLK; break;
            
        case WM_RBUTTONDOWN: uMsg = WM_NCRBUTTONDOWN; break;
        case WM_RBUTTONUP: uMsg = WM_NCRBUTTONDOWN; break;
        case WM_RBUTTONDBLCLK: uMsg = WM_NCRBUTTONDOWN; break;
            
        case WM_MBUTTONDOWN: uMsg = WM_NCRBUTTONDOWN; break;
        case WM_MBUTTONUP: uMsg = WM_NCRBUTTONDOWN; break;
        case WM_MBUTTONDBLCLK: uMsg = WM_NCRBUTTONDOWN; break;
            
        case CM_MOUSEENTRY: uMsg = CM_NCMOUSEENTRY; break;
        case WM_MOUSELEAVE: uMsg = CM_NCMOUSELEAVE; break;
            
        case WM_SETCURSOR: uMsg = CM_NCSETCURSOR; break;
        }
    }
    //        DbgTrace("hittest%x: obj:%x; msg:%x; code:%d, nc:%x", GetTickCount(), CTuiWnd::s_htOver.hWnd, uMsg, CTuiWnd::s_htOver.dwCode, CTuiWnd::s_htOver.bNC);
    return pWnd->SendMessage(uMsg, wParam, lParam);
}

static TMBool _CView_IsSameHitInfo(TUIHITINFO& ho, TUIHITINFO& htNew)
{
    return ho.hWnd == htNew.hWnd
        && ho.bNC == htNew.bNC
        && ho.dwCode == htNew.dwCode
        && ho.dwCode2 == htNew.dwCode2
        && ho.pTaget == htNew.pTaget;
}

static void _CView_SetClkMessage(TUIHITINFO& htOld, TUIHITINFO& htNew, CTuiPoint pt)
{
    htOld = htNew;
    htOld.pt = pt;
}

// return: is skip message
static TMBool _CView_SetDblClkMessage(TUIHITINFO& htOld, TUIHITINFO& htNew, CTuiPoint pt)
{
    if (!_CView_IsSameHitInfo(htOld, htNew))
        return true;
    _CView_SetClkMessage(htOld, htNew, pt);
    return false;
}

void CView::DispatchBasicMouseMessage (HTUI hOwner, TMUInt uMsg, TMWParam wParam, CPointI ptCurI, int dx, int dy)
{
    DbgAssert(uMsg >= WM_MOUSEFIRST && uMsg <= WM_MBUTTONDBLCLK);

    CTuiZoom dpi(hOwner->GetDPI());
    CTuiPoint ptCur(dpi.DPtoLP(ptCurI.X() - dx), dpi.DPtoLP(ptCurI.Y() - dy));
    m_dwState &= ~0x1ff;
    m_dwState |= (wParam & 0xff);
    
    MoveMouse(hOwner, ptCur, uMsg);

    //    DbgTrace("hittest%x: obj:%x; code:%d, nc:%x", GetTickCount(), s_htOver.hWnd, s_htOver.dwCode, s_htOver.bNC);
    TMBool bSkip = false;
    switch (uMsg)
    {
    case WM_LBUTTONDOWN: _CView_SetClkMessage(m_htDownL, m_htOver, ptCur); break;
    case WM_RBUTTONDOWN: _CView_SetClkMessage(m_htDownR, m_htOver, ptCur); break;
    case WM_MBUTTONDOWN: _CView_SetClkMessage(m_htDownM, m_htOver, ptCur); break;
    case WM_LBUTTONDBLCLK: bSkip = _CView_SetDblClkMessage(m_htDownL, m_htOver, ptCur); break;
    case WM_RBUTTONDBLCLK: bSkip = _CView_SetDblClkMessage(m_htDownR, m_htOver, ptCur); break;
    case WM_MBUTTONDBLCLK: bSkip = _CView_SetDblClkMessage(m_htDownM, m_htOver, ptCur); break;
    }

    HTUI pNew = m_htOver.hWnd;
    DbgAssert(pNew == m_htOver.hWnd);
    if (pNew && !bSkip)
    {
        TM_IF_DBG(TUIHITINFO htOld = m_htOver; htOld);
        DbgAssert(pNew == m_htOver.hWnd);
        DbgAssert(pNew->IsWindowEnabled());
        TMLParam lParam = ptCur.ToLPARAM();// MAKEWPARAM(pt.x, pt.y);
        _SendMouseMessage(m_htOver.bNC, pNew, uMsg, wParam, lParam);
    }
    g_mgrTooltip.RelayMouseEvent(hOwner, uMsg, wParam, ptCur.ToLPARAM());
}

void CView::MoveMouse(HTUI hOwner, TUIPoint pt, TMUInt uMsg)
{
    if (m_pCapture)
    {
        m_htPrev = m_htOver;
        m_htOver.pt = pt;
        m_htOver.hWnd = m_pCapture;
        return;
    }
 
    DbgAssert(hOwner);

    TUIHITINFO ht = {0};
    ht.pt = pt;
    hOwner->GetAndHitMouseTarget(ht, uMsg, CWP_SKIPBINDWINDOW);
//     DbgAssert(!hOwner->IsLifeDestroyed());

    DbgAssert(!ht.hWnd  || !ht.hWnd->IsLifeInvalid());
    DbgAssert(!ht.hHint || !ht.hHint->IsLifeInvalid());

    SetHitHot(ht);
}

TMBool CView::SetCapture (CTuiWgt* pWin)
{
    if (m_pCapture == pWin)
        return true;

    if (pWin)
    {
        if (pWin->IsLifeInvalid())
            return false;

        TMUInt32 dwStyle = pWin->GetStyle ();
        if ((!(dwStyle & WS_VISIBLE)) || (dwStyle & WS_DISABLED))
        {
            return false;
        }

//         DbgTrace("CView::SetCapture(HTUI:%x, TMHWnd:%x)", pWin, pWin->GetHWND());

//        m_htOver.hWnd = pWin;
        if (m_pCapture)
        {
            _CView_SendCaptureChangedMessage(m_pCapture, pWin);
            return true;
        }

        m_pCapture = pWin;
//        m_dwState |= WST_CAPTURED;
        InternalSetCapture (pWin);

        //DbgAssert(CTuiWidget(pWin).IsCapture());
    }
    else
    {
        InternalReleaseCapture();
        if (m_pCapture)
            _CView_SendCaptureChangedMessage(m_pCapture, 0);
    }
//        ReleaseCapture();

    return true;

}

// void CView::RecheckHot()
// {
//     HTUI hWgt = m_htOver.hWnd;
//     if (!hWgt)
//         return;
//     hWgt = hWgt->GetOwner();
//     MoveMouse(hWgt, m_htOver.pt, 0);
// }

void CView::SetHitHot(const TUIHITINFO& ht)
{
    DbgAssert(!m_htOver.hWnd || !m_htOver.hWnd->IsLifeInvalid());
    DbgAssert(!m_htOver.hHint || !m_htOver.hHint->IsLifeInvalid());
    DbgAssert(!m_htPrev.hWnd || !m_htPrev.hWnd->IsLifeInvalid());
    DbgAssert(!m_htPrev.hHint || !m_htPrev.hHint->IsLifeInvalid());
    DbgAssert(!ht.hWnd || !ht.hWnd->IsLifeInvalid());
    DbgAssert(!ht.hHint || !ht.hHint->IsLifeInvalid());
    _CheckRef(m_htOver.hWnd);
    _CheckRef(m_htOver.hHint);
    _CheckRef(m_htPrev.hWnd);
    _CheckRef(m_htPrev.hHint);
    _CheckRef(ht.hWnd);
    _CheckRef(ht.hHint);

    // save ht.
    m_htPrev = m_htOver;
    m_htOver = ht;
    if (!m_htOver.hHint)
        m_htOver.hHint = ht.hWnd;
    if (ht.hWnd && !ht.hWnd->CanInput())
        m_htOver.hWnd = 0;

    // tooltip
    if (m_htOver.hHint != m_htPrev.hHint)
    {
//         DbgTrace("CView::SetHitHot::tooltip(old:%p, new:%p)", m_htPrev.hHint, m_htOver.hHint);
        g_mgrTooltip.SetToolTip(m_htOver.hHint, eTTActiveNormal);
    }

    // check mouse leave and entry message
    if (_CView_IsSameHitInfo(m_htPrev, m_htOver))
        return;

    Ref po(m_htPrev.hWnd);
    Ref pNew(m_htOver.hWnd);
    if (po)
    {
        _SendMouseMessage(m_htPrev.bNC, po, WM_MOUSELEAVE, (TMWParam)&m_htPrev, (TMLParam)&m_htOver);
        if (po == m_htPrev.hWnd)
            _SendMouseMessage(m_htPrev.bNC, po, CM_MOUSESWITCH, (TMWParam)&m_htPrev, (TMLParam)&m_htOver);
    }
    
    if (pNew && pNew == m_htOver.hWnd)
    {
        _SendMouseMessage(ht.bNC, pNew, CM_MOUSEENTRY, (TMWParam)&m_htPrev, (TMLParam)&m_htOver);
        if (pNew != po && pNew == m_htOver.hWnd)
            _SendMouseMessage(ht.bNC, pNew, CM_MOUSESWITCH, (TMWParam)&m_htPrev, (TMLParam)&m_htOver);
        if (pNew == m_htOver.hWnd)
            _SendMouseMessage(ht.bNC, pNew, WM_SETCURSOR, 0, MAKELONG(HTCLIENT, CM_MOUSEENTRY));
    }

    if (pNew != po)
        FRAMEINFO::InvalidateHotScrollbar(po, pNew);
}

void CView::SetHitHotLeave(HTUI hMgr, TMHWnd hWnd)
{
    if (!m_htOver.hWnd)
        return;   // have been leave
//     if (hWnd != m_htOver.hWnd->GetHWND()) // !WIN::TrackMouseLeave(hWnd ? hWnd : hMgr->GetHWND(), false, true))
//         return;
//     if (!WIN::TrackMouseLeave(hWnd ? hWnd : hMgr->GetHWND(), false, true))
//         return;
//     if (hMgr != m_htOver.hWnd && !hMgr->IsChild(m_htOver.hWnd))
//         return;   // the new hot is in another TMHWnd, so have been leave

    TUIHITINFO ht = {0};
    SetHitHot(ht);
}

TMBool CView::DispatchAppMessage(HTUI hMgr, TMHWnd hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult)
{
    if (AppOnKey(hMgr, uMsg, wParam, lParam))
        return true;

//     if ((uMsg >= WM_KEYDOWN && uMsg <= WM_KEYLAST)
//         || uMsg == WM_HELP || /*uMsg == WM_CANCELMODE ||*/ uMsg == WM_IME_CHAR
//         || uMsg == WM_KILLFOCUS || uMsg == WM_SETFOCUS)
//     {
//         lResult = hMgr->SendMessage(uMsg, wParam, lParam);
//         return true;
//     }
    
    switch (uMsg)
    {
    case WM_CONTEXTMENU:
        {
            CTuiWgt* pHot;
            if (lParam == KEYBOARD_MENU) // Keyboard generated menu
                pHot = GetFocus();
            else
                pHot = GetOver();
            if (pHot)
                pHot->SendMessage(WM_CONTEXTMENU, wParam, lParam);
        }
        return true;

//     case WM_SETCURSOR:
//         if (LOWORD(lParam) == HTCLIENT && m_htOver.hWnd)
//         {
//             lResult = _SendMouseMessage (m_htOver.bNC, m_htOver.hWnd, uMsg, wParam, lParam);
//             return lResult;
//         }
//         break;
        
    case WM_MOUSEWHEEL:
        if (m_htOver.hWnd)
        {
            lResult = m_htOver.hWnd->SendMessage(uMsg, wParam, lParam);
            return true;
        }
        break;
        
    case WM_MOUSELEAVE:
    case WM_NCMOUSELEAVE:
        {
//             DbgTrace("WM_MOUSELEAVE 0x%X %d", hWnd, GetTickCount());
            TMResult lr;
            CheckCaptureChange(lr);
            SetHitHotLeave(hMgr, hWnd);
        }
        return true;
        
//     case WM_ACTIVATE:
//         if (WA_INACTIVE != LOWORD(wParam))
//         {
// //             SetFocus(hMgr->GetDefalutItem(), false);
//             break;
//         }
//         else
//             hMgr->SetDefaultItem(GetFocus());
        // through
    case WM_CAPTURECHANGED:
        return CheckCaptureChange(lResult);

    case WM_SETFOCUS:
        AppOnSetFocus(hMgr, hWnd);
        break;
        
//     case WM_KILLFOCUS:
//         AppOnKillFocus(hMgr, hWnd);
//         break;

    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
        {
            CSize sz(lParam);
            OnAppSize(hMgr, sz.Width(), sz.Height());
        }
        break;

//     case CM_ZOOMED:
//         {
//             int zm = GetZoom();
//             sz.cx = sz.cx * 100 / zm;
//             sz.cy = sz.cy * 100 / zm;
//             hMgr->SetWindowPos(0, 0, 0, sz.cx, sz.cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
//         }
//         break;

    }

    return false;
}

TMBool CView::CheckCaptureChange(TMResult& lResult)
{
    if (m_pCapture)
    {
//         DbgTrace("CView::CheckCaptureChange(HTUI:%x, TMHWnd:%x)", m_pCapture, m_pCapture->GetHWND());
        HTUI p = m_pCapture;
        m_pCapture = 0;
        lResult = p->SendMessage(WM_CAPTURECHANGED, 0, 0);
        return true;
    }
    return false;
}


void CView::OnAppSize(HTUI pMgr, long width, long height)
{
    CTuiZoom zm(pMgr->GetDPI());
    long nOldW = zm.LPtoDP(pMgr->GetWidth());
    long nOldH = zm.LPtoDP(pMgr->GetHeight());
    if (nOldW == width && nOldH == height)
        return;
    GXDips w = (zm.DPtoLP(width));
    GXDips h = (zm.DPtoLP(height));
    pMgr->CTuiWgt::SetWindowPos(0, 0, 0, w, h, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
//    DispatchMessage(pMgr, CM_RESIZE, width, height);
}

inline static void CheckClear(TUIHITINFO& hi, CTuiWgt* p)
{
    if (hi.hWnd == p)
        hi.hWnd = 0;
    if (hi.hHint == p)
        hi.hHint = 0;
}

void CView::ClearChildInfo(CTuiWgt* p, TMUInt uMsg)
{
    if (p->IsLifeInvalid())
        uMsg = WM_DESTROY;

    // drag, focus, capture
    CDragData::StopDrag(p);
    CleanFocus(p);
    if (p == m_pCapture)
        ReleaseCapture();
    DbgAssert(p != m_pCapture);
    
    // tooltip; disabled window can show tooltip
    if (uMsg != WM_ENABLE && uMsg != CM_ENABLE_P && g_mgrTooltip.IsTooltipTarget(p))
        g_mgrTooltip.SetToolTip(0, eTTActiveNormal);
    
    if (uMsg == WM_DESTROY)
    {
        CheckClear(m_htOver, p);
        CheckClear(m_htPrev, p);
        CheckClear(m_htDownL, p);
        CheckClear(m_htDownR, p);
        CheckClear(m_htDownM, p);
        g_input.RemoveFocusChain(p);
    }
//     else if (m_htOver.hWnd == p)
//     {
//         TUIHITINFO ht = {0};
//         SetHitHot(ht);
//     }

    g_input.Clear(p);
}

void CView::ClearChildHitInfo (CTuiWgt* p, void* pItem)
{
    if (m_htDownL.hWnd == p && m_htDownL.pTaget == pItem)
        m_htDownL.pTaget = 0;
    if (m_htDownR.hWnd == p && m_htDownR.pTaget == pItem)
        m_htDownR.pTaget = 0;
    if (m_htDownM.hWnd == p && m_htDownM.pTaget == pItem)
        m_htDownM.pTaget = 0;
    if (m_htOver.hWnd == p && m_htOver.pTaget == pItem)
        m_htOver.pTaget = 0;
    g_input.Gesture().ClearItem(p, pItem);
//     for (int i = 0; i < tm_countof(m_htGesture); ++i)
//     {
//         if (m_htGesture[i].hWnd == p && m_htGesture[i].pTaget == pItem)
//             m_htGesture[i].pTaget = 0;
//     }
}


TM_END_NAMESPACE



