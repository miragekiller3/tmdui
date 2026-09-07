/*
 *  @file
 *  @brief  CMemFile, CMemBuf; stream is a container that can read & write;
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2005.11.11
 *
 *  Copyright (C) 2005 miragekiller
 */

///////////////////////////////////////////////////////////////////////////////
//  3 modes of Mem management: 
//      1.CMemBufT<const T*>: bind a const pointer,readable and not writeable
//      2.CMemBufT<T*>: bind a non const pointer,readable and writeable, can not change the buffer size 
//      3.CMemFileT<T>: manage momery itself,readable and writeable and can change the file size
//  T must be char or BYTE
///////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef TMMEMFILE_H
#define TMMEMFILE_H

#ifdef __cplusplus

// #include <string.h> // for memcpy
// #include <stdlib.h> // for malloc / realloc / free

#include "tmio.h"
#include "tmcom.h"


///////////////////////////////////////////////////////////////////////////////
// helper class
TM_BGN_NAMESPACE

template<class T> class _CMemFileBase
{
public:
    typedef T*                                      iterator;
    typedef const T*                                const_iterator;
    
    typedef size_t                                  size_type;
    typedef ptrdiff_t                               difference_type;

    TM_UNUSED_IC_GetDebugInfo();

protected:
    iterator        m_pHead;
    size_type       m_dwSize;
    difference_type m_nPos;

    // type must be one of char*, byte*
    inline static void chk(const TMUInt8*) {}
    inline static void chk(const TMInt8*) {}

public:
    _CMemFileBase()
        : m_pHead(0)
        , m_dwSize(0)
        , m_nPos(0)
    {
        chk(m_pHead);
    }
    
    void Swap(_CMemFileBase& rhs)
    {
        tm_swap(m_pHead, rhs.m_pHead);
        tm_swap(m_dwSize, rhs.m_dwSize);
        tm_swap(m_nPos, rhs.m_nPos);
    }
    
    const_iterator GetHead() const { return m_pHead; }
    const_iterator GetCurr() const { return m_pHead + m_nPos; }
    const_iterator GetTail() const { return m_pHead + m_dwSize; }
    
    iterator GetHead() { return m_pHead; }
    iterator GetCurr() { return m_pHead + m_nPos; }
    iterator GetTail() { return m_pHead + m_dwSize; }
    
    size_type GetSize() const { return m_dwSize; }
    difference_type Tell() const { return m_nPos; }
    TMBool Eof() const { return Tell() >= GetSize(); }
    
    difference_type SeekSet(difference_type lOffset) { m_nPos = lOffset; return Tell(); }
    difference_type SeekCur(difference_type lOffset) { m_nPos += lOffset; return Tell(); }
    difference_type SeekEnd(difference_type lOffset) { m_nPos = m_dwSize + lOffset; return Tell(); }
    
    difference_type Skip(difference_type n) {
        difference_type pos = Tell(); SeekCur(n); return pos; }
    
    difference_type Seek(difference_type lOffset, ESeekType eSeekType)
    {
        difference_type nRet = 0;
        switch(eSeekType)
        {
        case e_stEnd: nRet = SeekEnd(lOffset); break;
        case e_stCur: nRet = SeekCur(lOffset); break;
        case e_stSet: nRet = SeekSet(lOffset); break;
        default: break;
        }
        return nRet;
    }
};



///////////////////////////////////////////////////////////////////////////////
// CMemBufT
template<class T> class CMemBufT : public _CMemFileBase<T>
{
public:
    typedef CMemBufT                                class_type;
    typedef _CMemFileBase<T>                        base_class;
    typedef typename base_class::iterator           iterator;
    typedef typename base_class::const_iterator     const_iterator;
    typedef void*                                   pointer;
    typedef const void*                             const_pointer;
    typedef size_t                                  size_type;
    typedef ptrdiff_t                               difference_type;
    
    using base_class::m_dwSize;
    using base_class::m_nPos;
    using base_class::m_pHead;
    using base_class::GetCurr;
    using base_class::SeekCur;
    using base_class::GetSize;

private:
    size_type _write(const_pointer pData, size_type dwSize, void*)
    {
//             CHECKMEMORY();
        DbgAssert(m_dwSize >= m_nPos + dwSize);
        MemCpy((void*)GetCurr(), pData, dwSize);
        SeekCur(dwSize);
//             CHECKMEMORY();
        return dwSize;
    }

    size_type _write(const_pointer, size_type, const void*)
    {
        return -1;
    }

    void _init(iterator p, size_type dwSize) {
        m_pHead = p; m_dwSize = dwSize; m_nPos = 0; }

public:
    CMemBufT(){}
    ~CMemBufT(){ CHECKMEMORY(); }

    CMemBufT(const_iterator p, size_type dwSize)
    {
        Assign(p, dwSize);
    }

    CMemBufT(const_iterator p1, const_iterator p2)
    {
        Assign(p1, p2);
    }

//         CMemBufT (const CRangeT<const_iterator>& rg)
//         {
//             Assign (rg);
//         }

    void Assign(const_iterator p, size_type dwSize)
    {
        m_pHead = const_cast<iterator>(p);
        m_dwSize = dwSize;
        m_nPos = 0;
    }
    
    void Assign(const_iterator p1, const_iterator p2)
    {
        return Assign(p1, p2 - p1);
    }
    
//         void Assign (const CRangeT<const_iterator>& rg)
//         {
//             return Assign (rg.GetHead(), rg.GetSize());
//         }
    
    size_type Read(pointer pData, size_type dwSize)
    {
        size_type nSize = GetSize();
        if (m_nPos >= nSize)
            return 0;
        if (m_nPos + dwSize > nSize)
            dwSize = nSize - m_nPos;

//             CHECKMEMORY();
        MemCpy(pData, GetCurr(), dwSize);
        SeekCur(dwSize);
        DbgAssert((size_type)m_nPos <= nSize);
//             CHECKMEMORY();
        return dwSize;
    }
    
    size_type Write(const_pointer pData, size_type dwSize)
    {
        return _write(pData, dwSize, m_pHead);
    }
};

