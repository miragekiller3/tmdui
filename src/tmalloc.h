/**
 *  @file
 *  @brief  memory management 
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2007-12-31
 *
 *  Copyright (C) 2007 miragekiller                             
 */

#pragma once

#ifndef TMALLOC_H
#define TMALLOC_H

#include <algorithm>
#include "tmio.h"


TM_BGN_NAMESPACE


template <class T>
struct CRealloc : public CNoCopyable
{
private:
    TM_SELF_ALLOC(void);

public:
    typedef T* iterator;
    typedef const T* const_itertor;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;

    T* m_p;

    CRealloc() : m_p(0) {}

    CRealloc(int n) : m_p((T*)MemAlloc(n * sizeof(T), CRealloc<T>)) {}

    ~CRealloc() { MemFree(m_p); }

    operator T* () { return m_p; }

    operator const T* () const { return m_p; }

    void resize(size_t n)
    {
        DbgAssert(n);
        m_p = (T*)MemRealloc(m_p, sizeof(T) * n, CRealloc<T>);
    }

    void reset()
    {
        MemFree(m_p);
    }

    void assign(const_itertor first, size_type n)
    {
        resize(n);
        std::copy(first, first + n, begin());
    }

    void assign(const_itertor first, const_itertor last)
    {
        resize(last - first);
        std::copy(first, last, begin());
    }

    void assign(size_type n, const T& x = T())
    {
        resize(n);
        std::fill(begin(), end(n), x);
    }

    iterator begin() { return m_p; }
    const_itertor begin() const { return m_p; }
    iterator end(int n) { return m_p + n; }
    const_itertor end(int n) const { return m_p + n; }
};


TM_END_NAMESPACE



#endif // TMALLOC_H
