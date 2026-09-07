/*
 *  @file
 *  @brief  DirectX11 support
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021.8.3
 *
 *  Copyright (C) 2021 miragekiller
 */

#pragma once

#ifndef GXD3D11_H
#define GXD3D11_H

#include <d3d11.h>
#include <dxgi.h>
#include "tmthread.h"
#include "tmgdi.h"
#include "tmdll.h"


struct dll_d3d11    { DECLARE_DLL_LOAD("d3d11.dll", LOAD_LIBRARY_SEARCH_SYSTEM32); };


// #pragma comment(lib, "d3d11")
// #pragma comment(lib, "dcomp")

inline static DXGI_FORMAT dxGXFormat2DXGIFormat(EGXFormat nBpp)
{
    switch (nBpp)
    {
    case eGXFormatPAL2:
    case eGXFormatPAL4:
    case eGXFormatPAL8:
        return DXGI_FORMAT_UNKNOWN;
        
    case eGXFormat16:       return DXGI_FORMAT_B5G6R5_UNORM;
//     case eGXFormat24:       return D3DFMT_R8G8B8;
    case eGXFormat32A:      return DXGI_FORMAT_B8G8R8A8_UNORM;
    case eGXFormat32X:      return DXGI_FORMAT_B8G8R8X8_UNORM;

    case eGXFormatNV12:     return DXGI_FORMAT_NV12;
//    case eGXFormatNV21:     return DXGI_FORMAT_NV21;
//     case eGXFormatI420:     return DXGI_FORMAT_420_OPAQUE;
        
    default:
        return DXGI_FORMAT_UNKNOWN;
    }
}

inline static EGXFormat dxDXGIFormat2GXFormat(DXGI_FORMAT nBpp)
{
    switch (nBpp)
    {
    case DXGI_FORMAT_B5G6R5_UNORM: return eGXFormat16;
//     case D3DFMT_R8G8B8:   return eGXFormat24;
    case DXGI_FORMAT_B8G8R8A8_UNORM: return eGXFormat32A;
    case DXGI_FORMAT_B8G8R8X8_UNORM: return eGXFormat32X;
//     case D3DFMT_A8B8G8R8: return eGXFormatA8B8G8R8;
//     case D3DFMT_X8B8G8R8: return eGXFormatX8B8G8R8;
        
    default:
        return eGXFormatUnknow;
    }
}


#ifdef TM_DBG
inline void D3D11SetDebugObjectName(ID3D11DeviceChild* resource, const char* name)
{
    resource->SetPrivateData(WKPDID_D3DDebugObjectName, StrLen(name), name);
}
#else
#   define D3D11SetDebugObjectName(x,y)
#endif


struct CD3D11Device : public TM::CNoCopyable
{
public:
    typedef ID3D11InputLayout           T_VertexLayout;
    typedef ID3D11VertexShader          T_VertexShader;
    typedef ID3D11PixelShader           T_PixelShader;

    struct VS_DESC
    {
        const void*                     chCode;
        size_t                          szCode;
        CONST D3D11_INPUT_ELEMENT_DESC* decl;
        size_t                          szDecl;
    };

    struct PS_DESC
    {
        const void*                     chCode;
        size_t                          szCode;
    };

    struct _BufferImpl
    {
        struct Base : public TM::CNoCopyable
        {
            CComPtr<ID3D11Buffer> obj;

            HRESULT _Create(CD3D11Device* device, size_t sz, D3D11_BIND_FLAG BindFlags, D3D11_SUBRESOURCE_DATA* pInit = 0)
            {
                DbgAssert(!obj);
                HRESULT hr;
                DbgVerifyHResult(hr = device->_CreateCBuffer(sz, &obj, BindFlags, pInit));
                return hr;
            }

            void _Recreate(CD3D11Device* device, const void* pData, int nSize, int& nOldSize, D3D11_BIND_FLAG BindFlags)
            {
                if (nSize > nOldSize)
                {
                    obj.Release();
                    D3D11_SUBRESOURCE_DATA data = { pData };
                    _Create(device, nSize,BindFlags, &data);
                    nOldSize = nSize;
                }
                else
                    _SetData(device, pData, nSize);
            }

            void Release(CD3D11Device*) { obj.Release(); }
        
