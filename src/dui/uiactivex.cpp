/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.02.02
 *
 *  Copyright (C) 2006 miragekiller
 */

#include "uiactivex.h"
#include "../tmduiatl.h"
#include <atlcom.h>

#pragma comment(lib, "Rpcrt4.lib")


TM_BGN_NAMESPACE


///////////////////////////////////////////////////////////////////////////////
// CActiveXCont
CActiveXCont::CActiveXCont()
    : m_bInPlaceActive(false)
    , m_bUIActive(false)
//     , m_bMDIApp(false)
//     , m_bCapture(false)
//     , m_bHaveFocus(false)
    , m_bInReleaseAll(false)
    , m_bShow(false)
//     , m_hAccel(0)
{
    AXTRACE("CActiveXCont::CActiveXCont");
}

TMBool CActiveXCont::AttachWidget(CTuiDummyBase* hWnd, const CLSID& iid)
{
    CDummyChild::AttachWidget(hWnd);
//     CreateControl(iid); // TM_T2W(GetWindowText()));

    C3rdAutoDestroy<CActiveXCont> _auto(this);

    TMResult hr = S_FALSE;
    DbgAssert(!m_spUnknown);
    
    CComPtr<IUnknown> spUnkControl;
    spUnkControl.CoCreateInstance(iid, 0, CLSCTX_INPROC_SERVER);
//    spUnkControl.CoCreateInstance(CLSID_InternetExplorer, 0, CLSCTX_SERVER);
    
    if (spUnkControl)
        hr = BindObject(spUnkControl);
    
    if (FAILED(hr) || m_spUnknown == NULL)
    {
        // We don't have a control or something failed so release
        ReleaseAll();
        return false;
    }

    m_pWnd->PostMessage(CAX_ONOBJINIT, 0, 0);
    return true;
}

TMUInt32 CActiveXCont::SyncGetScreenPos(HTUI hWgt, CRectI& rc)
{
    if (!m_wnd)
        return 0;
    return CDummyChild::SyncGetScreenPos(hWgt, rc);
}

void CActiveXCont::DoSyncSetScreenPos(TMUInt32 flag)
{
    AXTRACE("CActiveXCont::DoSyncSetScreenPos() %d", _GetRef());
    DbgAssert(m_pWnd->IC_IsValidInstance(true));

    C3rdAutoDestroy<CActiveXCont> _auto(this);

    if ((flag & SWP_HIDEWINDOW) && m_bShow)
    {
        //         DbgAssert(m_bShow);
        m_bShow = false;
        DoVerb(OLEIVERB_HIDE);
    }

    // flash OCX have bug: if show and move, it need show first
    TMBool bMoveSize;
    if (!((flag & SWP_NOSIZE) && (flag & SWP_NOMOVE)))
    {
        bMoveSize = true;
        UpdatePixelPaddingRect();
    }
    else
        bMoveSize = false;
    
    if ((flag & SWP_SHOWWINDOW) && !m_bShow)
    {
        DoVerb(OLEIVERB_INPLACEACTIVATE, &m_rcPos); // OLEIVERB_SHOW will auto set focus
        m_bShow = true;
    }
    
    if (bMoveSize)
    {
        CComQIPtr<IOleInPlaceObject> ptr(m_spUnknown);
        if (ptr)
            DbgVerifyHResult(ptr->SetObjectRects(&m_rcPos, &m_rcPos));
    }
}

