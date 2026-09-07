/**
 *  @file
 *  @brief  semaphore operator<br>
 *  you can use these function & class in platform win32 and posix and System V.<br>
 *  these classes have the same behaviors on different platforms.
 *  the semaphore support thread and process
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2009.11.21
 *
 *  Copyright (C) 2009 miragekiller
 */

#pragma once

#ifndef TMSEMAPHORE_H
#define TMSEMAPHORE_H

#include "tmthread.h"


/**
 *  @defgroup process
 *  @defgroup synchronization
 */

/**
 *  @defgroup semaphore
 *  @ingroup thread process synchronization
 *  nested semaphore "c" function prototype
 *  @see CSemaphore
 *  @{
 */

#ifdef TM_DOXYGEN

/**
 *  create a semaphore object
 *  @param[in] pKey
 *  string specifying the name of the semaphore object.
 *  The name is limited to MAX_PATH characters. Name comparison is case sensitive.
 *  If lpName is NULL, the semaphore object is created without a name.
 *  @param[in] nRef
 *  Specifies an initial count for the semaphore object.
 *  This value must be greater than or equal to zero.
 *  The count is increased by a specified amount by calling the SemUnlock function.
 *  @param[in] bOpenIfExists
 *  If the named semaphore object existed before the function call,
 *  the function open the existed object if bOpenIfExists == true.
 *  @return if error return 0; else return handle of object.
 *  @see SemOpen
 */
TMHSem SemCreate(TMCStr pKey, int nRef, TMBool bOpenIfExists);

/**
 * opens an existing named semaphore object
 * @see SemCreate
 */
TMHSem SemOpen(TMCStr pKey);

/** destroy a semaphore object */
void SemDestroy(TMHSem hSem);

/** see WaitForSingleObject(h, INFINITE) in MSDN */
void SemLock(TMHSem hSem);

/** see WaitForSingleObject(h, 0) in MSDN */
ESyncRet SemTryLock(TMHSem hSem);

/** increases the count of the specified semaphore object */
void SemUnlock(TMHSem hSem);

#endif // TM_DOXYGEN


#ifdef TM_WIN

#   include <windows.h>

    typedef HANDLE TMHSem;

TM_INLINE TMHSem SemCreate(TMCStr pKey, int nRef, TMBool bOpenIfExists)
{
    HANDLE hd = CreateSemaphore(NULL, nRef, LONG_MAX, pKey);
    if (!bOpenIfExists && (GetLastError() == ERROR_ALREADY_EXISTS))
    {
        CloseHandle (hd);
        return 0;
    }
    return hd;
}

TM_INLINE TMHSem SemOpen(TMCStr pKey)
{
    DbgAssert(pKey);
    return OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, pKey);
}

TM_INLINE void SemDestroy(TMHSem hSem)
{
    DbgAssert(hSem);
    CloseHandle (hSem);
}

TM_INLINE void SemLock(TMHSem hSem)
{
    DbgAssert(hSem);
    WaitForSingleObject(hSem, INFINITE);
}

TM_INLINE ESyncRet SemTryLock(TMHSem hSem)
{
    DbgAssert(hSem);
    return (ESyncRet)WaitForSingleObject(hSem, 0);
}

TM_INLINE void SemUnlock(TMHSem hSem)
{
    DbgAssert(hSem);
    ReleaseSemaphore(hSem, 1, NULL);
}

#elif defined(TM_ANDROID)

#   include <semaphore.h>
#   include "tmio.h"

typedef struct _ATHSem
{
    union
    {
        sem_t* sem;
        sem_t usem;
    };
    char name[1];
}* TMHSem;

TM_INLINE TMHSem _SemMake(sem_t* sem, TMCStr pKey)
{
    _ATHSem* pa;
    int n;
    if (SEM_FAILED == sem)
        return 0;
    n = StrLen(pKey);
    pa = (_ATHSem*)MemAlloc(sizeof(_ATHSem) + n);
    pa->sem = sem;
    MemCpy(pa->name, pKey, n + 1);
    return pa;
}

TM_INLINE TMHSem SemOpen(TMCStr pKey)
{
    sem_t* sem = sem_open(pKey, O_RDWR);
    return _SemMake(sem, pKey);
}

TM_INLINE TMHSem SemCreate(TMCStr pKey, int nRef, TMBool bOpenIfExists)
{
    sem_t* sem;
    sem_t usem;
    TMHSem h = 0;
    if (pKey)
    {
        if (0 == sem_init(&usem, false, nRef))
        {
            h = (_ATHSem*)MemAlloc(sizeof(_ATHSem));
            h->usem = usem;
            h->name[0] = 0;
        }
        return h;
    }
    else if (bOpenIfExists)
    {
        h = SemOpen(pKey);
        if (h)
            return h;
    }
    sem = sem_open(pKey, O_CREAT | O_EXCL | O_RDWR, 666/*S_IREAD | S_IWRITE*/, nRef);
    return _SemMake(sem, pKey);
}

