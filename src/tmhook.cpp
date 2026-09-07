/*
 *  @file
 *  @brief  hook for windows
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2010.11.11
 *
 *  Copyright (C) 2010 miragekiller
 */

#include "tmhook.h"
#include "tmlist.h"
#include "tmcallset.h"
#include "tmmessagequeue.h"


//#define HOOK_TRACE DbgTrace
#define HOOK_TRACE DbgTraceNull


TM_BGN_NAMESPACE


struct CHooks
{
    // _CALLBACK
    struct _CALLBACK
    {
    public:
        typedef _CALLBACK value_type;
        
        PFNHOOKPROC proc;
        TMLParam    data;
        
        _CALLBACK(PFNHOOKPROC p, TMLParam d) : proc(p), data(d) {}
        _CALLBACK() {}
        
        bool operator == (const _CALLBACK& rhs) const { return rhs.proc == proc && rhs.data == data; }
        
        TMUInt8 IsDeleted() const { return !this->proc; }
        void SetDeleted() { this->proc = 0; }
    };
    typedef CCallSetT<_CALLBACK>        CCallSet;
    typedef std::pair<int, HOOKPROC>    CHookParam;

    // SETHOOK, for WH_SETHOOK
    struct SETHOOK
    {
        const _CALLBACK* callback;
        int idHook;
        HOOKPROC proc;
    };
    enum { WH_SETHOOK       = WM_USER + 100 }; // lp: SETHOOK*
    enum { WH_REMOVEHOOK    = WM_USER + 101 }; // lp: _CALLBACK*

    // CHook
    struct TM_NO_VTABLE CHook
        : public CMessageThreadT<CHook>
    {
    public:
        friend CMessageThreadT<CHook>;

        // member
        CCallSet            m_aProc;
        HHOOK               m_hHook;

        ~CHook()
        {
            //         TraceDebugInfo(typeid(this).name());
        }
    
        LRESULT CallChain(TMBool bProcess, int nCode, WPARAM wParam, LPARAM lParam)
        {
            if (bProcess)
            {
                CALLSET_BEGIN_CALL_EX(CCallSet, m_aProc, 0);
                if (p.proc(p.data, nCode, wParam, lParam))
                {
                    HOOK_TRACE("%s::CallChain(%p, %d, %X, %X) stop call chain", typeid(this).name(), p.proc, nCode, wParam, lParam);
                    return 1;
                }
                CALLSET_END_CALL_EX();
            }
            return CallNextHookEx(m_hHook, nCode, wParam, lParam);
        }

    protected:
        void _SetHook(const _CALLBACK& pfn, int idHook, HOOKPROC proc, HINSTANCE hMod = 0, DWORD dwThreadId = GetCurrentThreadId())
        {
            m_aProc.SafeAppend(pfn);
            if (!m_hHook)
                m_hHook = SetWindowsHookEx(idHook, proc, hMod, dwThreadId);
        }
    
        void _RemoveHook(const _CALLBACK& pfn)
        {
            m_aProc.Erase(pfn);
            if (m_hHook && !m_aProc.GetSize())
            {
                UnhookWindowsHookEx(m_hHook);
                m_hHook = 0;
            }
        }

        TMResult WndProc(TMUInt uMsg, TMWParam wParam, TMLParam lParam)
        {
            if (uMsg == WH_SETHOOK)
            {
                SETHOOK* p = (SETHOOK*)lParam;
                this->_SetHook(*p->callback, p->idHook, p->proc, _ModuleATL.GetModuleInstance(), 0);
                return 1;
            }
            else if (uMsg == WH_REMOVEHOOK)
            {
                this->_RemoveHook(*(_CALLBACK*)lParam);
                return 0 != m_hHook;
            }
        
            return 0;
        }
    
        void _SetThreadHook(const _CALLBACK& p, int idHook, HOOKPROC proc)
        {
            if (!IsValid())
                this->Create();
            SETHOOK sh = { &p, idHook, proc };
            SendMessage(WH_SETHOOK, 0, (LPARAM)&sh);
        }
    
        void _RemoveThreadHook(const _CALLBACK& p)
        {
            if (!IsValid())
                return;
            TMBool b = SendMessage(WH_REMOVEHOOK, 0, (LPARAM)&p);
            if (b)
                return;
            this->Exit();
            this->Join();
        }

        virtual void SetHook(const _CALLBACK& cb) = 0;
        virtual void RemoveHook(const _CALLBACK& cb) { _RemoveHook(cb); }

    public:
        inline void SetHook(PFNHOOKPROC proc, TMLParam lData) { SetHook(_CALLBACK(proc, lData)); }
        inline void RemoveHook(PFNHOOKPROC proc, TMLParam lData) { _RemoveHook(_CALLBACK(proc, lData)); }
    };