////////////////////////////////////////////////////////////////////////////////
// protected function
void CActiveXCont::ReleaseAll()
{
    AXTRACE("CActiveXCont::ReleaseAll(%p)", this);

    if (!m_spUnknown)
    {
        DbgAssert(!m_spOleObject);
//         DbgAssert(!m_spInPlaceFrame);
//         DbgAssert(!m_spInPlaceUIWindow);
        DbgAssert(!m_spViewObject);
//         DbgAssert(!m_spInPlaceObjectWindowless);
        DbgAssert(!m_spServices);
        return;
    }

    if (m_bInReleaseAll)
        return;
    m_bInReleaseAll = TRUE;

//     try
    {
//     RemoveHook();
//         HWND hWnd = m_wnd;

        DetachWidget();
        UnbindHWND();
//    SetWindowLess(true);
//     ::DestroyWindow(hWnd);

   if (m_spViewObject != NULL)
        DbgVerifyHResult(m_spViewObject->SetAdvise(DVASPECT_CONTENT, 0, NULL));
    
//     if (m_dwAdviseSink != 0xCDCDCDCD)
//     {
//         AtlUnadvise(m_spUnknown, m_iidSink, m_dwAdviseSink);
//         m_dwAdviseSink = 0xCDCDCDCD;
//     }
    
    if (m_spOleObject)
    {
        DbgVerifyHResult(m_spOleObject->Unadvise(m_dwOleObject));
        // the next 2 function will call ::GetMessage.
        DbgVerifyHResult(m_spOleObject->Close(OLECLOSE_NOSAVE));
        DbgVerifyHResult(m_spOleObject->SetClientSite(NULL));
        m_spOleObject.Release();
    }
    
    {
        CComQIPtr<IObjectWithSite> spSite(m_spUnknown);
        if (spSite)
            DbgVerifyHResult(spSite->SetSite(NULL));
    }
    
    m_spViewObject.Release();
//     m_spInPlaceObjectWindowless.Release();
    m_spUnknown.Release();
    m_spServices.Release();
    }
//     catch(...)
//     {
//         m_spUnknown.Detach();
//         m_spOleObject.Detach();
//         m_spServices.Detach();
//         m_spViewObject.Detach();
//     }
    
//     m_spInPlaceUIWindow.Release();
//     m_spInPlaceFrame.Release();
    
    m_dwViewObjectType = 0;
    m_bInPlaceActive = FALSE;
//    m_bWindowless = FALSE;
//     m_bInPlaceActive = FALSE;
    m_bUIActive = FALSE;
//     m_bCapture = FALSE;
    m_bInReleaseAll = FALSE;
    m_bShow = false;
    
//     if (m_hAccel != NULL)
//     {
//         DestroyAcceleratorTable(m_hAccel);
//         m_hAccel = NULL;
//     }
}

TMResult CActiveXCont::BindObject(IUnknown* pUnkControl)
{
//    ::ShowWindow(m_pWnd->GetHWND(), SW_SHOW);

    if (pUnkControl == NULL)
        return S_OK;
    AXTRACE("CActiveXCont::BindObject");
    
    m_spUnknown = pUnkControl;
    
    HRESULT hr = S_OK;
    m_spOleObject = pUnkControl;
    if (m_spOleObject)
    {
        DbgVerifyHResult(m_spOleObject->GetMiscStatus(DVASPECT_CONTENT, &m_dwMiscStatus));
        if (m_dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST)
        {
            CComQIPtr<IOleClientSite> spClientSite(GetControllingUnknown());
            DbgVerifyHResult(m_spOleObject->SetClientSite(spClientSite));
        }
        
        TMBool bInited = false;


        if (!bInited) // If user hasn't initialized the control, initialize/load using IPersistStreamInit or IPersistStream
        {
            m_pWnd->Notify(XN_INIT);
            if (m_pWnd->IsLifeInvalid())
                return E_FAIL;

//             CComQIPtr<IPersistStreamInit> spPSI(m_spOleObject);
//             if (spPSI)
//                 hr = spPSI->InitNew();
//             
//             CComQIPtr<IPersistPropertyBag> ptrP(pUnkControl);
//             if (ptrP)
//             {
//                 IPropertyBagImpl_Xml bag(strInit.substr(38));
//                 if (bag.m_ptrXml && SUCCEEDED(ptrP->Load(&bag, 0)))
//                     bInited = true;
//             }

            if (FAILED(hr)) // If the initialization of the control failed...
            {
                // Clean up and return
                if (m_dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST)
                    DbgVerifyHResult(m_spOleObject->SetClientSite(NULL));
                
                m_dwMiscStatus = 0;
                m_spOleObject.Release();
                m_spUnknown.Release();
                
                return hr;
            }
        }
        
        if (0 == (m_dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST))
        {
            CComQIPtr<IOleClientSite> spClientSite(GetControllingUnknown());
            DbgVerifyHResult(m_spOleObject->SetClientSite(spClientSite));
        }
        
        m_dwViewObjectType = 0;
        hr = m_spOleObject->QueryInterface(__uuidof(IViewObjectEx), (void**) &m_spViewObject);
        if (FAILED(hr))
        {
            hr = m_spOleObject->QueryInterface(__uuidof(IViewObject2), (void**) &m_spViewObject);
            if (SUCCEEDED(hr))
                m_dwViewObjectType = 3;
        } else
            m_dwViewObjectType = 7;
        
        if (FAILED(hr))
        {
            hr = m_spOleObject->QueryInterface(__uuidof(IViewObject), (void**) &m_spViewObject);
            if (SUCCEEDED(hr))
                m_dwViewObjectType = 1;
        }
        CComQIPtr<IAdviseSink> spAdviseSink(GetControllingUnknown());
        DbgVerifyHResult(m_spOleObject->Advise(spAdviseSink, &m_dwOleObject));
        
        if ((m_dwMiscStatus & OLEMISC_INVISIBLEATRUNTIME) == 0)
        {
            if (!Edit_IsObjectInDocMode(m_pWnd))
            {
                DoVerb(OLEIVERB_INPLACEACTIVATE, 0); // active and don't show toolbar and menubar
//#ifdef TM_DBG
//                for (CWindowAT hWnd = m_wnd; hWnd; hWnd = hWnd.GetWindow(GW_CHILD))
//                {
//                    TMBool b = hWnd.GetStyle() & WS_VISIBLE;
//                    RECT rc;
//                    hWnd.GetWindowRect(&rc);
//                    int x = 0;
//                }
//#endif
            }
        }
    }

    m_bShow = true;
    DoSyncSetScreenPos(SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE);

    CComQIPtr<IObjectWithSite> spSite(pUnkControl);
    if (spSite != NULL)
        DbgVerifyHResult(spSite->SetSite(GetControllingUnknown()));
    
    return hr;
}

