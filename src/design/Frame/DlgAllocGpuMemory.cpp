/**
 *  @file
 *  @brief
 *  CDlgAllocGpuMemory
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2022-6-29
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2022 miragekiller                             
 *  All rights reserved  
 */

#include "stdafx.h"
#include "DlgAllocGpuMemory.h"
#include "tmgdix.h"
#include "tmdebugext.h"

using namespace TM;


///////////////////////////////////////////////////////////////////////////////
// CD3DDev

const int _w = 1024 / 4;
const int _h = 1024;

CComPtr<IGXApp> Edit_CreateD9Device(int nAdapter);
CComPtr<IGXApp> Edit_CreateD11Device(int nAdapter);


struct CD3DDevX : public CD3DDev
{
    CComPtr<IGXApp> m_device;
    TM::CVector<CComPtr<IGXTexture> > m_aTex;

    ~CD3DDevX()
    {
        DbgAssert(!m_aTex.size());
    }

    void Alloc(int n) tm_override
    {
//         CImage img;
//         img.CreateIndirect(_w, _h, 32);

        for (int i = 0; i < n; ++i)
        {
            CComPtr<IGXTexture> tex;
            m_device->TextureCreateRenderTarget(&tex, _w, _h);
            if (!tex)
                return;
            m_aTex.push_back(tex);

            //tex->ResetAsImage(img);
            m_device->Push(eGXPropRenderTarget);
            m_device->SetRenderTarget(tex);
            m_device->GrandientFill(CRect(0,0,_w,_h), TMRGB(255,122,0), TMRGB(0,133,254), e_gfCenter);
            m_device->Pop();
        }
    }
    
    void Free(int n) tm_override
    {
        tm_setmin(n, m_aTex.size());
        for (int i = 0; i < n; ++i)
            m_aTex.pop_back();
    }

    virtual int GetSize() tm_override
    {
        return m_aTex.size();
    }

    virtual void GetDeviceDesc(GXADAPTERDESC& desc) tm_override
    {
        m_device->GetDeviceType(&desc);
    }
};

struct CD3DDev9 : public CD3DDevX
{
    TM_SELF_ALLOC(CD3DDev9);

    CD3DDev9(int nAdapter)
    {
        m_device = Edit_CreateD9Device(nAdapter);
    }
};

struct CD3DDev11 : public CD3DDevX
{
    TM_SELF_ALLOC(CD3DDev11);
    
    CD3DDev11(int nAdapter)
    {
        m_device = Edit_CreateD11Device(nAdapter);
    }
};

struct CD3DDevGX : public CD3DDevX
{
    TM_SELF_ALLOC(CD3DDevGX);
    
    CD3DDevGX()
    {
        m_device = _gxApp;
    }
};


CD3DDev* CD3DDev::CreateDevice(int nDeviceType, int nAdapter)
{
    if (nDeviceType == IDC_DEV_D3D11)
    {
        CD3DDev11* p = new CD3DDev11(nAdapter);
        if (p->m_device)
            return p;
        delete p;
    }
    else if (nDeviceType == IDC_DEV_D3D9)
    {
        CD3DDev9* p2 = new CD3DDev9(nAdapter);
        if (p2->m_device)
            return p2;
        delete p2;
        return 0;
    }
    else if (nDeviceType == IDC_DEV_GX && nAdapter == 0)
    {
        return new CD3DDevGX();
    }

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
// CWndGpuAdapter
CWndGpuAdapter::CWndGpuAdapter()
{
}

CWndGpuAdapter::~CWndGpuAdapter()
{
}

TMResult CWndGpuAdapter::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
    SetTimer(1000);
    //     _tuiApp.AddMessageFilter(this);
    return 0;
}

TMResult CWndGpuAdapter::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
    m_device->Free(INT_MAX);
    delete m_device;
    //     _tuiApp.RemoveMessageFilter(this);
    return 0;
}