            HRESULT _SetData(CD3D11Device* device, const void* pConstantData, UINT nSize)
            {
                DbgAssert(obj);
                ID3D11DeviceContext* context = device->GetContext();
                return device->WriteCBuffer(context, obj, pConstantData, nSize);
            }
        };

        template <D3D11_BIND_FLAG BindFlags> struct BaseT : public Base
        {
            HRESULT Create(CD3D11Device* device, size_t sz) { return _Create(device, sz, BindFlags); }
            HRESULT SetDataVec4(CD3D11Device* device, const void* pData, UINT Vector4fCount) { return _SetData(device, pData, Vector4fCount * sizeof(float) * 4); }
        };

        template <D3D11_BIND_FLAG BindFlags> struct BaseRT : public Base
        {
            int nSize;
            BaseRT() : nSize(0) {}
            void Recreate(CD3D11Device* device, const void* pData, int ns) { this->_Recreate(device, pData, ns, nSize, BindFlags); }
        };
    };
    
    struct T_VSCBuffer : public _BufferImpl::BaseT<D3D11_BIND_CONSTANT_BUFFER>
    {
    public:
        void SetCurrent(CD3D11Device* device)
        {
            DbgAssert(obj);
            device->GetContext()->VSSetConstantBuffers(0, 1, &obj.p);
        }
    };
    
    struct T_PSCBuffer : public _BufferImpl::BaseT<D3D11_BIND_CONSTANT_BUFFER>
    {
    public:
        void SetCurrent(CD3D11Device* device)
        {
            DbgAssert(obj);
            device->GetContext()->PSSetConstantBuffers(0, 1, &obj.p);
        }
    };
    
    T_VSCBuffer* GetVSCBufffer() { return 0; }
    T_PSCBuffer* GetPSCBufffer() { return 0; }

    struct T_VS : public TM::CNoCopyable
    {
        ID3D11InputLayout*  ptrLayout;
        ID3D11VertexShader* ptrShader;

        T_VS() : ptrLayout(0), ptrShader(0) {}
        
        void Release(CD3D11Device*)
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
//             CD3D11Device* device,
//             CONST D3D11_INPUT_ELEMENT_DESC* pVertexElements, UINT NumElements,
//             const void* pShaderBytecode, size_t BytecodeLength)
//         {
//             return device->CreateVertexLayoutAndShader(pVertexElements, NumElements,
//                 pShaderBytecode, BytecodeLength, &ptrLayout, &ptrShader);
//         }

        HRESULT Create(CD3D11Device* device, VS_DESC desc)
        {
            DbgAssert(!ptrLayout);
            DbgAssert(!ptrShader);
            return device->CreateVertexLayoutAndShader(desc.decl, desc.szDecl,
                desc.chCode, desc.szCode, &ptrLayout, &ptrShader);
        }
        
        void SetCurrent(CD3D11Device* device)
        {
            DbgAssert(ptrLayout);
            DbgAssert(ptrShader);
            device->SetVertexLayout(ptrLayout);
            device->SetVertexShader(ptrShader);
        }
    };
    
    struct T_PS : public TM::CNoCopyable
    {
        ID3D11PixelShader*  ptrShader;
        
        T_PS() : ptrShader(0) {}

        void Release(CD3D11Device*) { if (ptrShader) { ptrShader->Release(); ptrShader = 0; } }
        
        operator bool () const { return 0 != ptrShader; }
        
        HRESULT Create(CD3D11Device* device, PS_DESC desc, const T_VS&, const VS_DESC&)
        {
            return Create(device, desc);
        }

        HRESULT Create(CD3D11Device* device, PS_DESC desc)
        {
            DbgAssert(!ptrShader);
            return device->CreatePixelShader(desc.chCode, desc.szCode, &ptrShader);
        }

        void SetCurrent(CD3D11Device* device)
        {
            DbgAssert(ptrShader);
            device->SetPixelShader(ptrShader);
        }
    };

    struct T_IBO : public _BufferImpl::BaseRT<D3D11_BIND_INDEX_BUFFER>  // Index Buffer Objects
    {
        void DrawUINT(CD3D11Device* device, D3D11_PRIMITIVE_TOPOLOGY Topology, UINT nCount, const UINT* p)
        {
            Recreate(device, p, sizeof(UINT) * nCount);
            ID3D11DeviceContext* context = device->GetContext();
            context->IASetIndexBuffer(obj, DXGI_FORMAT_R32_UINT, 0);
            context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            context->DrawIndexed(nCount, 0, 0);
        }
    };

