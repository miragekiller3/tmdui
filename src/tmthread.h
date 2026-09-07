/**
 *  @file
 *  @brief  thread operator<br>
 *  you can use these function & class in platform win32 and posix.<br>
 *  these classes have the same behaviors on different platforms.
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2002.12.21
 *
 *  Copyright (C) 2002 miragekiller
 */

#pragma once

#ifndef TMTHREAD_H
#define TMTHREAD_H

#include "tmio.h"
#include "tmatomic.h"

#ifdef TM_WIN
#   include <process.h>
#   include "tmthread_win.h"
#else
#   include "tmthread_gnu.h"
#endif

// #ifndef TMNATIVE_H
// #   include "tmnative.h"
// #endif

#ifdef TM_QT
#   include <QThread>
#endif


// #include <functional>


/**
 *  @defgroup thread
 *  @defgroup synchronization
 *
 *      @defgroup critical
 *      @ingroup thread synchronization
 *
 *      @defgroup mutex
 *      @ingroup thread synchronization
 *
 *      @defgroup event
 *      @ingroup thread synchronization
 *
 *      @defgroup state
 *      @ingroup thread synchronization
 */



// for doxygen
#ifdef TM_DOXYGEN

/** */
enum ESyncRet;

/**
 *  @addtogroup critical
 *  nested critical "c" function prototype.
 *  one thread can enter the same critical section many times,
 *  of course need to leave for the same times.
 *  @see CCritical
 *  @{
 */
struct TMCritical;

/** see InitializeCriticalSection in MSDN */
void CriticalCreate(TMCritical* pStruct);

/** see TryEnterCriticalSection in MSDN */
ESyncRet CriticalTryLock(TMCritical* pStruct);

/**
 *  see EnterCriticalSection in MSDN
 *  @remark only one thread can lock critical when it unlock.
 */
void CriticalLock(TMCritical* pStruct);

/**
 *  see LeaveCriticalSection in MSDN
 *  @remark only owner can unlock critical
 */
void CriticalUnlock(TMCritical* pStruct);

/** see DeleteCriticalSection in MSDN */
void CriticalDestroy(TMCritical* pStruct);

/** @} */


/**
 *  @addtogroup mutex
 *  un_nested mutex "c" function prototype.
 *  the same thread cannot lock the same mutex more than once.
 *  when user call "lock" 2nd in same thread, the thread will block.
 *  @see CMutex
 *  @{
 */
struct TMMutex;

/** see CreateEvent (0, 0, 1, 0) in MSDN */
void MutexCreate(TMMutex* pStruct);

/** see WaitForSingleObject in MSDN */
ESyncRet MutexTryLock(TMMutex* pStruct);

/**
 *  see WaitForSingleObject in MSDN
 *  @remark only one thread can lock mutex when it unlock
 */
void MutexLock(TMMutex* pStruct);

/**
 *  see SetEvent in MSDN
 *  @remark Only the thread that acquires the mutex can release it.
 */
void MutexUnlock(TMMutex* pStruct);

/** see CloseHandle in MSDN */
void MutexDestroy(TMMutex* pStruct);

/** @} */


/**
 *  @addtogroup event
 *  event "c" function prototype.
 *  this is a windows auto-reset event object
 *  @see CEvent
 *  @{
 */
struct TMEvent;

/** see CreateEvent (0, 0, 0, 0) in MSDN */
void EventCreate(TMEvent* pStruct);

/**
 *  wait for an event -- wait for an event object
 *  to be set to signaled.  Must be paired with a
 *  call to reset within the same thread.<br>
 *  see WaitForSingleObject in MSDN
 *  @see EventTimeLock
 */
void EventLock(TMEvent* pStruct);

/**
 *  The function returns when one of the following occurs:
 *  - The specified object is in the signaled state.
 *  - The time-out interval elapses.
 *  see WaitForSingleObject in MSDN
 *  @see EventLock
 */
ESyncRet EventTimeLock(TMEvent* pStruct, unsigned long dwMilliseconds);

/**
 *  set an event to signaled, let one of waiting thread through and then unsigned.
 *  see PulseEvent in MSDN.
 */
void EventSignal(TMEvent* pStruct);

/** see CloseHandle in MSDN */
void EventDestroy(TMEvent* pStruct);

/** @} */


/**
 *  @addtogroup state
 *  state "c" function prototype.
 *  state is a windows manual-reset event.<br>
 *  when state is signaled or goto signaled,
 *  the wait function return, the CState keep state after Lock().
 *  @see CState
 *  @{
 */
struct TMState;

/** see CreateEvent(0, 1, 0, 0) in MSDN */
void StateCreate(TMState* pStruct, TMBool bSignaled);

/**
 *  wait for an event -- wait for an event object
 *  to be set to signaled.  Must be paired with a
 *  call to reset within the same thread.<br>
 *  see WaitForSingleObject in MSDN.
 *  @see StateTimeLock
 */
void StateLock(TMState* pStruct);

/**
 *  The function returns when one of the following occurs:
 *  - The specified object is in the signaled state.
 *  - The time-out interval elapses.
 *  see WaitForSingleObject in MSDN.
 *  @see StateLock
 */
ESyncRet StateTimeLock(TMState* pStruct, unsigned long dwMilliseconds);

/**
 *  set an event to signaled, make all thread through.
 *  _|---
 *  see SetEvent in MSDN.
 */
void StateSignal(TMState* pStruct);

/**
 *  signal event, make all waiting thread through. then un_signal event.
 *  _|---|
 *  see PulseEvent in MSDN.
 */
void StatePulse(TMState* pStruct);

/**
 *  reset an event flag to un_signaled.
 *  see ResetEvent in MSDN.
 */
void StateUnsignal(TMState* pStruct);

/** */
TMBool StateIsSignaled(TMState* pStruct);

/** see CloseHandle in MSDN */
void StateDestroy(TMState* pStruct);

/** @} */

#endif // #ifdef TM_DOXYGEN



/**
 *  @addtogroup thread
 *  thread "c" function prototype
 *  @see CThreadID
 *  @{
 */
#ifdef TM_WIN

typedef struct
{
    HANDLE      hThread;
    unsigned    dwID;
} TMThread;

#   define TMThreadID                   DWORD
#   define TM_DECALRE_THREAD_PROC(f)    DWORD WINAPI f (LPVOID pArg)
#   define TM_THREAD_PROC               LPTHREAD_START_ROUTINE
#   define TM_THREAD_PROC_RET           DWORD

#else

#   define TMThread                     pthread_t
#   define TMThreadID                   pthread_t
#   define TM_DECALRE_THREAD_PROC(f)    void* f (void* pArg)
    typedef void* (*TM_THREAD_PROC)(void* pArg);
#   define TM_THREAD_PROC_RET           void*

#endif

/** init a thread struct */
TM_INLINE void ThreadInit(TMThread* pStruct)
{
    DbgAssert(pStruct);
    TM_IF_WIN (pStruct->dwID = 0; pStruct->hThread = 0);
    TM_NO_WIN (*pStruct = 0);
}

/** get current thread */
TM_INLINE void ThreadSelf(TMThread* pStruct)
{
    DbgAssert(pStruct);
#if (TM_WIN)
    pStruct->hThread = GetCurrentThread();
    pStruct->dwID = GetCurrentThreadId();
#else
    *pStruct = pthread_self();
#endif
}

/** get current thread id */
TM_INLINE TMThreadID ThreadSelfId()
{
#if (TM_WIN)
    return GetCurrentThreadId();
#else
    return pthread_self();
#endif
}

/** check thread struct is valid */
TM_INLINE TMBool ThreadValid(const TMThread* pStruct)
{
    DbgAssert(pStruct);
    return TM_SW_WIN (pStruct->dwID, 0 != *pStruct);
}

/** check thread is alive(not exist) */
TM_INLINE TMBool ThreadIsAlive(const TMThread* pStruct)
{
    DbgAssert(ThreadValid (pStruct));
#ifdef TM_WIN
    DWORD dwExitCode = 0;
    GetExitCodeThread(pStruct->hThread, &dwExitCode);
    return dwExitCode == STILL_ACTIVE;
#else
    int ret = pthread_kill(*pStruct, 0);
    DbgAssert(ret != EINVAL);
    return ret != ESRCH;
#endif
}

/** check thread is equal */
TM_INLINE TMBool ThreadEqual(const TMThread* ps1, const TMThread* ps2)
{
    DbgAssert(ps1 && ps2);
    TM_IF_WIN (return ps1->hThread == ps2->hThread && ps1->dwID == ps2->dwID);
    TM_NO_WIN (return pthread_equal(*ps1, *ps2));
}

/** check thread is equal current thread */
TM_INLINE TMBool ThreadEqualCurrent(const TMThread* ps1)
{
    TMThread sNow;
    ThreadSelf(&sNow);
    return ThreadEqual(ps1, &sNow);
}

/** see CreateThread in MSDN */
TM_INLINE void ThreadCreate(TMThread* pStruct, TM_THREAD_PROC pfn, void* param)
{
    DbgAssert(pStruct);
#ifdef TM_WIN
    DbgAssert(!ThreadValid(pStruct));
#endif
// #if defined(TM_WIN) && defined(_MT)
//     pStruct->hThread = (HANDLE)_beginthreadex (0, 0,
//         (unsigned (__stdcall *)(void *))pfn, param, 0, &pStruct->dwID);
// #elif defined(TM_WIN) && !defined(_MT)
#if defined(TM_WIN)
    pStruct->hThread = CreateThread(0, 0, pfn,  param, 0, (PULONG)&pStruct->dwID);
#else
    pthread_create (pStruct, 0, pfn, param);
#endif
}

/** wait thread end and release thread resource */
TM_INLINE int ThreadJoin(TMThread* pStruct)
{
#ifdef TM_WIN
    DWORD ret;

    DbgAssert(pStruct);
    if (!pStruct->hThread)
        return -1;

    ret = WaitForSingleObject (pStruct->hThread, INFINITE);
    DbgAssert(ret == WAIT_OBJECT_0);

    DWORD dwExitCode = 0;
    GetExitCodeThread(pStruct->hThread, &dwExitCode);

    // call CloseHandle to compatible with the posix thread functions
    // thread is invalid after posix pthread_join
    CloseHandle (pStruct->hThread);

    pStruct->hThread = 0;
    pStruct->dwID = 0;

    return dwExitCode;
#else
    DbgAssert(pStruct);
    if (!*pStruct)
        return -1;
    pthread_join (*pStruct, 0);
    *pStruct = 0;
    return 0;
#endif
}

/** let thread free, if the tread detached, you need't call ThreadJoin() */
TM_INLINE void ThreadDetach(TMThread* pStruct)
{
    DbgAssert(pStruct);
    if (!ThreadValid(pStruct))
        return;

    TM_IF_WIN (CloseHandle(pStruct->hThread); pStruct->hThread = 0);
    TM_NO_WIN (pthread_detach(*pStruct));
}

/** kill a thread */
TM_INLINE void ThreadKill(TMThread* pStruct)
{
    DbgAssert(pStruct);
    if (!ThreadValid(pStruct))
        return;
    TM_IF_WIN (TerminateThread(pStruct->hThread, 0); CloseHandle(pStruct->hThread));
//    TM_NO_WIN (pthread_cancel(*pStruct));
    TM_NO_WIN (pthread_kill(*pStruct, SIGKILL));
    ThreadInit(pStruct);
}

/** pause a thread */
TM_INLINE void ThreadSuspend(TMThread* pStruct)
{
#ifdef TM_WIN
    DbgAssert(pStruct);
    SuspendThread (pStruct->hThread);
#endif
}

