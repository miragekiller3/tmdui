/*
 *  @file
 *  @brief  video support for windows
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2017.7.17
 *
 *  Copyright (C) 2017 miragekiller
 */

// https://github.com/microsoft/FFmpegInterop.git

#pragma once

#ifndef UIVIDEO_H
#define UIVIDEO_H

#include "uiwidget.h"
#include "../tmdshow.h"
#include "../tmmessagequeue.h"


TM_BGN_NAMESPACE


class CTuiVideoImpl;
// typedef CRendererVMR9Impl<CTuiVideoImpl>            D3D_renderer;
//typedef CRendererEVRImpl<CTuiVideoImpl>             D3D_renderer;


///////////////////////////////////////////////////////////////////////////////
// CRenderNormal
class CRenderNormal
    : public CBaseRefT<CRenderNormal, CRenderNormalImpl<CRenderNormal> >
    , public TM::CRefObjectRoot<0>
{
private:
    CTuiVideoImpl*          m_pWnd;

    // IPin
    virtual HRESULT STDMETHODCALLTYPE ReceiveConnection(IPin *pConnector, const AM_MEDIA_TYPE *pmt) tm_override;
    
    // IMemInputPin
    virtual HRESULT STDMETHODCALLTYPE Receive(IMediaSample *pSample) tm_override;
    virtual HRESULT STDMETHODCALLTYPE ReceiveMultiple(IMediaSample **pSamples, long nSamples, long *nSamplesProcessed) tm_override;

    void SyncTexture(IMediaSample *pSample);

public:
    CRenderNormal(CTuiVideoImpl* pWnd) : m_pWnd(pWnd)
    {
        InitRenderer();
    }
};


///////////////////////////////////////////////////////////////////////////////
// CVideoGB
struct RERENDERFILE
{
    FILTER_STATE    fs;
    REFERENCE_TIME  tm;
    TMBool          mute;
    float           vol;
};

struct CVideoGB
    : protected CGraphBuilderCaptureT<CGraphBuilderVideo>
    , protected CMessageThreadT<CVideoGB>
{
private:
    HANDLE          m_hEvent;
    CTuiVideoImpl*  m_pWnd;
    TM_SELF_ALLOC(CVideoGB);

    enum
    {
        WM_RENDERFILE   = WM_USER + 300, // lp: RERENDERFILE*
        WM_CLOSEEXIT    = WM_USER + 301, // lp: RERENDERFILE*
    };

public:
    typedef CGraphBuilderCaptureT<CGraphBuilderVideo>   capture_class;
    typedef CGraphBuilderVideo                          video_class;
    typedef CMessageThreadT<CVideoGB>                   thread_class;
    friend thread_class;

private:
    CVideoGB(CTuiVideoImpl* pWnd);
    ~CVideoGB();
    TMBool RawRenderFile(CTuiVideoImpl* pWnd);
    void NotifyEvent();

    // CMessageThreadT
    void WaitMessage();
    TMResult WndProc(TMUInt uMsg, TMWParam wParam, TMLParam lParam);

public:
    static CVideoGB* VideoCreate(CTuiVideoImpl* pWnd, RERENDERFILE* rr);
    static void VideoDestroy(CVideoGB*& p, RERENDERFILE* rf);
    TMBool SetVolume(TMBool bMute, float fVol) { return this->SendMessage(VM_SETVOLUME, bMute, (TMLParam)&fVol); }
    IGraphBuilder* GetGB() { return m_ptrGraphBuilder; }
    using thread_class::SendMessage;
};


///////////////////////////////////////////////////////////////////////////////
// CTuiVideoImpl
class TM_NO_VTABLE CTuiVideoImpl : public CTuiWgt
{
private:
    typedef CAutoLockT<CCritical>                       CAutoLock;
    friend CRenderNormal;
    friend CVideoGB;

    CVideoGB*               m_video;
    CRefPtr<IGXTexture>     m_txtVideo;
    float                   m_volume;
    CString                 m_strFile;
    CComPtr<IMoniker>       m_ptrMoniker;
    CCritical               m_critical;

public:
    CTuiVideoImpl();
    ~CTuiVideoImpl();

public:
    TM_BGN_MASK_SYMBOL2 (CSymbol_VStyle, CSymbol_WsStyle)
//        TM_MASK_SYMBOL  (VS_AUTOPLAY)
//        TM_MASK_SYMBOL  (VS_LOOP)
        TM_MASK_SYMBOL  (VS_MUTED)
        TM_MASK_SYMBOL  (VS_NOTIFY)
        TM_MASK_SYMBOL  (VS_KEEPAR)
        TM_MASK_SYMBOL  (VS_R2L)
        TM_MASK_SYMBOL  (VS_B2T)
        TM_MASK_SYMBOL  (VS_X2Y)
        TM_MASK_BREAK   ()
    TM_END_MASK_SYMBOL  ()
        
    TM_DECLARE_FLAG     (_VS_MUTED,     m_dwStyle, VS_MUTED);
    TM_DECLARE_FLAG     (_VS_NOTIFY,    m_dwStyle, VS_NOTIFY);
    TM_DECLARE_FLAG     (_VS_PLAYING,   m_dwStyle, VS_PLAYING);
    TM_DECLARE_FLAG     (_VS_KEEPAR,    m_dwStyle, VS_KEEPAR);
    TM_DECLARE_FLAG     (_VS_X2Y,       m_dwStyle, VS_X2Y);

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_MASK   ("Style", m_dwStyle, CSymbol_VStyle)
        OBJ_PROP_TEXT   ("src", m_strFile)
        CHAIN_OBJ_PROP  (CTuiWgt)
    END_OBJ_PROP_MAP    ()
        
    BEGIN_TUIMSG_MAP    (CTuiVideoImpl)
        MESSAGE_HANDLER (WM_NCCREATE,               OnCreate)
        MESSAGE_HANDLER (VM_GETVOLUME,              OnGetVolume)
        MESSAGE_HANDLER (VM_GETSRC,                 OnGetSrc)

        ENTRY_LIFE_MAP  (this)
        MESSAGE_HANDLER (VM_SETSRC,                 OnSetSrc)
        MESSAGE_HANDLER (VM_SETSRCM,                OnSetSrcM)
        MESSAGE_HANDLER (VM_LOAD,                   OnLoad)
        MESSAGE_HANDLER (VM_PLAY,                   OnPlay)
        MESSAGE_HANDLER (VM_PAUSE,                  OnPause)
        MESSAGE_HANDLER (VM_SETVOLUME,              OnSetVolume)
        MESSAGE_HANDLER (VM_SETMUTE,                OnSetMute)
        MESSAGE_HANDLER (VM_GETMUTE,                OnGetMute)
        MESSAGE_HANDLER (VM_GETDURATION,            OnDirectMsg)
        MESSAGE_HANDLER (VM_SETTIME,                OnDirectMsg)
        MESSAGE_HANDLER (VM_GETTIME,                OnDirectMsg)
        MESSAGE_HANDLER (VM_GETENDED,               OnDirectMsg)
        MESSAGE_HANDLER (VM_GETVIDEOSIZE,           OnDirectMsg)

        if (uMsg == CM_RENDERCHANGED)
        {
            CTuiWgt::ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
            OnRenderChanged(uMsg, wParam, lParam, bHandled);
            return false;
        }

        LEAVE_LIFE_MAP  ()

        CHAIN_MSG_MAP   (CTuiWgt)
    END_TUIMSG_MAP      ()
        
    TMResult OnCreate(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnPreCommand(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnRenderChanged(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

//     TMResult OnGetRender(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { return (TMResult)static_cast<IBaseFilter*>(this); }
    TMResult OnSetSrc(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnSetSrcM(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnGetSrc(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

    TMResult OnLoad(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnPlay(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnPause(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnSetVolume(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnGetVolume(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnSetMute(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnGetMute(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled) { return Is_VS_MUTED(); }

    TMResult OnDirectMsg(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

private:
    void ReCreateTexture(long Width, long Height, EGXFormat fmt);
    void AfterPresent();
    TMBool DoLoad(RERENDERFILE* rf);
    void SyncState(bool bPlaying);

protected:
    // CTuiWgt
    virtual void DoPrintClient(IGXApp* gapp, TMLParam lParam) tm_override;
    virtual void Reclaim() tm_override;
    virtual void OnSetLife(TMUInt32 life) tm_override;

#ifdef TUI_EDIT_MODE
    CString GetDebugInfo() tm_override;
#endif // TUI_EDIT_MODE
};




TM_END_NAMESPACE



#endif // UIVIDEO_H
