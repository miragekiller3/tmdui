/*
 *  @file
 *  @brief  D3D9 render
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2011.4.13
 *
 *  Copyright (C) 2011 miragekiller
 */

#pragma once

#ifndef GXAPP_D3D9_H
#define GXAPP_D3D9_H

#include "gxapp.h"
#include "../gxd3d9.h"
#include "gxhelper.h"


typedef interface ID3DXEffect *LPD3DXEFFECT;

// debug view see instance:
//  CInstanceCountT<TM::CTextureD3D9NV12>::s_nCount
//  CInstanceCountT<TM::CTextureD3D9>::s_nCount
//  CInstanceCountT<TM::CSwapChainD3D9>::s_nCount
//  CInstanceCountT<TM::CSurfaceD3D9>::s_nCount
//  CInstanceCountT<TM::CTextureImage>::s_nCount
//  CInstanceCountT<TM::CGXApp3DT<TM::CGXAppD3D9> >::s_nCount


TM_BGN_NAMESPACE

class CTextureD3D9;
struct CGXAppD3D9;
typedef CRefObjectT<CGXApp3DT<CGXAppD3D9> > CGXAppD3D9T;
inline int D3D9GetFreeMem(IDirect3DDevice9* dev) { return dev->GetAvailableTextureMem() / (1024 * 128); }

////////////////////////////////////////////////////////////////////////////////
// _CDrawStrip
struct _CDrawStrip
{
//     DWORD FVF;
    D3DPRIMITIVETYPE PType;
    UINT PCount;
    const void* pData;
    UINT VertexStreamZeroStride;
    
    template <class T> _CDrawStrip(T& shape)
    {
        typedef typename T::value_type value_type;
//         FVF = 0;//value_type::eFVF;
        DbgAssert(T::ePT == ePT_TRIANGLESTRIP || T::ePT == ePT_TRIANGLELIST);
        PType = (T::ePT == ePT_TRIANGLELIST ? D3DPT_TRIANGLELIST : D3DPT_TRIANGLESTRIP);
        PCount = shape.size();
        pData = shape.buffer();
        VertexStreamZeroStride = sizeof(value_type);
    }
};


////////////////////////////////////////////////////////////////////////////////
// CTextureD3D9Base
class TM_NO_VTABLE CTextureD3D9Base : public CTexture_UpdateLayeredWindowBuffer
{
public:
    typedef _CDrawStrip CDrawStrip;
    CComPtr<CGXAppD3D9> m_ptrDevice;
    D3DSURFACE_DESC     m_ddsd;

protected:
    ~CTextureD3D9Base();

public:
    // IGXTexture
    virtual IGXApp* GetApp() tm_override;
    virtual int GetWidth() tm_override { return m_ddsd.Width; }
    virtual int GetHeight() tm_override { return m_ddsd.Height; }
    virtual EGXFormat GetFormat() tm_override { return dxD3DFormat2GXFormat(m_ddsd.Format); }
};


////////////////////////////////////////////////////////////////////////////////
// CTextureD3D9
class TM_NO_VTABLE CTextureD3D9 : public CTextureD3D9Base
{
public:
    CComPtr<IDirect3DSurface9>  m_ptrSurface;
    TM_IF_DBG(ETextureType      m_eType);

    TM_BGN_INTERFACE_MAP_QM(m_ptrSurface)
    TM_END_INTERFACE_MAP()

public:
    CTextureD3D9() {}
    ~CTextureD3D9() {}
    static CTextureD3D9* CreateTex(CGXAppD3D9* app, int w, int h, D3DFORMAT fmt, DWORD Usage, D3DPOOL Pool, CTextureD3D9* pTo);
    IDirect3DSurface9* GetSurface9() { return m_ptrSurface; }
    void Init(CGXAppD3D9* app, IDirect3DSurface9* ptrSurface);
    void NativeBlt(_CDrawStrip& ds);
    void ColorFill(TMColor clr);

    // IGXTexture
    virtual void*   GetNativeHandle() tm_override { return static_cast<IUnknown*>(m_ptrSurface); }
    virtual TMBool  Resize(int w, int h) tm_override;
    virtual TMBool  ResetAsObject(const void* pData, EGXReset eReset) tm_override;
    virtual void    GetSurface(IGXSurface** ppSurface) tm_override;

    // IGXTextureImpl
    virtual void    OnSetRenderTarget(CGXAppBase* _app);
    virtual void    OnResetRenderTarget(CGXAppBase* _app, GXPTexture pNew);
};


////////////////////////////////////////////////////////////////////////////////
// CTextureD3D9NV12, DXGI_FORMAT_NV12
class TM_NO_VTABLE CTextureD3D9NV12 : public CTextureD3D9Base
{
public:
    CComPtr<IDirect3DTexture9>  m_ptr[3]; // Y-UV-a
    
public:
    static CTextureD3D9NV12* CreateTex(CGXAppD3D9* pD3D, int w, int h, D3DPOOL Pool, CTextureD3D9NV12* pTo);
    void Init(CGXAppD3D9* app, IDirect3DTexture9* pTexture);
    void NativeBlt(_CDrawStrip& ds);
    TMBool ResetAsDib(const GXDibData& data);
    
    // IGXTexture
    virtual void*   GetNativeHandle() tm_override { return 0; }
    virtual void    GetSurface(IGXSurface** ppSurface) tm_override { DbgAssert(0); };
    virtual TMBool  Resize(int w, int h) tm_override;
    virtual TMBool  ResetAsObject(const void* pData, EGXReset eReset) tm_override;
//     virtual EGXRet  Present(TMHWnd hWnd, EGXPresent eFlag, GXPRESENTFX* fx) tm_override { DbgAssert(0); return eGXRetFail; }
};


////////////////////////////////////////////////////////////////////////////////
// CSwapChainD3D9
class TM_NO_VTABLE CSwapChainD3D9 : public CTextureD3D9
{
public:
    HWND m_hWnd;
    CComPtr<IDirect3DSwapChain9> m_ptrSwapChain;
    
    TM_BGN_INTERFACE_MAP_QM(m_ptrSwapChain)
    TM_END_INTERFACE_MAP()

    static CSwapChainD3D9* CreateChain(CGXAppD3D9* app, TMHWnd hWnd, int w, int h, EGXFormat fmt, CSwapChainD3D9* pTo);
    void NativeBlt(_CDrawStrip& ds);

    // IGXTexture
    virtual TMBool  Resize(int w, int h) tm_override;
    virtual EGXRet  Present(TMHWnd hWnd, EGXPresent eFlag, GXPRESENTFX* fx) tm_override;
};


////////////////////////////////////////////////////////////////////////////////
// CGXAppD3D9
typedef CD3dVertex CVertex9;
struct CGXAppD3D9
    : public CGXAppBase
    , public CD3D9Device
    , public CShaderImpl2<CGXAppD3D9, CD3D9Device>
    , public CGXDeviceLockNull
{
    typedef CGXDeviceAutoLockT<CGXAppD3D9>      CAutoLock;
    typedef CVertexListForIndT<CVertex9>        CVertexListForInd;
    typedef CVertex9                            CVertex_Texture;
    typedef CVertex9                            CVertex_FillRect;
    typedef CVertex9                            CVertex_FrameRect;
    typedef CVertex9                            CVertex_DrawFocusRect;
    typedef _CDrawStrip                         CDrawStrip;

#ifdef TM_DBG
    int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf)
    {
        CGXAppD3D9* p = (CGXAppD3D9*)cls->m_pClass;
        return IC_Size(p->m_vertex.m_aTv) + IC_Size(p->m_stk);
    }
#endif // TM_DBG

    CComPtr<IDirect3DSurface9>      m_ptrOldRenderTarget;

    CTextureD3D9*                   m_pBk;
    CVertexListForInd               m_vertex;

    LPD3DXEFFECT m_pEffect;
    LPDIRECT3DVERTEXDECLARATION9 m_pDecl;

    TM_BGN_INTERFACE_MAP_QM(m_pd3dDevice9)
    TM_END_INTERFACE_MAP()

    CGXAppD3D9();
    ~CGXAppD3D9();

    void _gx_check(TMBool bThread = true, TMBool bBk = true)
    {
        if (bBk)
            DbgAssert(m_pBk);
        CGXAppBase::_gx_check(bThread, bBk);
//        DbgAssert(m_font);
//         DbgAssert((static_cast<CGXAppD3D9T*>(this))->IsCreatorThread());
    }

    TMBool CreateDevice(int nAdapter = D3DADAPTER_DEFAULT);
    EGXRet CheckError(HRESULT hr);
    void ClipDevNow();
    void InitDevTransform();
    void DrawStrip(_CDrawStrip& ds);
    HRESULT NativeCreateTexture(UINT w, UINT h, DWORD Usage, D3DFORMAT fmt, D3DPOOL Pool, IDirect3DTexture9** ppTexture)
    {
        return m_pd3dDevice9->CreateTexture(w, h, 1, Usage, fmt, Pool, ppTexture, NULL);
    }

    // state
    void SamplersSetLinear();
    void SamplersResetLinear();
    void BlendSetOver(); // alpha blend
    void BlendResetOver() { BlendSetCopy(); }
    void BlendSetCopy();    // FillRect, FrameRect, VertexFill
    void BlendResetCopy() {}
    void BlendSetInvert();  // InvertRect and DrawFocusRect
    void BlendResetInvert();

    /////////////////////////////////////////////////////////////////////////
    // IGXApp
    virtual void*   GetNativeHandle() tm_override { return static_cast<IUnknown*>(m_pd3dDevice9); }
    virtual EGXDevice GetDeviceType(GXADAPTERDESC* desc) tm_override;

//     virtual EGXRet SetRenderTarget(GXPTexture texture);
    virtual GXPTexture GetRenderTarget() tm_override;

    // Transform
//     virtual void GetTransform(TM::CAffine* aff, int* dpi = 0, ATLocal* lcId = 0) = 0;
//     virtual void SetTransform(const TM::CAffine* aff, const int* dpi = 0, const ATLocal* lcId = 0, EGXTransform eMode = eGXTransformSet) = 0;
    
    // clip and push.
//     virtual TMBool ClipAndPush(const GXRect& rc, TMBool bIntersect = false, EGXProp dwPush = eGXPropNone) = 0; // dwPush : mask of EGXProp
//     virtual void GetClip(GXRect& rc) = 0;
//     virtual void Push(EGXProp dwFlag) = 0; // mask of EGXProp
//     virtual EGXPrp Pop() = 0;
    
    // font and text
//     virtual void FontSetCurrent(const GXFont& ft) = 0;
//     virtual void FontGetCurrent(GXFont& ft) = 0;
//     virtual GXDips FontGetHeight() = 0;
//     virtual TMColor SetTextColor(TMColor clr) = 0; // support alpha channel.
//     virtual TMColor GetTextColor() = 0;
    
    // texture
    virtual void TextureCreateRenderTarget(IGXTexture** tex, int w, int h, EGXFormat format) tm_override;
    virtual void TextureCreateSwapChain(IGXTexture** tex, TMHWnd hWnd, int w, int h, EGXFormat format) tm_override;
    virtual void TextureCreateIndirect(IGXTexture** tex, int w, int h, EGXFormat format) tm_override;
//     virtual GXPTexture TextureCreateFromImage(HIMAGE img) tm_override;
//     virtual void TextureCreateAlphaVideo(IGXTexture** tex, int w, int h, EGXFormat format) tm_override;
    
    // shapes
    virtual void Clear(TMColor clr) tm_override;
//     virtual void FillRect(const GXRect& rc, TMColor clr) = 0;
//     virtual void FrameRect(const GXRect& rc, GXDips width, TMColor clr) = 0;
//     virtual void InvertRect(const GXRect& rc) = 0;
//     virtual void DrawFocusRect(const GXRect& rc) = 0;
//     virtual void DrawRectEdge(const GXRect& rc, GXDips wBorderH, GXDips wBorderV, TMColor clr[4]) = 0; // clr:
//     virtual void GrandientFill(const GXRect& rc, TMColor clrFrom, TMColor clrTo, EGrandientFill eFill) = 0;
//     virtual void DrawIcon(const GXRect& rc, TMHIcon hIcon) = 0;

    // CGXAppBase
    virtual EGXRet InitProp(GXPTexture texture, int nDPI, TMUInt32 dwFlag) tm_override;
//     virtual void VertexBegin() {}
//     virtual void VertexAddColor(GXDips x, GXDips y, TMColor clr) {}
    virtual void VertexFill(const GXIndices* pt, TMUInt nt) tm_override;

	virtual EGXRet PresentCanvas(IGXCanvas* canvas, const TM::CAffine* aff) tm_override;
};


TM_END_NAMESPACE

#endif // GXAPP_D3D9_H


