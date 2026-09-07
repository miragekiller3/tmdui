/**
 *  @file
 *  @brief  declare based, public function
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2005.11.11
 *
 *  Copyright (C) 2005 miragekiller
 */

#pragma once

#ifndef TMIO_H
#define TMIO_H

#include <ios>
#include <string.h>     // memXXX
#include "tmdebug.h"


/**
 *  @name MemXXX
 *  if use malloc a mem in a.dll, and free it in b.dll,
 *  the function will have issue.
 *  so we use these three function to avoid the issue.
 *  @{
 */

/** same ::malloc */
TM_CAPI void* MemAlloc1(size_t dwSize);

/** same ::malloc */
TM_CAPI void* MemAllocBy1(size_t dwSize, void* pInit);

/** same ::realloc */
TM_CAPI void* MemRealloc1(void* p, size_t dwSize);

/** same ::free; p can't be 0 */
TM_CAPI void MemFree(void* p);

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning (disable: 4996) // xxxx was declared deprecated
#endif

TM_INLINE void MemCpy(void* dest, const void* src, size_t count)
{
    memcpy(dest, src, count);
//    std::copy((const char*)src, (const char*)src + count, (char*)dest);
}

TM_INLINE void MemSet(void* dest, char c, size_t count)
{
    DbgAssert(count == 0 ? true : (0 != dest));
    memset(dest, c, count);
//    std::fill_n((char*)dest, count, c);
}

TM_INLINE int MemCmp(const void* buf1, const void* buf2, size_t count)
{
    if (count == 0)
        return 0;
    DbgAssert(buf2 && buf1);
    return memcmp(buf1, buf2, count);
//     std::pair<const char*, const char*> pa =
//         std::mismatch((const char*)buf1, (const char*)buf1 + count, (const char*)buf2);
//     if (pa.first == (const char*)buf1 + count)
//         return 0;
//     return pa.first[0] > pa.second[0] ? 1 : -1;
}

#ifdef _MSC_VER
#   pragma warning(pop)
#endif

template <class T>
inline void MemZeroT(T& val)
{
    MemSet(&val, 0, sizeof(T));
}

template <class T>
inline void MemCpyT(T* dst, const T* src)
{
    struct _c { TMUInt8 _x[sizeof(T)]; };
    *(_c*)dst = *(const _c*)src;
}

#ifdef TM_DBG

struct _MemAllocInit
{
    TMDbgClassInfo ci;

    _MemAllocInit(size_t dwSize, const char* pTypeName, PFN_GetDebugInfo pfn)
    {
        MemZeroT(ci);
        TMDbgClassInfo_Init_typename(ci, pTypeName);
        ci.m_pfnInfo = pfn;
        DbgLeakOpt(&ci, eDbgLeakOptTypeDef);
    }

    void* Alloc(size_t sz) { return MemAlloc1(sz); }
    void* AllocBy(size_t sz, void* p) { return MemAllocBy1(sz, p); }
    void* Realloc(void* p, size_t sz) { return MemRealloc1(p,sz); }
};

#   define MemAlloc(sz,T)       (_MemAllocInit(sz,typeid(T).name(),0).Alloc(sz))
#   define MemAllocBy(sz,p,T)   (_MemAllocInit(sz,typeid(T).name(),0).AllocBy(sz,p))
#   define MemRealloc(p,sz,T)   (_MemAllocInit(sz,typeid(T).name(),0).Realloc(p,sz))
#   define MemAllocF(sz,T,f)    (_MemAllocInit(sz,typeid(T).name(),TMDbgClassInfo_func(f)).Alloc(sz)) // f: PFN_GetDebugInfo

#else
#   define MemAlloc(sz,T)       MemAlloc1(sz)
#   define MemAllocBy(sz,p,T)   MemAllocBy1(sz,p)
#   define MemRealloc(p,sz,T)   MemRealloc1(p,sz)
#   define MemAllocF(sz,T,f)    MemAlloc1(sz)
#endif

/* let alloc/free in same module. */
#define _TM_SELF_ALLOC(_Tx, f) \
    bool IC_IsValidInstance(TMBool bCheckThread = false) { return TM_IF_DBG(0 != TMDbgClassInfo_Check(this, bCheckThread)) TM_NO_DBG(true); } \
    bool IC_IsCreatorThread() { return TM_IF_DBG(IC_IsValidInstance)(true); } \
    void IC_SetID(TMUInt32 id) { TM_IF_DBG(TMDbgClassInfo_SetID(this, id)); } \
    void IC_CheckInstance() { DbgAssert(IC_IsValidInstance(true)); } \
    void* operator new(size_t n)    { return MemAllocF(n, _Tx, f); } \
    void operator delete(void* p)   { if (p) MemFree(p); }

#define TM_SELF_ALLOC(_Tx)          _TM_SELF_ALLOC(_Tx, 0)
#define TM_SELF_ALLOC_F(_Tx)        _TM_SELF_ALLOC(_Tx, &_Tx::IC_GetDebugInfo)


// TM_DECALRE_SELF_ALLOCATOR(tm_allocator,(MemAlloc(n, T)),MemFree(p));


// struct CAllocatorTM
// {
//     template <class T> struct A
//     {
//         static void* Alloc()        { return MemAlloc(sizeof(T), T); }
//         static void Free(void* p)   { if (p) MemFree(p); }
//     };
// };
//
// #define TM_SELF_ALLOC2(_Allocator,_Tx) \
//     void* operator new(size_t n)    { DbgAssert(n == sizeof(_Tx)); return _Allocator::A<_Tx>::Alloc(); } \
//     void operator delete(void* p)   { _Allocator::A<_Tx>::Free(p); }
//
// #define TM_SELF_ALLOC_POOL(_Tx)     TM_SELF_ALLOC2(CAllocatorTMPool, _Tx)


/** @} */




#ifdef TM_CPP

TM_BGN_NAMESPACE


///////////////////////////////////////////////////////////////////////////////
// io class

typedef std::ios::seekdir ESeekType;
#define e_stSet std::ios::beg
#define e_stCur std::ios::cur
#define e_stEnd std::ios::end


TM_END_NAMESPACE


#endif // TM_CPP


#endif /* end of #ifndef TMIO_H */
