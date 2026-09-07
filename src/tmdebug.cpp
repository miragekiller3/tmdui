/**
 *  @file
 *  @brief  debug function 
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2002-09-10
 *
 *  Copyright (C) 2002 miragekiller                             
 */

#include "tmdebug.h"
#include "tmstring.h"
#include "tmtime.h"
#include "tmlist.h"
#include "tmfile.h"
#include "tmatomic.h"
#include "tmthread.h"
#include "tmdebugext.h"
#include "tmmessagequeue.h"
#include "tmmessagebox.h"
#include "tmfilesystem.h"
#include "tmdebugext.h"

#ifdef TM_QT
#   include <qglobal.h>
#endif

#ifdef TM_WIN
#   include <windows.h>
#elif TM_ANDROID
#   include <android/log.h>
#endif

#include <time.h>
#include <assert.h>



///////////////////////////////////////////////////////////////////////////////
// check at defined type
# pragma message("/************************************************************************/")
#ifdef TM_MSC
#   pragma warning (disable : 4003)     //not enough actual parameters for macro '_TM_LINK_OF'
#endif
# pragma message("/*********************** begin TM macro defined *************************/")

////////////////////////////////////////////////////////////////////
// complier

#ifdef TM_CLANG
#   pragma message("compiler: " "TM_CLANG(" TM_NUMBER_OF(TM_CLANG) ")")
#endif
#ifdef TM_INTEL
#   pragma message("compiler: " "TM_INTEL(" TM_NUMBER_OF(TM_INTEL) ")")
#endif
#ifdef TM_GNUC
#   pragma message("compiler: " "TM_GNUC(" TM_NUMBER_OF(TM_GNUC) ")")
#endif
#ifdef TM_BORLAND
#   pragma message("compiler: " "TM_BORLAND(" TM_NUMBER_OF(TM_BORLAND) ")")
#endif
#ifdef TM_MWERKS
#   pragma message("compiler: " "TM_MWERKS(" TM_NUMBER_OF(TM_MWERKS) ")")
#endif
#ifdef TM_MSC
#   pragma message("compiler: " "TM_MSC(" TM_NUMBER_OF(TM_MSC) ")")
#endif

////////////////////////////////////////////////////////////////////
// platform
#ifdef TM_GNU
#   pragma message("platform: " "TM_GNU")
#endif
#ifdef TM_QNX
#   pragma message("platform: " "TM_QNX")
#endif
#ifdef TM_ANDROID
#   pragma message("platform: " "TM_ANDROID")
#endif
#ifdef TM_LINUX
#   pragma message("platform: " "TM_LINUX")
#endif
#ifdef TM_BSD
#   pragma message("platform: " "TM_BSD")
#endif
#ifdef TM_SUN
#   pragma message("platform: " "TM_SUN")
#endif
#ifdef TM_CYGWIN
#   pragma message("platform: " "TM_CYGWIN")
#endif
#ifdef TM_WIN
#   pragma message("platform: " "TM_WIN")
#endif
#ifdef TM_WIN_DESKTOP
#   pragma message("platform: " "TM_WIN_DESKTOP")
#endif
#ifdef TM_WIN_PHONE
#   pragma message("platform: " "TM_WIN_PHONE")
#endif
#ifdef TM_WIN_METRO
#   pragma message("platform: " "TM_WIN_METRO")
#endif
#ifdef TM_WIN32
#   pragma message("platform: " "TM_WIN32")
#endif
#ifdef TM_WIN64
#   pragma message("platform: " "TM_WIN64")
#endif
#ifdef TM_APPLE
#   pragma message("platform: " "TM_APPLE")
#endif
#ifdef TM_MAC64
#   pragma message("platform: " "TM_MAC64")
#endif
#ifdef TM_MAC32
#   pragma message("platform: " "TM_MAC32")
#endif
#ifdef TM_IOS
#   pragma message("platform: " "TM_IOS")
#endif

#ifdef TM_UNICODE
#   pragma message("platform: " "TM_UNICODE")
#endif

#ifdef TM_PLATFORM64
#   pragma message("platform: " "TM_PLATFORM64")
#endif

#ifdef TM_QT
#   pragma message("platform: " "TM_QT(" TM_NUMBER_OF(TM_QT) ")")
#endif

#ifdef TM_GTK
#   pragma message("platform: " "TM_GTK(" TM_NUMBER_OF(TM_GTK) ")")
#endif

#ifdef TM_MINGW
#   pragma message("platform: " "TM_MINGW(" TM_NUMBER_OF(TM_MINGW) ")")
#endif


////////////////////////////////////////////////////////////////////
// CPU types
#ifdef TM_CPU_LE
#   pragma message("CPU type: " "TM_CPU_LE")
#endif
#ifdef TM_CPU_BE
#   pragma message("CPU type: " "TM_CPU_BE")
#endif
#ifdef TM_CPU_64
#   pragma message("CPU type: " "TM_CPU_64")
#endif
#ifdef TM_ARM
#   pragma message("CPU type: " "TM_ARM")
#endif
#ifdef TM_X86
#   pragma message("CPU type: " "TM_X86")
#endif
#ifdef TM_X64
#   pragma message("CPU type: " "TM_X64")
#endif
#ifdef TM_PPC
#   pragma message("CPU type: " "TM_PPC")
#endif
#ifdef TM_PPC64
#   pragma message("CPU type: " "TM_PPC64")
#endif
#ifdef TM_MIPS
#   pragma message("CPU type: " "TM_MIPS")
#endif
#ifdef TM_68K
#   pragma message("CPU type: " "TM_68K")
#endif
#ifdef TM_MPPC
#   pragma message("CPU type: " "TM_MPPC")
#endif
#ifdef TM_SH
#   pragma message("CPU type: " "TM_SH")
#endif


#pragma message("#define TM_CPP             " TM_NUMBER_OF(TM_CPP))
#pragma message("#define TM_INLINE          " TM_NUMBER_OF(TM_INLINE))
#pragma message("#define TMClass            " TM_NUMBER_OF(TMClass))
#pragma message("#define TM_Interface       " TM_NUMBER_OF(TM_Interface))
#pragma message("#define TM_API             " TM_NUMBER_OF(TM_API))
#pragma message("#define TM_CAPI            " TM_NUMBER_OF(TM_CAPI))
#pragma message("#define tm_override        " TM_NUMBER_OF(tm_override))
#pragma message("#define tm_final           " TM_NUMBER_OF(tm_final))
#pragma message("#define tm_constexpr(x)    " TM_NUMBER_OF(tm_constexpr(x)))
#pragma message("#define tm_noexcept        " TM_NUMBER_OF(tm_noexcept))
#pragma message("#define tm_for             " TM_NUMBER_OF(tm_for))
#pragma message("#define TM_TYPENAME        " TM_NUMBER_OF(TM_TYPENAME))
#pragma message("#define TM_TEMPLATE_NULL   " TM_NUMBER_OF(TM_TEMPLATE_NULL))
#pragma message("#define tm_deprecated(x)   " TM_NUMBER_OF(tm_deprecated(x)))
#pragma message("#define tm_noreturn        " TM_NUMBER_OF(tm_noreturn))
#pragma message("#define tm_fallthrough     " TM_NUMBER_OF(tm_fallthrough))
#pragma message("#define tm_nodiscard       " TM_NUMBER_OF(tm_nodiscard))
#pragma message("#define tm_maybe_unused    " TM_NUMBER_OF(tm_maybe_unused))
#pragma message("#define tm_no_unique_addr  " TM_NUMBER_OF(tm_no_unique_addr))
#pragma message("#define tm_likely          " TM_NUMBER_OF(tm_likely))
#pragma message("#define tm_unlikely        " TM_NUMBER_OF(tm_unlikely))
#pragma message("#define tm_unused(x)       " TM_NUMBER_OF(tm_unused(x)))
#pragma message("#define tm_assume(x)       " TM_NUMBER_OF(tm_assume(x)))
#pragma message("#define tm_thread_local    " TM_NUMBER_OF(tm_thread_local))


// #if (TM_TYPENAME
// #   pragma message("#define TM_TYPENAME  " TM_NUMBER_OF(TM_TYPENAME))
// #else
// #   pragma message("#define TM_TYPENAME")
// #endif
#pragma message("#define TM_UCHAR_MAX       " TM_NUMBER_OF(TM_UCHAR_MAX))
#pragma message("#define TM_USHRT_MAX       " TM_NUMBER_OF(TM_USHRT_MAX))
#pragma message("#define TM_UINT_MAX        " TM_NUMBER_OF(TM_UINT_MAX))
#pragma message("#define TM_ULONG_MAX       " TM_NUMBER_OF(TM_ULONG_MAX))
#pragma message("#define TM_LLONG           " TM_NUMBER_OF(TM_LLONG))
#pragma message("#define TM_ULLONG_MAX      " TM_NUMBER_OF(TM_ULLONG_MAX))
#ifdef TM_WCHAR
#   pragma message("#define TM_WCHAR           " TM_NUMBER_OF(TM_WCHAR))
#else
#   pragma message("#undef  TM_WCHAR           (wchar_t is not stand type)")
#endif
#pragma message("#define TM_WCHAR_MAX       " TM_NUMBER_OF(TM_WCHAR_MAX))


