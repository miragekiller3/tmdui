/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2025.06.03
 *
 *  Copyright (C) 2025 miragekiller
 */

#include "stdafx.h"

#include "MemHook.h"
#include "tmminhook.h"
#include "tmtime.h"
#include "tmdebugext.h"
#include "tmthread.h"
#include "tmtime.h"
#include "tmfilename.h"
#include <shellapi.h>
#pragma comment(lib, "dbghelp.lib")

using namespace TM;

int nAllCount;
int nAllStack;
// void* pStack[200];
// int nNow;

#undef DbgAssert
void DBK()
{
 DebugBreak();
}
#define DbgAssert(x)    if ((x)) {} else DBK()

struct CCyc
{
    int nCount;
    TMUInt64 tm;
    CCyc() : tm(0), nCount(0) {}
    void Add(const CCycle& r) { tm += r.Elapse(); ++nCount; }
};

static CCyc s_cycAddMem;
static CCyc s_cycDelMem;
static CCyc s_cycAddCB;
static CCyc s_cycMakeHash;
static CCyc s_cycMakeCB;

//+   s_cycAddMem{ nCount = 12064898 tm = 1152005858 }	CCyc
//+    s_cycAddCB{ nCount = 11727867 tm = 100722819 }	CCyc
//+ s_cycMakeHash{ nCount = 11727867 tm = 3180630 }	CCyc
//+   s_cycMakeCB{ nCount = 11727867 tm = 387015276 }	CCyc
//+   s_cycDelMem{ nCount = 10448690 tm = 124179049 }	CCyc


struct CSym
{
    HANDLE hProcess;
    char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
    PSYMBOL_INFO symbol;

    CSym()
        : hProcess(0)
    {
    }

    ~CSym()
    {
        SymCleanup(hProcess);
    }

    void Init()
    {
        if (hProcess)
            return;
        hProcess = GetCurrentProcess();
        SymInitialize(hProcess, NULL, TRUE);
        SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);// | SYMOPT_DEBUG);// | SYMOPT_AUTO_PUBLICS);
        symbol = (PSYMBOL_INFO)symbolBuffer;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;
    }

    CString GetFunctionNameFromAddress(void* address)
    {
        DWORD64 displacement = 0;
        CString str;
        if (SymFromAddr(hProcess, (DWORD64)address, &displacement, symbol))
        {
            TCHAR mdname[512];
            int n = GetModuleFileName((HMODULE)symbol->ModBase, mdname, tm_countof(mdname));
            str = CFnView(CStrView(mdname, n)).GetName();
            str += '!';
            str.Append(symbol->Name, symbol->NameLen);
        }
        else
            str.Format(L"0x%p", address);
        return str;
    }

    // SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
    TM::CString GetSourceAndLine(void* address)
    {
        IMAGEHLP_LINE64 lineInfo = { sizeof(IMAGEHLP_LINE64) };
        DWORD displacement;
        CStringA str;
        if (SymGetLineFromAddr64(hProcess, (DWORD64)address, &displacement, &lineInfo))
            str.Format("%s:%d", lineInfo.FileName, lineInfo.LineNumber);
        return str;
    }

    void JumpToSource(void* address)
    {
       IMAGEHLP_LINE64 lineInfo = { sizeof(IMAGEHLP_LINE64) };
       DWORD displacement;
       if (!SymGetLineFromAddr64(hProcess, (DWORD64)address, &displacement, &lineInfo))
           return;

       TMCStr fn = L"c:\\temp\\goto_vs.vbs";
       {
           CStringA str;
           str.Format(
               "Set dte = GetObject(, \"VisualStudio.DTE\")\r\n"
               "dte.ExecuteCommand \"File.OpenFile\", \"\"\"%s\"\"\"\r\n"
               "dte.ExecuteCommand \"Edit.GoTo\", \"%d\"",
               lineInfo.FileName, lineInfo.LineNumber);
           CFile f;
           f.CreateAlways(fn, f.e_faWrite);
           f.Write(str, str.length());
       }
       ::ShellExecute(NULL, TM_T("open"), TM_T("cscript.exe"), fn, NULL, SW_HIDE);
    }

    //SymEnumerateModules64(GetCurrentProcess(),
    //    [](PCSTR ModuleName, DWORD64 BaseOfDll, PVOID UserContext) -> BOOL {
    //        SymLoadModuleEx(GetCurrentProcess(), NULL, ModuleName, NULL,
    //            BaseOfDll, 0, NULL, 0);
    //        return TRUE;
    //    }, NULL);

    //SymEnumSymbols(GetCurrentProcess(), moduleBase, "*",
    //    [](PSYMBOL_INFO pSymInfo, ULONG SymbolSize, PVOID UserContext) -> BOOL {
    //        printf("Function: %-30s Addr: 0x%llX Size: %lu\n",
    //            pSymInfo->Name, pSymInfo->Address, pSymInfo->Size);
    //        return TRUE;
    //    }, NULL);
};