    struct T_VBO : public _BufferImpl::BaseRT<D3D11_BIND_VERTEX_BUFFER> // Vertex Buffer Objects
    {
        void Init(CD3D11Device*, T_IBO*, CONST VS_DESC*) {}
        
        void SetCurrent(CD3D11Device* device, UINT stride, UINT offset = 0)
        {
            device->GetContext()->IASetVertexBuffers(0, 1, &obj.p, &stride, &offset);
        }

        void Draw(CD3D11Device* device, D3D11_PRIMITIVE_TOPOLOGY Topology, UINT nCount)
        {
            ID3D11DeviceContext* context = device->GetContext();
            context->IASetPrimitiveTopology(Topology);
            context->Draw(nCount, 0);
        }
    };

    
public:
    TM::CCritical                   m_critical;
    CComPtr<IDXGIFactory>           m_factory;
    CComPtr<ID3D11Device>           m_device;   // thread safe

private:
    CComPtr<ID3D11DeviceContext>    m_context;  // not thread safe
    friend T_VS;
    friend T_PS;
    friend _BufferImpl::Base;

private:
    HRESULT _CreateDevice(IDXGIAdapter* Adapter, UINT createDeviceFlags)
    {
        DECLARE_DLL_FUNC(dll_d3d11, HRESULT, WINAPI, D3D11CreateDevice,
            (IDXGIAdapter* pAdapter,
            D3D_DRIVER_TYPE DriverType,
            HMODULE Software,
            UINT Flags,
            CONST D3D_FEATURE_LEVEL* pFeatureLevels,
            UINT FeatureLevels,
            UINT SDKVersion,
            ID3D11Device** ppDevice,
            D3D_FEATURE_LEVEL* pFeatureLevel,
            ID3D11DeviceContext** ppImmediateContext));
        if (!_D3D11CreateDevice)
            return TYPE_E_CANTLOADLIBRARY;

        D3D_DRIVER_TYPE driverTypes[] =
        {
            D3D_DRIVER_TYPE_HARDWARE,
            D3D_DRIVER_TYPE_UNKNOWN,
//             D3D_DRIVER_TYPE_WARP,
//             D3D_DRIVER_TYPE_REFERENCE,
        };
        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_0, // shader model 5
            D3D_FEATURE_LEVEL_10_0, // shader model 4
//             D3D_FEATURE_LEVEL_9_1, // shader model 2
        };
//         D3D_FEATURE_LEVEL fl = D3D_FEATURE_LEVEL_9_1; //D3D_FEATURE_LEVEL_11_0; // D3D_FEATURE_LEVEL_9_1

        HRESULT hr;
        tm_for (int i = 0; i < tm_countof(driverTypes); ++i)
        {
            hr = _D3D11CreateDevice(
                Adapter,
                driverTypes[i],
                0,
                createDeviceFlags,
                featureLevels, tm_countof(featureLevels),
                D3D11_SDK_VERSION,
                &m_device, nullptr, &m_context);
            if (SUCCEEDED(hr))
            {
                CComQIPtr<IDXGIDevice> dxgiDev(m_device);
                CComPtr<IDXGIAdapter> adapter;
                dxgiDev->GetParent(__uuidof(IDXGIAdapter), (void**)&adapter);
                adapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_factory);
//                 DXGI_ADAPTER_DESC desc;
//                 adapter->GetDesc(&desc);
//                 if (desc.VendorId == nVendorID_Microsoft)
//                 {
//                     DbgTrace("CD3D11Device::CreateDevice() failed: not support Microsoft virtual Driver");
//                     return E_FAIL;
//                 }
                break;
            }
        }

        return hr;
    }

    
public:
    operator bool () const { return m_device != 0; }

    void LockDevice() { m_critical.Lock(); }
    void UnlockDevice() { m_critical.Unlock(); }

    ID3D11DeviceContext* GetContext() const
    {
        DbgAssert(m_context);
        DbgAssert(m_critical.IsLockThread());
        return m_context;
    }

    bool CreateDevice(IDXGIAdapter* Adapter = 0)
    {
        HRESULT hr;

#ifdef TM_DBG
        // D3D11CreateDevice will failed if not install "Graphics Tools"
        // on Settings panel->System->Apps & features->Manage optional Features
        DbgVerifyHResult(hr = _CreateDevice(Adapter, D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG));
        if (SUCCEEDED(hr))
            return true;
#endif

        DbgVerifyHResult(hr = _CreateDevice(Adapter, D3D11_CREATE_DEVICE_BGRA_SUPPORT));
        return SUCCEEDED(hr);
// 
//         D3D_DRIVER_TYPE driverTypes[] =
//         {
//             D3D_DRIVER_TYPE_HARDWARE,
//             D3D_DRIVER_TYPE_UNKNOWN,
// //             D3D_DRIVER_TYPE_WARP,
// //             D3D_DRIVER_TYPE_REFERENCE,
//         };
//         D3D_FEATURE_LEVEL featureLevels[] =
//         {
//             D3D_FEATURE_LEVEL_11_0, // shader model 5
//             D3D_FEATURE_LEVEL_10_0, // shader model 4
// //             D3D_FEATURE_LEVEL_9_1, // shader model 2
//         };
// //         D3D_FEATURE_LEVEL fl = D3D_FEATURE_LEVEL_9_1; //D3D_FEATURE_LEVEL_11_0; // D3D_FEATURE_LEVEL_9_1
//         UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
//         TM_IF_DBG(createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG);
//         tm_for (int i = 0; i < tm_countof(driverTypes); ++i)
//         {
//             HRESULT hr = _D3D11CreateDevice(
//                 Adapter,
//                 driverTypes[i],
//                 0,
//                 createDeviceFlags,
//                 featureLevels, tm_countof(featureLevels),
//                 D3D11_SDK_VERSION,
//                 &m_device, nullptr, &m_context);
//             if (SUCCEEDED(hr))
//             {
//                 CComQIPtr<IDXGIDevice> dxgiDev(m_device);
//                 CComPtr<IDXGIAdapter> adapter;
//                 dxgiDev->GetParent(__uuidof(IDXGIAdapter), (void**)&adapter);
//                 adapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_factory);
// #ifdef TM_DBG
//                 DXGI_ADAPTER_DESC desc;
//                 adapter->GetDesc(&desc);
//                 StrPrintf(g_chDbgAdapterIdentifier, tm_countof(g_chDbgAdapterIdentifier),
//                     TM_T("%X - %X - %s"), desc.VendorId, desc.DeviceId, TM_W2T(desc.Description));
// #endif
//                 return true;
//             }
//         }
// 
// #ifdef TM_DBG
//         // D3D11CreateDevice will failed if not install "Graphics Tools"
//         // on Settings panel->System->Apps & features->Manage optional Features
//         createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
//         tm_for (int i = 0; i < tm_countof(driverTypes); ++i)
//         {
//             HRESULT hr = _D3D11CreateDevice(
//                 Adapter,
//                 driverTypes[i],
//                 0,
//                 createDeviceFlags,
//                 featureLevels, tm_countof(featureLevels),
//                 D3D11_SDK_VERSION,
//                 &m_device, nullptr, &m_context);
//             if (SUCCEEDED(hr))
//             {
//                 CComQIPtr<IDXGIDevice> dxgiDev(m_device);
//                 CComPtr<IDXGIAdapter> adapter;
//                 dxgiDev->GetParent(__uuidof(IDXGIAdapter), (void**)&adapter);
//                 adapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_factory);
//                 return true;
//             }
//         }
// #endif
//         return false;
    }

    void GetDeviceDesc(GXADAPTERDESC& desc)
    {
        DbgAssertS(offsetof(GXADAPTERDESC, AdapterLuid) == offsetof(DXGI_ADAPTER_DESC, AdapterLuid));

        CComQIPtr<IDXGIDevice> dxgiDev(m_device);
        CComPtr<IDXGIAdapter> adapter;
        DbgVerifyHResult(dxgiDev->GetParent(__uuidof(IDXGIAdapter), (void**)&adapter));
        DbgVerifyHResult(adapter->GetDesc((DXGI_ADAPTER_DESC*)&desc));
        LARGE_INTEGER li;
        DbgVerifyHResult(adapter->CheckInterfaceSupport(__uuidof(ID3D10Device), &li));
        desc.DriverVersion = li.QuadPart;
    }

    void SetScissorRect(const TMRect& rc)
    {
        GetContext()->RSSetScissorRects(1, &rc);
    }
    
    void SetTextureAt(DWORD Stage, ID3D11ShaderResourceView* pTexture)
    {
        GetContext()->PSSetShaderResources(Stage, Stage + 1, &pTexture);
    }

    void SetTexture1(ID3D11ShaderResourceView* tex0)
    {
        SetTextureAt(0, tex0);
    }
    
    void SetTexture2(ID3D11ShaderResourceView* tex0, ID3D11ShaderResourceView* tex1)
    {
        ID3D11ShaderResourceView* tex[] = { tex0, tex1 };
        GetContext()->PSSetShaderResources(0, 2, tex);
    }
    
    void SetTexture3(ID3D11ShaderResourceView* tex0, ID3D11ShaderResourceView* tex1, ID3D11ShaderResourceView* tex2)
    {
        ID3D11ShaderResourceView* tex[] = { tex0, tex1, tex2 };
        GetContext()->PSSetShaderResources(0, 3, tex);
    }

    void SetRenderTarget(ID3D11RenderTargetView* pRenderTargetView, ID3D11DepthStencilView* pDepthStencilView = 0)
    {
        GetContext()->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);
    }

    HRESULT CreateVertexLayout(
        const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT NumElements,
        const void *pShaderBytecode, size_t BytecodeLength,
        ID3D11InputLayout** ppInputLayout)
    {
        DbgAssert(m_device);
        HRESULT hr;
        DbgVerifyHResult(m_device->CreateInputLayout(pInputElementDescs, NumElements, pShaderBytecode, BytecodeLength, ppInputLayout));
        return hr;
    }

    void SetVertexLayout(ID3D11InputLayout *pInputLayout)
    {
        DbgAssert(m_device);
        GetContext()->IASetInputLayout(pInputLayout);
    }

    HRESULT CreateVertexLayoutAndShader(
        const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT NumElements,
        const void *pShaderBytecode, size_t BytecodeLength,
        ID3D11InputLayout** ppInputLayout, ID3D11VertexShader **ppShader)
    {
        DbgAssert(m_device);
        HRESULT hr;
        DbgVerifyHResult(hr = m_device->CreateInputLayout(pInputElementDescs, NumElements, pShaderBytecode, BytecodeLength, ppInputLayout));
        DbgVerifyHResult(hr |= m_device->CreateVertexShader(pShaderBytecode, BytecodeLength, NULL, ppShader));
        return hr;
    }

    HRESULT CreateVertexShader(const void *pShaderBytecode, size_t BytecodeLength, ID3D11VertexShader **ppShader)
    {
        DbgAssert(m_device);
        HRESULT hr;
        DbgVerifyHResult(hr = m_device->CreateVertexShader(pShaderBytecode, BytecodeLength, NULL, ppShader));
        return hr;
    }

    void SetVertexShader(ID3D11VertexShader* pShader)
    {
        GetContext()->VSSetShader(pShader, NULL, 0);
    }
    
    HRESULT CreatePixelShader(const void *pShaderBytecode, size_t BytecodeLength, ID3D11PixelShader **ppShader)
    {
        DbgAssert(m_device);
        HRESULT hr;
        DbgVerifyHResult(hr = m_device->CreatePixelShader(pShaderBytecode, BytecodeLength, NULL, ppShader));
        return hr;
    }

    void SetPixelShader(ID3D11PixelShader* pShader)
    {
        GetContext()->PSSetShader(pShader, NULL, 0);
    }

