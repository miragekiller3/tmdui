/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.02.02
 *
 *  Copyright (C) 2006 miragekiller
 */

#pragma once

#ifndef UIACTIVEX_H
#define UIACTIVEX_H

#include <atlhost.h>

#include "uiwidget.h"
#include "../tmcom.h"
#include "../tmhook.h"
#include "uidummy.h"
#include "view.h"

//#include "tmformatmsg.h"

#define AXTRACE DbgTrace
//#define AXTRACE DbgTraceNull

#define AXTRACENOTIMPL(funcname) AXTRACE(#funcname); return E_NOTIMPL

// class CPropBoxOleProp;

TM_BGN_NAMESPACE



// class TM_NO_VTABLE IOleInPlaceFrameImpl
//     : public IOleInPlaceFrame
// {
// public:
//     STDMETHOD(GetWindow)(TMHWnd* phwnd)
//     {
//         AXTRACE("CTuiActiveXImpl::IOleInPlaceFrameImpl::GetWindow");
//         return E_NOTIMPL;
//     }
// 
//     STDMETHOD(ContextSensitiveHelp)(TMBool /*fEnterMode*/)
//     {
//         AXTRACE("CTuiActiveXImpl::IOleInPlaceFrameImpl::ContextSensitiveHelp");
//         return S_OK;
//     }
// 
// // IOleInPlaceUIWindow
//     STDMETHOD(GetBorder)(TMRect* /*lprectBorder*/)
//     {
//         AXTRACE("CTuiActiveXImpl::IOleInPlaceFrameImpl::GetBorder");
//         return S_OK;
//     }
// 
//     STDMETHOD(RequestBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
//     {
//         AXTRACE("CTuiActiveXImpl::IOleInPlaceFrameImpl::RequestBorderSpace");
//         return INPLACE_E_NOTOOLSPACE;
//     }
// 
//     STDMETHOD(SetBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
//     {
//         AXTRACE("CTuiActiveXImpl::IOleInPlaceFrameImpl::SetBorderSpace");
//         return S_OK;
//     }
// 
//     STDMETHOD(SetActiveObject)(IOleInPlaceActiveObject* pActiveObject, LPCOLESTR /*pszObjName*/)
//     {
//         AXTRACE("CTuiActiveXImpl::IOleInPlaceFrameImpl::SetActiveObject");
//         return S_OK;    
//     }
// 
// // IOleInPlaceFrameWindow
//     STDMETHOD(InsertMenus)(TMHMenu /*hmenuShared*/, LPOLEMENUGROUPWIDTHS /*lpMenuWidths*/)
//     {
//         AXTRACE("CTuiActiveXImpl::IOleInPlaceFrameImpl::InsertMenus");
//         return S_OK;
//     }
// 
//     STDMETHOD(SetMenu)(TMHMenu /*hmenuShared*/, HOLEMENU /*holemenu*/, TMHWnd /*hwndActiveObject*/)
//     {
//         AXTRACE("CTuiActiveXImpl::IOleInPlaceFrameImpl::SetMenu");
//         return S_OK;
//     }
// 
//     STDMETHOD(RemoveMenus)(TMHMenu /*hmenuShared*/)
//     {
//         AXTRACE("CTuiActiveXImpl::IOleInPlaceFrameImpl::RemoveMenus");
//         return S_OK;
//     }
// 
//     STDMETHOD(SetStatusText)(LPCOLESTR /*pszStatusText*/)
//     {
//         AXTRACE("CTuiActiveXImpl::IOleInPlaceFrameImpl::SetStatusText");
//         return S_OK;
//     }
// 
//     STDMETHOD(EnableModeless)(TMBool /*fEnable*/)
//     {
//         AXTRACE("CTuiActiveXImpl::IOleInPlaceFrameImpl::EnableModeless");
//         return S_OK;
//     }
// 
//     STDMETHOD(TranslateAccelerator)(LPMSG /*lpMsg*/, WORD /*wID*/)
//     {
//         AXTRACE("CTuiActiveXImpl::IOleInPlaceFrameImpl::TranslateAccelerator");
//         return S_FALSE;
//     }
// };