// TMResult CWndGpuAdapter::OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
// {
//     //    EndDialog(wID);
//     DestroyWindow();
//     return 0;
// }
// 
// TMResult CWndGpuAdapter::OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
// {
//     //    EndDialog(wID);
//     DestroyWindow();
//     return 0;
// }

TMResult CWndGpuAdapter::OnAlloc(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//     if (!m_device)
//         return 0;
    TM::CTuiWidget wnd((TM::HTUI)hWndCtl);
    int n = wnd.GetTag();
    if (n > 0)
        m_device->Alloc(n);
    else
        m_device->Free(-n);
    SetDlgItemText(IDC_EDIT_33094, MEM2T(m_device->GetSize() * 1024 * 1024));
    return 0;
}

//CTuiMsgFilter
// BOOL CWndGpuAdapter::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }

// CTimerProc
void CWndGpuAdapter::OnTimer(...)
{
    GXADAPTERDESC desc = { L"GDI" };
    m_device->GetDeviceDesc(desc);
    SetWindowText(desc.Description);
    SetDlgItemText(IDC_EDIT_31272, MEM2T(desc.DedicatedVideoMemory));
}


// void CWndGpuAdapter::Alloc(int n)
// {
//     for (int i = 0; i < n; ++i)
//     {
//         IDirect3DTexture9* tex = 0;
//         DbgVerifyHResult(m_device.m_pd3dDevice9->CreateTexture(1024 / 4, 1024 / 8, 1,
//             0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &tex, 0));
//         if (!tex)
//             return;
//         m_aTex.push_back(tex);
//     }
// }
// 
// void CWndGpuAdapter::Free(int n)
// {
//     tm_setmin(n, m_aTex.size());
//     for (int i = 0; i < n; ++i)
//     {
//         IDirect3DTexture9* tex = m_aTex.back();
//         tex->Release();
//         m_aTex.pop_back();
//     }
// }

TMBool CWndGpuAdapter::Create(HTUI hParent, int nDeviceType, int nAdapter)
{
    m_device = CD3DDev::CreateDevice(nDeviceType, nAdapter);
    if (!m_device)
        return false;
    root_class::Create(hParent);
    root_class::ShowWindow(SW_SHOW);
    return true;
}


///////////////////////////////////////////////////////////////////////////////
// CDlgAllocGpuMemory
CDlgAllocGpuMemory::CDlgAllocGpuMemory()
{
}

CDlgAllocGpuMemory::~CDlgAllocGpuMemory()
{
}

TMResult CDlgAllocGpuMemory::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{

//     tm_for (int i = 0; i < 16; ++i)
//     {
//         CWndGpuAdapter::CPtr ptr;
//         ptr.CreateInstance();
//         if (!ptr->Init(i))
//             break;
//         ptr->Create(m_hWnd);
//         ptr->ShowWindow(SW_SHOW);
//     }
    OnDev(0, IDC_DEV_D3D9, 0, bHandled);

//     _tuiApp.AddMessageFilter(this);
    return 0;
}

// TMResult CDlgAllocGpuMemory::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     bHandled = false;
// //     _tuiApp.RemoveMessageFilter(this);
//     return 0;
// }

TMResult CDlgAllocGpuMemory::OnDev(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    for (;;)
    {
        CTuiWidget wnd(Item(IDC_GROUPBOX_GPU_ADAPTER));
        if (!wnd)
            break;
        wnd.DestroyWindow();
    }

    CTuiWidget wndP(Item(IDC_WINDOW_31292));
    tm_for (int i = 0; i < 16; ++i)
    {
        CWndGpuAdapter::CPtr ptr;
        if (!ptr.CreateInstance()->Create(wndP, wID, i))
            break;
    }

    return 0;
}

TMResult CDlgAllocGpuMemory::OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgAllocGpuMemory::OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

//CTuiMsgFilter
// BOOL CDlgAllocGpuMemory::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }
