/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2020.2.24
 *
 *  Copyright (C) 2020 miragekiller
 */

#pragma once

#ifndef GXD3D9_H
#define GXD3D9_H

#include "tmgdix.h"
#include "tmdll.h"

#ifdef TM_DBG
#   define D3D_DEBUG_INFO
#endif
#include <d3d9.h>

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "dxguid.lib")

#if (TM_MSC <  1300)
#   pragma comment (lib, "d3dx9.lib") // for IID_IDirect3DTexture9
#endif


typedef IDirect3D9* (WINAPI *PFNDirect3DCreate9)(UINT SDKVersion);

struct CDirect3DCreater
{
    TM::CDll m_dll;
    PFNDirect3DCreate9 m_pfnDirect3DCreate9;

    CDirect3DCreater()
    {
#ifdef TM_DBG
        DllLoad(TM_T("d3d9.dll"), LOAD_LIBRARY_SEARCH_APPLICATION_DIR); // enable forced3d9 dll
#endif
        m_dll.LoadLibrary(TM_T("d3d9.dll"), LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (m_dll)
            m_pfnDirect3DCreate9 = (PFNDirect3DCreate9)m_dll.GetProcAddress("Direct3DCreate9");
    }

    operator TMBool() const { return 0 != m_pfnDirect3DCreate9; }
};


inline static D3DFORMAT dxGXFormat2D3DFormat(EGXFormat nBpp)
{
    switch (nBpp)
    {
    case eGXFormatPAL2:
    case eGXFormatPAL4:
    case eGXFormatPAL8:
        return D3DFMT_UNKNOWN;
        
    case eGXFormat16:       return D3DFMT_X1R5G5B5;
    case eGXFormat24:       return D3DFMT_R8G8B8;
    case eGXFormat32A:      return D3DFMT_A8R8G8B8;
    case eGXFormat32X:      return D3DFMT_X8R8G8B8;
    case eGXFormatA8B8G8R8: return D3DFMT_A8B8G8R8;
    case eGXFormatX8B8G8R8: return D3DFMT_X8B8G8R8;
        
    default:
        return (D3DFORMAT)nBpp;;
    }
}

inline static EGXFormat dxD3DFormat2GXFormat(D3DFORMAT nBpp)
{
    switch (nBpp)
    {
    case D3DFMT_X1R5G5B5: return eGXFormat16;
    case D3DFMT_R8G8B8:   return eGXFormat24;
    case D3DFMT_A8R8G8B8: return eGXFormat32A;
    case D3DFMT_X8R8G8B8: return eGXFormat32X;
    case D3DFMT_A8B8G8R8: return eGXFormatA8B8G8R8;
    case D3DFMT_X8B8G8R8: return eGXFormatX8B8G8R8;
        
    default:
        return (EGXFormat)nBpp;
    }
}

inline static void dxMakePresentParamters(D3DPRESENT_PARAMETERS& pp, UINT w, UINT h, D3DSWAPEFFECT SwapEffect)
{
    pp.Windowed = TRUE;
    pp.SwapEffect = SwapEffect;
    pp.BackBufferWidth = w;
    pp.BackBufferHeight = h;
}

inline static bool dxCreateDevice9(UINT Adapter, IDirect3DDevice9** ppd3dDevice9, IDirect3D9* ptrD3D, HWND hWnd, UINT w = 0, UINT h = 0, D3DSWAPEFFECT SwapEffect = D3DSWAPEFFECT_COPY, TMBool bDisableHardware = false)
{
    DbgAssert(ptrD3D);

    D3DPRESENT_PARAMETERS pp = {0};
    dxMakePresentParamters(pp, w, h, SwapEffect);
    
    CComPtr<IDirect3DDevice9> pd3dDevice9;
    if (!bDisableHardware)
    {
        DbgVerifyHResult(ptrD3D->CreateDevice(Adapter,
            D3DDEVTYPE_HAL,
            hWnd, // ::GetShellWindow()
            D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE | D3DCREATE_MULTITHREADED,
            &pp,
            ppd3dDevice9));
        if (*ppd3dDevice9)
            return true;
    }

    DbgVerifyHResult(ptrD3D->CreateDevice(Adapter,
        D3DDEVTYPE_HAL,
        hWnd, // ::GetShellWindow()
        D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE | D3DCREATE_MULTITHREADED,
        &pp,
        ppd3dDevice9));
    return 0 != *ppd3dDevice9;
}


struct CD3D9Device : public TM::CNoCopyable
{
public:
    typedef IDirect3DVertexDeclaration9 T_VertexLayout;
    typedef IDirect3DVertexShader9      T_VertexShader;
    typedef IDirect3DPixelShader9       T_PixelShader;