///////////////////////////////////////////////////////////////////////////////
// temp buffer
struct CTempMemBuff : public CCallBack
{
    time_t time;
    void* pMem;
    int nSize;
    bool bAlloc;
    bool bDataOk;
};
//#define MAX_CACHE_BUFFER    41000
//#define MAX_CACHE_SAFE      40000
enum { MAX_CACHE_BUFFER    = 1 << 14 };
enum { MAX_CACHE_SAFE      = MAX_CACHE_BUFFER - 512 };
CTempMemBuff    s_buf[MAX_CACHE_BUFFER];
TMAtmInt        s_nSet = 0; // 0 - MAX_CACHE_BUFFER
TMULong         s_nGet = 0; // 1 - MAX_CACHE_BUFFER
TMAtmInt        s_nCached = 0;
CState          s_state;
CThreadID       s_thread;
// int nMax;

struct CMemMgr
{
    struct CMem
    {
        time_t          m_time;
        CallBackInfo*   m_pcb;
        int             m_nSize;

        CMem() : m_nSize(0)
        {
        }

        void _Init(CCallBackMap& acb, CallBackInfo& cb)
        {
            CCycle cyc;
            CCallBackMap::iterator iter = acb.insert((CCallBackMap::value_type&)cb).first;
            s_cycAddCB.Add(cyc);
            
            m_pcb = (CallBackInfo*)&*iter;
            m_pcb->second.m_nSize += m_nSize;
            ++m_pcb->second.m_nCount;
        }
        
        void Init(CCallBackMap& acb, int sz)
        {
            m_time = CTime::GetCurrentTime();
            m_nSize = sz;
            CallBackInfo cb;
            cb.first.Init();
            _Init(acb, cb);
        }

        void Init(CCallBackMap& acb, CTempMemBuff& buf)
        {
            m_time = buf.time;
            m_nSize = buf.nSize;
            buf.MakeHash();
            CallBackInfo cb(buf, CCallBackVal());
            _Init(acb, cb);
        }
    };

    typedef TM::CMap<void*, CMem, std::less<void*>, my_alloc_map(void*, CMem) > CMemMap;
    size_t          m_nAllMem;
//     HANDLE          m_heap;
    CMemMap         m_aMem[1];//[0xff + 1];
    CCallBackMap    m_aCallBack;
    CSym            m_sym;
    int             m_bMainThreadDisable;
    TMThreadID      m_tid;

    // for temp data


    static CMemMgr s_mgr;

    CMemMgr()
        : m_nAllMem(0)
        , m_bMainThreadDisable(0)
//         , m_heap(HeapCreate(0, 10000000, 0))
    {
        m_tid = ThreadSelfId();
    }

    ~CMemMgr()
    {
//         HeapDestroy(m_heap);
    }

#define  P2ID(x) 0
//     int P2ID(LPVOID p)
//     {
//         int n = ((int)(INT_PTR)p) & 0xff00;
//         return n >> 8;
//     }

//     void CachePut(HANDLE hHeap, void* p, SIZE_T sz, bool bAlloc)
//     {
//         if (!p
//             || (m_bMainThreadDisable && ThreadSelfId() == m_tid)
//             || _dbgApp.IsMemHeap(hHeap))
//             return;
//         ++nAllCount;
// 
//         int n = AtomicInc(&s_nSet) - 1;
//         DbgAssert(n < tm_countof(s_buf));
// //         int m = s_nGet;
// //         if (n < m) // restart s_buf;
// //             Sleep(20);
// //         if (n > MAX_CACHE_BUFFER / 2)
// //             Sleep(10);
//         //int ds = s_nGet - n;
//         //if (ds >= 0 && ds < 100)
//         //{
//         //    while(s_buf[n])
//         //    Sleep((99 - ds) / 10);
//         //    ds = s_nGet - n - 1;
//         //}
// 
//         int nCached = AtomicInc(&s_nCached);
//         if (nCached > MAX_CACHE_SAFE)
//             Sleep(nCached - MAX_CACHE_SAFE);
// 
//         CTempMemBuff& buf = s_buf[n];
//         DbgAssert(!buf.pMem);
//         DbgAssert(!buf.bDataOk);
//         buf.time = CTime::GetCurrentTime();
//         buf.pMem = p;
//         buf.nSize = sz;
//         if (bAlloc)
//             buf.MackCallBack();
//         buf.bAlloc = bAlloc;
//         buf.bDataOk = true;
//         s_state.Signal();
//     }
//     
//     void CacheGet()
//     {
//         int nMax = s_nSet;
//         BOOL bOut = false;
//         
//         // check buffer end.
//         if (s_nSet > MAX_CACHE_SAFE)
//         {
//             for (; !AtomicCompareSet(&s_nSet, nMax, 0) ; nMax = s_nSet) {}
//             DbgAssert(nMax < tm_countof(s_buf));
//             bOut = true;
//         }
//         
//         for (; s_nGet < nMax; )
//         {
//             int n = s_nGet;
//             CTempMemBuff& buf = s_buf[n];
//             if (!buf.bDataOk)
//             {
//                 Sleep(1);
// //                DbgAssert(buf.pMem);
//                 continue;
//             }
//             if (!buf.pMem)
//             {
//                 AtomicDec(&s_nCached);
//                 ++s_nGet;
//                 continue;
//             }
//             if (buf.bAlloc)
//                 _Add(buf);
//             else
//                 _Del(buf.pMem);
//             buf.pMem = 0;
//             buf.bDataOk = false;
//             AtomicDec(&s_nCached);
//             ++s_nGet;
//             continue;
//         }
// 
//         if (bOut)
//             s_nGet = 0;
//     }

    void CachePut(HANDLE hHeap, void* p, SIZE_T sz, bool bAlloc)
    {
        if (!p
            || (m_bMainThreadDisable && ThreadSelfId() == m_tid)
            || _dbgApp.DbgIsMemHeap(hHeap))
            return;
        ++nAllCount;
        
        TMULong n = AtomicInc(&s_nSet) - 1;
        n = n % MAX_CACHE_BUFFER;
        
        CTempMemBuff& buf = s_buf[n];
        DbgAssert(!buf.pMem);
        DbgAssert(!buf.bDataOk);
        buf.time = CTime::GetCurrentTime();
        buf.pMem = p;
        buf.nSize = sz;
        if (bAlloc)
            buf.MackCallBack();
        buf.bAlloc = bAlloc;
        buf.bDataOk = true;

        int nCached = AtomicInc(&s_nCached);
        s_state.Signal();

        if (nCached > MAX_CACHE_SAFE)
            Sleep(nCached - MAX_CACHE_SAFE);
    }
    
    void CacheGet()
    {
        for ( ; ; )
        {
            TMULong n = s_nGet % MAX_CACHE_BUFFER;
            CTempMemBuff& buf = s_buf[n];
            if (!buf.bDataOk)
            {
                Sleep(1);
//                DbgAssert(buf.pMem);
                continue;
            }
            DbgAssert(buf.bDataOk);
            DbgAssert(buf.pMem);
            if (buf.bAlloc)
                _Add(buf);
            else
                _Del(buf.pMem);
            buf.pMem = 0;
            buf.bDataOk = false;
            ++s_nGet;
            if (AtomicDec(&s_nCached) > 0)
                continue;
            break;
        }
    }

    static TM_DECALRE_THREAD_PROC(CacheProc)
    {
        for (;;)
        {
            s_state.Unsignal();
            s_state.Lock();
            s_mgr.CacheGet();
        }
        return 0;
    }

    void _Add(CTempMemBuff& buf)
    {
        ++nAllCount;
        void* p = buf.pMem;
        size_t dwBytes = buf.nSize;

        _dbgApp.DbgMemLock();
        CCycle cyc;
        CMem& mem = m_aMem[P2ID(p)][p];
        s_cycAddMem.Add(cyc);
        
        if (!mem.m_nSize)
        {
            mem.Init(m_aCallBack, buf);
            m_nAllMem += dwBytes;
        }
        else
        {
            int dn = dwBytes - mem.m_nSize;
            m_nAllMem += dn;
            mem.m_nSize = dwBytes;
            mem.m_pcb->second.m_nSize += dn;
        }
        _dbgApp.DbgMemUnlock();
    }

    void NormalAdd(HANDLE hHeap, LPVOID p, SIZE_T dwBytes)
    {
        if (!p
            || (m_bMainThreadDisable && ThreadSelfId() == m_tid)
            || _dbgApp.DbgIsMemHeap(hHeap))
            return;
        ++nAllCount;
//         if (nAllCount == 9902572)
//         {
//             OutputDebugStringA("");
//         }
//         pStack[nNow++] = p;
//         if (nNow >= 6)
//             nNow = 0;


//         HeapLock(m_heap);
        _dbgApp.DbgMemLock();

        CCycle cyc;
        CMem& mem = m_aMem[P2ID(p)][p];
        s_cycAddMem.Add(cyc);

        if (!mem.m_nSize)
        {
            mem.Init(m_aCallBack, dwBytes);
            m_nAllMem += dwBytes;
        }
        else
        {
            int dn = dwBytes - mem.m_nSize;
            m_nAllMem += dn;
            mem.m_nSize = dwBytes;
            mem.m_pcb->second.m_nSize += dn;
        }
        _dbgApp.DbgMemUnlock();
//        HeapUnlock(m_heap);
//         DbgAssert(dwBytes == mem.m_nSize);
    }

    void _Del(LPVOID p)
    {
        _dbgApp.DbgMemLock();
        CCycle cyc;
        CMemMap::recorder rec(m_aMem[P2ID(p)]);
        rec.find(p);
        if (rec)
        {
            CMem& mem = rec.get_data();
            m_nAllMem -= mem.m_nSize;
            --mem.m_pcb->second.m_nCount;
            ++mem.m_pcb->second.m_nDel;
            if (mem.m_pcb->second.m_nCount == 0)
                DbgAssert(mem.m_pcb->second.m_nSize == mem.m_nSize);
            mem.m_pcb->second.m_nSize -= mem.m_nSize;
            mem.m_pcb->second.m_nSizeDel += mem.m_nSize;
            rec.erase();
            s_cycDelMem.Add(cyc);
        }
        _dbgApp.DbgMemUnlock();
    }

    void NormalDel(HANDLE hHeap, LPVOID p)
    {
        if (!p || _dbgApp.DbgIsMemHeap(hHeap))
            return;
        _Del(p);
    }

    void Add(HANDLE hHeap, LPVOID p, SIZE_T dwBytes)
    {
        CachePut(hHeap, p, dwBytes, true);
//        NormalAdd(hHeap, p, dwBytes);
    }

    void Del(HANDLE hHeap, LPVOID p)
    {
        CachePut(hHeap, p, 0, false);
//        NormalDel(hHeap, p);
    }

};
CMemMgr CMemMgr::s_mgr;

// char* _MyAlloc(size_t n) { return (char*)HeapAlloc(CMemMgr::s_mgr.m_heap, 0, n); }
// void  _MyFree(void* p) { HeapFree(CMemMgr::s_mgr.m_heap, 0, p); }


DECLARE_MINHOOK_FUNC(dll_kernel32, LPVOID, WINAPI, HeapAlloc,
                     (HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes))
{
    void* p = _HeapAlloc_Org(hHeap, dwFlags, dwBytes);
    CMemMgr::s_mgr.Add(hHeap, p, dwBytes);
    return p;
}

DECLARE_MINHOOK_FUNC(dll_ntdll, LPVOID, WINAPI, RtlAllocateHeap,
                     (HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes))
{
    void* p = _RtlAllocateHeap_Org(hHeap, dwFlags, dwBytes);
    CMemMgr::s_mgr.Add(hHeap, p, dwBytes);
    return p;
}


DECLARE_MINHOOK_FUNC(dll_kernel32, LPVOID, WINAPI, HeapReAlloc,
                     (HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes))
{
    CMemMgr::s_mgr.Del(hHeap, lpMem);
    void* p = _HeapReAlloc_Org(hHeap, dwFlags, lpMem, dwBytes);
    CMemMgr::s_mgr.Add(hHeap, p, dwBytes);
    return p;
}

DECLARE_MINHOOK_FUNC(dll_ntdll, LPVOID, WINAPI, RtlReAllocateHeap,
                     (HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes))
{
    CMemMgr::s_mgr.Del(hHeap, lpMem);
    void* p = _RtlReAllocateHeap_Org(hHeap, dwFlags, lpMem, dwBytes);
    CMemMgr::s_mgr.Add(hHeap, p, dwBytes);
    return p;
}

DECLARE_MINHOOK_FUNC(dll_kernel32, BOOL, WINAPI, HeapFree,
                     (HANDLE hHeap, DWORD dwFlags, LPVOID lpMem))
{
    CMemMgr::s_mgr.Del(hHeap, lpMem);
    BOOL b = _HeapFree_Org(hHeap, dwFlags, lpMem);
    return b;
}

DECLARE_MINHOOK_FUNC(dll_ntdll, BOOL, WINAPI, RtlFreeHeap,
                     (HANDLE hHeap, DWORD dwFlags, LPVOID lpMem))
{
    CMemMgr::s_mgr.Del(hHeap, lpMem);
    BOOL b = _RtlFreeHeap_Org(hHeap, dwFlags, lpMem);
    return b;
}


///////////////////////////////////////////////////////////////////////////////
// CCallBack
void CCallBack::Init()
{
    MackCallBack();
    MakeHash();
}

void CCallBack::MackCallBack()
{
    CCycle cyc;
    m_nCallBack = _dbgApp.GetCallStack(32, (void**)&m_callback, 5);
    s_cycMakeCB.Add(cyc);
}

void CCallBack::MakeHash()
{
    m_hash = 0;
    CCycle cyc;
    for (int i = 0; i < m_nCallBack; )
    {
        void* p = m_callback[i];
        //if (p == _HeapAlloc_My || p == _HeapReAlloc_My)
        //{
        //    std::copy(&(m_callback[i + 1]), &(m_callback[m_nCallBack]), &(m_callback[0]));
        //    m_hash = 0;
        //    m_nCallBack -= i;
        //    i = 0;
        //    continue;
        //}
        m_hash += (UINT_PTR)p;
        ++i;
    }
    s_cycMakeHash.Add(cyc);
}


CCallBackMap& BeginGetCallBackMap()
{
    CMemMgr::s_mgr.m_sym.Init();
    _dbgApp.DbgMemLock();
//    HeapLock(CMemMgr::s_mgr.m_heap);
    return CMemMgr::s_mgr.m_aCallBack;
}

void EndGetCallBackMap()
{
    _dbgApp.DbgMemUnlock();
//    HeapUnlock(CMemMgr::s_mgr.m_heap);
}

CString GetCallBackStr(void* f)
{
    return CMemMgr::s_mgr.m_sym.GetFunctionNameFromAddress(f);
}

void JumpToSource(void* f)
{
    CMemMgr::s_mgr.m_sym.JumpToSource(f);
}

TM::CString GetSourceAndLine(void* f)
{
    return CMemMgr::s_mgr.m_sym.GetSourceAndLine(f);
}

void HookMainThreadEnable()
{
    DbgAssert(ThreadSelfId() == CMemMgr::s_mgr.m_tid);
    --CMemMgr::s_mgr.m_bMainThreadDisable;
}

void HookMainThreadDisable()
{
    DbgAssert(ThreadSelfId() == CMemMgr::s_mgr.m_tid);
    ++CMemMgr::s_mgr.m_bMainThreadDisable;
}

bool EnumSymbols(LPCSTR dllPath, PSYM_ENUMERATESYMBOLS_CALLBACK proc, void* param, DWORD64 pStart, DWORD64 pEnd)
{
    CMemMgr::s_mgr.m_sym.Init();

    HMODULE hModule = GetModuleHandleA(dllPath);
    char c[2048] = {0};
    GetModuleFileNameA(hModule, c, 2048);


    DWORD64 baseAddr = (DWORD64)hModule;
    HANDLE hProcess = CMemMgr::s_mgr.m_sym.hProcess;
//    DbgVerify(baseAddr = SymLoadModuleEx(hProcess, NULL, 0, dllPath, baseAddr, pEnd - pStart, NULL, 0));
    DbgVerify(SymLoadModuleEx(hProcess, NULL, 0, dllPath, baseAddr, pEnd - pStart, NULL, 0));

    BOOL b;
    DbgVerify(b = SymEnumSymbols(hProcess, baseAddr, "*", proc, param));
//    DbgVerify(b = SymEnumSymbolsForAddr(hProcess, baseAddr, proc, param));

 //   SymUnloadModule64(hProcess, baseAddr);
    return 0 != b;
}

static void* s_paNow;
 //bool EnumSymbols(HMODULE hModule, PSYM_ENUMERATESYMBOLS_CALLBACK proc, void* param, DWORD64 pStart, DWORD64 pEnd)
 //{
 //    CMemMgr::s_mgr.m_sym.Init();
 //    s_paNow = param;
 //    
 //    HANDLE hProcess = CMemMgr::s_mgr.m_sym.hProcess;
 //    SYMBOL_INFO_PACKAGE s = { {sizeof(SYMBOL_INFO)} };
 //    s.si.MaxNameLen = MAX_SYM_NAME + 1;
 ////     s.si.ModBase = (DWORD64)hModule;
 //    DWORD64 disp = 0;
 ////     BOOL b;
 ////     DbgVerify(b = SymFromAddr(hProcess, pStart, &disp, &s.si));
 ////     if (!b)
 ////         return false;
 //    s.si.Address = pStart - 1;
 ////     if (s.si.Address < (ULONG64)pStart)
 ////     {
 ////         s.si.Address = (ULONG64)pStart - 1;
 ////         SymNext(hProcess, &s.si);
 ////     }
 //
 //    TM::CClock clock;
 //    for (TM::CClock clock2; s.si.Address < pEnd; )
 //    {
 //        DWORD64 old = s.si.Address;
 //        for (;;)
 //        {
 //            if (!SymNext(hProcess, &s.si))
 //                return true;
 //            if (old < s.si.Address)
 //                break;
 //            s.si.Address = ++old;
 //            if (old >= pEnd)
 //                return true;
 //        }
 //
 //        CStrViewA sv(s.si.Name, s.si.NameLen);
 //        if (sv.Find("`RTTI ") >= 0)
 //            s.si.TypeIndex = -1;
 //        else if (sv.Find("`vftable'") >= 0)
 //            s.si.TypeIndex = -2;
 //        else if (sv.Find("::__LINE__Var") >= 0)
 //            s.si.TypeIndex = -3;
 //        proc(&s.si, sizeof(SYMBOL_INFO_PACKAGE), param);
 //
 //        if (clock2 > clock + 10 * 1000)
 //        {
 //            if (param != s_paNow)
 //                return true;
 //            MSG msg;
 //            while(::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
 //            {
 //                if (msg.message == WM_QUIT)
 //                    return true;
 //                ::TranslateMessage(&msg);
 //                ::DispatchMessage(&msg);
 //            }
 //        }
 //        else
 //            clock2.Reset();
 //    }
 //
 //    return true;
 //}

