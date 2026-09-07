/*
 *  @file
 *  @brief  video support for windows
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2017.7.17
 *
 *  Copyright (C) 2017 miragekiller
 */

#include "uivideo.h"
#include "uiwindow_win.h"

// html5: console.log(stream) && console.log(window.URL.createObjectURL(stream)):
// LocalMediaStream { id: "{d1f12f39-0b64-4627-b1c4-77b0d30aea…", active: true, onaddtrack: null, currentTime: 0 }
// blob:null/b8a95fc4-3f37-482a-b28b-98216462fde5

// Intel(R) HD Graphics(ver 10.18.15.4256) support YUY2 D3D9 texture, but display error.


TM_BGN_NAMESPACE

static EGXFormat _CTuiVideoRenderer_Video2GXFmt(const GUID& type)
{
    if (type == MEDIASUBTYPE_RGB8)
        return eGXFormatPAL8;
    if (type == MEDIASUBTYPE_RGB565)
        return eGXFormatR5G6B5;
    if (type == MEDIASUBTYPE_RGB24)
        return eGXFormat24;
    if (type == MEDIASUBTYPE_RGB32)
        return eGXFormat32X;
    if (type == MEDIASUBTYPE_ARGB32)
        return eGXFormat32A;
    return (EGXFormat)type.Data1;
}

struct _TextureNormal
{
    inline static void Create(CRefPtr<IGXTexture>& tex, long w, long h, EGXFormat fmt)
    {
        DbgAssert(!tex);
        _gxApp->TextureCreateIndirect(&tex, w, h, fmt);
        //        txt.CreateIndirect(w, h, fmt);
        //txt.CreateRenderTarget(w, h, fmt);
    }
    
    inline static TMBool IsValid(const GUID& guid)
    {
        return true;
    }
    
    inline static void ResetData(IGXTexture* tex, BYTE* pBits)
    {
        DbgAssert(tex);
        
        GXDibData data;
        data.bits = pBits;
        
        EGXFormat fmt = tex->GetFormat();
        int bpp = gxFormat2BPP(fmt);
        if (!bpp)
        { // nv12
            data.flag = 0;
            data.wb = tex->GetWidth();
        }
        else
        {
            data.flag = GXDibData::eBottomUp;
            data.wb = DWORD_WBYTES(bpp * tex->GetWidth());
        }
        tex->ResetAsObject(&data, eGXResetCopyDib);
    }
};

//typedef _TextureVideo _TextureNow;
typedef _TextureNormal _TextureNow;

///////////////////////////////////////////////////////////////////////////////
// CRenderNormal
// IPin
HRESULT STDMETHODCALLTYPE CRenderNormal::ReceiveConnection(IPin *pConnector, const AM_MEDIA_TYPE *pmt)
{
    dshow_trace_method(TM_T("CTuiVideoImpl::ReceiveConnection"), TM_T("(pConnector:%s, %s - %s - %s)"),
        DS_DEBUG_PIN2STR(pConnector), DbgFormatGuid(pmt->majortype).c_str(),
        DbgFormatGuid(pmt->formattype).c_str(), DbgFormatGuid(pmt->subtype).c_str());
    
    if (pmt->majortype != MEDIATYPE_Video)
        return E_INVALIDARG;
    
    BITMAPINFOHEADER* bmiHeader;
    if (pmt->formattype == FORMAT_VideoInfo2 || pmt->formattype == FORMAT_MPEG2Video)
        bmiHeader = &((VIDEOINFOHEADER2*)pmt->pbFormat)->bmiHeader;
    else if (pmt->formattype == FORMAT_VideoInfo || pmt->formattype == FORMAT_MPEGVideo)
        bmiHeader = &((VIDEOINFOHEADER*)pmt->pbFormat)->bmiHeader;
    else
        return E_INVALIDARG;
    
    EGXFormat fmt = _CTuiVideoRenderer_Video2GXFmt(pmt->subtype);
    dshow_trace("CTuiVideoImpl::ReceiveConnection(fmt:%s, compress:%s)", GX_DEBUG_FORMAT2STR(fmt), GX_DEBUG_FORMAT2STR(bmiHeader->biCompression));
    if (!_TextureNow::IsValid(pmt->subtype))
        return DDERR_INVALIDPIXELFORMAT;
    
    CRenderBase::CAutoLock lock(m_pWnd->m_critical);
    m_pWnd->ReCreateTexture(bmiHeader->biWidth, bmiHeader->biHeight, fmt);
    if (!m_pWnd->m_txtVideo)
        return DDERR_INVALIDPIXELFORMAT;
    
    HRESULT hr;
    DbgVerifyHResult(hr = m_pPin->ReceiveConnection(pConnector, pmt));
    return S_OK;
}