void CActiveXCont::DoVerb(TMLong lVerb, LPCRECT lprcPosRect)
{
    AXTRACE("CActiveXCont::DoVerb(%d) %d", lVerb, _GetRef());
    if (m_spOleObject)
        DbgVerifyHResult(m_spOleObject->DoVerb(lVerb, NULL, this, 0, 0, lprcPosRect));
}

void CActiveXCont::StopNoRedirect()
{
    // IE: ExecWB(OLECMDID_STOP, &var) and ptr->Stop() will navigate to res://ieframe.dll/navcancl.htm
    CComVariant var(false);
    CComQIPtr<IOleCommandTarget> ptrCmdTarget(m_spUnknown);
    if (ptrCmdTarget)
        DbgVerifyHResult(ptrCmdTarget->Exec(0, OLECMDID_STOP, OLECMDEXECOPT_DODEFAULT, &var, 0));
}

void CActiveXCont::UpdatePixelPaddingRect()
{
    AXTRACE("CActiveXCont::UpdatePixelPaddingRect");
    CTuiRect rc;
    m_pWnd->GetPaddingRect(&rc);
    //         CTuiZoom wzm(GetDPI());
    //         wzm.LPtoDP(m_rcPos);
    //         if (Edit_IsObjectInDocMode(this))
    {
        m_pWnd->ClientToScreen(rc, m_rcPos);
        CWindow(m_pWnd->GetHWND()).ScreenToClient(&m_rcPos);
    }
}

// CHookCallWnd
TMBool CActiveXCont::OnCallWndProc(CWPSTRUCT* ps)
{
    if (ps->message == WM_DESTROY && ps->hwnd == ::GetParent(m_wnd))
    {
        DestroyDummy(eUnbindDummyDetach);  // don't let system destroy window.
        return false;
    }

//     {
//         // don't let system destroy window.
// //         DbgVerify(::ShowWindow(m_wnd, SW_HIDE));
// //         DbgVerify(::SetParent(m_wnd, GetDesktopWindow()));
//         DbgVerify(::SetParent(m_wnd, HWND_MESSAGE));
//     }

    return CDummyChild::OnCallWndProc(ps);
}

// CHookGetMessageEx
// IE process VK_TAB
TMBool CActiveXCont::PreTranslateMessage(MSG* msg)
{
    DbgAssert(!IsWindowLess());
    if (msg->message >= WM_KEYFIRST && msg->message <= WM_KEYLAST)
    {
        CComQIPtr<IOleInPlaceActiveObject> qptr(m_spUnknown);
        if (qptr)
        {
            TMResult ht = qptr->TranslateAccelerator(msg);
            if (S_OK == ht)
                return true;
        }
    }
    return CDummyChild::PreTranslateMessage(msg);
}

struct _CActiveXCont_Close : public CWidgetCallT<_CActiveXCont_Close, CTuiActiveXImpl*>
{
    static void Done(CTuiActiveXImpl* lp)
    {
        DbgTrace("_CActiveXCont_Close::done()");
        CActiveXCont* p = static_cast<CActiveXCont*>((IDummy*)lp->m_ptrDummyImpl);
        DbgAssert(p);
        p->ReleaseAll();
        p->Release();
    }
};

