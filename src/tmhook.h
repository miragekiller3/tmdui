/*
 *  @file
 *  @brief  hook for windows
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2010.11.11
 *
 *  Copyright (C) 2010 miragekiller
 */

#pragma once

#ifndef TMHOOK_H
#define TMHOOK_H

#include "tmdef.h"


#if TM_WIN_DESKTOP

#include "tmdll.h" // CHookShellUser32Impl


enum EHookType
{
    eHookTypeMSGFILTER,
    eHookTypeKEYBOARD,
    eHookTypeGETMESSAGE,
    eHookTypeCALLWNDPROC,
    eHookTypeCBT,
    eHookTypeSYSMSGFILTER,
    eHookTypeMOUSE,
    eHookTypeSHELL,
    eHookTypeFOREGROUNDIDLE,
    eHookTypeCALLWNDPROCRET,
    eHookTypeKEYBOARD_LL,
    eHookTypeMOUSE_LL,

    eHookTypeThreadKEYBOARD_LL,

    eHookType_End,
};


typedef TMBool (*PFNHOOKPROC)(TMLParam lData, int nCode, TMWParam wParam, TMLParam lParam);

TM_CAPI void HookInstall(EHookType type, PFNHOOKPROC proc, TMLParam lData);
TM_CAPI void HookRemove(EHookType type, PFNHOOKPROC proc, TMLParam lData);


TM_BGN_NAMESPACE

///////////////////////////////////////////////////////////////////////////////
// CHookImpl
TM_BGN_PRIVATE_NAMESPACE

template <EHookType eType, class T, class T_Proc> class CHookImpl
{
public:
    ~CHookImpl()
    {
        RemoveHook();
    }

    void SetHook()
    {
        T* pthis = static_cast<T*>(this);
        HookInstall(eType, T_Proc::_HookProc, (TMLParam)pthis);
    }
    
    void RemoveHook()
    {
        T* pthis = static_cast<T*>(this);
        HookRemove(eType, T_Proc::_HookProc, (TMLParam)pthis);
    }
};

#ifdef TM_DBG
template <class T> struct _HOOK_CAST
{
    T p;
    TMBool b;
    HWND hWnd;
    TMCharT cClass[256];
    
    operator T () const { return p; }
    
    _HOOK_CAST(LPARAM lp) : p ((T)lp)
    {
        hWnd = p->hwnd;
        b = ::IsWindow(p->hwnd);
        ::GetClassName(p->hwnd, cClass, 256);
    }
    
    ~_HOOK_CAST()
    {
        // don't destroy window on this hook;
        TMBool b2 = ::IsWindow(p->hwnd);
        DbgAssert(b == b2);
    }
};
#   define HOOK_CAST(x) PRI::_HOOK_CAST<x>
#else
#   define HOOK_CAST(x) (x)
#endif // TM_DBG


TM_END_PRIVATE_NAMESPACE


///////////////////////////////////////////////////////////////////////////////
// hook sample:
// class AAA : public CHookXXXImpl<AAA>
// {
//    void Start() { this->SetHook(); }
//    TMBool OnXXXProc(...) { ... }
// };


///////////////////////////////////////////////////////////////////////////////
// CHookCallWndImpl
// callback return true: stop hook chain;
// don't call DestroyWindow(self) on the hook, because self can't receive WM_DESTROY
template <class T>
class CHookCallWndImpl : public PRI::CHookImpl<eHookTypeCALLWNDPROC, T, CHookCallWndImpl<T> >
{
private:
    friend PRI::CHookImpl<eHookTypeCALLWNDPROC, T, CHookCallWndImpl<T> >;
    static TMBool _HookProc(TMLParam lData, int nCode, TMWParam wParam, TMLParam lParam)
    {
        T* pthis = reinterpret_cast<T*>(lData);
        return pthis->OnCallWndProc(HOOK_CAST(CWPSTRUCT*)(lParam));
    }

public:
    typedef CHookCallWndImpl CHookCallWnd;

//     TMBool OnCallWndProc(CWPSTRUCT* msg);
//     TMBool OnCallWndProc(...) { ... }
};


///////////////////////////////////////////////////////////////////////////////
// CHookCallWndRetImpl
// don't call DestroyWindow(self) on the hook, because self can't receive WM_DESTROY
// this hook can't capture mouse message
template <class T>
class CHookCallWndRetImpl : public PRI::CHookImpl<eHookTypeCALLWNDPROCRET, T, CHookCallWndRetImpl<T> >
{
private:
    friend PRI::CHookImpl<eHookTypeCALLWNDPROCRET, T, CHookCallWndRetImpl<T> >;
    static TMBool _HookProc(TMLParam lData, int nCode, TMWParam wParam, TMLParam lParam)
    {
        T* pthis = reinterpret_cast<T*>(lData);
        return pthis->OnCallWndRetProc(HOOK_CAST(CWPRETSTRUCT*)(lParam));
    }
    
public:
    typedef CHookCallWndRetImpl CHookCallWndRet;

//     TMBool OnCallWndRetProc(CWPRETSTRUCT* msg);
//     TMBool OnCallWndRetProc(...) { ... }
};


///////////////////////////////////////////////////////////////////////////////
// CHookCbtImpl
// don't call DestroyWindow(self) on the hook, because self can't receive WM_DESTROY
template <class T>
class CHookCbtImpl : public PRI::CHookImpl<eHookTypeCBT, T, CHookCbtImpl<T> >
{
private:
    friend PRI::CHookImpl<eHookTypeCBT, T, CHookCbtImpl<T> >;
    static TMBool _HookProc(TMLParam lData, int nCode, TMWParam wParam, TMLParam lParam)
    {
        T* pthis = reinterpret_cast<T*>(lData);
        return pthis->OnCbt(nCode, wParam, lParam);
    }
    
public:
    typedef CHookCbtImpl CHookCbt;

//     TMBool OnCbt(int nCode, WPARAM wParam, LPARAM lParam);
//     TMBool OnCbt(...) { ... }
};


///////////////////////////////////////////////////////////////////////////////
// CHookForegroundIdleImpl
template <class T>
class CHookForegroundIdleImpl : public PRI::CHookImpl<eHookTypeFOREGROUNDIDLE, T, CHookForegroundIdleImpl<T> >
{
private:
    friend PRI::CHookImpl<eHookTypeFOREGROUNDIDLE, T, CHookForegroundIdleImpl<T> >;
    static TMBool _HookProc(TMLParam lData, int nCode, TMWParam wParam, TMLParam lParam)
    {
        T* pthis = reinterpret_cast<T*>(lData);
        return pthis->OnForegroundIdle();
    }
    
public:
    typedef CHookForegroundIdleImpl CHookForegroundIdle;

//     TMBool OnForegroundIdle();
};


///////////////////////////////////////////////////////////////////////////////
// CHookGetMessageImpl
template <class T>
class CHookGetMessageImpl : public PRI::CHookImpl<eHookTypeGETMESSAGE, T, CHookGetMessageImpl<T> >
{
private:
    friend PRI::CHookImpl<eHookTypeGETMESSAGE, T, CHookGetMessageImpl<T> >;
    static TMBool _HookProc(TMLParam lData, int nCode, TMWParam wParam, TMLParam lParam)
    {
        T* pthis = reinterpret_cast<T*>(lData);
        return pthis->OnGetMessage((TMBool)wParam, (MSG*)lParam);
    }
    
public:
    typedef CHookGetMessageImpl CHookGetMessage;

//     TMBool OnGetMessage(TMBool bRemove, MSG* msg);
//     TMBool OnGetMessage(...) { ... }
};


///////////////////////////////////////////////////////////////////////////////
// CHookGetMessageExImpl
// only this hook can modify message
// return true to discard message
template <class T>
class CHookGetMessageExImpl : public PRI::CHookImpl<eHookTypeGETMESSAGE, T, CHookGetMessageExImpl<T> >
{
private:
    friend PRI::CHookImpl<eHookTypeGETMESSAGE, T, CHookGetMessageExImpl<T> >;
    static TMBool _HookProc(TMLParam lData, int nCode, TMWParam wParam, TMLParam lParam)
    {
        T* pthis = reinterpret_cast<T*>(lData);
        MSG* pMsg = (MSG*)lParam;
        TMBool b = pthis->PreTranslateMessage(pMsg);
        if (b)
            MemZeroT(*pMsg);
        return b;
    }
    
public:
    typedef CHookGetMessageExImpl CHookGetMessageEx;
    
//     TMBool PreTranslateMessage(MSG* msg);
//     TMBool PreTranslateMessage(...) { ... }
};


///////////////////////////////////////////////////////////////////////////////
// CHookKeyboardImpl
// return true to discard message
template <class T>
class CHookKeyboardImpl : public PRI::CHookImpl<eHookTypeKEYBOARD, T, CHookKeyboardImpl<T> >
{
private:
    friend PRI::CHookImpl<eHookTypeKEYBOARD, T, CHookKeyboardImpl<T> >;
    static TMBool _HookProc(TMLParam lData, int nCode, TMWParam wParam, TMLParam lParam)
    {
        T* pthis = reinterpret_cast<T*>(lData);
        return pthis->OnKeyboard(wParam, lParam);
    }
    
public:
    typedef CHookKeyboardImpl CHookKeyboard;

//     TMBool OnKeyboard(UINT uVirtKey, UINT uFlags);
//     TMBool OnKeyboard(...) { ... }
};


///////////////////////////////////////////////////////////////////////////////
// CHookKeyboardLLImpl
// only global
// return true to discard message
template <class T>
class CHookKeyboardLLImpl : public PRI::CHookImpl<eHookTypeKEYBOARD_LL, T, CHookKeyboardLLImpl<T> >
{
private:
    friend PRI::CHookImpl<eHookTypeKEYBOARD_LL, T, CHookKeyboardLLImpl<T> >;
    static TMBool _HookProc(TMLParam lData, int nCode, TMWParam wParam, TMLParam lParam)
    {
        T* pthis = reinterpret_cast<T*>(lData);
        return pthis->OnKeyboardLL(wParam, (KBDLLHOOKSTRUCT*)lParam);
    }
    
public:
    typedef CHookKeyboardLLImpl CHookKeyboardLL;

//     TMBool OnKeyboardLL(UINT uMsg, KBDLLHOOKSTRUCT* ps);
//     TMBool OnKeyboardLL(...) { ... }
};


///////////////////////////////////////////////////////////////////////////////
// CHookThreadKeyboardLLImpl
// only global
// return true to discard message
// the hook run on a thread 
template <class T>
class CHookThreadKeyboardLLImpl : public PRI::CHookImpl<eHookTypeThreadKEYBOARD_LL, T, CHookThreadKeyboardLLImpl<T> >
{
private:
    friend PRI::CHookImpl<eHookTypeThreadKEYBOARD_LL, T, CHookThreadKeyboardLLImpl<T> >;
    static TMBool _HookProc(TMLParam lData, int nCode, TMWParam wParam, TMLParam lParam)
    {
        T* pthis = reinterpret_cast<T*>(lData);
        return pthis->OnKeyboardLL(wParam, (KBDLLHOOKSTRUCT*)lParam);
    }
    
public:
    typedef CHookThreadKeyboardLLImpl CHookThreadKeyboardLL;
    
//     TMBool OnKeyboardLL(UINT uMsg, KBDLLHOOKSTRUCT* ps);
//     TMBool OnKeyboardLL(...) { ... }
};


///////////////////////////////////////////////////////////////////////////////
// CHookMouseImpl
// only hook current module mouse
// return true to discard process message
template <class T>
class CHookMouseImpl : public PRI::CHookImpl<eHookTypeMOUSE, T, CHookMouseImpl<T> >
{
private:
    friend PRI::CHookImpl<eHookTypeMOUSE, T, CHookMouseImpl<T> >;
    static TMBool _HookProc(TMLParam lData, int nCode, TMWParam wParam, TMLParam lParam)
    {
        T* pthis = reinterpret_cast<T*>(lData);
        return pthis->OnMouse(wParam, (MOUSEHOOKSTRUCT*)lParam);
    }
    
public:
    typedef CHookMouseImpl CHookMouse;
    
//     TMBool OnMouse(UINT uMsg, MOUSEHOOKSTRUCT* pm);
//     TMBool OnMouse(...) { ... }
};


///////////////////////////////////////////////////////////////////////////////
// CHookMouseLLImpl
// hook all system mouse, only global
// return true to discard process and system message
template <class T>
class CHookMouseLLImpl : public PRI::CHookImpl<eHookTypeMOUSE_LL, T, CHookMouseLLImpl<T> >
{
private:
    friend PRI::CHookImpl<eHookTypeMOUSE_LL, T, CHookMouseLLImpl<T> >;
    static TMBool _HookProc(TMLParam lData, int nCode, TMWParam wParam, TMLParam lParam)
    {
        T* pthis = reinterpret_cast<T*>(lData);
        return pthis->OnMouseLL(wParam, (MSLLHOOKSTRUCT*)lParam);
    }
    
public:
    typedef CHookMouseLLImpl CHookMouseLL;
    
//     TMBool OnMouseLL(UINT uMsg, MSLLHOOKSTRUCT* pm);
//     TMBool OnMouseLL(...) { ... }
};


///////////////////////////////////////////////////////////////////////////////
// CHookMsgFilterImpl
template <class T>
class CHookMsgFilterImpl : public PRI::CHookImpl<eHookTypeMSGFILTER, T, CHookMsgFilterImpl<T> >
{
private:
    friend PRI::CHookImpl<eHookTypeMSGFILTER, T, CHookMsgFilterImpl<T> >;
    static TMBool _HookProc(TMLParam lData, int nCode, TMWParam wParam, TMLParam lParam)
    {
        T* pthis = reinterpret_cast<T*>(lData);
        return pthis->OnMsgFilter(nCode, (MSG*)lParam);
    }
    
public:
    typedef CHookMsgFilterImpl CHookMsgFilter;
    
//     TMBool OnMsgFilter(int nEvent, MSG* msg);
//     TMBool OnMsgFilter(...) { ... }
};


///////////////////////////////////////////////////////////////////////////////
// CHookShellImpl
template <class T>
class CHookShellImpl : public PRI::CHookImpl<eHookTypeSHELL, T, CHookShellImpl<T> >
{
private:
    friend PRI::CHookImpl<eHookTypeSHELL, T, CHookShellImpl<T> >;
    static TMBool _HookProc(TMLParam lData, int nCode, TMWParam wParam, TMLParam lParam)
    {
        T* pthis = reinterpret_cast<T*>(lData);
        return pthis->OnShell(nCode, wParam, lParam);
    }
    
public:
    typedef CHookShellImpl CHookShell;
    
//     TMBool OnShell(int nCode, WPARAM wParam, LPARAM lParam);
//     TMBool OnShell(...) { ... }
};


///////////////////////////////////////////////////////////////////////////////
// CHookSysMsgFilterImpl
template <class T>
class CHookSysMsgFilterImpl : public PRI::CHookImpl<eHookTypeSYSMSGFILTER, T, CHookSysMsgFilterImpl<T> >
{
private:
    friend PRI::CHookImpl<eHookTypeSYSMSGFILTER, T, CHookSysMsgFilterImpl<T> >;
    static TMBool _HookProc(TMLParam lData, int nCode, TMWParam wParam, TMLParam lParam)
    {
        T* pthis = reinterpret_cast<T*>(lData);
        return pthis->OnSysMsgFilter(nCode, (MSG*)lParam);
    }
    
public:
    typedef CHookSysMsgFilterImpl CHookSysMsgFilter;
    
//     TMBool OnSysMsgFilter(int nEvent, MSG* msg);
//     TMBool OnSysMsgFilter(...) { ... }
};



///////////////////////////////////////////////////////////////////////////////
// CHookShellUser32Impl
// user must call     CHAIN_MSG_MAP(CHookShellUser32)
// HSHELL_GETMINRECT        A pointer to a SHELLHOOKINFO structure.
// HSHELL_WINDOWACTIVATED    A handle to the activated window.
// HSHELL_RUDEAPPACTIVATED    A handle to the activated window.
// HSHELL_WINDOWREPLACING    A handle to the window replacing the top-level window.
// HSHELL_WINDOWREPLACED    A handle to the window being replaced.
// HSHELL_WINDOWCREATED        A handle to the window being created.
// HSHELL_WINDOWDESTROYED    A handle to the top-level window being destroyed.
// HSHELL_ACTIVATESHELLWINDOW    Not used.
// HSHELL_TASKMAN            Can be ignored.
// HSHELL_REDRAW            A handle to the window that needs to be redrawn.
// HSHELL_FLASH                A handle to the window that needs to be flashed.
// HSHELL_ENDTASK            A handle to the window that should be forced to exit.
// HSHELL_APPCOMMAND        The APPCOMMAND which has been unhandled by the application or other hooks. See WM_APPCOMMAND and use the GET_APPCOMMAND_LPARAM macro to retrieve this parameter.
// HSHELL_MONITORCHANGED     A handle to the window that moved to a different monitor.
template <class T> class CHookShellUser32Impl
{
private:
    TMUInt32 WM_SHELLHOOK_INT;

public:
    typedef CHookShellUser32Impl CHookShellUser32;

    CHookShellUser32Impl() : WM_SHELLHOOK_INT(0) {}
    ~CHookShellUser32Impl() { RemoveHook(); }

    void SetHook()
    {
        DECLARE_DLL_FUNC(dll_user32, BOOL, WINAPI, RegisterShellHookWindow, (HWND hwnd));
        WM_SHELLHOOK_INT = RegisterWindowMessage(TM_T("SHELLHOOK"));
        _RegisterShellHookWindow((static_cast<T*>(this))->m_hWnd);
    }

    void RemoveHook()
    {
        DECLARE_DLL_FUNC(dll_user32, BOOL, WINAPI, DeregisterShellHookWindow, (HWND hwnd));
        _DeregisterShellHookWindow((static_cast<T*>(this))->m_hWnd);
    }

    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0)
    {
        if (uMsg == WM_SHELLHOOK_INT && WM_SHELLHOOK_INT)
        {
            (static_cast<T*>(this))->OnShellUser32(wParam, lParam);
        }
        return false;
    }

//     void OnShellUser32(int nCode, WPARAM wParam) {}
};




TM_END_NAMESPACE


#endif // TM_WIN_DESKTOP

#endif // TMHOOK_H