#pragma message("#define TMInt              " TM_NUMBER_OF(TMInt))
#pragma message("#define TMUInt             " TM_NUMBER_OF(TMUInt))
#pragma message("#define TMShort            " TM_NUMBER_OF(TMShort))
#pragma message("#define TMUShort           " TM_NUMBER_OF(TMUShort))
#pragma message("#define TMLong             " TM_NUMBER_OF(TMLong))
#pragma message("#define TMULong            " TM_NUMBER_OF(TMULong))
//#pragma message("#define TMULLong           " TM_NUMBER_OF(TMULLong))
#pragma message("#define TMChar             " TM_NUMBER_OF(TMChar))
#pragma message("#define TMUChar            " TM_NUMBER_OF(TMUChar))

#pragma message("#define TM_INT8            " TM_NUMBER_OF(TM_INT8))
#pragma message("#define TM_UINT8_MAX       " TM_NUMBER_OF(TM_UINT8_MAX))
#pragma message("#define TM_INT16           " TM_NUMBER_OF(TM_INT16))
#pragma message("#define TM_UINT16_MAX      " TM_NUMBER_OF(TM_UINT16_MAX))
#pragma message("#define TM_INT32           " TM_NUMBER_OF(TM_INT32))
#pragma message("#define TM_UINT32_MAX      " TM_NUMBER_OF(TM_UINT32_MAX))
#pragma message("#define TM_INT64           " TM_NUMBER_OF(TM_INT64))
#pragma message("#define TM_UINT64_MAX      " TM_NUMBER_OF(TM_UINT64_MAX))

#pragma message("#define TMIntPtr           " TM_NUMBER_OF(TMIntPtr))
#pragma message("#define TMUIntPtr          " TM_NUMBER_OF(TMUIntPtr))
#pragma message("#define TMLongPtr          " TM_NUMBER_OF(TMLongPtr))
#pragma message("#define TMULongPtr         " TM_NUMBER_OF(TMULongPtr))
#pragma message("#define TMWParam           " TM_NUMBER_OF(TMWParam))
#pragma message("#define TMLParam           " TM_NUMBER_OF(TMLParam))
#pragma message("#define TMResult           " TM_NUMBER_OF(TMResult))

#pragma message("#define TMCStr             " TM_NUMBER_OF(TMCStr))
#pragma message("#define TMFilename         " TM_NUMBER_OF(TMFilename))


#ifdef TM_PROTECTED
#   pragma message("#define TM_PROTECTED")
#else
#   pragma message("#undef  TM_PROTECTED       (not support protected mode)")
#endif
#ifdef TM_DBG
#   pragma message("#define TM_DBG             " TM_NUMBER_OF(TM_DBG))
#else
#   pragma message("#undef  TM_DBG")
#endif

#pragma message("#define TM_MB_MAX          " TM_NUMBER_OF(TM_MB_MAX))
#pragma message("#define PATH_MAX           " TM_NUMBER_OF(PATH_MAX))

#pragma message("#define StrVPrintfW        " TM_NUMBER_OF(StrVPrintfW))
#pragma message("#define StrPrintfA         " TM_NUMBER_OF(StrPrintfA))



# pragma message("/************************ end TM macro defined **************************/")
#ifdef TM_MSC
#   pragma warning (default : 4003)     //not enough actual parameters for macro '_TM_LINK_OF'
#endif
# pragma message("/************************************************************************/")





// check uintx size
DbgAssertSx (_test_int8_size,   sizeof(TMInt8)   == 1);
DbgAssertSx (_test_uint8_size,  sizeof(TMUInt8)  == 1);

DbgAssertSx (_test_int16_size,  sizeof(TMInt16)  == 2);
DbgAssertSx (_test_uint16_size, sizeof(TMUInt16) == 2);

DbgAssertSx (_test_int32_size,  sizeof(TMInt32)  == 4);
DbgAssertSx (_test_uint32_size, sizeof(TMUInt32) == 4);

