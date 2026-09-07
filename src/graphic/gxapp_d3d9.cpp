/*
 *  @file
 *  @brief  D3D9 render
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2011.4.13
 *
 *  Copyright (C) 2011 miragekiller
 */

#include "gxapp_d3d9.h"
#include "../tmplatform_win.h"
#include "../tmdebugext.h"

#include "hlsl/_d9_VS.h"
#include "hlsl/_d9_PS.h"
#include "hlsl/_d9_PS_tex.h"
#include "hlsl/_d9_PS_NV12.h"
#include "hlsl/_d9_PS_NV12A.h"
#include "hlsl/_d9_layout.h"



// #define TYPEMODE_C_DECL9
// #include "hlsl\gxvs_i.h"

#ifdef GXCANVAS_H
#define TYPEMODE_C_DECL9
#include "hlsl\gxvs2d_i.h"
#endif

static const CD3D9Device::VS_DESC cg_descVS9 = { cg3_VS, layout_d9_VS_INPUT1 };


TM_BGN_NAMESPACE

////////////////////////////////////////////////////////////////////////////////
// CSurfaceD3D9
struct CSurfaceD3D9 : public IGXSurfaceImpl
{
private:
    CRefPtr<CTextureD3D9> m_tex;
    CComPtr<IDirect3DSurface9> m_surface;
    TMBool m_bWriteBack;

public:
    TM_UNUSED_IC_GetDebugInfo();

    void Init(CTextureD3D9* tex)
    {
        tex->m_ptrDevice->LockDevice();
        m_tex = tex;
    }

    ~CSurfaceD3D9()
    {
        m_tex->m_ptrDevice->UnlockDevice();
    }
    
private:
    void MakeCache(EGXLockRect flag)
    {
        if (m_tex->m_ddsd.Usage & D3DUSAGE_RENDERTARGET) // rendertarget & swapchain can't getdc/lockrect
        {
#ifdef TM_DBG
            HDC hdc;
            DbgAssert(FAILED(m_tex->m_ptrSurface->GetDC(&hdc)));
#endif
            HRESULT hr;
            IDirect3DDevice9* device = m_tex->m_ptrDevice->m_pd3dDevice9;
            DbgVerifyHResult(hr = device->CreateOffscreenPlainSurface(
                m_tex->GetWidth(),
                m_tex->GetHeight(),
                m_tex->m_ddsd.Format,
                D3DPOOL_SYSTEMMEM,
                &m_surface,
                NULL));
            if (flag != GX_LOCK_RECT_WRITE)
            {
                DbgVerifyHResult(hr = device->GetRenderTargetData(m_tex->m_ptrSurface, m_surface));
                if (FAILED(hr))
                {
                    m_tex->m_ptrDevice->CheckError(hr);
                    m_surface.Release();
                }
            }
            m_bWriteBack = (flag != GX_LOCK_RECT_READ);
        }
        else
        {
            m_surface = m_tex->m_ptrSurface;
            m_bWriteBack = false;
        }
    }

    void WriteBack()
    {
        DbgAssert(m_surface);
        if (!m_bWriteBack)
            return;
        DbgAssert(m_surface != m_tex->m_ptrSurface);
        DbgVerifyHResult(m_tex->m_ptrDevice->m_pd3dDevice9->UpdateSurface(m_surface, 0, m_tex->m_ptrSurface, 0));
    }

    virtual void LockRect(TMDibBuff& dib, const TMRect* rc, EGXLockRect flag) tm_override
    {
        DbgAssert(!m_surface);
        MakeCache(flag);
        if (!m_surface)
            return; // maybe lost.

        D3DLOCKED_RECT dr = {0};
        HRESULT hr;
        DWORD f[3] = { D3DLOCK_READONLY, D3DLOCK_DISCARD, 0 };
        DbgAssert(flag >= GX_LOCK_RECT_READ && flag <= GX_LOCK_RECT_READWRITE);
        DbgVerifyHResult(hr = CD3D9Device::_D3D9On12Bugs::LockSur9(m_tex->m_ptrDevice, m_surface, dr, rc, f[flag - 1]));
        if (FAILED(hr))
            m_tex->m_ptrDevice->CheckError(hr);
        else
            dib.Set(dr.pBits, dr.Pitch, rc ? rc->bottom - rc->top : m_tex->GetHeight());
    }
    
    virtual void UnlockRect() tm_override
    {
        DbgAssert(m_surface);
        DbgVerifyHResult(m_surface->UnlockRect());
        WriteBack();
    }
    
    virtual TMHDC GetDC(TMBool bDiscard) tm_override
    {
        DbgAssert(!m_surface);
        MakeCache(bDiscard ? GX_LOCK_RECT_WRITE : GX_LOCK_RECT_READWRITE);
        HDC hdc = 0;
        if (m_surface)
            DbgVerifyHResult(m_surface->GetDC(&hdc));
        return hdc;
    }

    virtual void ReleaseDC(TMHDC hdc) tm_override
    {
        if (m_surface)
        {
            DbgVerifyHResult(m_surface->ReleaseDC(hdc));
            WriteBack();
        }
    }

//     virtual void ToTexture(IGXTexture** ppTex) tm_override
//     {
//         IGXTexture* p = m_tex;
//         p->AddRef();
//         *ppTex = p;
//     }
};

////////////////////////////////////////////////////////////////////////////////
// CTextureD3D9Base
// IGXTexture
CTextureD3D9Base::~CTextureD3D9Base()
{
    GX_TRACE(L"CTextureD3D9Base::~CTextureD3D9Base(%d, %d); AvailableMem:%dM",
        m_ddsd.Width, m_ddsd.Height, D3D9GetFreeMem(m_ptrDevice->m_pd3dDevice9));
}

IGXApp* CTextureD3D9Base::GetApp()
{
    return m_ptrDevice;
}


////////////////////////////////////////////////////////////////////////////////
// CTextureD3D9
CTextureD3D9* CTextureD3D9::CreateTex(CGXAppD3D9* app, int w, int h, D3DFORMAT fmt, DWORD Usage, D3DPOOL Pool, CTextureD3D9* pTo)
{
    HRESULT hr;
    CComPtr<IDirect3DTexture9> ptrTexture;
    FAILED_CHECK(app, pTo, app->NativeCreateTexture(w, h, Usage, fmt, Pool, &ptrTexture));
    DbgAssert(ptrTexture);
    DbgAssert(ptrTexture->GetLevelCount() == 1);

    CComPtr<IDirect3DSurface9> ptrSurface;
    FAILED_CHECK(app, pTo, ptrTexture->GetSurfaceLevel(0, &ptrSurface));
    DbgAssert(ptrSurface);

    if (!pTo)
        pTo = CTextureBltT<CTextureD3D9, CGXAppD3D9::CVertex_Texture>::Inst();//  new CRefObjectT<CTextureD3D9>(1);
    TM_IF_DBG(pTo->m_eType = (Usage ? eTextureTypeRenderTarget : eTextureTypeTexture));
    pTo->Init(app, ptrSurface);
    return pTo;
}

void CTextureD3D9::Init(CGXAppD3D9* app, IDirect3DSurface9* ptrSurface)
{
    DbgAssert(app);
    DbgAssert(ptrSurface);
    m_ptrDevice = app;
    m_ptrSurface = ptrSurface;
    MemZeroT(m_ddsd);
    ptrSurface->GetDesc(&m_ddsd);
    DBG_INIT(this);

//     HDC hdc = CreateDC();
//     if (hdc)
//         DeleteDC(hdc);
// 
//     CDibHandle32 dib;
//     LockRect(dib, 0, GX_LOCK_RECT_READ);
//     UnlockRect();
}

void CTextureD3D9::NativeBlt(_CDrawStrip& ds)
{
    m_ptrDevice->SetTextureAt(0, m_ptrSurface);
//    m_ptrDevice->SetPixelShader(m_ptrDevice->m_pShaderPixel_Tex);
    m_ptrDevice->m_psTex.SetCurrent(m_ptrDevice);
    m_ptrDevice->DrawStrip(ds);
    m_ptrDevice->m_psNor.SetCurrent(m_ptrDevice);
//    m_ptrDevice->SetPixelShader(m_ptrDevice->m_pShaderPixel);
//    DbgVerifyHResult(m_ptrDevice->m_pd3dDevice9->SetTexture(0, 0));
    m_ptrDevice->SetTexture1(0);
}

void CTextureD3D9::ColorFill(TMColor clr)
{
    IDirect3DSurface9* ptrSurface = GetSurface9();
    DbgAssert(ptrSurface);
    if (SUCCEEDED(m_ptrDevice->m_pd3dDevice9->ColorFill(ptrSurface, 0, clr))) // only support render target.
        return;
    NativeColorFill(clr);
}

void CTextureD3D9::OnSetRenderTarget(CGXAppBase* app)
{
    GX_TRACE("CTextureD3D9::OnSetRenderTarget(%p)", app);
    DbgAssert(app == m_ptrDevice);
    IDirect3DSurface9* ptrSurface = GetSurface9();
    IDirect3DDevice9* pd3dDevice9 = m_ptrDevice->m_pd3dDevice9;
    DbgAssert(ptrSurface);
    
    m_ptrDevice->CD3D9Device::SetRenderTarget(ptrSurface);

    // view port
//     D3DVIEWPORT9 vp;
//     vp.X = vp.Y = 0;
//     vp.Width = m_ddsd.Width;
//     vp.Height = m_ddsd.Height;
//     vp.MinZ = 0;
//     vp.MaxZ = 1;
//     pd3dDevice9->SetViewport(&vp);
    
    HRESULT hr;
    if (!m_ptrDevice->m_pBk)
        DbgVerifyHResult(hr = pd3dDevice9->BeginScene());
}

void CTextureD3D9::OnResetRenderTarget(CGXAppBase* app, GXPTexture pNew)
{
    GX_TRACE("CTextureD3D9::OnResetRenderTarget(%p)", app);
    DbgAssert(app == m_ptrDevice);
    if (!pNew)
    {
        HRESULT hr;
        DbgVerifyHResult(hr = m_ptrDevice->m_pd3dDevice9->EndScene());
        DbgVerifyHResult(hr = m_ptrDevice->m_pd3dDevice9->SetRenderTarget(0, m_ptrDevice->m_ptrOldRenderTarget));
        DbgAssert(SUCCEEDED(hr));
    }
}

struct CCopySurface
{
    CComQIPtr<IDirect3DSurface9> ptrSurface;
    CComQIPtr<IDirect3DTexture9> ptrTexture;
    CComQIPtr<IDirect3DSwapChain9> ptrSwapChain;
    
    CCopySurface(IUnknown* punknown)
    {
        ptrSurface = punknown;
        if (ptrSurface)
            return;
        ptrTexture = punknown;
        if (ptrTexture)
        {
            DbgVerifyHResult(ptrTexture->GetSurfaceLevel(0, &ptrSurface));
            return;
        }
        ptrSwapChain = punknown;
        if (ptrSwapChain)
            DbgVerifyHResult(ptrSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &ptrSurface));
    }

    TMBool CopyTo(CTextureD3D9* p)
    {
        if (!ptrSurface)
            return false;
        IDirect3DDevice9* pDevice = p->m_ptrDevice->m_pd3dDevice9;
        if (SUCCEEDED(pDevice->StretchRect(ptrSurface, NULL, p->m_ptrSurface, NULL, D3DTEXF_NONE)))
            return true;
        if (SUCCEEDED(pDevice->UpdateSurface(ptrSurface, NULL, p->m_ptrSurface, NULL)))
            return true;
        return false;
    }
    
    TMBool MoveTo(CTextureD3D9* p)
    {
        if (!ptrSurface)
            return false;
        p->Init(p->m_ptrDevice, ptrSurface);
        return true;
    }
};

TMBool CTextureD3D9::Resize(int w, int h)
{
    GX_TRACE("CTextureD3D9::Resize(%d, %d)", w, h);
    return 0 != CreateTex(m_ptrDevice, w, h, m_ddsd.Format, m_ddsd.Usage, m_ddsd.Pool, this);
}

TMBool CTextureD3D9::ResetAsObject(const void* pData, EGXReset eReset)
{
    DbgAssert(pData);

    CAutoMsgAnalysis awm("CTextureD3D9::ResetAsObject");

    switch (eReset)
    {
    case eGXResetCopyDib:
        return ResetAsDib(*(GXDibData*)pData);
        
    case eGXResetCopyHIMAGE:
    case eGXResetMoveHIMAGE:
        return ReplaceByImage((HIMAGE)pData);

    case eGXResetCopyIUnknown:
        {
            CCopySurface cs((IUnknown*)pData);
            return cs.CopyTo(this);
        }

    case eGXResetMoveIUnknown:
        {
            CCopySurface cs((IUnknown*)pData);
            return cs.MoveTo(this);
        }
    }
    return false;
}