// IDummy
void CActiveXCont::DestroyDummy(EUnbindDummy eReason)
{
    if (m_wnd)
    {
        DbgVerify(::SetParent(m_wnd, HWND_MESSAGE));
        this->UnbindHWND();
    }

//     DispEventUnadviseAll(m_spUnknown);
    StopNoRedirect();
    m_spExtUnknown.Release();

    if (eReason == eUnbindDummyDetach)
    {
        m_pWnd->PostMessage(WM_CLOSE, 0, 0);
        return;
    }

    DbgAssert(m_pWnd);
    CTuiActiveXImpl* p = static_cast<CTuiActiveXImpl*>(m_pWnd);
    DbgAssert(p->m_ptrDummyImpl == this);
    _CActiveXCont_Close::Call(p);

// 
//     this->DetachWidget();
//     this->DestroyRelease();
}

TMBool CActiveXCont::SyncIsTargetVisible()
{
    return m_bShow;
}

void CActiveXCont::SyncSetTargetScreenPos(CRect* rcSreen, TMUInt32 flag)
{
    AXTRACE("CActiveXCont::SyncSetTargetScreenPos");
//     DbgAssert(!IsWindowLess());
    DoSyncSetScreenPos(flag);
}

// IDummy
TMBool CActiveXCont::ProcessWindowMessage(HTUI hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)
{
    switch (uMsg)
    {
//     case WM_DESTROY:
//         DestroyDummy(eUnbindDummyDetach); //StopNoRedirect();
//         break;

    case WM_SETFOCUS:
        AXTRACE ("CActiveXCont::OnSetFocus");
//         m_bHaveFocus = TRUE;
        if (!m_bInReleaseAll)
        {
            if (!m_bUIActive)
            {
                DoVerb(OLEIVERB_UIACTIVATE); // active and show toolbar and menu bar
//                 CComQIPtr<IOleClientSite> spClientSite(GetControllingUnknown());
//                 if (spClientSite != NULL)
//                     m_spOleObject->DoVerb(OLEIVERB_UIACTIVATE, NULL, spClientSite, 0, 0, &m_rcPos); // active and show toolbar and menu bar
            }
        }
        break;

    case WM_FORWARDMSG:
        {
            ATLASSERT(lParam != 0);
            CComQIPtr<IOleInPlaceActiveObject> spInPlaceActiveObject(m_spUnknown);
            if (spInPlaceActiveObject)
            {
                LPMSG lpMsg = (LPMSG)lParam;
                lResult = (spInPlaceActiveObject->TranslateAccelerator(lpMsg) == S_OK);
            }
        }
        return true;

    case CAX_GETHOST:
        {
            IUnknown* pUnk = m_spExtUnknown ? static_cast<IUnknown*>(m_spExtUnknown) : GetUnknown();
            lResult = (TMResult)pUnk;
        }
        return true;

    case CAX_GETCONTROL:
        {
            IUnknown* pUnk = m_spUnknown;
            lResult = (TMResult)pUnk;
        }
        return true;

    case CAX_SETEXT:
        {
            IUnknown* pUnk = (IUnknown*)wParam;
            m_spExtUnknown = pUnk;
        }
        return lResult = true;

    case WM_PRINTCLIENT:
        DbgAssert(!hWnd->IsLifeDestroyed());
        if (hWnd->Is_WS_PRI_DIRTY())
        {
            SyncWindowLess();
            SyncWin(hWnd, true);
            if (hWnd->IsLifeInvalid())
                return true;
        }
        hWnd->RenderSimpleBk(_gxApp, 0, CAppColor::GetColorWindowBk);
        if (m_spViewObject && IsWindowLess())
        {
            CTuiRect rcClient;
            hWnd->GetPaddingRect(&rcClient);
            CRect rcc;
            CTuiZoom zoom(hWnd->GetDPI());
            zoom.LPtoDP(rcClient, rcc);
            int w = rcc.Width(), h = rcc.Height();
            CImage img;
            img.CreateIndirect(w, h, eGXFormat32A);
            if (img)
            {
                ImageFill(img, 0);
                {
                    CImageDC dc(img);
                    CRect rc(0, 0, w, h);
                    DbgVerifyHResult(m_spViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, dc.m_hDC, (RECTL*)&rc, (RECTL*)&rc, NULL, 0));
                }
                CTexture tex;
                tex.CreateFromImage(img);
                tex->Blt(rcClient, CRect(0,0,0,0), GX_BLT_DFTSRCWH);
            }
        }
        return true;

    default:
        break;
    }

    if (IsWindowLess())
    {
        if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) // windowless mode
        {
            // Mouse messages handled when a windowless control has captured the cursor
            // or if the cursor is over the control
            CComQIPtr<IOleInPlaceObjectWindowless> ptr(m_spUnknown);
            if (ptr)
            {
                DWORD dwHitResult = IsCapture() ? HITRESULT_HIT : HITRESULT_OUTSIDE;
                if (dwHitResult == HITRESULT_OUTSIDE && m_spViewObject != NULL)
                {
                    POINT ptMouse = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                    DbgVerifyHResult(m_spViewObject->QueryHitPoint(DVASPECT_CONTENT, &m_rcPos, ptMouse, 0, &dwHitResult));
                }
                if (dwHitResult == HITRESULT_HIT && (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MBUTTONDBLCLK) && m_bInPlaceActive)
                {
                    CComQIPtr<IOleInPlaceObjectWindowless> ptr(m_spUnknown);
                    if (ptr)
                        ptr->OnWindowMessage(uMsg, wParam, lParam, &lResult);
                }
            }
        }
        else if (m_bInPlaceActive
            && ((uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST)
            || (uMsg >= WM_IME_SETCONTEXT && uMsg <= WM_IME_KEYUP)
            || uMsg == WM_HELP || uMsg == WM_CANCELMODE))
        {
            CComQIPtr<IOleInPlaceObjectWindowless> ptr(m_spUnknown);
            if (ptr && (S_OK == ptr->OnWindowMessage(uMsg, wParam, lParam, &lResult)))
                return true;
        }
        else if (uMsg == CM_WINDOWPOSCHANGED)
        {
            CComQIPtr<IOleInPlaceObjectWindowless> ptr(m_spUnknown);
            if (ptr)
            {
                TUIWINDOWPOS* wp = (TUIWINDOWPOS*)lParam;
                DoSyncSetScreenPos(wp->flags & ~(SWP_SHOWWINDOW | SWP_HIDEWINDOW));
            }
        }
        return false;
    }

    if (uMsg == CM_LANGUAGECHANGED)