// IMemInputPin
HRESULT STDMETHODCALLTYPE CRenderNormal::Receive(IMediaSample *pSample)
{
    dshow_trace("CTuiVideoImpl::Receive(%p)", pSample);
    //    pSample->AddRef();
    
    {
        CRenderBase::CAutoLock lock(m_pWnd->m_critical);
        SyncTexture(pSample);
        m_pWnd->AfterPresent();
    }
    
    return m_pMemInputPin->Receive(pSample);
}

// camera+YUY2 will call the function
HRESULT STDMETHODCALLTYPE CRenderNormal::ReceiveMultiple(IMediaSample **pSamples, long nSamples, long *nSamplesProcessed)
{
    dshow_trace_method("CTuiVideoImpl::ReceiveMultiple");
    DbgAssert(nSamples > 0);
    HRESULT hr = Receive(pSamples[nSamples - 1]);
    *nSamplesProcessed = nSamples;
    return hr;
}

void CRenderNormal::SyncTexture(IMediaSample *pSample)
{
    if (!m_pWnd->m_txtVideo)
        return;
    
    BYTE * pBits;
    DbgVerifyHResult(pSample->GetPointer(&pBits));
    _TextureNow::ResetData(m_pWnd->m_txtVideo, pBits);
}


///////////////////////////////////////////////////////////////////////////////
// CVideoGB
CVideoGB::CVideoGB(CTuiVideoImpl* pWnd)
    : m_hEvent(0)
    , m_pWnd(pWnd)
{
    this->Create();
}

CVideoGB::~CVideoGB()
{
    DbgAssert(!m_ptrGraphBuilder);
    DbgAssert(!m_ptrCapture);
}

void CVideoGB::WaitMessage()
{
    for(;;)
    {
        if (!m_hEvent)
        {
            CMessageThreadT<CVideoGB>::WaitMessage();
            return;
        }

        DWORD result = MsgWaitForMultipleObjects(1, &m_hEvent, false, INFINITE, QS_ALLINPUT);
        if(result != WAIT_OBJECT_0)
            return;
        NotifyEvent();
    }
}

TMResult CVideoGB::WndProc(TMUInt uMsg, TMWParam wParam, TMLParam lParam)
{
    switch (uMsg)
    {
    case WM_RENDERFILE:
        {
            OleInitialize(0);
            RERENDERFILE* rf = (RERENDERFILE*)lParam;
            if (!RawRenderFile(m_pWnd))
                return false;
            if (rf)
            {
                RawSetState(rf->fs);
                RawSetPositions(rf->tm);
                RawPutVolume(rf->mute, rf->vol);
            }
            else
            {
                RawPause();
                RawPutVolume(m_pWnd->Is_VS_MUTED(), m_pWnd->m_volume);
            }
        }
        return true;

    case WM_CLOSEEXIT:
        if (lParam)
        {
            RERENDERFILE* rf = (RERENDERFILE*)lParam;
            rf->fs = RawGetState();
            rf->tm = RawGetPositions();
        }
        if (m_ptrCapture)
            capture_class::DetachVideo();
        else if (m_ptrGraphBuilder)
            video_class::DetachVideo();
        if (m_ptrGraphBuilder)
            RawClose();
        this->Exit();
        return 0;

    case VM_GETENDED:
        {
            CComQIPtr<IMediaSeeking> ptr(m_ptrGraphBuilder);
            if (ptr)
            {
                REFERENCE_TIME nPos1 = 0, nPos2 = 0;
                DbgVerifyHResult(ptr->GetPositions(&nPos1, &nPos2));
                return nPos1 == nPos2;
            }
        }
        return true;

    case VM_SETVOLUME: return this->RawPutVolume(wParam, *(float*)lParam);
    case VM_GETDURATION: return RawGetDuratione(*((TMUInt64*)lParam), 100);
    case VM_SETTIME: return RawSetPositions(*(TMUInt64*)lParam / 100);
    case VM_GETTIME: return *(TMUInt64*)lParam = RawGetPositions() * 100, true;
    case VM_PLAY: return RawRun();
    case VM_PAUSE: return RawPause();
    case VM_GETVIDEOSIZE: return RawGetSize(*((TMSize*)lParam), wParam && m_pWnd->Is_VS_X2Y());
    }

    return 0;
}

