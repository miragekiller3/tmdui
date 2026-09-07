/**
 *  @file
 *  @brief  debug function 
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2002-09-10
 *
 *  Copyright (C) 2002 miragekiller                             
 */

#pragma once

#ifndef TMDEBUG_H
#define TMDEBUG_H

#include "tmdef.h"

#include <stdarg.h>
#include <stdio.h>

#ifdef TM_WIN
#   include <windows.h>
#endif

#ifdef TM_MSC
#   include <crtdbg.h>
# else
// #   include <assert.h>
//#   include <stdarg.h>
#endif


/*****************************************************************************/
#define TM_D(x)     TM_IF_DBG(x)



/*****************************************************************************/
/*  used to add the file name and line number to the output information during compilation
//      programmer can skip to the file error location by double-click in the output window
//  Sample:
//      #pragma message (FILE_AND_LINE "aaa") */

#define FILE_AND_LINE __FILE__"(" TM_NUMBER_OF(__LINE__) "): "


/*****************************************************************************/
/* protect function */
#ifdef TM_PROTECTED
#   define DbgProtect(ext,ret)      do { if (ext) return ret; } while (0)
#   define DbgProtectVoid(ext)     do { if (ext) return; }  while (0)
#else
#   define DbgProtect(ext,ret)
#   define DbgProtectVoid(ext)
#endif



/*****************************************************************************/
/* static assert function used to find the errors in compilation */
/* if expr is 0 "CStaticAssert<0> undefined " error will show in compilation */
#if !defined(TM_CPP)
#   define DbgAssertSx(name,expr)
#   define DbgAssertS(expr)
#elif (TM_CPP >= TM_CPP11)
#   define DbgAssertSx(name,expr)  static_assert(expr, #name)
#   define DbgAssertS(expr)        static_assert(expr, #expr)
#else
    template <bool x> struct _CStaticAssert;
    template <> struct _CStaticAssert<true> { enum { value = 1 }; };

#   define DbgAssertSx(name,expr)  enum { name = sizeof(_CStaticAssert<(bool)(expr)>) }
#   define DbgAssertS(expr)        DbgAssertSx(__testassert,expr)
#endif /* TM_CPP */

#define DbgAssertS1(expr)     DbgAssertSx(__testassert1,expr)
#define DbgAssertS2(expr)     DbgAssertSx(__testassert2,expr)
#define DbgAssertS3(expr)     DbgAssertSx(__testassert3,expr)
#define DbgAssertS4(expr)     DbgAssertSx(__testassert4,expr)
#define DbgAssertS5(expr)     DbgAssertSx(__testassert5,expr)
#define DbgAssertS6(expr)     DbgAssertSx(__testassert6,expr)
#define DbgAssertS7(expr)     DbgAssertSx(__testassert7,expr)
#define DbgAssertS8(expr)     DbgAssertSx(__testassert8,expr)
#define DbgAssertS9(expr)     DbgAssertSx(__testassert9,expr)



// DbgIsPtr
TM_INLINE bool DbgIsPtr(void* p)
{
#ifdef TM_DBG
    if (p == 0)
        return true;
    if (p == (void*)0xdddddddd)
        return false;
//    TM_IF_WIN(return !IsBadReadPtr(p, sizeof(int)));
#endif // TM_DBG;
    return true;
}



///////////////////////////////////////////////////////////////////////////////
// IDebug
struct CDebugLeakInfoList;
struct CMemDump;
struct TMDbgClassInfo;
class ITrace;
struct ITraceHook;

enum EDbgLeakOptType
{
    eDbgLeakOptTypeDef,         // define and save a class info for next function call. return old info;
    eDbgLeakOptTypeAdd,         // add a class info;
    eDbgLeakOptTypeRemove,      // remove a class info;
    eDbgLeakOptTypeCheck,       // Check class by p->m_pClass.
    eDbgLeakOptTypeIsThread,    // check class by p->m_pClass. is current thread == create thread; release mode: return true;
    eDbgLeakOptTypeSetID,       // find class by by p->m_pClass. set finded->m_id = p->m_id;
    //     eDbgLeakOptTypeFindByStr,   // return is count of same string > 1
    //     eDbgLeakOptTypeTraceByStr,  // trace all object info that same m_cInfo.
    eDbgLeakOptTypeFindByMem,   // find valid class info by mem. p is mem adress, not const TMDbgClassInfo*
};