/** resume a thread */
TM_INLINE void ThreadResume(TMThread* pStruct)
{
#ifdef TM_WIN
    DbgAssert(pStruct);
    ResumeThread (pStruct->hThread);
#endif
}

/** sleep self */
TM_INLINE void ThreadSleep(unsigned long dwMilliseconds)
{
    TM_IF_WIN (Sleep(dwMilliseconds));
    TM_NO_WIN (usleep(dwMilliseconds));

//     timeval tv;
//     tv.tv_sec = dwMilliseconds / 1000;                     // seconds
//     tv.tv_usec = (dwMilliseconds % 1000) * 1000;         // microseconds
//     select (0, 0, 0, 0, &tv);
}

/** allow self to yield the processor to another */
TM_INLINE void ThreadYield()
{
#ifdef TM_WIN
#   if (_WIN32_WINNT >= 0x0400)
    SwitchToThread ();
#   else
    Sleep (0);
#   endif // _WIN32_WINNT >= 0x0400
#else
    ThreadSleep (0);
#endif // TM_WIN
}

/** @} */


/**
 *  @addtogroup thread
 *  sync call "c" function prototype
 *  @see CThreadID
 *  @{
 */

typedef void (*PFNCall1)(void* pArg);

enum ECallType
{
    eCallTypeSync,
    eCallTypeAsync,
    eCallTypeSyncAsync, // if call thread is main thread then call sync, else call async.
};

TM_CAPI void CallInit(); // add ref to call manager
TM_CAPI void CallTerm(); // release to call manager
TM_CAPI TMThreadID CallGetTID();
TM_CAPI void CallFlush(void* token = 0, TMBool bCancel = false); // token, bCancel: only for ICallBaseAsync;
TM_CAPI TMBool CallCreate(ECallType eType, PFNCall1 pfn, void* pArg = 0);
TM_CAPI TMBool CallCreateEx(ECallType eType, struct ICallBase* pfn, void* token = 0);


///////////////////////////////////////////////////////////////////////////////
// sample:
// struct _SetThememFrame
// {
//     TM_DECLARE_ASYNC_REF_CALL(CWidget*);
//     static void Done(CWidget* mgr) { mgr->ChangeFrame(); }
// };
// _SetThememFrame::Call(&mgr);
#define TM_DECLARE_ASYNC_REF_CALL(P) \
    static void _done(void* lp) \
    { \
        P h = (P)lp; \
        Done(h); \
        h->Release(); \
    } \
    static TMBool Call(P lp) \
    { \
        lp->AddRef(); \
        TMBool b = CallCreate(eCallTypeAsync, _done, lp); \
        if (!b) \
            lp->Release(); \
        return b; \
    }



#ifdef TM_CPP
///////////////////////////////////////////////////////////////////////////////
// sample 1:
//  int a;
//  CallCreateT(eCallTypeSync, [a](TMBool bCancel) { return; });
// sample 2:
//  struct _CTextureGLBase_DelTex
//  {
//     GLuint              tex;
//     CGXAppGL*           app;
//     void operator()(TMBool bCancel) { gl_verify(glDeleteTextures(1, &tex)); app->Release(); }
//  };
//  _CTextureGLBase_DelTex tmp = { tex, m_ptrDevice };
//  tmp.app->AddRef();
//  DbgVerify(CallCreateT(eCallTypeSyncAsync, tmp, m_ptrDevice));
    struct ICallBaseAsync;
    struct ICallBase
    {
    public:
        virtual void done() = 0;
        virtual ICallBaseAsync* convert() = 0;
    };

    struct ICallBaseAsync
    {
    public:
        virtual ~ICallBaseAsync() {}
        virtual void done(TMBool bCancel) = 0;
    };

    template <class F> struct CFuncAsyncT : public ICallBaseAsync
    {
    public:
        TM_SELF_ALLOC(F);
        TM_NO_COPYABLE(CFuncAsyncT);

    public:
        F m_p;
        CFuncAsyncT(const F& pfn) : m_p(pfn) {}
        virtual void done(TMBool bCancel) tm_override { m_p(bCancel); }
    };

    template <class F>
    struct CFuncRefT : public ICallBase
    {
        F& m_p;
        CFuncRefT(const F& pfn) : m_p(const_cast<F&>(pfn)) {}
        virtual void done() tm_override { m_p(false); }
        virtual ICallBaseAsync* convert() tm_override { return new CFuncAsyncT<F>(m_p); }
    };

    template<class F>
    TM_INLINE TMBool CallCreateT(ECallType eType, const F& pfn, void* token = 0)
    {
        CFuncRefT<F> ref(pfn);
        return CallCreateEx(eType, &ref, token);
    }

