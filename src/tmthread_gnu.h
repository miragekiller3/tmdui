/*
 *  @file
 *  @brief  thread
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2002.12.21
 *
 *  Copyright (C) 2002 miragekiller
 */

#pragma once

#ifndef TMTHREAD_GNU_H
#define TMTHREAD_GNU_H

#include <pthread.h>
#include <errno.h>
#include <sys/time.h>


typedef enum
{
    e_srOK      = 0,
    e_srBusy    = EBUSY,
    e_srFailed  = EINVAL,
} ESyncRet;


//////////////////////////////////////////////////////////////////////////////
// mutex
#define TMMutex pthread_mutex_t

TM_INLINE void MutexCreate (TMMutex* pStruct)
{
    DbgAssert(pStruct);
    pthread_mutex_init (pStruct, 0);
}

TM_INLINE ESyncRet MutexTryLock (TMMutex* pStruct)
{
    DbgAssert(pStruct);
    return (ESyncRet)pthread_mutex_trylock (pStruct);
}

TM_INLINE void MutexLock (TMMutex* pStruct)
{
    DbgAssert(pStruct);
    pthread_mutex_lock (pStruct);
}

TM_INLINE void MutexUnlock (TMMutex* pStruct)
{
    DbgAssert(pStruct);
    pthread_mutex_unlock (pStruct);
}

TM_INLINE void MutexDestroy (TMMutex* pStruct)
{
    DbgAssert(pStruct);
    pthread_mutex_destroy (pStruct);
}


//////////////////////////////////////////////////////////////////////////////
// critical
#ifdef PTHREAD_MUTEX_RECURSIVE_NP
#   define _PTHREAD_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
#elif defined (PTHREAD_MUTEX_RECURSIVE)
#   define _PTHREAD_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE
#endif

#define TMCritical TMMutex
TM_INLINE void CriticalCreate (TMCritical* pStruct)
{
    DbgAssert(pStruct);

    pthread_mutexattr_t attr;
    pthread_mutexattr_init (&attr);
    pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init (pStruct, &attr);
}

#define CriticalTryLock     MutexTryLock
#define CriticalLock        MutexLock
#define CriticalUnlock      MutexUnlock
#define CriticalDestroy     MutexDestroy


// struct TMCritical
// {
//     pthread_mutex_t mutex;
//     pthread_t       owner;
//     int             ref;
// };
//
// TM_INLINE void CriticalCreate (TMCritical* pStruct)
// {
//     DbgAssert(pStruct);
//     pthread_mutex_init (&(pStruct->mutex), 0);
// }
//
// TM_INLINE ESyncRet __critical_lock (int (*pfn)(pthread_mutex_t*), TMCritical* pStruct)
// {
//     DbgAssert(pStruct);
//     pthread_t s = pthread_self();
//     if (pthread_equal (s, pStruct->owner))
//     {
//         ++pStruct->ref; // self inc, so need't use "AtomicInc" function
//         return e_srOK;
//     }
//
//     int n = pfn (&(pStruct->mutex));
//     if (n == e_srOK)
//     {
//         pStruct->owner = s;
//         pStruct->ref = 1;
//     }
//
//     return (ESyncRet)n;
// }
//
//
// TM_INLINE ESyncRet CriticalTryLock (TMCritical* pStruct)
// {
//     DbgAssert(pStruct);
//     return __critical_lock (pthread_mutex_trylock, pStruct);
// }
//
// TM_INLINE void CriticalLock (TMCritical* pStruct)
// {
//     DbgAssert(pStruct);
//     __critical_lock (pthread_mutex_lock, pStruct);
// }
//
// TM_INLINE void CriticalUnlock (TMCritical* pStruct)
// {
//     DbgAssert(pStruct);
//     if (pthread_equal (pthread_self(), pStruct->owner))
//     {
//         if (--(pStruct->ref) > 0)
//             return;
//
//         memset (&(pStruct->owner), 0, sizeof pStruct->owner);
//         pthread_mutex_unlock (&pStruct->mutex);
//     }
// }
//
// TM_INLINE void CriticalDestroy (TMCritical* pStruct)
// {
//     DbgAssert(pStruct);
//     pthread_mutex_destroy (&(pStruct->mutex));
// }


//////////////////////////////////////////////////////////////////////////////
// event
typedef struct
{
    pthread_cond_t cond;
    pthread_mutex_t mutex;
} TMEvent;

TM_INLINE void EventCreate (TMEvent* pStruct)
{
    DbgAssert(pStruct);
    pthread_mutex_init (&pStruct->mutex, 0);
    pthread_cond_init (&pStruct->cond, 0);
}

TM_INLINE void EventLock (TMEvent* pStruct)
{
    typedef void (*__routine)(void *);    // Routine to call
    pthread_cleanup_push ((__routine)&pthread_mutex_unlock, (void*)&pStruct->mutex);
    pthread_mutex_lock (&pStruct->mutex);
    pthread_cond_wait (&pStruct->cond, &pStruct->mutex);
    pthread_mutex_unlock (&pStruct->mutex);
    pthread_cleanup_pop (0);
}

TM_INLINE ESyncRet _pthread_cond_timedwait (pthread_cond_t* cond,
                                            pthread_mutex_t* mutex,
                                            unsigned long dwMilliseconds)
{
    struct timeval now;
    struct timespec ts;
    gettimeofday (&now, NULL);
    ts.tv_sec  = now.tv_sec + (dwMilliseconds / 1000);   // second
    ts.tv_nsec = (dwMilliseconds % 1000) * 1000000;      //nanosecond
    return (ESyncRet) pthread_cond_timedwait (cond, mutex, &ts);
}

TM_INLINE ESyncRet EventTimeLock (TMEvent* pStruct, unsigned long dwMilliseconds)
{
    ESyncRet ret;
    pthread_mutex_lock (&pStruct->mutex);
    ret = (ESyncRet) _pthread_cond_timedwait (&pStruct->cond, &pStruct->mutex, dwMilliseconds);
    pthread_mutex_unlock (&pStruct->mutex);
    return ret;
}

TM_INLINE void EventSignal (TMEvent* pStruct)
{
    DbgAssert(pStruct);
    pthread_cond_signal (&pStruct->cond);
}

TM_INLINE void EventDestroy (TMEvent* pStruct)
{
    DbgAssert(pStruct);
    pthread_cond_destroy (&pStruct->cond);
    pthread_mutex_destroy (&pStruct->mutex);
}


//////////////////////////////////////////////////////////////////////////////
// state
typedef struct
{
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    TMBool signaled;
} TMState;

TM_INLINE void StateCreate (TMState* pStruct, TMBool bSignaled)
{
    DbgAssert(pStruct);
    pthread_mutex_init (&pStruct->mutex, 0);
    pthread_cond_init (&pStruct->cond, 0);
    pStruct->signaled = bSignaled;
}

TM_INLINE void StateLock (TMState* pStruct)
{
    pthread_mutex_lock (&pStruct->mutex);
    if (!pStruct->signaled)
        pthread_cond_wait (&pStruct->cond, &pStruct->mutex);
    pthread_mutex_unlock (&pStruct->mutex);
}

TM_INLINE ESyncRet StateTimeLock (TMState* pStruct, unsigned long dwMilliseconds)
{
    ESyncRet ret = e_srOK;
    pthread_mutex_lock (&pStruct->mutex);
    if (!pStruct->signaled)
        ret = _pthread_cond_timedwait (&pStruct->cond, &pStruct->mutex, dwMilliseconds);
    pthread_mutex_unlock (&pStruct->mutex);
    return ret;
}

TM_INLINE void StateSignal (TMState* pStruct)
{
    DbgAssert(pStruct);
    pthread_mutex_lock (&pStruct->mutex);
    if (!pStruct->signaled)
    {
        pStruct->signaled = true;
        pthread_cond_signal (&pStruct->cond);
    }
    pthread_mutex_unlock (&pStruct->mutex);
}

TM_INLINE void StateUnsignal (TMState* pStruct)
{
    DbgAssert(pStruct);
    pthread_mutex_lock (&pStruct->mutex);
    pStruct->signaled = false;
    pthread_mutex_unlock (&pStruct->mutex);
}

TM_INLINE void StatePulse (TMState* pStruct)
{
    DbgAssert(pStruct);
    pthread_mutex_lock (&pStruct->mutex);
    pthread_cond_broadcast (&pStruct->cond);
    pStruct->signaled = false;
    pthread_mutex_unlock (&pStruct->mutex);
}

TM_INLINE TMBool StateIsSignaled (TMState* pStruct)
{
    DbgAssert(pStruct);
    return pStruct->signaled;
}

TM_INLINE void StateDestroy (TMState* pStruct)
{
    DbgAssert(pStruct);
    pthread_cond_destroy (&pStruct->cond);
    pthread_mutex_destroy (&pStruct->mutex);
}



///////////////////////////////////////////////////////////////////////////////
// this class compatible with win32
namespace TM
{
    enum EWaitRet
    {
        // posix
        e_wrObject0     = 0,
        e_wrTimeout     = ETIMEDOUT,
        e_wrFailed      = EINVAL,
    };
} // end of namespace TM




///////////////////////////////////////////////////////////////////////////////
// this class not compatible with win32
TM_BGN_NAMESPACE


//class CRelTime : public timespec //  relatively time
//{
//public:
//    CRelTime (unsigned long dwMilliseconds)
//    {
//        tv_sec  = dwMilliseconds / 1000;                    // second
//        tv_nsec = (dwMilliseconds % 1000) * 1000000;      //nanosecond
//    }
//};
//
//class CAbsTime : public timespec // absolute time
//{
//public:
//    CAbsTime (unsigned long dwMilliseconds)
//    {
//        timeval now;
//        gettimeofday (&now, NULL);
//
//        tv_sec  = now.tv_sec + (dwMilliseconds / 1000);   // second
//        tv_nsec = (dwMilliseconds % 1000) * 1000000;      //nanosecond
//    }
//};
//
//class CTimeval : public timeval
//{
//public:
//    CTimeval (unsigned long dwMilliseconds)
//    {
//        tv_sec = dwMilliseconds / 1000;                     // seconds
//        tv_usec = (dwMilliseconds % 1000) * 1000;         // microseconds
//    }
//};

// class CCond : public pthread_cond_t
// {
// public:
//     explicit CCond ()
//     {
//         pthread_cond_init (this, 0);
//     }
// 
//     ~CCond ()
//     {
//         pthread_cond_destroy (this);
//     }
// 
//     TMBool Signal ()
//     {
//         return 0 == pthread_cond_signal (this);
//     }
// 
//     TMBool Broadcast ()
//     {
//         return 0 == pthread_cond_broadcast (this);
//     }
// 
//     EWaitRet Wait (pthread_mutex_t& mutex)
//     {
//         return (EWaitRet) pthread_cond_wait (this, &mutex);
//     }
// 
//     EWaitRet Wait (const CAbsTime& timeout, pthread_mutex_t& mutex)
//     {
//         return (EWaitRet) pthread_cond_timedwait (this,
//             &mutex, &timeout);
//     }
// 
// };

} // end of namespace TM

/*
///////////////////////////////////////////////////////////////////////////////
// this class compatible with win32
namespace TM
{
///////////////////////////////////////////////////////////////////////////
// CMutex
//   when user call "lock" 2nd in same thread, the tread will block
// class CMutex : public pthread_mutex_t
// {
// public:
//     explicit CMutex ()
//     {
//         pthread_mutex_init (this, 0);
//     }
//
//     ~CMutex ()
//     {
//         pthread_mutex_destroy (this);
//     }
//
//     TMBool Trylock ()
//     {
//         return !pthread_mutex_trylock (this);
//     }
//
//     TMBool Lock ()
//     {
//         //pthread_mutex_timedlock
//         return 0 == pthread_mutex_lock (this);
//     }
//
//     TMBool Unlock ()
//     {
//         return 0 == pthread_mutex_unlock (this);
//     }
// };

///////////////////////////////////////////////////////////////////////////
// CCritical
// Critical Section
// the same thread can lock time after time
// class CCritical
// {
// protected:
//     CMutex      m_mutex;
//     pthread_t   m_owner;
//     int         m_ref;
//
// public:
//     CCritical ()
//         : m_owner (0)
//         , m_ref (0)
//     {
//     }
//
//     ~CCritical () {}
//
//     TMBool Lock ()
//     {
//         pthread_t dwID = pthread_self ();
//         if (dwID == m_owner)
//         {
//             DbgAssert(dwID);
//
//             m_ref++;
//         }
//         else
//         {
//             m_mutex.Lock ();
//             m_ref = 1;
//             m_owner = dwID;
//         }
//
//         return TMTrue;
//     }
//
//     TMBool Unlock ()
//     {
//         pthread_t dwID = pthread_self ();
//         if (dwID == m_owner)
//         {
//             m_ref--;
//             if (m_ref >= 0)
//             {
//                 m_owner = 0;
//                 m_mutex.Unlock ();
//             }
//             return TMTrue;
//         }
//
//         return TMFalse;
//     }
//
//     TMBool TryLock ()
//     {
//         if (pthread_self () == m_owner)
//             return TMTrue;
//
//         return m_ref <= 0;
//     }
// };

///////////////////////////////////////////////////////////////////////////
// CEvent
// when event state is signaled or goto signaled,
// the wait function return
// class CEvent
// {
// protected:
//     CMutex  m_mutex;
//     CCond   m_cond;
//
// public:
//     // used by thread 1
//     TMBool Unlock ()
//     {
//         return m_cond.Signal ();
//     }
//
//     TMBool UnlockAll ()
//     {
//         return m_cond.Broadcast();
//     }
//
//     // used by thread 2
//     EWaitRet Lock ()
//     {
//         m_mutex.Lock ();
//         EWaitRet ret = m_cond.Wait (m_mutex);
//         m_mutex.Unlock ();
//         return ret;
//     }
//
//     // used by thread 2
//     EWaitRet Lock (unsigned long dwMilliseconds)
//     {
//         m_mutex.Lock ();
//         EWaitRet ret = m_cond.Wait (dwMilliseconds, m_mutex);
//         m_mutex.Unlock ();
//         return ret;
//     }
// };

///////////////////////////////////////////////////////////////////////////
// CState
// when state is signaled or goto signaled,
// the wait function return
// class CState
// {
// protected:
//     CMutex  m_mutex;
//     CCond   m_cond;
//     TMBool  m_bSignaled;
//
// public:
//     explicit CState   (TMBool bSignaled = TMFalse)
//         : m_bSignaled (bSignaled)
//     {}
//
//     TMBool IsSignaled () const
//     {
//         return m_bSignaled;
//     }
//
//     // used by thread 1
//     TMBool Signal ()
//     {
//         if (m_bSignaled)
//             return TMTrue;
//
//         m_mutex.Lock();
//         m_bSignaled = TMTrue;
//         m_mutex.Unlock();
//         return m_cond.Signal ();
//     }
//
//     // used by thread 1
//     TMBool Unsignal ()
//     {
//         if (!m_bSignaled)
//             return TMTrue;
//
//         m_mutex.Lock();
//         m_bSignaled = TMFalse;
//         m_mutex.Unlock();
//         return TMTrue; //m_cond.Signal ();
//     }
//
//     // set to Signal, and then reset to Unsignal
//     // let all waited thread go ahead
//     TMBool Pulse ()
//     {
//         if (!m_bSignaled)
//             m_cond.Broadcast ();
//
//         return TMTrue;
//     }
//
//     // used by thread 2
//     // through when signaled;
//     EWaitRet Lock ()
//     {
//         EWaitRet ret = e_wrObject0;
//         m_mutex.Lock ();
//         if (!m_bSignaled)
//             ret = m_cond.Wait (m_mutex);
//         m_mutex.Unlock ();
//         return ret;
//     }
//
//     // used by thread 2
//     EWaitRet Lock (unsigned long dwMilliseconds)
//     {
//         EWaitRet ret = e_wrObject0;
//         m_mutex.Lock ();
//         if (!m_bSignaled)
//             ret = m_cond.Wait (dwMilliseconds, m_mutex);
//         m_mutex.Unlock ();
//         return ret;
//     }
// };

// class CThread
// {
// protected:
//     pthread_t   m_dwID;
//
//     static void* _ThreadProc (void* pArg)
//     {
//         CThread* p = (CThread*) pArg;
//         return (void*) p->ThreadProc ();
//     }
//
//     virtual unsigned long ThreadProc () = 0;
//
// public:
//     explicit CThread    ()
//         : m_dwID        (0)
//     {
//     }
//
//     ~CThread ()
//     {
//     }
//
//     operator TMBool () const { return 0 != m_dwID; }
//
//     TMBool operator ! () const { return !m_dwID; }
//
// public: // used by thread 1
//     TMBool Create ()
//     {
//         DbgAssert(!m_dwID);
//
//         TMBool ret = !pthread_create (&m_dwID, 0, _ThreadProc, this);
//         return ret;
//     }
//
//     void Join ()
//     {
//         if (m_dwID)
//         {
//             pthread_join (m_dwID, 0);
//             m_dwID = 0;
//         }
//     }
//
//     void Detach ()
//     {
//         if (m_dwID)
//             pthread_detach (m_dwID);
//     }
//
// public:
//     static void Sleep (unsigned long dwMilliseconds)
//     {
//         select (0, 0, 0, 0, &CTimeval (dwMilliseconds));
//     }
//
//     static void Yield ()
//     {
//         Sleep (0);
//     }
// };


TM_END_NAMESPACE
*/

#endif // TMTHREAD_GNU_H



