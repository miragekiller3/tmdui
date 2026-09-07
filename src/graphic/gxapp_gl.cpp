/*
 *  @file
 *  @brief  opengl render
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2011.4.13
 *
 *  Copyright (C) 2011 miragekiller
 */


#if TM_ANDROID

#   define TM_RENDER_GLES           1
#   include "canvas_android.h"

#elif TM_QNX

#   define TM_RENDER_GLES           1

#   include "canvas_gles.h"

#elif (TM_WIN)

#   define TM_RENDER_OPENGL         1

#   include "gxapp_gl.h"
#   pragma comment (lib, "opengl32.lib")

#endif

#include "hlsl/_gl_VS.h"
#include "hlsl/_gl_PS.h"
#include "hlsl/_gl_PS_tex.h"
#include "hlsl/_gl_PS_NV12.h"
#include "hlsl/_gl_PS_NV12A.h"

#define GX_GL_VAO_IMPL(size,type,stride,offset,normalized,i) \
    gl_verify(dev->glEnableVertexAttribArray(i)); \
    gl_verify(dev->glVertexAttribPointer(i,size,type,normalized,stride,(const void *)offset));
#define GX_GL_LOC_IMPL(id,name) \
    gl_verify(dev->glBindAttribLocation(prog, id, "in_var_" #name));
#include "hlsl/_gl_layout.h"

static const CGLDevice::VS_DESC cg_descVSGL = { gl_VS, ubo_name_CBuffer, InitVAO_VS_INPUT1, InitLOC_VS_INPUT1 };


#define GX_GL_USE_SHADER 1
//#define GX_GL_USE_SHADER 0


TM_BGN_NAMESPACE

///////////////////////////////////////////////////////////////////////////////
// helper class
struct CAutoContext
{
    HDC m_hOldDc;
    HGLRC m_hOldRc;

    CAutoContext(HDC hdc, HGLRC hrc)
        : m_hOldDc(wglGetCurrentDC())
        , m_hOldRc(wglGetCurrentContext())
    {
        DbgVerify(wglMakeCurrent(hdc, hrc));

#ifdef TM_DBG
        GLboolean b = 0;
        gl_verify(glGetBooleanv(GL_DOUBLEBUFFER, &b));
        DbgAssert(!b);
#endif
    }
    
    ~CAutoContext()
    {
        DbgVerify(wglMakeCurrent(m_hOldDc, m_hOldRc));
    }
};

template <class T_TexType, class T_HWndOrInit>
T_TexType* _CTextureGL_CreateTex(CGXAppGL* app, int w, int h, T_TexType* pTo, T_HWndOrInit bInit)
{
    gl_trace("_CTextureGL_CreateTex(dev:%p, w:%d, h:%d)", app, w, h);
    if (!pTo)
    {
        pTo = CTextureBltT<T_TexType, CGLVertexX>::Inst();
        pTo->m_ptrDevice = app;
    }
    if ((pTo->m_width == w) && (pTo->m_height == h))
        return pTo;

    if ((pTo->m_width = w) && (pTo->m_height = h) && pTo->Init(bInit))
        return pTo;
    pTo->Release();
    return 0;
}

inline void _CGXAppGL_CallFlush(CGXAppGL* p)
{
    CallFlush(p);
}


////////////////////////////////////////////////////////////////////////////////
// IGLDataReset
struct IGLDataReset
{
    virtual ~IGLDataReset() {}
    virtual void done(CTextureGLBase* tex) = 0;

//     static TMBool CTextureGL::ResetAsObject(CTextureGLBase* const void* pData, EGXReset eReset)
};

class CDataResetImage : public IGLDataReset
{
private:
    CImage m_img;
public:
    CDataResetImage(HIMAGE img) : m_img(img) {}
    virtual void done(CTextureGLBase* tex)
    {
        tex->ReplaceByImage(m_img);
    }
};

class CDataResetTex : public IGLDataReset
{
private:
    GXDibData dd;

public:
    CDataResetTex(CTextureGLBase* tex, const void* pData)
    {
        GXDibData* pd = (GXDibData*)pData;
        dd = *pd;
        if (dd.bits)
        {
            EGXFormat fmt = tex->GetFormat();
            size_t sz;
            if (fmt == eGXFormatNV12)
                sz = DWORD_ALIGNED(tex->m_width) * tex->m_height * 1.5;
            else
                sz = DWORD_WBYTES(tex->m_width * 32) * tex->m_height;
            dd.bits = MemAllocBy(sz, pd->bits, CDataResetTex);
        }
        if (dd.alpha.m_bits)
        {
            size_t sz = DWORD_ALIGNED(tex->m_width) * tex->m_height;
            dd.alpha.m_bits = MemAllocBy(sz, pd->alpha.m_bits, CDataResetTex);
        }
    }

    ~CDataResetTex()
    {
        MemFree(dd.bits);
        MemFree(dd.alpha.m_bits);
    }
    
    virtual void done(CTextureGLBase* tex)
    {
        void* p = this;
        tex->ResetAsDib(dd);
    }
};


////////////////////////////////////////////////////////////////////////////////
// CSurfaceGL
struct CSurfaceGL : public IGXSurfaceImpl
{
private:
    CRefPtr<CTextureGL> m_tex;
    TMBool m_bWriteBack;
    CImage m_img;

public:
    void Init(CTextureGL* tex)
    {
        tex->m_ptrDevice->_gx_check(true, false);
        m_tex = tex;
    }

    ~CSurfaceGL()
    {
    }
    
private:
    void MakeCache(EGXLockRect flag)
    {
        m_bWriteBack = (flag != GX_LOCK_RECT_READ);

        m_img.CreateIndirect(m_tex->GetWidth(), m_tex->GetHeight(), eGXFormat32A);
        if (m_img && (flag & GX_LOCK_RECT_READ))
        {
            m_tex->SubmitResetData(false);
            gl_verify(glBindTexture(GL_TEXTURE_2D, m_tex->m_texID));
            gl_verify(glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_img.m_hImage->dib.m_bits));
            gl_verify(glBindTexture(GL_TEXTURE_2D, 0));
        }
        else
            m_tex->SubmitResetData(true);
    }

    void WriteBack()
    {
        DbgAssert(m_img);
        if (!m_bWriteBack)
            return;
        m_tex->ResetAsImage(m_img);
    }

    virtual void LockRect(TMDibBuff& dib, const TMRect* rc, EGXLockRect flag) tm_override
    {
        DbgAssert(!m_img);
        MakeCache(flag);
		if (!m_img)
			return;
        ImageLockRect(m_img, dib, rc, flag);
    }
    
    virtual void UnlockRect() tm_override
    {
        DbgAssert(m_img);
        WriteBack();
    }
    
    virtual TMHDC GetDC(TMBool bDiscard) tm_override
    {
        DbgAssert(!m_img);
        MakeCache(bDiscard ? GX_LOCK_RECT_WRITE : GX_LOCK_RECT_READWRITE);
		if (!m_img)
			return 0;
        return ImageCreateDC(m_img);
    }

    virtual void ReleaseDC(TMHDC hdc) tm_override
    {
        DbgAssert(m_img);
        ImageDeleteDC(m_img, hdc);
        WriteBack();
    }

//     virtual void ToTexture(IGXTexture** ppTex) tm_override
//     {
//         IGXTexture* p = m_tex;
//         p->AddRef();
//         *ppTex = p;
//     }
};


////////////////////////////////////////////////////////////////////////////////
// CTextureGLBase
IGXApp* CTextureGLBase::GetApp()
{
    return m_ptrDevice;
}

CTextureGLBase::~CTextureGLBase()
{
    SubmitResetData(true);
}

void CTextureGLBase::InitAs(GLuint& tex, GLint targetformat, int w, int h, GLenum srcFormat, const GLvoid *pixels)
{
    DbgAssert(!m_ptrDevice->GetTexture0());
    if (!tex)
    {
        gl_verify(glGenTextures(1, &tex));
        gl_verify(glBindTexture(GL_TEXTURE_2D, tex));
        gl_verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        gl_verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
        gl_verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0));
        m_ptrDevice->SamplersResetLinear();
    }
    else
        gl_verify(glBindTexture(GL_TEXTURE_2D, tex));

    gl_verify(glTexImage2D(GL_TEXTURE_2D, 0, targetformat, w, h, 0, srcFormat, GL_UNSIGNED_BYTE, pixels));
    gl_verify(glBindTexture(GL_TEXTURE_2D, 0));
}

void CTextureGLBase::SubmitResetData(TMBool bDiscard)
{
    m_ptrDevice->m_criTexData.Lock();
    IGLDataReset* p = m_pReset;
    m_pReset = 0;
    if (!bDiscard && p)
        p->done(this);
    delete p;
    m_ptrDevice->m_criTexData.Unlock();
}

struct _CTextureGLBase_DelTex
{
    GLuint              tex;
    CGXAppGL*           app;
    void operator()(TMBool bCancel) { gl_verify(glDeleteTextures(1, &tex)); app->Release(); }
};

void CTextureGLBase::DelTex(GLuint& tex)
{
    DbgAssert(tex);
    _CTextureGLBase_DelTex tmp = { tex, m_ptrDevice };
    tmp.app->AddRef();
    DbgVerify(CallCreateT(eCallTypeSyncAsync, tmp, m_ptrDevice));
    tex = 0;
}

TMBool CTextureGLBase::ResetAsObject(const void* pData, EGXReset eReset)
{
    m_ptrDevice->m_criTexData.Lock();

    TMBool b = false;
    delete m_pReset;
    m_pReset = 0;

    switch (eReset)
    {
    case eGXResetCopyDib:
        {
            if (CallGetTID() == ThreadSelfId()) // sync
                ResetAsDib(*(GXDibData*)pData);
            else
                m_pReset = new CDataResetTex(this, pData);
            b = true;
        }
        break;
        
    case eGXResetCopyHIMAGE:
    case eGXResetMoveHIMAGE:
        if (CallGetTID() == ThreadSelfId()) // sync
            ReplaceByImage((HIMAGE)pData);
        else
            m_pReset = new CDataResetImage((HIMAGE)pData);
        b = true;
        break;
    }

    m_ptrDevice->m_criTexData.Unlock();
    return b;
}


////////////////////////////////////////////////////////////////////////////////
// CTextureGL
CTextureGL::CTextureGL()
    : m_texID(0)
{
//    gl_verify(glGenTextures(1, &m_texID));
}

CTextureGL::~CTextureGL()
{
    gl_trace("CTextureGL::~CTextureGL(this:%p, texid:%d)", this, m_texID);

    if (m_texID)
        DelTex(m_texID);
//    DbgAssert(m_texID);
//     if (m_texID)
//         gl_verify(glDeleteTextures(1, &m_texID));
//     _CGXAppGL_CallFlush(m_ptrDevice);
}

TMBool CTextureGL::Init(TMBool bInit)
{
    m_ptrDevice->_gx_check(false, false);
//     DbgAssert(m_texID);
    DbgAssert(m_width);
    DbgAssert(m_height);

    if (bInit && m_texID)
        DelTex(m_texID);

//     {
//         Call_DelTex::call(m_ptrDevice, m_texID);
//         m_texID = 0;
//     }
// 
//     gl_verify(glBindTexture(GL_TEXTURE_2D, m_texID));
//     m_ptrDevice->SamplersResetLinear();
//     gl_verify(glBindTexture(GL_TEXTURE_2D, 0));
// //      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
// //      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
// 
// //     gl_verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)); //GL_NEAREST);
// //     gl_verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));//GL_LINEAR); //GL_LINEAR);
// 
// //         if (img)
// //         {
// //             GLint fmt = ImageConvertForGL(img);
// //             void* pbits = ImageLockBits(img, true);
// //             glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, fmt, GL_UNSIGNED_BYTE, pbits);
// //         }
// //         else
// //         {
// //             glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
// //         }
// //     static byte p[2048 * 2048 * 4] = { 0 };
//     if (bInit)
//         InitAs(m_texID, GL_RGBA8, m_width, m_height, GL_RGBA);
// //        gl_verify(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
//     gl_verify(glBindTexture(GL_TEXTURE_2D, 0));
    gl_trace("CTextureGL::Init(dev:%p, w:%d, h:%d, bInit:%d, texid:%d)", m_ptrDevice, m_width, m_height, bInit, m_texID);
    return true;
}

TMUInt _CTextureGL_ImageConvertForGL(HIMAGE hImage)
{
    if (!hImage)
        return false;
    EGXFormat bpp = ImageGetFormat(hImage);
    if (bpp == eGXFormat32A)
        return GL_BGRA;
    ImageConvertBitCount(hImage, eGXFormat24); 
    return GL_BGR;
}

void CTextureGL::NativeBlt(_CDrawStripGL& ds)
{
    m_ptrDevice->_gx_check(true, false);

    SubmitResetData(false);

    m_ptrDevice->SetTexture1(m_texID);
#if GX_GL_USE_SHADER
    DbgAssert(m_ptrDevice->m_psNor.IsCurrent());
    m_ptrDevice->m_psTex.SetCurrent(m_ptrDevice);
    m_ptrDevice->DrawStrip(ds);
    m_ptrDevice->m_psNor.SetCurrent(m_ptrDevice);
#else
    gl_verify(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
    m_ptrDevice->DrawStrip(ds);
    gl_verify(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
#endif
    m_ptrDevice->SetTexture1(0);
}

// CTextureGLBase
// TMBool CTextureGL::ResetAsDib(const GXDibData& data)
// {
// 
// }

TMBool CTextureGL::ReplaceByImage(HIMAGE hImage)
{
    m_ptrDevice->_gx_check(true, false);
    GLint fmt = _CTextureGL_ImageConvertForGL(hImage);
    void* pbits = ImageLockBits(hImage, true);
    if (!pbits)
        return false;
    InitAs(m_texID, GL_RGBA8, m_width, m_height, fmt, pbits);
//     gl_verify(glBindTexture(GL_TEXTURE_2D, m_texID));
//     gl_verify(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, fmt, GL_UNSIGNED_BYTE, pbits));
//     gl_verify(glBindTexture(GL_TEXTURE_2D, 0));
	return true;
}

TMBool CTextureGL::Resize(int w, int h)
{
    gl_trace("CTextureGL::Resize(%d, %d)", w, h);
    m_ptrDevice->_gx_check(true, false);
    return 0 != _CTextureGL_CreateTex(m_ptrDevice, w, h, this, false);
}

// struct _CTextureGL_ResetAsDib
// {
//     CTextureGL* tex;
//     const void* data;
//     TMBool      b;
// 
//     static void f(void* pData)
//     {
//         _CTextureGL_ResetAsDib* p = (_CTextureGL_ResetAsDib*)pData;
//         p->b = p->tex->ResetAsDib(*(GXDibData*)p->data);
//     }
// };
// 
// 
// TMBool CTextureGL::ResetAsObject(const void* pData, EGXReset eReset)
// {
//     switch (eReset)
//     {
//     case eGXResetCopyDib:
//         {
//             _CTextureGL_ResetAsDib cr = { this, pData };
//             CallCreate(eCallTypeSync, cr.f, &cr);
//             return cr.b;
//         }
//         
//     case eGXResetCopyHIMAGE:
//     case eGXResetMoveHIMAGE:
//         return ReplaceByImage((HIMAGE)pData);
//     }
//     return false;
// }

void CTextureGL::GetSurface(IGXSurface** ppSurface)
{
    gl_trace("CTextureGL::CopyOut()");
    DbgAssert(!*ppSurface);
    CSurfaceGL* p = new CRefObjectT<CSurfaceGL>();
    p->Init(this);
    *ppSurface = p;
}

////////////////////////////////////////////////////////////////////////////////
// CTextureGLNV12
CTextureGLNV12::CTextureGLNV12()
{
//    gl_verify(glGenTextures(2, m_texID));
    m_texID[0] = 0;
    m_texID[1] = 0;
    m_texID[2] = 0;
}

CTextureGLNV12::~CTextureGLNV12()
{
    if (m_texID[0])
        DelTex(m_texID[0]);
    if (m_texID[1])
        DelTex(m_texID[1]);
    if (m_texID[2])
        DelTex(m_texID[2]);
// 
//     DbgAssert(m_texID[0]);
//     DbgAssert(m_texID[1]);
//     gl_verify(glDeleteTextures(tm_countof(m_texID), m_texID));
}

TMBool CTextureGLNV12::Init(TMBool bInit)
{
    // create texture
    m_ptrDevice->_gx_check(false, false);

    DbgAssert(m_width);
    DbgAssert(m_height);
//     DbgAssert(m_texID[0]);
//     DbgAssert(m_texID[1]);

    if (bInit)
    {
        if (m_texID[0])
            DelTex(m_texID[0]);
        if (m_texID[1])
            DelTex(m_texID[1]);
        if (m_texID[2])
            DelTex(m_texID[2]);
    }

    return true;
}

TMBool CTextureGLNV12::Resize(int w, int h)
{
    gl_trace("CTextureGLNV12::Resize(%d, %d)", w, h);
    m_ptrDevice->_gx_check(true, false);
    return 0 != _CTextureGL_CreateTex(m_ptrDevice, w, h, this, false);
}

void CTextureGLNV12::NativeBlt(_CDrawStripGL& ds)
{
    m_ptrDevice->_gx_check(true, false);
    DbgAssert(m_ptrDevice->m_psNor.IsCurrent());

    SubmitResetData(false);

    if (m_texID[2])
    {
        m_ptrDevice->m_pShaderPixel_NV12A.SetCurrent(m_ptrDevice);
        m_ptrDevice->SetTexture3(m_texID[0], m_texID[1], m_texID[2]);
    }
    else
    {
        m_ptrDevice->m_pShaderPixel_NV12.SetCurrent(m_ptrDevice);
        m_ptrDevice->SetTexture2(m_texID[0], m_texID[1]);
    }

    m_ptrDevice->DrawStrip(ds);

    m_ptrDevice->SetTexture3(0, 0, 0);
    m_ptrDevice->m_psNor.SetCurrent(m_ptrDevice);
}

struct CNV12CopyGL : public CNV12CopyBase
{
public:
    CTextureGLNV12* tex;
    TMBool b;

public:
    CNV12CopyGL(CTextureGLNV12* _tex) : tex(_tex), b(false)
    {
        tex->m_ptrDevice->_gx_check(true, false);
        tex->m_ptrDevice->LockDevice();
    }

    ~CNV12CopyGL()
    {
        tex->m_ptrDevice->UnlockDevice();
    }

private:
    virtual TMDibBuff LockRect(void* surface)
    {
        return TMDibBuff(0,0,0);
    }

    virtual void UnlockRect(void* surface)
    {
    }

    virtual void CopyData(void* pTex, int height, TMUInt8* plane, int stride)
    {
//         TMDibBuff dst(LockRect(pTex));
//         if (!dst)
//             return;
//         dst.m_height = height;
//         TMDibBuff src(plane, stride, height);
//         DibBltRef(dst, src);
//         UnlockRect(pTex);
    }
    
    virtual void CopyDataA(void* pTex, int width, int height, const CDibHandleGray& alpha)
    {
//         CTrueDib<CDibHandleGray> dst(LockRect(pTex));
//         if (!dst)
//             return;
//         dst.m_height = height;
//         CTrueDib<CDibHandleGray> src(alpha);
//         dst.SetWidth(width);
//         src.SetWidth(width);
//         DibStretch(dst, src);
//         UnlockRect(pTex);
    }

    virtual void ReleaseA()
    {
        DbgAssert(tex->m_texID[2]);
        gl_verify(glDeleteTextures(1, &tex->m_texID[2]));
        tex->m_texID[2] = 0;
    }

    virtual void* RecreateA(int w, int h)
    {
        GLuint ptr = tex->m_texID[2];
        if (!ptr)
            gl_verify(glGenTextures(1, &tex->m_texID[2]));
        return (void*)ptr;
    }
};

// struct _CTextureGLNV12L_ResetAsDib
// {
//     CTextureGLNV12* tex;
//     const void*     data;
//     TMBool          b;
// 
//     static void f(void* pData)
//     {
//         _CTextureGLNV12L_ResetAsDib* p = (_CTextureGLNV12L_ResetAsDib*)pData;
//         CNV12CopyGL cd(p->tex);
//         p->b = cd.ResetAsDib(*(GXDibData*)p->data, p->tex->GetWidth(), p->tex->GetHeight(), (void*)p->tex->m_texID[0], (void*)p->tex->m_texID[1]);
//     }
// };
// 
// TMBool CTextureGLNV12::ResetAsObject(const void* pData, EGXReset eReset)
// {
//     DbgAssert(pData);
// 
//     if (eReset == eGXResetCopyDib)
//     {
//         _CTextureGLNV12L_ResetAsDib cr = { this, pData };
//         CallCreate(eCallTypeSync, cr.f, &cr);
//         return cr.b;
//     }
//     return false;
// }

TMBool CTextureGLNV12::ResetAsDib(const GXDibData& data)
{
    m_ptrDevice->_gx_check(true, false);
    CNV12CopyGL cd(this);
    TMBool b = cd.ResetAsDib(data, GetWidth(), GetHeight(), (void*)m_texID[0], (void*)m_texID[1]);

    if (m_texID[0] && !m_ptrDevice->m_pShaderPixel_NV12)
    {
        CGXAppGL::PS_DESC descNV12      = { gl_PS_NV12 };
        m_ptrDevice->m_pShaderPixel_NV12 .Create(m_ptrDevice, descNV12,  m_ptrDevice->m_vs, cg_descVSGL);
    }
    if (m_texID[2] && !m_ptrDevice->m_pShaderPixel_NV12A)
    {
        CGXAppGL::PS_DESC descNV12A     = { gl_PS_NV12A };
        m_ptrDevice->m_pShaderPixel_NV12A.Create(m_ptrDevice, descNV12A, m_ptrDevice->m_vs, cg_descVSGL);
    }
    return b && m_ptrDevice->m_pShaderPixel_NV12;
}


////////////////////////////////////////////////////////////////////////////////
// CTextureGLRenderTarget
TMBool CTextureGLRenderTarget::Resize(int w, int h)
{
    gl_trace("CTextureGLRenderTarget::Resize(%d, %d)", w, h);
    return 0 != _CTextureGL_CreateTex(m_ptrDevice, w, h, this, true);
}

// CTextureGLBase
void CTextureGLRenderTarget::OnSetRenderTarget(CGXAppBase* app)
{
    m_ptrDevice->_gx_check(true, false);

    if (!m_texID)
    {
        InitAs(m_texID, GL_RGBA8, m_width, m_height, GL_RGBA);
        SubmitResetData(false);
    }

    // init fbo
    DbgAssert(!fboId);
    gl_verify(m_ptrDevice->glGenFramebuffers(1, &fboId));
    gl_verify(m_ptrDevice->glBindFramebuffer(GL_FRAMEBUFFER, fboId));

    // bind texture
//          glBindTexture(GL_TEXTURE_2D, 0);

    gl_trace("CTextureGLRenderTarget::OnSetRenderTarget(this:%p, texid:%d, app:%p)", this, m_texID, app);
    gl_verify(m_ptrDevice->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texID, 0));

    // rbo
    int w = m_width;
    int h = m_height;
//     gl_verify(m_ptrDevice->glGenRenderbuffers(1, &rboId));
//     gl_verify(m_ptrDevice->glBindRenderbuffer(GL_RENDERBUFFER, rboId));
//     gl_verify(m_ptrDevice->glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h)); // GL_DEPTH_COMPONENT16
//     gl_verify(m_ptrDevice->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboId));
// 
//     //cbo
//     GLuint colorRenderBuffer;
//     app->glGenRenderbuffers(1, &colorRenderBuffer);
//     app->glBindRenderbuffer(GL_RENDERBUFFER, colorRenderBuffer);
//     app->glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderBuffer);


    gl_verify(glViewport(0, 0, w, h));

    gl_check_fb();

// glDisable(GL_SCISSOR_TEST);
//     glClearColor(1, 0.586f, 0.949f, 0.2f);
//      glClear(GL_COLOR_BUFFER_BIT);
     glEnable(GL_SCISSOR_TEST);

// 
//      m_ptrDevice->m_psNor.ResetCurrent(m_ptrDevice);
//     glColor4ub(255, 0, 0, 255);
//     glRects(100, 50, 400, 300);
//     m_ptrDevice->m_psNor.SetCurrent(m_ptrDevice);

//     GLuint x;
//     gl_verify(glGenTextures(1, &x));
//     gl_verify(glBindTexture(GL_TEXTURE_2D, x));
//     gl_verify(glBindTexture(GL_TEXTURE_2D, 0));

//     glCheckFramebufferStatus



    return;
}


// void _TestDrawTriangleVAO_330(CGXAppGL* dev)
// {
//     const char* vsSrc =
//         "#version 330\n"
//         "layout(location=0) in vec2 aPos;\n"
//         "void main(){ gl_Position = vec4(aPos, 1.0, 1.0); }\n";
//     const char* fsSrc =
//         "#version 330\n"
//         "out vec4 FragColor;\n"
//         "void main(){ FragColor = vec4(1.0, 0.2, 0.2, 1.0); }\n";
// 
//     GLuint vs = dev->glCreateShader(GL_VERTEX_SHADER);
//     dev->glShaderSource(vs, 1, &vsSrc, NULL);
//     dev->glCompileShader(vs);
//     GLint ok = 0; dev->glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
//     if (!ok) { dev->glDeleteShader(vs); return; }
// 
//     GLuint fs = dev->glCreateShader(GL_FRAGMENT_SHADER);
//     dev->glShaderSource(fs, 1, &fsSrc, NULL);
//     dev->glCompileShader(fs);
//     dev->glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
//     if (!ok) { dev->glDeleteShader(vs); dev->glDeleteShader(fs); return; }
// 
//     GLuint prog = dev->glCreateProgram();
//     dev->glAttachShader(prog, vs);
//     dev->glAttachShader(prog, fs);
//     dev->glLinkProgram(prog);
//     GLint linkOK = 0; dev->glGetProgramiv(prog, GL_LINK_STATUS, &linkOK);
//     dev->glDeleteShader(vs);
//     dev->glDeleteShader(fs);
//     if (!linkOK) { dev->glDeleteProgram(prog); return; }
// 
//     GLfloat verts[] = {
//         -0.5f, -0.5f,
//          0.5f, -0.5f,
//          0.0f,  0.5f
//     };
// 
//     GLuint vao = 0, vbo = 0;
//     dev->glGenVertexArrays(1, &vao);
//     dev->glBindVertexArray(vao);
// 
//     dev->glGenBuffers(1, &vbo);
//     dev->glBindBuffer(GL_ARRAY_BUFFER, vbo);
//     dev->glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
// 
//     dev->glEnableVertexAttribArray(0);
//     dev->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void*)0);
// 
//     dev->glUseProgram(prog);
//     glDrawArrays(GL_TRIANGLES, 0, 3);
// 
//     dev->glBindVertexArray(0);
//     dev->glUseProgram(0);
//     dev->glDeleteBuffers(1, &vbo);
//     dev->glDeleteVertexArrays(1, &vao);
//     dev->glDeleteProgram(prog);
// }
// 
// void _TestDrawTriangleVAO_140(CGXAppGL* dev)
// {
//     const char* vsSrc =
//         "#version 140\n" "attribute vec2 aPos;\n"
//         "void main(){ gl_Position = vec4(aPos, 1.0, 1.0); }\n";
//     const char* fsSrc =
//         "#version 140\n"
//         "void main(){ gl_FragColor = vec4(1.0, 0.2, 0.2, 1.0); }\n";
// 
//     GLuint vs = dev->glCreateShader(GL_VERTEX_SHADER);
//     dev->glShaderSource(vs, 1, &vsSrc, NULL);
//     dev->glCompileShader(vs);
//     GLint ok = 0; dev->glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
//     if (!ok) { dev->glDeleteShader(vs); return; }
// 
//     GLuint fs = dev->glCreateShader(GL_FRAGMENT_SHADER);
//     dev->glShaderSource(fs, 1, &fsSrc, NULL);
//     dev->glCompileShader(fs);
//     dev->glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
//     if (!ok) { dev->glDeleteShader(vs); dev->glDeleteShader(fs); return; }
// 
//     GLuint prog = dev->glCreateProgram();
//     dev->glAttachShader(prog, vs);
//     dev->glAttachShader(prog, fs);
//     dev->glBindAttribLocation(prog, 0, "aPos");
//     dev->glLinkProgram(prog);
//     GLint linkOK = 0; dev->glGetProgramiv(prog, GL_LINK_STATUS, &linkOK);
//    dev-> glDeleteShader(vs); dev->glDeleteShader(fs);
//     if (!linkOK) { dev->glDeleteProgram(prog); return; }
// 
//     GLfloat verts[] = {
//         -0.5f, -0.5f,
//          0.5f, -0.5f,
//          0.0f,  0.5f
//     };
// 
//     GLuint vao = 0, vbo = 0;
//     dev->glGenVertexArrays(1, &vao);
//     dev->glBindVertexArray(vao);
// 
//     dev->glGenBuffers(1, &vbo);
//     dev->glBindBuffer(GL_ARRAY_BUFFER, vbo);
//     dev->glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
// 
//     dev->glEnableVertexAttribArray(0);
//     dev->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void*)0);
// 
//     dev->glUseProgram(prog);
//     glDrawArrays(GL_TRIANGLES, 0, 3);
// 
//     dev->glBindVertexArray(0);
//     dev->glUseProgram(0);
//     dev->glDeleteBuffers(1, &vbo);
//     dev->glDeleteVertexArrays(1, &vao);
//     dev->glDeleteProgram(prog);
// }

void CTextureGLRenderTarget::OnResetRenderTarget(CGXAppBase* app, GXPTexture pNew)
{
//     _TestDrawTriangleVAO_330(m_ptrDevice);
//    _TestDrawTriangleVAO_140(m_ptrDevice);
    m_ptrDevice->_gx_check(true, false);

    glFinish();

//     m_ptrDevice->m_psNor.SetCurrent(m_ptrDevice);

    DbgAssert(app == m_ptrDevice);
    m_ptrDevice->_gx_check(true, true);
    gl_trace("CTextureGLRenderTarget::OnResetRenderTarget(this:%p, texid:%d, app:%p, pnew:%p)", this, m_texID, app, pNew);

    gl_verify(m_ptrDevice->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0));
    gl_verify(m_ptrDevice->glBindFramebuffer(GL_FRAMEBUFFER, 0));
    gl_verify(m_ptrDevice->glDeleteFramebuffers(1, &fboId));
//     gl_check_fb();
//     gl_verify(m_ptrDevice->glDeleteRenderbuffers(1, &rboId));
    fboId = 0;
//     rboId = 0;
    CTextureGL::OnResetRenderTarget(app, pNew);
}


