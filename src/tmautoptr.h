/**
 *  @file
 *  @brief  auto point operation 
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2002-09-02
 *
 *  Copyright (C) 2002 miragekiller                             
 */

#pragma once

#ifndef TMAUTOPTR_H
#define TMAUTOPTR_H

#include "tmdebug.h"


TM_BGN_NAMESPACE


// the object needn't release
struct CA_Null
{
    template < class T >
        inline void operator () (const T&) const {}
};

// use default destruct function
struct CA_Destruct
{
    template < class T >
        inline void operator () (T& val) const { val.~T(); }
};

// use method "delete" to release object
struct CA_Delete
{
    template < class T >
        inline void operator () (T& p) const { delete p; }
};

// use method "delete[]" to release object
struct CA_DeleteA
{
    template <class T>
        inline void operator () (T& p) const { delete [] p; }
};

// use to delete std::map iterator
// sample: std::for_each(pMap->begin(), pMap->end(), CA_DeletePair());
struct CA_DeletePair
{
    template < class T >
        inline void operator () (T& p) const { delete p.second; }
};

// use method "T->Relase()" to release object
struct CA_Release
{
    template < class T >
        inline void operator () (T& p) const { p.Release(); }
};

struct CA_ReleaseP
{
    template < class T >
        inline void operator () (T& p) const { if (p) p->Release(); }
};



template < class T, class T_Release = CA_Delete >
    class CAutoPtr
{
public:
    typedef T                                       value_type;
    typedef value_type*                             pointer;
    typedef const value_type                        const_pointer;

protected:
    pointer m_ptr;

public:
    explicit CAutoPtr(pointer ptr = 0) : m_ptr(ptr) {}
    CAutoPtr(const CAutoPtr& rhs) : m_ptr (((CAutoPtr&)rhs).Detach()) {}
    ~CAutoPtr() { T_Release()(m_ptr); }

    void Attach(pointer ptr)
    {
        DbgAssert(!m_ptr);
        m_ptr = ptr;
    }

    pointer Detach()
    {
        pointer old = m_ptr;
        m_ptr = 0;
        return old;
    }

    CAutoPtr& operator = (const CAutoPtr& rhs)
    {
        reset(((CAutoPtr&)rhs).Detach());
        return *this;
    }

    T* get() const { return m_ptr; }

    T* operator-> () const { DbgAssert(m_ptr); return m_ptr; }

    operator TMBool() const { return m_ptr != 0; }

//     pointer release () { return Detach (); }

    void reset(pointer ptr = 0)
    {
        if (m_ptr != ptr)
        {
            pointer ptemp = m_ptr;
            m_ptr = ptr;
            T_Release () (ptemp);
        }
    }
};

// template <class T>
// class CStaticInstanceCheckBaseT
// {
// public:
//     static T* g_pInstance;
// 
// protected:
//     CStaticInstanceCheckBaseT()
//     {
//         DbgAssert(!g_pInstance);
//         g_pInstance = static_cast<T*>(this);
//     }
// 
//     ~CStaticInstanceCheckBaseT()
//     {
//         DbgAssert(g_pInstance);
//         g_pInstance = 0;
//     }
// 
// public:
//     void Construct() {}
// 
//     static T* GetInstance()
//     {
//         return g_pInstance;
//     }
// 
//     static T* CheckGetInstance()
//     {
//         DbgAssert(g_pInstance);
//         return g_pInstance;
//     }
// };
// template <class T> T* CStaticInstanceCheckBaseT<T>::g_pInstance;
// 
// 
// template <class T>
// class CStaticInstanceBaseT : public CStaticInstanceCheckBaseT<T>
// {
// public:
//     using CStaticInstanceCheckBaseT<T>::g_pInstance;
// 
//     void Construct() {}
// 
//     static T* CreateInstance()
//     {
//         if (!g_pInstance)
//         {
//             new T();
//             DbgAssert(g_pInstance);
//             g_pInstance->Construct();
//         }
//         return g_pInstance;
//     }
// };


TM_END_NAMESPACE




#endif // end of TMAUTOPTR_H