void CTextureD3D9::GetSurface(IGXSurface** ppSurface)
{
    GX_TRACE("CTextureD3D9::CopyOut()");
    DbgAssert(!*ppSurface);
    CSurfaceD3D9* p = new CRefObjectT<CSurfaceD3D9>();
    p->Init(this);
    *ppSurface = p;
}


////////////////////////////////////////////////////////////////////////////////
// CTextureD3D9NV12
CTextureD3D9NV12* CTextureD3D9NV12::CreateTex(CGXAppD3D9* pD3D, int w, int h, D3DPOOL Pool, CTextureD3D9NV12* pTo)
{
    // create texture
    HRESULT hr;
    CComPtr<IDirect3DTexture9> Y,U;
    FAILED_CHECK(pD3D, 0, pD3D->NativeCreateTexture(w, h, D3DUSAGE_DYNAMIC, D3DFMT_L8, Pool, &Y));
    DbgAssert(Y);
    FAILED_CHECK(pD3D, 0, pD3D->NativeCreateTexture(w / 2, h / 2, D3DUSAGE_DYNAMIC, D3DFMT_A8L8, Pool, &U));
    DbgAssert(U);

    if (!pD3D->m_pShaderPixel_NV12)
    {
        CGXAppD3D9::PS_DESC descNV12      = { cg3_PS_NV12 };
        CGXAppD3D9::PS_DESC descNV12A     = { cg3_PS_NV12A };
        pD3D->m_pShaderPixel_NV12 .Create(pD3D, descNV12);
        pD3D->m_pShaderPixel_NV12A.Create(pD3D, descNV12A);
    }

    if (!U || !pD3D->m_pShaderPixel_NV12 || !pD3D->m_pShaderPixel_NV12A)
        return 0;
    
    if (!pTo)
        pTo = CTextureBltT<CTextureD3D9NV12, CGXAppD3D9::CVertex_Texture>::Inst();// new CRefObjectT<CTextureD3D9NV12>(1);
    pTo->Init(pD3D, Y);
    pTo->m_ddsd.Format = (D3DFORMAT)eGXFormatNV12;
    pTo->m_ptr[0] = Y;
    pTo->m_ptr[1] = U;
    
    return pTo;
}

void CTextureD3D9NV12::Init(CGXAppD3D9* app, IDirect3DTexture9* pTexture)
{
    DbgAssert(app);
    DbgAssert(pTexture);
    m_ptrDevice = app;
    MemZeroT(m_ddsd);
    pTexture->GetLevelDesc(0, &m_ddsd);
}

void CTextureD3D9NV12::NativeBlt(_CDrawStrip& ds)
{
    IDirect3DDevice9* pd3dDevice9 = m_ptrDevice->m_pd3dDevice9;
    IDirect3DBaseTexture9* p[] = { m_ptr[0], m_ptr[1], m_ptr[2] };


//     DbgVerifyHResult(pd3dDevice9->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA));
    if (m_ptr[2])
    {
        m_ptrDevice->m_pShaderPixel_NV12A.SetCurrent(m_ptrDevice);
//        m_ptrDevice->SetPixelShader(m_ptrDevice->m_pShaderPixel_NV12A);
        m_ptrDevice->SetTexture3(m_ptr[0], m_ptr[1], m_ptr[2]);
    }
    else
    {
        m_ptrDevice->m_pShaderPixel_NV12.SetCurrent(m_ptrDevice);
//        m_ptrDevice->SetPixelShader(m_ptrDevice->m_pShaderPixel_NV12);
        m_ptrDevice->SetTexture2(m_ptr[0], m_ptr[1]);
    }

//     m_ptrDevice->SetVertexShader(m_ptrDevice->m_pShaderVertex); // m_pShaderVertex
//     ds.FVF = 0;

    m_ptrDevice->DrawStrip(ds);

//     m_ptrDevice->SetVertexShader(0); // m_pShaderVertex

    m_ptrDevice->SetTexture3(0, 0, 0);

//     DbgVerifyHResult(pd3dDevice9->SetTexture    (0, 0));
//     DbgVerifyHResult(pd3dDevice9->SetTexture    (1, 0));
//     DbgVerifyHResult(pd3dDevice9->SetTexture    (2, 0));
    m_ptrDevice->m_psNor.SetCurrent(m_ptrDevice);
//    m_ptrDevice->SetPixelShader(m_ptrDevice->m_pShaderPixel);
//     DbgVerifyHResult(pd3dDevice9->SetPixelShader(0));
//     DbgVerifyHResult(pd3dDevice9->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ONE));
}

TMBool CTextureD3D9NV12::Resize(int w, int h)
{
    GX_TRACE("CTextureD3D9NV12::Resize(%d, %d)", w, h);
    return 0 != CreateTex(m_ptrDevice, w, h, m_ddsd.Pool, this);
}

struct CNV12CopyD3D9 : public CNV12CopyBase
{
private:
    CTextureD3D9NV12* tex;

public:
    CNV12CopyD3D9(CTextureD3D9NV12* _tex) : tex(_tex)
    {
        tex->m_ptrDevice->LockDevice();
    }

    ~CNV12CopyD3D9()
    {
        tex->m_ptrDevice->UnlockDevice();
    }

private:
    virtual TMDibBuff LockRect(void* surface)
    {
        D3DLOCKED_RECT d3d_rect = { 0 };
        IDirect3DTexture9* pTex = (IDirect3DTexture9*)(surface);
        HRESULT hr;
        DbgVerifyHResult(hr = CD3D9Device::_D3D9On12Bugs::LockTex9(tex->m_ptrDevice, pTex, d3d_rect));
        if (FAILED(hr))
            return tex->m_ptrDevice->CheckError(hr), TMDibBuff(0,0,0);
        DbgAssert(d3d_rect.pBits);
        return TMDibBuff(d3d_rect.pBits, d3d_rect.Pitch, 1);
    }

    virtual void UnlockRect(void* surface)
    {
        IDirect3DTexture9* pTex = (IDirect3DTexture9*)(surface);
        pTex->UnlockRect(0);
    }

    virtual void ReleaseA()
    {
        tex->m_ptr[2].Release();
    }