    struct VS_DESC
    {
        const void* code;
        CONST D3DVERTEXELEMENT9* decl;
    };
    struct PS_DESC
    {
        const void* code;
    };

    struct _Buffer
    {
        static HRESULT Create(CD3D9Device* device, size_t sz) { return S_OK; }
        static void Release(CD3D9Device*) {}
        static void SetCurrent(CD3D9Device*) {}
        
    };
    
    struct T_VSCBuffer : public _Buffer
    {
        HRESULT SetDataVec4(CD3D9Device* device, const void* pConstantData, UINT Vector4fCount)
        {
            return device->SetVSConstantF(pConstantData, Vector4fCount);
        }
    };
    
    struct T_PSCBuffer : public _Buffer
    {
        HRESULT SetDataVec4(CD3D9Device* device, const void* pConstantData, UINT Vector4fCount)
        {
            return device->SetPSConstantF(pConstantData, Vector4fCount);
        }
    };
    T_VSCBuffer* GetVSCBufffer() { return 0; }
    T_PSCBuffer* GetPSCBufffer() { return 0; }

    struct T_VS : public TM::CNoCopyable  // vertex shader
    {
        IDirect3DVertexDeclaration9*    ptrLayout;
        IDirect3DVertexShader9*         ptrShader;
        
        T_VS() : ptrLayout(0), ptrShader(0) {}

        void Release(CD3D9Device*)
        {
            if (ptrLayout)
            {
                ptrLayout->Release();
                ptrLayout = 0;
            }
            if (ptrShader)
            {
                ptrShader->Release();
                ptrShader = 0;
            }
        }
        
//         HRESULT Create(
//             CD3D9Device* device,
//             CONST D3DVERTEXELEMENT9* pVertexElements, UINT NumElements,
//             const void* pShaderBytecode, size_t BytecodeLength)
//         {
//             return device->CreateVertexLayoutAndShader(pVertexElements, NumElements,
//                 pShaderBytecode, &ptrLayout, &ptrShader);
//         }

        HRESULT Create(CD3D9Device* device, VS_DESC desc)
        {
            DbgAssert(!ptrLayout);
            DbgAssert(!ptrShader);
            return device->CreateVertexLayoutAndShader(desc.decl,
                desc.code, &ptrLayout, &ptrShader);
        }
        
        void SetCurrent(CD3D9Device* device)
        {
            DbgAssert(ptrLayout);
            DbgAssert(ptrShader);
            device->SetVertexLayout(ptrLayout);
            device->SetVertexShader(ptrShader);
        }

//         HRESULT SetConstantF(CD3D9Device* device, const void* pConstantData, UINT Vector4fCount)
//         {
//             return device->SetVSConstantF(pConstantData, Vector4fCount);
//         }
    };
    
    struct T_PS : public TM::CNoCopyable   // pixel shader
    {
        IDirect3DPixelShader9*          ptrShader;
        
        T_PS() : ptrShader(0) {}

        void Release(CD3D9Device*) { if (ptrShader) { ptrShader->Release(); ptrShader = 0; } }

        operator bool () const { return 0 != ptrShader; }

        HRESULT Create(CD3D9Device* device, PS_DESC desc, const T_VS&, const VS_DESC&)
        {
            return Create(device, desc);
        }

        HRESULT Create(CD3D9Device* device, PS_DESC desc)
        {
            DbgAssert(!ptrShader);
            return device->CreatePixelShader(desc.code, &ptrShader);
        }

        void SetCurrent(CD3D9Device* device)
        {
            DbgAssert(ptrShader);
            device->SetPixelShader(ptrShader);
        }
    };

