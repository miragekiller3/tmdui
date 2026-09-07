/*
 *  @file
 *  @brief  D3D11 render
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021.05.09
 *
 *  Copyright (C) 2021 miragekiller
 */

#include "gxapp_d3d11.h"
#include "tmdebugext.h"
#include <d3d9.h>

#include "hlsl/_d11_VS.h"
#include "hlsl/_d11_PS.h"
#include "hlsl/_d11_PS_tex.h"
#include "hlsl/_d11_PS_NV12.h"
#include "hlsl/_d11_PS_NV12A.h"
#include "hlsl/_d11_layout.h"

static const CD3D11Device::VS_DESC cg_descVS11 = { cg4_VS,  tm_countof(cg4_VS), layout_d11_VS_INPUT1, tm_countof(layout_d11_VS_INPUT1) };



TM_BGN_NAMESPACE


// more adapter like intel may be throw 0xC0000005
struct _D3D11Bugs
{
    static HRESULT Map11(ID3D11DeviceContext* context, ID3D11Resource* pResource, D3D11_MAP MapType, D3D11_MAPPED_SUBRESOURCE* resource)
    {
        TM_TRY_ALL(return context->Map(pResource, 0, MapType, 0, resource));
        return E_UNEXPECTED;
    }
};


// for Rivatuner Statistics Server crash:
// RTSSHooks64.dll!O000000180047b710RTSSHooks64.dll!O000000180037196()
// RTSSHooks64.dll!O0o00001800412fe0()
// HRESULT hr = swapChainD->swapChain->Present(swapChainD->swapInterval，presentFlags);
struct _RTSSHooksBugs11
{
    static HRESULT PresentSC(IDXGISwapChain* chain)
    {
        TM_TRY_ALL(return chain->Present(0, 0));
        return E_UNEXPECTED;
    }
};

#define D3D11_CREATE_TEX2D_PARAM_LOCKRECT \
    D3D11_USAGE_STAGING, \
    D3D11_BIND_FLAG(0), \
    D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE, \
    D3D11_RESOURCE_MISC_FLAG(0)

#define D3D11_CREATE_TEX2D_PARAM_GETDC \
    D3D11_USAGE_DEFAULT, \
    D3D11_BIND_RENDER_TARGET, \
    D3D11_CPU_ACCESS_FLAG(0), \
    D3D11_RESOURCE_MISC_GDI_COMPATIBLE \

#define D3D11_CREATE_TEX2D_PARAM_NV12 \
    D3D11_USAGE_DYNAMIC, \
    D3D11_BIND_SHADER_RESOURCE, \
    D3D11_CPU_ACCESS_WRITE, \
    D3D11_RESOURCE_MISC_FLAG(0)

#define D3D11_CREATE_TEX2D_PARAM_SWAPCHAIN_BLT \
    D3D11_USAGE_DEFAULT,    \
    D3D11_BIND_SHADER_RESOURCE, \
    D3D11_CPU_ACCESS_FLAG(0), \
    D3D11_RESOURCE_MISC_FLAG(0)

#define D3D11_CREATE_TEX2D_PARAM_RENDERTARGET \
    D3D11_USAGE_DEFAULT, \
    D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, \
    D3D11_CPU_ACCESS_FLAG(0), \
    D3D11_RESOURCE_MISC_GDI_COMPATIBLE

#define D3D11_CREATE_TEX2D_PARAM_INDIRECT \
    D3D11_USAGE_DYNAMIC, \
    D3D11_BIND_SHADER_RESOURCE, \
    D3D11_CPU_ACCESS_WRITE, \
    D3D11_RESOURCE_MISC_FLAG(0)


static HRESULT _CGXAppD3D11_CreateTex2D(
    ID3D11Texture2D** tex,
    CGXAppD3D11* app,
    int w, int h, DXGI_FORMAT fmt,
    D3D11_USAGE Usage,
    D3D11_BIND_FLAG BindFlags,
    D3D11_CPU_ACCESS_FLAG CPUAccess,
    D3D11_RESOURCE_MISC_FLAG MiscFlags)
{
    D3D11_TEXTURE2D_DESC desc = 
    {
        w, // Width
        h, // Height
        1, // MipLevels
        1, // ArraySize
        fmt, //Format DXGI_FORMAT_R8G8B8A8_UNORM; 
        { 1, 0 }, // SampleDesc
        Usage, // desc.Usage
        BindFlags, //BindFlags
        CPUAccess, //CPUAccessFlags
        MiscFlags
    };
    return app->m_device->CreateTexture2D(&desc, NULL, tex);
}

static HRESULT _CGXAppD3D11_CreateShaderView(
    CGXAppD3D11* app,
    ID3D11Texture2D* tex,
    DXGI_FORMAT fmt,
    ID3D11ShaderResourceView** view)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    SRVDesc.Format = fmt;
    SRVDesc.Texture2D.MipLevels = -1;
    SRVDesc.Texture2D.MostDetailedMip = 0;
    HRESULT hr;
    FAILED_CHECK(app, hr, app->m_device->CreateShaderResourceView(tex, &SRVDesc, view));
    return hr;
}


////////////////////////////////////////////////////////////////////////////////
// CSurfaceD3D11
struct CSurfaceD3D11 : public IGXSurfaceImpl
{
public:
    TM_UNUSED_IC_GetDebugInfo();

private:
    CRefPtr<CTextureD3D11>      m_texture;
    CComPtr<ID3D11Texture2D>    m_texLock;
    CComQIPtr<IDXGISurface1>    m_texDC;
    TMBool m_bWriteBack;

    void MakeSurface(
        CComPtr<ID3D11Texture2D>& buffer,
        D3D11_USAGE Usage,
        D3D11_BIND_FLAG BindFlags,
        D3D11_CPU_ACCESS_FLAG CPUAccessFlags,
        D3D11_RESOURCE_MISC_FLAG MiscFlags, DXGI_FORMAT Format)
    {
        m_texture->m_ptrDevice->_gx_check(false, false);

        HRESULT hr;
        FAILED_CHECK(
            m_texture->m_ptrDevice,
            ;,
            _CGXAppD3D11_CreateTex2D(
                &buffer,
                m_texture->m_ptrDevice,
                m_texture->m_ddsd.Width, m_texture->m_ddsd.Height, Format, // m_texture->m_ddsd.Format,
                Usage, BindFlags, CPUAccessFlags, MiscFlags));
    }