    virtual void* RecreateA(int w, int h)
    {
        CComPtr<IDirect3DTexture9>& ptr = tex->m_ptr[2];
        if (ptr)
        {
            D3DSURFACE_DESC ddsd;
            ptr->GetLevelDesc(0, &ddsd);
            if (ddsd.Width != w || ddsd.Height != h)
                ptr.Release();
        }
        
        if (!ptr)
        {
            HRESULT hr;
            FAILED_CHECK(tex->m_ptrDevice, 0, tex->m_ptrDevice->NativeCreateTexture(
                w, h, D3DUSAGE_DYNAMIC, D3DFMT_L8, D3DPOOL_DEFAULT, &ptr));
        }
        return ptr;
    }
};

TMBool CTextureD3D9NV12::ResetAsObject(const void* pData, EGXReset eReset)
{
    DbgAssert(pData);

    if (eReset == eGXResetCopyDib)
    {
        CNV12CopyD3D9 cd(this);
        return cd.ResetAsDib(*(GXDibData*)pData, GetWidth(), GetHeight(), m_ptr[0].p, m_ptr[1].p);
//        return ResetAsDib(*(GXDibData*)pData);
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////
// CSwapChainD3D9
CSwapChainD3D9* CSwapChainD3D9::CreateChain(CGXAppD3D9* app, TMHWnd hWnd, int w, int h, EGXFormat _bpp, CSwapChainD3D9* pTo)
{
    GX_TRACE(L"CTextureD3D9Base::CreateChain(%d, %d) 0; AvailableMem:%dM",
        w, h, D3D9GetFreeMem(app->m_pd3dDevice9));

    if (pTo)
    {
        pTo->m_ptrSwapChain.Release();
        pTo->m_ptrSurface.Release();
        GX_TRACE(L"CTextureD3D9Base::CreateChain(%d, %d) 1; AvailableMem:%dM",
            w, h, D3D9GetFreeMem(app->m_pd3dDevice9));
    }

    D3DFORMAT fmt = dxGXFormat2D3DFormat(_bpp);
    if (D3DFMT_UNKNOWN == fmt)
        return 0;
    
    D3DPRESENT_PARAMETERS pp = {0};
    pp.Windowed = TRUE;
    pp.hDeviceWindow = hWnd;
    pp.SwapEffect = D3DSWAPEFFECT_COPY;
    //         pp.BackBufferFormat = D3DFMT_UNKNOWN; // dm.Format;
    pp.BackBufferWidth = w;
    pp.BackBufferHeight = h;
    pp.BackBufferFormat = fmt;
    pp.BackBufferCount = 1;
    pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    
    HRESULT hr;
    CComPtr<IDirect3DSwapChain9> ptrSwapChain;
    FAILED_CHECK(app, 0, CD3D9Device::_D3D9On12Bugs::CreateSC(app, &pp, &ptrSwapChain));
    DbgAssert(ptrSwapChain);

    GX_TRACE(L"CTextureD3D9Base::CreateChain(%d, %d) 2; AvailableMem:%dM",
        w, h, D3D9GetFreeMem(app->m_pd3dDevice9));

    CComPtr<IDirect3DSurface9> ptrSurface;
    FAILED_CHECK(app, 0, ptrSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &ptrSurface));
    DbgAssert(ptrSurface);

    if (!pTo)
        pTo = CTextureBltT<CSwapChainD3D9, CGXAppD3D9::CVertex_Texture>::Inst();// new CRefObjectT<CSwapChainD3D9>(1);
    TM_IF_DBG(pTo->m_eType = eTextureTypeSwapChain);
    pTo->Init(app, ptrSurface);
    pTo->m_ptrSwapChain = ptrSwapChain;
    pTo->m_hWnd = hWnd;

    GX_TRACE(L"CTextureD3D9Base::CreateChain(%d, %d) 3; AvailableMem:%dM",
        w, h, D3D9GetFreeMem(app->m_pd3dDevice9));

    return pTo;
}

void CSwapChainD3D9::NativeBlt(_CDrawStrip& ds)
{
    CRefObjectT<CTextureD3D9> tex;
    CreateTex(m_ptrDevice, m_ddsd.Width, m_ddsd.Height, m_ddsd.Format, D3DUSAGE_RENDERTARGET, D3DPOOL_DEFAULT, &tex);
    DbgVerifyHResult(m_ptrDevice->m_pd3dDevice9->StretchRect(m_ptrSurface, NULL, tex.m_ptrSurface, NULL, D3DTEXF_NONE));
    tex.NativeBlt(ds);
}

TMBool CSwapChainD3D9::Resize(int w, int h)
{
    GX_TRACE("CSwapChainD3D9::Present(%d, %d)", w, h);
    return 0 != CreateChain(m_ptrDevice, m_hWnd, w, h, GetFormat(), this);
}

// XP not support RTL present
struct CXPPresent
{
private:
    HWND m_hWnd;
    LONG m_dwExstyle;
    LONG_PTR m_proc;
    
    static LRESULT CALLBACK NullWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (uMsg == WM_STYLECHANGING)
        {
            STYLESTRUCT* p = (STYLESTRUCT*)lParam;
            p->styleOld = p->styleNew;
        }
        return 0;
    }
    
public:
    CXPPresent(HWND hWnd)
    {
        m_dwExstyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);
        if (m_dwExstyle & WS_EX_LAYOUTRTL)
        {
            m_hWnd  = hWnd;
            m_proc = ::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)&NullWndProc);
            ::SetWindowLong(hWnd, GWL_EXSTYLE, m_dwExstyle & ~WS_EX_LAYOUTRTL);
        }
    }
    
    ~CXPPresent()
    {
        if (m_dwExstyle & WS_EX_LAYOUTRTL)
        {
            ::SetWindowLong(m_hWnd, GWL_EXSTYLE, m_dwExstyle);
            ::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, m_proc);
        }
    }
};

EGXRet CSwapChainD3D9::Present(TMHWnd hWnd, EGXPresent eFlag, GXPRESENTFX* fx) 
{
    GX_TRACE("CSwapChainD3D9::Present(hWnd:%p, eFlag:%x, fx:%p)", hWnd, eFlag, fx);
    DbgAssert(m_ptrSwapChain);
    
    if (eFlag == eGXPresentLayered)
        return this->PresentLayeredWin(hWnd, eFlag, fx);// PresentLayeredWin(hWnd, eFlag, fx);
    
//     if ((eFlag & eGXPresentDCMask) == eGXPresentDCPaint)
//     { CPaintDC dc(hWnd); } // don't use ValidateRect

    HRESULT hr;
    {
        CGXAppD3D9::CAutoLock lock(m_ptrDevice);
        if (m_ptrDevice->m_bXP) // && m_ptrDevice->m_local.IsRTL())
        {
            CXPPresent xpp(hWnd);
            hr = CD3D9Device::_RTSSHooksBugs::PresentSC(m_ptrSwapChain, hWnd, fx->rcDirty);
        }
        else
            hr = CD3D9Device::_RTSSHooksBugs::PresentSC(m_ptrSwapChain, hWnd, fx->rcDirty);
    }
    
    DbgVerifyHResult(hr);
    if (FAILED(hr))
        return m_ptrDevice->CheckError(hr);
    
    return eGXRetSuccess;
}


////////////////////////////////////////////////////////////////////////////////
// CGXAppD3D9
static inline void _CGXAppD3D9_ResetBlend(IDirect3DDevice9* pd3dDevice9)
{
    DbgVerifyHResult(pd3dDevice9->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ONE));
    DbgVerifyHResult(pd3dDevice9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));
}

// static inline void _CGXAppD3D9_AlphaBlendEnable(IDirect3DDevice9* device)
// {
//     DbgVerifyHResult(device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE));
// }


CGXAppD3D9::CGXAppD3D9()
    : m_pBk(0)
{
    // make sure D3DCOLOR same as TMColor
    DbgAssertS(D3DCOLOR_RGBA(254,127,63,31) == TMRGBA(254,127,63,31));

}

CGXAppD3D9::~CGXAppD3D9()
{
    DbgTrace("CGXAppD3D9::~CGXAppD3D9()");
    DbgAssert(!m_stk.size());
}

TMBool CGXAppD3D9::CreateDevice(int nAdapter)
{
    GX_TRACE("CGXAppD3D9::CreateDevice()");

    if (!CD3D9Device::CreateDevice(nAdapter))
        return false;

    ResetFontDevice();

    DbgVerifyHResult(m_pd3dDevice9->GetRenderTarget(0, &m_ptrOldRenderTarget));

    // init device
    DbgVerifyHResult(m_pd3dDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE)); // D3DCULL_CCW
    DbgVerifyHResult(m_pd3dDevice9->SetRenderState(D3DRS_LIGHTING, FALSE)); // turn off D3D lighting to use vertex colors
    DbgVerifyHResult(m_pd3dDevice9->SetRenderState(D3DRS_ZENABLE, FALSE));

    // use dest alpha. must set D3DRS_SEPARATEALPHABLENDENABLE to true.
    DbgVerifyHResult(m_pd3dDevice9->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE));
    DbgVerifyHResult(m_pd3dDevice9->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ZERO));

    
//     DbgVerifyHResult(m_pd3dDevice9->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE)); // Enable alpha blending.
//     DbgVerifyHResult(m_pd3dDevice9->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA));
//     DbgVerifyHResult(m_pd3dDevice9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));
    _CGXAppD3D9_ResetBlend(m_pd3dDevice9);
    
    EnableScissor();
//    DbgVerifyHResult(m_pd3dDevice9->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE));
//     DbgVerifyHResult(m_pd3dDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME));

//     DbgVerifyHResult(m_pd3dDevice9->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE));
    
    DbgVerifyHResult(m_pd3dDevice9->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT));
    DbgVerifyHResult(m_pd3dDevice9->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT));
    DbgVerifyHResult(m_pd3dDevice9->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT));
//    DbgVerifyHResult(m_pd3dDevice9->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR));
//     DbgVerifyHResult(m_pd3dDevice9->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP)); // default: D3DTADDRESS_WRAP
//     DbgVerifyHResult(m_pd3dDevice9->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP)); // default: D3DTADDRESS_WRAP
//     DbgVerifyHResult(m_pd3dDevice9->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
//     DbgVerifyHResult(m_pd3dDevice9->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));


//     DbgVerifyHResult(m_pd3dDevice9->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR));
//     DbgVerifyHResult(m_pd3dDevice9->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR));
//     
//     DbgVerifyHResult(m_pd3dDevice9->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE));
//     DbgVerifyHResult(m_pd3dDevice9->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE));
//     DbgVerifyHResult(m_pd3dDevice9->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE));
    
//     m_pd3dDevice9->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
//     m_pd3dDevice9->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
//     m_pd3dDevice9->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

#ifdef GXCANVAS_H
    // 编译HLSL效果
    LPD3DXBUFFER pErrorMsgs = 0;
    D3DXCreateEffectFromFile(m_pd3dDevice9, L"Z:\\TMDui\\src\\Graphic\\hlsl\\line_all_iup.hlsl.h", NULL, NULL, 
        D3DXSHADER_DEBUG, NULL, &m_pEffect, &pErrorMsgs);
    if (pErrorMsgs)
    {
        const char* p = (const char*)pErrorMsgs->GetBufferPointer();
        const char* p2 = (const char*)pErrorMsgs->GetBufferPointer();
        p;
	}
    HRESULT hr = m_pd3dDevice9->CreateVertexDeclaration(decl_VS_INPUT::p, &m_pDecl);
#endif // GXCANVAS_H

//     D3DVERTEXELEMENT9 decl[] = {
// #   undef VS_INPUT_ITEM
// #   define VS_INPUT_ITEM(a,b,c) VS_D3DVERTEXELEMENT_ITEM(VS_INPUT,a,b,c)
// #   include "hlsl\gx_line_vs_i.h"
//         //         {0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
//         //         {0, 12, D3DDECLTYPE_SHORT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
//         D3DDECL_END()
//     };
    //     D3DVERTEXELEMENT9 decl2[] = { 
    //         { 0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    //         { 0, 8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
    //         { 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
    //         { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },
    //         D3DDECL_END()
    // };

    PS_DESC descPS      = { cg3_PS };
    PS_DESC descPSTex   = { cg3_PS_Tex };
    DbgReturn0HResult(ShaderInit(cg_descVS9, descPS, descPSTex, sizeof(CBuffer)));

//      this->SetVertexLayout(m_pVertexLayout); // m_pVertexLayout
//      this->SetVertexShader(0); // m_pShaderVertex
//      this->SetPixelShader (m_pShaderPixel); // m_pShaderPixel


    return true;
}