#endif


/** @} */



#ifdef TM_CPP

TM_BGN_NAMESPACE


template <class T> class CAutoLockT;


/**
 *  @class CCritical
 *  @ingroup critical
 *  @brief packaged critical function
 *  debug on windows: stop on RtlEnterCritialSection; (_RTL_CRITICAL_SECTION*)rdi
 */
class CCritical
{
protected:
    TMCritical m_obj;
    TM_NO_COPYABLE(CCritical);

public:
    typedef CAutoLockT<CCritical>   CAutoLock;

public:
    /** @see CriticalCreate */
    CCritical() { CriticalCreate(&m_obj); }

    /** @see CriticalDestroy */
    ~CCritical() { CriticalDestroy(&m_obj); }

    /** @see CriticalTryLock */
    ESyncRet TryLock() { return CriticalTryLock(&m_obj); }

    /** @see CriticalLock */
    void Lock() { CriticalLock(&m_obj); }

    /** @see CriticalUnlock */
    void Unlock() { CriticalUnlock(&m_obj); }

#ifdef TM_WIN
    TMBool IsLockThread() const { return ThreadSelfId() == (TMThreadID)(TMULongPtr)(m_obj.OwningThread); }
    int GetRecursionCount() const { return m_obj.RecursionCount; }
#endif
};


/**
 *  @class CMutex
 *  @ingroup mutex
 *  @brief packaged mutex function
 */
class CMutex
{
protected:
    TMMutex m_obj;
    TM_IF_DBG(TMThreadID m_tid_dbg);
    TM_NO_COPYABLE(CMutex);

public:
    /** @see MutexCreate */
    CMutex() { MutexCreate(&m_obj); }

    /** @see MutexDestroy */
    ~CMutex() { MutexDestroy(&m_obj); }

    /** @see MutexTryLock */
    ESyncRet TryLock() { return MutexTryLock(&m_obj); }

    /** @see MutexLock */
    void Lock() { MutexLock(&m_obj); TM_IF_DBG(m_tid_dbg = ThreadSelfId()); }

    /** @see MutexUnlock */
    void Unlock() { TM_IF_DBG(m_tid_dbg = 0); MutexUnlock(&m_obj); }

    TM_IF_DBG(bool DbgIsLocked() { return m_tid_dbg == ThreadSelfId(); })
};


/**
 *  @class CEvent
 *  @ingroup event
 *  this is a windows auto-reset event object
 *  @brief packaged event function
 */
class CEvent
{
public:
    TMEvent m_obj;
    TM_NO_COPYABLE(CEvent);

public:
    /** @see EventCreate */
    CEvent() { EventCreate(&m_obj); }

    /** @see EventDestroy */
    ~CEvent() { EventDestroy(&m_obj); }

    /** @see EventLock */
    void Lock() { EventLock(&m_obj); }

    /** @see EventTimeLock */
    ESyncRet Lock(unsigned long dwMilliseconds) { return EventTimeLock(&m_obj, dwMilliseconds); }

    /** donothing */
    void Unlock() {}

    /** @see EventSignal */
    void Signal() { EventSignal(&m_obj); }
};


/**
 *  @class CState
 *  @ingroup state
 *  this is a windows manual-reset event object
 *  @brief packaged state function
 */
class CState
{
protected:
    TMState m_obj;
    TM_NO_COPYABLE(CState);

public:
    /** @see StateCreate */
    CState (TMBool bSignaled = false) { StateCreate (&m_obj, bSignaled); }

    /** @see StateDestroy */
    ~CState () { StateDestroy (&m_obj); }

    operator TMState() const { return m_obj; }

    /** @see StateLock */
    void Lock () { StateLock (&m_obj); }

    /** @see StateTimeLock */
    ESyncRet Lock (unsigned long dwMilliseconds) { return StateTimeLock (&m_obj, dwMilliseconds); }

    /** donothing */
    void Unlock () {}

    /** @see StateSignal */
    void Signal () { StateSignal (&m_obj); }

    /** @see StatePulse */
    void Pulse () { StatePulse (&m_obj); }

    /** @see StatePulse */
    TMBool IsSignaled () { return StateIsSignaled (&m_obj); }

    /** @see StateUnsignal */
    void Unsignal () { StateUnsignal (&m_obj); }

};


/**
 *  @class CAutoLockT
 *  @ingroup synchronization mutex critical semaphore
 *  used to lock automatically, release synchronization object
 *  @brief sample
 *  @code
 *  struct CAAAA
    {
        CMutex m;
        int x;

        void Set (int n)
        {
            // call statck: m.Lock(); x=n; m.Unlock();
            CAutoLockT<CMutex> _m(m);
            x = n;
        }
    };
  * @endcode
  * @see mutex critical semaphore
  */
template <class T> class CAutoLockT
{
private:
    T& m_cont;
    TM_NO_COPYABLE(CAutoLockT);

public:
    CAutoLockT (T& cont) : m_cont (cont) { cont.Lock (); }
    ~CAutoLockT () { m_cont.Unlock (); }
};


/**
 *  @class IThread
 *  @ingroup thread
 *  @brief callback for CThreadID
 */

class IThread
{
public:
    virtual unsigned ThreadProc() = 0; // tm_override
};

/**
 *  @class CThreadID
 *  @ingroup thread
 *  @brief packaged thread function
 *  you have 3 way to create a thread:
 *  @code
    // way 1
    class CMyThread : public IThread, public CThreadID
    {
    };
    CMyThread th;
    th.Create (&th);

    // way 2
    class CMyThread : public IThread
    {
    public:
        CThreadID th;
        void Create () { th.Create (this); }
    }

    // way 3
    TM_DECALRE_THREAD_PROC (ThreadProc)
    {
        ...
        return (TM_THREAD_PROC_RET)(0);
    }
    CThreadID th;
    th.Create (ThreadProc, x);

 *  @endcode
 */
class CThreadID
{
public:
    TMThread    m_obj;
    TM_NO_COPYABLE(CThreadID);

private:
    static TM_DECALRE_THREAD_PROC (_ThreadProc)
    {
        IThread* p = (IThread*) (pArg);
        return (TM_THREAD_PROC_RET)(TMIntPtr)p->ThreadProc ();
    }

public:
    /** */
    CThreadID() { ThreadInit (&m_obj); }

    /**
     *  @copydoc ThreadDetach
     *  @see ThreadDetach
     */
    ~CThreadID () { ThreadDetach (&m_obj); }

    /**
     *  @copydoc ThreadValid
     */
    TMBool IsValid () const { return ThreadValid(&m_obj); }

    /**
     *  @copydoc ThreadIsAlive
     */
    TMBool IsAlive () const { return ThreadIsAlive(&m_obj); }

    /** */
    operator TMBool() const { return IsValid(); }

    /**
     *  @copydoc ThreadDetach
     *  @see ThreadDetach
     */
    void Detach () { ThreadDetach (&m_obj); }

    /**
     *  @copydoc ThreadCreate
     *  @see ThreadCreate
     */
    void Create (IThread* pi) { ThreadCreate (&m_obj, _ThreadProc, pi); }

    /**
     *  @copydoc ThreadCreate
     *  @see ThreadCreate
     */
    void CreateBy (TM_THREAD_PROC proc, void* pArg) { ThreadCreate (&m_obj, proc, pArg); }

    /**
     *  @copydoc ThreadSuspend
     *  @see ThreadSuspend
     */
    void Suspend () { ThreadSuspend (&m_obj); }

    /**
     *  @copydoc ThreadResume
     *  @see ThreadResume
     */
    void Resume () { ThreadResume (&m_obj); }

    /**
     *  @copydoc ThreadJoin
     *  @see ThreadJoin
     */
    int Join () { return ThreadJoin (&m_obj); }

    /** @copydoc ThreadKill */
    void Kill () { ThreadKill(&m_obj); }

public: // call for current thread
    /**
     *  @copydoc ThreadSleep
     *  @see ThreadSleep
     */
    static void Sleep(unsigned long dwMilliseconds) { ThreadSleep(dwMilliseconds); }

    /**
     *  @copydoc ThreadYield
     *  @see ThreadYield
     */
    static void Yield() { ThreadYield(); }
};


// event for thread;
//   sample:
//   theadproc()
//   {
//       HANDLE h[2]= { m_hMyEvent, m_state };
//       if (WaitForMultipleObjects(2, h, false, INFINITE) == WAIT_OBJECT_0) { ... } else { return 0; }
//   }
class CThreadIDEx : public CThreadID
{
public:
    CState m_state; // Signal: exit thread;

public:
    void Create (IThread* pi)
    {
        m_state.Unsignal();
        CThreadID::Create(pi);
    }

    void CreateBy (TM_THREAD_PROC proc, void* pArg)
    {
        m_state.Unsignal();
        CThreadID::CreateBy(proc, pArg);
    }

    void SetStop()
    {
        m_state.Signal();
    }

    void Join ()
    {
        m_state.Signal();
        CThreadID::Join();
    }

    void Kill ()
    {
        m_state.Signal();
        CThreadID::Kill();
    }
};


/**
 *  @class CRefThread
 *  @ingroup thread
 *  @brief a thread have reference count.
 *  if --ref == 0, it will delete class.
 *  T: CThreadID or CThreadIDEx
 */
template <class T>
class CRefThreadT
    : public T
    , public IThread
{
protected:
    CAtomicInt m_nRef;
    TM_NO_COPYABLE(CRefThreadT);
    using T::m_obj;

private:
    static TM_DECALRE_THREAD_PROC (_ThreadProc)
    {
        CRefPtr<CRefThreadT> p;
        p.Attach((CRefThreadT*) (pArg));
        TM_THREAD_PROC_RET ret = (TM_THREAD_PROC_RET)p->ThreadProc ();
        return ret;
    }

public:
    // let alloc/free in same module.
    TM_SELF_ALLOC(T);


public:
    /** the init reference count = 1 */
    CRefThreadT () : m_nRef (1) {}

    /** */
    virtual ~CRefThreadT () {}

    /** */
    TM_REFAPI AddRef () { return ++m_nRef; }

    /** delete this if reference count == 0 */
    TM_REFAPI Release ()
    {
        TMAtmInt n = --m_nRef;
        if (n <= 0)
            delete this;
        return n;
    }

    /** */
    void Create () { AddRef(); ThreadCreate (&m_obj, _ThreadProc, this); }
};

typedef CRefThreadT<CThreadID> CRefThread;
typedef CRefThreadT<CThreadIDEx> CRefThreadEx;


struct CThreadNative
    : public CThreadID
    , public IThread
{
private:
    using CThreadID::Create;
    using CThreadID::CreateBy;

public:
    typedef CThreadNative   thread_class;

    void Create() { CThreadID::Create(this); }
};

#ifdef TM_QT

struct CThreadQt
    : public QThread
    , public IThread
{
private:
    int m_ret;

    virtual void run() tm_override
    {
        m_ret = ThreadProc();
    }

public:
    typedef CThreadQt   thread_class;

    CThreadQt() : m_ret(0) {}

    void Create() { QThread::start(); }

    int Join() { wait(); return m_ret; }

    void Kill() { terminate(); }

    static void Sleep(unsigned long dwMilliseconds) { msleep(dwMilliseconds); }
};

typedef CThreadQt       CThread;

#else

typedef CThreadNative   CThread;

#endif


TM_END_NAMESPACE

#endif //#ifdef TM_CPP


///////////////////////////////////////////////////////////////////////////////
// std::thread
#if defined(TM_MSC) && (TM_MSC < 1800)

_STD_BEGIN

#define _NOEXCEPT
#define _Thr_set_null(thr)  (thr.dwID = 0)
#define _Thr_is_null(thr)   (thr.dwID == 0)


class thread : public TM::CNoCopyable
{
public:
    class id
    {
    private:
        friend thread;
        TMThread m_obj;

    public:
        id() _NOEXCEPT { _Thr_set_null(m_obj); }
        id(const thread& _Thrd) : m_obj(_Thrd.m_obj) {}
        id(TMThread _Thrd) : m_obj(_Thrd) {}

        bool operator==(id _Right) _NOEXCEPT { return m_obj.dwID == _Right.m_obj.dwID; }
        bool operator!=(id _Right) _NOEXCEPT { return m_obj.dwID != _Right.m_obj.dwID; }
        bool operator <(id _Right) _NOEXCEPT { return m_obj.dwID  < _Right.m_obj.dwID; }
        bool operator<=(id _Right) _NOEXCEPT { return m_obj.dwID <= _Right.m_obj.dwID; }
        bool operator >(id _Right) _NOEXCEPT { return m_obj.dwID  > _Right.m_obj.dwID; }
        bool operator>=(id _Right) _NOEXCEPT { return m_obj.dwID >= _Right.m_obj.dwID; }
    };

    typedef void *native_handle_type;
    friend id;

private:
    TMThread m_obj;

public:
    thread() _NOEXCEPT { ThreadInit(&m_obj); }
    explicit thread(TM_THREAD_PROC proc, void* pArg) _NOEXCEPT { ThreadCreate(&m_obj, proc, pArg); }

    ~thread() _NOEXCEPT
    {   // clean up
        DbgAssert(!joinable());
    }

    void swap(thread& _Other) _NOEXCEPT
    {
        tm_swap(m_obj, _Other.m_obj);
    }

    bool joinable() const _NOEXCEPT
    {
        return (!_Thr_is_null(m_obj));
    }

    void join();

    void detach()
    {   // detach thread
        DbgAssert(!joinable());
        ThreadDetach(&m_obj);
        _Thr_set_null(m_obj);
    }

    id get_id() const _NOEXCEPT { return m_obj; }

    native_handle_type native_handle() { return m_obj.hThread; }

};

namespace this_thread
{
    inline thread::id get_id() _NOEXCEPT
    {
        TMThread _tr;
        ThreadSelf(&_tr);
        return _tr;
    }

    inline void yield() _NOEXCEPT
    {
        ThreadYield();
    }

    inline void sleep_until(unsigned long dwMilliseconds) _NOEXCEPT
    {
        ThreadSleep(dwMilliseconds);
    }

}   // namespace this_thread

inline void thread::join()
{
    DbgAssert(joinable());
    DbgAssert(!_Thr_is_null(m_obj));
    DbgAssert(get_id() != this_thread::get_id());
    ThreadJoin(&m_obj);
    _Thr_set_null(m_obj);
}

#undef _NOEXCEPT
#undef _Thr_set_null
#undef _Thr_is_null


_STD_END

#else // #if defined(TM_MSC) && (TM_MSC < 1800)

#   include <thread>

#endif // #if defined(TM_MSC) && (TM_MSC < 1800) else




#endif // #ifndef TMTHREAD_H