//            FireAmbientPropertyChange(DISPID_AMBIENT_LOCALEID);
        WIN::SetRTLOnLanguageChanged(m_wnd, lParam);
    return CDummyChild::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult);
}

void CActiveXCont::SyncWindowLess()
{
    HWND hWnd = 0;
    CComQIPtr<IOleInPlaceActiveObject> qptr(m_spUnknown);
    if (qptr)
        DbgVerifyHResult(qptr->GetWindow(&hWnd));
    AXTRACE("CActiveXCont::SyncWindowLess(%p)", hWnd);
    if (hWnd == m_wnd)
        return;
    if (m_wnd)
        UnbindHWND();
    if (hWnd)
        BindHWND(hWnd);

//     CWindow wnd;
//     if (!b)
//     {
//         CComQIPtr<IOleInPlaceActiveObject> qptr(m_spUnknown);
//         DbgAssert(qptr);
//         qptr->GetWindow(&wnd.m_hWnd);
//     }
//     SetWindow(wnd, false);
}


////////////////////////////////////////////////////////////////////////////////
// CTuiActiveXImpl
TM_REGISTER_CLASS1(CTuiActiveX);

CTuiActiveXImpl::CTuiActiveXImpl()
{
    m_dwStyle |= WS_TABSTOP;
    Set_WS_PRI_DIRTY();
}

// message handle
// "{8856F961-340A-11D0-A96B-00C04FD705A2}"
//TMResult CTuiActiveXImpl::OnCreate (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
void CTuiActiveXImpl::Recreate()
{
    DbgAssert(!IsLifeInvalid());
    DbgAssert(!m_ptrDummyImpl);

    CLSID clsid;
    HRESULT hr = CLSIDFromString((BSTR)CString(m_strIID.substr(0, 38)).c_str(), &clsid);
    if (SUCCEEDED(hr))
    {
        CActiveXCont* p = new CRefObjectT<CActiveXCont>();
        p->AttachWidget(this, clsid);
    }
}

// void StreamGetNames(CTuiActiveXImpl& wnd, CSet<CString>& aList)
// {
//     // get property
//     CComDispatchDriver disp(wnd.m_spUnknown);
//     if (!disp)
//         return;
// 
//     CComPtr<ITypeInfo> ptrTypeInfo;
//     disp->GetTypeInfo(0, LOCALE_USER_DEFAULT, &ptrTypeInfo);
//     if (ptrTypeInfo)
//     {
//         LPTYPEATTR pTypeAttr = 0;
//         if (SUCCEEDED(ptrTypeInfo->GetTypeAttr(&pTypeAttr)))
//         {
//             DbgAssert(pTypeAttr != NULL);
//             for (UINT i = 0; i < pTypeAttr->cFuncs; i++)
//             {
//                 FUNCDESC* pfd;
//                 if (FAILED(ptrTypeInfo->GetFuncDesc(i, &pfd)) || (pfd->invkind != INVOKE_PROPERTYPUT))
//                     continue;
//                 
//                 CComBSTR bstrName;
//                 if (SUCCEEDED(ptrTypeInfo->GetDocumentation(pfd->memid, &bstrName, NULL, NULL, NULL)))
//                     aList.insert((BSTR)bstrName);
//                 ptrTypeInfo->ReleaseFuncDesc(pfd);
//             }
//             ptrTypeInfo->ReleaseTypeAttr(pTypeAttr);
//         }
//     }
// }

// static CString MakeIIDString(CLSID& iid)
// {
//     CString str;
//     int n = StringFromGUID2(iid, str.GetBuffer(256), 256);
//     str.SetLength(n ? n - 1 : 0);
//     return str;
// }
// 
// CString CTuiActiveXImpl::GetIIDString()
// {
//     if (!m_spOleObject)
//         return CString();
// 
//     CLSID clsid;
//     m_spOleObject->GetUserClassID(&clsid);
//     return MakeIIDString(clsid);
// }
// 
// static CString MakeXmlString(IPersist* p, IDomElement* ptr)
// {
//     CLSID iid;
//     p->GetClassID(&iid);
//     CString str(MakeIIDString(iid));
//     ptr->transformNode(str);
//     return str;
// }
// 
// CString StreamDoWrite(const CTuiActiveXImpl& _wnd)
// {
//     CTuiActiveXImpl& wnd = (CTuiActiveXImpl&)_wnd;
// 
//     CString strRet;
//     if (!wnd.m_spOleObject)
//         return strRet;
// 
//     IPropertyBagImpl_Xml bag;
//     CComQIPtr<IPersistPropertyBag2> ptrP2(wnd.m_spUnknown);
//     if (ptrP2 && SUCCEEDED(ptrP2->Save(&bag, false, false)))
//         return MakeXmlString(ptrP2, bag.m_ptrXml);
// 
//     CComQIPtr<IPersistPropertyBag> ptrP(wnd.m_spUnknown);
//     if (ptrP && SUCCEEDED(ptrP->Save(&bag, false, false)))
//         return MakeXmlString(ptrP, bag.m_ptrXml);
// 
//     // get clsid
//     strRet = wnd.GetIIDString();
//     strRet += L"<Property ";
// 
//     // get property
//     CComDispatchDriver disp(wnd.m_spUnknown);
//     DbgAssert(disp);
//     CComPtr<ITypeInfo> ptrTypeInfo;
//     disp->GetTypeInfo(0, LOCALE_USER_DEFAULT, &ptrTypeInfo);
//     if (ptrTypeInfo)
//     {
//         LPTYPEATTR pTypeAttr = 0;
//         if (SUCCEEDED(ptrTypeInfo->GetTypeAttr(&pTypeAttr)))
//         {
//             DbgAssert(pTypeAttr != NULL);
//             for (UINT i = 0; i < pTypeAttr->cFuncs; i++)
//             {
//                 FUNCDESC* pfd;
//                 if (FAILED(ptrTypeInfo->GetFuncDesc(i, &pfd)) || (pfd->invkind != INVOKE_PROPERTYPUT))
//                     continue;
// 
//                 CComBSTR bstrName;
//                 if (SUCCEEDED(ptrTypeInfo->GetDocumentation(pfd->memid, &bstrName, NULL, NULL, NULL)))
//                 {
//                     strRet += bstrName;
//                     strRet += L"=\"";
//                     CComVariant var;
//                     DbgVerifyHResult(disp.GetPropertyByName(bstrName, &var));
//                     var.ChangeType(VT_BSTR);
//                     strRet += var.bstrVal;
//                     strRet += L"\"";
//                 }
//                 ptrTypeInfo->ReleaseFuncDesc(pfd);
//             }
//             ptrTypeInfo->ReleaseTypeAttr(pTypeAttr);
//         }
//     }
// 
//     strRet += L"></Property>";
//     return strRet;
// }