////////////////////////////////////////////////////////////////////////////////
// CTextureGLSwapChain
CTextureGLSwapChain::CTextureGLSwapChain()
    : m_wnd(0)
    , m_hdc(0)
{
    gl_trace("CTextureGLSwapChain::CTextureGLSwapChain(this:%p)", this);
    TM_IF_DBG(m_eType = eTextureTypeSwapChain);
}

CTextureGLSwapChain::~CTextureGLSwapChain()
{
    gl_trace("CTextureGLSwapChain::~CTextureGLSwapChain(this:%p)", this);
    ReleaseDC(m_wnd, m_hdc);
}

TMBool CTextureGLSwapChain::Init(HWND hWnd)
{
    gl_trace("CTextureGLSwapChain::Init(this:%p, dev:%p, hwnd:%p, w:%d, h:%d)", this, m_ptrDevice, hWnd, m_width, m_height);
    m_wnd = hWnd;
    return CTextureGLRenderTarget::Init(true);
}

// void testFBO(CGXAppGL* dev, int w, int h, GLuint& tex)
// {
//     // ***************************** Define a Second Output Buffer and Bind it for Writing
//     GLuint  fboId;
//     dev->glGenFramebuffers(1, &fboId);
//     dev->glBindFramebuffer(GL_FRAMEBUFFER, fboId);
//     
//     // ******************************* Set the Viewport to the Texture Size and Reshape
//     
//     glViewport(0,0,w, h);
//     
//     // ************************************** Create the Output Imgage Texture
//     GLuint  wispOutputTexture;
//     glGenTextures(1, &wispOutputTexture);
//     glBindTexture(GL_TEXTURE_2D, wispOutputTexture);
//     static char c[1024 * 1024 * 4] = {0};
// //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, c);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, c);
// 
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glBindTexture(GL_TEXTURE_2D, 0);
//     
//     // ********************************* Attach the Output Texture to the Frame Buffer
//     
//     dev->glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, wispOutputTexture, 0);
//     
//     
//     glClearColor(0.5f, 0.586f, 0.949f, 1.0f);	// rgb(33,150,243)
//     glClear(GL_COLOR_BUFFER_BIT);
//     
//                 glColor4ub(255, 0, 0, 255);
//     glRects(100, 50, 400, 300);
//     glColor4ub(255, 255, 255, 255);
//     glFlush();
// 
//     
//     // ************************************ Reset the Viewport and Frame Buffer
//     
//     dev->glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
//     
//     
//     glDeleteTextures(1, &tex);
//         tex = wispOutputTexture;
// }