#ifdef TM_DBG
#   define _dbgApp              (*DbgGetApp())
#   define _DBGAPP_VIRTUAL      virtual
#else
#   define _dbgApp              IDebug()
#   define _DBGAPP_VIRTUAL
#endif // TM_DBG 

class TM_NO_VTABLE IDebug
{
public:
    _DBGAPP_VIRTUAL char*   DbgMemAlloc(size_t n) { return (char*)malloc(n); }
    _DBGAPP_VIRTUAL void    DbgMemFree(void* p) { free(p); }
    _DBGAPP_VIRTUAL size_t  DbgMemSize(void* p) { return 0; }
    _DBGAPP_VIRTUAL void    DbgMemLock() {}
    _DBGAPP_VIRTUAL void    DbgMemUnlock() {}
    _DBGAPP_VIRTUAL bool    DbgIsMemHeap(void* hHeap) { return !hHeap; }
    
    _DBGAPP_VIRTUAL int     GetCallStack(int nFrames, void** ppBackTrace, int nSkip = 4) { return 0; }
    
    _DBGAPP_VIRTUAL CDebugLeakInfoList* LeakGetBegin() { return 0; }
    _DBGAPP_VIRTUAL void    LeakGetEnd() {}
    _DBGAPP_VIRTUAL void    LeakFlip() {}
    _DBGAPP_VIRTUAL void    LeakDump(CMemDump* dumpOld, CMemDump* dumpNew, int& nDebugMem) {}
    _DBGAPP_VIRTUAL const TMDbgClassInfo* LeakOpt(const TMDbgClassInfo* p, EDbgLeakOptType eType) { return (eType == eDbgLeakOptTypeFindByMem) ? 0 : p; }
    
    _DBGAPP_VIRTUAL ITrace* TraceCreate() { return 0; }
    _DBGAPP_VIRTUAL void    TraceAddHook(ITraceHook* p) {}
    _DBGAPP_VIRTUAL void    TraceDelHook(ITraceHook* p) {}
    _DBGAPP_VIRTUAL bool    TraceSetAssert(TMBool bEnable) { return false; }
    _DBGAPP_VIRTUAL bool    TraceSetToSys(TMBool bEnable)  { return false; }
    _DBGAPP_VIRTUAL bool    TraceSetToCon(TMBool bEnable)  { return false; }
    _DBGAPP_VIRTUAL void    TraceSetToFile(TMCStr file)  {}
    
};
TM_CAPI IDebug* DbgGetApp();

#undef _DBGAPP_VIRTUAL


// inline const TMDbgClassInfo* DbgLeakOpt(const TMDbgClassInfo* p, EDbgLeakOptType eType)
// {
//     return _dbgApp.LeakOpt(p, eType);
// }

#ifdef TM_DBG
#   define DbgLeakOpt(p,tp)                   _dbgApp.LeakOpt(p,tp) //DbgLeakOpt
#else
TM_INLINE const TMDbgClassInfo* DbgLeakOpt(const TMDbgClassInfo* p, EDbgLeakOptType eType) { return 0; }
#endif // TM_DBG

///////////////////////////////////////////////////////////////////////////////
// dbg_allocator
template<class T> struct dbg_allocator
{
    TM_SELF_ALLOCATOR(dbg_allocator,_dbgApp.DbgMemAlloc(n),_dbgApp.DbgMemFree(p));
};
#define dbg_allocator_map(k,v) tm_allocator_map(dbg_allocator,k,v)


///////////////////////////////////////////////////////////////////////////////
// TM_DBG_ALLOC
#define TM_DBG_ALLOC() \
    void* operator new(size_t n)    { return _dbgApp.DbgMemAlloc(n); } \
    void operator delete(void* p)   { if (p) _dbgApp.DbgMemFree(p); }


///////////////////////////////////////////////////////////////////////////////
// CDbgMemAutoLock
struct CDbgMemAutoLock
{
    CDbgMemAutoLock() { _dbgApp.DbgMemLock(); }
    ~CDbgMemAutoLock() { _dbgApp.DbgMemUnlock(); }
};




/*****************************************************************************/
/*  Following is code for debug and track function */
/*  Instructions(sample :)    */
/*      define static int unused=TM_SET_TRACE_XXX(***)  in stdafx.cpp */
/*      then call DbgTrace(...) to output the content */
/*      calling TM_SET_TRACE_XXX is not necessary
*/

typedef enum ETracePriority
{
    e_tpUnknown         = 0,
    e_tpDefault         = 1,        /* only for SetMinPriority() */
    e_tpVerbose         = 2,
    e_tpDebug           = 3,
    e_tpInfo            = 4,
    e_tpWarn            = 5,
    e_tpError           = 6,
    e_tpFatal           = 7,
    e_tpSilent          = 8,        /* only for SetMinPriority(); must be last */
    e_tpAssert          = 9,        /* only for SetMinPriority(); must be last */
    e_tpBreak           = 10,
    e_tpPriorityMask    = 0x00f,

    e_tpNoContextInfo   = 0x010,    // don't output time/thread info.
    e_tpNoEOF           = 0x040,    // don't new line after trace.
    e_tpHotKey          = 0x080,    // use hot key to open/close trace.
    e_tpAddIndent       = 0x100,
    e_tpDecIndent       = 0x200,
    e_tpSpy             = 0x400,    // only output to self ITraceSink.

    e_tpDebugNoInfo     = e_tpNoContextInfo | e_tpDebug,
} ETracePriority;


struct ITraceHook
{
    virtual void OnTrace(int tp, TMCStr pStr) = 0;
};


// unused function, support C call.
#ifdef TM_CPP
    struct _DbgUnused
    {
        _DbgUnused(...){}
        const _DbgUnused& operator()(...) const { return *this; }
    };
#else
    typedef void*               (*PFNDebugUnunsed0)(...);
    typedef PFNDebugUnunsed0    (*PFNDebugUnunsed1)(...);
    typedef PFNDebugUnunsed1    (*PFNDebugUnunsed2)(...);
    typedef PFNDebugUnunsed2    (*PFNDebugUnunsed3)(...);
    typedef PFNDebugUnunsed3    (*PFNDebugUnunsed4)(...);
    typedef PFNDebugUnunsed4    (*PFNDebugUnunsed5)(...);
    typedef PFNDebugUnunsed5    (*PFNDebugUnunsed6)(...);
    typedef PFNDebugUnunsed6    (*PFNDebugUnunsed7)(...);
    typedef PFNDebugUnunsed7    (*PFNDebugUnunsed8)(...);
    typedef PFNDebugUnunsed8    (*PFNDebugUnunsed9)(...);
    TM_INLINE  PFNDebugUnunsed9 _DbgUnused(...) { return (PFNDebugUnunsed9)(void*)&_DbgUnused; }
#endif // TM_CPP
#define DbgTraceNull   if (1) {} else _DbgUnused

// for trace, assert, verify
class TM_NO_VTABLE ITrace
{
public:
    virtual void Release() = 0;
    virtual void Append(TMCStrA fmt, va_list vl) = 0;
    virtual void Append(TMCStrW fmt, va_list vl) = 0;
    virtual void AppendLastError(TMCStrA fmt) = 0;
    virtual void Output(int tp) = 0;
};

template <int _tp>
class CDebugTraceT
{
protected:
    ITrace* m_pTrace;

    void Done()
    {
        if (m_pTrace)
        {
            ITrace* p = m_pTrace;
            m_pTrace = 0;
            p->Output(_tp);
            p->Release();
        }
    }

    void Discard()
    {
        if (m_pTrace)
        {
            m_pTrace->Release();
            m_pTrace = 0;
        }
    }

public:
    CDebugTraceT(TMCStr pTag = TM_T("TM "))
    {
        m_pTrace =  _dbgApp.TraceCreate();
        if (m_pTrace && pTag)
            m_pTrace->Append(pTag, 0);
    }

    CDebugTraceT& AppendV(TMCStrA fmt, va_list vl)
    {
        if (m_pTrace)
            m_pTrace->Append(fmt, vl);
        return *this;
    }

    CDebugTraceT& AppendV(TMCStrW fmt, va_list vl)
    {
        if (m_pTrace)
            m_pTrace->Append(fmt, vl);
        return *this;
    }

    CDebugTraceT& AppendLastError(TMCStrA fmt = 0)
    {
        if (m_pTrace)
            m_pTrace->AppendLastError(fmt);
        return *this;
    }

    CDebugTraceT& operator() (TMCStrA fmt, ...)
    {
        if (m_pTrace && fmt)
        {
            va_list vl;
            va_start(vl,fmt);
            m_pTrace->Append(fmt, vl);
            va_end(vl);
        }
        return *this;
    }

    CDebugTraceT& operator() (TMCStrW fmt, ...)
    {
        if (m_pTrace && fmt)
        {
            va_list vl;
            va_start(vl,fmt);
            m_pTrace->Append(fmt, vl);
            va_end(vl);
        }
        return *this;
    }

    void EndL() {  Done(); }

    void operator()()
    {
        Done();
    }

    ~CDebugTraceT()
    {
        Done();
    }
};

struct CDebugVerfy : public CDebugTraceT<e_tpDebug | e_tpNoContextInfo>
{
public:
    template <class T> CDebugVerfy(const T& _v, const char* str)
        : CDebugTraceT<e_tpDebug | e_tpNoContextInfo>(0)
    {
        if (_v)
            this->Discard();
        else
            this->AppendLastError(str);
    }
};

struct CDebugVerfyCom : public CDebugTraceT<e_tpDebug | e_tpNoContextInfo>
{
private:
    bool m_bFail;

public:
    CDebugVerfyCom(TMResult _v, const char* str)
        : CDebugTraceT<e_tpDebug | e_tpNoContextInfo>(0)
        , m_bFail(_v < 0)
    {
        if (m_bFail)
            this->operator()(str, _v);
        else
            this->Discard();
    }

    operator bool () const { return m_bFail; }
};

TM_INLINE  TMLParam _DbgTraceUnused(...) { return 0; }




/*
 *  void DbgTraceMethod(TMCStr tag, TMCStr format, ...);
 *  trace method entry/leave.
 *
 *  @code:
 *  void aaa(void* p, int i)
    {
        DbgTraceMethod(TM_T("aaa"), TM_T("p:%x, i:%d"), p, i);
        ...
    }
    output:
    entry aaa : p:0x1234, i:5
 *  @endcode
 *
 *  @remark:
 *  if use DbgTraceMethod in sub module must use {}:
 *  @code:
    case n:
    {
        DbgTraceMethod (TM_T("aaa - case n"), TM_T("p:%x, i:%d"), p, n);
        ...
    }
    break;
 *  @endcode
 *
 */

#if defined(TM_DBG)

struct _trace_method : public CDebugTraceT<e_tpDebug | e_tpDecIndent>
{
    typedef CDebugTraceT<e_tpDebug | e_tpAddIndent> trace_Entry;
    typedef CDebugTraceT<e_tpDebug | e_tpDecIndent> trace_Leave;

    template <class T> void Init(const T* p, const T* tag)
    {
        (*this)(p, tag);
    }

    _trace_method(TMCStrA tag)
    {
        Init("<---- %s", tag);
        trace_Entry() ("----> %s", tag);
    }

    _trace_method(TMCStrW tag)
    {
        Init(L"<---- %s", tag);
        trace_Entry() (L"----> %s", tag);
    }

    _trace_method(TMCStrA tag, TMCStrA fmt, ...)
    {
        Init("<---- %s", tag);
        va_list vl;
        va_start(vl, fmt);
        trace_Entry()("----> %s", tag).AppendV(fmt, vl);
        va_end(vl);
    }

    _trace_method(TMCStrW tag, TMCStrW fmt, ...)
    {
        Init(L"<---- %s", tag);
        va_list vl;
        va_start(vl, fmt);
        trace_Entry()(L"----> %s", tag).AppendV(fmt, vl);
        va_end(vl);
    }
};

#endif // #if TM_DBG


/*
 *  void DbgVerify(exp);
 *  trace if function return error
 *
 *  @code:
 *  void aaa(hWND hWnd)
    {
        DbgVerify(IsWindow())("hwnd:%x", hWnd);
        ...
    }
    output:
    if (IsWindow()) return true do nothing;
    else output "DbgVerify IsWindow(), lasterror:xxx hwnd:67899876"
 *  @endcode
 *
 */
#if defined(TM_CPP) && defined(TM_DBG)

#   define DbgTrace                         CDebugTraceT<e_tpDebug>()
#   define DbgTraceMethod                   _trace_method __trace_method

#   define DbgVerify(func)                  CDebugVerfy((func), FILE_AND_LINE "DbgVerify " #func  ", lasterror:%s")
#   define DbgVerifyHResult(func)           CDebugVerfyCom((func), FILE_AND_LINE "DbgVerifyHResult " #func  ", HRESULT:0x%X")
#   define DbgVerifyEqual(ret,func)         CDebugVerfy((ret) == (func), FILE_AND_LINE "DbgVerifyEqual(" #ret ") " #func ", lasterror:%s")
#   define DbgVerifyNotEqual(ret,func)      CDebugVerfy((ret) != (func), FILE_AND_LINE "DbgVerifyNotEqual(" #ret ") " #func ", lasterror:%s")

#   define DbgReturnHResult(func)           if (CDebugVerfyCom((func), FILE_AND_LINE "DbgVerifyHResult " #func  ", HRESULT:0x%X")) return
#   define DbgReturn0HResult(func)          if (CDebugVerfyCom((func), FILE_AND_LINE "DbgVerifyHResult " #func  ", HRESULT:0x%X")) return 0
#   define DbgReturnXHResult(func,x)        if (CDebugVerfyCom((func), FILE_AND_LINE "DbgVerifyHResult " #func  ", HRESULT:0x%X")) return x

#   define DbgAssert(exp)                   if ((exp)) {} else CDebugTraceT<e_tpAssert | e_tpNoContextInfo>().AppendLastError(#exp "\r\n" FILE_AND_LINE "\r\nlasterror:%s")
#   define DbgAssertK(exp)                  if ((exp)) {} else CDebugTraceT<e_tpAssert | e_tpNoContextInfo | e_tpHotKey>().AppendLastError(#exp "\r\n" FILE_AND_LINE "\r\nlasterror:%s")
#   define DbgBreak(exp)                    if ((exp)) {} else CDebugTraceT<e_tpBreak>().AppendLastError(#exp "\r\n" FILE_AND_LINE "\r\nlasterror:%s")


#else

#   define DbgTrace                         DbgTraceNull
#   define DbgTraceMethod                   DbgTrace

#   define DbgVerify(func)                  if ((func), 1) {} else _DbgUnused()
#   define DbgVerifyEqual(ret,func)         DbgVerify(func)
#   define DbgVerifyHResult(func)           DbgVerify(func)
#   define DbgVerifyNotEqual(ret,func)      DbgVerify(func)

#   define DbgReturnHResult(func)           if ((func) < 0) return
#   define DbgReturn0HResult(func)          if ((func) < 0) return 0
#   define DbgReturnXHResult(func,x)        if ((func) < 0) return x

#   define DbgAssert(exp)                   DbgTrace()
#   define DbgAssertK(exp)                  DbgTrace()
#   define DbgBreak(exp)                    DbgTrace()

#endif  // #if defined(TM_CPP) && defined(TM_DBG)


/* only cpp can call this class */
#if defined (_MSC_VER) && defined (_DEBUG) && defined (__cplusplus)

namespace TM
{
    class DbgMemLeak
    {
        _CrtMemState m_checkpoint;

    public:
        explicit DbgMemLeak()
        {
            _CrtMemCheckpoint(&m_checkpoint);
        };

        ~DbgMemLeak()
        {
            _CrtMemState checkpoint;
            _CrtMemCheckpoint(&checkpoint);
            _CrtMemState diff;
            _CrtMemDifference(&diff, &m_checkpoint, &checkpoint);
            _CrtMemDumpStatistics(&diff);
            _CrtMemDumpAllObjectsSince(&diff);
        };
    };

} // end of namespace TM

#   define DBG_MEM_LEAK(x)                  TM::DbgMemLeak x
#   define CHECKMEMORY()                    DbgAssert(_CrtCheckMemory())

#else

#   define DBG_MEM_LEAK(X)
#   define CHECKMEMORY()

#endif /* #if defined _MSC_VER && defined _DEBUG && defined __cplusplus */

// inline void CHECKMEMORY()
// {
//     if (!_CrtCheckMemory())
//     {
//         int x = 0;
//         x = 1;
//     }
// }


/* check function have message loop
 * sample:
 * {
 *   DebugCheckNoMessageLoop();
 *   aaa();
 * }
 * if aaa() call "GetMessage()/DispatchMessage()", DebugCheckNoMessageLoop() will pop assert window.
 */
#if defined(TM_DBG) && defined(GWLP_WNDPROC) && !defined(TUI_TEST_MODE)
    struct CDebugCheckNoMessageLoop
    {
        static TMBool& GetCheck() { static TMBool b; return b; }
        static HRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
        {
            GetCheck() = true;
            return DefWindowProc(hWnd, msg, wp, lp);
        }
        CDebugCheckNoMessageLoop()
        {
            static HWND hWnd = CreateWindow(TM_T("static"), 0, WS_CHILD, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0);
            ::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
            GetCheck() = false;
            ::PostMessage(hWnd, WM_USER, 0, 0);
        }
        ~CDebugCheckNoMessageLoop()
        {
            TMBool bNow = GetCheck();
            DbgAssert(bNow == false);
        }
    };
#   define DebugCheckNoMessageLoop() CDebugCheckNoMessageLoop TM_LINK_OF(_dcnml_,__LINE__)
#else
#   define DebugCheckNoMessageLoop()
#endif


#if defined(TM_CPP) && defined(TM_DBG)

////////////////////////////////////////////////////////////////////////////////
// write memory overflow detect
// check it 2 times when enter and leave function, to ensure if memory overflow occurs in the function
struct MemCheck
{
    MemCheck () { CHECKMEMORY(); }
    ~MemCheck () { CHECKMEMORY(); }
};

#define DbgMemCheck()  MemCheck __mem_check

#else

#define DbgMemCheck()

#endif




///////////////////////////////////////////////////////////////////////////////
// class leak check
// sample:
//      class CAAA { TM_CLASS_INSTANCE_CHECK(CAAA); ...}
//      if CAAA not destroy, the code will assert when thread exit

#ifndef TM_DBG_CAPTURE_CALLSTATCK
#   define TM_DBG_CAPTURE_CALLSTATCK 32
#endif

//#define TM_UNUSED_IC_GetDebugInfo()  enum { IC_GetDebugInfo = 0 }
#define TM_UNUSED_IC_GetDebugInfo()  char IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf) { return 0; }
//  int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf);

struct _FN_Dbg
{
    int IC_GetDebugInfo(struct TMDbgClassInfo* cls, char* pbuf);
};
typedef int (_FN_Dbg::*PFN_GetDebugInfo)(TMDbgClassInfo* cls, char* pbuf); // pbuf: 256 bytes; return BYTE of object;

struct TMDbgClassInfo
{
    enum ECheckValue    { eCheckValue = 0x1F2F3F4F };
    enum                { eStackSize = TM_DBG_CAPTURE_CALLSTATCK ? 32 : 1 };
    enum EType          { eTypeNormal, eTypeMemTM, eTypeQT };

    const char*         m_pTypeName;
    const char*         m_pClass;
    int                 m_idThread;
    TMUInt32            m_id;
    EType               m_eType : 4;
    char                m_bStackObj;
    char                m_bDumped;        // true: have dump. 
//     char                m_cInfo[256];
    int                 m_sizeCallStack;
    void*               m_addrCallStack[eStackSize];
    size_t              m_szClass;  // BYTE.
    size_t              m_szDebug;  // BYTE. 0: TMDbgClassInfo is a inner class
    size_t              m_szClass2; // BYTE.
    PFN_GetDebugInfo    m_pfnInfo;
    void*               m_pfnVoid;
    ECheckValue         m_checkByte;
};








#if defined(TM_CPP) && defined(TM_DBG)

#include <typeinfo>
// #include "tmtchar.h"

#ifdef TM_MSC
#   pragma warning (push)
#   pragma warning (disable : 4995) // '_snprintf': name was marked as #pragma deprecated
#   pragma warning (disable : 4996) // '_vsnprintf': This function or variable may be unsafe. Consider using _vsnprintf_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#endif // #ifdef TM_MSC


template <class T>
inline PFN_GetDebugInfo TMDbgClassInfo_func(int (T::*fn)(TMDbgClassInfo*, char*)) 
{
//     DbgAssertS(sizeof(fn) == sizeof(PFN_GetDebugInfo));
    return *reinterpret_cast<PFN_GetDebugInfo*>(&fn);
}

template <class T>
inline PFN_GetDebugInfo TMDbgClassInfo_func(char (T::*fn)(TMDbgClassInfo*, char*)) 
{
    return 0;
}

inline PFN_GetDebugInfo TMDbgClassInfo_func(int (__fastcall *fn)(TMDbgClassInfo*, char*)) 
{
//    DbgAssertS(sizeof(fn) == sizeof(PFN_GetDebugInfo));
    return *reinterpret_cast<PFN_GetDebugInfo*>(&fn);
}

inline PFN_GetDebugInfo TMDbgClassInfo_func(int) 
{
    return 0;
}

inline void TMDbgClassInfo_Init_typename(TMDbgClassInfo& ci, const char* pTypeName)
{
    if (pTypeName)
    {
        if (pTypeName[0] == 's' && pTypeName[1] == 't' && pTypeName[2] == 'r' && pTypeName[3] == 'u' && pTypeName[4] == 'c' && pTypeName[5] == 't' && pTypeName[6] == ' ')
            pTypeName += 7;
        else if (pTypeName[0] == 'c' && pTypeName[1] == 'l' && pTypeName[2] == 'a' && pTypeName[3] == 's' && pTypeName[4] == 's' && pTypeName[5] == ' ')
            pTypeName += 6;
        if (pTypeName[0] == 'T' && pTypeName[1] == 'M' && pTypeName[2] == ':' && pTypeName[3] == ':')
            pTypeName += 4;
    }
    ci.m_pTypeName = pTypeName;
}


template <class T>
inline void TMDbgClassInfo_Init(TMDbgClassInfo& ci, T* pClass, const char* pTypeName = typeid(T).name())
{
    ci.m_checkByte = TMDbgClassInfo::eCheckValue;
    TMDbgClassInfo_Init_typename(ci, pTypeName);
    ci.m_pClass = (const char*)pClass;
    ci.m_szClass2 = 0;
    ci.m_idThread = 0;
    ci.m_id = 0;
    ci.m_bDumped = false;
//     StrPrintfA(ci.m_cInfo, tm_countof(ci.m_cInfo), "%p %s", pClass, ci.m_pTypeName);// StrCopy(m_cInfo, tm_countof(m_cInfo), typeid(T).name());
    ci.m_pfnInfo = 0;
}

inline bool TMDbgClassInfo_IsInner(const TMDbgClassInfo* info, const void* mem)
{
    DbgAssert((!info->m_szDebug) ==
        (((const char*)info >= info->m_pClass) && ((const char*)info < info->m_pClass + info->m_szClass)));
    return (mem >= info->m_pClass) && (mem < info->m_pClass + info->m_szClass);
}

inline size_t TMDbgClassInfo_GetClassSize(const TMDbgClassInfo* info)
{
    return info->m_szClass2 + (info->m_szDebug ? info->m_szClass : info->m_szClass - sizeof(TMDbgClassInfo));
}

inline size_t TMDbgClassInfo_GetExtSize(const TMDbgClassInfo* info, char* pbuf = 0)
{
    if (info->m_pClass && info->m_pfnInfo)
        return (((_FN_Dbg*)info->m_pClass)->*info->m_pfnInfo)((TMDbgClassInfo*)info, pbuf);
    return 0;
}

inline size_t TMDbgClassInfo_GetRealSize(const TMDbgClassInfo* info)
{
    return TMDbgClassInfo_GetClassSize(info) + TMDbgClassInfo_GetExtSize(info);
}

inline const void* TMDbgClassInfo_Tail(const TMDbgClassInfo* info)
{
    return info->m_pClass + info->m_szClass;
}

inline const TMDbgClassInfo* TMDbgClassInfo_Check(void* pClass, TMBool bCheckThread)
{
    TMDbgClassInfo i;
    i.m_pClass = (const char*)pClass;
    return DbgLeakOpt(&i, bCheckThread ? eDbgLeakOptTypeIsThread : eDbgLeakOptTypeCheck);
}

inline void TMDbgClassInfo_SetID(void* pClass, TMUInt32 id)
{
    TMDbgClassInfo i;
    i.m_pClass = (const char*)pClass;
    i.m_id     = id;
    DbgLeakOpt(&i, eDbgLeakOptTypeSetID);
}


///////////////////////////////////////////////////////////////////////////////
// CInnerDbgInfoT only embedded in class T
// sample: class T { CInnerDbgInfoT<T> m_debuginfo; }
template <class T>
struct CInnerDbgInfoT : public TMDbgClassInfo
{
public:
    static int s_nCount;

private:
    void Init(T* pClass)
    {
        TMDbgClassInfo_Init(*this, pClass);
        this->m_eType = eTypeNormal;
        ++s_nCount;
        DbgLeakOpt(this, eDbgLeakOptTypeAdd);
        this->m_pfnInfo = 0; //TMDbgClassInfo_func(T::IC_GetDebugInfo) //TMDbgClassInfo_GetPfn<T>::pfn();// (PFN_GetDebugInfo)T::IC_GetDebugInfo;
        this->m_szDebug = 0;
        this->m_szClass = sizeof(T);
//         this->m_szClass2       = nExt;

        DbgAssert(TMDbgClassInfo_IsInner(this, pClass));
    }