// TMBool CPropBoxOleProp_ShowProp(CString& member, CString& str)
// {
//     CTuiActiveXImpl* pThis = ((CTuiActiveXImpl*)(((char*)&member) - (offsetof(CTuiActiveXImpl, m_prop))));
//     if (!pThis->m_spUnknown)
//         pThis->CreateControl();
// 
// //     if (pThis->m_spOleObject)
// //     {
// //         HWND hWnd = pThis->GetHWND();
// //         RECT rc = {0, 0, 640, 480};
// //         HRESULT hr = pThis->m_spOleObject->DoVerb(OLEIVERB_PROPERTIES, NULL, pThis, 0, hWnd, &rc);
// //         if (SUCCEEDED(hr))
// //             return true;
// //     }
// //     
// // //     OLEIVERB_PROPERTIES
// //     CAUUID cauuid;
// //     CComQIPtr<ISpecifyPropertyPages> ptrISPP(pThis->m_spUnknown);
// //     if (ptrISPP && S_OK == ptrISPP->GetPages(&cauuid))
// //     {
// //         IUnknown* p = pThis->m_spUnknown.operator->();
// //         HRESULT hr = OleCreatePropertyFrame(pThis->GetHWND(), 0, 0, pThis->GetWindowText(), 1, &p, cauuid.cElems, (GUID *)cauuid.pElems, LOCALE_USER_DEFAULT, 0, 0);
// //         CoTaskMemFree(cauuid.pElems);
// //         if (SUCCEEDED(hr))
// //             return true;
// //     }
// 
// //     CComQIPtr<IPropertyPage> ptrPropPage(pThis->m_spUnknown);
// //     if (ptrPropPage)
// //     {
// //         ptrPropPage->Show(SW_SHOW);
// //         return true;
// //     }
// 
//     CComQIPtr<IPersistPropertyBag2> ptrP2(pThis->m_spUnknown);
//     if (ptrP2)
//     {
//         ptrP2->Save(pThis, true, true);
//         pThis->SaveProp();
//         return true;
//     }
// 
//     CComQIPtr<IPersistPropertyBag> ptrP(pThis->m_spUnknown);
//     if (ptrP)
//     {
//         ptrP->Save(pThis, true, true);
//         pThis->SaveProp();
//         return true;
//     }
// 
//     return false;
// }

// edit load:
// QueryInterface(IID_IOleObject)
// OleObject::SetClientSite(p);
// OleObject::GetMiscStatus(1, &x); return IMPLEMENT_OLECTLTYPE(CAaaaCtrl, IDS_AAAA, _dwAaaaOleMisc)
// QueryInterface{IID_IPersistPropertyBag}
// PersistPropertyBag::Load
// QueryInterface({IID_IViewObject2})
// QueryInterface({IID_IOleInPlaceObject})
// QueryInterface({IID_IDataObject})
// QueryInterface({IID_IRunnableObject})
// OleObject::GetExtent(1, &x) return m_cxExtent & m_cyExtent
// OleObject::DoVerb(OLEIVERB_INPLACEACTIVATE);
// OleInPlaceObject::SetObjectRects
// OleObject::SetExtent(1, x);
// OleInPlaceObject::UIDeactivate()

// load to fp
// {00000008-0000-0000-C000-000000000046}
// QueryInterface({CLSID_HTML Document})
// {IID_IOleControl}
// OleControl::FreezeEvents(1)
// {IID_IClientSecurity}
// {IID_IQuickActivate}
// {IID_IOleObject}
// OleObject::SetClientSite(p)
// OleObject::Advise()
// {3AF24292-0C96-11CE-A0CF-00AA00600AB8}
// OleObject::GetMiscStatus(1)
// {IID_IPersistPropertyBag2}
// {IID_IPersistPropertyBag}
// {IID_IObjectSafety}
// {IID_IOleObject}
// OleObject::SetExtent(1)
// OleObject::GetExtent(1)
// PersistPropertyBag::Load()
//   AaaaCtrl::DoPropExchange()
//   OleObject::SetExtent(1)
//   {IID_IDispatchEx}
//   {IID_IDispatch}
//   DispatchImpl::Invoke(-525,2)
// {3AF24292-0C96-11CE-A0CF-00AA00600AB8}
// {IID_IActiveScript}
// {6D5140D3-7436-11CE-8034-00AA006009FA}
// {IID_IOleControl}
// OleControl::FreezeEvents(1)