    // D3D9On12.dll may be throw 0xC0000005
    struct _D3D9On12Bugs
    {
        static HRESULT LockSur9(CD3D9Device* app, IDirect3DSurface9* p, D3DLOCKED_RECT& dlr, CONST RECT* rc, DWORD flag)
        {
            if (!app->m_bD3D9On12)
                return p->LockRect(&dlr, rc, flag);
            TM_TRY_ALL(return p->LockRect(&dlr, rc, flag));
            return E_UNEXPECTED;
        }
        
        static HRESULT LockTex9(CD3D9Device* app, IDirect3DTexture9* p, D3DLOCKED_RECT& dlr)
        {
            if (!app->m_bD3D9On12)
                return p->LockRect(0, &dlr, 0, D3DLOCK_DISCARD);
            TM_TRY_ALL(return p->LockRect(0, &dlr, 0, D3DLOCK_DISCARD));
            return E_UNEXPECTED;
        }
        
        static HRESULT CreateSC(CD3D9Device* app, D3DPRESENT_PARAMETERS* pp, IDirect3DSwapChain9** ps)
        {
            if (!app->m_bD3D9On12)
                return app->m_pd3dDevice9->CreateAdditionalSwapChain(pp, ps);
            TM_TRY_ALL(return app->m_pd3dDevice9->CreateAdditionalSwapChain(pp, ps));
            return E_UNEXPECTED;
        }
    };

    // for Rivatuner Statistics Server crash:
    // KERNELBASE.dll!_RaiseException@16.()
    // msvcrt.dll!__CxxThrowException@8.()
    // d3d9.dll!InterpolateColor(unsigned long *,unsigned long,unsigned long,float)
    // d3d9.dll!CD3DHal::ValidateDraw(enum _D3DPRIMITIVETYPE,unsigned int,unsigned int,unsigned int,int,unsigned int,int)
    // d3d9.dll!CD3DBase::DrawPrimitive(enum _D3DPRIMITIVETYPE,unsigned int,unsigned int)
    // RTSSHooks.dll!027358e7()
    struct _RTSSHooksBugs
    {
        static HRESULT PresentSC(IDirect3DSwapChain9* psc, HWND hWnd, const TMRect* rc)
        {
            TM_TRY_ALL(return psc->Present(rc, rc, hWnd, NULL, 0));
            return E_UNEXPECTED;
        }
    };

public:
    CDirect3DCreater                m_creater;
    CComPtr<IDirect3DDevice9>       m_pd3dDevice9;
    UINT                            m_Adapter;
    bool                            m_bXP;                          // XP not support RTL.
    bool                            m_bD3D9On12;                    // D3D9On12.dll have more bugs
    bool                            m_D3DPBLENDCAPS_BLENDFACTOR;    // Intel 865G not support.

public:
    operator bool () const { return m_pd3dDevice9 != 0; }