//bool EnumSymbols(HMODULE hModule, PSYM_ENUMERATESYMBOLS_CALLBACK proc, void* param, DWORD64 pStart, DWORD64 pEnd)
//{
//    CMemMgr::s_mgr.m_sym.Init();
//    s_paNow = param;
//    
//    HANDLE hProcess = CMemMgr::s_mgr.m_sym.hProcess;
//    SYMBOL_INFO_PACKAGE s = { {sizeof(SYMBOL_INFO)} };
//    s.si.MaxNameLen = MAX_SYM_NAME + 1;
//    //     s.si.ModBase = (DWORD64)hModule;
//    DWORD64 disp = 0;
//    //     BOOL b;
//    //     DbgVerify(b = SymFromAddr(hProcess, pStart, &disp, &s.si));
//    //     if (!b)
//    //         return false;
//    s.si.Address = pStart - 1;
//    //     if (s.si.Address < (ULONG64)pStart)
//    //     {
//    //         s.si.Address = (ULONG64)pStart - 1;
//    //         SymNext(hProcess, &s.si);
//    //     }
//    
//    TM::CClock clock;
//    for (TM::CClock clock2; s.si.Address < pEnd; )
//    {
//        DWORD64 old = s.si.Address;
//        for (;;)
//        {
//            old += s.si.Size ? s.si.Size : 8;
//            if (!SymFromAddr(hProcess, old, &disp, &s.si))
//                return true;
//            if (old < s.si.Address)
//                break;
// //           s.si.Address = ++old;
//            if (old >= pEnd)
//                return true;
//        }
//        
//        CStrViewA sv(s.si.Name, s.si.NameLen);
//        if (sv.Find("`RTTI ") >= 0)
//            s.si.TypeIndex = -1;
//        else if (sv.Find("`vftable'") >= 0)
//            s.si.TypeIndex = -2;
//        else if (sv.Find("::__LINE__Var") >= 0)
//            s.si.TypeIndex = -3;
//        proc(&s.si, sizeof(SYMBOL_INFO_PACKAGE), param);
//        
//        if (clock2 > clock + 10 * 1000)
//        {
//            if (param != s_paNow)
//                return true;
//            MSG msg;
//            while(::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
//            {
//                if (msg.message == WM_QUIT)
//                    return true;
//                ::TranslateMessage(&msg);
//                ::DispatchMessage(&msg);
//            }
//        }
//        else
//            clock2.Reset();
//    }
//    
//    return true;
//}



// error: SymPrev will get first sym in (pStart, pEnd)
 bool EnumSymbols(HMODULE hModule, PSYM_ENUMERATESYMBOLS_CALLBACK proc, void* param, DWORD64 pStart, DWORD64 pEnd)
 {
     CMemMgr::s_mgr.m_sym.Init();
     s_paNow = param;
     
     HANDLE hProcess = CMemMgr::s_mgr.m_sym.hProcess;
     SYMBOL_INFO_PACKAGE s = { {sizeof(SYMBOL_INFO)} };
     s.si.MaxNameLen = MAX_SYM_NAME + 1;
     //     s.si.ModBase = (DWORD64)hModule;
     DWORD64 disp = 0;
     //     BOOL b;
     //     DbgVerify(b = SymFromAddr(hProcess, pStart, &disp, &s.si));
     //     if (!b)
     //         return false;
     s.si.Address = pEnd;
     //     if (s.si.Address < (ULONG64)pStart)
     //     {
     //         s.si.Address = (ULONG64)pStart - 1;
     //         SymNext(hProcess, &s.si);
     //     }
     
     TM::CClock clock;
     for (TM::CClock clock2; s.si.Address > pStart; )
     {
         DWORD64 old = s.si.Address;
         if (!SymPrev(hProcess, &s.si))
             return true;
         if (old <= s.si.Address)
             return true;

//              break;
//          for (;;)
//          {
//              if (!SymPrev(hProcess, &s.si))
//                  return true;
//              if (old > s.si.Address)
//                  break;
//              return true;
//              s.si.Address = --old;
//              if (old < pStart)
//                  return true;
//          }
         
         CStrViewA sv(s.si.Name, s.si.NameLen);
         if (sv.Find("`RTTI ") >= 0)
             s.si.TypeIndex = -1;
         else if (sv.Find("`vftable'") >= 0)
             s.si.TypeIndex = -2;
         else if (sv.Find("`vbtable'") >= 0)
             s.si.TypeIndex = -3;
         else if (sv.Find("`local vftable'") >= 0)
             s.si.TypeIndex = -4;
         else if (sv.Find("::__LINE__Var") >= 0)
             s.si.TypeIndex = -5;
         proc(&s.si, sizeof(SYMBOL_INFO_PACKAGE), param);
         
         if (clock2 > clock + 5 * 1000)
         {
             if (param != s_paNow)
                 return true;
             MSG msg;
             while(::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
             {
                 if (msg.message == WM_QUIT)
                     return true;
                 ::TranslateMessage(&msg);
                 ::DispatchMessage(&msg);
             }
         }
         else
             clock2.Reset();
     }
     
     return true;
 }

// bool EnumSymbols(HMODULE hModule, PSYM_ENUMERATESYMBOLS_CALLBACK proc, void* param, DWORD64 pStart, DWORD64 pEnd)
// {
//     CMemMgr::s_mgr.m_sym.Init();
//     s_paNow = param;
//     
//     HANDLE hProcess = CMemMgr::s_mgr.m_sym.hProcess;
//     SYMBOL_INFO_PACKAGE s = { {sizeof(SYMBOL_INFO)} };
//     s.si.MaxNameLen = MAX_SYM_NAME + 1;
//     //     s.si.ModBase = (DWORD64)hModule;
//     DWORD64 disp = 0;
//     BOOL b;
//     DbgVerify(b = SymFromAddr(hProcess, pStart, &disp, &s.si));
//     if (!b)
//         return false;
//     if (s.si.Address < (ULONG64)pStart)
//     {
//         s.si.Address = (ULONG64)pStart;
//         SymNext(hProcess, &s.si);
//     }
//     
//     TM::CClock clock;
//     for (TM::CClock clock2; s.si.Address < pEnd; )
//     {
//         ULONG oldIndex = s.si.Index;
//         CStrViewA sv(s.si.Name, s.si.NameLen);
//         if (sv.Find("`RTTI ") >= 0)
//             s.si.TypeIndex = -1;
//         else if (sv.Find("`vftable'") >= 0)
//             s.si.TypeIndex = -2;
//         else if (sv.Find("::__LINE__Var") >= 0)
//             s.si.TypeIndex = -3;
//         proc(&s.si, sizeof(SYMBOL_INFO_PACKAGE), param);
//         for (;;)
//         {
//             SymFromIndex(hProcess, (ULONG64)hModule, s.si.Index, &s.si);
//             if (oldIndex >= s.si.Index)
//             {
//                 s.si.Index = ++oldIndex;
//                 if (s.si.Address >= pEnd)
//                     return true;
//                 continue;
//             }
//             break;
//         }
//         if (clock2 > clock + 10 * 1000)
//         {
//             if (param != s_paNow)
//                 return true;
//             MSG msg;
//             while(::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
//             {
//                 if (msg.message == WM_QUIT)
//                     return true;
//                 ::TranslateMessage(&msg);
//                 ::DispatchMessage(&msg);
//             }
//         }
//         else
//             clock2.Reset();
//     }
//     
//     return true;
// }


SYM_TYPE IsLoadSymbal(LPCSTR dllPath)
{
    CMemMgr::s_mgr.m_sym.Init();
    HMODULE hModule = GetModuleHandleA(dllPath);
    IMAGEHLP_MODULE mi = { sizeof(IMAGEHLP_MODULE) };
    SymGetModuleInfo(CMemMgr::s_mgr.m_sym.hProcess, (DWORD64)hModule, &mi);
    return mi.SymType;
}

CString SymbolsGetTypeNameFromIndex(DWORD64 modBase, DWORD typeIndex)
{
    CString str;
    LPTSTR typeName = NULL;
    SymGetTypeInfo(CMemMgr::s_mgr.m_sym.hProcess, modBase, typeIndex, TI_GET_SYMNAME, &typeName);
    if (typeName)
    {
        str = typeName;
        LocalFree(typeName);
    }
    return str;
}

void SymbolsAnalyzePData(HMODULE hModule, PSYM_ENUMERATESYMBOLS_CALLBACK proc, void* param, DWORD64 pStart, DWORD64 pEnd)
{
    SYMBOL_INFO_PACKAGE s = { {sizeof(SYMBOL_INFO)} };
    s.si.MaxNameLen = MAX_SYM_NAME + 1;
    HANDLE hProcess = CMemMgr::s_mgr.m_sym.hProcess;

    for (; pStart < pEnd; pStart += sizeof(IMAGE_RUNTIME_FUNCTION_ENTRY))
    {
        IMAGE_RUNTIME_FUNCTION_ENTRY* entry = (IMAGE_RUNTIME_FUNCTION_ENTRY*)pStart;
        DWORD64 disp = 0;
        DWORD64 addr = (DWORD64)hModule + entry->BeginAddress;
        DbgVerify(SymFromAddr(hProcess, addr, &disp, &s.si));
        s.si.Address = addr;
        s.si.Size = entry->EndAddress - entry->BeginAddress;
        proc(&s.si, sizeof(SYMBOL_INFO_PACKAGE), param);
    }
}

void GetResourceTypeName(CString& str, WORD id, int nLevel)
{
    static const char* const types[] = 
    {
        "0", "CURSOR", "BITMAP", "ICON", "MENU", "DIALOG",
        "STRING", "FONTDIR", "FONT", "ACCELERATOR", "RCDATA",
        "MESSAGETABLE", "GROUPCURSOR", "13", "GROUPICON",
        "15", "VERSION", "DLGINCLUDE", "18", "PLUGPLAY", "VXD",
        "ANICURSOR", "ANIICON", "HTML", "MANIFEST"
    };
    if (nLevel == 0 && id > 0 && id <= tm_countof(types))
        str += types[id];
    else
        str.AppendFormat(L"%d", id);
}

HMODULE s_hModule;
void ParseResourceDirectory(const CString& str, PSYM_ENUMERATESYMBOLS_CALLBACK proc, void* param, DWORD64 rsrcBase, IMAGE_RESOURCE_DIRECTORY* dir, int level = 0)
{
    IMAGE_RESOURCE_DIRECTORY_ENTRY* entries = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)(dir + 1);

    for (int i = 0; i < dir->NumberOfNamedEntries + dir->NumberOfIdEntries; ++i)
    {
        CString strName(str);
        if (entries[i].NameIsString)
        {
            IMAGE_RESOURCE_DIR_STRING_U* s = (IMAGE_RESOURCE_DIR_STRING_U*)(rsrcBase + (entries[i].NameOffset));
            strName.Append(s->NameString, s->Length);
        }
        else
            GetResourceTypeName(strName, entries[i].Id, level);
        strName += L" ";

        if (entries[i].DataIsDirectory)
        {
            IMAGE_RESOURCE_DIRECTORY* subDir = (IMAGE_RESOURCE_DIRECTORY*)(rsrcBase + (entries[i].OffsetToDirectory));
            ParseResourceDirectory(strName, proc, param, rsrcBase, subDir, level + 1);
        }
        else
        {
            IMAGE_RESOURCE_DATA_ENTRY* dataEntry = (IMAGE_RESOURCE_DATA_ENTRY*)(rsrcBase + entries[i].OffsetToData);
            SYMBOL_INFO_PACKAGE s = { {sizeof(SYMBOL_INFO)} };
            s.si.Address = (DWORD64)s_hModule + dataEntry->OffsetToData;
            s.si.Size = dataEntry->Size;
            strName.CopyTo(s.si.Name, 255);
            proc(&s.si, sizeof(SYMBOL_INFO_PACKAGE), param);
        }
    }
}