private:
    HRESULT _CreateCBuffer(size_t sz, ID3D11Buffer **ppBuffer, D3D11_BIND_FLAG BindFlags, D3D11_SUBRESOURCE_DATA* pInit = 0)
    {
        DbgAssert(m_device);
        D3D11_BUFFER_DESC cbd = {0};
        cbd.Usage = D3D11_USAGE_DYNAMIC;
        cbd.ByteWidth = sz;
        cbd.BindFlags = BindFlags;
        cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        return m_device->CreateBuffer(&cbd, pInit, ppBuffer);
    }

    // more adapter like intel may be throw 0xC0000005
    static HRESULT Map11(ID3D11DeviceContext* context, ID3D11Resource* pResource, D3D11_MAP MapType, D3D11_MAPPED_SUBRESOURCE* resource)
    {
        TM_TRY_ALL(return context->Map(pResource, 0, MapType, 0, resource));
        return E_UNEXPECTED;
    }

    static HRESULT WriteCBuffer(ID3D11DeviceContext* context, ID3D11Buffer* pConstant, const void* pConstantData, size_t sz)
    {
        HRESULT hr;
        D3D11_MAPPED_SUBRESOURCE mappedData;
        DbgVerifyHResult(hr = Map11(context, pConstant, D3D11_MAP_WRITE_DISCARD, &mappedData));
        if (SUCCEEDED(hr))
        {
            memcpy(mappedData.pData, pConstantData, sz);
            context->Unmap(pConstant, 0);
        }
        return hr;
    }