// class IPropertyBagImpl_Xml : public IPropertyBag , public IPropertyBag2
// {
// public:
//     CDomDocument m_doc;
//     CDomElementPtr m_ptrXml;
// 
//     TM_UNUSED_ADDREF_RELEASE();
//     TM_BGN_INTERFACE_MAP        (IPropertyBag)
//         TM_INTERFACE_HANDLER    (IPropertyBag)
//         TM_INTERFACE_HANDLER    (IPropertyBag2)
//     TM_END_INTERFACE_MAP        ()
// 
//     IPropertyBagImpl_Xml(const CString& str)
//     {
//         m_doc->LoadXml(str);
//         m_ptrXml = m_doc->get_firstChild();
//     }
// 
//     IPropertyBagImpl_Xml()
//     {
//         m_ptrXml = m_doc->createElement(L"Property");
//     }
//     
//     // IUnknown methods
//     
//     // IPropertyBag methods
//     STDMETHOD(Read)(LPCOLESTR pszPropName, VARIANT *pVar, IErrorLog *pErrorLog)
//     {
//         CDomString str;
//         if (!m_ptrXml->getAttribute(pszPropName, str))
//             return E_INVALIDARG;
//         int nvt = pVar->vt;
//         CComVariant* p = (CComVariant*)pVar;
//         *p = str.c_str();
//         p->ChangeType(nvt);
//         return S_OK;
//     }
//     
//     STDMETHOD(Write)(LPCOLESTR pszPropName, VARIANT *pVar)
//     {
//         CComVariant var(*pVar);
//         var.ChangeType(VT_BSTR);
//         m_ptrXml->setAttribute(pszPropName, var.bstrVal);
//         return S_OK;
//     }
//     
//     // IPropertyBag2 methods
//     STDMETHOD(Read)(
//         ULONG cProperties, 
//         PROPBAG2 *pPB2, 
//         IErrorLog *pErrorLog,
//         VARIANT *pvarValue,
//         HRESULT *phrError)
//     {
//         for (int i = 0; i < cProperties; ++i)
//         {
//             HRESULT hr = Read(pPB2[i].pstrName, pvarValue + i, pErrorLog);
//             if (SUCCEEDED(hr))
//                 hr = ((CComVariant&)(pvarValue[i])).ChangeType(pPB2[i].vt);
//             if (phrError)
//                 phrError[i] = hr;
//         }
//         return S_OK;
//     }
//     
//     STDMETHOD(Write)(ULONG cProperties, PROPBAG2 *pPB2, VARIANT *pVar)
//     {
//         for (int i = 0; i < cProperties; ++i)
//             Write(pPB2[i].pstrName, pVar + i);
//         return S_OK;
//     }
//     
//     STDMETHOD(CountProperties)(ULONG *pcProperties)
//     {
//         return m_ptrXml->get_attributes()->get_length();
//     }
//     
//     STDMETHOD(GetPropertyInfo)(
//         ULONG iProperty,
//         ULONG cProperties,
//         PROPBAG2 *pPropBag,
//         ULONG *pcProperties)
//     {
//         CDomAttributeListPtr aAttrib(m_ptrXml->get_attributes());
//         int nLen = aAttrib->get_length();
//         if (iProperty >= nLen)
//             return E_INVALIDARG;
//         
//         for (int i = 0; i < *pcProperties; ++i)
//         {
//             pPropBag->dwType = PROPBAG2_TYPE_DATA;
//             pPropBag->vt = VT_BSTR;
//             pPropBag->cfType = CF_TEXT;
//             pPropBag->pstrName = ::SysAllocString(aAttrib->item(i + iProperty)->get_nodeName().c_str());
//             pPropBag->dwHint = i + iProperty;
//         }
//         return S_OK;
//     }
//     
//     STDMETHOD(LoadObject)(
//         LPCOLESTR pstrName,
//         DWORD dwHint,
//         IUnknown *pUnkObject,
//         IErrorLog *pErrorLog)
//     {
//         return E_NOTIMPL;
//     }
// };
// 


class TM_NO_VTABLE CActiveXCont
    : public CDummyChild
    , public IOleClientSite
    , public IOleInPlaceSiteWindowless
    , public IOleControlSite
    , public IOleContainer
    , public IObjectWithSiteImpl<CActiveXCont>
    , public IServiceProvider
    , public IAdviseSink
//     , public C3rdObjectBaseT<CActiveXCont>
{
protected:
    CComPtr<IUnknown> m_spExtUnknown;
    CComPtr<IUnknown> m_spUnknown;
    CComQIPtr<IOleObject> m_spOleObject;
    //     CComPtr<IOleInPlaceFrame> m_spInPlaceFrame;
    //     CComPtr<IOleInPlaceUIWindow> m_spInPlaceUIWindow;
    CComPtr<IViewObjectEx> m_spViewObject;
//     CComPtr<IOleInPlaceObjectWindowless> m_spInPlaceObjectWindowless;
    CComPtr<IServiceProvider>  m_spServices;
    
    DWORD m_dwViewObjectType;
    
    // state
    unsigned long m_bInPlaceActive:1;
    unsigned long m_bUIActive:1;
    unsigned long m_bInReleaseAll:1;
    unsigned long m_bShow:1;
    
    DWORD m_dwOleObject; // for IOleObject::Advise, Unadvise
    DWORD m_dwMiscStatus;
    CRect m_rcPos;
    
public:
    TM_BGN_INTERFACE_MAP        (IOleClientSite)
        TM_INTERFACE_MAP_MEMBER (m_spExtUnknown)
        TM_INTERFACE_HANDLER    (IOleClientSite)
        TM_INTERFACE_HANDLER    (IOleInPlaceSiteWindowless)
        TM_INTERFACE_HANDLER    (IOleInPlaceSiteEx)
        TM_INTERFACE_HANDLER    (IOleInPlaceSite)
        TM_INTERFACE_HANDLER    (IOleWindow)
        TM_INTERFACE_HANDLER    (IOleControlSite)
        TM_INTERFACE_HANDLER    (IOleContainer)
        TM_INTERFACE_HANDLER    (IObjectWithSite)
        TM_INTERFACE_HANDLER    (IAdviseSink)
//        TM_INTERFACE_HANDLER    (IServiceProvider)
//        TM_INTERFACE_HANDLER    (IOleInPlaceUIWindow)
//        TM_INTERFACE_HANDLER    (IOleInPlaceFrame)
    TM_END_INTERFACE_MAP        ()

    using IOleClientSite::AddRef;
    using IOleClientSite::Release;

    CActiveXCont();
    TMBool AttachWidget(CTuiDummyBase* hWnd, const CLSID& iid);
    IUnknown* GetControllingUnknown() { return GetUnknown(); }
    void SyncWindowLess();
    void ReleaseAll();
    TMResult BindObject(IUnknown* pUnkControl);
    void UpdatePixelPaddingRect();
    void DoVerb(TMLong lVerb, LPCRECT lprcPosRect = 0);
    TMBool IsCapture() { return m_pWnd->IsCapture(); };
    void StopNoRedirect();
    
    // for IDummy
    void DoSyncSetScreenPos(TMUInt32 flag);

    // IDummy
    virtual TMBool SyncIsValid() tm_override { return m_spUnknown != 0; }
    virtual void DestroyDummy(EUnbindDummy eReason) tm_override;
    virtual TMBool SyncIsTargetVisible() tm_override;// { return m_bShow; }
    virtual TMUInt32 SyncGetScreenPos(HTUI hWgt, CRectI& rc) tm_override;
    virtual void SyncSetTargetScreenPos(CRect* rcSreen, TMUInt32 flag) tm_override;// { DoSyncSetScreenPos(flag); }
    virtual TMBool ProcessWindowMessage(HTUI hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID = 0) tm_override;

    // CHookCallWnd
    virtual TMBool OnCallWndProc(CWPSTRUCT* ps) tm_override;

    // CHookGetMessageEx
    virtual TMBool PreTranslateMessage(MSG* msg) tm_override;

    // IObjectWithSite
// IObjectWithSiteImpl<CTuiActiveXHost> will do it
    STDMETHOD(SetSite)(IUnknown* pUnkSite) tm_override
    {
        AXTRACE(_T("CActiveXCont::IObjectWithSite::SetSite\r\n"));

        typedef IObjectWithSiteImpl<CActiveXCont> base_site;
        HRESULT hr = base_site::SetSite(pUnkSite);
        
        if (SUCCEEDED(hr) && m_spUnkSite)
        {
            // Look for "outer" IServiceProvider
            hr = m_spUnkSite->QueryInterface(__uuidof(IServiceProvider), (void**)&m_spServices);
            DbgAssert(SUCCEEDED(hr) && "No ServiceProvider!");
        }
        
        if (pUnkSite == NULL)
            m_spServices.Release();
        
        return hr;
    }

// IOleClientSite
    STDMETHOD(SaveObject)() tm_override
    {
        AXTRACENOTIMPL(_T("CActiveXCont::IOleClientSite::SaveObject"));
    }
    STDMETHOD(GetMoniker)(TMULong /*dwAssign*/, TMULong /*dwWhichMoniker*/, IMoniker** /*ppmk*/) tm_override
    {
        AXTRACENOTIMPL(_T("CActiveXCont::IOleClientSite::GetMoniker"));
    }
    STDMETHOD(GetContainer)(IOleContainer** ppContainer) tm_override
    {
        AXTRACE(_T("CActiveXCont::IOleClientSite::GetContainer\n"));
        TMResult hr = E_POINTER;
        if (ppContainer)
        {
            hr = E_NOTIMPL;
            (*ppContainer) = NULL;
            if (m_spUnkSite)
                hr = m_spUnkSite->QueryInterface(IID_IOleContainer, (void**)ppContainer);
            if (FAILED(hr))
                hr = QueryInterface(IID_IOleContainer, (void**)ppContainer);
        }
        return hr;
    }
    STDMETHOD(ShowObject)() tm_override
    {
        AXTRACE(_T("CActiveXCont::IOleClientSite::ShowObject\r\n"));
        m_pWnd->Invalidate();
        return S_OK;
    }
    STDMETHOD(OnShowWindow)(TMBool /*fShow*/) tm_override
    {
        AXTRACENOTIMPL(_T("CActiveXCont::IOleClientSite::OnShowWindow"));
    }
    STDMETHOD(RequestNewObjectLayout)() tm_override
    {
        AXTRACENOTIMPL(_T("CActiveXCont::IOleClientSite::RequestNewObjectLayout"));
    }

// IOleInPlaceSite
    STDMETHOD(GetWindow)(TMHWnd* phwnd) tm_override
    {
        AXTRACE("CActiveXCont::IOleInPlaceSite::GetWindow");
        *phwnd = m_pWnd->GetHWND();
        return S_OK;
    }
    STDMETHOD(ContextSensitiveHelp)(TMBool /*fEnterMode*/) tm_override
    {
        AXTRACENOTIMPL(_T("CActiveXCont::IOleWindow::ContextSensitiveHelp"));
    }
    STDMETHOD(CanInPlaceActivate)() tm_override
    {
        AXTRACE ("CActiveXCont::IOleInPlaceSite::CanInPlaceActivate");
        return S_OK;
    }
    STDMETHOD(OnInPlaceActivate)() tm_override
    {
        AXTRACE("CActiveXCont::IOleInPlaceSite::OnInPlaceActivate");
        DbgAssert(m_bInPlaceActive == FALSE);
//         DbgAssert(m_spInPlaceObjectWindowless == NULL);

        m_bInPlaceActive = TMTrue;
        OleLockRunning(m_spOleObject, TMTrue, TMFalse);
        m_pWnd->SetDirtyAndInvalidate();
//         SetWindowLess(false);
//         m_spOleObject->QueryInterface(IID_IOleInPlaceObject, (void**)&m_spInPlaceObjectWindowless);
        return S_OK;
    }
    STDMETHOD(OnUIActivate)() tm_override
    {
        AXTRACE("CActiveXCont::IOleInPlaceSite::OnUIActivate");
        m_bUIActive = TMTrue;
        return S_OK;
    }
    STDMETHOD(GetWindowContext)(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, TMRect* lprcPosRect, TMRect* lprcClipRect, LPOLEINPLACEFRAMEINFO pFrameInfo) tm_override
    {
        AXTRACE("CActiveXCont::IOleInPlaceSite::GetWindowContext");
        return E_NOTIMPL;
// 
//         if (ppFrame != NULL)
//             *ppFrame = NULL;
//         if (ppDoc != NULL)
//             *ppDoc = NULL;
//         if (ppFrame == NULL || ppDoc == NULL || lprcPosRect == NULL || lprcClipRect == NULL)
//         {
//             DbgAssert(false);
//             return E_POINTER;
//         }
//         
//         if (!m_spInPlaceFrame)
//         {
//             CComObject<CAxFrameWindow>* pFrameWindow;
//             HRESULT hRet = CComObject<CAxFrameWindow>::CreateInstance(&pFrameWindow);
//             
//             if (FAILED(hRet))
//             {
//                 return hRet;
//             }
//             
//             pFrameWindow->QueryInterface(__uuidof(IOleInPlaceFrame), (void**) &m_spInPlaceFrame);
//             DbgAssert(m_spInPlaceFrame);
//         }
//         if (!m_spInPlaceUIWindow)
//         {
//             CComObject<CAxUIWindow>* pUIWindow;
//             HRESULT hRet = CComObject<CAxUIWindow>::CreateInstance(&pUIWindow);
//             
//             if (FAILED(hRet))
//             {
//                 return hRet;
//             }        
//             
//             pUIWindow->QueryInterface(__uuidof(IOleInPlaceUIWindow), (void**) &m_spInPlaceUIWindow);
//             DbgAssert(m_spInPlaceUIWindow);
//         }
//         HRESULT hr=S_OK;
//         hr=m_spInPlaceFrame.CopyTo(ppFrame);
//         if (FAILED(hr))
//         {
//             return hr;
//         }
//         hr=m_spInPlaceUIWindow.CopyTo(ppDoc);
//         if (FAILED(hr))
//         {
//             return hr;
//         }
// 
//         {
//             CTuiRect rc;
//             GetPaddingRect(&rc);
//             rc.CopyTo(*lprcPosRect);
//             rc.CopyTo(*lprcClipRect);
//         }
//         
//         if (m_hAccel == NULL)
//         {
//             ACCEL ac = { 0,0,0 };
//             m_hAccel = CreateAcceleratorTable(&ac, 1);
//         }
//         pFrameInfo->cb = sizeof(OLEINPLACEFRAMEINFO);
//         pFrameInfo->fMDIApp = m_bMDIApp;
//         pFrameInfo->hwndFrame = GetHWND();
//         pFrameInfo->haccel = m_hAccel;
//         pFrameInfo->cAccelEntries = (m_hAccel != NULL) ? 1 : 0;
// 
//         return hr;
    }
    STDMETHOD(Scroll)(TMSize /*scrollExtant*/) tm_override
    {
        AXTRACENOTIMPL(_T("CActiveXCont::IOleInPlaceSite::Scroll"));
    }
    STDMETHOD(OnUIDeactivate)(TMBool fUndoable) tm_override
    {
        AXTRACE("CActiveXCont::IOleInPlaceSite::OnUIDeactivate(%d) %d", fUndoable, _GetRef());
        m_bUIActive = false;
        return S_OK;
    }
    STDMETHOD(OnInPlaceDeactivate)() tm_override
    {
        AXTRACE("CActiveXCont::IOleInPlaceSite::OnInPlaceDeactivate() %d", _GetRef());
        m_bInPlaceActive = false;
//         m_spInPlaceObjectWindowless.Release();
//         ::DestroyWindow(m_wnd);// m_pWnd->GetHWND());
        return S_OK;
    }
    STDMETHOD(DiscardUndoState)() tm_override
    {
        AXTRACENOTIMPL(_T("CActiveXCont::IOleInPlaceSite::DiscardUndoState"));
    }
    STDMETHOD(DeactivateAndUndo)() tm_override
    {
        AXTRACENOTIMPL(_T("CActiveXCont::IOleInPlaceSite::DeactivateAndUndo"));
    }
    STDMETHOD(OnPosRectChange)(const TMRect* /*lprcPosRect*/) tm_override
    {
        AXTRACENOTIMPL(_T("CActiveXCont::IOleInPlaceSite::OnPosRectChange"));
    }

//     STDMETHOD(OnPosRectChange)(_In_ LPCRECT lprcPosRect)
//     {
//         AXTRACE(   _T("IOleInPlaceSite::OnPosRectChange"));
//         if (lprcPosRect==NULL) { return E_POINTER; }
//         
//         // Use MoveWindow() to resize the CAxHostWindow.
//         // The CAxHostWindow handler for OnSize() will
//         // take care of calling IOleInPlaceObject::SetObjectRects().
//         
//         // Convert to parent window coordinates for MoveWindow().
//         RECT rect = *lprcPosRect;
//         ClientToScreen(&rect);
//         HWND hWnd = GetParent();
//         
//         // Check to make sure it's a non-top-level window.
//         if (hWnd != NULL)
//         {
//             CWindow wndParent(hWnd);
//             wndParent.ScreenToClient(&rect);
//             wndParent.Detach ();
//         }
//         // Do the actual move.
//         MoveWindow(&rect);
//         
//         return S_OK;
//     }

// IOleInPlaceSiteEx
    STDMETHOD(OnInPlaceActivateEx)(TMBool* /*pfNoRedraw*/, TMULong dwFlags) tm_override
    {
        AXTRACE("CActiveXCont::IOleInPlaceSiteEx::OnInPlaceActivateEx");
        m_bInPlaceActive = TMTrue;
        OleLockRunning(m_spOleObject, TMTrue, TMFalse);
        
//         TMResult hr = E_FAIL;
//         if (dwFlags & ACTIVATE_WINDOWLESS)
//             hr = m_spOleObject->QueryInterface(IID_IOleInPlaceObjectWindowless, (void**) &m_spInPlaceObjectWindowless);
//         bool bWindowless = SUCCEEDED(hr);
//         if (!bWindowless)
//             hr = m_spOleObject->QueryInterface(IID_IOleInPlaceObject, (void**) &m_spInPlaceObjectWindowless);

//         if (m_spInPlaceObjectWindowless)
//         {
//             m_spInPlaceObjectWindowless->SetObjectRects(&m_rcPos, &m_rcPos);
//         }
//         SetWindowLess(dwFlags & ACTIVATE_WINDOWLESS);
        m_pWnd->SetDirtyAndInvalidate();
        return S_OK;
    }
    STDMETHOD(OnInPlaceDeactivateEx)(TMBool /*fNoRedraw*/) tm_override
    {
        AXTRACE("CActiveXCont::IOleInPlaceSiteEx::OnInPlaceDeactivateEx");
        m_bInPlaceActive = FALSE;
//         m_spInPlaceObjectWindowless.Release();
        return S_OK;
    }
    STDMETHOD(RequestUIActivate)() tm_override
    {
        AXTRACE("CActiveXCont::IOleInPlaceSiteEx::RequestUIActivate");
        return S_OK;
    }

// IOleInPlaceSiteWindowless
    STDMETHOD(CanWindowlessActivate)() tm_override
    {
        AXTRACE("CActiveXCont::IOleInPlaceSiteWindowless::CanWindowlessActivate");
        return S_OK;//m_bCanWindowlessActivate ? S_OK : S_FALSE;
    }
    STDMETHOD(GetCapture)() tm_override
    {
        AXTRACE("CActiveXCont::IOleInPlaceSiteWindowless::GetCapture");
        return IsCapture() ? S_OK : S_FALSE;
    }
    STDMETHOD(SetCapture)(TMBool fCapture) tm_override
    {
        AXTRACE("CActiveXCont::IOleInPlaceSiteWindowless::SetCapture");
        if (fCapture)
        {
            m_pWnd->SetCapture();
//             m_bCapture = TMTrue;
        }
        else if (IsCapture())
        {
            m_pWnd->GetView()->ReleaseCapture();
//             m_bCapture = TMFalse;
        }
        return S_OK;
    }
    
    STDMETHOD(GetFocus)() tm_override
    {
        AXTRACE("CActiveXCont::IOleInPlaceSiteWindowless::GetFocus");
        return m_pWnd->IsFocus() ? S_OK : S_FALSE;
    }

    STDMETHOD(SetFocus)(TMBool fFocus) tm_override
    {
        AXTRACE("CActiveXCont::IOleInPlaceSiteWindowless::SetFocus");
        m_pWnd->GetView()->SetFocus(fFocus ? m_pWnd : 0, eFocusReasonUnknown);
        return S_OK;
    }

    STDMETHOD(GetDC)(const TMRect* /*pRect*/, TMULong /*grfFlags*/, TMHDC* phDC) tm_override
    {
        AXTRACE("CActiveXCont::IOleInPlaceSiteWindowless::GetDC");
//         if (phDC)
//         {
//             *phDC = ::GetWindowDC(0);// CTuiWgt::GetDC(true);
//             return S_OK;
//         }

        return E_POINTER;
    }
    STDMETHOD(ReleaseDC)(TMHDC hDC) tm_override
    {
       AXTRACE("CActiveXCont::IOleInPlaceSiteWindowless::ReleaseDC");
//         ::ReleaseDC(0, hDC);
//         CTuiWgt::ReleaseDC(hDC);
        return S_OK;
    }
    STDMETHOD(InvalidateRect)(const TMRect* pRect, TMBool fErase) tm_override
    {
        AXTRACE("CActiveXCont::IOleInPlaceSiteWindowless::InvalidateRect");
        m_pWnd->Invalidate();
//         SetDirty();
        return S_OK;
    }
    STDMETHOD(InvalidateRgn)(HRGN hRGN, TMBool fErase) tm_override
    {
        AXTRACE("CActiveXCont::IOleInPlaceSiteWindowless::InvalidateRgn");
        m_pWnd->Invalidate();
        //CWindowImpl<CTuiActiveXHost>::InvalidateRgn(hRGN, fErase);
        return S_OK;
    }
    STDMETHOD(ScrollRect)(INT /*dx*/, INT /*dy*/, const TMRect* /*pRectScroll*/, const TMRect* /*pRectClip*/) tm_override
    {
        AXTRACE("CActiveXCont::IOleInPlaceSiteWindowless::ScrollRect");
        return S_OK;
    }
    STDMETHOD(AdjustRect)(TMRect* /*prc*/) tm_override
    {
        AXTRACE("CActiveXCont::IOleInPlaceSiteWindowless::AdjustRect");
        return S_OK;
    }
    STDMETHOD(OnDefWindowMessage)(TMUInt msg, TMWParam wParam, TMLParam lParam, TMResult* plResult) tm_override
    {
        AXTRACE("CActiveXCont::IOleInPlaceSiteWindowless::OnDefWindowMessage");
        *plResult = 0;//SendMessage (msg, wParam, lParam);
        return S_OK;
    }

// IOleControlSite
    STDMETHOD(OnControlInfoChanged)() tm_override
    {
        AXTRACE("CActiveXCont::IOleControlSite::OnControlInfoChanged");
        return S_OK;
    }
    STDMETHOD(LockInPlaceActive)(TMBool /*fLock*/) tm_override
    {
        AXTRACE("CActiveXCont::IOleControlSite::LockInPlaceActive");
        return S_OK;
    }
    STDMETHOD(GetExtendedControl)(IDispatch** ppDisp) tm_override
    {
        AXTRACE("CActiveXCont::IOleControlSite::GetExtendedControl");
        if (ppDisp == NULL)
            return E_POINTER;
        return m_spOleObject.QueryInterface(ppDisp);
    }
    STDMETHOD(TransformCoords)(POINTL* /*pPtlHimetric*/, POINTF* /*pPtfContainer*/, TMULong /*dwFlags*/) tm_override
    {
        AXTRACE("CActiveXCont::IOleControlSite::TransformCoords");
        return S_OK;
    }
    STDMETHOD(TranslateAccelerator)(LPMSG /*lpMsg*/, TMULong /*grfModifiers*/) tm_override
    {
        AXTRACE("CActiveXCont::IOleControlSite::TranslateAccelerator");
        return S_FALSE;
    }
    STDMETHOD(OnFocus)(TMBool fGotFocus) tm_override
    {
        AXTRACE("CActiveXCont::IOleControlSite::OnFocus(%d) %d", fGotFocus, _GetRef());
//         m_bHaveFocus = (0 != fGotFocus);
        return S_OK;
    }
    STDMETHOD(ShowPropertyFrame)() tm_override
    {
        AXTRACE("CActiveXCont::IOleControlSite::ShowPropertyFrame");
        return E_NOTIMPL;
    }

// IAdviseSink
    STDMETHOD_(void, OnDataChange)(FORMATETC* /*pFormatetc*/, STGMEDIUM* /*pStgmed*/) tm_override
    {
        AXTRACE("CActiveXCont::IAdviseSink::OnDataChange");
    }
    STDMETHOD_(void, OnViewChange)(TMULong /*dwAspect*/, TMLong /*lindex*/) tm_override
    {
        AXTRACE("CActiveXCont::IAdviseSink::OnViewChange");
    }
    STDMETHOD_(void, OnRename)(IMoniker* /*pmk*/) tm_override
    {
        AXTRACE("CActiveXCont::IAdviseSink::OnRename");
    }
    STDMETHOD_(void, OnSave)() tm_override
    {
        AXTRACE("CActiveXCont::CTuiActiveXImpl::IAdviseSink::OnSave");
    }
    STDMETHOD_(void, OnClose)() tm_override
    {
        AXTRACE("CActiveXCont::IAdviseSink::OnClose");
    }

// IOleContainer
    STDMETHOD(ParseDisplayName)(IBindCtx* /*pbc*/, LPOLESTR /*pszDisplayName*/, ULONG* /*pchEaten*/, IMoniker** /*ppmkOut*/) tm_override
    {
        AXTRACE("CActiveXCont::IOleContainer::ParseDisplayName");
        return E_NOTIMPL;
    }
    STDMETHOD(EnumObjects)(TMULong /*grfFlags*/, IEnumUnknown** ppenum) tm_override
    {
        AXTRACE("CActiveXCont::IOleContainer::EnumObjects");
        return E_NOTIMPL;
//         if (ppenum == NULL)
//             return E_POINTER;
//         *ppenum = NULL;
//         typedef CComObject<CComEnum<IEnumUnknown, &IID_IEnumUnknown, IUnknown*, _CopyInterface<IUnknown> > > enumunk;
//         enumunk* p = NULL;
//         ATLTRY(p = new enumunk);
//         if (p == NULL)
//             return E_OUTOFMEMORY;
//         TMResult hRes = p->Init(reinterpret_cast<IUnknown**>(&m_spUnknown), reinterpret_cast<IUnknown**>(&m_spOleObject), GetUnknown(), AtlFlagCopy);
//         if (SUCCEEDED(hRes))
//             hRes = p->QueryInterface(IID_IEnumUnknown, (void**)ppenum);
//         if (FAILED(hRes))
//             delete p;
//         return hRes;
    }
    STDMETHOD(LockContainer)(TMBool fLock) tm_override
    {
        AXTRACE("CActiveXCont::IOleContainer::LockContainer");
//         m_bLocked = fLock;
        return E_NOTIMPL;
    }

    STDMETHOD(QueryService)(REFGUID rsid, REFIID riid, void** ppvObj) tm_override
    {
        AXTRACE("CActiveXCont::IServiceProvider::QueryService");
//         TMResult hr = E_NOINTERFACE;
//         // Try for service on this object
// 
//         // No services currently
// 
//         if (m_spUnkSite)
//         {
//             // Look for "outer" IServiceProvider
//             CComPtr<IServiceProvider> spServices;
//             TMResult hr = m_spUnkSite->QueryInterface(IID_IServiceProvider, (void**)&spServices);
//             if (SUCCEEDED(hr))
//                 hr = spServices->QueryService(rsid, riid, ppvObj);
//         }

        DbgAssert(ppvObj != NULL);
        if (ppvObj == NULL)
            return E_POINTER;
        *ppvObj = NULL;
        
        HRESULT hr = E_NOINTERFACE;
        // Try for service on this object
        
        // No services currently
        
        // If that failed try to find the service on the outer object
        if (m_spServices)
        {
            hr = m_spServices->QueryService(rsid, riid, ppvObj);
        }
        
        return hr;
    }
    
};


///////////////////////////////////////////////////////////////////////////////
// CTuiActiveXImpl
class TM_NO_VTABLE CTuiActiveXImpl
    : public CTuiDelayDummy
{
protected:
    CString m_strIID;

public:
    BGN_OBJ_PROP_MAP            ()
        OBJ_PROP_OLE            ("Object", m_strIID)
        CHAIN_OBJ_PROP          (CTuiDelayDummy)
    END_OBJ_PROP_MAP            ()

//     BEGIN_TUIMSG_MAP            (CTuiActiveXImpl)
//         MESSAGE_HANDLER         (WM_NCCREATE,                   OnCreate)
//         CHAIN_MSG_MAP           (CTuiDummyBase);
//     END_TUIMSG_MAP()
// 
//     TMResult OnCreate (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

public:
    CTuiActiveXImpl();
    virtual void Recreate() tm_override;
};



TM_END_NAMESPACE



#endif // UIACTIVEX_H