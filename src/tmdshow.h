/**
 *    @file
 *    @brief
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2018-07-28
 *
 *    Copyright (C) 2018 miragekiller                               
 */

#pragma once

#ifndef TMDSHOW_H
#define TMDSHOW_H

// #include <qedit.h>
#include "gxd3d9.h"
#include "tmcom_win.h"
#include "tmformatguid.h"
#include "tmthread.h"
#include "tmani.h"

#define _INC_WINDOWSX // don't include <windowsx.h>
#include <dshow.h>
#define __dxtrans_h__
#define __IDxtCompositor_INTERFACE_DEFINED__
#define __IDxtAlphaSetter_INTERFACE_DEFINED__
#define __IDxtJpeg_INTERFACE_DEFINED__
#define __IDxtKey_INTERFACE_DEFINED__
#include <dvdmedia.h> // for VIDEOINFOHEADER2
#include <vmr9.h>


// #pragma comment(lib, "strmbasd.lib ")
// #pragma comment(lib, "winmm.lib ")
#pragma comment(lib, "strmiids.lib ")


TM_BGN_NAMESPACE


#define dshow_trace         DbgTrace
#define dshow_trace_method  DbgTraceMethod
// #define dshow_trace DbgTraceNull
// #define dshow_trace_method DbgTraceNull


#define DS_FILTER_NAME L"TM Video Renderer"

struct _DebugPin2Str
{
    CString done(IPin* p) const
    {
        if (!p)
            return L"";

        PIN_INFO info = {0};
        p->QueryPinInfo(&info);
        FILTER_INFO fInfo = {0};
        info.pFilter->QueryFilterInfo(&fInfo);
        CStringW str;
        str.Format(L"%p(%s::%s)", p, fInfo.achName, info.achName);
        return str;
    }
};

#define DS_DEBUG_PIN2STR(x) _DebugPin2Str().done(x).c_str()


inline static void dsGetFirstPin(IBaseFilter* pBF, CComPtr<IPin>& ptrPin)
{
    CComPtr<IEnumPins> ptrEnumPin;
    pBF->EnumPins(&ptrEnumPin);
    ptrEnumPin->Next(1, &ptrPin, 0);
}

inline static AM_MEDIA_TYPE* dsCreateMediaType()
{
    AM_MEDIA_TYPE *pMediaType =
        (AM_MEDIA_TYPE *)CoTaskMemAlloc(sizeof(AM_MEDIA_TYPE));
    return pMediaType;
}

inline static void dsFreeMediaType(AM_MEDIA_TYPE& mt)
{
    CoTaskMemFree(mt.pbFormat);
    if (mt.pUnk)
        mt.pUnk->Release();
}

inline static void dsDeleteMediaType(AM_MEDIA_TYPE *pmt)
{
    DbgAssert(pmt);
    dsFreeMediaType(*pmt);
    CoTaskMemFree(pmt);
}

class CRenderBase
{
public:
    typedef CAutoLockT<CCritical>                       CAutoLock;

    CCritical               m_critical;
    CComPtr<IBaseFilter>    m_pBaseFilter;

public:
//     void UninitRenderer()
//     {
//         dshow_trace_method("CRenderBase::UninitRenderer");
//         m_pBaseFilter.Release();
//     }
};



///////////////////////////////////////////////////////////////////////////
// CPinProxy


class CPinProxy
    : public IPin
    , public IMemInputPin
{
public:
    CComPtr<IPin> m_pPin;
    CComQIPtr<IMemInputPin> m_pMemInputPin;

    TM_UNUSED_ADDREF_RELEASE();
    TM_BGN_INTERFACE_MAP(IPin)
        dshow_trace(TM_T("CPinProxy::QueryInterface:%s"),  DbgFormatGuid(iid).c_str());
        if (m_pMemInputPin) { TM_INTERFACE_HANDLER(IMemInputPin) }
        TM_INTERFACE_HANDLER(IPin)
        TM_INTERFACE_MAP_MEMBER(m_pPin)
    TM_END_INTERFACE_MAP()
    
    //struct CEnumMT : public CEnumImplNBaseT<CEnumMT, AM_MEDIA_TYPE*, IEnumMediaTypes>
    //{
    //    HRESULT CopyTo(int nPos, AM_MEDIA_TYPE*& val)
    //    {
    //        dshow_trace("CEnumMT::CopyTo:%d", nPos);
    //        if (nPos > 2)
    //            return E_FAIL;
    //        AM_MEDIA_TYPE* p = dsCreateMediaType();
    //        MemZeroT(*p);
    //        p->majortype = MEDIATYPE_Video;
    //        p->formattype = FORMAT_VideoInfo;
    //        if (nPos == 0)
    //            p->subtype = MEDIASUBTYPE_RGB24;
    //        else if (nPos == 1)
    //            p->subtype = MEDIASUBTYPE_RGB32;
    //        else
    //            p->subtype = MEDIASUBTYPE_ARGB32;
    //        val = p;
    //        return S_OK;
    //    }

    //    int GetCount() { return 3; }
    //};

public:
    // IPin
    virtual HRESULT STDMETHODCALLTYPE Connect(IPin *pReceivePin, const AM_MEDIA_TYPE *pmt) tm_override
    {
        dshow_trace_method(TM_T("CBasePinProxy::Connect"), TM_T("(pin:%s, %s - %s - %s)"),
            DS_DEBUG_PIN2STR(pReceivePin), DbgFormatGuid(pmt->majortype).c_str(),
            DbgFormatGuid(pmt->formattype).c_str(), DbgFormatGuid(pmt->subtype).c_str());
        HRESULT hr;
        DbgVerifyHResult(hr = m_pPin->Connect(pReceivePin, pmt));
        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE ReceiveConnection(IPin *pConnector, const AM_MEDIA_TYPE *pmt) tm_override
    {
        // in the function user decide media type.
        dshow_trace_method("CBasePinProxy::ReceiveConnection");
        HRESULT hr;
        DbgVerifyHResult(hr = m_pPin->ReceiveConnection(pConnector, pmt));
        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE Disconnect() tm_override
    {
        dshow_trace_method("CBasePinProxy::Disconnect");
        HRESULT hr;
        DbgVerifyHResult(hr = m_pPin->Disconnect());
        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE ConnectedTo(IPin **ppPin) tm_override
    {
        dshow_trace_method("CBasePinProxy::ConnectedTo");
        HRESULT hr;
        DbgVerifyHResult(hr = m_pPin->ConnectedTo(ppPin));
        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE ConnectionMediaType(AM_MEDIA_TYPE *pmt) tm_override
    {
        dshow_trace_method("CBasePinProxy::ConnectionMediaType");
        HRESULT hr;
        DbgVerifyHResult(hr = m_pPin->ConnectionMediaType(pmt));
        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE QueryPinInfo(PIN_INFO *pInfo) tm_override
    {
        dshow_trace_method("CBasePinProxy::QueryPinInfo");
        HRESULT hr;
        DbgVerifyHResult(hr = m_pPin->QueryPinInfo(pInfo));
        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE QueryDirection(PIN_DIRECTION *pPinDir) tm_override
    {
        dshow_trace_method("CBasePinProxy::QueryDirection");
        HRESULT hr;
        DbgVerifyHResult(hr = m_pPin->QueryDirection(pPinDir));
        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE QueryId(LPWSTR *Id) tm_override
    {
        dshow_trace_method("CBasePinProxy::QueryId");
        HRESULT hr;
        DbgVerifyHResult(hr = m_pPin->QueryId(Id));
        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE QueryAccept(const AM_MEDIA_TYPE *pmt) tm_override
    {
        dshow_trace_method(TM_T("CBasePinProxy::QueryAccept"), TM_T("(%s - %s - %s)"),
            DbgFormatGuid(pmt->majortype).c_str(), DbgFormatGuid(pmt->formattype).c_str(), DbgFormatGuid(pmt->subtype).c_str());
        HRESULT hr;
        DbgVerifyHResult(hr = m_pPin->QueryAccept(pmt));
        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE EnumMediaTypes(IEnumMediaTypes **ppEnum) tm_override
    {
        dshow_trace_method("CBasePinProxy::EnumMediaTypes");
//         *ppEnum = new CEnumMT();
//         return S_OK;
        HRESULT hr;
        DbgVerifyHResult(hr = m_pPin->EnumMediaTypes(ppEnum));
#ifdef TM_DBG
        if (SUCCEEDED(hr))
        {
            ULONG count;
            AM_MEDIA_TYPE *pmt;
            while ((*ppEnum)->Next(1, &pmt, &count), count == 1)
            {
                DbgAssert(pmt);
                DbgTrace(TM_T("BasePinProxy::EnumMediaTypes(%s - %s - %s)"), DbgFormatGuid(pmt->majortype).c_str(), DbgFormatGuid(pmt->formattype).c_str(), DbgFormatGuid(pmt->subtype).c_str());
                dsDeleteMediaType(pmt);
            }
            (*ppEnum)->Reset();
        }
#endif
        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE QueryInternalConnections(IPin **apPins, ULONG *nPin) tm_override
    {
        dshow_trace_method("CBasePinProxy::QueryInternalConnections");
        HRESULT hr;
        DbgVerifyHResult(hr = m_pPin->QueryInternalConnections(apPins, nPin));
        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE EndOfStream() tm_override
    {
        dshow_trace_method("CBasePinProxy::EndOfStream");
        HRESULT hr;
        DbgVerifyHResult(hr = m_pPin->EndOfStream());
        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE BeginFlush() tm_override
    {
        dshow_trace_method("CBasePinProxy::BeginFlush");
        HRESULT hr;
        DbgVerifyHResult(hr = m_pPin->BeginFlush());
        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE EndFlush() tm_override
    {
        dshow_trace_method("CBasePinProxy::EndFlush");
        HRESULT hr;
        DbgVerifyHResult(hr = m_pPin->EndFlush());
        return hr;
    }

    virtual HRESULT STDMETHODCALLTYPE NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate) tm_override
    {
        dshow_trace_method("CBasePinProxy::NewSegment");
        HRESULT hr;
        DbgVerifyHResult(hr = m_pPin->NewSegment(tStart, tStop, dRate));
        return hr;
    }

    // IMemInputPin
    virtual HRESULT STDMETHODCALLTYPE GetAllocator(IMemAllocator **ppAllocator) tm_override
    {
        dshow_trace_method("CMemInputPinProxy::GetAllocator");
        HRESULT hr;
        DbgVerifyHResult(hr = m_pMemInputPin->GetAllocator(ppAllocator));
        return hr;
    }
    
    virtual HRESULT STDMETHODCALLTYPE NotifyAllocator(IMemAllocator *pAllocator, BOOL bReadOnly) tm_override
    {
        dshow_trace_method("CMemInputPinProxy::NotifyAllocator");
        HRESULT hr;
        DbgVerifyHResult(hr = m_pMemInputPin->NotifyAllocator(pAllocator, bReadOnly));
        return hr;
    }
    
    virtual HRESULT STDMETHODCALLTYPE GetAllocatorRequirements(ALLOCATOR_PROPERTIES *pProps) tm_override
    {
        dshow_trace_method("CMemInputPinProxy::GetAllocatorRequirements");
        HRESULT hr;
        DbgVerifyHResult(hr = m_pMemInputPin->GetAllocatorRequirements(pProps));
        return hr;
    }
    
    virtual HRESULT STDMETHODCALLTYPE Receive(IMediaSample *pSample) tm_override
    {
        // in the function user render data.
        dshow_trace_method("CMemInputPinProxy::Receive");
        HRESULT hr;
        DbgVerifyHResult(hr = m_pMemInputPin->Receive(pSample));
        return hr;
    }
    
    virtual HRESULT STDMETHODCALLTYPE ReceiveMultiple(IMediaSample **pSamples, long nSamples, long *nSamplesProcessed) tm_override
    {
        dshow_trace_method("CMemInputPinProxy::ReceiveMultiple");
        HRESULT hr;
        DbgVerifyHResult(hr = m_pMemInputPin->ReceiveMultiple(pSamples, nSamples, nSamplesProcessed));
        return hr;
    }
    
    virtual HRESULT STDMETHODCALLTYPE ReceiveCanBlock() tm_override
    {
        dshow_trace_method("CMemInputPinProxy::ReceiveCanBlock");
        HRESULT hr;
        DbgVerifyHResult(hr = m_pMemInputPin->ReceiveCanBlock());
        return hr;
    }
};


///////////////////////////////////////////////////////////////////////////
// CBaseFilterProxy
template <class T>
class CFilterProxy : public IBaseFilter
{
public:
    CComPtr<IBaseFilter>    m_pBaseFilter;

private:
    TM_UNUSED_ADDREF_RELEASE();
    TM_BGN_INTERFACE_MAP(IBaseFilter)
        dshow_trace(TM_T("CFilterProxy::QueryInterface:%s"),  DbgFormatGuid(iid).c_str());
        TM_INTERFACE_HANDLER(IPersist)
        TM_INTERFACE_HANDLER(IMediaFilter)
        TM_INTERFACE_HANDLER(IBaseFilter)
        TM_INTERFACE_MAP_MEMBER(GetThis()->m_pBaseFilter)
    TM_END_INTERFACE_MAP()
        
    T* GetThis() { return static_cast<T*>(this); }

public:
    // IPersist
    virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID *pClassID) tm_override
    {
        dshow_trace_method("CBaseFilterProxy::GetClassID");
        HRESULT hr;
        DbgVerifyHResult(hr = GetThis()->m_pBaseFilter->GetClassID(pClassID));
        dshow_trace(TM_T("CBaseFilterProxy::GetClassID(%s)"), DbgFormatGuid(*pClassID).c_str());
        return hr;
    }
    
    // IMediaFilter
    virtual HRESULT STDMETHODCALLTYPE Stop() tm_override
    {
        dshow_trace_method("CBaseFilterProxy::Stop");
        HRESULT hr;
        DbgVerifyHResult(hr = GetThis()->m_pBaseFilter->Stop());
        return hr;
    }
    
    virtual HRESULT STDMETHODCALLTYPE Pause() tm_override
    {
        dshow_trace_method("CBaseFilterProxy::Pause");
        HRESULT hr;
        DbgVerifyHResult(hr = GetThis()->m_pBaseFilter->Pause());
        return hr;
    }
    
    virtual HRESULT STDMETHODCALLTYPE Run(REFERENCE_TIME tStart) tm_override
    {
        dshow_trace_method("CBaseFilterProxy::Run");
        HRESULT hr;
        DbgVerifyHResult(hr = GetThis()->m_pBaseFilter->Run(tStart));
        return hr;
    }
    
    virtual HRESULT STDMETHODCALLTYPE GetState(DWORD dwMilliSecsTimeout, FILTER_STATE *State) tm_override
    {
        dshow_trace_method("CBaseFilterProxy::GetState");
        HRESULT hr;
        DbgVerifyHResult(hr = GetThis()->m_pBaseFilter->GetState(dwMilliSecsTimeout, State));
        dshow_trace("CBaseFilterProxy::GetState(dwMilliSecsTimeout:%d, State:%d)", dwMilliSecsTimeout, *State);
        return hr;
    }
    
    virtual HRESULT STDMETHODCALLTYPE SetSyncSource(IReferenceClock *pClock) tm_override
    {
        dshow_trace_method("CBaseFilterProxy::SetSyncSource");
        HRESULT hr;
        DbgVerifyHResult(hr = GetThis()->m_pBaseFilter->SetSyncSource(pClock));
        return hr;
    }
    
    virtual HRESULT STDMETHODCALLTYPE GetSyncSource(IReferenceClock **pClock) tm_override
    {
        dshow_trace_method("CBaseFilterProxy::GetSyncSource");
        HRESULT hr;
        DbgVerifyHResult(hr = GetThis()->m_pBaseFilter->GetSyncSource(pClock));
        return hr;
    }
    
    // IBaseFilter
//     virtual HRESULT STDMETHODCALLTYPE EnumPins(IEnumPins **ppEnum) tm_override
//     {
//         dshow_trace_method("CBaseFilterProxy::EnumPins");
//         HRESULT hr;
//         DbgVerifyHResult(hr = GetThis()->m_pBaseFilter->EnumPins(ppEnum));
//         IPin* cc[10];
//         ULONG x = 0;
//         (*ppEnum)->Next(10, cc, &x);
//         return hr;
//     }
    
    virtual HRESULT STDMETHODCALLTYPE FindPin(LPCWSTR Id, IPin **ppPin) tm_override
    {
        dshow_trace_method("CBaseFilterProxy::FindPin");
        HRESULT hr;
        DbgVerifyHResult(hr = GetThis()->m_pBaseFilter->FindPin(Id, ppPin));
        return hr;
    }
    
    virtual HRESULT STDMETHODCALLTYPE QueryFilterInfo(FILTER_INFO *pInfo) tm_override
    {
        dshow_trace_method("CBaseFilterProxy::QueryFilterInfo");
        HRESULT hr;
        DbgVerifyHResult(hr = GetThis()->m_pBaseFilter->QueryFilterInfo(pInfo));
        return hr;
    }
    
    virtual HRESULT STDMETHODCALLTYPE JoinFilterGraph(IFilterGraph *pGraph, LPCWSTR pName) tm_override
    {
        dshow_trace_method("CBaseFilterProxy::JoinFilterGraph");
        HRESULT hr;
        DbgVerifyHResult(hr = GetThis()->m_pBaseFilter->JoinFilterGraph(pGraph, pName));
        return hr;
    }
    
    virtual HRESULT STDMETHODCALLTYPE QueryVendorInfo(LPWSTR *pVendorInfo) tm_override
    {
        dshow_trace_method("CBaseFilterProxy::QueryVendorInfo");
        HRESULT hr;
        DbgVerifyHResult(hr = GetThis()->m_pBaseFilter->QueryVendorInfo(pVendorInfo));
        return hr;
    }
};

class CAMGraphBuilderCallback : public IAMGraphBuilderCallback
{
private:
    TM_UNUSED_ADDREF_RELEASE();
    TM_BGN_INTERFACE_MAP(IAMGraphBuilderCallback)
        dshow_trace(TM_T("CAMGraphBuilderCallback::QueryInterface:%s"),  DbgFormatGuid(iid).c_str());
        TM_INTERFACE_HANDLER(IAMGraphBuilderCallback)
    TM_END_INTERFACE_MAP()

    virtual HRESULT STDMETHODCALLTYPE SelectedFilter(IMoniker *pMon) tm_override
    {
        dshow_trace_method("CAMGraphBuilderCallback::SelectedFilter");
        CComBSTR str;
        MonikerGetFriendlyName(pMon, &str);
        dshow_trace(TM_T("CAMGraphBuilderCallback::SelectedFilter(%s)"), (BSTR)str);
        if (str == L"Microsoft DTV-DVD Video Decoder") // soft render not support the decoder filter
            return E_FAIL;
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE CreatedFilter(IBaseFilter *pFil) tm_override
    {
        dshow_trace_method("CAMGraphBuilderCallback::CreatedFilter");
        return S_OK;
    }

public:
    void Init(IUnknown* pGraphBuild)
    {
        CComQIPtr<IObjectWithSite> pObjectWithSite(pGraphBuild);
        DbgAssert(pObjectWithSite);
        DbgVerifyHResult(pObjectWithSite->SetSite(this));
    }

    void Uninit(IUnknown* pGraphBuild)
    {
        CComQIPtr<IObjectWithSite> pObjectWithSite(pGraphBuild);
        if (pObjectWithSite)
            DbgVerifyHResult(pObjectWithSite->SetSite(0));
    }
};



template <class T>
class CRenderNormalImpl
    : public CFilterProxy<T>
    , public CPinProxy
{
private:
    CAMGraphBuilderCallback m_callback;

protected:
    T* GetThis() { return static_cast<T*>(this); }

    // IBaseFilter
    virtual HRESULT STDMETHODCALLTYPE EnumPins(IEnumPins **ppEnum) tm_override
    {
        dshow_trace_method("CRenderNormalImpl::EnumPins");
        *ppEnum = new CEnumInterface1<IPin, IEnumPins>(this);
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE JoinFilterGraph(IFilterGraph *pGraph, LPCWSTR pName) tm_override
    {
        dshow_trace_method("CRenderNormalImpl::JoinFilterGraph");
        HRESULT hr = CFilterProxy<T>::JoinFilterGraph(pGraph, pName);
        if (SUCCEEDED(hr) && pGraph)
            m_callback.Init(pGraph);
        return hr;
    }

    // IPin
    virtual HRESULT STDMETHODCALLTYPE QueryPinInfo(PIN_INFO *pInfo) tm_override
    {
        HRESULT hr = CPinProxy::QueryPinInfo(pInfo);
        DbgAssert(SUCCEEDED(hr));
        DbgAssert(pInfo->pFilter == GetThis()->m_pBaseFilter);
        pInfo->pFilter->Release();
        pInfo->pFilter = this;
        pInfo->pFilter->AddRef();
        return hr;
    }

    void InitProxy()
    {
        IBaseFilter* p = GetThis()->m_pBaseFilter;
        DbgAssert(p);
        dsGetFirstPin(p, m_pPin);
        m_pMemInputPin = m_pPin;
    }

public:
    IBaseFilter* GetRenderer() { return this; }

    TMBool InitRenderer()
    {
        dshow_trace_method("CRenderNormalImpl::InitRenderer");
        DbgAssert(!GetThis()->m_pBaseFilter);
        DECLARE_GUID(CLSID_NullRenderer, 0xC1F400A4, 0x3F08, 0x11d3, 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37);
        DbgVerifyHResult(GetThis()->m_pBaseFilter.CoCreateInstance(CLSID_NullRenderer, 0, CLSCTX_INPROC_SERVER));
        if (GetThis()->m_pBaseFilter)
        {
            InitProxy();
            return true;
        }
        return false;
    }
    
//     void UninitRenderer()
//     {
//         dshow_trace_method("CRenderNormalImpl::UninitRenderer");
//         GetThis()->m_pBaseFilter.Release();
//         m_pPin.Release();
//         m_pMemInputPin.Release();
//     }
};


///////////////////////////////////////////////////////////////////////////
// CRendererVMR9T
// template <class T>
// class CRendererVMR9Impl
//     : public IVMRImagePresenter9
//     , public IVMRSurfaceAllocator9
// {
// protected:
//     typedef typename CRenderBase::CAutoLock     CAutoLock_VMR9;
//     TM::CVector<CComPtr<IDirect3DSurface9> >    m_aSurfaces;
// 
//     T* GetThis() { return static_cast<T*>(this); }
//     void ClearSurfaces() { m_aSurfaces.clear(); }
// 
// public:
//     TM_UNUSED_ADDREF_RELEASE    ();
//     TM_BGN_INTERFACE_MAP        (IVMRSurfaceAllocator9)
//         dshow_trace(TM_T("CRendererVMR9Impl::QueryInterface:%s"), DbgFormatGuid(iid).c_str());
//         TM_INTERFACE_HANDLER    (IVMRSurfaceAllocator9)
//         TM_INTERFACE_HANDLER    (IVMRImagePresenter9)
//     TM_END_INTERFACE_MAP        ()
// 
//     IBaseFilter* GetRenderer() { return GetThis()->m_pBaseFilter; }
// 
//     TMBool InitRenderer(IDirect3DDevice9* pDevice)
//     {
//         dshow_trace_method("CRendererVMR9Impl::InitRenderer");
//         DbgAssert(!GetThis()->m_pBaseFilter);
//         DbgVerifyHResult(GetThis()->m_pBaseFilter.CoCreateInstance(CLSID_VideoMixingRenderer9, 0, CLSCTX_INPROC_SERVER));
//         CComQIPtr<IVMRFilterConfig9> ptrFilterConfig(GetThis()->m_pBaseFilter);
//         if (ptrFilterConfig)
//         {
//             DbgVerifyHResult(ptrFilterConfig->SetRenderingMode(VMR9Mode_Renderless));
//             //DbgVerifyHResult(ptrFilterConfig->SetNumberOfStreams(2)); // don't call, the function will set VMR to mix mode.
//             
//             CComQIPtr<IVMRSurfaceAllocatorNotify9> ptrNotify9(ptrFilterConfig);
//             if (ptrNotify9)
//             {
//                 DbgVerifyHResult(ptrNotify9->AdviseSurfaceAllocator(1, this));
//                 CComPtr<IDirect3D9> ptrD3D;
//                 pDevice->GetDirect3D(&ptrD3D);
//                 HMONITOR hMonitor = ptrD3D->GetAdapterMonitor(D3DADAPTER_DEFAULT);
//                 DbgVerifyHResult(ptrNotify9->SetD3DDevice(pDevice, hMonitor));
//                 return true;
//             }
//         }
//         return false;
//     }
// 
//     void UninitRenderer()
//     {
//         dshow_trace_method("CRendererVMR9Impl::UninitRenderer");
//         ClearSurfaces();
//         GetThis()->m_pBaseFilter.Release();
//     }
// 
//     void OnPlaying(TMBool) {}
// 
// 
//     // IVMRSurfaceAllocator9
//     virtual HRESULT STDMETHODCALLTYPE InitializeDevice(DWORD_PTR dwUserID, VMR9AllocationInfo *lpAllocInfo, DWORD *lpNumBuffers) tm_override
//     {
//         if (!lpNumBuffers)
//             return E_POINTER;
// 
//         dshow_trace_method("CRendererVMR9Impl::InitializeDevice", "; numBuffers:%d, format:%s, Flags:%d",
//             *lpNumBuffers, GX_DEBUG_FORMAT2STR(lpAllocInfo->Format), lpAllocInfo->dwFlags);
// 
//         //if (lpAllocInfo->Format == gxMakeFourCC1('NV12'))
//         //    return E_FAIL;
//         //if (lpAllocInfo->Format == gxMakeFourCC1('YUY2'))
//         //    return E_FAIL;
//         
// 
//         // ?
//         //if (lpAllocInfo->dwFlags & VMR9AllocFlag_3DRenderTarget)
//         //    lpAllocInfo->dwFlags |= VMR9AllocFlag_TextureSurface;
// 
//         CAutoLock_VMR9 Lock(GetThis()->m_critical);
//         ClearSurfaces();
//         m_aSurfaces.resize(*lpNumBuffers);
//         CComQIPtr<IVMRSurfaceAllocatorNotify9> ptrNotify9(GetThis()->m_pBaseFilter);
//         HRESULT hr = ptrNotify9->AllocateSurfaceHelper(lpAllocInfo, lpNumBuffers, &m_aSurfaces[0]);
// 
// 
// #ifdef TM_DBG
//         if (SUCCEEDED(hr))
//         {
//             for (int i = 0; i < *lpNumBuffers; ++i)
//             {
//                 D3DSURFACE_DESC dds;
//                 m_aSurfaces[i]->GetDesc(&dds);
//                 dshow_trace("id:%d, Type:%d, format:%s, Usage:%d, Pool:%d, MultiSampleType:%d, MultiSampleQuality:%d, w:%d, h:%d, type:%d)",
//                     i, dds.Type, GX_DEBUG_FORMAT2STR(dds.Format), dds.Usage, dds.Pool, dds.MultiSampleType, dds.MultiSampleQuality, dds.Width, dds.Height, m_aSurfaces[i]->GetType());
// //             type;
// //             D3DRTYPE_SURFACE                =  1,
// //             D3DRTYPE_VOLUME                 =  2,
// //             D3DRTYPE_TEXTURE                =  3,
// //             D3DRTYPE_VOLUMETEXTURE          =  4,
// //             D3DRTYPE_CUBETEXTURE            =  5,
// //             D3DRTYPE_VERTEXBUFFER           =  6,
// //             D3DRTYPE_INDEXBUFFER            =  7,
// //             format;
// //             #define D3DFMT_R8G8B8       20
// //             #define D3DFMT_A8R8G8B8     21
// //             #define D3DFMT_X8R8G8B8     22
// //             #define D3DFMT_R5G6B5       23
// //             #define D3DFMT_X1R5G5B5     24
// //             Usage;
// //             #define D3DUSAGE_RENDERTARGET       (0x00000001L)
// //             #define D3DUSAGE_DEPTHSTENCIL       (0x00000002L)
// //             #define D3DUSAGE_DYNAMIC            (0x00000200L)
// //             Pool;
// //             D3DPOOL_DEFAULT                 = 0,
// //             D3DPOOL_MANAGED                 = 1,
// //             D3DPOOL_SYSTEMMEM               = 2,
// //             D3DPOOL_SCRATCH                 = 3,
//             }
//         }
// #endif
//         return hr;
//     }
// 
//     virtual HRESULT STDMETHODCALLTYPE TerminateDevice(DWORD_PTR dwID)
//     {
//         dshow_trace_method("CRendererVMR9Impl::TerminateDevice");
//         ClearSurfaces();
//         return S_OK;
//     }
// 
//     virtual HRESULT STDMETHODCALLTYPE GetSurface(DWORD_PTR dwUserID, DWORD SurfaceIndex, DWORD SurfaceFlags, IDirect3DSurface9 **lplpSurface) tm_override
//     {
//         dshow_trace_method("CRendererVMR9Impl::GetSurface", "; SurfaceIndex:%d", SurfaceIndex);
//         if (!lplpSurface)
//             return E_POINTER;
//         if (SurfaceIndex >= m_aSurfaces.size()) 
//             return E_FAIL;
//         CAutoLock_VMR9 Lock(GetThis()->m_critical);
//         return m_aSurfaces[SurfaceIndex].CopyTo(lplpSurface);
//     }
// 
//     virtual HRESULT STDMETHODCALLTYPE AdviseNotify(IVMRSurfaceAllocatorNotify9 *lpIVMRSurfAllocNotify) tm_override
//     {
//         dshow_trace_method("CRendererVMR9Impl::AdviseNotify");
//         return S_OK;
//     };
// 
//     // IVMRImagePresenter9
//     virtual HRESULT STDMETHODCALLTYPE StartPresenting(DWORD_PTR dwUserID) tm_override
//     {
//         dshow_trace_method("CRendererVMR9Impl::StartPresenting");
//         return S_OK;
//     };
// 
//     virtual HRESULT STDMETHODCALLTYPE StopPresenting(DWORD_PTR dwUserID) tm_override
//     {
//         dshow_trace_method("CRendererVMR9Impl::StopPresenting");
//         return S_OK;
//     };
// 
//     virtual HRESULT STDMETHODCALLTYPE PresentImage(DWORD_PTR dwUserID, VMR9PresentationInfo *lpPresInfo) tm_override
//     {
//         dshow_trace_method("CRendererVMR9Impl::PresentImage");
//         return S_OK;
//     };
// };



///////////////////////////////////////////////////////////////////////////
// CGraphBuilder
struct CGraphBuilder
{
public:
    CComPtr<IGraphBuilder> m_ptrGraphBuilder;

public:
    operator bool () const { return m_ptrGraphBuilder != 0; }

    void SetEventSink(HWND hWnd, void* pVoid, TMUInt uMsg)
    {
        dshow_trace_method ("CGraphBuilder::SetEventSink");
        DebugCheckNoMessageLoop();
        DbgAssert(m_ptrGraphBuilder);
        DbgAssert(hWnd);
        CComQIPtr<IMediaEventEx> ptrEvent(m_ptrGraphBuilder);
        DbgVerifyHResult(ptrEvent->SetNotifyWindow((OAHWND)hWnd, uMsg, (LONG_PTR)pVoid));
    }

    void UnsetEventSink()
    {
        dshow_trace_method ("CGraphBuilder::UnsetEventSink");
        DbgAssert(m_ptrGraphBuilder);
        DebugCheckNoMessageLoop();
        CComQIPtr<IMediaEventEx> ptrEvent(m_ptrGraphBuilder);
        DbgVerifyHResult(ptrEvent->SetNotifyWindow(0, 0, 0));
    }

    TMBool RawCreate()
    {
        DbgAssert(!m_ptrGraphBuilder);
        HRESULT hr;
        dshow_trace_method ("CGraphBuilder::RawCreate");
//         DebugCheckNoMessageLoop();
        DbgVerifyHResult(hr = m_ptrGraphBuilder.CoCreateInstance(CLSID_FilterGraph)); // the function will call "PeekMessage"
        return SUCCEEDED(hr);
    }

    void RawClose()
    {
        DbgAssert(m_ptrGraphBuilder);
        dshow_trace_method("CGraphBuilder::RawClose()");

        { // stop the graph
            CComQIPtr<IMediaControl> pMc(m_ptrGraphBuilder);
            if (pMc)
            {
                HRESULT hr;
                OAFilterState state;
                do { hr = pMc->GetState(100, &state); }
                while (S_OK == hr && State_Stopped != state);
            }
        }

        // remove all filter
        for (;;)
        {
            CComPtr<IEnumFilters> pEnum;
            if (S_OK != m_ptrGraphBuilder->EnumFilters(&pEnum))
                break;
            CComPtr<IBaseFilter> pFilter;
            if (S_OK != pEnum->Next(1, &pFilter, NULL))
                break;
            m_ptrGraphBuilder->RemoveFilter(pFilter);
        }
        
        dshow_trace("CGraphBuilder::RawClose(ref:%d)", DbgUnknownGetRef(m_ptrGraphBuilder));
        m_ptrGraphBuilder.Release(); // the function will call GetMessage();
    }

    TMBool RawSetPositions(REFERENCE_TIME nPos)
    {
        CComQIPtr<IMediaSeeking> ptr(m_ptrGraphBuilder);
        if (ptr)
        {
            dshow_trace_method ("CGraphBuilder::RawSetPositions");
            DebugCheckNoMessageLoop();
            HRESULT hr;
            DbgVerifyHResult(hr = ptr->SetPositions(&nPos, AM_SEEKING_AbsolutePositioning, NULL, 0));
            return SUCCEEDED(hr);
        }
        return false;
    }
    
    REFERENCE_TIME RawGetPositions()
    {
        CComQIPtr<IMediaSeeking> ptr(m_ptrGraphBuilder);
        if (ptr)
        {
            dshow_trace_method ("CGraphBuilder::RawGetPositions");
            REFERENCE_TIME nPos = 0;
            DbgVerifyHResult(ptr->GetCurrentPosition(&nPos));
            return nPos;
        }
        return 0;
    }
    
    FILTER_STATE RawGetState()
    {
        OAFilterState fs = State_Stopped;
        CComQIPtr<IMediaControl> ptr(m_ptrGraphBuilder);
        if (ptr)
            ptr->GetState(0, &fs);
        return (FILTER_STATE)fs;
    }
    
    void RawSetState(FILTER_STATE fs)
    {
        CComQIPtr<IMediaControl> ptr(m_ptrGraphBuilder);
        if (!ptr)
            return;
        
        dshow_trace_method ("CGraphBuilder::RawSetState");
        DebugCheckNoMessageLoop();
        switch (fs)
        {
        case State_Running: ptr->Run(); break;
        case State_Paused:  ptr->Pause(); break;
        case State_Stopped: ptr->Stop(); break;
        }
    }

    void RawStop()
    {
        CComQIPtr<IMediaControl> ptr(m_ptrGraphBuilder);
        if (ptr)
        {
            dshow_trace_method ("CGraphBuilder::RawStop");
            DebugCheckNoMessageLoop();
            HRESULT hr;
            DbgVerifyHResult(hr = ptr->Stop());
        }
    }

    TMBool RawRun()
    {
        CComQIPtr<IMediaControl> ptr(m_ptrGraphBuilder);
        if (ptr)
        {
            dshow_trace_method ("CGraphBuilder::RawRun");
            DebugCheckNoMessageLoop();
            HRESULT hr;
            DbgVerifyHResult(hr = ptr->Run());
            return SUCCEEDED(hr);
        }
        return false;
    }

    TMBool RawPause()
    {
        CComQIPtr<IMediaControl> ptr(m_ptrGraphBuilder);
        if (ptr)
        {
            dshow_trace_method ("CGraphBuilder::RawPause");
            DebugCheckNoMessageLoop();
            HRESULT hr;
            DbgVerifyHResult(hr = ptr->Pause());
            return SUCCEEDED(hr);
        }
        return false;
    }

    TMBool RawPutVolume(TMBool bMute, float fVol)
    {
        CComQIPtr<IBasicAudio> ptr(m_ptrGraphBuilder);
        if (ptr)
        {
            dshow_trace_method("CGraphBuilder::DoSetVolume");
            DebugCheckNoMessageLoop();
            long n;
            if (bMute)
                n = -10000L;
            else
            {
                n = -pow(2.0f, (1.0f - fVol) * 13.28f); //(long)(fVol * (4000.0f)) - 4000L;
                dshow_trace("CGraphBuilder::DoSetVolume:%d", n);
            
                if (n == 0)
                {
                    DbgAssert(0);
                    n = -1;
                }
            }
            HRESULT hr;
            DbgVerifyHResult(hr = ptr->put_Volume(n));
            return SUCCEEDED(hr);
        }
        return false;
    }

    TMBool RawGetDuratione(TMUInt64& duratione, int nScale = 1)
    {
        CComQIPtr<IMediaSeeking> ptr(m_ptrGraphBuilder);
        if (ptr)
        {
            DebugCheckNoMessageLoop();
            REFERENCE_TIME nPos = 0;
            DbgVerifyHResult(ptr->GetDuration(&nPos));
            duratione = nPos * nScale;
            return true;
        }
        return false;
    }

    bool RawGetSize(TMSize& sz, bool bX2Y)
    {
        CComQIPtr<IBasicVideo> ptr(m_ptrGraphBuilder);
        if (ptr)
        {
            if (bX2Y)
            {
                ptr->get_SourceWidth(&sz.cy);
                ptr->get_SourceHeight(&sz.cx);
            }
            else
            {
                ptr->get_SourceWidth(&sz.cx);
                ptr->get_SourceHeight(&sz.cy);
            }
            return true;
        }
        return false;
    }
    
    TMBool RawRenderFile(TMCStr file)
    {
        DebugCheckNoMessageLoop();
        HRESULT hr;
        DbgVerifyHResult(hr = m_ptrGraphBuilder->RenderFile(file, 0));
        return SUCCEEDED(hr);
    }
};


///////////////////////////////////////////////////////////////////////////
// CGraphBuilderVideoT
template <class T>
class CGraphBuilderVideoT : public T
{
public:
    TMBool CreateVideo(IBaseFilter *pfRender) //, HWND hWnd, void* pVoid, TMUInt uMsg)
    {
        if (this->m_ptrGraphBuilder)
            return false;

        dshow_trace_method ("CGraphBuilderVideo::CreateVideo");
        this->RawCreate();
        DbgVerifyHResult(this->m_ptrGraphBuilder->AddFilter(pfRender, DS_FILTER_NAME));
//        this->SetEventSink(hWnd, pVoid, uMsg);
        return true;
    }
    
    void DetachVideo()
    {
        if (!this->m_ptrGraphBuilder)
            return;

        dshow_trace_method ("CGraphBuilderVideo::CloseVideo");
        this->UnsetEventSink();
        this->RawStop();
//         DbgVerifyHResult(m_ptrGraphBuilder->RemoveFilter(pfRender));
//         this->RawClose();
    }
};
typedef CGraphBuilderVideoT<CGraphBuilder> CGraphBuilderVideo;


///////////////////////////////////////////////////////////////////////////
// CGraphBuilderCaptureT
template <class T>
class CGraphBuilderCaptureT : public T
{
public:
    typedef CComPtr<IMoniker>           CMonikerPtr;
    typedef TM::CVector<CMonikerPtr>    CMonikerList;

protected:
    CComPtr<ICaptureGraphBuilder2>  m_ptrCapture;

public:
    void CreateVideo(IBaseFilter *pfRender)
    {
        if (this->m_ptrGraphBuilder)
            return;
        
        dshow_trace_method ("CGraphBuilderCapture::CreateVideo");
        this->RawCreate();
        DbgAssert(!m_ptrCapture);
        m_ptrCapture.CoCreateInstance(CLSID_CaptureGraphBuilder2);
        DbgVerifyHResult(m_ptrCapture->SetFiltergraph(this->m_ptrGraphBuilder));
        DbgVerifyHResult(this->m_ptrGraphBuilder->AddFilter(pfRender, L"TM Video Renderer"));
    }
    
    void DetachVideo()
    {
        if (!this->m_ptrGraphBuilder)
            return;
        
        dshow_trace_method ("CGraphBuilderCapture::DetachVideo");
        this->RawStop();
//         DbgVerifyHResult(m_ptrGraphBuilder->RemoveFilter(pfRender));
        DbgVerifyHResult(m_ptrCapture->SetFiltergraph(0));
        m_ptrCapture.Release();
//         this->RawClose();
    }
    
    static void EnumerateDevices(CMonikerList& aDevices)
    {
        CComPtr<ICreateDevEnum> ptrCreateDevEnum;
        ptrCreateDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER);
        if (!ptrCreateDevEnum)
            return;
        
        CComPtr<IEnumMoniker> pEm;
        ptrCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
        if (!pEm)
            return;
        
        pEm->Reset();
        ULONG cFetched;
        IMoniker *pM;
        while (S_OK == pEm->Next(1, &pM, &cFetched))
        {
            aDevices.push_back(pM);
            pM->Release();
        }
    }
    
    static TMBool Moniker2Filter(IMoniker* pMoniker, IBaseFilter **pFilter)
    {
        DbgAssert(pMoniker);
        DbgAssert(pFilter);
        HRESULT hr;
        DbgVerifyHResult(hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)pFilter));
        return SUCCEEDED(hr);
    }
    
    TMBool Open(IMoniker* pMoniker, IBaseFilter* pGrabber = 0, IBaseFilter *pfRender = 0)
    {
        CComPtr<IBaseFilter> pfCapture;
        Moniker2Filter(pMoniker, &pfCapture);
        if (!pfCapture)
            return false;
        
        HRESULT hr = this->m_ptrGraphBuilder->AddFilter(pfCapture, L"Capture Filter");
        if (FAILED(hr))
            return false;
        
        // try to render preview/capture pin
        hr = this->m_ptrCapture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, pfCapture, pGrabber, pfRender);
        if (FAILED(hr))
            hr = this->m_ptrCapture->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pfCapture, pGrabber, pfRender);
        
        return (SUCCEEDED(hr));
    }
};
typedef CGraphBuilderCaptureT<CGraphBuilder> CGraphBuilderCapture;


TM_END_NAMESPACE


#if 0 //TM_MSC >= 1800

#include <mfapi.h>
//#include <mfidl.h>
#include <mferror.h>
#include <dxva2api.h>
#include <evr9.h>
#include <evcode.h> // EVR event codes (IMediaEventSink)


#pragma comment(lib, "dxva2.lib ")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "evr.lib")
#pragma comment(lib, "Mfplat.lib")


TM_BGN_NAMESPACE



template <bool t_bManaged>
class CMFMediaTypeT : public CRefPtrBaseT<IMFMediaType, t_bManaged>
{
public:
    typedef CRefPtrBaseT<IMFMediaType, t_bManaged> base_class;
    DECALRE_REFPTR_IMPL(CMFMediaTypeT, IMFMediaType);

protected:
    BOOL IsValid()
    {
        return this->p != NULL;
    }

    IMFMediaType* GetMediaType()
    {
        DbgAssert(IsValid());
        return this->p;
    }

    void CheckPointer(void* _p, ...) { DbgAssert(_p); }

public:
    // address-of operator
    IMFMediaType** operator&()
    {
        return &this->p;
    }

    HRESULT CreateEmptyType()
    {
        DbgAssert(IsValid());
        HRESULT hr;
        DbgVerifyHResult(hr = MFCreateMediaType(&this->p));
        return hr;
    }

    // Direct wrappers of IMFMediaType methods.
    // (For these methods, we leave parameter validation to the IMFMediaType implementation.)

    // Retrieves the major type GUID.
    HRESULT GetMajorType(GUID *pGuid)
    {
        return GetMediaType()->GetMajorType(pGuid);
    }

    // Specifies whether the media data is compressed
    HRESULT IsCompressedFormat(BOOL *pbCompressed)
    {
        return GetMediaType()->IsCompressedFormat(pbCompressed);
    }

    // Compares two media types and determines whether they are identical.
    HRESULT IsEqual(IMFMediaType *pType, DWORD *pdwFlags)
    {
        return GetMediaType()->IsEqual(pType, pdwFlags);
    }

    // Retrieves an alternative representation of the media type.
    HRESULT GetRepresentation(GUID guidRepresentation, LPVOID *ppvRepresentation)
    {
        return GetMediaType()->GetRepresentation(guidRepresentation, ppvRepresentation);
    }

    // Frees memory that was allocated by the GetRepresentation method.
    HRESULT FreeRepresentation(GUID guidRepresentation, LPVOID pvRepresentation)
    {
        return GetMediaType()->FreeRepresentation(guidRepresentation, pvRepresentation);
    }


    // Helper methods

    // CopyFrom: Copy all of the attributes from another media type into this type.
    HRESULT CopyFrom(IMFMediaType *pType)
    {
        DbgAssert(IsValid());
        DbgAssert(pType);
        return pType->CopyAllItems(this->p);
    }

    // Sets the major type GUID.
    HRESULT SetMajorType(GUID guid)
    {
        return GetMediaType()->SetGUID(MF_MT_MAJOR_TYPE, guid);
    }

    // Retrieves the subtype GUID.
    HRESULT GetSubType(GUID* pGuid)
    {
        CheckPointer(pGuid, E_POINTER);
        return GetMediaType()->GetGUID(MF_MT_SUBTYPE, pGuid);
    }

    // Sets the subtype GUID.
    HRESULT SetSubType(GUID guid)
    {
        return GetMediaType()->SetGUID(MF_MT_SUBTYPE, guid);
    }

    // Extracts the FOURCC code from the subtype.
    // Not all subtypes follow this pattern.
    HRESULT GetFourCC(DWORD *pFourCC)
    {
        DbgAssert(IsValid());
        DbgAssert(pFourCC);
        GUID guidSubType;
        HRESULT hr = GetSubType(&guidSubType);
        *pFourCC = guidSubType.Data1;
        return hr;
    }

    //  Queries whether each sample is independent of the other samples in the stream.
    HRESULT GetAllSamplesIndependent(BOOL* pbIndependent)
    {
        DbgAssert(pbIndependent);
        return GetMediaType()->GetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, (UINT32*)pbIndependent);
    }

    //  Specifies whether each sample is independent of the other samples in the stream.
    HRESULT SetAllSamplesIndependent(BOOL bIndependent)
    {
        return GetMediaType()->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, (UINT32)bIndependent);
    }

    // Queries whether the samples have a fixed size.
    HRESULT GetFixedSizeSamples(BOOL *pbFixed)
    {
        DbgAssert(pbFixed);
        return GetMediaType()->GetUINT32(MF_MT_FIXED_SIZE_SAMPLES, (UINT32*)pbFixed);
    }

    // Specifies whether the samples have a fixed size.
    HRESULT SetFixedSizeSamples(BOOL bFixed)
    {
        return GetMediaType()->SetUINT32(MF_MT_FIXED_SIZE_SAMPLES, (UINT32)bFixed);
    }

    // Retrieves the size of each sample, in bytes. 
    HRESULT GetSampleSize(UINT32 *pnSize)
    {
        DbgAssert(pnSize);
        return GetMediaType()->GetUINT32(MF_MT_SAMPLE_SIZE, pnSize);
    }

    // Sets the size of each sample, in bytes. 
    HRESULT SetSampleSize(UINT32 nSize)
    {
        return GetMediaType()->SetUINT32(MF_MT_SAMPLE_SIZE, nSize);
    }

    // Retrieves a media type that was wrapped by the MFWrapMediaType function.
    HRESULT Unwrap(IMFMediaType **ppOriginal)
    {
        DbgAssert(ppOriginal);
        return ::MFUnwrapMediaType(GetMediaType(), ppOriginal);
    }

    // The following versions return reasonable defaults if the relevant attribute is not present (zero/FALSE).
    // This is useful for making quick comparisons betweeen media types. 

    BOOL AllSamplesIndependent()
    {
        return (BOOL)MFGetAttributeUINT32(GetMediaType(), MF_MT_ALL_SAMPLES_INDEPENDENT, FALSE);
    }

    BOOL FixedSizeSamples()
    {
        return (BOOL)MFGetAttributeUINT32(GetMediaType(), MF_MT_FIXED_SIZE_SAMPLES, FALSE);
    }

    UINT32 SampleSize()
    {
        return MFGetAttributeUINT32(GetMediaType(), MF_MT_SAMPLE_SIZE, 0);
    }
};

typedef CMFMediaTypeT<true>        CMFMediaType;
typedef CMFMediaTypeT<false>    MFMediaTypeHandle;


template <bool t_bManaged>
class CMFVideoMediaTypeT : public CMFMediaTypeT<t_bManaged>
{
public:
    typedef CMFMediaTypeT<t_bManaged> base_class;
    DECALRE_REFPTR_IMPL(CMFVideoMediaTypeT, IMFMediaType);

public:
    // Retrieves a description of how the frames are interlaced.
    HRESULT GetInterlaceMode(MFVideoInterlaceMode *pmode)
    {
        this->CheckPointer(pmode, E_POINTER);
        return this->GetMediaType()->GetUINT32(MF_MT_INTERLACE_MODE, (UINT32*)pmode);
    }

    // Sets a description of how the frames are interlaced.
    HRESULT SetInterlaceMode(MFVideoInterlaceMode mode)
    {
        return this->GetMediaType()->SetUINT32(MF_MT_INTERLACE_MODE, (UINT32)mode);
    }

    // This returns the default or attempts to compute it, in its absence.
    //HRESULT GetDefaultStride(LONG *plStride)
    //{
    //    return MediaFoundationSamples::GetDefaultStride(GetMediaType(), plStride);
    //}

    // Sets the default stride. Only appropriate for uncompressed data formats.
    HRESULT SetDefaultStride(LONG nStride)
    {
        return this->GetMediaType()->SetUINT32(MF_MT_DEFAULT_STRIDE, (UINT32)nStride);
    }

    // Retrieves the width and height of the video frame.
    HRESULT GetFrameDimensions(UINT32 *pdwWidthInPixels, UINT32 *pdwHeightInPixels)
    {
        return MFGetAttributeSize(this->GetMediaType(), MF_MT_FRAME_SIZE, pdwWidthInPixels, pdwHeightInPixels);
    }

    // Sets the width and height of the video frame.
    HRESULT SetFrameDimensions(UINT32 dwWidthInPixels, UINT32 dwHeightInPixels)
    {
        return MFSetAttributeSize(this->GetMediaType(), MF_MT_FRAME_SIZE, dwWidthInPixels, dwHeightInPixels);
    }

    // Retrieves the data error rate in bit errors per second
    HRESULT GetDataBitErrorRate(UINT32 *pRate)
    {
        this->CheckPointer(pRate, E_POINTER);
        return this->GetMediaType()->GetUINT32(MF_MT_AVG_BIT_ERROR_RATE, pRate);
    }

    // Sets the data error rate in bit errors per second
    HRESULT SetDataBitErrorRate(UINT32 rate)
    {
        return this->GetMediaType()->SetUINT32(MF_MT_AVG_BIT_ERROR_RATE, rate);
    }

    // Retrieves the approximate data rate of the video stream.
    HRESULT GetAverageBitRate(UINT32 *pRate)
    {
        this->CheckPointer(pRate, E_POINTER);
        return this->GetMediaType()->GetUINT32(MF_MT_AVG_BITRATE, pRate);
    }

    // Sets the approximate data rate of the video stream.
    HRESULT SetAvgerageBitRate(UINT32 rate)
    {
        return this->GetMediaType()->SetUINT32(MF_MT_AVG_BITRATE, rate);
    }

    // Retrieves custom color primaries.
    HRESULT GetCustomVideoPrimaries(MT_CUSTOM_VIDEO_PRIMARIES *pPrimaries)
    {
        this->CheckPointer(pPrimaries, E_POINTER);
        return this->GetMediaType()->GetBlob(MF_MT_CUSTOM_VIDEO_PRIMARIES, (UINT8*)pPrimaries, sizeof(MT_CUSTOM_VIDEO_PRIMARIES), NULL);
    }

    // Sets custom color primaries.
    HRESULT SetCustomVideoPrimaries(const MT_CUSTOM_VIDEO_PRIMARIES& primary)
    {
        return this->GetMediaType()->SetBlob(MF_MT_CUSTOM_VIDEO_PRIMARIES, (const UINT8*)&primary, sizeof(MT_CUSTOM_VIDEO_PRIMARIES));
    }

    // Gets the number of frames per second.
    HRESULT GetFrameRate(UINT32 *pnNumerator, UINT32 *pnDenominator)
    {
        this->CheckPointer(pnNumerator, E_POINTER);
        this->CheckPointer(pnDenominator, E_POINTER);
        return MFGetAttributeRatio(this->GetMediaType(), MF_MT_FRAME_RATE, pnNumerator, pnDenominator);
    }

    // Gets the frames per second as a ratio.
    HRESULT GetFrameRate(MFRatio *pRatio)
    {
        this->CheckPointer(pRatio, E_POINTER);
        return GetFrameRate((UINT32*)&pRatio->Numerator, (UINT32*)&pRatio->Denominator);
    }
    
    // Sets the number of frames per second.
    HRESULT SetFrameRate(UINT32 nNumerator, UINT32 nDenominator)
    {
        return MFSetAttributeRatio(this->GetMediaType(), MF_MT_FRAME_RATE, nNumerator, nDenominator);
    }

    // Sets the number of frames per second, as a ratio.
    HRESULT SetFrameRate(const MFRatio& ratio)
    {
        return MFSetAttributeRatio(this->GetMediaType(), MF_MT_FRAME_RATE, ratio.Numerator, ratio.Denominator);
    }

    // Queries the geometric aperture.
    HRESULT GetGeometricAperture(MFVideoArea *pArea)
    {
        this->CheckPointer(pArea, E_POINTER);
        return this->GetMediaType()->GetBlob(MF_MT_GEOMETRIC_APERTURE, (UINT8*)pArea, sizeof(MFVideoArea), NULL);
    }
    
    // Sets the geometric aperture.
    HRESULT SetGeometricAperture(const MFVideoArea& area)
    {
        return this->GetMediaType()->SetBlob(MF_MT_GEOMETRIC_APERTURE, (UINT8*)&area, sizeof(MFVideoArea));
    }
    
    // Retrieves the maximum number of frames from one key frame to the next.
    HRESULT GetMaxKeyframeSpacing(UINT32 *pnSpacing)
    {
        this->CheckPointer(pnSpacing, E_POINTER);
        return this->GetMediaType()->GetUINT32(MF_MT_MAX_KEYFRAME_SPACING, pnSpacing);
    }

    // Sets the maximum number of frames from one key frame to the next.
    HRESULT SetMaxKeyframeSpacing(UINT32 nSpacing)
    {
        return this->GetMediaType()->SetUINT32(MF_MT_MAX_KEYFRAME_SPACING, nSpacing);
    }

    // Retrieves the region that contains the valid portion of the signal.
    HRESULT GetMinDisplayAperture(MFVideoArea *pArea)
    {
        this->CheckPointer(pArea, E_POINTER);
        return this->GetMediaType()->GetBlob(MF_MT_MINIMUM_DISPLAY_APERTURE, (UINT8*)pArea, sizeof(MFVideoArea), NULL);
    }

    // Sets the the region that contains the valid portion of the signal.
    HRESULT SetMinDisplayAperture(const MFVideoArea& area)
    {
        return this->GetMediaType()->SetBlob(MF_MT_MINIMUM_DISPLAY_APERTURE, (UINT8*)&area, sizeof(MFVideoArea));
    }


    // Retrieves the aspect ratio of the output rectangle for a video media type. 
    HRESULT GetPadControlFlags(MFVideoPadFlags *pFlags)
    {
        this->CheckPointer(pFlags, E_POINTER);
        return this->GetMediaType()->GetUINT32(MF_MT_PAD_CONTROL_FLAGS, (UINT32*)pFlags);
    }

    // Sets the aspect ratio of the output rectangle for a video media type. 
    HRESULT SetPadControlFlags(MFVideoPadFlags flags)
    {
        return this->GetMediaType()->SetUINT32(MF_MT_PAD_CONTROL_FLAGS, flags);
    }

    // Retrieves an array of palette entries for a video media type. 
    HRESULT GetPaletteEntries(MFPaletteEntry *paEntries, UINT32 nEntries)
    {
        this->CheckPointer(paEntries, E_POINTER);
        return this->GetMediaType()->GetBlob(MF_MT_PALETTE, (UINT8*)paEntries, sizeof(MFPaletteEntry) * nEntries, NULL);
    }

    // Sets an array of palette entries for a video media type. 
    HRESULT SetPaletteEntries(MFPaletteEntry *paEntries, UINT32 nEntries)
    {
        this->CheckPointer(paEntries, E_POINTER);
        return this->GetMediaType()->SetBlob(MF_MT_PALETTE, (UINT8*)paEntries, sizeof(MFPaletteEntry) * nEntries);
    }

    // Retrieves the number of palette entries.
    HRESULT GetNumPaletteEntries(UINT32 *pnEntries)
    {
        this->CheckPointer(pnEntries, E_POINTER);
        UINT32 nBytes = 0;
        HRESULT hr = S_OK;
        hr = this->GetMediaType()->GetBlobSize(MF_MT_PALETTE, &nBytes);
        if (SUCCEEDED(hr))
        {
            if (nBytes % sizeof(MFPaletteEntry) != 0)
            {
                hr = E_UNEXPECTED;
            }
        }
        if (SUCCEEDED(hr))
        {
            *pnEntries = nBytes / sizeof(MFPaletteEntry);
        }
        return hr;
    }

    // Queries the 4? region of video that should be displayed in pan/scan mode.
    HRESULT GetPanScanAperture(MFVideoArea *pArea)
    {
        this->CheckPointer(pArea, E_POINTER);
        return this->GetMediaType()->GetBlob(MF_MT_PAN_SCAN_APERTURE, (UINT8*)pArea, sizeof(MFVideoArea), NULL);
    }
    
    // Sets the 4? region of video that should be displayed in pan/scan mode.
    HRESULT SetPanScanAperture(const MFVideoArea& area)
    {
        return this->GetMediaType()->SetBlob(MF_MT_PAN_SCAN_APERTURE, (UINT8*)&area, sizeof(MFVideoArea));
    }
    
    // Queries whether pan/scan mode is enabled.
    HRESULT IsPanScanEnabled(BOOL *pBool)
    {
        this->CheckPointer(pBool, E_POINTER);
        return this->GetMediaType()->GetUINT32(MF_MT_PAN_SCAN_ENABLED, (UINT32*)pBool);
    }

    // Sets whether pan/scan mode is enabled.
    HRESULT SetPanScanEnabled(BOOL bEnabled)
    {
        return this->GetMediaType()->SetUINT32(MF_MT_PAN_SCAN_ENABLED, (UINT32)bEnabled);
    }

    // Queries the pixel aspect ratio
    HRESULT GetPixelAspectRatio(UINT32 *pnNumerator, UINT32 *pnDenominator)
    {
        this->CheckPointer(pnNumerator, E_POINTER);
        this->CheckPointer(pnDenominator, E_POINTER);
        return MFGetAttributeRatio(this->GetMediaType(), MF_MT_PIXEL_ASPECT_RATIO, pnNumerator, pnDenominator);
    }       

    // Sets the pixel aspect ratio
    HRESULT SetPixelAspectRatio(UINT32 nNumerator, UINT32 nDenominator)
    {
        return MFSetAttributeRatio(this->GetMediaType(), MF_MT_PIXEL_ASPECT_RATIO, nNumerator, nDenominator);
    }

    HRESULT SetPixelAspectRatio(const MFRatio& ratio)
    {
        return MFSetAttributeRatio(this->GetMediaType(), MF_MT_PIXEL_ASPECT_RATIO, ratio.Numerator, ratio.Denominator);
    }

    // Queries the intended aspect ratio.
    HRESULT GetSourceContentHint(MFVideoSrcContentHintFlags *pFlags)
    {
        this->CheckPointer(pFlags, E_POINTER);
        return this->GetMediaType()->GetUINT32(MF_MT_SOURCE_CONTENT_HINT, (UINT32*)pFlags);
    }

    // Sets the intended aspect ratio.
    HRESULT SetSourceContentHint(MFVideoSrcContentHintFlags nFlags)
    {
        return this->GetMediaType()->SetUINT32(MF_MT_SOURCE_CONTENT_HINT, (UINT32)nFlags);
    }

    // Queries an enumeration which represents the conversion function from RGB to R'G'B'.
    HRESULT GetTransferFunction(MFVideoTransferFunction *pnFxn)
    {
        this->CheckPointer(pnFxn, E_POINTER);
        return this->GetMediaType()->GetUINT32(MF_MT_TRANSFER_FUNCTION, (UINT32*)pnFxn);
    }

    // Set an enumeration which represents the conversion function from RGB to R'G'B'.
    HRESULT SetTransferFunction(MFVideoTransferFunction nFxn)
    {
        return this->GetMediaType()->SetUINT32(MF_MT_TRANSFER_FUNCTION, (UINT32)nFxn);
    }

    // Queries how chroma was sampled for a Y'Cb'Cr' video media type.
    HRESULT GetChromaSiting(MFVideoChromaSubsampling *pSampling)
    {
        this->CheckPointer(pSampling, E_POINTER);
        return this->GetMediaType()->GetUINT32(MF_MT_VIDEO_CHROMA_SITING, (UINT32*)pSampling);
    }
    
    // Sets how chroma was sampled for a Y'Cb'Cr' video media type.
    HRESULT SetChromaSiting(MFVideoChromaSubsampling nSampling)
    {
        return this->GetMediaType()->SetUINT32(MF_MT_VIDEO_CHROMA_SITING, (UINT32)nSampling);
    }
    
    // Queries the optimal lighting conditions for viewing.
    HRESULT GetVideoLighting(MFVideoLighting *pLighting)
    {
        this->CheckPointer(pLighting, E_POINTER);
        return this->GetMediaType()->GetUINT32(MF_MT_VIDEO_LIGHTING, (UINT32*)pLighting);
    }
    
    // Sets the optimal lighting conditions for viewing.
    HRESULT SetVideoLighting(MFVideoLighting nLighting)
    {
        return this->GetMediaType()->SetUINT32(MF_MT_VIDEO_LIGHTING, (UINT32)nLighting);
    }
    
    // Queries the nominal range of the color information in a video media type. 
    HRESULT GetVideoNominalRange(MFNominalRange *pRange)
    {
        this->CheckPointer(pRange, E_POINTER);
        return this->GetMediaType()->GetUINT32(MF_MT_VIDEO_NOMINAL_RANGE, (UINT32*)pRange);
    }

    // Sets the nominal range of the color information in a video media type. 
    HRESULT SetVideoNominalRange(MFNominalRange nRange)
    {
        return this->GetMediaType()->SetUINT32(MF_MT_VIDEO_NOMINAL_RANGE, (UINT32)nRange);
    }

    // Queries the color primaries for a video media type.
    HRESULT GetVideoPrimaries(MFVideoPrimaries *pPrimaries)
    {
        this->CheckPointer(pPrimaries, E_POINTER);
        return this->GetMediaType()->GetUINT32(MF_MT_VIDEO_PRIMARIES, (UINT32*)pPrimaries);
    }

    // Sets the color primaries for a video media type.
    HRESULT SetVideoPrimaries(MFVideoPrimaries nPrimaries)
    {
        return this->GetMediaType()->SetUINT32(MF_MT_VIDEO_PRIMARIES, (UINT32)nPrimaries);
    }

    // Gets a enumeration representing the conversion matrix from the 
    // Y'Cb'Cr' color space to the R'G'B' color space.
    HRESULT GetYUVMatrix(MFVideoTransferMatrix *pMatrix)
    {
        this->CheckPointer(pMatrix, E_POINTER);
        return this->GetMediaType()->GetUINT32(MF_MT_YUV_MATRIX, (UINT32*)pMatrix);
    } 

    // Sets an enumeration representing the conversion matrix from the 
    // Y'Cb'Cr' color space to the R'G'B' color space.
    HRESULT SetYUVMatrix(MFVideoTransferMatrix nMatrix)
    {
        return this->GetMediaType()->SetUINT32(MF_MT_YUV_MATRIX, (UINT32)nMatrix);
    } 

    // 
    // The following versions return reasonable defaults if the relevant attribute is not present (zero/FALSE).
    // This is useful for making quick comparisons betweeen media types. 
    //

    MFRatio GetPixelAspectRatio() // Defaults to 1:1 (square pixels)
    {
        MFRatio PAR = { 0, 0 };
        HRESULT hr = S_OK;

        hr = MFGetAttributeRatio(this->GetMediaType(), MF_MT_PIXEL_ASPECT_RATIO, (UINT32*)&PAR.Numerator, (UINT32*)&PAR.Denominator);
        if (FAILED(hr))
        {
            PAR.Numerator = 1;
            PAR.Denominator = 1;
        }
        return PAR;
    }       

    BOOL IsPanScanEnabled() // Defaults to FALSE
    {
        return (BOOL)MFGetAttributeUINT32(this->GetMediaType(), MF_MT_PAN_SCAN_ENABLED, FALSE);
    }

    // Returns (in this order) 
    // 1. The pan/scan region, only if pan/scan mode is enabled.
    // 2. The geometric aperture.
    // 3. The entire video area.
    //HRESULT GetVideoDisplayArea(MFVideoArea *pArea)
    //{
    //    CheckPointer(pArea, E_POINTER);

    //    return MediaFoundationSamples::GetVideoDisplayArea(GetMediaType(), pArea);
    //}
};

typedef CMFVideoMediaTypeT<true>    CMFVideoMediaType;
typedef CMFVideoMediaTypeT<false>    CMFVideoMediaTypeHandle;



///////////////////////////////////////////////////////////////////////////
// CRendererEVRT
template <class T>
class CRendererEVRImpl
    : public IMFGetService
    , public IMFVideoDeviceID
    , public IMFVideoPresenter // Inherits IMFClockStateSink
    //, public IMFRateSupport
    , public IMFTopologyServiceLookupClient
    , public CAni
  
   
    //, public IMFVideoDisplayControl

    //, public IDirect3DDeviceManager9
    //, public IMFAsyncCallback
    //, public IQualProp
    //, public IEVRTrustedVideoPlugin
    //, public IMFStreamSink
{
protected:
    typedef typename CRenderBase::CAutoLock     CAutoLock_EVR;
    CComPtr<IDirect3DDevice9>                   m_pDevice9;
    
    UINT                                        m_DeviceResetToken;     // Reset token for the D3D device manager.
    CComPtr<IDirect3DDeviceManager9>            m_pDeviceManager;        // Direct3D device manager.

    CComPtr<IMFTransform>                        m_pMixer;               // The mixer.
    CComPtr<IMediaEventSink>                    m_pMediaEventSink;      // The EVR's event-sink interface.
//     CComPtr<IMFClock>                           m_pClock;

    CComPtr<IMFMediaType>                        m_pMediaType;           // Output media type

    CComPtr<IMFSample>                          m_pSample;              // samples cache


protected:
    T* GetThis() { return static_cast<T*>(this); }

public:
    TM_UNUSED_ADDREF_RELEASE    ();
    TM_BGN_INTERFACE_MAP        (IMFGetService)
        dshow_trace(TM_T("CRendererEVRT::QueryInterface:%s"), DbgFormatGuid(iid).c_str());
        TM_INTERFACE_HANDLER    (IMFGetService)
        TM_INTERFACE_HANDLER    (IMFVideoDeviceID)
        TM_INTERFACE_HANDLER    (IMFVideoPresenter) // MFCreateVideoPresenter
        TM_INTERFACE_HANDLER    (IMFTopologyServiceLookupClient) // MFCreateTopology
            //        TM_INTERFACE_MAP(T)
    TM_END_INTERFACE_MAP        ()

    IBaseFilter* GetRenderer() { return GetThis(); } //->m_pBaseFilter;

    TMBool InitRenderer(IDirect3DDevice9* pDevice)
    {
        dshow_trace_method("CRendererEVRT::InitRenderer");

        m_pDevice9 = pDevice;
        
        // Create the device manager
        DllLoad(TM_T("dxva2.dll"), LOAD_LIBRARY_SEARCH_SYSTEM32);
        DECLARE_DLL_FUNC_IMPL2("dxva2.dll", BOOL, WINAPI, DXVA2CreateDirect3DDeviceManager9,
            (UINT* pResetToken, IDirect3DDeviceManager9** ppDeviceManager));
        if (!DXVA2CreateDirect3DDeviceManager9)
            return false;
        if (FAILED(DXVA2CreateDirect3DDeviceManager9(&m_DeviceResetToken, &m_pDeviceManager)))
            return false;
        if (FAILED(m_pDeviceManager->ResetDevice(pDevice, m_DeviceResetToken)))
            return false;

        // create filter
        DbgVerifyHResult(GetThis()->m_pBaseFilter.CoCreateInstance(CLSID_EnhancedVideoRenderer, 0, CLSCTX_INPROC_SERVER));
        CComQIPtr<IMFGetService> pMFGS(GetThis()->m_pBaseFilter);
        if (pMFGS)
        {
            //CComQIPtr<IEVRFilterConfig> pConfig(pMFGS);
            //DbgVerifyHResult(pConfig->SetNumberOfStreams(3));
            
            CComPtr<IMFVideoRenderer> pMFVR;
            if (FAILED(pMFGS->GetService(MR_VIDEO_RENDER_SERVICE, IID_PPV_ARGS(&pMFVR))))
                return false;
            if (FAILED(pMFVR->InitializeRenderer(nullptr, this)))
                return false;

//             CComQIPtr<IMFMediaSink> spSink(pMFGS);
//             CComQIPtr<IMFStreamSink> spStreamSink(pMFGS);;

            //GetThis()->InitProxy();

            //CComQIPtr<IMFMediaSink> pMediaSink(pMFVR);;
            //DbgVerifyHResult(pMFGS->GetService(MR_VIDEO_RENDER_SERVICE, IID_PPV_ARGS(&pMediaSink)));
            ////pMediaSink->AddStreamSink(this);

            //CComQIPtr<IMFActivate> pActive(pMFVR);;
            //DbgVerifyHResult(pMFGS->GetService(MR_VIDEO_RENDER_SERVICE, IID_PPV_ARGS(&pActive)));
            //pMediaSink->AddStreamSink(this);


            //CComPtr<IDirect3DDeviceManager9> pDeviceManager;
            //if (FAILED(pMFGS->GetService(MR_VIDEO_RENDER_SERVICE, IID_PPV_ARGS(&pMFVR))))
            //    return;
            //if (FAILED(pDeviceManager->ResetDevice(pDevice, m_DeviceResetToken)))
            //    return;
        }
        return true;
    }

    void UninitRenderer()
    {
        dshow_trace_method("CRendererEVRT::UninitRenderer");
        m_pDeviceManager.Release();
        GetThis()->m_pBaseFilter.Release();
    }

    // IMFGetService methods
    virtual STDMETHODIMP GetService(REFGUID guidService, REFIID iid, LPVOID *ppvObject) tm_override
    {
        dshow_trace(TM_T("CRendererEVRT::GetService:(%s, %s)"), DbgFormatGuid(guidService).c_str(), DbgFormatGuid(iid).c_str());

        // The only service GUID that we support is MR_VIDEO_RENDER_SERVICE.
        if (guidService != MR_VIDEO_RENDER_SERVICE && MR_VIDEO_ACCELERATION_SERVICE != guidService)
            return MF_E_UNSUPPORTED_SERVICE;

        // First try to get the service interface from the D3DPresentEngine object.
        DbgAssert(m_pDeviceManager);
        HRESULT hr;
        DbgVerifyHResult(hr = m_pDeviceManager->QueryInterface(iid, ppvObject));
        return hr;
    }

    // IMFVideoDeviceID methods
    virtual STDMETHODIMP GetDeviceID(IID* pDeviceID)
    {
        dshow_trace_method("CRendererEVRT::GetDeviceID");
        *pDeviceID = __uuidof(IDirect3DDevice9);
        return S_OK;
    }

    // IMFVideoPresenter::IMFClockStateSink
    virtual STDMETHODIMP OnClockStart(MFTIME hnsSystemTime, LONGLONG llClockStartOffset) tm_override
    {
        dshow_trace_method("CRendererEVRT::OnClockStart");
        return S_OK;
    }

    virtual STDMETHODIMP OnClockStop(MFTIME hnsSystemTime) tm_override
    {
        dshow_trace_method("CRendererEVRT::OnClockStop");
        return S_OK;
    }

    virtual STDMETHODIMP OnClockPause(MFTIME hnsSystemTime) tm_override
    {
        dshow_trace_method("CRendererEVRT::OnClockPause");
        return S_OK;
    }

    virtual STDMETHODIMP OnClockRestart(MFTIME hnsSystemTime) tm_override
    {
        dshow_trace_method("CRendererEVRT::OnClockRestart");
        return S_OK;
    }

    virtual STDMETHODIMP OnClockSetRate(MFTIME hnsSystemTime, float flRate) tm_override
    {
        dshow_trace_method("CRendererEVRT::OnClockSetRate");
        return S_OK;
    }

    // IMFVideoPresenter
    virtual STDMETHODIMP ProcessMessage(MFVP_MESSAGE_TYPE eMessage, ULONG_PTR ulParam) tm_override
    {
        dshow_trace_method("CRendererEVRT::ProcessMessage", "(eMessage:%d, param:%x)", eMessage, ulParam);
        switch (eMessage)
        {
        case MFVP_MESSAGE_FLUSH:                    // 0. The presenter should discard any pending samples
            return GetThis()->EVROnMessageFlush();
        
        case MFVP_MESSAGE_INVALIDATEMEDIATYPE:      // 1. The mixer's output format has changed. The EVR will initiate format negotiation, as described previously
            return GetThis()->EVROnMessageInvalidateMediaType();

        case MFVP_MESSAGE_PROCESSINPUTNOTIFY:       // 2. One input stream on the mixer has received a new sample
            //m_bHaveNewFrame = true;
            return GetThis()->EVROnMessageProcessInputNotify();

        case MFVP_MESSAGE_BEGINSTREAMING:           // 3. The EVR switched from stopped to paused. The presenter should allocate resources
            return GetThis()->EVROnMessageBeginStreaming();

        case MFVP_MESSAGE_ENDSTREAMING:             // 4. The EVR switched from running or paused to stopped. The presenter should free resources
            return GetThis()->EVROnMessageEndStreaming();

        case MFVP_MESSAGE_ENDOFSTREAM:              // 5. All input streams have ended.
            //struct CAAA
            //{
            //    static void PFNCall(void* pArg)
            //    {
            //        T* pThis = (T*)pArg;
            //        pThis->SyncState(false);
            //    }
            //};
            //CallCreate(eCallTypeSync, CAAA::PFNCall, GetThis());
            return GetThis()->EVROnMessageEndOfStream();

        case MFVP_MESSAGE_STEP:                     // 6. Requests a frame step.
            return GetThis()->EVROnMessageStep((DWORD)ulParam);

        case MFVP_MESSAGE_CANCELSTEP:               // 7. Cancels a frame step
            return GetThis()->EVROnMessageCancelStep();
        }
        return S_OK;
    }

    virtual STDMETHODIMP GetCurrentMediaType(IMFVideoMediaType** ppMediaType) tm_override
    {
        dshow_trace_method("CRendererEVRT::GetCurrentMediaType");
        if (m_pMediaType)
            return m_pMediaType->QueryInterface(IID_PPV_ARGS(ppMediaType));
        return MF_E_NOT_INITIALIZED;
    }

    // IMFTopologyServiceLookupClient methods
    virtual STDMETHODIMP InitServicePointers(IMFTopologyServiceLookup *pLookup) tm_override
    {
        dshow_trace_method("CRendererEVRT::InitServicePointers");

        // Ask for the mixer. (Required.)
        DWORD dwObjectCount = 1;
        DbgVerifyHResult(pLookup->LookupService(
            MF_SERVICE_LOOKUP_GLOBAL,
            0,
            MR_VIDEO_MIXER_SERVICE,
            __uuidof(IMFTransform),
            (void**)&m_pMixer,
            &dwObjectCount
       ));

        // Ask for the EVR's event-sink interface. (Required.)
        dwObjectCount = 1;
        DbgVerifyHResult(pLookup->LookupService(
            MF_SERVICE_LOOKUP_GLOBAL,
            0,
            MR_VIDEO_RENDER_SERVICE,
            __uuidof(IMediaEventSink),
            (void**)(void**)&m_pMediaEventSink,
            &dwObjectCount
       ));

//         dwObjectCount = 1;
//         DbgVerifyHResult(pLookup->LookupService(MF_SERVICE_LOOKUP_GLOBAL, 0, MR_VIDEO_RENDER_SERVICE,
//             IID_PPV_ARGS(&m_pClock), &dwObjectCount));
// 

        return S_OK;
    }

    virtual STDMETHODIMP ReleaseServicePointers() tm_override
    {
        dshow_trace_method("CRendererEVRT::ReleaseServicePointers");
        m_pMixer.Release();
        m_pMediaEventSink.Release();
//         m_pClock.Release();
        return S_OK;
    }

    //  CAniProc
    virtual void OnAni(TMHAni tm, TMUInt, TMUInt nFrame, void* data) tm_override
    {
        if (!m_pSample || !m_pMixer)
            return;

        IGraphBuilder* pGraphBuilder = GetThis()->m_ptrGraphBuilder;
        CComQIPtr<IMediaSeeking> ptrMediaSeeking(pGraphBuilder);
        DbgAssert(ptrMediaSeeking);
        REFERENCE_TIME nPosNow = 0;
        DbgVerifyHResult(ptrMediaSeeking->GetCurrentPosition(&nPosNow));

        for (TMBool bGet = false; ; bGet = true)
        {
            LONGLONG nsSampleTime = 0, nsSampleDuration = 0;
            DbgVerifyHResult(m_pSample->GetSampleTime(&nsSampleTime));
            DbgVerifyHResult(m_pSample->GetSampleDuration(&nsSampleDuration));
            LONGLONG nsDTime = nsSampleTime + nsSampleDuration - nPosNow;
            dshow_trace("CRendererEVRT::ProcessOutput(now:%lld, sampletime:%lld, sampleDuration:%lld, dtime:%lld)", nPosNow, nsSampleTime, nsSampleDuration, nsDTime);
            if (nsDTime >= 0)
            {
                nsDTime /= 10000;
                if (nsDTime)
                {
                    dshow_trace("CRendererEVRT::OnAni::SetDelayOnce(%dms)", (int)nsDTime);
                    SetDelayOnce(nsDTime);
                }
                if (bGet)
                    GetThis()->AfterPresent();
                return;
            }

            // Now we are ready to get an output sample from the mixer. 
            MFT_OUTPUT_DATA_BUFFER dataBuffer = { 0 };
            dataBuffer.dwStreamID = 0;
            dataBuffer.pSample = m_pSample;
            dataBuffer.dwStatus = 0;

            HRESULT hr;
            DWORD dwStatus;
            //static  LONGLONG xx = 0;
            //xx += 1000;
            //m_pSample->SetSampleTime(xx);
            DbgVerifyHResult(hr = m_pMixer->ProcessOutput(0, 1, &dataBuffer, &dwStatus));
            if (dataBuffer.pEvents)
                dataBuffer.pEvents->Release();
            if (FAILED(hr))
            {
                if (SUCCEEDED(ptrMediaSeeking->GetDuration(&nsSampleDuration))) // check end of stream
                {
                    if (nsSampleDuration <= nPosNow)
                    {
                        m_pMediaEventSink->Notify(EC_COMPLETE, (LONG_PTR)S_OK, 0);
                        KillAni();
                    }

//                        GetThis()->SyncState(false);
                }
                return;
            }
            
            //#ifdef TM_DBG
            //            LONGLONG nsSampleTime, nsSampleDuration;
            //            m_pSample->GetSampleTime(&nsSampleTime);
            //            m_pSample->GetSampleDuration(&nsSampleDuration);
            //            dshow_trace("CRendererEVRT::ProcessOutput(sampletime:%lld, sampleDuration)", nsSampleTime, nsSampleDuration);
            //#endif
                        //return hr;
        }
    }

    HRESULT CheckFormat(D3DFORMAT format)
    {
        HRESULT hr;
        D3DDEVICE_CREATION_PARAMETERS params;
        if (FAILED(hr = m_pDevice9->GetCreationParameters(&params)))
            return hr;

        UINT uAdapter = params.AdapterOrdinal;
        D3DDEVTYPE type = params.DeviceType;

        CComPtr<IDirect3D9> pD3D9;
        if (FAILED(hr = m_pDevice9->GetDirect3D(&pD3D9)))
            return hr;

        D3DDISPLAYMODE mode;
        DbgVerifyHResult(pD3D9->GetAdapterDisplayMode(uAdapter, &mode));

        return pD3D9->CheckDeviceType(uAdapter, type, mode.Format, format, TRUE);
    }

    TMBool IsMediaTypeSupported(CMFVideoMediaType& videoType)
    {
        dshow_trace_method("CRendererEVRT::IsMediaTypeSupported");
        DbgAssert(videoType);

        // Reject compressed media types.
        //BOOL bCompressed = true;
        //DbgVerifyHResult(pMediaType->IsCompressedFormat(&bCompressed));
        //if (bCompressed)
        //    return false;

        //CMFVideoMediaTypeHandle videoType(pMediaType);
  //      // Validate the format.
  //      D3DFORMAT d3dFormat;
  //      if (FAILED(videoType.GetFourCC((DWORD*)&d3dFormat)))
  //          return false;

  //      // The D3DPresentEngine checks whether the format can be used as
  //      // the back-buffer format for the swap chains.
  //      dshow_trace("CRendererEVRT::IsMediaTypeSupported(%s)", GX_DEBUG_FORMAT2STR(d3dFormat));
  //      if (FAILED(CheckFormat(d3dFormat)))
  //          return false;

        // Reject interlaced formats.
        MFVideoInterlaceMode InterlaceMode;
        if (FAILED(videoType.GetInterlaceMode(&InterlaceMode)))
            return false;
        if (InterlaceMode != MFVideoInterlace_Progressive)
            return false;

        //return SUCCEEDED(MakeVideoSample(pMediaType));
            
        return true;
    }

    TMBool CreateOptimalVideoType(IMFMediaType* pProposedType, IMFMediaType **ppOptimalType)
    {
        /*
        HRESULT hr;
        // Helper object to manipulate the optimal type.
        // Clone the proposed type.
        CComPtr<IMFMediaType> pOptimalMediaType;
        if (FAILED(fnMFCreateMediaType(&pOptimalMediaType)))
            return false;
        if (FAILED(pProposedType->CopyAllItems(pOptimalMediaType)))
            return false;

        // Modify the new type.

        // For purposes of this SDK sample, we assume 
        // 1) The monitor's pixels are square.
        // 2) The presenter always preserves the pixel aspect ratio.

        // Set the pixel aspect ratio (PAR) to 1:1 (see assumption #1, above)
        if (FAILED(pOptimalMediaType->SetPixelAspectRatio(1, 1)))
            return false;

        // Get the output rectangle.
        RECT rcOutput = m_pD3DPresentEngine->GetDestinationRect();
        if (IsRectEmpty(&rcOutput))
        {
            // Calculate the output rectangle based on the media type.
            //CHECK_HR(hr = CalculateOutputRectangle(pProposedType, &rcOutput));
        }

        // Set the extended color information: Use BT.709 
        //DbgVerifyHResult(pOptimalMediaType->SetYUVMatrix(MFVideoTransferMatrix_BT709));
        //DbgVerifyHResult(pOptimalMediaType->SetTransferFunction(MFVideoTransFunc_709));
        //DbgVerifyHResult(pOptimalMediaType->SetVideoPrimaries(MFVideoPrimaries_BT709));
        //DbgVerifyHResult(pOptimalMediaType->SetVideoNominalRange(MFNominalRange_16_235));
        //DbgVerifyHResult(pOptimalMediaType->SetVideoLighting(MFVideoLighting_dim));

        // Set the target rect dimensions. 
        DbgVerifyHResult(hr = pOptimalMediaType->SetFrameDimensions(rcOutput.right, rcOutput.bottom));

        // Set the geometric aperture, and disable pan/scan.
        DbgVerifyHResult(hr = pOptimalMediaType->SetPanScanEnabled(FALSE));

        MFVideoArea displayArea = { 0, 0, rcOutput.right, rcOutput.bottom };
        DbgVerifyHResult(hr = pOptimalMediaType->SetGeometricAperture(displayArea));

        // Set the pan/scan aperture and the minimum display aperture. We don't care
        // about them per se, but the mixer will reject the type if these exceed the 
        // frame dimentions.
        DbgVerifyHResult(hr = pOptimalMediaType->SetPanScanAperture(displayArea));
        DbgVerifyHResult(hr = pOptimalMediaType->SetMinDisplayAperture(displayArea));

        // Return the pointer to the caller.
        *ppOptimalType = pOptimalMediaType.Detach();
        */
        return true;
    }

    void ReleaseResources()
    {
//        m_SamplePool.Clear();
//        m_pD3DPresentEngine->ReleaseResources();
    }

    TMBool SetMediaType(IMFMediaType *pMediaType)
    {
        // Note: pMediaType can be NULL (to clear the type)
        /*
        // Clearing the media type is allowed in any state (including shutdown).
        if (pMediaType == NULL)
        {
            SAFE_RELEASE(m_pMediaType);
            ReleaseResources();
            return S_OK;
        }

        HRESULT hr = S_OK;
        MFRatio fps = { 0, 0 };
        VideoSampleList sampleQueue;


        IMFSample *pSample = NULL;

        // Cannot set the media type after shutdown.
        CHECK_HR(hr = CheckShutdown());

        // Check if the new type is actually different.
        // Note: This function safely handles NULL input parameters.
        DWORD dwFlags = 0;
        if (SUCCEEDED(pMediaType->IsEqual(m_pMediaType, &dwFlags)))
            return true;

        // We're really changing the type. First get rid of the old type.
        SAFE_RELEASE(m_pMediaType);
        ReleaseResources();

        // Initialize the presenter engine with the new media type.
        // The presenter engine allocates the samples. 

        CHECK_HR(hr = m_pD3DPresentEngine->CreateVideoSamples(pMediaType, sampleQueue));

        // Mark each sample with our token counter. If this batch of samples becomes
        // invalid, we increment the counter, so that we know they should be discarded. 
        for (VideoSampleList::POSITION pos = sampleQueue.FrontPosition();
            pos != sampleQueue.EndPosition();
            pos = sampleQueue.Next(pos))
        {
            CHECK_HR(hr = sampleQueue.GetItemPos(pos, &pSample));
            CHECK_HR(hr = pSample->SetUINT32(MFSamplePresenter_SampleCounter, m_TokenCounter));

            SAFE_RELEASE(pSample);
        }


        // Add the samples to the sample pool.
        CHECK_HR(hr = m_SamplePool.Initialize(sampleQueue));

        // Set the frame rate on the scheduler. 
        if (SUCCEEDED(GetFrameRate(pMediaType, &fps)) && (fps.Numerator != 0) && (fps.Denominator != 0))
        {
            m_scheduler.SetFrameRate(fps);
        }
        else
        {
            // NOTE: The mixer's proposed type might not have a frame rate, in which case 
            // we'll use an arbitary default. (Although it's unlikely the video source
            // does not have a frame rate.)
            m_scheduler.SetFrameRate(g_DefaultFrameRate);
        }

        // Store the media type.
        assert(pMediaType != NULL);
        m_pMediaType = pMediaType;
        m_pMediaType->AddRef();

    done:
        if (FAILED(hr))
        {
            ReleaseResources();
        }
        */
        return 0;
    }

    int GetMediaTypeMerit(CMFVideoMediaType& videoType)
    {
        DWORD Format = 0;
        DbgVerifyHResult(videoType.GetFourCC((DWORD*)&Format));
        switch (Format) 
        {
        case gxMakeFourCC1('AI44'): return 38;   // Palettized, 4:4:4
        case gxMakeFourCC1('YVU9'): return 37;   // 8-bit, 16:1:1
        case gxMakeFourCC1('NV11'): return 36;   // 8-bit, 4:1:1
        case gxMakeFourCC1('Y41P'): return 35;
        case gxMakeFourCC1('Y41T'): return 34;
        case gxMakeFourCC1('P016'): return 33;   // 4:2:0
        case gxMakeFourCC1('P010'): return 32;
        case gxMakeFourCC1('IMC1'): return 31;
        case gxMakeFourCC1('IMC3'): return 30;
        case gxMakeFourCC1('IMC2'): return 29;
        case gxMakeFourCC1('IMC4'): return 28;
        case gxMakeFourCC1('YV12'): return 27;
        case gxMakeFourCC1('NV12'): return 26;
        case gxMakeFourCC1('I420'): return 25;
        case gxMakeFourCC1('IYUV'): return 24;
        case gxMakeFourCC1('Y216'): return 23;   // 4:2:2
        case gxMakeFourCC1('v216'): return 22;
        case gxMakeFourCC1('P216'): return 21;
        case gxMakeFourCC1('Y210'): return 20;
        case gxMakeFourCC1('v210'): return 19;
        case gxMakeFourCC1('P210'): return 18;
        case gxMakeFourCC1('YUY2'): return 17;
        case gxMakeFourCC1('UYVY'): return 16;
        case gxMakeFourCC1('Y42T'): return 15;
        case gxMakeFourCC1('YVYU'): return 14;
        case gxMakeFourCC1('Y416'): return 13;   // 4:4:4
        case gxMakeFourCC1('Y410'): return 12;
        case gxMakeFourCC1('v410'): return 11;
        case gxMakeFourCC1('AYUV'): return 10;
        case D3DFMT_X8R8G8B8:       return 2;

        case D3DFMT_A8R8G8B8:   // an accepted format, but fails on most surface types
        case D3DFMT_A8B8G8R8:
        case D3DFMT_X8B8G8R8:
        case D3DFMT_R8G8B8:
        case D3DFMT_R5G6B5:
        case D3DFMT_X1R5G5B5:
        case D3DFMT_A1R5G5B5:
        case D3DFMT_A4R4G4B4:
        case D3DFMT_R3G3B2:
        case D3DFMT_A8R3G3B2:
        case D3DFMT_X4R4G4B4:
        case D3DFMT_A8P8:
        case D3DFMT_P8:             return 1;
        case 0:                     return 0;
        default:                    return 3;
        }
    }

    HRESULT CreateOptimalOutputType(IMFMediaType* pFromType, IMFMediaType* pCurrentType, IMFMediaType** ppType)
    {
        HRESULT hr;
        IMFMediaType* pOptimalMediaType;
        if (FAILED(hr = MFCreateMediaType(&pOptimalMediaType)))
            return hr;
        if (FAILED(hr = pFromType->CopyAllItems(pOptimalMediaType)))
            return hr;

        const GUID colorAttributes[] = {
            MF_MT_VIDEO_LIGHTING,
            MF_MT_VIDEO_PRIMARIES,
            MF_MT_TRANSFER_FUNCTION,
            MF_MT_YUV_MATRIX,
            MF_MT_VIDEO_CHROMA_SITING
        };

        for (int i = 0; i < tm_countof(colorAttributes); ++i)
        {
            PROPVARIANT val;
            HRESULT hr = pFromType->GetItem(colorAttributes[i], &val);

            if (SUCCEEDED(hr))
            {
                hr = pOptimalMediaType->SetItem(colorAttributes[i], val);
                PropVariantClear(&val);
            }
            else if (hr == MF_E_ATTRIBUTENOTFOUND)
                hr = pOptimalMediaType->DeleteItem(colorAttributes[i]);
        }

        pOptimalMediaType->SetUINT32(MF_MT_PAN_SCAN_ENABLED, 0);

        /*
        const CRenderersSettings& r = GetRenderersSettings();

        UINT32 nominalRange;
        if (SUCCEEDED(pCurrentType->GetUINT32(MF_MT_VIDEO_NOMINAL_RANGE, &nominalRange))
            && nominalRange == MFNominalRange_0_255) {
            // EVR mixer always assume 16-235 input. To ensure that luminance range won't be expanded we requests 16-235 also on output.
            // Request 16-235 to ensure untouched luminance range on output. It is the only way to pass 0-255 without changes.
            nominalRange = MFNominalRange_16_235;
            m_LastSetOutputRange = -1; // -1 to prevent renegotiations because of different value than this in settings.
        }
        else {
            nominalRange = (r.m_AdvRendSets.iEVROutputRange == 1) ? MFNominalRange_16_235 : MFNominalRange_0_255;
            m_LastSetOutputRange = r.m_AdvRendSets.iEVROutputRange;
        }
        pOptimalMediaType->SetUINT32(MF_MT_VIDEO_NOMINAL_RANGE, nominalRange);

        ULARGE_INTEGER ui64Size;
        pOptimalMediaType->GetUINT64(MF_MT_FRAME_SIZE, &ui64Size.QuadPart);

        CSize videoSize((LONG)ui64Size.HighPart, (LONG)ui64Size.LowPart);
        MFVideoArea Area = MakeArea(0, 0, videoSize.cx, videoSize.cy);
        pOptimalMediaType->SetBlob(MF_MT_GEOMETRIC_APERTURE, (UINT8*)&Area, sizeof(MFVideoArea));

        ULARGE_INTEGER ui64AspectRatio;
        pOptimalMediaType->GetUINT64(MF_MT_PIXEL_ASPECT_RATIO, &ui64AspectRatio.QuadPart);

        UINT64 ui64ARx = UINT64(ui64AspectRatio.HighPart) * ui64Size.HighPart;
        UINT64 ui64ARy = UINT64(ui64AspectRatio.LowPart)  * ui64Size.LowPart;
        UINT64 gcd = GCD(ui64ARx, ui64ARy);
        if (gcd > 1) {
            ui64ARx /= gcd;
            ui64ARy /= gcd;
        }

        CSize aspectRatio((LONG)ui64ARx, (LONG)ui64ARy);
        if (videoSize != m_nativeVideoSize || aspectRatio != m_aspectRatio) {
            SetVideoSize(videoSize, aspectRatio);

            // Notify the graph about the change
            if (m_pSink) {
                m_pSink->Notify(EC_VIDEO_SIZE_CHANGED, MAKELPARAM(m_nativeVideoSize.cx, m_nativeVideoSize.cy), 0);
            }
        }
        */

        *ppType = pOptimalMediaType;
        //(*ppType)->AddRef();

        return hr;
    }

    HRESULT RenegotiateMediaType()
    {
        DbgAssert(m_pMixer);

#ifdef TM_DBG
        tm_for (DWORD iTypeIndex = 0; iTypeIndex < 1000; ++iTypeIndex) //!bFoundMediaType && (hr != MF_E_NO_MORE_TYPES))
        {
            CMFVideoMediaType videoType;
            if (FAILED(m_pMixer->GetOutputAvailableType(0, iTypeIndex, &videoType)))
            {
                iTypeIndex = 1000;
                DbgVerifyHResult(m_pMixer->GetInputCurrentType(0, &videoType));
            }
            GUID guidSubType;
            videoType.GetSubType(&guidSubType);

            BOOL bCompressed = true;
            DbgVerifyHResult(videoType.IsCompressedFormat(&bCompressed));
            MFVideoInterlaceMode InterlaceMode = MFVideoInterlace_Unknown;
            DbgVerifyHResult(videoType.GetInterlaceMode(&InterlaceMode));
            MFNominalRange nominalRange;
            DbgVerifyHResult(videoType.GetVideoNominalRange(&nominalRange));
            dshow_trace("CRendererEVRT::RenegotiateMediaType(%d, %s, %s, bCompressed:%d, InterlaceMode:%d, nominalRange:%d)",
                iTypeIndex, TM_T2A(DbgFormatGuid(guidSubType)), GX_DEBUG_FORMAT2STR(guidSubType.Data1), bCompressed, InterlaceMode, nominalRange);
        }
#endif

        CComPtr<IMFMediaType> pCurrentType;
        DbgVerifyHResult(m_pMixer->GetInputCurrentType(0, &pCurrentType));

        // Loop through all of the mixer's proposed output types.
        typedef CMultiMap<int, CComPtr<IMFMediaType> > CTypeList;
        CTypeList aTypes;
        HRESULT hr;
        for (DWORD iTypeIndex = 0; ; ++iTypeIndex) //!bFoundMediaType && (hr != MF_E_NO_MORE_TYPES))
        {
            CMFVideoMediaType pMixerType;
            // Step 1. Get the next media type supported by mixer.
            if (FAILED(hr = m_pMixer->GetOutputAvailableType(0, iTypeIndex, &pMixerType)))
                break;

            // From now on, if anything in this loop fails, try the next type,
            // until we succeed or the mixer runs out of types.

            // Step 2. Check if we support this media type. 
            // Note: None of the modifications that we make later in CreateOptimalVideoType
            // will affect the suitability of the type, at least for us. (Possibly for the mixer.)
            if (!IsMediaTypeSupported(pMixerType))
                continue;

            int nMerit = GetMediaTypeMerit(pMixerType);
            if (nMerit >= 0)
                aTypes.insert(1000 - nMerit, CComPtr<IMFMediaType>(pMixerType));
        }

        for (CTypeList::recorder rec(aTypes); rec; ++rec)//++rec)
        {
            IMFMediaType* pMixerType = rec.get_data();

            // Step 3. Adjust the mixer's type to match our requirements.
            CComPtr<IMFMediaType> pOptimalType;
            if (FAILED(CreateOptimalOutputType(pMixerType, pCurrentType, &pOptimalType)))
                continue;

#ifdef TM_DBG
            GUID guidSubType;
            CMFVideoMediaTypeHandle videoType(pMixerType);
            videoType.GetSubType(&guidSubType);
            dshow_trace("CRendererEVRT::RenegotiateMediaType::SetOutputType(%s, %s)", TM_T2A(DbgFormatGuid(guidSubType)), GX_DEBUG_FORMAT2STR(guidSubType.Data1));
#endif
                        // Step 4. Check if the mixer will accept this media type.
            if (FAILED(hr = m_pMixer->SetOutputType(0, pOptimalType, MFT_SET_TYPE_TEST_ONLY)))
                continue;

            if (FAILED(MakeVideoSample(pMixerType)))
                continue;


            // Step 5. Try to set the media type on ourselves.
            //if (SUCCEEDED(hr))
            //{
            //    hr = SetMediaType(pOptimalType);
            //}

            // Step 6. Set output media type on mixer.
            if (FAILED(hr = m_pMixer->SetOutputType(0, pOptimalType, 0)))
                continue;

            break;

            //assert(SUCCEEDED(hr)); // This should succeed unless the MFT lied in the previous call.

            //// If something went wrong, clear the media type.
            //if (FAILED(hr))
            //{
            //    SetMediaType(NULL);
            //}

            //if (SUCCEEDED(hr))
            //{
            //    bFoundMediaType = TRUE;
            //}
        }

        return hr;

        return 0;
    }

//    HRESULT RenegotiateMediaType()
//    {
//        DbgAssert(m_pMixer);
//
//#ifdef TM_DBG
//        tm_for (DWORD iTypeIndex = 0; iTypeIndex < 1000; ++iTypeIndex) //!bFoundMediaType && (hr != MF_E_NO_MORE_TYPES))
//        {
//            CComPtr<IMFMediaType> pMixerType;
//            if (FAILED(m_pMixer->GetOutputAvailableType(0, iTypeIndex, &pMixerType)))
//            {
//                iTypeIndex = 1000;
//                DbgVerifyHResult(m_pMixer->GetInputCurrentType(0, &pMixerType));
//            }
//            CMFVideoMediaTypeHandle videoType(pMixerType);
//            GUID guidSubType;
//            videoType.GetSubType(&guidSubType);
//            dshow_trace("CRendererEVRT::RenegotiateMediaType(%d, %s, %s)", iTypeIndex, TM_T2A(DbgFormatGuid(guidSubType)), GX_DEBUG_FORMAT2STR(guidSubType.Data1));
//        }
//#endif
//        return S_OK;
//
//        // Loop through all of the mixer's proposed output types.
//        CComPtr<IMFMediaType> pMixerType;
//        CComPtr<IMFMediaType> pOptimalType;
//        HRESULT hr;
//        for (DWORD iTypeIndex = 0; ; ++iTypeIndex) //!bFoundMediaType && (hr != MF_E_NO_MORE_TYPES))
//        {
//            // Step 1. Get the next media type supported by mixer.
//            if (FAILED(hr = m_pMixer->GetOutputAvailableType(0, iTypeIndex, &pMixerType)))
//                break;
//
//            // From now on, if anything in this loop fails, try the next type,
//            // until we succeed or the mixer runs out of types.
//
//            // Step 2. Check if we support this media type. 
//            // Note: None of the modifications that we make later in CreateOptimalVideoType
//            // will affect the suitability of the type, at least for us. (Possibly for the mixer.)
//            if (!IsMediaTypeSupported(pMixerType))
//                continue;
//
//            // Step 3. Adjust the mixer's type to match our requirements.
//            pOptimalType = pMixerType;
////            if (!CreateOptimalVideoType(pMixerType, &pOptimalType))
////                continue;
//
//            // Step 4. Check if the mixer will accept this media type.
//            if (FAILED(hr = m_pMixer->SetOutputType(0, pOptimalType, MFT_SET_TYPE_TEST_ONLY)))
//                continue;
//
//            // Step 5. Try to set the media type on ourselves.
//            //if (SUCCEEDED(hr))
//            //{
//            //    hr = SetMediaType(pOptimalType);
//            //}
//
//            // Step 6. Set output media type on mixer.
//            if (FAILED(hr = m_pMixer->SetOutputType(0, pOptimalType, 0)))
//                continue;
//
//            break;
//
//            //assert(SUCCEEDED(hr)); // This should succeed unless the MFT lied in the previous call.
//
//            //// If something went wrong, clear the media type.
//            //if (FAILED(hr))
//            //{
//            //    SetMediaType(NULL);
//            //}
//
//            //if (SUCCEEDED(hr))
//            //{
//            //    bFoundMediaType = TRUE;
//            //}
//        }
//
//        return hr;
//        
//        return 0;
//    }

     HRESULT MakeVideoSample(IMFMediaType *pFormat)
    {
        DbgAssert(pFormat);
        m_pSample.Release();
        HRESULT hr;

        CMFVideoMediaTypeHandle videoType(pFormat);
        UINT32 width, height;
        D3DFORMAT d3dFormat;
        if (FAILED(hr = videoType.GetFrameDimensions(&width, &height)))
            return hr;
        if (FAILED(hr = videoType.GetFourCC((DWORD*)&d3dFormat)))
            return hr;

        CComQIPtr<IDirect3DSurface9> pSurface;
//        if (FAILED(hr = GetThis()->EVRCreateBuffer(width, height, dxD3DFormat2GXFormat(d3dFormat), pSurface)))
        if (FAILED(hr = GetThis()->EVRCreateBuffer(width, height, eGXFormat32X, pSurface)))
            return hr;

        return MFCreateVideoSampleFromSurface(pSurface, &m_pSample);
    }

    void OnPlaying(bool bPlaying)
    {
        if (!bPlaying)
            KillAni();
        else
            SetAni(10);
    }


    // need class T impl
    STDMETHODIMP EVROnMessageFlush() { return S_OK; }
    STDMETHODIMP EVROnMessageInvalidateMediaType() { return RenegotiateMediaType(); } 
    STDMETHODIMP EVROnMessageProcessInputNotify() { return S_OK;  } //return ProcessOutput();
    STDMETHODIMP EVROnMessageBeginStreaming() { return S_OK; }
    STDMETHODIMP EVROnMessageEndStreaming() { return S_OK; }
    STDMETHODIMP EVROnMessageEndOfStream() { return S_OK; }
    STDMETHODIMP EVROnMessageStep(DWORD cSteps) { return S_OK; }
    STDMETHODIMP EVROnMessageCancelStep() { return S_OK; }

    STDMETHODIMP EVRCreateBuffer(int w, int h, D3DFORMAT d3dFormat, CComQIPtr<IDirect3DSurface9>& ptrSurface) { return E_FAIL; }
};



TM_END_NAMESPACE

#else

TM_BGN_NAMESPACE

template <class T>
class CRendererEVRImpl
{
public:
    IBaseFilter* GetRenderer() { return 0; } //->m_pBaseFilter;
    
    TMBool InitRenderer(IDirect3DDevice9* pDevice) { return false; }
    void UninitRenderer() {}
    void ProcessOutput(const void*) {}
    void OnPlaying(bool bPlaying) {}

};


TM_END_NAMESPACE

#endif // #if TM_MSC >= 1500


#endif // TMDSHOW_H