    // CheckAction
    struct TCheckAction
    {
        inline static TMBool exec(int nCode) { return nCode == HC_ACTION; }
    };
    
    struct TCheckThan0
    {
        inline static TMBool exec(int nCode) { return nCode >= 0; }
    };

    // CHookF
    template <EHookType type, int idHook, class T_CheckAction> struct CHookF
    {
        static LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
        {
            TMBool bProcess = T_CheckAction::exec(nCode);
            return CHooks::s_hooks[type].CallChain(bProcess, nCode, wParam, lParam);
        }
    };

    // CHookNormal
    template <EHookType type, int idHook, class T_CheckAction>
    struct CHookNormal : public CHook
    {
        virtual void SetHook(const _CALLBACK& cb) tm_final
        {
            DbgAssert(this == &s_hooks[type]);
            this->_SetHook(cb, idHook, CHookF<type, idHook, T_CheckAction>::HookProc, 0, GetCurrentThreadId());
        }
    };
    
    // CHookGlobal
    template <EHookType type, int idHook, class T_CheckAction>
    struct CHookGlobal : public CHook
    {
        virtual void SetHook(const _CALLBACK& cb) tm_final
        {
            DbgAssert(this == &s_hooks[type]);
            this->_SetHook(cb, idHook, CHookF<type, idHook, T_CheckAction>::HookProc, _ModuleATL.GetModuleInstance(), 0);
        }
    };
    
    // CHookThread
    template <EHookType type, int idHook, class T_CheckAction>
    struct CHookThread : public CHook
    {
        virtual void SetHook(const _CALLBACK& cb) tm_final
        {
            DbgAssert(this == &s_hooks[type]);
            this->_SetThreadHook(cb, idHook, CHookF<type, idHook, T_CheckAction>::HookProc);
        }
        
        virtual void RemoveHook(const _CALLBACK& cb) tm_final
        {
            this->_RemoveThreadHook(cb);
        }
    };

    // members
    CHookNormal<eHookTypeMSGFILTER,         WH_MSGFILTER,       TCheckThan0 > m_hook_MSGFILTER;
    CHookNormal<eHookTypeKEYBOARD,          WH_KEYBOARD,        TCheckThan0 > m_hook_KEYBOARD;
    CHookNormal<eHookTypeGETMESSAGE,        WH_GETMESSAGE,      TCheckAction> m_hook_GETMESSAGE;
    CHookNormal<eHookTypeCALLWNDPROC,       WH_CALLWNDPROC,     TCheckAction> m_hook_CALLWNDPROC;
    CHookNormal<eHookTypeCBT,               WH_CBT,             TCheckThan0 > m_hook_CBT;
    CHookNormal<eHookTypeSYSMSGFILTER,      WH_SYSMSGFILTER,    TCheckThan0 > m_hook_SYSMSGFILTER;
    CHookNormal<eHookTypeMOUSE,             WH_MOUSE,           TCheckThan0 > m_hook_MOUSE;
    CHookNormal<eHookTypeSHELL,             WH_SHELL,           TCheckThan0 > m_hook_SHELL;
    CHookNormal<eHookTypeFOREGROUNDIDLE,    WH_FOREGROUNDIDLE,  TCheckAction> m_hook_FOREGROUNDIDLE;
    CHookNormal<eHookTypeCALLWNDPROCRET,    WH_CALLWNDPROCRET,  TCheckAction> m_hook_CALLWNDPROCRET;
    CHookGlobal<eHookTypeKEYBOARD_LL,       WH_KEYBOARD_LL,     TCheckThan0 > m_hook_KEYBOARD_LL;
    CHookGlobal<eHookTypeMOUSE_LL,          WH_MOUSE_LL,        TCheckThan0 > m_hook_MOUSE_LL;
    CHookThread<eHookTypeThreadKEYBOARD_LL, WH_MOUSE_LL,        TCheckThan0 > m_hook_ThreadKEYBOARD_LL;

    static CHooks s_hooks;

    CHook* GetHooks()
    {
        DbgAssertS((sizeof(CHook) * eHookType_End) == sizeof(CHooks));
        return &m_hook_MSGFILTER;
    }

    CHook& operator[] (int n)
    {
        DbgAssert(n >= 0);
        DbgAssert(n < eHookType_End);
        return GetHooks()[n];
    }
};

CHooks CHooks::s_hooks;


TM_END_NAMESPACE

void HookInstall(EHookType type, PFNHOOKPROC proc, TMLParam lData)
{
    TM::CHooks::s_hooks[type].SetHook(proc, lData);
}

void HookRemove(EHookType type, PFNHOOKPROC proc, TMLParam lData)
{
    TM::CHooks::s_hooks[type].RemoveHook(proc, lData);
}


