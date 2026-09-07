/**
 *  @file
 *  @brief
 *    atomic operator.<br>
 *    prevents more than one thread from using the same variable simultaneously;
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2009.11.06
 *
 *  Copyright (C) 2009 miragekiller
 */

#pragma once

#ifndef TMATOMIC_H
#define TMATOMIC_H

#include "tmio.h" // _TM_SELF_ALLOC


/**
 *  @addtogroup atomic
 *  @ingroup thread
 *  atomic "c" function prototype
 *  @par sample:
 *  @code
    // impl a thread safed IUnknown
    struct CUnKnown : public IUnknown
    {
        TMAtmInt m_nRef;
        CUnKnown () : m_nRef (0) {}
        virtual ~CUnKnown () {}
        TM_REFAPI AddRef () { return AtomicInc (&m_nRef); }
        TM_REFAPI Release () { if (AtomicDec(&m_nRef) == 0) delete this; }
    }
 *  @endcode
 *  @{
 */

// atomic "c" function prototype
#ifdef TM_DOXYGEN

/**
 * *pVal = n;
 * @return old *pVal
 */
TMRefInt AtomicSet(TMAtmInt* volatile pVal, TMInt n);

/**
 * *pVal += n;
 * @return new *pVal
 */
TMRefInt AtomicAdd(TMAtmInt* volatile pVal, TMInt n);

/**
 * ++*pVal;
 * @return new *pVal
 */
TMRefInt AtomicInc(TMAtmInt* volatile pVal);

/**
 * --*pVal;
 * @return new *pVal
 */
TMRefInt AtomicDec(TMAtmInt* volatile pVal);

/**
 * if (*pVal == nCompare) *pVal = nNew;
 * @return  true if the swap occured.
 */
bool AtomicCompareSet(TMAtmInt* volatile pVal, TMInt nCompare, TMInt nNew);

/**
 * lock for call once
 */
void OnceLock();

/**
 * unlock for call once
 */
void OnceUnlock();


#endif // #if TM_DOXYGEN


typedef TMInt32                             TMAtmInt;
typedef TMInt64                             TMAtmI64;
typedef TMULong                             TMRefInt;
#define TM_REFAPI                           TMRefInt TM_IF_WIN(STDMETHODCALLTYPE)


#if defined(TM_WIN) || defined(TM_MINGW)

#   include <windows.h>
#   include <unknwn.h>

#if defined(TM_MSC) && TM_MSC <= 1200
    inline LONG __InterlockedCompareExchange(LPLONG d, LONG e, LONG c) { return (LONG)::InterlockedCompareExchange((PVOID*)d, (PVOID)e, (PVOID)c); }
#else
#   define __InterlockedCompareExchange InterlockedCompareExchange
#endif

#endif


TM_INLINE TMAtmInt AtomicSet(TMAtmInt volatile* p, TMInt v)
{
#ifdef TM_WIN
    return InterlockedExchange((LONG*)p, v);
#elif defined(TM_GNUC)
    return __atomic_exchange_n((TMAtmInt*)p, v, __ATOMIC_SEQ_CST);
#else
#   error not support
#endif
}

inline TMAtmInt AtomicAdd(TMAtmInt volatile* p, TMInt v)
{
#ifdef TM_WIN
    return InterlockedExchangeAdd((LONG*)p, v);
#elif defined(TM_GNUC)
    return __atomic_add_fetch((TMAtmInt*)p, v, __ATOMIC_SEQ_CST);
#else
#   error not support
#endif
}

inline TMAtmInt AtomicInc(TMAtmInt volatile* p)
{
#ifdef TM_WIN
    return InterlockedIncrement((LONG*)p);
#elif defined(TM_GNUC)
    return AtomicAdd(p, 1);
#else
#   error not support
#endif
}

inline TMAtmInt AtomicDec(TMAtmInt volatile* p)
{
#ifdef TM_WIN
    return InterlockedDecrement((LONG*)p);
#elif defined(TM_GNUC)
    return AtomicAdd(p, -1);
#else
#   error not support
#endif
}

inline bool AtomicCompareSet(TMAtmInt volatile* p, TMInt comp, TMInt set)
{
#ifdef TM_WIN
    return comp == ::__InterlockedCompareExchange((LONG*)p, set, comp);
#elif defined(TM_GNUC)
    return __atomic_compare_exchange_n((TMAtmInt*)p, &comp, set, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
#else
#   error not support
#endif
}


#if !(defined(TM_MSC) && (TM_MSC < 1300))

TM_INLINE TMAtmI64 AtomicSet(TMAtmI64 volatile* p, TMAtmI64 v)
{
#ifdef TM_WIN
    return InterlockedExchange64((LONG64 volatile*)p, v);
#elif defined(TM_GNUC)
    return __atomic_exchange_n((TMAtmI64*)p, v, __ATOMIC_SEQ_CST);
#else
#   error not support
#endif
}

inline TMAtmI64 AtomicAdd(TMAtmI64 volatile* p, TMInt v)
{
#ifdef TM_WIN
    return InterlockedExchangeAdd64((LONG64 volatile*)p, v);
#elif defined(TM_GNUC)
    return __atomic_add_fetch((TMAtmI64*)p, v, __ATOMIC_SEQ_CST);
#else
#   error not support
#endif
}

inline TMAtmI64 AtomicInc(TMAtmI64 volatile* p)
{
#ifdef TM_WIN
    return InterlockedIncrement64((LONG64 volatile*)p);
#elif defined(TM_GNUC)
    return AtomicAdd(p, 1);
#else
#   error not support
#endif
}

inline TMAtmI64 AtomicDec(TMAtmI64 volatile* p)
{
#ifdef TM_WIN
    return InterlockedDecrement64((LONG64 volatile*)p);
#elif defined(TM_GNUC)
    return AtomicAdd(p, -1);
#else
#   error not support
#endif
}

inline bool AtomicCompareSet(TMAtmI64 volatile* p, TMInt comp, TMInt set)
{
#ifdef TM_WIN
    return comp == ::InterlockedCompareExchange64((LONG64 volatile*)p, set, comp);
#elif defined(TM_GNUC)
    return __atomic_compare_exchange_n((TMAtmI64*)p, &comp, set, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
#else
#   error not support
#endif
}

#endif // !(defined(TM_MSC) && (TM_MSC < 1300))



void OnceLock();
void OnceUnlock();


#ifndef TM_MSC

#   define S_OK                        ((ComResult)0x00000000L)
#   define S_FALSE                     ((ComResult)0x00000001L)
#   define E_OUTOFMEMORY               ((ComResult)0x8007000EL)
#   define E_INVALIDARG                ((ComResult)0x80070057L)
#   define E_NOINTERFACE               ((ComResult)0x80004002L)
#   define E_POINTER                   ((ComResult)0x80004003L)
#   define E_HANDLE                    ((ComResult)0x80070006L)
#   define E_ABORT                     ((ComResult)0x80004004L)
#   define E_FAIL                      ((ComResult)0x80004005L)
#   define E_ACCESSDENIED              ((ComResult)0x80070005L)
#   define E_PENDING                   ((ComResult)0x8000000AL)

#   define SUCCEEDED(Status)           ((ComResult)(Status) >= 0)
#   define FAILED(Status)              ((ComResult)(Status) < 0)

#endif


#ifdef TM_CPP


TM_BGN_NAMESPACE


/**
 *  @class CAtomicInt
 *  @brief packaged AtomicXxx function
 *  @par sample:
 *  @code
    // impl a thread safed IUnknown
    struct CUnKnown : public IUnknown
    {
        CAtomicInt m_nRef;
        virtual ~CUnKnown () {}
        LONG AddRef () { return ++m_nRef; }
        LONG Release () { if (--m_nRef == 0) delete this; }
    }
 *  @endcode
 */
template<int n> struct _CAtomic_Helper;
TM_TEMPLATE_NULL struct _CAtomic_Helper<sizeof(TMAtmInt)>   { typedef TMAtmInt type; typedef TMAtmInt int_type; };
TM_TEMPLATE_NULL struct _CAtomic_Helper<sizeof(TMAtmI64)>   { typedef TMAtmI64 type; typedef TMAtmI64 int_type; };

template <class T>
class CAtomicT
{
private:
    typedef typename _CAtomic_Helper<sizeof(T)>::type       _core_type;
    typedef typename _CAtomic_Helper<sizeof(T)>::int_type   _int_type;
    typedef T                                               value_type;

    volatile T m_nVal;

private:
    template <class _T> _T _NT(_T*, ...);
                      char _NT(long);
                      char _NT(void*);
#   define _def_step enum { eStep = sizeof(_NT((T)0)) }

public:
    /** */ CAtomicT (T n = 0) : m_nVal(n) {}
    
    /** */ operator T () const { return Get(); }
    /** */ value_type operator = (value_type n) { Set(n); return n; }
    /** */ value_type operator ++ () { return Inc(); }
    /** */ value_type operator -- () { return Dec(); }
    /** */ value_type operator += (long n) { return Add(n); }
    /** */ value_type operator -= (long n) { return Add(-n); }
    /** */ value_type Get() const { return (value_type)m_nVal; }
    
    /** val = n */
    value_type Set(value_type n) { return (value_type)AtomicSet((_core_type*)&m_nVal, (_int_type)n); }
    
    /** ++val */
    value_type Inc() { _def_step; return eStep == sizeof(char) ? (value_type)AtomicInc((_core_type*)&m_nVal) : Add(1); }
    
    /** --val */
    value_type Dec() { _def_step; return eStep == sizeof(char) ? (value_type)AtomicDec((_core_type*)&m_nVal) : Add(-1); }
    
    /** val += n */
    value_type Add(long n) { _def_step; return (value_type)AtomicAdd((_core_type*)&m_nVal, (_int_type)(n * eStep)); } // AtomicAdd((_core_type*)&m_nVal, n); }

    /** if(val==nComp) val = nSet */
    bool CompareSet(value_type nComp, value_type nSet) { return (value_type)AtomicCompareSet((_core_type*)&m_nVal, (_int_type&)nComp, (_int_type&)nSet); }

#   undef _def_step
};

typedef CAtomicT<int>    CAtomicInt;


/**
 *  @class CRefPtr
 *  @brief auto addref/release for a class
 *  @par sample:
 *  @code
    void xxx (IUnknown* p);
    {
        CRefPtr<IUnknown> ptr(p); // auto call p->AddRef();
        ...
        //after leave this function, CRefT auto call p->Release();
    }
 *  @endcode
 */

// _NoAddRefReleaseOnCRefPtr
template <class T>
class _CRefPtr_NoAddRefRelease : public T
{
private:
    ~_CRefPtr_NoAddRefRelease();
    TM_REFAPI AddRef();
    TM_REFAPI Release();
};

// CRefPtrBaseT
template <class T, bool t_bManaged, bool t_bHaveDestructor = true>
class CRefPtrBaseT
{
public:
    typedef T _PtrClass;
    typedef _CRefPtr_NoAddRefRelease<T>* out_pointer;
    template <bool> struct bool2typeT {};

    T* p;

protected:
    tm_constexpr11 CRefPtrBaseT()
        : p(0)
    {
    }

    tm_constexpr17 CRefPtrBaseT(T* lp)
        : p(lp)
    {
        if (lp && t_bManaged)
            lp->AddRef();
    }

    ~CRefPtrBaseT()
    {
        _Release(bool2typeT<0 != t_bManaged>(), p);
    }

    template <class _Ptr> inline void _Release(bool2typeT<false>, _Ptr* p) {}
    template <class _Ptr> inline void _Release(bool2typeT<true>, _Ptr*& p)
    {
        T* pTemp = p;
        if (pTemp)
        {
            p = 0;
            pTemp->Release();
        }
    }

public:
    void Release()
    {
        _Release(bool2typeT<true>(), p);
    }

    tm_constexpr11 operator T*() const
    {
        return p;
    }

    tm_constexpr11 T& operator*() const
    {
        DbgAssert(p!=NULL);
        return *p;
    }

    //The assert on operator& usually indicates a bug. If this is really
    //what is needed, however, take the address of the p member explicitly.
    tm_constexpr11 T** operator&()
    {
        DbgAssert(p==NULL);
        return &p;
    }

    tm_constexpr11 out_pointer operator->() const
    {
        DbgAssert(p!=NULL);
        return (out_pointer)p;
    }

    T* operator =(T* lp)
    {
        Assign(lp);
        return p;
    }

    tm_constexpr11 bool operator !() const
    {
        return (p == NULL);
    }

    tm_constexpr11 bool operator <(T* pT) const
    {
        return p < pT;
    }

    tm_constexpr11 bool operator ==(T* pT) const
    {
        return p == pT;
    }

    tm_constexpr11 bool operator !=(T* pT) const
    {
        return p != pT;
    }

    void Assign(T* lp)
    {
        if (lp != p)
        {
            T* pp = p;
            p = lp;
            if (t_bManaged)
            {
                if (lp)
                    lp->AddRef();
                if (pp)
                    pp->Release();
            }
        }
    }

    void Attach(T* p2)
    {
//         if (p2)
//             DbgAssert(p2 != p);
        if (p && t_bManaged)
            p->Release();
        p = p2;
    }

    T* Detach()
    {
        T* pt = p;
        p = NULL;
        return pt;
    }

    void Swap(CRefPtrBaseT& rhs)
    {
        tm_swap(p, rhs.p);
    }

    ComResult CopyTo(T** ppT)
    {
        DbgAssert(ppT != NULL);
        if (ppT == NULL)
            return E_POINTER;
        *ppT = p;
        if (p)
            p->AddRef();
        return S_OK;
    }
};

/* ref object class implement */
#define DECALRE_REFPTR_IMPL(CLS,T) \
    tm_constexpr11 CLS() {} \
    tm_constexpr11 CLS(T* lp) : base_class(lp) {} \
    tm_constexpr11 CLS(const CLS& lp) : base_class(lp.p) {} \
    T* operator = (T* lp) { return base_class::operator = (lp); } \
    T* operator = (const CLS& lp) { return base_class::operator = (lp.p); }

// template <class T, bool t_bHaveDestructor = true>
// class CRefPtrHandle : public CRefPtrBaseT<T, false, t_bHaveDestructor>
// {
// public:
//     typedef CRefPtrBaseT<T, false, t_bHaveDestructor>  base_class;
//     DECALRE_REFPTR_IMPL(CRefPtrHandle, T);
// };

template <class T, bool t_bHaveDestructor = true>
class CRefPtr : public CRefPtrBaseT<T, true, t_bHaveDestructor>
{
public:
    typedef CRefPtrBaseT<T, true, t_bHaveDestructor>  base_class;
    DECALRE_REFPTR_IMPL(CRefPtr, T);
};


/**
 *  @class CStaticRefPtr
 *  @brief auto addref/release for a static class
 *  @par sample:
 *  @code
    CStaticRefPtr<AAA> g_ptr;
    g_ptr->...
 *  @endcode
 */
// template <class T>
// class CStaticRefPtr
// {
// public:
//     typedef T _PtrClass;
//
// private:
//     TMAtmInt    m_ref;
//     TMUInt8     m_buf[sizeof(T)];
//     TM_IF_DBG(_PtrClass* m_pObjDebugView);
//
//     _PtrClass* SGetThis() const
//     {
//         return (_PtrClass*)m_buf;
//     }
//
//     TM_REFAPI SAddRef()
//     {
//         DbgAssert(m_ref <= INT_MAX);
//         DbgAssert(m_ref >= 0);
//         return AtomicInc(&m_ref);
//     }
//
//     TM_REFAPI SRelease()
//     {
//         DbgAssert(m_ref <= INT_MAX);
//         DbgAssert(m_ref > 0);
//         TMAtmInt x = AtomicDec(&m_ref);
//         if (x <= 0)
//             SGetThis()->~T();
//         return x;
//     }
//
// public:
//     TMAtmInt CreateInstance(_PtrClass** p)
//     {
//         _PtrClass* pt = SGetThis();
//         if (m_ref == 0)
//         {
// #ifdef TM_DBG
//             m_pObjDebugView = pt;
//             for (int i = 0; i < sizeof(m_buf) / sizeof(int); ++i)
//                 DbgAssert(((int*)pt)[i] == 0);
// #endif
//             pt->T();
//         }
//         *p = pt;
//         return SAddRef();
//     }
//
//     TMAtmInt GetInstance(_PtrClass** p)
//     {
//         if (m_ref)
//         {
//             *p = SGetThis();
//             return SAddRef();
//         }
//         return 0;
//     }
//
//     _PtrClass* GetInstanceNoAddRef()
//     {
//         _PtrClass* p = m_ref ? SGetThis() : 0;
//         return p;
//     }
//
//     _PtrClass* CheckGetInstanceNoAddRef()
//     {
//         DbgAssert(m_ref);
//         return SGetThis();
//     }
//
//     TM_REFAPI AddRef()
//     {
//         return SAddRef();
//     }
//
//     TM_REFAPI Release()
//     {
//         return SRelease();
//     }
//
//     _PtrClass* operator->() const { return CheckGetInstanceNoAddRef(); }
// };



/** @} */ // @addtogroup thread


///////////////////////////////////////////////////////////////////////////////
// class CRefObjectRootT
TM_BGN_PRIVATE_NAMESPACE
struct ref_helper
{
    template<class T> inline static TMRefInt GetRef(T* p)
    {
        return p->m_nRef;
    }

    template<class T> inline static TMRefInt AddRef(T* p)
    {
        DbgAssert(p->m_nRef <= INT_MAX);
        DbgAssert(p->m_nRef >= 0);
        return AtomicInc(&p->m_nRef);
    }

    template<class T> inline static TMRefInt Release(T* p)
    {
        DbgAssert(p->m_nRef <= INT_MAX);
        DbgAssert(p->m_nRef > 0);
        return AtomicDec(&p->m_nRef);
    }

    template<class T> inline static TMRefInt ReleaseAutoDelete(T* p)
    {
        DbgAssert(p->m_nRef <= INT_MAX);
        DbgAssert(p->m_nRef > 0);
        TMRefInt n = AtomicDec(&p->m_nRef);
        if (n <= 0)
            delete p;
        return n;
    }
};
TM_END_PRIVATE_NAMESPACE

template <int n = 1>
class CRefObjectRoot
{
protected:
    TMAtmInt m_nRef;
    friend PRI::ref_helper;

protected:
    CRefObjectRoot(int nRef = n) : m_nRef(nRef) {}

public:
    TMRefInt GetRef() const { return m_nRef; }
};


/**
 *  @class CBaseRefT
 *  @brief auto addref/release for a exist class, the new class have no virtual table
 *  @par sample:
 *  @code
    class CBase : public CBaseRefT<Base, TM::CRefObjectRoot<1>> { int a, b, c };
    class CBase : public CBaseRefT<Base, IBase>, public TM::CRefObjectRoot<1> { int a, b, c };
    CRefPtr<CBase> ptr = new CBase(); // create a new CBase instance that have reference count
 *  @endcode
 */
template <class T, class P = CRefObjectRoot<1> >
class TM_NO_VTABLE CBaseRefT : public P
{
private:
    T* _GetT() { return static_cast<T*>(this); }

public:
    typedef CBaseRefT CBaseRef;
    // let alloc/free in same module.
    TM_SELF_ALLOC_F(T);
    TM_UNUSED_IC_GetDebugInfo();

public:
    static void RealDeleteSelf(T* p) { delete p; }
    static void RealCheckReleseRef(T* p, TMRefInt n) {}

    TM_REFAPI AddRef()
    {
        return PRI::ref_helper::AddRef(_GetT());
    }

    TM_REFAPI Release()
    {
        T* p = _GetT();
        TMRefInt x = PRI::ref_helper::Release(p);
        T::RealCheckReleseRef(p, x);
        if (x <= 0)
            T::RealDeleteSelf(p);
        return x;
    }

    TMRefInt GetRef() const { return this->m_nRef; }
};


/**
 *  @class CRefObjectT
 *  @brief auto addref/release for a exist class
 *  @par sample:
 *  @code
    struct Base { int a, b, c };
    typedef CRefObjectT<Base> CBase;
    CRefPtr<CBase> ptr = new CBase(0); // create a new Base instance that have reference count
 *  @endcode
 */
template <class T, int nDftRef = 1>
class CRefObjectT : public CBaseRefT<CRefObjectT<T>, T>, public CRefObjectRoot<nDftRef>
{
public:
    CRefObjectT(int nRef = nDftRef) : CRefObjectRoot<nDftRef>(nRef) {}
};


/**
 *  @class CStaticRefObjectT
 *  @brief auto addref/release for a static class
 *  @par sample:
 *  @code
    struct Base : public CStaticRefObjectT<Base, 1, IUnknown> {}
    Base* p = Base::CreateInstance();
    p->...
    p->Release();
 *  @endcode
 */
struct CNullBase {};

template <class T, class P = CNullBase>
class TM_NO_VTABLE CStaticBaseRefT : public P
{
public:
    typedef CStaticBaseRefT            class_type;
    TM_CLASS_INSTANCE_CHECK(T);
    TM_NO_COPYABLE(CStaticBaseRefT);

private:
    static TMAtmInt s_ref;
    static TMUInt8  s_buf[];
    TM_IF_DBG(static T* s_pObjDebugView);

protected:
    void* operator new(size_t n)   { return s_buf; }
    void operator delete(void* p)  { DbgAssert(s_ref == 0); }

    CStaticBaseRefT()
    {
        DbgTrace("static %s::%s()", typeid(T).name(), typeid(T).name());
    }

    ~CStaticBaseRefT()
    {
        DbgAssert(s_ref == 0);
        DbgTrace("static %s::~%s()", typeid(T).name(), typeid(T).name());
    }

private:
    static T* SGetThis()
    {
        return (T*)s_buf;
    }
    
    static TMRefInt SGetRef() { return s_ref; }
    
    static TM_REFAPI SAddRef()
    {
        DbgAssert(s_ref <= INT_MAX);
        DbgAssert(s_ref >= 0);
        return AtomicInc(&s_ref);
    }
    
    static TM_REFAPI SRelease()
    {
        DbgAssert(s_ref <= INT_MAX);
        DbgAssert(s_ref > 0);
        TMRefInt x = AtomicDec(&s_ref);
        if (x <= 0)
            delete SGetThis();
        return x;
    }

public:
    static TMRefInt InstanceInit()
    {
        TMRefInt n = SAddRef();
        if (n == 1)
            new T();
        return n;
   }

    static TMRefInt InstanceTerm()
    {
        return SRelease();
    }

    static TMRefInt CreateInstance(T** p)
    {
        TMRefInt n = SAddRef();
        if (n == 1)
            new T();
        *p = SGetThis();
        return n;
    }

    static TMRefInt GetInstance(T** p)
    {
        if (s_ref)
        {
            *p = SGetThis();
            return SAddRef();
        }
        return 0;
    }

    static T* GetInstanceAlwaysAddRef()
    {
        TMRefInt n = SAddRef();
        if (n == 1)
            return new T();
        return SGetThis();
    }

    static T* GetInstanceNoAddRef()
    {
        T* p = s_ref ? SGetThis() : 0;
        return p;
    }

    static T* CheckGetInstanceNoAddRef()
    {
        DbgAssert(s_ref);
        return SGetThis();
    }

    TM_REFAPI AddRef()
    {
        DbgAssert(this == SGetThis());
        return SAddRef();
    }

    TM_REFAPI Release()
    {
        DbgAssert(this == SGetThis());
        return SRelease();
    }
};

template <class T, class P> TMAtmInt   CStaticBaseRefT<T, P>::s_ref;
template <class T, class P> TMUInt8    CStaticBaseRefT<T, P>::s_buf[sizeof(T)];
#ifdef TM_DBG
template <class T, class P>
T* CStaticBaseRefT<T, P>::s_pObjDebugView = (T*)CStaticBaseRefT<T, P>::s_buf;
#endif



// bThreadSafe valid only bAutoClean == true;
// ref:
//  0: not inited; 1: creating; 2: have create. if ref == 2 destroy obj; then set ref = 0;
//  bAutoClean mode
//    true:  only create once on a process; auto ++ref when created; auto --ref when exit; 
//    false: can create->release->create->release...; 
template <class T, bool bAutoClean, class P = CNullBase, bool bThreadSafe = false>
class TM_NO_VTABLE CStaticBaseRefT2 : public P
{
public:
    typedef CStaticBaseRefT2            class_type;
    typedef CStaticBaseRefT2            ref_class;
    TM_CLASS_INSTANCE_CHECK(T);
    TM_NO_COPYABLE(CStaticBaseRefT2);
    enum { eRefMin = 2 }; 

private:
    static volatile TMAtmInt    s_ref;
    static TMUInt8              s_buf[];
    TM_IF_DBG(static T*         s_pObjDebugView);

protected:
    void* operator new(size_t n);
    void operator delete(void* p){}

    CStaticBaseRefT2()
    {
        DbgTrace("static %s::%s()", typeid(T).name(), typeid(T).name());
    }

    ~CStaticBaseRefT2()
    {
        DbgTrace("static %s::~%s()", typeid(T).name(), typeid(T).name());
    }

private:
    static T* SGetThis()
    {
        return (T*)s_buf;
    }

    static TM_REFAPI SAddRef()
    {
        DbgAssert(s_ref <= INT_MAX);
        DbgAssert(s_ref >= 0);
        return AtomicInc(&s_ref);
    }
    
    static TM_REFAPI SRelease()
    {
        DbgAssert(s_ref <= INT_MAX);
        DbgAssert(s_ref > eRefMin);
        TMAtmInt x = AtomicDec(&s_ref);
        if (x <= eRefMin)
        {
            T* p = SGetThis();
            x = AtomicDec(&s_ref);
            p->~T();
        }
        return x;
    }

    static void __Create()
    {
        DbgAssert(s_ref == 0);
        DbgAssert(!s_pObjDebugView);
        SAddRef();
        T* p = SGetThis();
        TM_D(s_pObjDebugView = p);
        ::new (p) T();
        if (bAutoClean)
        {
            atexit(T::CStaticBaseRefT_OnExit);
            AtomicAdd(&s_ref, 2);
        }
        else
            SAddRef();
    }

    // if create add ref; if get not add ref.
    static T* _Instance()
    {
        if (bThreadSafe && bAutoClean && s_ref < eRefMin)
        {
            OnceLock();
            if (s_ref == 0)
                __Create();
            OnceUnlock();
        }
        else if (s_ref == 0)
            __Create();
        DbgAssert(s_ref >= 1);
        return SGetThis();
    }

public:
    static void CStaticBaseRefT_OnExit()
    {
        SRelease();
    }

public:
    // can called any mode.
    // new instance: ref = AutoClean ? 2 : 1;
    // get instance: ++ref;
    template <class T_Out>
    static void Instance(T_Out** ppout)
    {
        T* p = _Instance();
        p->AddRef();
        *ppout = _Instance();
    }

    // only can called auto clean mode.
    // new instance: ref = 1;
    // get instance: ref no change;
    static T* Instance()
    {
        DbgAssertS(bAutoClean);
        return _Instance();
    }

    static T* GetInstanceNoAddRef()
    {
        T* p = s_ref ? SGetThis() : 0;
        return p;
    }
    
    static T* CheckGetInstanceNoAddRef()
    {
        DbgAssert(s_ref);
        return SGetThis();
    }

    TM_REFAPI AddRef()
    {
        DbgAssert(this == SGetThis());
        return SAddRef();
    }

    TM_REFAPI Release()
    {
        DbgAssert(this == SGetThis());
        return SRelease();
    }
};

template<class T, bool b, class P, bool bts> volatile TMAtmInt CStaticBaseRefT2<T, b, P, bts>::s_ref;
template<class T, bool b, class P, bool bts> TMUInt8  CStaticBaseRefT2<T, b, P, bts>::s_buf[sizeof(T)];
#ifdef TM_DBG
template<class T, bool b, class P, bool bts> T*       CStaticBaseRefT2<T, b, P, bts>::s_pObjDebugView;
#endif



// this function for debug;
// you can put break point to AddRef and Release;
template <class T>
class CDebugRefObjectT : public CRefObjectT<T>
{
public:
    CDebugRefObjectT(TMRefInt n = 1)
        : CRefObjectT<T> (n)
    {}

    TM_REFAPI AddRef ()
    {
        return CRefObjectT<T>::AddRef();
    }

    TM_REFAPI Release()
    {
        return CRefObjectT<T>::Release();
    }
};


TM_END_NAMESPACE


#endif // TM_CPP

#endif // TMATOMIC_H