#ifdef TM_INT64
    DbgAssertSx (_test_int64_size,  sizeof(TMInt64)  == 8);
    DbgAssertSx (_test_uint64_size, sizeof(TMUInt64) == 8);
#endif // end of #ifdef TM_INT64

DbgAssertSx (_test_wchar_t_size,  sizeof(wchar_t)  == 2 || sizeof(wchar_t)  == 4);
DbgAssertSx (_test_ATCharW_size,  sizeof(TMCharW)  == sizeof(wchar_t));



#ifdef TM_DBG


//sys/time.h
//int gettimeofday (struct timeval *tp, struct timezone *tzp)

TM_INLINE int _DbgGetThreadSelf()
{
    return (int)(TMLongPtr)(void*)TM_SW_WIN(GetCurrentThreadId, pthread_self)();
}

TM_INLINE void _DbgBreak()
{
    TM_SW_WIN(DebugBreak(), assert(0));
}

///////////////////////////////////////////////////////////////////////////////
// _KaWin
// use hot key to open/close trace.
struct _KaWin
    : public TM::IDebugHotkeySink
{
    typedef TM::CDebugHotKeyT<VK_F2>                    CDebugHotKey;
    bool bKeyPress;

    _KaWin()
    {
        CDebugHotKey::AddSink(this);
    }

    ~_KaWin()
    {
        CDebugHotKey::DelSink(this);
    }

    virtual void Flip() tm_override
    {
        bKeyPress = !bKeyPress;
    }
};

///////////////////////////////////////////////////////////////////////////////
// CDebugTrace
struct CDebugTrace
{
    // normal message box only block call, can't block thread;
    // so I use thread create message box, and send message to block thread.
#ifdef TM_WIN
    static void _DoAssert(const std::tstring& str)
    {
        int b = MessageBox(0, str.c_str(), TM_T("assert"), MB_OKCANCEL | MB_DEFAULT_DESKTOP_ONLY);
        if (IDOK == b)
            _DbgBreak();
    };

#elif defined (TM_APPLE)
    struct CAssertWin
        : public TM::CThread
        , public TM::CMessageQueueT<CAssertWin>
    {
    public:
        typedef TM::CMessageBoxNative CAlert;

        TMResult WndProc(TMUInt uMsg, TMWParam wParam, TMLParam lParam)
        {
            if (uMsg == WM_USER)
                return CAlert::done(0, (TMCStr)lParam, TM_T("assert"), CAlert::eTypeOkCancel);
            return message_queue_class::WndProc(uMsg, wParam, lParam);// CRefWindow::WndProc(uMsg, wParam, lParam);
        }

        // IThread
        virtual unsigned ThreadProc() tm_override
        {
            message_queue_class::Create();
            this->Exec();
            return 0;
        }

        CAssertWin()
        {
            thread_class::Create();
        }

        ~CAssertWin()
        {
            this->Exit();
            this->Join();
        }

        void Assert(const std::tstring& str)
        {
            if (IsNull())
                return;
            if (CAlert::eResultOk == this->SendMessage(WM_USER, 0, (TMLParam)str.c_str()))
                _DbgBreak();
        }
    };

    static void _DoAssert(const std::tstring& str)
    {
        CAssertWin as;
        as.Assert(str);
    }

#else
    static void _DoAssert(const std::tstring& str)
    {
#ifdef TM_QT
        qt_assert_x("TM", TM_T2A(str), "", 0);
#endif
    };

#endif // #ifdef TM_QT #else

    typedef TM::CSet<ITraceHook*> CTraceHookList;

    bool                m_bDisableNormal;
    bool                m_bDisableAssert;
    bool                m_bConsole;
    TMCharT             m_fn[2048];
    TM::CAtomicInt      m_indent;
    CTraceHookList      m_aSink;
    static CDebugTrace* s_trace;

    CDebugTrace()
        : m_bDisableNormal(0)
        , m_bDisableAssert(0)
        , m_bConsole(0)
    {
        m_fn[0] = 0;
        s_trace = this;
    }

    ~CDebugTrace()
    {
        s_trace = 0;
    }

