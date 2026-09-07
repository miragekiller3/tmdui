/**
 *  @file
 *  @brief
 *  CDlgBlockInject
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2023-8-4
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2023 miragekiller                             
 *  All rights reserved  
 */

#include "stdafx.h"
#include "DlgBlockInject.h"
#include "tmminhook.h"

using namespace TM;


// hook
typedef HMODULE(WINAPI *PFNLoadLibraryExW)(LPCWSTR  lpLibFileName, HANDLE hFile, DWORD dwFlags);
typedef HMODULE(WINAPI *PFNLoadLibraryExA)(LPCSTR  lpLibFileName, HANDLE hFile, DWORD dwFlags);
typedef HMODULE(WINAPI* PFNLoadLibraryW)(LPCWSTR  lpLibFileName);
typedef HMODULE(WINAPI* PFNLoadLibraryA)(LPCSTR  lpLibFileName);


CString s_strFilter;
static PFNLoadLibraryExW s_pfnLoadLibraryExW;
static PFNLoadLibraryExA s_pfnLoadLibraryExA;
static PFNLoadLibraryW s_pfnLoadLibraryW;
static PFNLoadLibraryA s_pfnLoadLibraryA;
CMinHook::HFunc s_hLoadLibraryExW;
CMinHook::HFunc s_hLoadLibraryExA;
CMinHook::HFunc s_hLoadLibraryW;
CMinHook::HFunc s_hLoadLibraryA;

static bool _IsBlock(CString strFile)
{
    CStrView strLine;
    for (size_t n = 0; n = s_strFilter.SplitLine(n, strLine);)
    {
        strLine = strLine.Trim();
        if (strFile.FindI(strLine) >= 0)
            return true;
    }
    return false;
}

static HMODULE WINAPI MyLoadLibraryW(LPCWSTR lpLibFileName)
{
    if (_IsBlock(lpLibFileName))
            return 0;
    return s_pfnLoadLibraryW(lpLibFileName);
}

static HMODULE WINAPI MyLoadLibraryA(LPCSTR lpLibFileName)
{
    if (_IsBlock(lpLibFileName))
        return 0;
    return s_pfnLoadLibraryA(lpLibFileName);
}

static HMODULE WINAPI MyLoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
    if (_IsBlock(lpLibFileName))
        return 0;
    
    return s_pfnLoadLibraryExW(lpLibFileName, hFile, dwFlags);
}

static HMODULE WINAPI MyLoadLibraryExA(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
    if (_IsBlock(lpLibFileName))
        return 0;
    
    return s_pfnLoadLibraryExA(lpLibFileName, hFile, dwFlags);
}


// CDlgBlockInject
CDlgBlockInject::CDlgBlockInject()
{
}

CDlgBlockInject::~CDlgBlockInject()
{
}

TMResult CDlgBlockInject::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    Item(IDC_MEMO_1001).SetWindowText(s_strFilter);

//     _tuiApp.AddMessageFilter(this);
    return 0;
}

TMResult CDlgBlockInject::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
//     _tuiApp.RemoveMessageFilter(this);
    return 0;
}



TMResult CDlgBlockInject::OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    if (!s_hLoadLibraryExW)
    {
        CMinHook* hook = CMinHook::Inst();
        
        if (hook)
        {
            s_hLoadLibraryExW = hook->AddFunc(
                dll_kernel32::Load(), "LoadLibraryExW", (FARPROC)MyLoadLibraryExW, (void**)&s_pfnLoadLibraryExW);
            s_hLoadLibraryExA = hook->AddFunc(
                dll_kernel32::Load(), "LoadLibraryExA", (FARPROC)MyLoadLibraryExA, (void**)&s_pfnLoadLibraryExA);
            s_hLoadLibraryW = hook->AddFunc(
                dll_kernel32::Load(), "LoadLibraryW", (FARPROC)MyLoadLibraryW, (void**)&s_pfnLoadLibraryW);
            s_hLoadLibraryA = hook->AddFunc(
                dll_kernel32::Load(), "LoadLibraryA", (FARPROC)MyLoadLibraryA, (void**)&s_pfnLoadLibraryA);
        }
        
        if (!s_hLoadLibraryExW || !s_hLoadLibraryW || !s_hLoadLibraryExA || !s_hLoadLibraryA)
            MessageBox(GetHWND(), L"Create Hook Error", L"error", MB_OK);
    }
    
    Item(IDC_MEMO_1001).GetWindowText(s_strFilter);

    DestroyWindow();
    return 0;
}

TMResult CDlgBlockInject::OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    DestroyWindow();
    return 0;
}




//CTuiMsgFilter
// BOOL CDlgBlockInject::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }

