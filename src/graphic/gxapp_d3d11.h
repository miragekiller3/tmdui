/*
 *  @file
 *  @brief  D3D11 render
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021.5.9
 *
 *  Copyright (C) 2021 miragekiller
 */

#pragma once

#ifndef GXAPP_D3D11_H
#define GXAPP_D3D11_H

#include "gxapp.h"
#include "../gxd3d11.h"
#include "gxhelper.h"


// #define TYPEMODE_C_DECL11
// #include "hlsl\gxvs_i.h"


// debug view see instance:
//  CInstanceCountT<TM::CTextureD3D11NV12>::s_nCount
//  CInstanceCountT<TM::CTextureD3D11>::s_nCount
//  CInstanceCountT<TM::CSwapChainD3D11>::s_nCount
//  CInstanceCountT<TM::CSurfaceD3D11>::s_nCount
//  CInstanceCountT<TM::CTextureImage>::s_nCount
//  CInstanceCountT<TM::CGXApp3DT<TM::CGXAppD3D11> >::s_nCount

TM_BGN_NAMESPACE

class CTextureD3D11;
struct CGXAppD3D11;

TM_ENUM_2_FLAG(D3D11_BIND_FLAG);
TM_ENUM_2_FLAG(D3D11_USAGE);
TM_ENUM_2_FLAG(D3D11_CPU_ACCESS_FLAG);
TM_ENUM_2_FLAG(D3D11_RESOURCE_MISC_FLAG);


////////////////////////////////////////////////////////////////////////////////
// _CDrawStrip11
struct _CDrawStrip11
{
    D3D11_PRIMITIVE_TOPOLOGY PType;
    UINT nVertexCount;
    const void* pData;
    UINT nBufferSize;
    UINT nStride;
    
    template <class T> _CDrawStrip11(T& shape)
    {
        typedef typename T::value_type value_type;
        DbgAssert(T::ePT == ePT_TRIANGLESTRIP || T::ePT == ePT_TRIANGLELIST);
        PType = (T::ePT == ePT_TRIANGLELIST ? D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST : D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        nVertexCount = shape.countVertex();
        nBufferSize = shape.sizeBuffer();
        nStride = sizeof(value_type);
        pData = shape.buffer();
    }
};

////////////////////////////////////////////////////////////////////////////////
// CTextureD3D11Base
class TM_NO_VTABLE CTextureD3D11Base : public CTexture_UpdateLayeredWindowBuffer
{
public:
    typedef _CDrawStrip11           CDrawStrip;
    CComPtr<CGXAppD3D11>            m_ptrDevice;
    CD3D11_TEXTURE2D_DESC           m_ddsd;
    EGXFormat                       m_fmt;
    TM_IF_DBG(ETextureType m_eType);

public:
    TMColor ConvertBltTextColor(TMColor clr) { TMUInt8 a = ColorA(clr); return TMRGBA(a, a, a, a); }

public:
    // IGXTexture
    virtual IGXApp* GetApp() tm_override;
    virtual int GetWidth() tm_override { return m_ddsd.Width; }
    virtual int GetHeight() tm_override { return m_ddsd.Height; }
    virtual EGXFormat GetFormat() tm_override { return m_fmt; }

    // IGXTextureImpl
    virtual void OnSetRenderTarget(CGXAppBase* _app);
    virtual void OnResetRenderTarget(CGXAppBase* _app, GXPTexture pNew);

    // self
    virtual void GetRenderTarget(CComPtr<ID3D11RenderTargetView>& view) {}
};

////////////////////////////////////////////////////////////////////////////////
// CTextureD3D11
class TM_NO_VTABLE CTextureD3D11 : public CTextureD3D11Base
{
public:
    CComPtr<ID3D11Texture2D>            m_ptrSurface;
    CComPtr<ID3D11ShaderResourceView>   m_ptrResView;

public:
    ID3D11Texture2D* GetSurface() { return m_ptrSurface; }
    ~CTextureD3D11() { int x = 0; }

    static CTextureD3D11* CreateTex(
        CGXAppD3D11* app,
        int w, int h, EGXFormat fmtGX, DXGI_FORMAT fmt,
        D3D11_USAGE Usage,
        D3D11_BIND_FLAG BindFlags,
        D3D11_CPU_ACCESS_FLAG CPUAccess,
        D3D11_RESOURCE_MISC_FLAG MiscFlags,
        CTextureD3D11* pTo);
    void InitNoDebug(CGXAppD3D11* app, ID3D11Texture2D* ptrSurface);
    void Init(CGXAppD3D11* app, ID3D11Texture2D* ptrSurface) { InitNoDebug(app, ptrSurface); DBG_INIT(this); }
    void NativeBlt(CDrawStrip& ds);
    void ColorFill(TMColor clr);

    // IGXTexture
    virtual void*   GetNativeHandle() tm_override { return static_cast<IUnknown*>(m_ptrSurface); }
    virtual void    GetSurface(IGXSurface** ppSurface) tm_override;
    
    virtual TMBool  Resize(int w, int h) tm_override;
    virtual TMBool  ResetAsObject(const void* pData, EGXReset eReset) tm_override;
//     virtual TMBool  SaveFile(TMCStr pFile, TMBool bHaveAlpha) tm_override { return false; }
//     virtual int GetWidth() tm_override { return m_ddsd.Width; }
//     virtual int GetHeight() tm_override { return m_ddsd.Height; }
//     virtual EGXFormat GetFormat() tm_override { return m_fmt; }
//     virtual void    Blt(const GXRect& rcDst, const TMRectD& rcSrc, TMUInt32 uFlag, TMUInt8 alpha) tm_override;
//     virtual void    Blts(const GXRect* rcDsts, const TMRectD* rcSrcs, int nCount, TMUInt32 uFlag, TMUInt8 alpha) tm_override;
//     virtual EGXRet  Present(TMHWnd hWnd, EGXPresent eFlag, GXPRESENTFX* fx) tm_override;

    // IGXTextureImpl
//     virtual void    BltText(const GXRect& rcDst, const TMRectD& rcSrc, TMUInt32 uFlag, TMColor clr) tm_override;

    // CTextureD3D11Base
    virtual void GetRenderTarget(CComPtr<ID3D11RenderTargetView>& view) tm_override;
};

////////////////////////////////////////////////////////////////////////////////
// CTextureD3D11NV12
class TM_NO_VTABLE CTextureD3D11NV12 : public CTextureD3D11
{
public:
    CComPtr<ID3D11Texture2D>            m_ptrUV;
    CComPtr<ID3D11ShaderResourceView>   m_ptrViewUV;
    CComPtr<ID3D11Texture2D>            m_ptrAlpha;
    CComPtr<ID3D11ShaderResourceView>   m_ptrViewAlpha;

    static CTextureD3D11NV12* CreateTex(CGXAppD3D11* app, int w, int h, CTextureD3D11NV12* pTo);
    void NativeBlt(CDrawStrip& ds);

    // IGXTexture
    virtual void    GetSurface(IGXSurface** ppSurface) tm_override { DbgAssert(0); };
    virtual TMBool  ResetAsObject(const void* pData, EGXReset eReset) tm_override;
//     virtual EGXRet  Present(TMHWnd hWnd, EGXPresent eFlag, GXPRESENTFX* fx) tm_override { DbgAssert(0); return eGXRetFail; }
};


////////////////////////////////////////////////////////////////////////////////
// CSwapChainD3D11
class TM_NO_VTABLE CSwapChainD3D11
    : public CTextureD3D11
    , public CSwapChain
{
public:
    void SyncBuffer(CComPtr<ID3D11Texture2D>& tex);
    void NativeBlt(CDrawStrip& ds);

public:
    // IGXTexture
    virtual TMBool  Resize(int w, int h) tm_override;
    virtual EGXRet  Present(TMHWnd hWnd, EGXPresent eFlag, GXPRESENTFX* fx) tm_override;

    // CTextureD3D11Base
//     virtual void GetRenderTarget(CComPtr<ID3D11RenderTargetView>& view) tm_override;
};


////////////////////////////////////////////////////////////////////////////////
// CGXAppD3D11
struct CGXAppD3D11;
typedef CRefObjectT<CGXApp3DT<CGXAppD3D11> > CGXAppD3D11T;

struct CGXAppD3D11
    : public CGXAppBase
    , public CD3D11Device
    , public CShaderImpl2<CGXAppD3D11, CD3D11Device>
{
    typedef CVertexListForIndT<CD3dVertex>      CVertexListForInd;
    typedef CD3dVertex                          CVertex_Texture;
    typedef CD3dVertex                          CVertex_FillRect;
    typedef CD3dVertex                          CVertex_FrameRect;
    typedef CD3dVertex                          CVertex_DrawFocusRect;
    typedef _CDrawStrip11                       CDrawStrip;

#ifdef TM_DBG
    int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf)
    {
        CGXAppD3D11* p = (CGXAppD3D11*)cls->m_pClass;
        return IC_Size(p->m_vertex.m_aTv) + IC_Size(p->m_stk);
    }
#endif // TM_DBG

    struct CContextPtr : public CGXDeviceAutoLockT<CGXAppD3D11>
    {
        CContextPtr(CGXAppD3D11* p) : CGXDeviceAutoLockT<CGXAppD3D11>(p) {}
        ID3D11DeviceContext* operator-> () { return m_pObj->GetContext(); }
    };

    struct CLockPtr : public CGXDeviceAutoLockT<CGXAppD3D11>
    {
        CLockPtr(CGXAppD3D11* p) : CGXDeviceAutoLockT<CGXAppD3D11>(p) {}
        CD3D11Device* operator-> () { return m_pObj; }
    };
    
    // cbuffer
    T_VSCBuffer                           m_cbufferVS;
    T_VSCBuffer* GetVSCBufffer() { return &m_cbufferVS; }
//    T_PSCBuffer GetPSCBufffer() { return T_PSCBuffer(); }