TMBool CVideoGB::RawRenderFile(CTuiVideoImpl* pWnd)
{
    DbgAssert(!pWnd->IsLifeInvalid());
    DbgAssert(!m_ptrGraphBuilder);
    DbgAssert(!m_ptrCapture);

    CComPtr<IBaseFilter> pRender(new CRenderNormal(m_pWnd));// m_pMemInputPin ? normal_renderer::GetRenderer() : D3D_renderer::GetRenderer();
    if (pWnd->m_ptrMoniker)
    {
        capture_class::CreateVideo(pRender);
        this->Open(pWnd->m_ptrMoniker, 0, pRender);
        if (!this->RawRun())
            return false;
    }
    else
    {
        video_class::CreateVideo(pRender);//, m_hNotify, pWnd, WM_GRAPHNOTIFY);
        /*
        //    LPOLESTR pFLVDllSource = (LPOLESTR)OLESTR("FlvSource.dll");
        // //     LPOLESTR pFLVDllSource = (LPOLESTR)OLESTR("E:\\TOOLS\\播放器\\暴风影音\\Core\\codecs\\splitter.ax");
        // //     LPOLESTR pFLVDllSource = (LPOLESTR)OLESTR("E:\\TOOLS\\播放器\\QQ影音\\复件 RealMediaSplitter.ax.dll");
        //    
        //    if (_CTuiVideoImpl_RenderFilter(m_ptrGraphBuilder, pFLVDllSource, m_strFile))
        //        return true;
        */
        // open file
        HRESULT hr;
        DbgVerifyHResult(hr = m_ptrGraphBuilder->RenderFile(pWnd->m_strFile, 0));
        if (FAILED(hr))
            return false;
    }
    
    CComQIPtr<IMediaSeeking> ptrMediaSeeking(m_ptrGraphBuilder);
    if (ptrMediaSeeking)
        DbgVerifyHResult(ptrMediaSeeking->SetTimeFormat(&TIME_FORMAT_MEDIA_TIME));

    CComQIPtr<IMediaEvent> ptrEvent(m_ptrGraphBuilder);
    ptrEvent->GetEventHandle((OAEVENT*)&m_hEvent);

    return true;
}

void CVideoGB::NotifyEvent()
{
    dshow_trace_method("CVideoGB::NotifyEvent");
    CComQIPtr<IMediaEvent> ptrEvent(m_ptrGraphBuilder);
    if (!ptrEvent)
        return;
    long evCode;
    LONG_PTR param1, param2;
    while (SUCCEEDED(ptrEvent->GetEvent(&evCode, &param1, &param2, 0)))
    {
        dshow_trace("CTuiVideoImpl::NotifyEvent(%x)", evCode);
        ptrEvent->FreeEventParams(evCode, param1, param2);
        if (evCode == EC_COMPLETE || evCode == EC_USERABORT || evCode == EC_ERRORABORT)
            RawPause();
        m_pWnd->Notify(evCode, true); // EC_COMPLETE, EC_USERABORT, EC_ERRORABORT
    }
}

CVideoGB* CVideoGB::VideoCreate(CTuiVideoImpl* pWnd, RERENDERFILE* rr)
{
    CVideoGB* p = new CVideoGB(pWnd);
    if (p->SendMessage(WM_RENDERFILE, 0, (TMLParam)rr))
        return p;
    VideoDestroy(p, 0);
    return 0;
}

void CVideoGB::VideoDestroy(CVideoGB*& p, RERENDERFILE* rf)
{
    DebugCheckNoMessageLoop();
    if (p)
    {
        p->SendMessage(WM_CLOSEEXIT, 0, (TMLParam)rf);
        p->Join();
        delete p;
        p = 0;
    }
}


/////////////////////////////////////////////////////////
// CTuiVideoImpl
CTuiVideoImpl::CTuiVideoImpl()
    : m_volume(1.0f)
    , m_video(0)
{
}