TM_INLINE void SemDestroy(TMHSem hSem)
{
    DbgAssert(hSem);
    if (hSem->name[0])
    {
        sem_close(hSem->sem);
        sem_unlink(hSem->name);
    }
    else
        sem_destroy(&hSem->usem);
    MemFree(hSem);
}

TM_INLINE void SemLock(TMHSem hSem)
{
    sem_wait(hSem->name[0] ? hSem->sem : &hSem->usem);
}

TM_INLINE ESyncRet SemTryLock(TMHSem hSem)
{
    return (ESyncRet)sem_trywait(hSem->name[0] ? hSem->sem : &hSem->usem);
}

TM_INLINE void SemUnlock(TMHSem hSem)
{
    sem_post(hSem->name[0] ? hSem->sem : &hSem->usem);
}

#else

#   include <sys/types.h>
#   include <sys/ipc.h>
#   include <sys/sem.h>

    typedef int TMHSem;

    union __semun
    {
       int val;
       struct semid_ds *buf;
       ushort *array;
    };

//  JS Hash Function
TM_INLINE int JSHash(TMCStr str)
{
    unsigned int hash  = 1315423911;
    while (*str)
    { hash  ^=  ((hash << 5) + (*str++) + (hash >> 2)); }
    return hash;
}

TM_INLINE TMHSem SemCreate(TMCStr pKey, int nRef, TMBool bOpenIfExists)
{
    union __semun val;
    int n, nKey = pKey ? JSHash(pKey) : IPC_PRIVATE;
    n = semget (nKey, 1, 0600 | IPC_CREAT | IPC_EXCL);
    if (n == -1 && bOpenIfExists && EEXIST == errno)
        n = semget (nKey, 0, 0);
    if (n == -1)
        return 0;
    val.val = nRef;
    semctl(n, 0, SETVAL, val);
    return n;
}

TM_INLINE TMHSem SemOpen(TMCStr pKey)
{
    DbgAssert(pKey);
    int n = semget(JSHash(pKey), 0, 0);
    return n == -1 ? 0 : n;
}

TM_INLINE void SemDestroy(TMHSem hSem)
{
    DbgAssert(hSem);
    union __semun val;
    val.val = 0;
    semctl (hSem, 0, IPC_RMID, val);
}

TM_INLINE void SemLock(TMHSem hSem)
{
    DbgAssert(hSem);
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = 0;
    semop (hSem, &sb, 1);
}

TM_INLINE ESyncRet SemTryLock(TMHSem hSem)
{
    DbgAssert(hSem);
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = IPC_NOWAIT;
    return (ESyncRet)semop(hSem, &sb, 1);
}

TM_INLINE void SemUnlock(TMHSem hSem)
{
    DbgAssert(hSem);
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = 0;
    semop(hSem, &sb, 1);
}

#endif


#ifdef TM_CPP

TM_BGN_NAMESPACE


/**
 *  @class CSemaphore
 *  @brief packaged semaphore function
 */
class CSemaphore
{
protected:
    TMHSem m_obj;
    TM_NO_COPYABLE(CSemaphore);

public:
    /** */
    CSemaphore() : m_obj (0) {}

    /** @see SemDestroy */
    ~CSemaphore() { if (m_obj) SemDestroy(m_obj); }

    /**
     * @copydoc SemCreate
     * @see SemCreate
     */
    void Create(TMCStr pKey, int nRef, TMBool bOpenIfExists)
    {
        DbgAssert(!m_obj);
        m_obj = SemCreate(pKey, nRef, bOpenIfExists);
    }

    /**
     * @copydoc SemDestroy
     * @see SemDestroy
     */
    void Destroy()
    {
        DbgAssert(m_obj);
        SemDestroy(m_obj);
        m_obj = 0;
    }

    /**
     * @copydoc SemOpen
     * @see SemOpen
     */
    void Open(TMCStr pKey)
    {
        DbgAssert(!m_obj);
        m_obj = SemOpen(pKey);
    }

    /**
     * @copydoc SemTryLock
     * @see SemTryLock
     */
    ESyncRet TryLock() { return SemTryLock(m_obj); }

    /**
     * @copydoc SemLock
     * @see SemLock
     */
    void Lock() { SemLock(m_obj); }

    /**
     * @copydoc SemUnlock
     * @see SemUnlock
     */
    void Unlock() { SemUnlock(m_obj); }
};

TM_END_NAMESPACE

#endif //#ifdef TM_CPP

/** @} */ // defgroup semaphore

#endif // end of #ifndef TMSEMAPHORE_H