    void MakeTraceStr(std::tstring& strCache, int tp, TMCStr fmt)
    {
        if (fmt)
        {
            int n = m_indent;
            if (tp & e_tpAddIndent)
                n = (++m_indent) - 1;
            else if (tp & e_tpDecIndent)
            {
                n = --m_indent;
                tm_setmax(n, 0);
            }

            strCache.append(n * 2, ' ');

            if (!(e_tpNoContextInfo & tp))
            {
                TM::CTime time(TM::CTime::GetCurrentTime());
                tm _tm;
                time.GetLocalTm(_tm);

                TMCharT c[1024];
                StrPrintf(
                    c, tm_countof(c),
                    TM_T("%4d-%02d-%02d %02d:%02d:%02d.%03d [%d] "),
                    1900 + _tm.tm_year, _tm.tm_mon + 1, _tm.tm_mday,
                    _tm.tm_hour, _tm.tm_min, _tm.tm_sec, clock() % CLOCKS_PER_SEC,
                    _DbgGetThreadSelf());
                strCache += c;
            }

            strCache += fmt;
        }

        if (!(tp & e_tpNoEOF))
            strCache += TM_T("\r\n");
    }

    void VTraceNormal(const std::tstring& str, int tp)
    {
#if TM_WIN
        OutputDebugString(str.c_str());
//         Sleep(1);
#elif TM_ANDROID
        __android_log_print((android_LogPriority)(tp & e_tpPriorityMask), "", str.c_str());
#else
        printf("%s", str.c_str());
#endif
    }

    void VTraceF(const std::tstring& s, int tp)
    {
        if (!m_fn[0])
            return;
        TM::CFile f;
        f.OpenAlways(m_fn, f.e_faWrite);
        if (!f)
            return;
        TM::CStdStrA str(s);
        f.SeekEnd(0);
        f.Write(str, str.GetLength());
    }

    void VtraceConsole(const std::tstring& str, int tp)
    {
#ifdef TM_UNICODE
        _tprintf(TM_T("%s"), str.c_str());
#else
        printf("%s", str.c_str());
#endif
    }

    void VTrace(int tp, TMCStr fmt)
    {
        if (tp & e_tpHotKey)
        {
            static _KaWin _kw;
            if (!_kw.bKeyPress)
                return;
        }

        std::tstring str;
        MakeTraceStr(str, tp, fmt);

        if (!(tp & e_tpSpy))
        {
            if (!m_bDisableNormal)
                VTraceNormal(str, tp);
            if (m_bConsole)
                VtraceConsole(str, tp);
            VTraceF(str, tp);
        }

        if (!m_bDisableNormal || m_bConsole || !m_bDisableAssert || m_fn[0])
        {
            for (CTraceHookList::recorder rec(m_aSink); rec; ++rec)
            {
                ITraceHook* p = *rec;//rec.get_data();
                p->OnTrace(tp, str.c_str());
            }
        }

        if ((tp & e_tpPriorityMask) == e_tpAssert)
        {
            if (!m_bDisableAssert)
                _DoAssert(str);
        }
        else if ((tp & e_tpPriorityMask) == e_tpBreak)
            _DbgBreak();
    }
};

CDebugTrace*   CDebugTrace::s_trace;



///////////////////////////////////////////////////////////////////////////////
// CTraceStr
class CTraceStr : public ITrace
{
private:
    ~CTraceStr() {};

public:
    std::tstring m_str;

    virtual void Release() tm_override { delete this; }

    void _Append(TMCStr s)
    {
        m_str += s;
    }

    template <class T>
    void _Append(const T* s, ...)
    {
        TMCharT c[2048] = {0};
        StrConvert(0, c, 2048, s, StrLen(s));
        _Append(c);
    }

    virtual void Append(TMCStrA fmt, va_list vl) tm_override
    {
        if (vl)
        {
            TMCharA c[2048];
            StrVPrintf(c, 2048, fmt, vl);
            _Append(c);
        }
        else
            _Append(fmt);
    }

    virtual void Append(TMCStrW fmt, va_list vl) tm_override
    {
        if (vl)
        {
            TMCharW c[2048];
            StrVPrintf(c, 2048, fmt, vl);
            _Append(c);
        }
        else
            _Append(fmt);
    }

    virtual void AppendLastError(TMCStrA fmt) tm_override
    {
#ifdef TM_WIN
        char d[2048];
        d[0] = 0;
        DWORD nError = GetLastError();
        int n = wsprintfA(d, "%d,", nError);
        FormatMessageA(FORMAT_MESSAGE_MAX_WIDTH_MASK | FORMAT_MESSAGE_FROM_SYSTEM, NULL, nError, 0, &(d[n]), tm_countof(d) - n, NULL);
        if (!fmt)
            _Append(d);
        else
        {
            char e[2048];
            StrPrintf(e, 2048, fmt, d);
            _Append(e);
        }
#else
        _Append(fmt);
#endif // TM_WIN
    }