    static bool IsATIBlackDevice(const D3DADAPTER_IDENTIFIER9& id9)
    {
        // error device on layered window:
        // new create texture and blt success, but target see nothing.
        // old texture can blt to target.
        //vendorID:1002 - deviceID:68f9 - SubSysId:21301462 - Revision:0 - AMD Radeon HD 5450 - videomemory:-9437184
        //AMD68F9.1 = "AMD Radeon HD 6350"
        //AMD68F9.2 = "ATI Radeon HD 6250"
        //AMD68F9.3 = "ATI Radeon HD 6290"
        //AMD68F9.4 = "ATI Radeon HD 6230"
        //AMD68F9.5 = "ATI Radeon HD 6350"
        //AMD68F9.6 = "AMD Radeon R5 220"
        //AMD68F9.7 = "ATI Radeon HD 7350"
        //AMD68F9.8 = "AMD Radeon HD8350"
        //AMD68F9.9 = "AMD Radeon HD 7350"
        //AMD68F9.10 = "ATI Radeon HD 6350 Graphics"
        //AMD68F9.11 = "AMD Radeon HD 6350A Graphics"
        //AMD68F9.12 = "AMD Radeon HD 7350"
        //AMD68F9.13 = "AMD Radeon HD 8350"
        //AMD68F9.14 = "AMD Radeon R5 220"
        //AMD68F9.15 = "ATI Radeon HD 6350 Graphics"
        //AMD68F9.16 = "ATI Radeon HD 5470"
        //AMD68F9.17 = "ATI Radeon HD 5490"
        //AMD68F9.18 = "ATI Radeon HD 5530"
        //AMD68F9.19 = "ATI Radeon HD 6230"
        //AMD68F9.20 = "ATI Radeon HD 6350"
        //AMD68F9.21 = "ATI Radeon HD 7350"
        //AMD68F9.22 = "AMD Radeon HD 8350"
        //AMD68F9.23 = "AMD Radeon R5 220"
        //AMD68F9.24 = "ATI Radeon HD 5470"
        //AMD68F9.25 = "ATI Radeon HD 5530"
        //AMD68F9.26 = "ATI Radeon HD 5490"
        //AMD68F9.27 = "AMD Radeon R5 220"
        //AMD68F9.28 = "ATI Radeon HD 6250"
        //AMD68F9.29 = "ATI Radeon HD 6230"
        //AMD68F9.30 = "ATI Radeon HD 6290"
        //AMD68F9.31 = "ATI Radeon HD 5470"
        //AMD68F9.32 = "ATI Radeon HD 5490"
        //AMD68F9.33 = "ATI Radeon HD 5470"
        //AMD68F9.34 = "ATI Radeon HD 6350"
        //AMD68F9.35 = "ATI Radeon HD 6230"
        //AMD68F9.36 = "ATI Radeon HD 6250"
        //AMD68F9.37 = "ATI Radeon HD 6290"
        //AMD68F9.38 = "AMD Radeon HD 5450"
        return id9.VendorId == nVendorID_ATI && id9.DeviceId == 0x68f9;
    }

    static bool IsNvidiaBlackDevice(const D3DADAPTER_IDENTIFIER9& id9)
    {
        return false; // id9.VendorId == nVendorID_NVIDIA && id9.DeviceId == 0x1BB1;
    }

    bool CreateDevice(UINT Adapter = D3DADAPTER_DEFAULT)
    {
        m_bXP = COSVersion::GetVersion() <= COSVersion::eVersionXp;
        m_D3DPBLENDCAPS_BLENDFACTOR = true;
        m_bD3D9On12 = (0 != DllHandle(TM_T("d3d9on12.dll")));

        const UINT D3D9_PP_WIDTH  = 1;
        const UINT D3D9_PP_HEIGHT = 1;
        const D3DSWAPEFFECT D3D9_PP_EFF =  D3DSWAPEFFECT_COPY;

        DbgAssert(m_creater);
        CComPtr<IDirect3D9> ptrD3D;
        ptrD3D.Attach(m_creater.m_pfnDirect3DCreate9(D3D_SDK_VERSION));
        if (!ptrD3D)
            return false;

        D3DCAPS9 caps;
        if (FAILED(ptrD3D->GetDeviceCaps(Adapter, D3DDEVTYPE_HAL, &caps)))
            return false;

        if (caps.PixelShaderVersion < D3DPS_VERSION(2,0))
        {
            DbgTrace("CD3D9Device::CreateDevice() failed: PixelShaderVersion less 2.0");
            return false;
        }

        D3DADAPTER_IDENTIFIER9 id9;
        DbgVerifyHResult(ptrD3D->GetAdapterIdentifier(Adapter, 0, &id9));
        TMBool bDisableHardware = IsATIBlackDevice(id9) || IsNvidiaBlackDevice(id9);

        if (!(caps.RasterCaps & D3DPRASTERCAPS_SCISSORTEST))
        {
            DbgTrace("CD3D9Device::CreateDevice() failed: not support D3DPRASTERCAPS_SCISSORTEST");
            return false;
        }

        m_Adapter = Adapter;
        return dxCreateDevice9(
            Adapter,
            &m_pd3dDevice9,
            ptrD3D,
            ::GetDesktopWindow(), // ::GetShellWindow()
            D3D9_PP_WIDTH, D3D9_PP_HEIGHT, D3D9_PP_EFF, bDisableHardware
           );
    }

    void GetDeviceDesc(GXADAPTERDESC& desc)
    {
        CComPtr<IDirect3D9> ptrD3D;
        DbgVerifyHResult(m_pd3dDevice9->GetDirect3D(&ptrD3D));
        D3DADAPTER_IDENTIFIER9 id9;
        DbgVerifyHResult(ptrD3D->GetAdapterIdentifier(m_Adapter, 0, &id9));
        StrAssign(desc.Description, tm_countof(desc.Description), id9.Description, -1);
        desc.VendorId = id9.VendorId;
        desc.DeviceId = id9.DeviceId;
        desc.SubSysId = id9.SubSysId;
        desc.Revision = id9.Revision;
        desc.DedicatedVideoMemory = m_pd3dDevice9->GetAvailableTextureMem();
        desc.DedicatedSystemMemory = 0;
        desc.SharedSystemMemory = 0;
        desc.AdapterLuid = id9.DeviceIdentifier;
        desc.DriverVersion = id9.DriverVersion.QuadPart;
    }

    void EnableScissor()
    {
        DbgAssert(m_pd3dDevice9);
        DbgVerifyHResult(m_pd3dDevice9->SetRenderState(D3DRS_SCISSORTESTENABLE, true));
    }
    
    void DisableScissor()
    {
        DbgAssert(m_pd3dDevice9);
        DbgVerifyHResult(m_pd3dDevice9->SetRenderState(D3DRS_SCISSORTESTENABLE, false));
    }

    void SetScissorRect(const TMRect& rc)
    {
        DbgAssert(m_pd3dDevice9);
        DbgVerifyHResult(m_pd3dDevice9->SetScissorRect(&rc));
    }

    void SetTextureAt(DWORD Stage, IDirect3DSurface9* surface)
    {
        DbgAssert(surface);
        CComPtr<IDirect3DTexture9> tex;
        DbgVerifyHResult(surface->GetContainer(IID_IDirect3DTexture9, (void**)&tex));
        DbgAssert(tex);
        SetTextureAt(Stage, tex);
    }

    void SetTextureAt(DWORD Stage, IDirect3DBaseTexture9* pTexture)
    {
        DbgAssert(m_pd3dDevice9);
        DbgVerifyHResult(m_pd3dDevice9->SetTexture(Stage, pTexture));
    }

    void SetTexture1(IDirect3DBaseTexture9* tex0)
    {
        SetTextureAt(0, tex0);
    }

    void SetTexture2(IDirect3DBaseTexture9* tex0, IDirect3DBaseTexture9* tex1)
    {
        SetTextureAt(1, tex1);
        SetTextureAt(0, tex0);
    }
    
    void SetTexture3(IDirect3DBaseTexture9* tex0, IDirect3DBaseTexture9* tex1, IDirect3DBaseTexture9* tex2)
    {
        SetTextureAt(2, tex2);
        SetTexture2(tex0, tex1);
    }

    void SetRenderTarget(IDirect3DSurface9* pRenderTarget)
    {
        DbgAssert(m_pd3dDevice9);
        DbgVerifyHResult(m_pd3dDevice9->SetRenderTarget(0, pRenderTarget));
    }

    void SetRenderTarget(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pNewZStencil)
    {
        DbgAssert(m_pd3dDevice9);
        DbgVerifyHResult(m_pd3dDevice9->SetRenderTarget(0, pRenderTarget));
        DbgVerifyHResult(m_pd3dDevice9->SetDepthStencilSurface(pNewZStencil));
    }

    HRESULT CreateVertexLayout(
        CONST D3DVERTEXELEMENT9* pVertexElements, UINT NumElements,
        const void *pShaderBytecode, size_t BytecodeLength,
        IDirect3DVertexDeclaration9** ppDecl)
    {
        DbgAssert(m_pd3dDevice9);
        HRESULT hr;
        DbgVerifyHResult(hr = m_pd3dDevice9->CreateVertexDeclaration(pVertexElements, ppDecl));
        return hr;
    }

