/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2025.3.27
 *
 *  Copyright (C) 2025 miragekiller
 */

#pragma once

#ifndef TMMINHOOK_H
#define TMMINHOOK_H

#include "tmdll.h"
#include "tmatl.h"


struct CMinHook
{
public:
    typedef HRESULT MH_STATUS;
    enum { MH_OK = 0 };

#ifdef TM_WIN64
#   define MINIHOOK_DLL "minHook64.dll"
#else
#   define MINIHOOK_DLL "minHook32.dll"
#endif

//     struct dll_minhook   { DECLARE_DLL_LOAD(MINIHOOK_DLL, 0); };
    struct dll_minhook   { DECLARE_DLL_LOAD_FROM(MINIHOOK_DLL, _Module.m_hInst); };

private:
    CMinHook()
    {
        DECLARE_DLL_FUNC(dll_minhook, MH_STATUS, WINAPI, MH_Initialize, ());
        _MH_Initialize();
    }

public:
    ~CMinHook()
    {
        DECLARE_DLL_FUNC(dll_minhook, MH_STATUS, WINAPI, MH_Uninitialize, ());
        _MH_Uninitialize();
    }

    static CMinHook* Inst()
    {
        if (!dll_minhook::Load())
            return 0;
        static CMinHook s_mh;
        return &s_mh;
    }

//     #define PAGE_EXECUTE_FLAGS (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)
//     BOOL IsExecutableAddress(LPVOID pAddress)
//     {
//         MEMORY_BASIC_INFORMATION mi;
//         NtVirtualQuery(pAddress, &mi, sizeof(mi));
//         
//         return (mi.State == MEM_COMMIT && (mi.Protect & PAGE_EXECUTE_FLAGS));
//     }

    typedef struct __HFunc* HFunc;
    HFunc AddFunc(LPVOID pTarget, LPVOID pMyFunc, void** pOldFunc)
    {
        DECLARE_DLL_FUNC(dll_minhook, MH_STATUS, WINAPI, MH_CreateHook, (LPVOID pTarget, LPVOID pDetour, LPVOID *ppOriginal));
        DECLARE_DLL_FUNC(dll_minhook, MH_STATUS, WINAPI, MH_EnableHook, (LPVOID pTarget));
        
        DbgAssert(!*pOldFunc);
        HRESULT hr = _MH_CreateHook(pTarget, pMyFunc, pOldFunc);
        if (MH_OK != hr)
            return 0;
        
        if (MH_OK == _MH_EnableHook(pTarget))
            return (HFunc)pTarget;
        
        RemoveFunc((HFunc)pTarget);
        return 0;
    }

    HFunc AddFunc(HMODULE hModule, LPCSTR pFuncName, FARPROC pMyFunc, void** pOldFunc)
    {
        void* pfn = ::GetProcAddress(hModule, pFuncName);
        if (!pfn)
            return 0;

        DbgAssert(!*pOldFunc);
        return AddFunc(pfn, pMyFunc, pOldFunc);
    }

    void RemoveFunc(LPVOID hf)
    {
        DECLARE_DLL_FUNC(dll_minhook, MH_STATUS, WINAPI, MH_RemoveHook, (LPVOID pTarget));
        DECLARE_DLL_FUNC(dll_minhook, MH_STATUS, WINAPI, MH_DisableHook, (LPVOID pTarget));

        _MH_DisableHook(hf);
        _MH_RemoveHook(hf);
    }

};



///////////////////////////////////////////////////////////////////////////////
// DECLARE_MINHOOK_FUNC_S
// sample:
// DECLARE_MINHOOK_FUNC_S(void, DFTCALL, postEvent, &QCoreApplication::postEvent, (QObject *receiver, QEvent *event, int priority))
//  {
//     _postEvent_Org(receiver, event, priority);
//  }
//  _postEvent_Install();
//  _postEvent_Uninstall();
#define DECLARE_MINHOOK_FUNC_S(ReturnType,CallMode,FuncName,FuncTarget,ParamList)   \
    typedef ReturnType (CallMode * PFN##FuncName) ParamList;                        \
    TM_IF_DBG(static PFN##FuncName _##FuncName##_Target);                           \
    static PFN##FuncName _##FuncName##_Org;                                         \
    ReturnType CallMode _##FuncName##_My ParamList;                                 \
    bool _##FuncName##_IsInstall() { return 0 != _##FuncName##_Org; }               \
    bool _##FuncName##_Uninstall()                                                  \
    {   \
        if (!_##FuncName##_IsInstall()) \
            return false; \
        PFN##FuncName _##FuncName##_Target = FuncTarget;    \
        DbgAssert(_##FuncName##_Target);    \
        CMinHook::Inst()->RemoveFunc(_##FuncName##_Target);   \
        _##FuncName##_Org = 0; \
        return true; \
    }   \
    void* _##FuncName##_Install()                         \
    {   \
        TM_NO_DBG(PFN##FuncName) _##FuncName##_Target = FuncTarget;    \
        DbgAssert(_##FuncName##_Target);    \
        return CMinHook::Inst()->AddFunc(_##FuncName##_Target, _##FuncName##_My, (void**)&_##FuncName##_Org);   \
    }   \
    ReturnType CallMode _##FuncName##_My ParamList


///////////////////////////////////////////////////////////////////////////////
// DECLARE_MINHOOK_FUNC
// sample:
//  DECLARE_MINHOOK_FUNC(dll_user32, HDC, WINAPI, GetDC, (HWND hWnd))
//  {
//     Trace("hooked GetDC calling");  return _GetDC_Org(hWnd);
//  }
//  _GetDC_Install();
//  _GetDC_Uninstall();
#define DECLARE_MINHOOK_FUNC2(DllClassName,ReturnType,CallMode,FuncName,FuncStr,ParamList) \
    DECLARE_MINHOOK_FUNC_S(ReturnType, CallMode, FuncName, ((PFN##FuncName)_DllSymSafe(DllClassName::Load(), #FuncStr)), ParamList)

#define DECLARE_MINHOOK_FUNC(DllClassName,ReturnType,CallMode,FuncName,ParamList) \
    DECLARE_MINHOOK_FUNC2(DllClassName,ReturnType,CallMode,FuncName,FuncName,ParamList)


#endif // TMMINHOOK_H