    D3D11_MAP MakeCacheLock(ID3D11DeviceContext* context, EGXLockRect flag)
    {
        m_texture->m_ptrDevice->_gx_check(false, false);

        if (flag == GX_LOCK_RECT_WRITE && !(m_texture->m_ddsd.BindFlags & D3D11_BIND_RENDER_TARGET))
        {
            m_texLock = m_texture->m_ptrSurface;
            m_bWriteBack = false;
            return D3D11_MAP_WRITE_DISCARD; // only texture can write
        }

        MakeSurface(m_texLock, D3D11_CREATE_TEX2D_PARAM_LOCKRECT, m_texture->m_ddsd.Format);
        DbgAssert(m_texLock);
        if (flag != GX_LOCK_RECT_WRITE)
        {
            context->CopyResource(m_texLock, m_texture->m_ptrSurface);
//             D3D11_BOX box = { 0, 0, 0, m_texture->GetWidth(), m_texture->GetHeight(), 1 };
//             m_texture->m_ptrDevice->m_context->CopySubresourceRegion(m_texLock, 0, 0, 0, 0, m_texture->m_ptrSurface, 0, &box);

        }
        m_bWriteBack = (flag != GX_LOCK_RECT_READ);

        D3D11_MAP f[3] = { D3D11_MAP_READ, D3D11_MAP_WRITE_DISCARD, D3D11_MAP_READ_WRITE };
        DbgAssert(flag >= GX_LOCK_RECT_READ && flag <= GX_LOCK_RECT_READWRITE);
        return f[flag - 1];
    }

    void MakeCacheDC(ID3D11DeviceContext* context, TMBool bDiscard)
    {
        m_texture->m_ptrDevice->_gx_check(false, false);

        if (m_texture->m_ddsd.BindFlags & D3D11_BIND_RENDER_TARGET)
        {
            m_texDC = m_texture->m_ptrSurface;
            m_bWriteBack = false;
            return;
        }

        CComPtr<ID3D11Texture2D> buffer;
        MakeSurface(buffer, D3D11_CREATE_TEX2D_PARAM_GETDC, DXGI_FORMAT_B8G8R8A8_UNORM); // D3D11_RESOURCE_MISC_GDI_COMPATIBLE only support DXGI_FORMAT_B8G8R8A8
        context->CopyResource(buffer, m_texture->m_ptrSurface);
        m_bWriteBack = !bDiscard;
        m_texDC = buffer;
//         D3D11_BOX box = { m_rc.Left(), m_rc.Top(), 0, m_rc.Right(), m_rc.Bottom(), 1 };
//         m_texture->m_ptrDevice->m_context->CopySubresourceRegion(buffer, 0, 0, 0, 0, m_texture->m_ptrSurface, 0, &box);
    }

public:
    void Init(CTextureD3D11* tex)
    {
        tex->m_ptrDevice->LockDevice();
        m_texture = tex;
        m_texture->m_ptrDevice->_gx_check(false, false);
    }

    ~CSurfaceD3D11()
    {
        m_texture->m_ptrDevice->_gx_check(false, false);
        m_texture->m_ptrDevice->UnlockDevice();
    }
    
    virtual void LockRect(TMDibBuff& dib, const TMRect* rc, EGXLockRect flag) tm_override
    {
        GX_TRACE("CSurfaceD3D11::LockRect(this:%p, flag:%x)", this, flag);
        m_texture->m_ptrDevice->_gx_check(false, false);
        DbgAssert(!m_texLock);
        DbgAssert(!m_texDC);

        ID3D11DeviceContext* context = m_texture->m_ptrDevice->GetContext();
        D3D11_MAP f = MakeCacheLock(context, flag);
        DbgAssert(m_texLock);
        
        D3D11_MAPPED_SUBRESOURCE resource = { 0 };
        HRESULT hr;
        DbgVerifyHResult(hr = _D3D11Bugs::Map11(context, m_texLock, f, &resource));
        if (FAILED(hr))
        {
            m_texture->m_ptrDevice->CheckError(hr);
            return;
        }

        DbgAssert(resource.pData);
        dib.m_stride = resource.RowPitch;
        dib.m_bits   = resource.pData;
        dib.m_height = m_texture->GetHeight();
        if (rc)
        {
            int bpp = gxFormat2BPP(m_texture->GetFormat());
            dib.m_bits = dib.GetPixelData(RectLeft(*rc), RectTop(*rc), bpp);
            dib.m_height = RectHeight(*rc);
        }
    }

    virtual void UnlockRect() tm_override
    {
        GX_TRACE("CSurfaceD3D11::UnlockRect(this:%p)", this);
        m_texture->m_ptrDevice->_gx_check(false, false);
        DbgAssert(m_texLock);

        ID3D11DeviceContext* context = m_texture->m_ptrDevice->GetContext();
        context->Unmap(m_texLock, 0);

        // write back
        if (!m_bWriteBack)
            return;
        DbgAssert(m_texLock != m_texture->m_ptrSurface);
        context->CopyResource(m_texture->m_ptrSurface, m_texLock);
    }
    
    virtual TMHDC GetDC(TMBool bDiscard) tm_override
    {
        GX_TRACE("CSurfaceD3D11::GetDC(this:%p, bDiscard:%d)", this, bDiscard);
        m_texture->m_ptrDevice->_gx_check(false, false);
        DbgAssert(!m_texLock);
        DbgAssert(!m_texDC);
        MakeCacheDC(m_texture->m_ptrDevice->GetContext(), bDiscard);
        if (!m_texDC)
            return 0; // maybe device removed

        HDC hdc = 0;
        HRESULT hr = m_texDC->GetDC(bDiscard, &hdc);
        DbgVerifyHResult(hr);
        return hdc;
    }
    