void SymbolsAnalyzeResource(HMODULE hModule, PSYM_ENUMERATESYMBOLS_CALLBACK proc, void* param, DWORD64 pStart, DWORD64 pEnd)
{
    s_hModule = hModule;
    ParseResourceDirectory(CString(), proc, param, pStart, (IMAGE_RESOURCE_DIRECTORY*)pStart);
}


// CMemByFunc::CMemByFunc()
//     : m_nSize(0)
// {
//     m_aMem.reserve(CMemMgr::s_mgr.m_aMem[0].size());
// }
// 
// void ParseMemByFunc(CMemByFuncMap& aMap)
// {
//     for (int i = 0; i < tm_countof(CMemMgr::s_mgr.m_aMem); ++i)
//     {
//         for (CMemMgr::CMemMap::recorder rec(CMemMgr::s_mgr.m_aMem[i]); rec; ++rec)
//         {
//             const CMemMgr::CMemMap::value_type& v = *rec.get_iter();
//             for (int j = 0; j < v.second.m_nCallBack; ++j)
//             {
//                 CMemByFunc& mbf = aMap[v.second.m_callback[j]];
//                 mbf.m_nSize += v.second.m_nSize;
//                 mbf.m_aMem.push_back(v.first);
//             }
//         }
//     }
// }

///////////////////////////////////////////////////////////////////////////////

// QClassBase* AddObjectQArrayData(void* p, int sub, const TM::CString& str)
// {
// //     QClassBase* pi;
// //     if (sub == 0)
// //         pi = QClassQArrayData::Create(p, "QArrayData");
//     //     else
//     //         pi = QClassQFontEngine::Create(p, "QFontEngine");
//     
//     return (QClassQArrayData*)p;
// }

void MemHookInit()
{
//     pStack[0] = 0;
//     nNow = 0;
    //_HeapFree_Install();
    //_HeapReAlloc_Install();
    //_HeapAlloc_Install();

    if (CMinHook::Inst())
    {
        _RtlFreeHeap_Install();
        _RtlReAllocateHeap_Install();
        _RtlAllocateHeap_Install();
        s_thread.CreateBy(CMemMgr::CacheProc, 0);
    }
}