    void SetVertexLayout(IDirect3DVertexDeclaration9* pDecl)
    {
        DbgAssert(m_pd3dDevice9);
        DbgVerifyHResult(m_pd3dDevice9->SetVertexDeclaration(pDecl));
    }

    HRESULT CreateVertexLayoutAndShader(
        CONST D3DVERTEXELEMENT9* pVertexElements,
        const void *pShaderBytecode,
        IDirect3DVertexDeclaration9** ppDecl, IDirect3DVertexShader9 **ppShader)
    {
        DbgAssert(m_pd3dDevice9);
        HRESULT hr;
        DbgVerifyHResult(hr = m_pd3dDevice9->CreateVertexDeclaration(pVertexElements, ppDecl));
        DbgVerifyHResult(hr |= m_pd3dDevice9->CreateVertexShader((const DWORD*)pShaderBytecode, ppShader));
        return hr;
    }

    HRESULT CreateVertexShader(const void *pShaderBytecode, IDirect3DVertexShader9 **ppShader)
    {
        DbgAssert(m_pd3dDevice9);
        HRESULT hr;
        DbgVerifyHResult(hr = m_pd3dDevice9->CreateVertexShader((const DWORD*)pShaderBytecode, ppShader));
        return hr;
    }

    void SetVertexShader(IDirect3DVertexShader9* pShader)
    {
        DbgAssert(m_pd3dDevice9);
        DbgVerifyHResult(m_pd3dDevice9->SetVertexShader(pShader));
    }
    
    HRESULT CreatePixelShader(const void *pShaderBytecode, IDirect3DPixelShader9 **ppShader)
    {
        DbgAssert(m_pd3dDevice9);
        HRESULT hr;
        DbgVerifyHResult(hr = m_pd3dDevice9->CreatePixelShader((const DWORD*)pShaderBytecode, ppShader));
        return hr;
    }

    void SetPixelShader(IDirect3DPixelShader9* pShader)
    {
        DbgAssert(m_pd3dDevice9);
        DbgVerifyHResult(m_pd3dDevice9->SetPixelShader(pShader));
    }
    
    HRESULT InitVSConstantBuffer(size_t sz) { return S_OK; }
    HRESULT InitPSConstantBuffer(size_t sz) { return S_OK; }

    HRESULT SetVSConstantF(const void* pConstantData, UINT Vector4fCount)
    {
        DbgAssert(m_pd3dDevice9);
        HRESULT hr;
        DbgVerifyHResult(hr = m_pd3dDevice9->SetVertexShaderConstantF(0, (const float*)pConstantData, Vector4fCount));
        return hr;
    }

    HRESULT SetPSConstantF(const void* pConstantData, UINT Vector4fCount)
    {
        DbgAssert(m_pd3dDevice9);
        HRESULT hr;
        DbgVerifyHResult(hr = m_pd3dDevice9->SetPixelShaderConstantF(0, (const float*)pConstantData, Vector4fCount));
        return hr;
    }

    HRESULT SurfaceLockRect(IDirect3DSurface9* p, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags)
    {
        HRESULT hr;
        DbgVerifyHResult(hr = _D3D9On12Bugs::LockSur9(this, p, *pLockedRect, pRect, Flags));
        return hr;
    }

    HRESULT TextureLockRect(IDirect3DTexture9* p, D3DLOCKED_RECT* pLockedRect)
    {
        HRESULT hr;
        DbgVerifyHResult(hr = _D3D9On12Bugs::LockTex9(this, p, *pLockedRect));
        return hr;
    }