    // States
    CComPtr<ID3D11RasterizerState>      m_pRasterizerState;
    CComPtr<ID3D11SamplerState>         m_pSamplerState;
    CComPtr<ID3D11SamplerState>         m_pSamplerStateLinear;
    CComPtr<ID3D11BlendState>           m_pBlendStateAlpha;     // alpha blend
    CComPtr<ID3D11BlendState>           m_pBlendStateCopy;      // srccopy
    CComPtr<ID3D11BlendState>           m_pBlendStateInvert;    // invert dest

    T_VBO                               m_pVertex;
    T_IBO                               m_pIndex;

    CComPtr<ID3D11Texture2D>            m_ptrOldRenderTarget;

    CTextureD3D11*                      m_pBk;
    CVertexListForInd                   m_vertex;

    CGXAppD3D11();
    ~CGXAppD3D11();

    void _gx_check(TMBool bThread = true, TMBool bBk = true)
    {
        if (bBk)
            DbgAssert(m_pBk);
        CGXAppBase::_gx_check(bThread, bBk);
    }

//    void _gx_check()
//    {
//        DbgAssert(m_pBk);
//        DbgAssert(m_font);
////         DbgAssert((static_cast<CGXAppD3D11T*>(this))->IsCreatorThread());
//    }

    TMBool CreateDevice(IDXGIAdapter* Adapter = 0);

    // state
    void SamplersSetLinear();
    void SamplersResetLinear();
    void BlendSetOver(); // alpha blend
    void BlendResetOver() { BlendSetCopy(); }
    void BlendSetCopy();    // FillRect, FrameRect, VertexFill
    void BlendResetCopy() {}
    void BlendSetInvert();  // InvertRect, DrawFocusRect
    void BlendResetInvert();
    
    void ClipDevInit() { CGXAppBase::ClipDevInit(); if (m_pBk) ClipDevNow(); }

    void ClipDevNow();
    EGXRet CheckError(HRESULT hr);
    void InitDevTransform();
    void DrawStrip(_CDrawStrip11& ds);
//    CContextPtr GetLockContext() { return this; }

    // CGXAppBase
    virtual EGXRet InitProp(GXPTexture texture, int nDPI, TMUInt32 dwFlag) tm_override;
//     virtual IGXTextureImpl* TextureCreateFromTextImage(HIMAGE h, TMColor clr) { return CGXAppBase::TextureCreateFromTextImage(h, 0xffffffff); }

    /////////////////////////////////////////////////////////////////////////
    // IGXApp
    virtual void*   GetNativeHandle() tm_override { return static_cast<IUnknown*>(m_device); }
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
//     virtual void FontSetCurrent(const GXFont& ft);
//     virtual void FontGetCurrent(GXFont& ft);
//     virtual void FontGetDefault(GXFont& ft); // get system default font
//     virtual GXDips FontGetHeight();
//     virtual void PushFont();
//     virtual void PopFont();
//     virtual TMColor SetTextColor(TMColor clr);
//     virtual TMColor GetTextColor() { return m_clrText; }
    
    // texture
    virtual void TextureCreateRenderTarget(IGXTexture** tex, int w, int h, EGXFormat format) tm_override;
    virtual void TextureCreateSwapChain(IGXTexture** tex, TMHWnd hWnd, int w, int h, EGXFormat format) tm_override;

    virtual void TextureCreateIndirect(IGXTexture** tex, int w, int h, EGXFormat format) tm_override;
//     virtual GXPTexture TextureCreateFromImage(HIMAGE img) tm_override;
    
//     virtual GXBrush BrushCreate(GXPTexture texture) tm_override;
//     virtual void BrushRelease(GXBrush brush) tm_override;
//     virtual void BrushTile(const GXRect& rcDst, GXBrush brush, TMUInt32 uFlag, TMUInt8 alpha) tm_override;
    
    // shapes
    virtual void Clear(TMColor clr) tm_override;
//     virtual void FillRect(const GXRect& rc, TMColor clr) tm_override;
//     virtual void FrameRect(const GXRect& rc, GXDips width, TMColor clr) tm_override;
//     virtual void InvertRect(const GXRect& rc) tm_override;
//     // GX_API void gxXorRect(const GXRect& rc, TMColor clr = TMRGB(255,255,255));
// //     virtual void DrawLine(const GXPoint& from, const GXPoint& to, int width, TMColor clr);
// //     virtual void DrawPolyline(const GXPoint* pt, int n, int width, TMColor clr);
//     virtual void DrawFocusRect(const GXRect& rc) tm_override;
//     //virtual void DrawDC(const GXRect& rcDst, HDC hSrc, const TMRectI& rcSrc, TMUInt32 dwRop) tm_override;
//     
//     virtual void VertexBegin() tm_override { m_vertex.Clear(); }
//     virtual void VertexAddColor(GXDips x, GXDips y, TMColor clr) tm_override { m_vertex.Add(GetDPI(), x, y, clr); }
    virtual void VertexFill(const GXIndices* pt, TMUInt nt) tm_override;
};


TM_END_NAMESPACE

#endif // GXAPP_D3D11_H


