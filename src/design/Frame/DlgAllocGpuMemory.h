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

#ifndef DLGALLOCGPUMEMORY_H
#define DLGALLOCGPUMEMORY_H

#include "tuires.h"
#include "tmtimer.h"

///////////////////////////////////////////////////////////////////////////////
// CD3DDev
class CD3DDev
{
public:
    static CD3DDev* CreateDevice(int nDeviceType, int nAdapter);
    virtual ~CD3DDev() {}
    virtual void Alloc(int n) = 0;
    virtual void Free(int n) = 0;
    virtual int GetSize() = 0;
    virtual void GetDeviceDesc(GXADAPTERDESC& desc) = 0;
};


///////////////////////////////////////////////////////////////////////////////
// CWndGpuAdapter
class CWndGpuAdapter
    : public TM::CTuiWidgetImpl<CWndGpuAdapter> //, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CWndGpuAdapter>
//     , public TM::CTuiMsgFilter
    , public TM::CTimerImpl<CWndGpuAdapter>
{
private:
    CD3DDev* m_device;

public:
    CWndGpuAdapter();
    ~CWndGpuAdapter();
    
public:
    enum { IDD = IDC_GROUPBOX_GPU_ADAPTER };
    
    BEGIN_TUIMSG_MAP (CWndGpuAdapter)
//     BEGIN_MSG_MAP (CWndGpuAdapter)
        MESSAGE_HANDLER     (WM_CREATE,                 OnInitDialog)
        MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT,     OnPrintClient)
//         COMMAND_ID_HANDLER  (IDOK,                      OnOk)
//         COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)


        COMMAND_HANDLER     (IDC_ALLOC_GM, BN_CLICKED,  OnAlloc)
    END_MSG_MAP()
    
    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
//     TMResult OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
//     TMResult OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnAlloc(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);

    // CTimerImpl
    void OnTimer(...);

//     void Alloc(int n);
//     void Free(int n);
    TMBool Create(TM::HTUI hParent, int nDeviceType, int nAdapter);
};

///////////////////////////////////////////////////////////////////////////////
// CDlgAllocGpuMemory
class CDlgAllocGpuMemory
    : public TM::CTuiWidgetImpl<CDlgAllocGpuMemory> //, TM::CTuiWindow>
//     : public TM::CTuiDialogImpl<CDlgAllocGpuMemory>
//     , public TM::CTuiMsgFilter
{
public:
    CDlgAllocGpuMemory();
    ~CDlgAllocGpuMemory();
    
public:
    enum { IDD = IDC_MANAGE_ALLOC_GPU_MEMORY };
    
    BEGIN_TUIMSG_MAP (CDlgAllocGpuMemory)
//     BEGIN_MSG_MAP (CDlgAllocGpuMemory)
        MESSAGE_HANDLER     (WM_INITDIALOG,             OnInitDialog)
//         MESSAGE_HANDLER     (WM_DESTROY,                OnDestroy)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT,     OnPrintClient)

        COMMAND_HANDLER     (IDC_DEV_D3D9,  BN_CLICKED, OnDev)
        COMMAND_HANDLER     (IDC_DEV_D3D11, BN_CLICKED, OnDev)
        COMMAND_HANDLER     (IDC_DEV_GX,    BN_CLICKED, OnDev)
        COMMAND_ID_HANDLER  (IDOK,                      OnOk)
        COMMAND_ID_HANDLER  (IDCANCEL,                  OnCancel)
    END_MSG_MAP()
    
    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnDev(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
    TMResult OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);

    //CTuiMsgFilter
//     virtual BOOL PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult);
};


#endif // DLGALLOCGPUMEMORY_H