// static void _CGXAppGL_InitFlipDevice(CGXAppGL* device)
// {
//     DbgAssert(device->m_psNor.IsCurrent());
//     device->m_psNor.ResetCurrent(device);
//     
//     //     gl_verify(glDisable(GL_DEPTH_TEST));
//     gl_verify(glDisable(GL_SCISSOR_TEST));
//     gl_verify(glDisable(GL_BLEND));
//     //     gl_verify(glDisable(GL_POINT_SMOOTH));
//     //     gl_verify(glDisable(GL_LINE_SMOOTH));
//     
//     gl_verify(glEnable(GL_TEXTURE_2D));
//     gl_verify(glEnableClientState(GL_VERTEX_ARRAY));
//     gl_verify(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
//     gl_verify(glDisableClientState(GL_COLOR_ARRAY));
// }
// 
// static void _CGXAppGL_UnInitFlipDevice(CGXAppGL* device)
// {
//     gl_verify(glEnable(GL_SCISSOR_TEST));
//     gl_verify(glEnable(GL_BLEND));
//     //     gl_verify(glEnable(GL_POINT_SMOOTH));
//     //     gl_verify(glEnable(GL_LINE_SMOOTH));
// #if GX_GL_USE_SHADER
//     gl_verify(glDisableClientState(GL_VERTEX_ARRAY));
//     gl_verify(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
//     device->m_psNor.SetCurrent(device);
// #else
//     gl_verify(glEnableClientState(GL_COLOR_ARRAY));
// #endif
// }
// 
// void CTextureGLSwapChain::DoPresent()
// {
//     gl_trace("CTextureGLSwapChain::DoPresent(this:%p,)", this);
// 
//     _CGXAppGL_InitFlipDevice(m_ptrDevice);
// //      glClearColor(0.129f, 0.586f, 0.949f, 1.0f);	// rgb(33,150,243)
// //      glClear(GL_COLOR_BUFFER_BIT);
// 
// 
//     gl_verify(glViewport(0, 0, m_width, m_height));
//     gl_verify(glBindTexture(GL_TEXTURE_2D, m_texID));
// 
//     struct VCT { GLfloat dx, dy, sx, sy; };
//     VCT pv[4] = {
//         -1, -1, 0, 0,
//          1, -1, 1, 0,
//         -1,  1, 0, 1,
//          1,  1, 1, 1,
//     };
//     gl_verify(glMatrixMode(GL_PROJECTION));
//     gl_verify(glLoadIdentity());
//     gl_verify(glVertexPointer(2, GL_FLOAT, sizeof(VCT), &(pv->dx)));
//     gl_verify(glTexCoordPointer(2, GL_FLOAT, sizeof(VCT), &(pv->sx)));
// //    gl_verify(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, pi));
//     gl_verify(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
// //    gl_verify(glDrawArrays(GL_TRIANGLE_STRIP, 0, 6));
//     gl_verify(glBindTexture(GL_TEXTURE_2D, 0));
// 
//     _CGXAppGL_UnInitFlipDevice(m_ptrDevice);
// 
// }


void CTextureGLSwapChain::DoPresent()
{
    m_ptrDevice->_gx_check(true, false);

    GLuint fbo;
    m_ptrDevice->DisableScissor();
    gl_verify(m_ptrDevice->glGenFramebuffers(1, &fbo));
    gl_verify(m_ptrDevice->glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo));
    gl_verify(m_ptrDevice->glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texID, 0));
    gl_verify(m_ptrDevice->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
//     glDrawBuffer(GL_FRONT);
    gl_verify(m_ptrDevice->glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST));
    gl_verify(m_ptrDevice->glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
    gl_verify(m_ptrDevice->glBindFramebuffer(GL_FRAMEBUFFER, 0));
    gl_verify(m_ptrDevice->glDeleteFramebuffers(1, &fbo));
    m_ptrDevice->EnableScissor();
}

TMBool CTextureGLSwapChain::Resize(int w, int h)
{
    m_ptrDevice->_gx_check(true, false);
    gl_trace("CTextureGLRenderTarget::Resize(%d, %d)", w, h);
    return 0 != _CTextureGL_CreateTex(m_ptrDevice, w, h, this, m_wnd);
}

EGXRet CTextureGLSwapChain::Present(TMHWnd hWnd, EGXPresent eFlag, GXPRESENTFX* fx)
{
//     DbgTrace("CTextureGLSwapChain::Present(hWnd:%p, this:%p) entry", hWnd, this);
    m_ptrDevice->_gx_check(true, false);

#if GX_GL_USE_SHADER
    DbgAssert(!m_ptrDevice->m_pVertex.IsCurrent());
#endif

    if (eFlag == eGXPresentLayered)
        return this->PresentLayeredWin(hWnd, eFlag, fx);// PresentLayeredWin(hWnd, eFlag, fx);

    //DbgAssert(!fx);
    gl_trace("CTextureGLSwapChain::Present(this:%p, hDest:%p)", this, hWnd);
    { CPaintDC dc(hWnd); }

#ifdef TM_DBG
    CRect rc;
    GetClientRect(hWnd, &rc);
//     DbgAssert(rc.Width() == m_width);
//     DbgAssert(rc.Height() == m_height);
#endif

    if (gxGetApp() != m_ptrDevice)
        return eGXRetDeviceChanged;
    
    // init device
    if (!m_hdc)
    {
        m_hdc = GetDC(m_wnd);
        m_ptrDevice->InitDC(m_hdc);
    }

    {
//        CAutoContext ac(m_hdc, m_ptrDevice->m_hrcDev);// m_hrc);
        CAutoContext ac(m_hdc, m_ptrDevice->m_wndDevice.hRC);
//     glClearColor(1, 0.586f, 0.949f, 1.0f);	// rgb(33,150,243)
//     glClear(GL_COLOR_BUFFER_BIT);
// 
//          testFBO(m_ptrDevice, m_width, m_height, m_texID);
        DoPresent();
        
        gl_verify(glFlush());
        gl_verify(glFinish());
//         SwapBuffers(m_hdc);
//          GdiFlush();
//         Sleep(1);
    }
//     ::ReleaseDC(m_wnd, m_hdc);
//     m_hdc = GetDC(m_wnd);

//     DbgTrace("CTextureGLSwapChain::Present(hWnd:%p, this:%p) leave", hWnd, this);

    return eGXRetSuccess;
}


////////////////////////////////////////////////////////////////////////////////
// CGXAppGL
CGXAppGL::CGXAppGL()
    : m_pBk (0)
{
    gl_trace("CGXAppGL::CGXAppGL(this:%p)", this);
}

CGXAppGL::~CGXAppGL()
{
    gl_trace("CGXAppGL::~CGXAppGL(this:%p)", this);

    _CGXAppGL_CallFlush(this);
    m_pIndex.Release(this);
    m_pVertex.Release(this);
    m_cbufferVS.Release(this);

//     gl_verify(wglMakeCurrent(0, 0));
//     gl_verify(wglDeleteContext(m_hrcDev));
//     ReleaseDC(m_hDevice, m_hdcDev);
//     CTextureGLBase::m_ptrDevice = 0;
}

//CGXAppGL* CGXAppGL::CreateInst()
TMBool CGXAppGL::CreateDevice()
{
    if (!CGLDevice::CreateDevice())
        return false;

    DbgVerify(wglMakeCurrent(m_wndDevice.hDC, m_wndDevice.hRC));

    gl_verify(glDisable(GL_DEPTH_TEST));
    gl_verify(glDisable(GL_CULL_FACE));
    EnableScissor();
    
//    gl_verify(glEnable(GL_BLEND));
//    gl_verify(device->glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA));
    gl_verify(glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA));
//         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//         glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
//    glBlendFunc(GL_SRC_ALPHA,GL_ONE);

    m_font = CGXFont::FontCreateDefaultFromSys();

    PS_DESC descPS      = { gl_PS,  0 };
    PS_DESC descPSTex   = { gl_PS_Tex, 0 };
    DbgReturn0HResult(ShaderInit(cg_descVSGL, descPS, descPSTex, sizeof(CBuffer)));

#if GX_GL_USE_SHADER
    m_pVertex.Init(this, &m_pIndex, &cg_descVSGL);
#else
    m_psNor.ResetCurrent(this);
    gl_verify(glEnableClientState(GL_VERTEX_ARRAY));
    gl_verify(glEnableClientState(GL_COLOR_ARRAY));
    gl_verify(glEnable(GL_TEXTURE_2D));
    gl_verify(glDisable(GL_COLOR_MATERIAL));
    gl_verify(glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST));
#endif
//     m_pVertex.ResetCurrent(this);

    return true;
}

void CGXAppGL::SamplersSetLinear()
{
    gl_verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)); //GL_NEAREST);
    gl_verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));//GL_LINEAR); //GL_LINEAR);
}

void CGXAppGL::SamplersResetLinear()
{
    gl_verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)); //GL_NEAREST);
    gl_verify(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));//GL_LINEAR); //GL_LINEAR);
}

void CGXAppGL::BlendSetOver()
{
    gl_verify(glEnable(GL_BLEND));
}

void CGXAppGL::BlendSetCopy()
{
    gl_verify(glDisable(GL_BLEND));
}

void CGXAppGL::BlendSetInvert()
{
    gl_verify(glEnable(GL_COLOR_LOGIC_OP));
    gl_verify(glLogicOp(GL_INVERT));
#if !GX_GL_USE_SHADER
    gl_verify(glDisableClientState(GL_COLOR_ARRAY));
#endif
}

void CGXAppGL::BlendResetInvert()
{
    gl_verify(glDisable(GL_COLOR_LOGIC_OP));
#if !GX_GL_USE_SHADER
    gl_verify(glEnableClientState(GL_COLOR_ARRAY));
#endif
}

void CGXAppGL::ClipDevNow()
{
    CRect rc(m_rcClip);
//     GXRect rcx(m_rcClip);
// //     if (m_nWidth)
// //     {
// //         GXDips n = (GXDips)m_nWidth - rcx.left;
// //         rcx.left = (GXDips)m_nWidth - rcx.right;
// //         rcx.right = n;
// //     }
//     rcx.Round(rc);
    
    if (IsRTL())
    {
        int w = m_pBk->GetWidth();
        int n = w - rc.left;
        rc.left = w - rc.right;
        rc.right = n;
    }

    tm_setmax(rc.left, 0);
    tm_setmax(rc.top, 0);
    DbgAssert(m_pBk);
    if (m_pBk)
        rc.KeepHSetT(m_pBk->m_height - rc.bottom);
//     {
//         gl_verify(glScissor(rc.left, m_pBk->m_height - rc.bottom, rc.Width(), rc.Height()));
//         gl_trace("CGXAppGL::ClipDevNow1(%d,%d,%d,%d)", rc.left, m_pBk->m_height - rc.bottom, rc.Width(), rc.Height());
//     }
//     else
    {
        this->SetScissorRect(rc);
//         gl_verify(glScissor(rc.left, rc.top, rc.Width(), rc.Height()));
        gl_trace("CGXAppGL::ClipDevNow2(%d,%d,%d,%d)", rc.left, rc.top, rc.Width(), rc.Height());
    }

//     CRect rc(m_rcClip);
//     if (IsRTL())
//     {
//         int w = m_pBk->GetWidth();
//         int n = w - rc.left;
//         rc.left = w - rc.right;
//         rc.right = n;
//     }
//     rc &= CRect(0, 0, m_pBk->GetWidth(), m_pBk->GetHeight());
//     this->SetScissorRect(rc);
//     gl_trace("CGXAppGL::ClipDevNow(" TM_FMT_RCI_STR ")", TM_FMT_RCI_VAL(rc));

}

void CGXAppGL::InitDevTransform()
{
    _gx_check();

    float fDPI = GetDPI();
    float f1 = (float)TM_DPI_SYSTEM_DEFAULT / fDPI;
    float f2 = fDPI / (float)TM_DPI_SYSTEM_DEFAULT;
    CAffine aff(f1,0, 0,f1, 0,0);
    aff *= m_aff;
    aff.Scale(f2, f2);
    
    float w = m_pBk->GetWidth();
    float h = m_pBk->GetHeight();
    const float df = 1.0f;
    CAffine affProj;
    if (IsRTL())
        affProj.Set(-2.0f / (float)w, 0, 0, -2.0f / h, 1, 1);
    else
        affProj.Set(2.0f / (float)w, 0, 0, -2.0f / h, -1, 1);
    
    //affProj *= aff;
    aff *= affProj;
    
    GLfloat mt[] =
    {
        aff.M11(), aff.M12(), 0, 0,
        aff.M21(), aff.M22(), 0, 0,
        0, 0, 1, 0,
        aff.Dx(), aff.Dy(), 0, 1,
    };
    gl_trace("CGXAppGL::InitDevTransform(this:%p, %f,%f, %f,%f, %f,%f)",
        this, aff.M11(), aff.M12(), aff.M21(), aff.M22(), aff.Dx(), aff.Dy());

#if GX_GL_USE_SHADER
    GetVSCBufffer()->SetDataVec4(this, &mt, 4);
#else
    gl_verify(glMatrixMode(GL_PROJECTION));
    gl_verify(glLoadMatrixf(mt));
#endif
}

void CGXAppGL::DrawStrip(CDrawStrip& ds)
{
#if GX_GL_USE_SHADER
    DbgAssert(!m_pVertex.IsCurrent());

    m_pVertex.Recreate(this, ds.pData, ds.nBufferSize); 
    m_pVertex.SetCurrent(this);
    m_pVertex.Draw(this, ds.mode, ds.count);
    m_pVertex.ResetCurrent(this);
#else
    gl_verify(glVertexPointer(2, GL_FLOAT, sizeof(CGLVertexX), &(ds.pData->Pos)));
    gl_verify(glColorPointer(4, GL_FLOAT, sizeof(CGLVertexX), &(ds.pData->Dif)));
    gl_verify(glTexCoordPointer(2, GL_FLOAT, sizeof(CGLVertexX), &(ds.pData->Tex)));

//     gl_verify(glVertexPointer(2, GL_SHORT, sizeof(CGLVertexX), &(ds.pData->x)));
//     gl_verify(glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(CGLVertexX), &(ds.pData->r)));
//     gl_verify(glTexCoordPointer(2, GL_FLOAT, sizeof(CGLVertexX), &(ds.pData->tu)));
    gl_verify(glDrawArrays(ds.mode, 0, ds.count));
#endif
}


EGXRet CGXAppGL::InitProp(GXPTexture texture, int nDPI, TMUInt32 dwFlag)
{
    return _GXApp_InitProp(this, static_cast<CTextureGLRenderTarget*>(texture), nDPI, dwFlag);
}

GXPTexture CGXAppGL::GetRenderTarget()
{
    return m_pBk;
}

// TMBool CGXAppGL::Clip(const GXRect& rc, TMBool bIntersect, TMBool bAutoPush)
// {
//     GXRect rcx(rc);
//     rcx.OffsetRect(m_aff.Dx(), m_aff.Dy());
//     return CGXAppBase::Clip(rcx, bIntersect, bAutoPush);
// }

void CGXAppGL::TextureCreateRenderTarget(IGXTexture** tex, int w, int h, EGXFormat format)
{
    gl_trace("CGXAppGL::TextureCreateRenderTarget");
    DbgAssert(!*tex);
    *tex = _CTextureGL_CreateTex(this, w, h, (CTextureGLRenderTarget*)0, true);

//     gl_trace("CGXAppGL::TextureCreateRenderTarget");
//     DbgAssert(!*tex);
//     CTextureGLRenderTarget* p = new CRefObjectT<CTextureGLRenderTarget>(1);
//     if (p->Init(this, w, h, true))
//         *tex = p;
//     else
//         delete p;
}

void CGXAppGL::TextureCreateSwapChain(IGXTexture** tex, TMHWnd hWnd, int w, int h, EGXFormat format)
{
    gl_trace("CGXAppGL::TextureCreateSwapChain");
    DbgAssert(!*tex);
    *tex = _CTextureGL_CreateTex(this, w, h, (CTextureGLSwapChain*)0, hWnd);
//     CTextureGLSwapChain* p = new CRefObjectT<CTextureGLSwapChain>(1);
//     if (p->Init(this, hWnd, w, h))
//         *tex = p;
//     else
//         delete p;
}

void CGXAppGL::TextureCreateIndirect(IGXTexture** tex, int w, int h, EGXFormat format)
{
    gl_trace("CGXAppGL::TextureCreateIndirect");
    DbgAssert(!*tex);
    switch (format)
    {
//     case eGXFormat16:
//     case eGXFormat24:
    case eGXFormat32A:
    case eGXFormat32X:  *tex = _CTextureGL_CreateTex(this, w, h, (CTextureGL*)0, false); return;
    case eGXFormatNV12: *tex = _CTextureGL_CreateTex(this, w, h, (CTextureGLNV12*)0, false); return;
    default: DbgTrace("CGXAppGL::TextureCreateIndirect(%d, %d, %d) fail", w, h, format); return;
    }
}

////////////////////////////////////////////////////////////////////////////////
// shape
void CGXAppGL::Clear(TMColor clr)
{
    _gx_check();
    this->DisableScissor();
    gl_verify(glClearColor(ColorRF(clr), ColorGF(clr), ColorBF(clr), ColorAF(clr)));
    gl_verify(glClear(GL_COLOR_BUFFER_BIT));
    this->EnableScissor();
}


void CGXAppGL::VertexFill(const GXIndices* pt, TMUInt nt)
{
    typedef CVertexListForInd::value_type value_type;
//     glEnableClientState(GL_VERTEX_ARRAY);
//     gl_verify(glEnableClientState(GL_COLOR_ARRAY));
//     BlendSetCopy(0);
#if GX_GL_USE_SHADER
    DbgAssert(m_psNor.IsCurrent());
    DbgAssert(!m_pVertex.IsCurrent());
    m_pVertex.Recreate(this, m_vertex.buffer(), m_vertex.sizeBuffer()); 
    m_pIndex.Recreate(this, pt, nt * sizeof(GLuint));

    m_pVertex.SetCurrent(this);
    m_pIndex.DrawUINT(this, GL_TRIANGLES, nt, pt);
    m_pVertex.ResetCurrent(this);
#else
    gl_verify(glVertexPointer(2, GL_FLOAT, sizeof(value_type), &(m_vertex.m_aTv[0].Pos)));// &(pv->x));
    gl_verify(glColorPointer(4, GL_FLOAT, sizeof(value_type), &(m_vertex.m_aTv[0].Dif)));
    gl_verify(glDrawElements(GL_TRIANGLES, nt, GL_UNSIGNED_INT, pt));
#endif
//     BlendResetCopy();
//    gl_verify(glDisableClientState(GL_COLOR_ARRAY));
//     glDisableClientState(GL_VERTEX_ARRAY);
}



TM_END_NAMESPACE