    HRESULT SwapChainCreate(D3DPRESENT_PARAMETERS*pp, IDirect3DSwapChain9** ps)
    {
        HRESULT hr;
        DbgVerifyHResult(hr = _D3D9On12Bugs::CreateSC(this, pp, ps));
        return hr;
    }

};



//     struct CAggD3DSurface : public IDirect3DSurface9
//     {
//         CImage m_img;
//         
//         TM_REFAPI AddRef  () { D3DAGG_TRACE("CAggD3DSurface::AddRef"); return 1; }
//         TM_REFAPI Release () { D3DAGG_TRACE("CAggD3DSurface::Release"); return 1; }
// 
//         TM_BGN_INTERFACE_MAP    (IDirect3DSurface9)
//             D3DAGG_TRACE("CAggD3DSurface::QueryInterface:%s",  _uuid_string(iid).c_str()); 
//             TM_INTERFACE_HANDLER(IDirect3DSurface9)
//         TM_END_INTERFACE_MAP()
// 
//         STDMETHOD(GetDevice)(THIS_ IDirect3DDevice9** ppDevice)
//         {
//             D3DAGG_TRACE("CAggD3DSurface::GetDevice");
//             return E_NOTIMPL;
//         }
//         STDMETHOD(SetPrivateData)(THIS_ REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags)
//         {
//             D3DAGG_TRACE("CAggD3DSurface::SetPrivateData");
//             return E_NOTIMPL;
//         }
//         STDMETHOD(GetPrivateData)(THIS_ REFGUID refguid,void* pData,DWORD* pSizeOfData)
//         {
//             D3DAGG_TRACE("CAggD3DSurface::GetPrivateData");
//             return E_NOTIMPL;
//         }
//         STDMETHOD(FreePrivateData)(THIS_ REFGUID refguid)
//         {
//             D3DAGG_TRACE("CAggD3DSurface::FreePrivateData");
//             return E_NOTIMPL;
//         }
//         STDMETHOD_(DWORD, SetPriority)(THIS_ DWORD PriorityNew)
//         {
//             D3DAGG_TRACE("CAggD3DSurface::SetPriority");
//             return E_NOTIMPL;
//         }
//         STDMETHOD_(DWORD, GetPriority)(THIS)
//         {
//             D3DAGG_TRACE("CAggD3DSurface::GetPriority");
//             return E_NOTIMPL;
//         }
//         STDMETHOD_(void, PreLoad)(THIS)
//         {
//             D3DAGG_TRACE("CAggD3DSurface::PreLoad");
//         }
//         STDMETHOD_(D3DRESOURCETYPE, GetType)(THIS)
//         {
//             D3DAGG_TRACE("CAggD3DSurface::GetType");
//             return D3DRTYPE_SURFACE;
//         }
//         STDMETHOD(GetContainer)(THIS_ REFIID riid,void** ppContainer)
//         {
//             D3DAGG_TRACE("CAggD3DSurface::GetContainer");
//             return E_NOTIMPL;
//         }
//         STDMETHOD(GetDesc)(THIS_ D3DSURFACE_DESC *pDesc)
//         {
//             D3DAGG_TRACE("CAggD3DSurface::GetDesc");
//             pDesc->Format = D3DFMT_A8R8G8B8;
//             pDesc->Type = D3DRTYPE_SURFACE;
//             pDesc->Usage = 0;
//             pDesc->Pool = D3DPOOL_SYSTEMMEM;
//             pDesc->MultiSampleType = D3DMULTISAMPLE_NONE;
//             pDesc->MultiSampleQuality = 0;
//             pDesc->Width = m_img.GetWidth();
//             pDesc->Height = m_img.GetHeight();
//             return S_OK;
//         }
//         STDMETHOD(LockRect)(THIS_ D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags)
//         {
//             D3DAGG_TRACE("CAggD3DSurface::LockRect");
//             CDibHandle32 dib;
//             ImageLockRect(m_img, dib, pRect);
//             pLockedRect->Pitch = dib.m_wb;
//             pLockedRect->pBits = dib.m_bits;
//             return S_OK;
//         }
//         STDMETHOD(UnlockRect)(THIS)
//         {
//             D3DAGG_TRACE("CAggD3DSurface::UnlockRect");
//             return E_NOTIMPL;
//         }
// 
//         STDMETHOD(GetDC)(THIS_ HDC *phdc)
//         {
//             D3DAGG_TRACE("CAggD3DSurface::GetDC");
//             return S_OK;
//         }
// 
//         STDMETHOD(ReleaseDC)(THIS_ HDC hdc)
//         {
//             D3DAGG_TRACE("CAggD3DSurface::ReleaseDC");
//             return S_OK;
//         }
//     };

#endif //GXD3D9_H