CTuiVideoImpl::~CTuiVideoImpl()
{
    DbgAssert(!m_video);
}

TMResult CTuiVideoImpl::OnCreate(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    dshow_trace_method("CTuiVideoImpl::OnCreate");
    bHandled = false;
    if (Edit_IsObjectInDocMode(this))
        return 0;
//     ChangeDevice();
    return 0;
}

TMResult CTuiVideoImpl::OnRenderChanged(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    DbgAssert(IC_IsCreatorThread());
    if (!m_video)
        return false;
        
    // reload video
    RERENDERFILE rf = { State_Stopped, 0, Is_VS_MUTED(), m_volume };
    CVideoGB::VideoDestroy(m_video, &rf);
    DbgAssert(!m_video);

    return DoLoad(&rf);
}

TMResult CTuiVideoImpl::OnSetSrc(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_strFile = TMCStr(lParam);
    m_ptrMoniker.Release();
    return true;
}

TMResult CTuiVideoImpl::OnSetSrcM(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_strFile.clear();
    m_ptrMoniker = (IMoniker*)lParam;
    return true;
}

TMResult CTuiVideoImpl::OnGetSrc(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    *(CStrPtr*)lParam = m_strFile;
    return true;
}

TMResult CTuiVideoImpl::OnLoad(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    DbgAssert(!IsLifeInvalid());
    CVideoGB::VideoDestroy(m_video, 0);
    return DoLoad(0);
}

TMResult CTuiVideoImpl::OnPlay(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    DbgAssert(!IsLifeInvalid());
    if (!m_video && !(m_video = CVideoGB::VideoCreate(this, 0)))
        return false;
    dshow_trace_method("CTuiVideoImpl::DoPlay");
    TMBool b = m_video->SendMessage(VM_PLAY);
    if (b)
        SyncState(true);
    return b;
}

TMResult CTuiVideoImpl::OnPause(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    dshow_trace_method("CTuiVideoImpl::OnPause");
    if (!m_video)
        return false;
    TMBool b = m_video->SendMessage(VM_PAUSE);
    if (b)
        SyncState(false);
    return b;
}

TMResult CTuiVideoImpl::OnSetVolume(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    dshow_trace_method("CTuiVideoImpl::OnSetVolume");
    m_volume = *(float*)lParam;
    if (m_video)
        return m_video->SetVolume(Is_VS_MUTED(), m_volume);
    return true;
}

TMResult CTuiVideoImpl::OnGetVolume(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    dshow_trace_method("CTuiVideoImpl::OnGetVolume");
    *(float*)lParam = m_volume;
    return true;
}

TMResult CTuiVideoImpl::OnSetMute(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    dshow_trace_method("CTuiVideoImpl::OnSetMute");
    Setting_VS_MUTED(lParam);
    if (m_video)
        m_video->SetVolume(Is_VS_MUTED(), m_volume);
    return true;
}

TMResult CTuiVideoImpl::OnDirectMsg(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    dshow_trace_method("CTuiVideoImpl::OnDirectMsg()");
    if (m_video)
        return m_video->SendMessage(uMsg, wParam, lParam);
    return false;
}

void CTuiVideoImpl::ReCreateTexture(long Width, long Height, EGXFormat fmt)
{
    m_txtVideo.Release();
    _TextureNow::Create(m_txtVideo, Width, Height, fmt);
}

void CTuiVideoImpl::AfterPresent()
{
    if (!Is_WS_PRI_DIRTY())
    {
        Set_WS_PRI_DIRTY();
        if (Is_VS_NOTIFY())
            Notify(CTuiVideo::VN_SEEKING, true);
        PostMessage(CM_INVALIDATE, 0, 0);
    }
}

TMBool CTuiVideoImpl::DoLoad(RERENDERFILE* rf)
{
    dshow_trace_method("CTuiVideoImpl::DoLoad");
    IC_CheckInstance();
    
    DbgAssert(!m_video);
    m_video = CVideoGB::VideoCreate(this, rf);
    if (!m_video)
        return false;
    
    // notify
    Notify(CTuiVideo::VN_LOAD);
    
    Invalidate();
    
    return true;
}

void CTuiVideoImpl::SyncState(bool bPlaying)
{
    if (!bPlaying == !Is_VS_PLAYING())
        return;
    dshow_trace_method("CTuiVideoImpl::SyncState");
    Setting_VS_PLAYING(bPlaying);
    Notify(bPlaying ? CTuiVideo::VN_PLAY : CTuiVideo::VN_PAUSED);
}