    void Init()
    {
        Init(TM_OTHIS(T, m_dbg_instanceCheck));
    }

public:
    CInnerDbgInfoT() { Init(); }
    CInnerDbgInfoT(const CInnerDbgInfoT& rhs) { Init(); }

    ~CInnerDbgInfoT ()
    {
        --s_nCount;
        DbgAssert(s_nCount >= 0);
        DbgAssert(eCheckValue == m_checkByte);
        const TMDbgClassInfo* p;
        DbgAssert(p = DbgLeakOpt(this, eDbgLeakOptTypeRemove)); // destroy error
    }

    void Check(bool bFullCheck) const
    {
        DbgAssert(eCheckValue == m_checkByte);
        DbgAssert(m_szClass);
        if (bFullCheck)
        { DbgAssert(DbgLeakOpt(this, eDbgLeakOptTypeCheck)); }
    }

    bool IsValid(TMBool bSameThread) const
    {
        if (eCheckValue != m_checkByte || !m_szClass)
            return false;
        if (bSameThread && !DbgLeakOpt(this, eDbgLeakOptTypeIsThread))
            return false;
        return true;
    }

//     bool HaveOther() const
//     {
//         return 0 != DbgLeakOpt(this, eDbgLeakOptTypeFindByStr);
//     }
};
template <class T> int CInnerDbgInfoT<T>::s_nCount;


template <class T> struct CInstanceCountT
{
    static int s_nCount;
    CInstanceCountT() { ++s_nCount; }
    ~CInstanceCountT() { --s_nCount; }
};
template <class T> int CInstanceCountT<T>::s_nCount;


#ifdef TM_MSC
#   pragma warning (pop)
#endif // #ifdef TM_MSC

// user can support static int IC_GetDebugInfo(_Tx* cls, char* pinfo);
#define TM_CLASS_INSTANCE_CHECK(T) \
    CInnerDbgInfoT<T> m_dbg_instanceCheck; \
    virtual void CheckInstance() { m_dbg_instanceCheck.Check(false); } \
    virtual bool IsValidInstance(TMBool bCheckThread = false) { return m_dbg_instanceCheck.IsValid(bCheckThread); } \
    virtual bool IsCreatorThread() { return IsValidInstance(true); } \
    virtual void _BlockInheirt() tm_final {} \
    friend CInnerDbgInfoT<T>;

// #define TM_CLASS_INSTANCE_SET_INFO(pthis,idorinfo)  pthis->m_dbg_instanceCheck.ResetT(pthis, idorinfo)

#define TM_CLASS_INSTANCE_COUNT(x)                  CInstanceCountT<x>  m_dbg_instanceCount;

#endif // #if defined(TM_CPP) && defined(TM_DBG)


#ifndef TM_CLASS_INSTANCE_CHECK
#   define TM_CLASS_INSTANCE_CHECK(T)               void CheckInstance() {}; bool IsValidInstance(TMBool) { return true; } bool IsCreatorThread() { return true; }
// #   define TM_CLASS_INSTANCE_SET_INFO(p,x)

#   define TM_CLASS_INSTANCE_COUNT(x)

#endif // TM_CLASS_INSTANCE_CHECK



///////////////////////////////////////////////////////////////////////////////
// class CInstanceCountT
// template <class T>
// struct CInstanceCountT
// {
//     static int s_nCount;
//
//     CInstanceCountT() { ++s_nCount; }
//     CInstanceCountT(const CInstanceCountT& rhs) { ++s_nCount; }
//     ~CInstanceCountT() { --s_nCount; }
// };
// template <class T> int CInstanceCountT<T>::s_nCount;
//
// #if defined(TM_CPP) && defined(TM_DBG)
// #   define TM_CLASS_INSTANCE_COUNT(T) CInstanceCountT<T> m_dbg_instanceCount; static int InstanceCount() { return CInstanceCountT<T>::s_nCount; }
//
// #else
// #   define TM_CLASS_INSTANCE_COUNT(T)
// #endif




TM_CAPI void __tm_this_is_debug_dll();



#endif /* end of " #ifndef TMDEBUG_H " */