    virtual void Output(int tp) tm_override
    {
        if (CDebugTrace::s_trace)
            CDebugTrace::s_trace->VTrace(tp, m_str.c_str());
        else if ((tp & e_tpPriorityMask) == e_tpAssert)
            CDebugTrace::_DoAssert(m_str);
    }
};



///////////////////////////////////////////////////////////////////////////////
// Lead Check
struct CDebugLeak : public TM::IDebugHotkeySink
{
//     typedef TM::CSet<const TMDbgClassInfo*>                     CSnapList;
//     typedef TM::CAutoLockT<TM::CCritical>                       CAutoLock;
    typedef TM::CDebugHotKeyT<TM_HOTKEY_EXT>                    CDebugHotKey;
//     TM::CCritical               m_critical;
    CDebugLeakInfoList          m_aInfo;

#   define DebugLeak_Trace(x,p) DbgTrace(x ": %p - %s:%d\r\n", p, p->m_pTypeName, p->m_id)

    CDebugLeak()
    {
    }

    void Init()
    {
        CDebugHotKey::AddSink(this);
    }

    void LeakClean()
    {
        CDebugHotKey::DelSink(this);
        if (m_aInfo.size())
        {
            for (CDebugLeakInfoList::recorder it(m_aInfo); it; ++it)
            {
                const TMDbgClassInfo* pi = it.get_data();
                DebugLeak_Trace("CDebugLeak find object leaks", pi);
                 ThreadSleep(1);
            }
            DbgTrace("CDebugLeak::~CDebugLeak();");
        }
        else
            DbgTrace ("CDebugLeak::~CDebugLeak(); no object leaks");
    }

//     static CDebugLeak& Inst()
//     {
//         static CDebugLeak s_leak;
//         return s_leak;
//     }

    void Dump(CMemDump* dumpOld, CMemDump* dumpNew, int& nDebugMem)
    {
//        CAutoLock _lock(m_critical);
        CDbgMemAutoLock _lock;
        tm_for (CDebugLeakInfoList::recorder rec(m_aInfo); rec; ++rec)
        {
            const TMDbgClassInfo* p = rec.get_data();
            CMemDump* dump = p->m_bDumped ? dumpOld : dumpNew;
            if (!dump)
                continue;

            int nCls = TMDbgClassInfo_GetClassSize(p);
            int nOut = TMDbgClassInfo_GetExtSize(p);
            int nDbg = p->m_szDebug ? p->m_szDebug : sizeof(TMDbgClassInfo);
#ifdef TM_QTSPY
            if (p->m_eType != p->eTypeQT)
            {
                nDebugMem += (nCls + nDbg + nOut);
                continue;
            }
#endif
            CMemDump::value& v = dump->aMap[CMemDump::key(p->m_pTypeName, nCls)];
            ++v.first;
            v.second += (nCls + nOut);
            ++dump->nAllCount;
            nDebugMem += nDbg;
        }
    }

    TMDbgClassInfo* GetCallStack()
    {
#if defined(TM_WIN) && TM_DBG_CAPTURE_CALLSTATCK
        static tm_thread_local TMDbgClassInfo cs = { "call stack", FILE_AND_LINE };
        cs.m_sizeCallStack = _dbgApp.GetCallStack(TMDbgClassInfo::eStackSize, &(cs.m_addrCallStack[0]));
        return &cs;
#else
        return 0;
#endif // TM_WIN
    }

    const TMDbgClassInfo* Done(const TMDbgClassInfo* p, EDbgLeakOptType eType)
    {
        static tm_thread_local const TMDbgClassInfo* pre = 0;

//        CAutoLock _lock(m_critical);
        CDbgMemAutoLock _lock;
        switch (eType)
        {
        case eDbgLeakOptTypeDef:
            {
                const TMDbgClassInfo* old = pre;
                pre = p;
                if (!p && !old)
                    return GetCallStack();
                return old;
            }

        case eDbgLeakOptTypeAdd:
            DbgAssert(p->m_pTypeName);
            DbgAssert(p->m_pClass);
            if (!p->m_idThread)
                ((TMDbgClassInfo*)p)->m_idThread = _DbgGetThreadSelf();
            DbgAssert(!Done(p, eDbgLeakOptTypeCheck));
            m_aInfo.insert(p);
            return p;
            
        case eDbgLeakOptTypeRemove:
        case eDbgLeakOptTypeCheck:
        case eDbgLeakOptTypeIsThread:
        case eDbgLeakOptTypeSetID:
//         case eDbgLeakOptTypeTraceByStr:
            {
                CDebugLeakInfoList::recorder it(m_aInfo);
                it.find(p->m_pClass);
                if (!it)
                {
                    if (eType == eDbgLeakOptTypeRemove)
                        DebugLeak_Trace("CDebugLeak not find objects", p);
                    return 0;
                }
                const TMDbgClassInfo* px = it.get_data();
                DbgAssert(px->eCheckValue == px->m_checkByte);
                if (px->m_eType == px->eTypeMemTM)
                    DbgAssert(*((TMDbgClassInfo::ECheckValue*)TMDbgClassInfo_Tail(px)) == TMDbgClassInfo::eCheckValue);
                if (eType == eDbgLeakOptTypeRemove)
                    it.erase();
                else if (eType == eDbgLeakOptTypeIsThread && px->m_idThread != _DbgGetThreadSelf())
                    return 0;
                else if (eType == eDbgLeakOptTypeSetID)
                    ((TMDbgClassInfo*)px)->m_id = p->m_id;
                return px;
            }
            return 0;

//         case eDbgLeakOptTypeIsThread:
//             return p->m_idThread == _DbgGetThreadSelf() ? p : 0;
// 
//         case eDbgLeakOptTypeFindByStr:
//             if (m_aInfo.count(p->m_cInfo) <= 1)
//                 return 0;
//             Done(p, eDbgLeakOptTypeTraceByStr);
//             return p;

        case eDbgLeakOptTypeFindByMem:
            {
                for (CDebugLeakInfoList::recorder rec(m_aInfo); rec; ++rec)
                {
                    const TMDbgClassInfo* px = rec.get_data();
                    if (TMDbgClassInfo_IsInner(px, p))
                        return px;
                }
                return GetCallStack();
            }
            return 0;
        }

        DbgAssert(0);
        return 0;
    }

    virtual void Flip() tm_override
    {
//        CAutoLock _lock(m_critical);
        CDbgMemAutoLock _lock;

        TM::CDebugCSVFile csvfile(TM_T("c:\\temp\\TMDui Leak Check"));
        csvfile.Append("thread,address,size,object,info\r\n");

        tm_for (CDebugLeakInfoList::recorder it(m_aInfo); it; ++it)
        {
            TMDbgClassInfo* p = const_cast<TMDbgClassInfo*>(it.get_data());
            if (p->m_bDumped)
                continue;
            p->m_bDumped = true;
#ifdef TM_QTSPY
            if (p->m_eType != p->eTypeQT)
                continue;
#endif

            char cdinfo[256] = {0};
            TMDbgClassInfo_GetExtSize(p, cdinfo);
            
            TM::CStdStrA strx;
            strx.Format("%d,0x%p,%d,\"%s:%d\",\"%s\"\r\n",
                p->m_idThread, p, TMDbgClassInfo_GetRealSize(p), p->m_pTypeName, p->m_id, &cdinfo);
            csvfile.Append(strx);
        }
    }
};



#endif // TM_DBG



#if defined(TM_DBG)

#ifdef TM_WIN

class CDebugMem : public IDebug
{
public:
    HANDLE      m_heap;
    size_t      m_nAllMem;

    CDebugMem()
        : m_nAllMem(0)
        , m_heap(HeapCreate(0, 10000000, 0))
    {
    }
    
    ~CDebugMem()
    {
        HeapDestroy(m_heap);
    }

    virtual char* DbgMemAlloc(size_t n) tm_final { m_nAllMem += n; return (char*)HeapAlloc(m_heap, 0, n); }
    
    virtual void  DbgMemFree(void* p) tm_final { m_nAllMem -= HeapSize(m_heap, 0, p); HeapFree(m_heap, 0, p); }

    virtual size_t DbgMemSize(void* p) tm_final { return HeapSize(m_heap, 0, p); }

    virtual void DbgMemLock() tm_final { HeapLock(m_heap); }
    
    virtual void DbgMemUnlock() tm_final { HeapUnlock(m_heap); }
    
    virtual bool DbgIsMemHeap(void* hHeap) tm_final { return hHeap == m_heap; }
    