void CTuiVideoImpl::DoPrintClient(IGXApp* gapp, TMLParam lParam)
{
    Reset_WS_PRI_DIRTY();
    RenderSimpleBk(gapp, 0, CAppColor::GetColorWindowBk);
    if (Edit_IsObjectInDocMode(this))
        return;
    
    CAutoLock lock(m_critical);
    if (m_txtVideo)
    {
        CTuiRect rc;
        GetPaddingRect(&rc);
        if (Is_VS_KEEPAR())
        {
            CTuiSize szWnd = rc.Size();
            CTuiSize szTex(m_txtVideo->GetWidth(), m_txtVideo->GetHeight());
            if (this->Is_VS_X2Y())
                szTex.Rotate();
            szTex.FitTo(szWnd);
            rc.KeepCSetW(szTex.Width());
            rc.KeepCSetH(szTex.Height());
        }
        TMUInt32 uFlag = GX_BLT_DFTSRCWH | GX_BLT_LINEAR | (this->m_dwStyle & (VS_ROTATE_MASK));
        if (this->m_dwStyle & VS_X2Y)
            rc.Rotate();
        m_txtVideo->Blt(rc, CRect(0,0,0,0), uFlag);
    }
}

void CTuiVideoImpl::Reclaim()
{
    {
        CAutoLock lock(m_critical);
        m_txtVideo.Release();
    }
    CTuiWgt::Reclaim();
}

void CTuiVideoImpl::OnSetLife(TMUInt32 life)
{
    if (life == WS_PRI_LIFE_DESTROYING)
    {
        dshow_trace_method("CTuiVideoImpl::OnSetLife(WS_PRI_LIFE_DESTROYING)");
        CVideoGB::VideoDestroy(m_video, 0);
        m_txtVideo.Release();
    }
    CTuiWgt::OnSetLife(life);
}


#ifdef TUI_EDIT_MODE

CString CTuiVideoImpl::GetDebugInfo()
{
    CString strF(L"\r\n\r\nfile:"), str;
    CStringA str2;
    {
        if (m_ptrMoniker)
        {
            CComBSTR strx;
            MonikerGetFriendlyName(m_ptrMoniker, &strx);
            LPOLESTR strName = NULL;
            m_ptrMoniker->GetDisplayName(NULL, NULL, &strName);
            strF += strx;
            strF += L" - ";
            strF += strName;
            CoTaskMemFree(strName);
        }
        else
            strF += m_strFile;

        CAutoLock lock(m_critical);
        if (m_video && m_video->GetGB())
        {
            CComPtr<IBaseFilter> filter;
            m_video->GetGB()->FindFilterByName(DS_FILTER_NAME, &filter);
            CComPtr<IPin> ptrPin;
            filter->FindPin(L"VMR Input0", &ptrPin);
            if (!ptrPin)
                dsGetFirstPin(filter, ptrPin);
    //        SUCCEEDED(m_pOuterEVR->FindPin(L"EVR Input0", &pPin)) &&

            if (ptrPin && m_txtVideo)
            {
                AM_MEDIA_TYPE mt;
                ptrPin->ConnectionMediaType(&mt);
                VIDEOINFOHEADER* head = (VIDEOINFOHEADER*)mt.pbFormat;
                if (head)
                {
                    str.Format(
                        L"\r\ndelay(ms):%g, bitRate:%gK, width:%d, height:%d, bpp:%d, volume:%g, ",
                        (double)head->AvgTimePerFrame / 10000.0f, head->dwBitRate / 1000.0f,
                        m_txtVideo->GetWidth(), m_txtVideo->GetHeight(), m_txtVideo->GetFormat(),
                        m_volume);
                
                    EGXFormat fmt = _CTuiVideoRenderer_Video2GXFmt(mt.subtype);
                    str2.Format("format:%s", GX_DEBUG_FORMAT2STR(fmt));
                }
                dsFreeMediaType(mt);
            }
        }
    }

    return CTuiWgt::GetDebugInfo() + strF + str + str2;
}

#endif // TUI_EDIT_MODE


TM_REGISTER_CLASS1 (CTuiVideo);


TM_END_NAMESPACE



