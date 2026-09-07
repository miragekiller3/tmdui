/*
 *  @file
 *  @brief  opengl render
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2011.4.23
 *
 *  Copyright (C) 2011 miragekiller
 */

#pragma once

#ifndef GXAPP_GL_H
#define GXAPP_GL_H

#include <gl/gl.h>
#include <glext/glext.h>
#include <glext/wglext.h>

#include "gxapp.h"
#include "../gxgl.h"
#include "gxhelper.h"


// don't use the macro
#undef GL_BYTE


TM_BGN_NAMESPACE

struct CGXAppGL;
struct IGLDataReset;
typedef CRefObjectT<CGXApp3DT<CGXAppGL> > CGXAppGLT;

////////////////////////////////////////////////////////////////////////////////
// _CDrawStripGL
struct CGLVertexX : public CD3dVertex
{
    static void CalcUV(float& x, float& y, float w, float h)
    {
        x = x / w;
        y = 1.0f - y / h;
    }
};

struct _CDrawStripGL
{
    GLenum mode;
    GLsizei count;
    const CGLVertexX* pData;
    UINT nBufferSize;

    template <class T> _CDrawStripGL(T& shape)
    {
        DbgAssert(T::ePT == ePT_TRIANGLESTRIP || T::ePT == ePT_TRIANGLELIST);
        mode = (T::ePT == ePT_TRIANGLELIST ? GL_TRIANGLES : GL_TRIANGLE_STRIP);
        count = shape.countVertex();
        pData = shape.buffer();
        nBufferSize = shape.sizeBuffer();

        typedef typename T::value_type value_type;
//         value_type::EnableClr();
//         shape.m_vertex->DrawXY();
//         shape.m_vertex->DrawClr();
//         shape.m_vertex->DrawUV();


    }
};

////////////////////////////////////////////////////////////////////////////////
// CTextureGLBase
class TM_NO_VTABLE CTextureGLBase : public CTexture_UpdateLayeredWindowBuffer //IGXTextureImpl
{
public:
    typedef _CDrawStripGL CDrawStrip;

    CRefPtr<CGXAppGL>       m_ptrDevice;
    int                     m_width;
    int                     m_height;
    IGLDataReset*           m_pReset;
    TM_IF_DBG(ETextureType m_eType);
    
    CTextureGLBase()
        : m_width(0)
        , m_height(0)
        , m_pReset(0)
    {
    }

    ~CTextureGLBase();

    void InitAs(GLuint& tex, GLint targetformat, int w, int h, GLenum srcFormat = GL_RGBA, const GLvoid *pixels = 0);
    void SubmitResetData(TMBool bDiscard);
    void DelTex(GLuint& tex);

public:
    // self
    virtual TMBool ResetAsDib(const GXDibData& data) { return CTexture_UpdateLayeredWindowBuffer::ResetAsDib(data); }
    virtual TMBool ReplaceByImage(HIMAGE hImage) { return false; }
    
public:
    // IGXTexture
    virtual IGXApp* GetApp() tm_override;
    virtual TMBool  ResetAsObject(const void* pData, EGXReset eReset) tm_override;
    virtual int     GetWidth() tm_override { return m_width; }
    virtual int     GetHeight() tm_override { return m_height; }
};

////////////////////////////////////////////////////////////////////////////////
// CTextureGL
class TM_NO_VTABLE CTextureGL : public CTextureGLBase
{
public:
    GLuint m_texID;
//     CImage m_img;
//     EGXLockRect m_eLock;

public:
    CTextureGL();
    ~CTextureGL();

//     static CTextureGL* CreateInst(CGXAppGL* dev, int w, int h);
    TMBool Init(TMBool bInit);
    void NativeBlt(_CDrawStripGL& ds);
// 	TMBool ReplaceByImage(HIMAGE hImage);

    // IGXTextureImpl
//     virtual void    BltText(const GXRect& rcDst, const ATRectD& rcSrc, TMUInt32 uFlag, TMColor clr) tm_override;

    // CTextureGLBase
//     virtual TMBool ResetAsDib(const GXDibData& data) tm_override;
    virtual TMBool ReplaceByImage(HIMAGE hImage) tm_override;

    // IGXTexture
    virtual void*   GetNativeHandle() tm_override { return (void*)m_texID; }
    virtual TMBool  Resize(int w, int h) tm_override;
//     virtual TMBool  ResetAsObject(const void* pData, EGXReset eReset) tm_override;
    virtual void    GetSurface(IGXSurface** ppSurface) tm_override;
//     virtual void     ColorFill(TMColor clr, const ATRectD* rc) tm_override;
//     virtual void    LockRect(TMRefDib& dib, const TMRect* rc, EGXLockRect flag) tm_override;
//     virtual void    UnlockRect() tm_override;
//     virtual void    Blt(const GXRect& rcDst, const ATRectD& rcSrc, TMUInt32 uFlag, TMUInt8 alpha) tm_override;
//     virtual void    Blts(const GXRect* rcDsts, const ATRectD* rcSrcs, int nCount, TMUInt32 uFlag, TMUInt8 alpha) tm_override;
//     virtual EGXRet  Present(TMHWnd hWnd, EGXPresent eFlag, GXPRESENTFX* fx) tm_override;
};

////////////////////////////////////////////////////////////////////////////////
// CTextureGLNV12, DXGI_FORMAT_NV12
class TM_NO_VTABLE CTextureGLNV12 : public CTextureGLBase
{
public:
    GLuint m_texID[3]; // Y-UV-a
    
public:
    CTextureGLNV12();
    ~CTextureGLNV12();
    TMBool Init(TMBool bInit);
    static CTextureGLNV12* CreateTex(CGXAppGL* pApp, int w, int h, D3DPOOL Pool, CTextureGLNV12* pTo);
    void NativeBlt(_CDrawStripGL& ds);

    // CTextureGLBase
    virtual TMBool ResetAsDib(const GXDibData& data) tm_override;
    
    // IGXTexture
    virtual void*   GetNativeHandle() tm_override { return 0; }
    virtual void    GetSurface(IGXSurface** ppSurface) tm_override { DbgAssert(0); };
    virtual TMBool  Resize(int w, int h) tm_override;
//     virtual TMBool  ResetAsObject(const void* pData, EGXReset eReset) tm_override;
    virtual EGXFormat GetFormat() tm_override { return eGXFormatNV12; }
//     virtual EGXRet  Present(TMHWnd hWnd, EGXPresent eFlag, GXPRESENTFX* fx) tm_override { DbgAssert(0); return eGXRetFail; }
};


////////////////////////////////////////////////////////////////////////////////
// CTextureGLRenderTarget
class TM_NO_VTABLE CTextureGLRenderTarget : public CTextureGL
{
protected:
    GLuint  fboId;
//     GLuint  rboId;
    
public:
    CTextureGLRenderTarget()
        : fboId (0)
//         , rboId (0)
    {
    }
    
    // IGXTexture
    virtual TMBool  Resize(int w, int h) tm_override;

    // IGXTextureImpl
    virtual void    OnSetRenderTarget(CGXAppBase* app) tm_override;
    virtual void    OnResetRenderTarget(CGXAppBase* app, GXPTexture pNew) tm_override;
};

////////////////////////////////////////////////////////////////////////////////
// CTextureGLSwapChain
class TM_NO_VTABLE CTextureGLSwapChain : public CTextureGLRenderTarget
{
protected:
    HDC	    m_hdc;
	HWND    m_wnd;

public:
    CTextureGLSwapChain();
    ~CTextureGLSwapChain();
    TMBool Init(HWND hWnd);
//     TMBool InitDevice();
    void DoPresent();

    // IGXTexture
//     virtual void*   GetNativeHandle() tm_override { return (void*)m_hrc; }
    virtual TMBool  Resize(int w, int h) tm_override;
    virtual TMBool  ResetAsObject(const void* pData, EGXReset eReset) tm_override { return false; }
    virtual EGXRet  Present(TMHWnd hWnd, EGXPresent eFlag, GXPRESENTFX* fx) tm_override;
};