    virtual void ReleaseDC(HDC hdc) tm_override
    {
        GX_TRACE("CSurfaceD3D11::ReleaseDC(this:%p, hdc:%x)", this, hdc);
        m_texture->m_ptrDevice->_gx_check(false, false);
        DbgAssert(m_texDC);
        DbgVerifyHResult(m_texDC->ReleaseDC(0));

        // WriteBackDC
        if (!m_bWriteBack)
            return;
        ID3D11DeviceContext* context = m_texture->m_ptrDevice->GetContext();
        CComQIPtr<ID3D11Resource> buffer(m_texDC);
        DbgAssert(buffer != m_texture->m_ptrSurface);
        context->CopyResource(m_texture->m_ptrSurface, buffer);
    }
};


////////////////////////////////////////////////////////////////////////////////
// CTextureD3D11Base
IGXApp* CTextureD3D11Base::GetApp()
{
    return m_ptrDevice;
}

void CTextureD3D11Base::OnSetRenderTarget(CGXAppBase* app)
{
    GX_TRACE("CTextureD3D11Base::OnSetRenderTarget(%p, %p)", app, this);
    m_ptrDevice->_gx_check(true, false);
    DbgAssert(app == m_ptrDevice);

    CComPtr<ID3D11RenderTargetView> view;
    GetRenderTarget(view); // maybe device removed.

    CGXAppD3D11::CContextPtr context(m_ptrDevice);
    m_ptrDevice->CD3D11Device::SetRenderTarget(view);

    // view port
    D3D11_VIEWPORT vp;
    vp.Width = m_ddsd.Width;
    vp.Height = m_ddsd.Height;
    vp.MinDepth = 0;
    vp.MaxDepth = 1;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    context->RSSetViewports(1, &vp);
}

void CTextureD3D11Base::OnResetRenderTarget(CGXAppBase* app, GXPTexture pNew)
{
    GX_TRACE("CTextureD3D11Base::OnResetRenderTarget(%p, %p)", app, pNew);
    m_ptrDevice->_gx_check(true, false);
    DbgAssert(app == m_ptrDevice);
//if (!pNew)
//{
//    HRESULT hr;
//    DbgVerifyHResult(hr = m_ptrDevice->m_pd3dDevice11->EndScene());
//    DbgVerifyHResult(hr = m_ptrDevice->m_pd3dDevice11->SetRenderTarget(0, m_ptrDevice->m_ptrOldRenderTarget));
//    DbgAssert(SUCCEEDED(hr));
//}
}

////////////////////////////////////////////////////////////////////////////////
// CTextureD3D11
CTextureD3D11* CTextureD3D11::CreateTex(
    CGXAppD3D11* app,
    int w, int h, EGXFormat fmtGX, DXGI_FORMAT fmt,
    D3D11_USAGE Usage,
    D3D11_BIND_FLAG BindFlags,
    D3D11_CPU_ACCESS_FLAG CPUAccess,
    D3D11_RESOURCE_MISC_FLAG MiscFlags,
    CTextureD3D11* pTo)
{
    app->_gx_check(false, false);
    HRESULT hr;
    CComPtr<ID3D11Texture2D> ptrTexture;
    FAILED_CHECK(app, 0, _CGXAppD3D11_CreateTex2D(&ptrTexture, app, w, h, fmt, Usage, BindFlags, CPUAccess, MiscFlags));

    // Create resource view
    CComPtr<ID3D11ShaderResourceView> ptrResView;
    if (BindFlags & D3D11_BIND_SHADER_RESOURCE)
        DbgReturn0HResult(_CGXAppD3D11_CreateShaderView(app, ptrTexture, fmt, &ptrResView));
    
    if (!pTo)
    {
        pTo = CTextureBltT<CTextureD3D11, CGXAppD3D11::CVertex_Texture>::Inst(); //CRefObjectT<CTextureD3D11>(1);
        TM_IF_DBG(pTo->m_eType = (BindFlags & D3D11_BIND_RENDER_TARGET) ? eTextureTypeRenderTarget : eTextureTypeTexture);
    }
    pTo->m_fmt = fmtGX;
    pTo->m_ptrResView = ptrResView;
    pTo->Init(app, ptrTexture);
    return pTo;
}

void CTextureD3D11::InitNoDebug(CGXAppD3D11* app, ID3D11Texture2D* ptrSurface)
{
    DbgAssert(app);
    DbgAssert(ptrSurface);
    m_ptrDevice = app;
    m_ptrSurface = ptrSurface;
    MemZeroT(m_ddsd);
    ptrSurface->GetDesc(&m_ddsd);
}

void CTextureD3D11::NativeBlt(CDrawStrip& ds)
{
    m_ptrDevice->_gx_check();
    m_ptrDevice->SetTexture1(m_ptrResView.p);
    m_ptrDevice->m_psTex.SetCurrent(m_ptrDevice);
//    m_ptrDevice->SetPixelShader(m_ptrDevice->m_pShaderPixel_Tex);
    m_ptrDevice->DrawStrip(ds);
    m_ptrDevice->m_psNor.SetCurrent(m_ptrDevice);
//    m_ptrDevice->SetPixelShader(m_ptrDevice->m_pShaderPixel);
}

void CTextureD3D11::ColorFill(TMColor clr)
{
    GX_TRACE("CTextureD3D11::ColorFill(this:%p, color:%X)", this, clr);
    m_ptrDevice->_gx_check(false, false);

    if (m_ddsd.BindFlags & D3D11_BIND_RENDER_TARGET)
    {
        CComPtr<ID3D11RenderTargetView> pNowRenderTargetTexture;
        this->GetRenderTarget(pNowRenderTargetTexture);
        float color[4] = { ColorRF(clr), ColorGF(clr), ColorBF(clr), ColorAF(clr) };
        CGXAppD3D11::CContextPtr(m_ptrDevice)->ClearRenderTargetView(pNowRenderTargetTexture, color);
    }
    else
        NativeColorFill(clr);
}