    virtual int GetCallStack(int nFrames, void** ppBackTrace, int nSkip) tm_final
    {
        DECLARE_DLL_FUNC(dll_ntdll, WORD, WINAPI, RtlCaptureStackBackTrace,
            (DWORD FramesToSkip, DWORD FramesToCapture, PVOID* BackTrace, PDWORD BackTraceHash));
        if (!_RtlCaptureStackBackTrace)
            return 0;
        return _RtlCaptureStackBackTrace(nSkip, nFrames, ppBackTrace, NULL);
    }
};

#else
class CDebugMem : public IDebug
{
// public: 
//     virtual char* DbgMemAlloc(size_t n) tm_final { return 0; }
//     virtual void  DbgMemFree(void* p) tm_final {}
//     virtual size_t DbgMemSize(void* p) tm_final { return 0; }
//     virtual void DbgMemLock() tm_final { }
//     virtual void DbgMemUnlock() tm_final {}
//     virtual bool DbgIsMemHeap(void* hHeap) tm_final { return false; }
//     virtual int GetCallStack(int nFrames, void** ppBackTrace, int nSkip) tm_final { return 0; }
};
#endif // TM_WIN

class _CDebug : public CDebugMem
{
public:
    CDebugTrace m_trace;
    CDebugLeak  m_leak;

public:
    _CDebug()
    {
        m_leak.Init();
    }

    ~_CDebug()
    {
    }

    virtual CDebugLeakInfoList* LeakGetBegin() tm_final
    {
        DbgMemLock();
        CDebugLeak& dl = m_leak;// CDebugLeak::Inst();
        //dl.m_critical.Lock();
        return &dl.m_aInfo;
    }

    virtual void LeakGetEnd() tm_final
    {
        DbgMemUnlock();
//        CDebugLeak& dl = m_leak;// CDebugLeak::Inst();
//        dl.m_critical.Unlock();
    }

    virtual void LeakFlip() tm_final
    {
        CDebugLeak& dl = m_leak;// CDebugLeak::Inst();
        dl.Flip();
    }

    virtual void LeakDump(CMemDump* dumpOld, CMemDump* dumpNew, int& nDebugMem) tm_final
    {
        CDebugLeak& dl = m_leak;// CDebugLeak::Inst();
        dl.Dump(dumpOld, dumpNew, nDebugMem);
    }

    virtual const TMDbgClassInfo* LeakOpt(const TMDbgClassInfo* p, EDbgLeakOptType eType) tm_final
    {
        TM_ANALYSIS_UNUSED(aa);
        if (p && (const void*)p->m_pClass == this)
            return p;
        return m_leak.Done(p, eType); 
    }

    virtual ITrace* TraceCreate() tm_final
    {
        return new CTraceStr();
    }
    
    virtual void TraceAddHook(ITraceHook* p) tm_final
    {
        m_trace.m_aSink.insert(p);
    }

    virtual void TraceDelHook(ITraceHook* p) tm_final
    {
        m_trace.m_aSink.erase(p);
    }

    virtual bool Set(TMBool bEnable) tm_final
    {
        bool b = m_trace.m_bDisableAssert;
        m_trace.m_bDisableAssert = !bEnable;
        return b;
    }

    virtual bool TraceSetToSys(TMBool bEnable) tm_final
    {
        bool b = m_trace.m_bDisableNormal;
        m_trace.m_bDisableNormal = !bEnable;
        return b;
    }

    virtual bool TraceSetToCon(TMBool bEnable) tm_final
    {
        bool b = m_trace.m_bConsole;
        m_trace.m_bConsole = !!bEnable;
        return b;
    }

    virtual void TraceSetToFile(TMCStr file) tm_final
    {
        m_trace.m_fn[0] = 0;
        if (!file)
            return;
        StrCopy(m_trace.m_fn, tm_countof(m_trace.m_fn), file);
        FsRemove(m_trace.m_fn);
    }

};



class CDebug : public TM::CStaticBaseRefT2<CDebug, true, _CDebug, true>
{
public:
    static void CStaticBaseRefT_OnExit()
    {
        CheckGetInstanceNoAddRef()->m_leak.LeakClean();
        ref_class::CStaticBaseRefT_OnExit();
    }
};

#endif

IDebug* DbgGetApp()
{
#ifdef TM_DBG
    return CDebug::Instance();
#else
    static IDebug s_dbg;
    return &s_dbg;
#endif
}


TM_IF_DBG(void __tm_this_is_debug_dll(){})