////////////////////////////////////////////////////////////////////////////////
// CGXAppGL
struct CGXAppGL
    : public CGXAppBase
    , public CGLDevice
    , public CShaderImpl2<CGXAppGL, CGLDevice>
    , public CGXDeviceLockNull
{
    typedef CGXDeviceAutoLockT<CGXAppGL>        CAutoLock;
    typedef CVertexListForIndT<CGLVertexX>      CVertexListForInd;
    typedef CGLVertexX                          CVertex_FillRect;
    typedef CGLVertexX                          CVertex_FrameRect;
    typedef CGLVertexX                          CVertex_DrawFocusRect;
    typedef CGLVertexX                          CGLVertex_tex;
    typedef _CDrawStripGL                       CDrawStrip;

    CCritical               m_criTexData;
    CVertexListForInd       m_vertex;
    CTextureGLRenderTarget* m_pBk;

    T_VBO                   m_pVertex;
    T_IBO                   m_pIndex;

    // cbuffer
    T_VSCBuffer                           m_cbufferVS;
    T_VSCBuffer* GetVSCBufffer() { return &m_cbufferVS; }
//    T_PSCBuffer GetPSCBufffer() { return T_PSCBuffer(); }

    CGXAppGL();
    ~CGXAppGL();

    void _gx_check(TMBool bThread = true, TMBool bBk = true)
    {
        if (bBk)
            DbgAssert(m_pBk);
        if (bThread)
            DbgAssert(wglGetCurrentContext() == m_wndDevice.hRC);
        CGXAppBase::_gx_check(bThread, bBk);
    }

    TMBool CreateDevice();
    void ClipDevNow();
    void InitDevTransform();
    void DrawStrip(CDrawStrip& ds);

    // state
    static void SamplersSetLinear();
    static void SamplersResetLinear();
    static void BlendSetOver(); // alpha blend
    static void BlendResetOver() { BlendSetCopy(); }
    static void BlendSetCopy();    // FillRect, FrameRect, VertexFill
    static void BlendResetCopy() {}
    static void BlendSetInvert();  // InvertRect and DrawFocusRect
    static void BlendResetInvert();


    // CGXAppBase
    virtual EGXRet InitProp(GXPTexture texture, int nDPI, TMUInt32 dwFlag) tm_override;
//     virtual IGXTextureImpl* TextureCreateFromTextImage(HIMAGE h, TMColor clr) { return CGXAppBase::TextureCreateFromTextImage(h, 0xffffffff); }

    /////////////////////////////////////////////////////////////////////////
    // IGXApp
    virtual EGXDevice GetDeviceType(GXADAPTERDESC* desc = 0) tm_override { return eGXGraDevGL; }
    
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

    // texture
    virtual void TextureCreateIndirect(IGXTexture** tex, int w, int h, EGXFormat format) tm_override;
    virtual void TextureCreateRenderTarget(IGXTexture** tex, int w, int h, EGXFormat format) tm_override;
    virtual void TextureCreateSwapChain(IGXTexture** tex, TMHWnd hWnd, int w, int h, EGXFormat format) tm_override;
//     virtual GXPTexture TextureCreateFromImage(HIMAGE img) tm_override;
    
    // shapes
    virtual void Clear(TMColor clr) tm_override;
//     virtual void FillRect(const GXRect& rc, TMColor clr) tm_override;
//     virtual void FrameRect(const GXRect& rc, GXDips width, TMColor clr) tm_override;
//     virtual void InvertRect(const GXRect& rc) tm_override;
    // GX_API void gxXorRect(const GXRect& rc, TMColor clr = ATRGB(255,255,255));
    //     virtual void DrawLine(const GXPoint& from, const GXPoint& to, int width, TMColor clr);
    //     virtual void DrawPolyline(const GXPoint* pt, int n, int width, TMColor clr);
    
//     virtual void DrawFocusRect(const GXRect& rc) tm_override;
    
//     virtual void VertexBegin() tm_override { m_vertex.Clear(); }
//     virtual void VertexAddColor(GXDips x, GXDips y, TMColor clr) tm_override { m_vertex.Add(GetDPI(), x, y, clr); }
    virtual void VertexFill(const GXIndices* pt, TMUInt nt) tm_override;
};

TM_END_NAMESPACE

#endif // GXAPP_GL_H
