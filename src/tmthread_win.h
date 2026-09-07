/*
 *  @file
 *  @brief  thread for windows
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2002.12.21
 *
 *  Copyright (C) 2002 miragekiller
 */

#pragma once

#ifndef TMTHREAD_WIN_H
#define TMTHREAD_WIN_H

#include <windows.h>

// #ifdef _MT
// 
// #ifndef _INC_PROCESS
// #   include <process.h>
// #endif
// 
// template < class T >
// inline HANDLE BeginThread (unsigned StackSize, T StartFunction, 
//                             void* ThreadParameter, unsigned CreationFlags, 
//                             unsigned* ThreadId)
// {
//     return (HANDLE) _beginthreadex (0, StackSize, 
//         (unsigned (__stdcall *) (void *)) StartFunction,
//         ThreadParameter, CreationFlags, ThreadId);
// }
// #else
// template < class T >
// inline HANDLE BeginThread (unsigned StackSize, T StartFunction, 
//                            PVOID ThreadParameter, unsigned CreationFlags, 
//                            unsigned* ThreadId)
// {
//     return CreateThread (0, StackSize, 
//         (LPTHREAD_START_ROUTINE ) StartFunction, 
//         ThreadParameter, CreationFlags, (PULONG)ThreadId);
// }
// #endif // #ifdef _MT

// <WINBASE.H> has the following invalid definition: #define Yield()
#undef Yield

typedef enum
{
    e_srOK      = WAIT_OBJECT_0,
    e_srBusy    = WAIT_TIMEOUT,
    e_srFailed  = WAIT_FAILED,
} ESyncRet;


//////////////////////////////////////////////////////////////////////////////
// critical
#define TMCritical CRITICAL_SECTION

TM_INLINE void CriticalCreate(TMCritical* pStruct)
{
    DbgAssert(pStruct);
#if TM_WIN_METRO
    InitializeCriticalSectionEx(pStruct, 0, 0);
#else
    InitializeCriticalSection(pStruct);
#endif
}

TM_INLINE ESyncRet CriticalTryLock(TMCritical* pStruct)
{
    DbgAssert(pStruct);
#if (_WIN32_WINNT >= 0x0400)
    return TryEnterCriticalSection(pStruct) ? e_srOK : e_srBusy;
#else
    return e_srFailed;
#endif
}

TM_INLINE void CriticalLock(TMCritical* pStruct)
{
    DbgAssert(pStruct);
    EnterCriticalSection(pStruct);
}

TM_INLINE void CriticalUnlock(TMCritical* pStruct)
{
    DbgAssert(pStruct);
    LeaveCriticalSection(pStruct);
}

TM_INLINE void CriticalDestroy(TMCritical* pStruct)
{
    DbgAssert(pStruct);
    DeleteCriticalSection(pStruct);
}


//////////////////////////////////////////////////////////////////////////////
// mutex
#define TMMutex HANDLE

TM_INLINE void MutexCreate(TMMutex* pStruct)
{
    DbgAssert(pStruct);
#if TM_WIN_METRO
    *pStruct = CreateEventEx(NULL, NULL, CREATE_EVENT_INITIAL_SET, EVENT_ALL_ACCESS);
#else
    *pStruct = CreateEvent(NULL, FALSE, TRUE, NULL);
#endif
}

TM_INLINE ESyncRet MutexTryLock(TMMutex* pStruct)
{
    DbgAssert(pStruct);
#if TM_WIN_METRO
    return (ESyncRet)WaitForSingleObjectEx(*pStruct, 0, FALSE);
#else
    return (ESyncRet)WaitForSingleObject(*pStruct, 0);
#endif
}

TM_INLINE void MutexLock(TMMutex* pStruct)
{
    DbgAssert(pStruct && *pStruct);
#if TM_WIN_METRO
    WaitForSingleObjectEx(*pStruct, INFINITE, FALSE);
#else
    WaitForSingleObject(*pStruct, INFINITE);
#endif
}

TM_INLINE void MutexUnlock(TMMutex* pStruct)
{
    DbgAssert(pStruct && *pStruct);
    SetEvent (*pStruct);
}

TM_INLINE void MutexDestroy(TMMutex* pStruct)
{
    DbgAssert(pStruct && *pStruct);
    CloseHandle (*pStruct);
}


//////////////////////////////////////////////////////////////////////////////
// event
#define TMEvent HANDLE

TM_INLINE void EventCreate(TMEvent* pStruct)
{
    DbgAssert(pStruct);
#if TM_WIN_METRO
    *pStruct = CreateEventEx (NULL, NULL, 0, EVENT_ALL_ACCESS);
#else
    *pStruct = CreateEvent(NULL, FALSE, FALSE, NULL);
#endif
}

TM_INLINE void EventLock(TMEvent* pStruct)
{
#if TM_WIN_METRO
    WaitForSingleObjectEx(*pStruct, INFINITE, FALSE);
#else
    WaitForSingleObject(*pStruct, INFINITE);
#endif
}

TM_INLINE ESyncRet EventTimeLock(TMEvent* pStruct, unsigned long dwMilliseconds)
{
#if TM_WIN_METRO
    return (ESyncRet)WaitForSingleObjectEx (*pStruct, dwMilliseconds, FALSE);
#else
    return (ESyncRet)WaitForSingleObject (*pStruct, dwMilliseconds);
#endif
}

TM_INLINE void EventSignal(TMEvent* pStruct)
{
    DbgAssert(pStruct);
#if TM_WIN_METRO
    SetEvent (*pStruct);
#else
    PulseEvent (*pStruct);
#endif
}

TM_INLINE void EventDestroy(TMEvent* pStruct)
{
    DbgAssert(pStruct);
    CloseHandle (*pStruct);
}

//////////////////////////////////////////////////////////////////////////////
// state
#define TMState TMEvent

TM_INLINE void StateCreate(TMEvent* pStruct, TMBool bSignaled)
{
    DbgAssert(pStruct);
#if TM_WIN_METRO
    DWORD dwFlags = CREATE_EVENT_MANUAL_RESET;
    if (bSignaled)
        dwFlags |= CREATE_EVENT_INITIAL_SET;
    *pStruct = CreateEventEx (NULL, NULL, dwFlags, EVENT_ALL_ACCESS);
#else
    *pStruct = CreateEvent(NULL, TRUE, bSignaled, NULL);
#endif
}

#define StateLock       EventLock
#define StateTimeLock   EventTimeLock
#define StateDestroy    EventDestroy

TM_INLINE void StateSignal(TMEvent* pStruct)
{
    DbgAssert(pStruct);
    SetEvent (*pStruct);
}

TM_INLINE void StateUnsignal(TMEvent* pStruct)
{
    DbgAssert(pStruct);
    ResetEvent (*pStruct);
}

TM_INLINE void StatePulse(TMEvent* pStruct)
{
    DbgAssert(pStruct);
#if TM_WIN_METRO
    DbgAssert(0);
#else
    PulseEvent (*pStruct);
#endif
}

TM_INLINE TMBool StateIsSignaled(TMState* pStruct)
{
    DbgAssert(pStruct);
#if TM_WIN_METRO
    return e_srOK == WaitForSingleObjectEx(*pStruct, 0, FALSE); 
#else
    return e_srOK == WaitForSingleObject(*pStruct, 0); 
#endif
}


#endif // TMTHREAD_WIN_H