public:
    HRESULT InitVSConstantBuffer(size_t sz)
    {
        HRESULT hr;
        CComPtr<ID3D11Buffer> pConstant;
        DbgVerifyHResult(hr = _CreateCBuffer(sz, &pConstant, D3D11_BIND_CONSTANT_BUFFER));
        GetContext()->VSSetConstantBuffers(0, 1, &pConstant.p);
        return hr;
    }

    HRESULT InitPSConstantBuffer(size_t sz)
    {
        HRESULT hr;
        CComPtr<ID3D11Buffer> pConstant;
        DbgVerifyHResult(hr = _CreateCBuffer(sz, &pConstant, D3D11_BIND_CONSTANT_BUFFER));
        GetContext()->PSSetConstantBuffers(0, 1, &pConstant.p);
        return hr;
    }

    HRESULT SetVSConstantF(const void* pConstantData, UINT Vector4fCount)
    {
        ID3D11DeviceContext* context = GetContext();
        CComPtr<ID3D11Buffer> pConstant;
        context->VSGetConstantBuffers(0, 1, &pConstant.p);
        DbgAssert(pConstant);
        return WriteCBuffer(context, pConstant, pConstantData, Vector4fCount * (4 * sizeof(float)));
    }

    HRESULT SetPSConstantF(UINT StartRegister, const void* pConstantData, UINT Vector4fCount)
    {
        ID3D11DeviceContext* context = GetContext();
        CComPtr<ID3D11Buffer> pConstant;
        context->PSGetConstantBuffers(0, 1, &pConstant.p);
        DbgAssert(pConstant);
        return WriteCBuffer(context, pConstant, pConstantData, Vector4fCount * (4 * sizeof(float)));
    }
};


