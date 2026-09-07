/*
 *  @file
 *  @brief  thunk
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2004.02.24
 *
 *  Copyright (C) 2004 miragekiller
 */

#pragma once

#ifndef TMTHUNK_H
#define TMTHUNK_H

#include "tmdll.h"
#include "tmio.h"


TM_BGN_NAMESPACE

TM_BGN_PRIVATE_NAMESPACE

///////////////////////////////////////////////////////////////////////////////
// _StdThunk
#if TM_X86
#   pragma pack(push, 1)
    struct _StdThunk
    {
        DWORD   m_mov;          // mov dword ptr [esp+0x4], pThis
        DWORD   m_this;         // pThis
        BYTE    m_jmp;          // jmp WndProc
        DWORD   m_relproc;      // relative jmp

        _StdThunk ()
            : m_mov     (0x042444C7)
            , m_this    (0)
            , m_jmp     (0xe9)
            , m_relproc (0)
        {
        }
    };
#   pragma pack(pop)

///////////////////////////////////////////////////////////////////////////////
// _CThunkImplT
template <class T> class _CThunkImplT
{
protected:
    T m_thunk;

public:
    TM_SELF_ALLOC(_CThunkImplT);

    _CThunkImplT()
    {
        DWORD dwFlag = 0;
        VirtualProtectEx(GetCurrentProcess(), this, sizeof(*this), PAGE_EXECUTE_READWRITE, &dwFlag);
    }

    void SetCallBack(void* pThis, WNDPROC proc)
    {
        m_thunk.m_this = (DWORD)pThis;
        m_thunk.m_relproc = (ptrdiff_t)proc - ((ptrdiff_t)this + sizeof(m_thunk));

        // write block from data cache and
        //  flush from instruction cache
        FlushInstructionCache(GetCurrentProcess(), &m_thunk, sizeof(m_thunk));
    }

    void* GetProcAddress() { return &m_thunk; }
};

typedef _CThunkImplT<_StdThunk> _CStdThunk;

static _CStdThunk*  WINAPI My_AllocateData() { return new _CStdThunk(); }
static void         WINAPI My_FreeData(_CStdThunk* hThunk) { delete hThunk; }
static void         WINAPI My_InitData(_CStdThunk* hThunk, WNDPROC Proc, void* FirstParameter) {hThunk->SetCallBack(FirstParameter, Proc); }
static WNDPROC      WINAPI My_DataToCode(_CStdThunk* hThunk) { return (WNDPROC)hThunk->GetProcAddress(); }

#else   // #ifdef TM_X86

struct _CStdThunk;

#endif // #ifdef TM_X86 else


TM_END_PRIVATE_NAMESPACE


///////////////////////////////////////////////////////////////////////////////
// CAtlThunk
struct dll_atlthunk { DECLARE_DLL_LOAD("atlthunk.dll", LOAD_LIBRARY_SEARCH_SYSTEM32); };
struct CAtlThunk : public TM::CNoCopyable
{
private:
    typedef PRI::_CStdThunk*            HAtlThunk;

    struct CAtlThunkFunc
    {
    private:
        typedef HAtlThunk   (WINAPI* F_AtlThunk_AllocateData)();
        typedef void        (WINAPI* F_AtlThunk_InitData)(HAtlThunk hThunk, WNDPROC Proc, void* FirstParameter);
        typedef WNDPROC     (WINAPI* F_AtlThunk_DataToCode)(HAtlThunk hThunk);
        typedef void        (WINAPI* F_AtlThunk_FreeData)(HAtlThunk hThunk);
        
    public:
        F_AtlThunk_AllocateData fAllocateData;
        F_AtlThunk_InitData     fInitData;
        F_AtlThunk_DataToCode   fDataToCode;
        F_AtlThunk_FreeData     fFreeData;

        CAtlThunkFunc()
        {
            TMHModule hm = dll_atlthunk::Load();
#ifndef TM_WIN64
            if (!hm)
            {
                fAllocateData = PRI::My_AllocateData;
                fInitData     = PRI::My_InitData;
                fDataToCode   = PRI::My_DataToCode;
                fFreeData     = PRI::My_FreeData;
            }
            else
#endif // TM_WIN64
            {
                DbgAssert(hm);
                fAllocateData = (F_AtlThunk_AllocateData) DllSym(hm, ("AtlThunk_AllocateData"));
                fInitData     = (F_AtlThunk_InitData)     DllSym(hm, ("AtlThunk_InitData"));
                fDataToCode   = (F_AtlThunk_DataToCode)   DllSym(hm, ("AtlThunk_DataToCode"));
                fFreeData     = (F_AtlThunk_FreeData)     DllSym(hm, ("AtlThunk_FreeData"));
            }
        }
    };

    static CAtlThunkFunc& _func()
    {
        static CAtlThunkFunc _f;
        return _f;
    }

    HAtlThunk m_hThunk;

public:
    CAtlThunk()
        : m_hThunk(0)
    {}

    ~CAtlThunk()
    {
        if (m_hThunk)
            _func().fFreeData(m_hThunk);
    }

    void Swap(CAtlThunk& rhs)
    {
        tm_swap(m_hThunk, rhs.m_hThunk);
    }

    WNDPROC GetWNDPROC()
    {
        DbgAssert(m_hThunk);
        return _func().fDataToCode(m_hThunk);
    }

    void Init(WNDPROC proc, void* pThis)
    {
        CAtlThunkFunc& _f = _func();
        if (!m_hThunk)
            m_hThunk = _f.fAllocateData();
        _f.fInitData(m_hThunk, proc, pThis);
    }
};


TM_END_NAMESPACE



#endif // TMTHUNK_H