EGXRet CGXAppD3D9::CheckError(HRESULT hr)
{
    GX_TRACE("CGXAppD3D9::CheckError(%x, AvailableMem:%dM)", hr, D3D9GetFreeMem(m_pd3dDevice9));
    CGXDevMgr* gm = CGXDevMgr::Instance();
    if (gm->IsDeviceLost() || !gm->IsCurrentDevice(this))
        return eGXRetDeviceChanged;

    gm->NotifyDispatchError(hr);

    if (D3DERR_OUTOFVIDEOMEMORY == hr ||        // 0x8876017C
        E_OUTOFMEMORY == hr)                    // 0x8007000e 
        return gm->SetDeviceLost(eDeviceResetGDI), eGXRetDeviceChanged;

    if (E_UNEXPECTED == hr ||                   // D3D9On12 or 3rd DLL bug
        DXGI_ERROR_DEVICE_RESET == hr ||        // 0x887A0007 D3D9On12 + Intel Iris Xe Graphics
        DXGI_ERROR_DEVICE_REMOVED == hr)        // 0x887A0005 D3D9On12 + Intel Iris Xe Graphics
        return gm->SetDeviceLost(eDeviceReset3s), eGXRetDeviceChanged;

    hr = m_pd3dDevice9->TestCooperativeLevel();
    switch (hr)
    {
    case D3DERR_DEVICELOST:                     // 0x88760868
        return gm->SetDeviceLost(eDeviceReset3s), eGXRetDeviceChanged;
        
    case D3DERR_DEVICENOTRESET:                 // 0x88760869
    case D3DERR_DRIVERINTERNALERROR:            // 0x88760827
        return gm->SetDeviceLost(eDeviceReset1ms), eGXRetDeviceChanged;

    default:
        break;
    }
    
    return eGXRetFail;
}

void CGXAppD3D9::InitDevTransform()
{
    _gx_check();

    float fDPI = GetDPI();
    float f1 = (float)TM_DPI_SYSTEM_DEFAULT / fDPI;
    float f2 = fDPI / (float)TM_DPI_SYSTEM_DEFAULT;
    CAffine aff(f1,0, 0,f1, 0,0);
    aff *= m_aff;
//     aff = m_aff;
    aff.Scale(f2, f2);


    float w = m_pBk->GetWidth();
    float h = m_pBk->GetHeight();
    const float df = 1.0f;
    CAffine affProj;
    if (IsRTL())
        affProj.Set(-2.0f / (float)w, 0, 0, -2.0f / h, (w - df) / w, (h + df) / h);
    else
        affProj.Set(2.0f / (float)w, 0, 0, -2.0f / h, -(w + df) / w, (h + df) / h);

    //affProj *= aff;
    aff *= affProj;

//     {
//         GXPoint pt(159,64);
//         aff.MapPoint(pt);
//         DbgTrace("CGXAppD3D9::InitDevTransform(%d,%d) map 159,64 to (%g,%g)-(%g,%g)",
//             m_pBk->GetWidth(), m_pBk->GetHeight(),
//             pt.x, pt.y,
//             (pt.x + 1.0f) * w / 2.0f + 0.5f, (1.0f - pt.y) * h / 2.0f + 0.5f);
//         float xx = 31.7813f;
//         int x = 0;
//     }

    D3DMATRIX mt =
    {
        aff.M11(), aff.M12(), 0, 0,
        aff.M21(), aff.M22(), 0, 0,
        0, 0, 1, 0,
        aff.Dx(), aff.Dy(), 0, 1,
    };
    GX_TRACE("CGXAppD3D9::InitDevTransform(%f, %f, %f, %f, %f, %f)", aff.M11(), aff.M12(), aff.M21(), aff.M22(), aff.Dx(), aff.Dy());
     DbgVerifyHResult(m_pd3dDevice9->SetTransform(D3DTS_PROJECTION, &mt));

    D3DMATRIX mt2 =
    {
        aff.M11(), aff.M21(), 0, aff.Dx(),
        aff.M12(), aff.M22(), 0, aff.Dy(),
        0, 0, 1, 0,
        0, 0, 0, 1,
    };
//    SetVSConstantF(&mt2, 4);
    GetVSCBufffer()->SetDataVec4(this, &mt2, 4);


//     D3DXMATRIX mt1;
//     D3DXMATRIX mt2;
//     D3DXMatrixOrthoOffCenterLH(&mt1, 0.5, 1000.5, 500, 0, -1, 1);
//     D3DXMatrixOrthoOffCenterLH(&mt2, 1000.5, 0.5, 500, 0, -1, 1);

}

void CGXAppD3D9::ClipDevNow()
{
    CRect rc(m_rcClip);
    if (IsRTL())
    {
        int w = m_pBk->GetWidth();
        int n = w - rc.left;
        rc.left = w - rc.right;
        rc.right = n;
    }
    rc &= CRect(0, 0, m_pBk->GetWidth(), m_pBk->GetHeight());
    this->SetScissorRect(rc);
    GX_TRACE("CGXAppD3D9::ClipDevNow(" TM_FMT_RCI_STR ")", TM_FMT_RCI_VAL(rc));
}

void CGXAppD3D9::DrawStrip(_CDrawStrip& ds)
{
//     DbgVerifyHResult(m_pd3dDevice9->SetFVF(ds.FVF));
    DbgVerifyHResult(m_pd3dDevice9->DrawPrimitiveUP(ds.PType, ds.PCount, ds.pData, ds.VertexStreamZeroStride));
}

void CGXAppD3D9::SamplersSetLinear()
{
    DbgVerifyHResult(m_pd3dDevice9->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
    DbgVerifyHResult(m_pd3dDevice9->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
}

void CGXAppD3D9::SamplersResetLinear()
{
    DbgVerifyHResult(m_pd3dDevice9->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT));
    DbgVerifyHResult(m_pd3dDevice9->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT));
}

void CGXAppD3D9::BlendSetOver()
{
    DbgVerifyHResult(m_pd3dDevice9->SetRenderState(D3DRS_ALPHABLENDENABLE, true));
}

void CGXAppD3D9::BlendSetCopy()
{
    DbgVerifyHResult(m_pd3dDevice9->SetRenderState(D3DRS_ALPHABLENDENABLE, false));
}

void CGXAppD3D9::BlendSetInvert()
{
    BlendSetOver();
    DbgVerifyHResult(m_pd3dDevice9->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_INVDESTCOLOR));
    DbgVerifyHResult(m_pd3dDevice9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO));
    DbgVerifyHResult(m_pd3dDevice9->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, true));
}

void CGXAppD3D9::BlendResetInvert()
{
    BlendResetOver();
    _CGXAppD3D9_ResetBlend(m_pd3dDevice9);
    DbgVerifyHResult(m_pd3dDevice9->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, false));
//     DbgVerifyHResult(m_pd3dDevice9->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD));
}

EGXDevice CGXAppD3D9::GetDeviceType(GXADAPTERDESC* desc)
{
    if (desc)
        this->GetDeviceDesc(*desc);
    return eGXGraDevD3D9 | m_font->FontGetDeviceType();
}

GXPTexture CGXAppD3D9::GetRenderTarget()
{
    return m_pBk;
}


////////////////////////////////////////////////////////////////////////////////
// texture
void CGXAppD3D9::TextureCreateRenderTarget(IGXTexture** tex, int w, int h, EGXFormat format)
{
    GX_TRACE("CGXAppD3D9::TextureCreateRenderTarget(%d, %d, %d)", w, h, format);
    DbgAssert(tex && !*tex);

    D3DFORMAT fmt = dxGXFormat2D3DFormat(format);
    if (D3DFMT_UNKNOWN == fmt)
        return;
    *tex = CTextureD3D9::CreateTex(this, w, h, fmt, D3DUSAGE_RENDERTARGET, D3DPOOL_DEFAULT, 0);
//     if (!*tex)
//         CheckError(S_FALSE);
}

void CGXAppD3D9::TextureCreateSwapChain(IGXTexture** tex, TMHWnd hWnd, int w, int h, EGXFormat format)
{
    GX_TRACE("CGXAppD3D9::TextureCreateSwapChain(%p, %d, %d, %d)", hWnd, w, h, format);
    DbgAssert(tex && !*tex);

    *tex = CSwapChainD3D9::CreateChain(this, hWnd, w, h, format, 0);
//     if (!*tex)
//         CheckError(S_FALSE);
}

void CGXAppD3D9::TextureCreateIndirect(IGXTexture** tex, int w, int h, EGXFormat format)
{
    GX_TRACE("CGXAppD3D9::TextureCreateIndirect(%d, %d, %d)", w, h, format);
    DbgAssert(tex && !*tex);

    switch (format)
    {
//     case eGXFormatI420:
//     case eGXFormatIYUV:
//     case eGXFormatYU12:
//     case eGXFormatYV12:
//     case eGXFormatNV21:
    case eGXFormatNV12:
        *tex = CTextureD3D9NV12::CreateTex(this, w, h, D3DPOOL_DEFAULT, 0);
        return;
    }
    
    D3DFORMAT fmt = dxGXFormat2D3DFormat(format);
    if (D3DFMT_UNKNOWN == fmt)
        return;
    
    *tex = CTextureD3D9::CreateTex(this, w, h, fmt, 0, D3DPOOL_MANAGED, 0);
}

void CGXAppD3D9::Clear(TMColor clr)
{
    _gx_check();
    this->DisableScissor();
    DbgVerifyHResult(m_pd3dDevice9->Clear(0, 0, D3DCLEAR_TARGET, clr, 0, 0l));
    this->EnableScissor();
}

// CGXAppBase
EGXRet CGXAppD3D9::InitProp(GXPTexture texture, int nDPI, TMUInt32 dwFlag)
{
    return _GXApp_InitProp(this, static_cast<CTextureD3D9*>(texture), nDPI, dwFlag);
}

void CGXAppD3D9::VertexFill(const GXIndices* pt, TMUInt nt)
{
    GX_TRACE("CGXAppD3D9::VertexFill(%d)", nt);
    BlendSetCopy();
    int nv = m_vertex.m_aTv.size();
//    DbgVerifyHResult(m_pd3dDevice9->SetFVF(_FVF | D3DFVF_DIFFUSE));
    DbgVerifyHResult(m_pd3dDevice9->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, nv, nt / 3, pt, D3DFMT_INDEX32, m_vertex.m_aTv.data(), m_vertex.eSize));
    BlendResetCopy();
}


EGXRet CGXAppD3D9::PresentCanvas(IGXCanvas* canvas, const TM::CAffine* aff)
{
#ifdef GXCANVAS_H

	CGXCanvas* cvs = static_cast<CGXCanvas*>(canvas);

    D3DMATRIX mt;
//     GX_TRACE("CGXAppD3D9::InitDevTransform(%f, %f, %f, %f, %f, %f)", aff.M11(), aff.M12(), aff.M21(), aff.M22(), aff.Dx(), aff.Dy());
    DbgVerifyHResult(m_pd3dDevice9->GetTransform(D3DTS_PROJECTION, &mt));


//     D3DMATRIX matProj =
//     {
//         aff.M11(), aff.M12(), 0, 0,
//         aff.M21(), aff.M22(), 0, 0,
//         0, 0, 1, 0,
//         aff.Dx(), aff.Dy(), 0, 1,
//     };
    DbgVerifyHResult(m_pEffect->SetMatrix("proj", (D3DXMATRIX*)&mt));
    DbgVerifyHResult(m_pEffect->Begin(NULL, 0));
    DbgVerifyHResult(m_pEffect->BeginPass(0));
    DbgVerifyHResult(m_pd3dDevice9->SetFVF(0)); 
    DbgVerifyHResult(m_pd3dDevice9->SetVertexDeclaration(m_pDecl));
    CBUFFER* cb = 0;

    for (CGXCanvas::CVctsList::recorder rec(cvs->m_aVcts); rec; ++rec)
    {
        CIndVcts& vcts = rec.get_data();
        if (!vcts.m_aVct.size())
            continue;

        if (cb != vcts.m_cbuffer)
        {
            cb = (CBUFFER*)vcts.m_cbuffer;
            HRESULT hr = m_pd3dDevice9->SetPixelShaderConstantF(0, (const float*)cb, cb->nCount);
            if (cb->style_type == TYPE_PATTERN || cb->style_type == TYPE_LGRADIENT_N || cb->style_type == TYPE_RGRADIENT_N)
            {
                CComQIPtr<IDirect3DSurface9> ptr(cb->image);
                SetTexture(0, ptr);
            }
            else
                m_pd3dDevice9->SetTexture(0, 0);
        }

        if (vcts.m_aInd.size()) // stroke
        {
            m_pd3dDevice9->DrawIndexedPrimitiveUP(
                D3DPT_TRIANGLELIST,
                0, vcts.m_aVct.size(),
                vcts.m_aInd.size() / 3,
                &*vcts.m_aInd.cbegin(),
                D3DFMT_INDEX16,
                &*vcts.m_aVct.cbegin(),
                vcts.eSize
            );
        }
        else // fill
        {
//             CRefObjectT<CTextureD3D9> tex;
//             CTextureD3D9::CreateTex(this, 1024, 768, D3DFMT_D24S8, D3DUSAGE_DEPTHSTENCIL, D3DPOOL_DEFAULT, &tex);
//             DbgVerifyHResult(m_pd3dDevice9->SetDepthStencilSurface(tex.m_ptrSurface));



            IDirect3DSurface9* ps = 0;
            DbgVerifyHResult(m_pd3dDevice9->CreateDepthStencilSurface(1024, 768, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, 0, &ps, 0));
            DbgAssert(ps);
            DbgVerifyHResult(m_pd3dDevice9->SetDepthStencilSurface(ps));
            m_pd3dDevice9->Clear(0, 0, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1, 0);
            
            {
                m_pd3dDevice9->SetRenderState(D3DRS_STENCILENABLE, TRUE);
                m_pd3dDevice9->SetRenderState(D3DRS_STENCILWRITEMASK, 0xFF);
                m_pd3dDevice9->SetRenderState(D3DRS_COLORWRITEENABLE, 0); // 不写颜色，只写 stencil
                m_pd3dDevice9->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_INVERT);
                m_pd3dDevice9->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INVERT);
                m_pd3dDevice9->DrawPrimitiveUP(
                    D3DPT_TRIANGLEFAN,
                    vcts.m_aVct.size() - 2,
                    &*vcts.m_aVct.cbegin(),
                    vcts.eSize
                    );
            }

//             {
//                 //shadow
//                 DbgVerifyHResult(m_pd3dDevice9->SetDepthStencilSurface(0));
//                 CTexture texTmp;
//                 texTmp.CreateRenderTarget(1024, 768, eGXFormatR16F);
//                 Push(eGXPropRenderTarget);
//                 CGXAppBase::SetRenderTarget(texTmp);
//                 m_pd3dDevice9->Clear(0, 0, D3DCLEAR_TARGET, 0, 0, 0);
//                 SetTexture(0, ps);
//                 float old = cb->style_type;
//                 cb->style_type = TYPE_SHADOW1;
//                 HRESULT hr = m_pd3dDevice9->SetPixelShaderConstantF(0, (const float*)cb, cb->nCount);
//                 m_pd3dDevice9->SetRenderState(D3DRS_COLORWRITEENABLE, 0xf);//D3DCOLORWRITEENABLE_ALL);
// 
//                 CIndVcts vcts;
//                 for (int i = 0; i < 1; ++i)
//                 {
//                     Vertex vct[4];
//                     InitCap(vct[0], MODE_TRI_NORMAL, 0, _float2(0    + i, 0));
//                     InitCap(vct[1], MODE_TRI_NORMAL, 0, _float2(1024 + i, 0));
//                     InitCap(vct[2], MODE_TRI_NORMAL, 0, _float2(1024 + i, 768));
//                     InitCap(vct[3], MODE_TRI_NORMAL, 0, _float2(0    + i, 768));
//                     vcts._AddVect_4(vct);
//                     InitCap(vct[0], MODE_TRI_NORMAL, 0, _float2(0    - i, 0));
//                     InitCap(vct[1], MODE_TRI_NORMAL, 0, _float2(1024 - i, 0));
//                     InitCap(vct[2], MODE_TRI_NORMAL, 0, _float2(1024 - i, 768));
//                     InitCap(vct[3], MODE_TRI_NORMAL, 0, _float2(0    - i, 768));
//                     vcts._AddVect_4(vct);
//                     InitCap(vct[0], MODE_TRI_NORMAL, 0, _float2(0    , 0   + i));
//                     InitCap(vct[1], MODE_TRI_NORMAL, 0, _float2(1024 , 0   + i));
//                     InitCap(vct[2], MODE_TRI_NORMAL, 0, _float2(1024 , 768 + i));
//                     InitCap(vct[3], MODE_TRI_NORMAL, 0, _float2(0    , 768 + i));
//                     vcts._AddVect_4(vct);
//                     InitCap(vct[0], MODE_TRI_NORMAL, 0, _float2(0    , 0   - i));
//                     InitCap(vct[1], MODE_TRI_NORMAL, 0, _float2(1024 , 0   - i));
//                     InitCap(vct[2], MODE_TRI_NORMAL, 0, _float2(1024 , 768 - i));
//                     InitCap(vct[3], MODE_TRI_NORMAL, 0, _float2(0    , 768 - i));
//                     vcts._AddVect_4(vct);
//                 }
//                 m_pd3dDevice9->DrawPrimitiveUP(
//                     D3DPT_TRIANGLEFAN,
//                     vcts.m_aVct.size() - 2,
//                     &*vcts.m_aVct.cbegin(),
//                     vcts.eSize
//                     );
//                 Pop();
//                 cb->style_type = old;
//                 hr = m_pd3dDevice9->SetPixelShaderConstantF(0, (const float*)cb, cb->nCount);
//                 DbgVerifyHResult(m_pd3dDevice9->SetDepthStencilSurface(ps));
//             }
            
            {
                m_pd3dDevice9->SetRenderState(D3DRS_COLORWRITEENABLE, 0xf);//D3DCOLORWRITEENABLE_ALL);
                m_pd3dDevice9->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
                m_pd3dDevice9->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
                m_pd3dDevice9->SetRenderState(D3DRS_STENCILREF, 0xff);
                m_pd3dDevice9->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
                m_pd3dDevice9->DrawPrimitiveUP(
                    D3DPT_TRIANGLEFAN,
                    vcts.m_aVct.size() - 2,
                    &*vcts.m_aVct.cbegin(),
                    vcts.eSize
                    );
            }
            
            m_pd3dDevice9->SetRenderState(D3DRS_STENCILENABLE, false);
            m_pd3dDevice9->SetDepthStencilSurface(0);
            ps->Release();
        }
    }

    m_pEffect->EndPass();
    m_pEffect->End();

    this->m_psNor.SetCurrent(this);
//    m_pd3dDevice9->SetVertexDeclaration(m_pVertexLayout);

#endif // GXCANVAS_H

	return eGXRetSuccess;
}


TM_END_NAMESPACE