// struct CDComposition
// {
// //     typedef IDCompositionDesktopDevice  device_type;
// //     typedef IDCompositionVisual2        visual_type;
//     typedef IDCompositionDevice         device_type;
//     typedef IDCompositionVisual         visual_type;
// 
//     CComPtr<device_type>                m_deviceDComp;
//     CComPtr<visual_type>                m_visualDComp;
//     CComPtr<IDCompositionTarget>        m_targetDComp;
// 
// public:
//     bool CreateDComp(CD3D11Device& device, HWND hWnd)
//     {
//         DECLARE_DLL_FUNC("dcomp.dll", HRESULT, WINAPI, DCompositionCreateDevice,
//             (IDXGIDevice *dxgiDevice, REFIID iid, void **dcompositionDevice));
//         CDllFunc_DCompositionCreateDevice::PFN pfn = CDllFunc_DCompositionCreateDevice::LoadDirSystem32();
//         if (!pfn)
//             return false;
//         
//         CComQIPtr<IDXGIDevice> dxgiDev(device.m_device);
//         if (FAILED(pfn(dxgiDev, __uuidof(device_type), (void**)&m_deviceDComp)))
//             return false;
//         
//         if (FAILED(m_deviceDComp->CreateTargetForHwnd(hWnd, true, &m_targetDComp)))
//             return false;
//         
//         if (FAILED(m_deviceDComp->CreateVisual(&m_visualDComp)))
//             return false;
// //         m_visualDComp->SetCompositeMode(DCOMPOSITION_COMPOSITE_MODE_DESTINATION_INVERT);
// 
// //         CComQIPtr<IDCompositionVisual2> ptr(m_visualDComp);
// //         ptr->SetOpacityMode(DCOMPOSITION_OPACITY_MODE_LAYER);
// //        m_visualDComp->SetBackFaceVisibility(DCOMPOSITION_BACKFACE_VISIBILITY_HIDDEN);
//         
//         if (FAILED(m_targetDComp->SetRoot(m_visualDComp)))
//             return false;
// 
// //         CComPtr<IDCompositionCompositeEffect> eff;
// //         CComQIPtr<IDCompositionDevice3> device3(m_deviceDComp);
// //         device3->CreateCompositeEffect(&eff);
// //         eff->SetMode(D2D1_COMPOSITE_MODE_SOURCE_COPY);
//         //m_visualDComp->SetEffect(eff);
// 
// 
//         return true;
//     }
// 
//     HRESULT Flip(TMHWnd hWnd, ID3D11DeviceContext* context, ID3D11Resource* texSrc, const CD3D11_TEXTURE2D_DESC& desc)
//     {
// //         CComPtr<IUnknown> sfcLayer;
// //         m_deviceDComp->CreateSurfaceFromHwnd(hWnd, &sfcLayer);
// //         CComQIPtr<IDXGISurface> sf(sfcLayer);
// //         CComQIPtr<IDCompositionSurface> sf2(sfcLayer);
// 
//         CComPtr<IDCompositionSurface> surface;
//         m_deviceDComp->CreateSurface(desc.Width, desc.Height, desc.Format, DXGI_ALPHA_MODE_PREMULTIPLIED, &surface);
// //        m_deviceDComp->CreateSurface(desc.Width, desc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ALPHA_MODE_IGNORE, &surface);
// 
//         CComPtr<IDXGISurface> dxgiSurface;
//         POINT offset;
//         RECT rect = { 0, 0, desc.Width, desc.Height };
//         HRESULT hr = surface->BeginDraw(&rect, __uuidof(IDXGISurface), (void**)&dxgiSurface, &offset);
// //         DbgTrace("CDComposition::Flip(%d, %d)", offset);
//         if (SUCCEEDED(hr))
//         {
//             CComQIPtr<ID3D11Resource> dxgiRes(dxgiSurface);
//             D3D11_BOX db = { 0, 0, 0, desc.Width, desc.Height, 1 };
//             context->CopySubresourceRegion(dxgiRes, 0, offset.x, offset.y, 0, texSrc, 0, &db);
//             surface->EndDraw();
// 
//             hr = m_visualDComp->SetContent(surface);
//             if (SUCCEEDED(hr))
//                 hr = m_deviceDComp->Commit();
//         }
//         return hr;
//     }
// };

struct CSwapChain
{
    CComPtr<IDXGISwapChain>             m_chain;

    template <class T>
    HRESULT CreateChain(T* device, HWND hWnd, int w, int h, DXGI_FORMAT fmt)
    {
        DXGI_SWAP_CHAIN_DESC swapDesc = {0};
        swapDesc.SampleDesc.Count = 1;        //The Number of Multisamples per Level
        swapDesc.SampleDesc.Quality = 0;      //between 0(lowest Quality) and one lesser than pDevice->CheckMultisampleQualityLevels
        swapDesc.BufferDesc.Width = w;
        swapDesc.BufferDesc.Height = h;
        swapDesc.BufferDesc.Format = fmt;
        swapDesc.BufferDesc.RefreshRate.Numerator = 60;
        swapDesc.BufferDesc.RefreshRate.Denominator = 1;
        swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapDesc.BufferCount = 1;
        swapDesc.OutputWindow = hWnd;
        swapDesc.Windowed = TRUE;
        swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE;
        swapDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
        HRESULT hr = device->m_factory->CreateSwapChain(device->m_device, &swapDesc, &m_chain);
        return hr;
    }

//     void Resize(int w, int h)
//     {
//         DbgVerifyHResult(m_chain->ResizeBuffers(1, w, h, DXGI_FORMAT_B8G8R8A8_UNORM, 0));
//     }
};



#endif //GXD3D11_H