void CTextureD3D11::GetRenderTarget(CComPtr<ID3D11RenderTargetView>& view)
{
    m_ptrDevice->_gx_check(true, false);
    HRESULT hr;
    FAILED_CHECK(m_ptrDevice, ;, m_ptrDevice->m_device->CreateRenderTargetView(m_ptrSurface, 0, &view));
}

void CTextureD3D11::GetSurface(IGXSurface** ppSurface)
{
    DbgAssert(!*ppSurface);
    CSurfaceD3D11* p = new CRefObjectT<CSurfaceD3D11>();
    p->Init(this);
    *ppSurface = p;
}

TMBool CTextureD3D11::Resize(int w, int h)
{
    m_ptrDevice->_gx_check();
    GX_TRACE("CTextureD3D11::Resize(this:%p, w:%d, h:%d)", this, w, h);
    return 0 != CreateTex(m_ptrDevice, w, h, m_fmt, m_ddsd.Format,
        m_ddsd.Usage,
        (D3D11_BIND_FLAG)m_ddsd.BindFlags,
        (D3D11_CPU_ACCESS_FLAG)m_ddsd.CPUAccessFlags,
        (D3D11_RESOURCE_MISC_FLAG)m_ddsd.MiscFlags,
        this);
}

TMBool CTextureD3D11::ResetAsObject(const void* pData, EGXReset eReset)
{
    GX_TRACE("CTextureD3D11::ResetAsObject(this:%p, pData:%p, eReset:%d)", this, pData, eReset);
    DbgAssert(pData);
    
    CAutoMsgAnalysis awm("CTextureD3D11::ResetAsObject");
    
    switch (eReset)
    {
    case eGXResetCopyDib:
        return ResetAsDib(*(GXDibData*)pData);
        
    case eGXResetCopyHIMAGE:
    case eGXResetMoveHIMAGE:
        return ReplaceByImage((HIMAGE)pData);
        
//     case eGXResetCopyIUnknown:
//         {
//             CCopySurface cs((IUnknown*)pData);
//             return cs.CopyTo(this);
//         }
//         
//     case eGXResetMoveIUnknown:
//         {
//             CCopySurface cs((IUnknown*)pData);
//             return cs.MoveTo(this);
//         }
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////
// CTextureD3D11NV12
struct CNV12CopyD3D11 : public CNV12CopyBase
{
private:
    CTextureD3D11NV12* tex;

public:
    CNV12CopyD3D11(CTextureD3D11NV12* _tex) : tex(_tex)
    {
        tex->m_ptrDevice->LockDevice();
    }

    ~CNV12CopyD3D11()
    {
        tex->m_ptrDevice->UnlockDevice();
    }
    
    virtual TMDibBuff LockRect(void* surface)
    {
        tex->m_ptrDevice->_gx_check(false, false);
        ID3D11Texture2D* pTex = (ID3D11Texture2D*)(surface);
        D3D11_MAPPED_SUBRESOURCE resource = { 0 };
        HRESULT hr;
        DbgVerifyHResult(hr = _D3D11Bugs::Map11(tex->m_ptrDevice->GetContext(), pTex, D3D11_MAP_WRITE_DISCARD, &resource));
        if (FAILED(hr))
            return tex->m_ptrDevice->CheckError(hr), TMDibBuff(0, 0, 0);
        DbgAssert(resource.pData);
        return TMDibBuff(resource.pData, resource.RowPitch, 1);
    }
    
    virtual void UnlockRect(void* surface)
    {
        tex->m_ptrDevice->_gx_check(false, false);
        ID3D11Texture2D* pTex = (ID3D11Texture2D*)(surface);
        tex->m_ptrDevice->GetContext()->Unmap(pTex, 0);
    }
    
    virtual void ReleaseA()
    {
        tex->m_ptrDevice->_gx_check(false, false);
        tex->m_ptrAlpha.Release();
        tex->m_ptrViewAlpha.Release();
    }
    
    virtual void* RecreateA(int w, int h)
    {
        tex->m_ptrDevice->_gx_check(false, false);
        CComPtr<ID3D11Texture2D>& ptr = tex->m_ptrAlpha;
        if (ptr)
        {
            D3D11_TEXTURE2D_DESC ddsd;
            ptr->GetDesc(&ddsd);
            if (ddsd.Width != w || ddsd.Height != h)
            {
                ptr.Release();
                tex->m_ptrViewAlpha.Release();
            }
        }
        
        if (!ptr)
        {
            DbgAssert(!tex->m_ptrViewAlpha);
            CreateTexAndView(tex->m_ptrDevice, w, h, DXGI_FORMAT_R8_UNORM, ptr, tex->m_ptrViewAlpha);
        }

        return ptr;
    }

    static TMBool CreateTexAndView(
        CGXAppD3D11* app,
        int w, int h, DXGI_FORMAT fmt,
        CComPtr<ID3D11Texture2D>& tex,
        CComPtr<ID3D11ShaderResourceView>& view)
    {
        app->_gx_check(false, false);
        HRESULT hr;
        FAILED_CHECK(app, false, _CGXAppD3D11_CreateTex2D(&tex, app, w, h, fmt, D3D11_CREATE_TEX2D_PARAM_NV12));
        DbgReturn0HResult(_CGXAppD3D11_CreateShaderView(app, tex, fmt, &view));
        return true;
    }
};

CTextureD3D11NV12* CTextureD3D11NV12::CreateTex(
    CGXAppD3D11* app,
    int w, int h, CTextureD3D11NV12* pTo)
{
    // texture Y 
    CComPtr<ID3D11Texture2D> ptrTexture;
    CComPtr<ID3D11ShaderResourceView> ptrResView;
    if (!CNV12CopyD3D11::CreateTexAndView(app, w, h, DXGI_FORMAT_R8_UNORM, ptrTexture, ptrResView))
        return 0;

    // texture UV 
    CComPtr<ID3D11Texture2D> ptrTextureUV;
    CComPtr<ID3D11ShaderResourceView> ptrResViewUV;
    if (!CNV12CopyD3D11::CreateTexAndView(app, w / 2, h / 2, DXGI_FORMAT_R8G8_UNORM, ptrTextureUV, ptrResViewUV))
        return 0;

    if (!pTo)
    {
        pTo = CTextureBltT<CTextureD3D11NV12, CGXAppD3D11::CVertex_Texture>::Inst(); //CRefObjectT<CTextureD3D11>(1);
        TM_IF_DBG(pTo->m_eType = eTextureTypeTexture);
    }
    pTo->m_fmt = eGXFormatNV12;
    pTo->m_ptrResView = ptrResView;
    pTo->m_ptrUV = ptrTextureUV;
    pTo->m_ptrViewUV = ptrResViewUV;
    pTo->InitNoDebug(app, ptrTexture);

    if (!app->m_pShaderPixel_NV12)
    {
        CGXAppD3D11::PS_DESC descNV12      = { cg4_PS_NV12,  sizeof(cg4_PS_NV12) };
        CGXAppD3D11::PS_DESC descNV12A     = { cg4_PS_NV12A,  sizeof(cg4_PS_NV12A) };
        app->m_pShaderPixel_NV12 .Create(app, descNV12);
        app->m_pShaderPixel_NV12A.Create(app, descNV12A);
    }

    return pTo;
}

void CTextureD3D11NV12::NativeBlt(CDrawStrip& ds)
{
    m_ptrDevice->_gx_check();
//     ID3D11ShaderResourceView* p[] = { m_ptrResView, m_ptrViewUV, m_ptrViewAlpha };
    if (m_ptrViewAlpha)
    {
        m_ptrDevice->SetTexture3(m_ptrResView, m_ptrViewUV, m_ptrViewAlpha);
//        m_ptrDevice->SetPixelShader(m_ptrDevice->m_pShaderPixel_NV12A);
        m_ptrDevice->m_pShaderPixel_NV12A.SetCurrent(m_ptrDevice);
    }
    else
    {
        m_ptrDevice->SetTexture2(m_ptrResView, m_ptrViewUV);
//        m_ptrDevice->SetPixelShader(m_ptrDevice->m_pShaderPixel_NV12);
        m_ptrDevice->m_pShaderPixel_NV12.SetCurrent(m_ptrDevice);
    }

    m_ptrDevice->DrawStrip(ds);
    m_ptrDevice->SetTexture3(0, 0, 0);

//    m_ptrDevice->SetPixelShader(0);
//    m_ptrDevice->SetPixelShader(m_ptrDevice->m_pShaderPixel);
    m_ptrDevice->m_psNor.SetCurrent(m_ptrDevice);
}

TMBool CTextureD3D11NV12::ResetAsObject(const void* pData, EGXReset eReset)
{
    DbgAssert(pData);
    
    if (eReset == eGXResetCopyDib)
    {
        CNV12CopyD3D11 cd(this);
        return cd.ResetAsDib(*(GXDibData*)pData, GetWidth(), GetHeight(), m_ptrSurface.p, m_ptrUV.p);
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////
// CSwapChainD3D11
void CSwapChainD3D11::SyncBuffer(CComPtr<ID3D11Texture2D>& tex)
{
    DbgVerifyHResult(m_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&tex));
}

void CSwapChainD3D11::NativeBlt(CDrawStrip& ds)
{
    CTextureD3D11* tex = CreateTex(
        m_ptrDevice, m_ddsd.Width, m_ddsd.Height, m_fmt, m_ddsd.Format,
        D3D11_CREATE_TEX2D_PARAM_SWAPCHAIN_BLT, 0);
    if (!tex)
        return;
    m_ptrDevice->GetContext()->CopyResource(tex->m_ptrSurface, m_ptrSurface);
    tex->NativeBlt(ds);
    tex->Release();
}

TMBool CSwapChainD3D11::Resize(int w, int h)
{
    GX_TRACE("CSwapChainD3D11::Resize(this:%p, w:%d, h:%d)", this, w, h);
    DbgAssert(m_chain);

    this->m_ptrSurface.Release();
    this->m_ptrResView.Release();

    DXGI_SWAP_CHAIN_DESC desc;
    m_chain->GetDesc(&desc);
    DbgReturn0HResult(m_chain->ResizeBuffers(desc.BufferCount, w, h, m_ddsd.Format, desc.Flags));
    m_ddsd.Width = w;
    m_ddsd.Height = h;
    SyncBuffer(m_ptrSurface);
    return true;
}

EGXRet CSwapChainD3D11::Present(TMHWnd hWnd, EGXPresent eFlag, GXPRESENTFX* fx)
{
    GX_TRACE("CSwapChainD3D11::Present(hWnd:%p, eFlag:%x, fx:%p)", hWnd, eFlag, fx);
    DbgAssert(m_chain);

    CGXAppD3D11::CContextPtr ptr(m_ptrDevice);

    if (eFlag == eGXPresentLayered)
        return this->PresentLayeredWin(hWnd, eFlag, fx);// PresentLayeredWin(hWnd, eFlag, fx);

//     if ((eFlag & eGXPresentDCMask) == eGXPresentDCPaint)
//     { CPaintDC dc(hWnd); } // don't use ValidateRect

    HRESULT hr = _RTSSHooksBugs11::PresentSC(m_chain);
    if (FAILED(hr))
        return m_ptrDevice->CheckError(hr);
    return eGXRetSuccess;
}


////////////////////////////////////////////////////////////////////////////////
// CGXAppD3D11::CBufPtr
// void CGXAppD3D11::CBufPtr::Recreate(CGXAppD3D11* device, const void* pData, int nSize, D3D11_BIND_FLAG type)
// {
//     if (nSize > m_nSize)
//     {
//         Release();
//         D3D11_SUBRESOURCE_DATA data = { pData };
//         D3D11_BUFFER_DESC BufferDesc =
//         {
//             nSize,                  // ByteWidth
//             D3D11_USAGE_DYNAMIC,
//             type,
//             D3D11_CPU_ACCESS_WRITE, // CPUAccessFlags
//             0,                      // MiscFlags
//         };
//         HRESULT hr;
//         FAILED_CHECK(device, ;, device->m_device->CreateBuffer(&BufferDesc, &data, &p));
//         m_nSize = nSize;
//     }
//     else
//     {
//         D3D11_MAPPED_SUBRESOURCE resource;
//         CGXAppD3D11::CContextPtr context(device);
//         if (SUCCEEDED(context->Map(p, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource)))
//         {
//             MemCpy(resource.pData, pData, nSize);
//             context->Unmap(p, 0);
//         }
//     }
// }


////////////////////////////////////////////////////////////////////////////////
// CGXAppD3D11
CGXAppD3D11::CGXAppD3D11()
    : m_pBk(0)
{
    GX_TRACE("CGXAppD3D11::CGXAppD3D11(this:%p)", this);
}

CGXAppD3D11::~CGXAppD3D11()
{
    DbgTrace("CGXAppD3D11::~CGXAppD3D11(this:%p)", this);
    DbgAssert(!m_stk.size());
}

// TMBool _CGXAppD3D11_IsSupportDComp(CGXAppD3D11& device)
// {
//     HWND hWnd = CWindowAT::NativeCreate(L"button", 0);
//     CDComposition dcomp;
//     CD3D11_TEXTURE2D_DESC desc;
//     TMBool b = dcomp.CreateDComp(device, hWnd);
//     ::DestroyWindow(hWnd);
//     return b;
// }

TMBool CGXAppD3D11::CreateDevice(IDXGIAdapter* Adapter)
{
    if (!CD3D11Device::CreateDevice(Adapter))
        return false;

//    m_font = CGXFont::FontCreateDefaultAuto();
//    m_font = CGXFont::FontCreateDefaultFromDWrite();
    ResetFontDevice();

    CContextPtr lock(this);

//     static TMBool s_bSupportComp = _CGXAppD3D11_IsSupportDComp(*this);
//     if (!s_bSupportComp)
//         return false;

    // constant
//     ConstantBuffer                      m_CBuffer;
//     D3DMATRIX m = { 1,0,0,0,  0,1,0,0,  0,0,1,0,  0,0,0,1 };
// //     m_CBuffer.world = m;//mXMMatrixIdentity();    // 单位矩阵的转置是它本身
// //     m_CBuffer.view = m;//XMMatrixTranspose(XMMatrixLookAtLH(
//     m_CBuffer.proj = m;//XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV2, AspectRatio(), 1.0f, 1000.0f));


//     D3D11_BUFFER_DESC cbd;
//     ZeroMemory(&cbd, sizeof(cbd));
//     cbd.Usage = D3D11_USAGE_DYNAMIC;
//     cbd.ByteWidth = sizeof(ConstantBuffer);
//     cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//     cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//     DbgReturn0HResult(m_device->CreateBuffer(&cbd, nullptr, &m_pConstant));
//     GetContext()->VSSetConstantBuffers(0, 1, &m_pConstant.p);

    //input layout
//     const D3D11_INPUT_ELEMENT_DESC layout[] =
//     {
//         { "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
//         { "COLOR",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//         { "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//     };
//    DbgReturn0HResult(m_device->CreateInputLayout(layout, tm_countof(layout), cg_VS, sizeof(cg_VS), &m_pInputLayout));
//     DbgReturn0HResult(this->CreateVertexLayoutAndShader(layout, tm_countof(layout), cg_VS, sizeof(cg_VS), &m_pInputLayout, &m_pShaderVertex));
//     this->SetVertexLayout(m_pInputLayout);
// 
//     // Shaders
// //    DbgReturn0HResult(this->CreateVertexShader(cg_VS,     sizeof(cg_VS),     &m_pShaderVertex));
//     DbgReturn0HResult(this->CreatePixelShader (cg_PS,     sizeof(cg_PS),     &m_pShaderPixel));
//     DbgReturn0HResult(this->CreatePixelShader (cg_PS_Tex, sizeof(cg_PS_Tex), &m_pShaderPixel_Tex));
//      this->SetVertexShader(m_pShaderVertex);
//      this->SetPixelShader (m_pShaderPixel);



    // States
    D3D11_BLEND_DESC BSDesc = { 0 };
    BSDesc.RenderTarget[0].BlendEnable = false;
    BSDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    BSDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    BSDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    DbgReturn0HResult(m_device->CreateBlendState(&BSDesc, &m_pBlendStateCopy));

    BSDesc.RenderTarget[0].BlendEnable = true;
    BSDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    BSDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    BSDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    BSDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    DbgReturn0HResult(m_device->CreateBlendState(&BSDesc, &m_pBlendStateAlpha));

//     BSDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
//     BSDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_BLEND_FACTOR;
//     BSDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
//     BSDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_BLEND_FACTOR;
//     DbgReturn0HResult(m_device->CreateBlendState(&BSDesc, &m_pBlendStateMix));

    BSDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE;
    BSDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_DEST_COLOR;
    BSDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
    DbgReturn0HResult(m_device->CreateBlendState(&BSDesc, &m_pBlendStateInvert));

    D3D11_RASTERIZER_DESC RSDesc;
    RSDesc.AntialiasedLineEnable = FALSE;
    RSDesc.CullMode = D3D11_CULL_NONE;//D3D11_CULL_BACK;
    RSDesc.DepthBias = 0;
    RSDesc.DepthBiasClamp = 0.0f;
    RSDesc.DepthClipEnable = false;//TRUE;
    RSDesc.FillMode = D3D11_FILL_SOLID;
    RSDesc.FrontCounterClockwise = FALSE;
    RSDesc.MultisampleEnable = TRUE;
    RSDesc.ScissorEnable = true;
    RSDesc.SlopeScaledDepthBias = 0.0f;
    DbgReturn0HResult(m_device->CreateRasterizerState(&RSDesc, &m_pRasterizerState));
    GetContext()->RSSetState(m_pRasterizerState);

    D3D11_SAMPLER_DESC SSDesc = {
        D3D11_FILTER_MIN_MAG_MIP_POINT,//D3D11_FILTER_ANISOTROPIC, // D3D11_FILTER_MIN_MAG_MIP_LINEAR
        D3D11_TEXTURE_ADDRESS_WRAP,
        D3D11_TEXTURE_ADDRESS_WRAP,
        D3D11_TEXTURE_ADDRESS_WRAP,
        0, 16,
        D3D11_COMPARISON_NEVER,
        { 0, 0, 0, 0 },
        0, D3D11_FLOAT32_MAX
    };
    if (m_device->GetFeatureLevel() < D3D_FEATURE_LEVEL_9_3)
    {
        SSDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        SSDesc.MaxAnisotropy = 0;
    }
    DbgReturn0HResult(m_device->CreateSamplerState(&SSDesc, &m_pSamplerState));
    GetContext()->PSSetSamplers(0, 1, &(m_pSamplerState.p));

    SSDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    DbgReturn0HResult(m_device->CreateSamplerState(&SSDesc, &m_pSamplerStateLinear));

    // States
//    m_context->OMSetDepthStencilState(m_pDepthStencilStateUI, 0);

    PS_DESC descPS      = { cg4_PS,  tm_countof(cg4_PS) };
    PS_DESC descPSTex   = { cg4_PS_Tex,  tm_countof(cg4_PS_Tex) };
    DbgReturn0HResult(ShaderInit(cg_descVS11, descPS, descPSTex, sizeof(CBuffer)));
    
    m_pVertex.Init(this, &m_pIndex, 0);


    return true;
}

void CGXAppD3D11::SamplersSetLinear()
{
    _gx_check();
    GetContext()->PSSetSamplers(0, 1, &(m_pSamplerStateLinear.p));
}

void CGXAppD3D11::SamplersResetLinear()
{
    _gx_check();
    GetContext()->PSSetSamplers(0, 1, &(m_pSamplerState.p));
}

void CGXAppD3D11::BlendSetOver()
{
    _gx_check();
    GetContext()->OMSetBlendState(m_pBlendStateAlpha, 0, 0xFFFFFFFF);
}

void CGXAppD3D11::BlendSetCopy()
{
//     _gx_check();
    GetContext()->OMSetBlendState(m_pBlendStateCopy, 0, 0xFFFFFFFF);
}

void CGXAppD3D11::BlendSetInvert()
{
    _gx_check();
    GetContext()->OMSetBlendState(m_pBlendStateInvert, 0, 0xFFFFFFFF);
}

void CGXAppD3D11::BlendResetInvert()
{
    _gx_check();
    GetContext()->OMSetBlendState(m_pBlendStateAlpha, 0, 0xFFFFFFFF);
}

void CGXAppD3D11::InitDevTransform()
{
    DbgAssert(m_pBk);

    float fDPI = GetDPI();
    float f1 = (float)TM_DPI_SYSTEM_DEFAULT / fDPI;
    float f2 = fDPI / (float)TM_DPI_SYSTEM_DEFAULT;
    CAffine aff(f1,0, 0,f1, 0,0);
    aff *= m_aff;
    aff.Scale(f2, f2);

    float w = m_pBk->GetWidth();
    float h = m_pBk->GetHeight();
    const float df = 0.0f;
    CAffine affProj;
    if (IsRTL())
        affProj.Set(-2.0f / (float)w, 0, 0, -2.0f / h, (w - df) / w, (h + df) / h);
    else
        affProj.Set(2.0f / (float)w, 0, 0, -2.0f / h, -(w + df) / w, (h + df) / h);

    aff *= affProj;

    matrix mt =
    {
        aff.M11(), aff.M21(), 0, aff.Dx(),
        aff.M12(), aff.M22(), 0, aff.Dy(),
        0, 0, 1, 0,
        0, 0, 0, 1,
    };
    CBuffer m_CBuffer;
    m_CBuffer.proj = mt;

    CLockPtr lock(this);
//     SetVSConstantF(&m_CBuffer, 4);
    GetVSCBufffer()->SetDataVec4(this, &m_CBuffer, 4);

//     CGXAppD3D11::CContextPtr context(this);
//     _gx_check();
//     D3D11_MAPPED_SUBRESOURCE mappedData;
//     if (SUCCEEDED(context->Map(m_pConstant, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData)))
//     {
//         *(ConstantBuffer*)mappedData.pData = m_CBuffer;
//         context->Unmap(m_pConstant, 0);
//     }
}

void CGXAppD3D11::ClipDevNow()
{
    DbgAssert(m_pBk);
    CRect rc(m_rcClip);
    if (IsRTL())
    {
        int w = m_pBk->GetWidth();
        int n = w - rc.left;
        rc.left = w - rc.right;
        rc.right = n;
    }
    rc &= CRect(0, 0, m_pBk->GetWidth(), m_pBk->GetHeight());
    CLockPtr(this)->SetScissorRect(rc);
    GX_TRACE("CGXAppD3D11::ClipDevNow(" TM_FMT_RCI_STR ")", TM_FMT_RCI_VAL(rc));
}

EGXDevice CGXAppD3D11::GetDeviceType(GXADAPTERDESC* desc)
{
    if (desc)
        this->GetDeviceDesc(*desc);
    return eGXGraDevD3D11 | m_font->FontGetDeviceType();
}

GXPTexture CGXAppD3D11::GetRenderTarget()
{
    return m_pBk;
}


////////////////////////////////////////////////////////////////////////////////
// texture
void CGXAppD3D11::TextureCreateRenderTarget(IGXTexture** tex, int w, int h, EGXFormat format)
{
    GX_TRACE("CGXAppD3D11::TextureCreateRenderTarget(this:%p, w:%d, h:%d, fmt:%d)", this, w, h, format);
    DbgAssert(tex && !*tex);

    DXGI_FORMAT fmt;
    switch (format)
    {
    case eGXFormat16:   fmt = DXGI_FORMAT_B5G6R5_UNORM;     break;
    case eGXFormat32A:  fmt = DXGI_FORMAT_B8G8R8A8_UNORM;   break;
    case eGXFormat32X:  fmt = DXGI_FORMAT_B8G8R8X8_UNORM;   break;
    default: DbgTrace("CGXAppD3D11::TextureCreateRenderTarget(%d, %d, %d) fail", w, h, format); return;
    }
    *tex = CTextureD3D11::CreateTex(
        this, w, h, format, fmt,
        D3D11_CREATE_TEX2D_PARAM_RENDERTARGET,
        0);
}

void CGXAppD3D11::TextureCreateSwapChain(IGXTexture** tex, TMHWnd hWnd, int w, int h, EGXFormat format)
{
    GX_TRACE("CGXAppD3D11::TextureCreateSwapChain(this:%p, hwnd:%p, w:%d, h:%d, fmt:%d)", this, hWnd, w, h, format);
    DbgAssert(tex && !*tex);

    CSwapChainD3D11* p = CTextureBltT<CSwapChainD3D11, CGXAppD3D11::CVertex_Texture>::Inst(); //new CRefObjectT<CSwapChainD3D11>(1);
    DXGI_FORMAT fmtDXGI = format == eGXFormat32A ? DXGI_FORMAT_B8G8R8A8_UNORM : DXGI_FORMAT_B8G8R8X8_UNORM;
    HRESULT hr;
    DbgVerifyHResult(hr = p->CreateChain(this, hWnd, w, h, fmtDXGI));
    if (FAILED(hr))
    {
        this->CheckError(hr);
        p->Release();
    }
    else
    {
        CComPtr<ID3D11Texture2D> _tex;
        p->SyncBuffer(_tex);
        p->m_fmt = format;
        p->Init(this, _tex);
        TM_IF_DBG(p->m_eType = eTextureTypeSwapChain);
        *tex = p;
    }
}

void CGXAppD3D11::TextureCreateIndirect(IGXTexture** tex, int w, int h, EGXFormat format)
{
    GX_TRACE("CGXAppD3D11::TextureCreateIndirect(this:%p, w:%d, h:%d, fmt:%d)", this, w, h, format);
    DbgAssert(tex && !*tex);
    DXGI_FORMAT fmt;
    switch (format)
    {
        case eGXFormat16:   fmt = DXGI_FORMAT_B5G6R5_UNORM;     break;
//         case eGXFormat24:   fmt = DXGI_FORMAT_B8G8R8A8_UNORM;   break;
        case eGXFormat32A:  fmt = DXGI_FORMAT_B8G8R8A8_UNORM;   break;
        case eGXFormat32X:  fmt = DXGI_FORMAT_B8G8R8X8_UNORM;   break;
        case eGXFormatNV12:
            *tex = CTextureD3D11NV12::CreateTex(this, w, h, 0);
            return;
        default: DbgTrace("CGXAppD3D11::TextureCreateIndirect(%d, %d, %d) fail", w, h, format); return;
    }
    *tex = CTextureD3D11::CreateTex(
        this, w, h, format, fmt,
        D3D11_CREATE_TEX2D_PARAM_INDIRECT,
        0);
}


////////////////////////////////////////////////////////////////////////////////
// shape
void CGXAppD3D11::Clear(TMColor clr)
{
    _gx_check();
    m_pBk->ColorFill(clr);
}

void CGXAppD3D11::DrawStrip(_CDrawStrip11& ds)
{
    DbgAssert(ds.nVertexCount);

    m_pVertex.Recreate(this, ds.pData, ds.nBufferSize); 

    // Draw
//     UINT Stride = ds.nStride;
//     UINT Offset = 0;
    m_pVertex.SetCurrent(this, ds.nStride); //GetContext()->IASetVertexBuffers(0, 1, &m_pVertex.obj.p, &Stride, &Offset);
    m_pVertex.Draw(this, ds.PType, ds.nVertexCount);
//     GetContext()->IASetPrimitiveTopology(ds.PType);
//     GetContext()->Draw(ds.nVertexCount, 0);
}

void CGXAppD3D11::VertexFill(const GXIndices* pt, TMUInt nt)
{
//    int nv = m_vertex.m_aTv.size();
    CGXAppD3D11::CLockPtr lock(this);

    m_pVertex.Recreate(this, m_vertex.buffer(), m_vertex.sizeBuffer()); 

//    BlendSetCopy(0);
    m_pVertex.SetCurrent(this, m_vertex.eSize);   //context->IASetVertexBuffers(0, 1, &m_pVertex.obj.p, &Stride, &Offset);
//     m_pIndex.Recreate(this, pt, nt * sizeof(GXIndices));
//     m_pIndex.SetCurrent(this, DXGI_FORMAT_R32_UINT);  // context->IASetIndexBuffer(m_pIndex.obj, DXGI_FORMAT_R32_UINT, 0);
//     UINT Stride = m_vertex.eSize;
//     UINT Offset = 0;
//     context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//     context->DrawIndexed(nt, 0, 0);
//    m_pIndex.Draw(this, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, nt, DXGI_FORMAT_R32_UINT);
    m_pIndex.DrawUINT(this, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, nt, pt);
//    BlendResetCopy();
}

EGXRet CGXAppD3D11::CheckError(HRESULT hr)
{
    GX_TRACE("CGXAppD3D11::CheckError(%x)", hr);
    CGXDevMgr* gm = CGXDevMgr::Instance();
    if (gm->IsDeviceLost() || !gm->IsCurrentDevice(this))
        return eGXRetDeviceChanged;

    gm->NotifyDispatchError(hr);

    if (D3DERR_OUTOFVIDEOMEMORY == hr ||    // 0x8876017C
        E_OUTOFMEMORY == hr)                // 0x8007000e 
        return (gm->SetDeviceLost(eDeviceResetGDI), eGXRetDeviceChanged);

    if (E_UNEXPECTED == hr ||               // intel bug
        DXGI_ERROR_DEVICE_RESET == hr ||
        DXGI_ERROR_DEVICE_REMOVED == hr ||  // 0x887A0005, no video memory
        DXGI_ERROR_DEVICE_HUNG == hr)
        return (gm->SetDeviceLost(eDeviceReset3s), eGXRetDeviceChanged);

    return eGXRetFail;
}

EGXRet CGXAppD3D11::InitProp(GXPTexture texture, int nDPI, TMUInt32 dwFlag)
{
    return _GXApp_InitProp(this, static_cast<CTextureD3D11*>(texture), nDPI, dwFlag);
}


TM_END_NAMESPACE