typedef CMemBufT<TMUInt8>                          CMemBuf;


///////////////////////////////////////////////////////////////////////////////
// CMemFileT
template<class T> class CMemFileT : public _CMemFileBase<T>
{
public:
    typedef CMemFileT                               class_type;
    typedef _CMemFileBase<T>                        base_class;
    typedef typename base_class::iterator           iterator;
    typedef typename base_class::const_iterator     const_iterator;
    typedef void*                                   pointer;
    typedef const void*                             const_pointer;
    typedef size_t                                  size_type;
    typedef ptrdiff_t                               difference_type;
    
    using base_class::m_pHead;
    using base_class::m_dwSize;
    using base_class::m_nPos;
    using base_class::GetSize;
    using base_class::GetCurr;
    using base_class::SeekCur;
    using base_class::GetHead;

public:
    CMemFileT() {}

    CMemFileT(const CMemFileT& rhs)
    {
        Assign(rhs.m_pHead, rhs.m_dwSize);
    }
    
    CMemFileT(const_iterator p, size_type dwSize)
    {
        Assign(p, dwSize);
    }
    
    CMemFileT(const_iterator p1, const_iterator p2)
    {
        Assign(p1, p2 - p1);
    }
    
//         CMemFileT (const CRangeT<const_iterator>& rg)
//         {
//             Assign (rg.GetHead(), rg.GetSize());
//         }

    ~CMemFileT()
    {
        CHECKMEMORY();
        if (m_pHead)
            MemFree(m_pHead);
    }

    CMemFileT& operator = (const CMemFileT& rhs)
    {
        Assign(rhs.m_pHead, rhs.m_dwSize);
        return *this;
    }

    void Assign(const_iterator p, size_type dwSize)
    {
        m_pHead = (iterator)MemRealloc(m_pHead, dwSize, CMemFileT);
        MemCpy(m_pHead, p, dwSize);
        m_dwSize = dwSize;
        m_nPos = 0;
    }
    
    void Assign(const_iterator p1, const_iterator p2)
    {
        return Assign(p1, p2 - p1);
    }
    
//         void Assign (const CRangeT<const_iterator>& rg)
//         {
//             return Assign (rg.GetHead(), rg.GetSize());
//         }

    void Resize(size_type dwSize)
    {
        if (m_dwSize < dwSize)
            m_pHead = (iterator)MemRealloc(m_pHead, dwSize, CMemFileT);
        m_dwSize = dwSize;
    }

    size_type Read(pointer pData, size_type dwSize)
    {
        size_type nSize = GetSize();
        if (m_nPos >= nSize)
            return 0;
        if (m_nPos + dwSize > nSize)
            dwSize = nSize - m_nPos;

//             CHECKMEMORY();
        MemCpy(pData, GetCurr(), dwSize);
        SeekCur(dwSize);
        DbgAssert(m_nPos <= nSize);
//             CHECKMEMORY();
        return dwSize;
    }

    size_type Write(const_pointer pData, size_type dwSize)
    {
//             CHECKMEMORY();
        if (m_dwSize < m_nPos + dwSize)
            Resize(m_nPos + dwSize);
        MemCpy(GetCurr(), pData, dwSize);
        SeekCur(dwSize);
//             CHECKMEMORY();
        return dwSize;
    }

//         size_type Write (const CRangeT<const_iterator>& rg)
//         {
//             return Write (rg.GetHead (), rg.GetSize ());
//         }

    TMBool ReadFromStream(IStream* stream, size_t size = 0)
    {
        DbgAssert(stream);
        if (!size)
        {
            STATSTG statstg;
            if (stream->Stat(&statstg, 0) != S_OK)
                return false;
            size = statstg.cbSize.LowPart;
        }
        Resize(size);
        TMULong l = 0;
        stream->Read(GetHead(), size, &l);
        return l == size;
    }
};

typedef CMemFileT<TMUInt8>                          CMemFile;


TM_END_NAMESPACE


#endif // end of #ifdef __cplusplus

#endif // end of #ifndef _ATMEMFILE_H