// load to IE
// not safe dialog
// QueryInterface({00000008-0000-0000-C000-000000000046})
// QueryInterface({CLSID_HTML Document})
// QueryInterface({IID_IOleControl}
// OleControl::FreezeEvents(true)
// QueryInterface({IID_IClientSecurity}
// {IID_IQuickActivate}
// {IID_IOleObject}
// OleObject::SetClientSite(p)
// OleObject::Advise(p)
// {3AF24292-0C96-11CE-A0CF-00AA00600AB8}
// OleObject::GetMiscStatus(1)
// {IID_IPersistPropertyBag2}
// {IID_IPersistPropertyBag}
// {IID_IObjectSafety}
// {IID_IOleObject}
// OleObject::SetExtent(1)
// OleObject::GetExtent(1)
// not safe dialog
// PersistPropertyBag::Load
//   OleObject::SetExtent(1)
//   {IID_IDispatchEx}
//   {IID_IDispatch}
//   OleDispatchImpl::Invoke(-525,2)
// {3AF24292-0C96-11CE-A0CF-00AA00600AB8}
// {IID_IActiveScript}
// {6D5140D3-7436-11CE-8034-00AA006009FA}
// {IID_IOleControl}
// OleControl::FreezeEvents(true)
// {IID_IOleCommandTarget}
// OleDispatchImpl::Invoke(-525,2)
// {IID_IOleControl}
// OleControl::FreezeEvents(false)
// OleDispatchImpl::Invoke(-525,2)
// {IID_IOleControl}
// OleControl::FreezeEvents(false)
// {00000008-0000-0000-C000-000000000046}
// {00000008-0000-0000-C000-000000000046}
// {3050F432-98B5-11CF-BB82-00AA00BDCE0B}
// {IID_IClientSecurity}
// ?
// {IID_IPersistHistory}
// {IID_IOleCommandTarget}
// {IID_IOleObject}
// OleObject::SetClientSite(0)
// OleDispatchImpl::Disconnect()
// {00000008-0000-0000-C000-000000000046}
// QueryInterface({CLSID_HTML Document})
// {IID_IOleControl}
// OleControl::FreezeEvents(1)
// {IID_IClientSecurity}
// {IID_IQuickActivate}
// {IID_IOleObject}
// OleObject::SetClientSite(p)
// OleObject::Advise(p)
// {3AF24292-0C96-11CE-A0CF-00AA00600AB8}
// OleObject::GetMiscStatus(1)
// {IID_IPersistHistory}
// {IID_IPersistPropertyBag2}
// {IID_IPersistPropertyBag}
// {IID_IObjectSafety}
// {IID_IOleObject}
// OleObject::SetExtent(1)
// OleObject::GetExtent(1)

// close:
// OleInPlaceObject::InPlaceDeactivate()
// DoVerb(OLEIVERB_HIDE);
// OleInPlaceObject::UIDeactivate()
// OleObject::SetClientSite(0)
// QueryInterface({IID_IMarshal})
// 

// vs2008 edit
// {IID_IDispatch}
// OleDispatchImpl::GetTypeInfo
// {IID_IDispatch}
// {IID_IPerPropertyBrowsing}
// {IID_IConnectionPointContainer}
// OleObject::GetMiscStatus
// OleObject::SetClientSite
// {IID_IPersistStorage}
// {IID_IPersistStreamInit}
// PersistStorage::InitNew
//    CAaaaCtrl::DoPropExchange(
// {IID_IRunnableObject}
// {1AB42240-8C70-11CE-9421-00AA0062BE57}
// {IID_DataSource}
// {IID_IOleCache}
// {IID_IOleObject}
// {IID_IOleLink}
// OleObject::GetMiscStatus(1)
// {IID_IViewObject2}
// OleObject::Advise
// OleObject::SetHostNames
// {IID_IRunnableObject}
// {IID_ISpecifyPropertyPages}
// {IID_IDataObject}
// OleObject::GetExtent(1)
// OleObject::SetExtent(1)
// {IID_IDispatch}
// OleDispatchImpl::Invoke(-518)
// {IID_IDispatch}


// OleObject::GetUserClassID
// {IID_IPersistStreamInit} 
//   CAaaaCtrl::DoPropExchange
// {IID_IVsGetCfgProvider}
// SpecifyPropertyPages::GetPages
// PerPropertyBrowsing::MapPropertyToPage
// PerPropertyBrowsing::GetPredefinedStrings


TM_END_NAMESPACE

