/**
 *  @file
 *  @brief  com object 
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2015-12-04
 *
 *  Copyright (C) 2015 miragekiller                             
 */

#pragma once

#ifndef TMCOM_H
#define TMCOM_H

#include "tmatomic.h"

struct _GUID;
template <class T> struct CUUIDOF { const static _GUID iid; };

// mingw::__mingw_uuidof is not constexpr, not support CComQIPtr, so replace it.
#ifdef TM_WIN
#   include <objidl.h>
#   if (defined(_INC__MINGW_H) ^ defined(__CRT_UUID_DECL))
#       error "mingw must define __CRT_UUID_DECL"
#   endif // _INC__MINGW_H ~
#   ifdef __CRT_UUID_DECL
#       if defined(__objidl_h__)
//#           error "please include tmcom.h first"
#       endif // defined(__objidl_h__)
#       undef __CRT_UUID_DECL
#       define __CRT_UUID_DECL(cls,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
            struct cls;  extern "C++" { \
            template <> __attribute__((weak)) const GUID CUUIDOF<cls>::iid = {l,w1,w2, {b1,b2,b3,b4,b5,b6,b7,b8}}; }
#       undef __uuidof
#       define __uuidof(x)     CUUIDOF<x>::iid
#   endif // __CRT_UUID_DECL
#endif // TM_WIN

#if TM_USE_MS_ATL
#   include <atlbase.h>
#endif


//#define COMTRACE DbgTrace
#define COMTRACE if (0) DbgTrace



///////////////////////////////////////////////////////////////////////////////
// IUnknown
#define TM_UNUSED_ADDREF_RELEASE()      \
    TM_REFAPI AddRef  () tm_override { return 1; }  \
    TM_REFAPI Release () tm_override { return 1; }

#define TM_UNUSED_ADDREF_RELEASE_NO_OVERRIDE()      \
    TM_REFAPI AddRef  () { return 1; }  \
    TM_REFAPI Release () { return 1; }

#define TM_VIRTUAL_ADDREF_RELEASE()     \
    TM_REFAPI AddRef  () = 0;           \
    TM_REFAPI Release () = 0;

#define TM_INTERFACE_HANDLER_EX2(t1,t2,uid) \
    if (uid == iid) \
    { \
        t1* pbase = this;   \
        t2* p = pbase; \
        *ppvObject = p; \
        p->AddRef(); \
        return S_OK; \
    } \

// M: member
#define TM_INTERFACE_HANDLER_M(t1,member) \
    if ((member) && (TM_UUID_OF(t1) == iid)) \
    { \
        t1* p = member;   \
        *ppvObject = p; \
        p->AddRef(); \
        return S_OK; \
    } \

// QM: query member
#define TM_INTERFACE_HANDLER_QM(t1,member) \
    if ((member) && (TM_UUID_OF(t1) == iid)) \
    { \
        return (member)->QueryInterface(iid, ppvObject); \
    } \


#if defined(RPC_USE_NATIVE_WCHAR) && defined(_NATIVE_WCHAR_T_DEFINED)
    typedef TMStr TMPStrRPC;
#else
    typedef TM_SW_UNICODE(unsigned short*, unsigned char *) TMPStrRPC;
#endif

#define TM_INTERFACE_HANDLER2(t1,t2)        TM_INTERFACE_HANDLER_EX2(t1, t2, TM_UUID_OF(t2))

#define TM_INTERFACE_HANDLER(t)             TM_INTERFACE_HANDLER2(t,t)

#define TM_INTERFACE_HANDLER_EX(t,uid)      TM_INTERFACE_HANDLER_EX2(t, t, uid)

#define TM_INTERFACE_FUNC(func)             if (S_OK == func(iid, ppvObject)) return S_OK;
#define TM_INTERFACE_MAP(base_class)        TM_INTERFACE_FUNC(base_class::_InternalQueryInterface) // if (S_OK == base_class::_InternalQueryInterface(iid, ppvObject)) return S_OK;
#define TM_INTERFACE_MAP_MEMBER(m)          if ((m) && (S_OK == (m)->QueryInterface(iid, ppvObject))) return S_OK;

#define TM_BGN_INTERFACE_()           \
    STDMETHODIMP QueryInterface(REFIID iid, void** ppvObject) tm_override \
    { \
        if (S_OK == this->_InternalQueryInterface(iid, ppvObject)) \
            return S_OK; \
        TM_INTERFACE_HANDLER_M(IUnknown, this->GetUnknown()); \
        *ppvObject = 0; \
        return E_NOINTERFACE; \
    } \
    ComResult _InternalQueryInterface(REFIID iid, void ** ppvObject) { \


#define TM_BGN_INTERFACE_MAP(t1)        \
    TMRefInt _GetRef() { GetUnknown()->AddRef(); return GetUnknown()->Release(); } \
    IUnknown* _GetRawUnknown() { t1* pbase = this; IUnknown* p = pbase; return p; } \
    IUnknown* GetUnknown() { return _GetRawUnknown(); } \
    TM_BGN_INTERFACE_()

#define TM_END_INTERFACE_MAP()          return E_NOINTERFACE; }

#define TM_END_INTERFACE_MAP_NOUNK()    return E_NOINTERFACE; }

#define TM_BGN_INTERFACE_MAP_QM(member) \
    STDMETHODIMP QueryInterface(REFIID iid, void** ppvObject) tm_override \
    { \
        if(member) \
            return member->QueryInterface(iid, ppvObject); \
        *ppvObject = 0; \




///////////////////////////////////////////////////////////////////////////////
// UUID
#ifndef GUID_DEFINED
#define GUID_DEFINED
    typedef struct _GUID
    {
        TMUInt32   Data1;
        unsigned short Data2;
        unsigned short Data3;
        unsigned char Data4[8];
    } GUID;

    typedef GUID IID;
    typedef GUID CLSID;
#   define REFIID                      const IID &

    typedef union _LARGE_INTEGER
    {
        struct {
            TMUInt32 LowPart;
            TMInt32 HighPart;
        };
        struct {
            TMUInt32 LowPart;
            TMInt32 HighPart;
        } u;
        TMInt64 QuadPart;
    } LARGE_INTEGER;
    
    typedef LARGE_INTEGER *PLARGE_INTEGER;
    
    
    typedef union _ULARGE_INTEGER
    {
        struct {
            TMUInt32 LowPart;
            TMUInt32 HighPart;
        };
        struct {
            TMUInt32 LowPart;
            TMUInt32 HighPart;
        } u;
        TMUInt64 QuadPart;
    } ULARGE_INTEGER;
    
    typedef ULARGE_INTEGER *PULARGE_INTEGER;
    
    typedef struct  tagSTATSTG
    {
        TMULong type;
        ULARGE_INTEGER cbSize;
    } STATSTG;
    
    typedef enum tagSTGTY
    {
        STGTY_STORAGE    = 1,
            STGTY_STREAM    = 2,
            STGTY_LOCKBYTES    = 3,
            STGTY_PROPERTY    = 4
    }    STGTY;
    
    typedef enum tagSTREAM_SEEK
    {
        STREAM_SEEK_SET    = 0,
            STREAM_SEEK_CUR    = 1,
            STREAM_SEEK_END    = 2
    }    STREAM_SEEK;

#endif // GUID_DEFINED

#ifdef TM_MSC
#   define TM_UUIDOF_MSC
# elif defined(TM_GNUC) && (TM_GNUC >= 600) && defined(TM_WIN)
#   define TM_UUIDOF_GCC
# else
#   define TM_UUIDOF_NONE
#endif // TM_MSC

#ifdef TM_UUIDOF_NONE
    //__attribute__((weak)) ?
//     template <class T> struct CUUIDOF { const static GUID iid; };
//    template <class T> const GUID CUUIDOF<T>::iid = {0};

#   define TM_DECLARE_UUID(cls, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        struct cls; \
        template <> __attribute__((weak)) const GUID CUUIDOF<cls>::iid = { 0x##l, 0x##w1, 0x##w2, { 0x##b1, 0x##b2, 0x##b3, 0x##b4, 0x##b5, 0x##b6, 0x##b7, 0x##b8 } }

#   define TM_DECLARE_UUID_NS(ns, cls, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        struct cls; } \
        template <> __attribute__((weak)) const GUID CUUIDOF<ns::cls>::iid = { 0x##l, 0x##w1, 0x##w2, { 0x##b1, 0x##b2, 0x##b3, 0x##b4, 0x##b5, 0x##b6, 0x##b7, 0x##b8 } }; \
        namespace ns {

#   define TM_UUID_OF(cls) CUUIDOF<cls>::iid

#else // TM_UUIDOF_NONE

#   ifdef TM_UUIDOF_GCC
#       define TM_DECLARE_UUID(cls, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
            struct __declspec(uuid(#l "-" #w1 "-" #w2 "-" #b1 #b2 "-" #b3 #b4 #b5 #b6 #b7 #b8)) cls
#   else
#       define TM_DECLARE_UUID(cls, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
            struct __declspec(uuid(TM_STRING_OF(l##-##w1##-##w2##-##b1##b2##-##b3##b4##b5##b6##b7##b8))) cls
#   endif // TM_UUIDOF_GCC

#   define TM_DECLARE_UUID_NS(ns, cls, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        TM_DECLARE_UUID(cls, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)

#   define TM_UUID_OF(cls) __uuidof(cls)

#   define DECLARE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        const GUID name = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

#endif // TM_UUIDOF_NONE else



#if defined(TM_WIN) || defined(TM_MINGW)

#   include <objidl.h>

#else

#   define STDMETHODCALLTYPE
#   define STDMETHOD(method)            virtual ComResult STDMETHODCALLTYPE method
#   define STDMETHOD_(type,method)      virtual type STDMETHODCALLTYPE method
#   define STDMETHODIMP                 ComResult STDMETHODCALLTYPE
#   define STDMETHODIMP_(type)          type STDMETHODCALLTYPE

    TM_INLINE bool operator == (const GUID& guidOne, const GUID& guidOther)
    {
        return !MemCmp(&guidOne, &guidOther, sizeof(GUID));
    }



TM_DECLARE_UUID(IUnknown,      00000000, 0000, 0000, c0, 00, 00, 00, 00, 00, 00, 46);
struct IUnknown
{
public:
    virtual ComResult QueryInterface(REFIID riid, void **ppvObject) = 0;
    virtual TM_REFAPI AddRef(void) = 0;
    virtual TM_REFAPI Release(void) = 0;
};

TM_DECLARE_UUID(ISequentialStream, 0c733a30, 2a1c, 11ce, ad, e5, 00, aa, 00, 44, 77, 3d);
struct ISequentialStream : public IUnknown
{
public:
    virtual ComResult Read(void *pv, TMULong cb, TMULong *pcbRead) = 0;
    virtual ComResult Write(const void *pv, TMULong cb, TMULong *pcbWritten) = 0;
};

TM_DECLARE_UUID(IStream,      0000000c, 0000, 0000, c0, 00, 00, 00, 00, 00, 00, 46);
struct IStream : public ISequentialStream
{
public:
    virtual ComResult Seek(LARGE_INTEGER dlibMove, COMDword dwOrigin, ULARGE_INTEGER *plibNewPosition) = 0;
    virtual ComResult SetSize(ULARGE_INTEGER libNewSize) = 0;
    virtual ComResult CopyTo(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten) = 0;
    virtual ComResult Commit(COMDword grfCommitFlags) = 0;
    virtual ComResult Revert() = 0;
    virtual ComResult LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, COMDword dwLockType) = 0;
    virtual ComResult UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, COMDword dwLockType) = 0;
    virtual ComResult Stat(STATSTG *pstatstg, COMDword grfStatFlag) = 0;
    virtual ComResult Clone(IStream **ppstm) = 0;
};

#endif // #ifndef TM_WIN

#if (!TM_USE_MS_ATL)


TM_BGN_NAMESPACE

#   define CComPtr      TM::CRefPtr
#   define CComQIPtr    TM::CComQIPtrT

template <class T, const IID* piid = &TM_UUID_OF(T)>
class CComQIPtrT : public CComPtr<T>
{
public:
    CComQIPtrT() tm_noexcept {}

    CComQIPtrT(T* lp) tm_noexcept
        : CComPtr<T>(lp)
    {
    }

    CComQIPtrT(const CComQIPtrT<T,piid>& lp) tm_noexcept
        : CComPtr<T>(lp.p)
    {
    }

    CComQIPtrT(IUnknown* lp) tm_noexcept
    {
        if (lp != NULL)
        {
            if (FAILED(lp->QueryInterface(*piid, (void **)&this->p)))
                this->p = NULL;
        }
    }

    T* operator = (T* lp) tm_noexcept
    {
        if (*this!=lp)
        {
            CComQIPtrT(lp).Swap(*this);
        }
        return *this;
    }

    T* operator=(const CComQIPtrT<T,piid>& lp) tm_noexcept
    {
        if (*this!=lp)
        {
            CComQIPtrT(lp).Swap(*this);
        }
        return *this;
    }

    T* operator=(IUnknown* lp) tm_noexcept
    {
        if (*this!=lp)
        {
            this->Release();
            lp->QueryInterface(*piid, (void **)&this->p);
        }
        return *this;
    }
};

TM_END_NAMESPACE

#endif // #if !TM_USE_MS_ATL


TM_BGN_NAMESPACE


////////////////////////////////////////////////////////////////////////////////
// LessGUID
struct LessGUID
{
    bool operator () (const GUID& rguid1, const GUID& rguid2) const
    {
        return MemCmp(&rguid1, &rguid2, sizeof(GUID)) < 0;
    }
};

////////////////////////////////////////////////////////////////////////////////
// CStream
class TM_NO_VTABLE IStreamImpl : public IStream
{
public:
    TM_UNUSED_ADDREF_RELEASE();

    TM_BGN_INTERFACE_MAP(IStream)
        TM_INTERFACE_HANDLER(IStream)
        TM_INTERFACE_HANDLER(ISequentialStream)
    TM_END_INTERFACE_MAP()

    ComResult STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize) tm_override
    {
        return E_FAIL;
    }

    ComResult STDMETHODCALLTYPE CopyTo(IStream *pstm,
        ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten) tm_override
    {
        return E_FAIL;
    }

    ComResult STDMETHODCALLTYPE Commit(COMDword grfCommitFlags) tm_override
    {
        return E_FAIL;
    }

    ComResult STDMETHODCALLTYPE Revert() tm_override
    {
        return E_FAIL;
    }

    ComResult STDMETHODCALLTYPE LockRegion(
        ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, COMDword dwLockType) tm_override
    {
        return E_FAIL;
    }

    ComResult STDMETHODCALLTYPE UnlockRegion(
        ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, COMDword dwLockType) tm_override
    {
        return E_FAIL;
    }

    ComResult STDMETHODCALLTYPE Stat(STATSTG *pstatstg, COMDword grfStatFlag) tm_override
    {
        return E_FAIL;
    }

    ComResult STDMETHODCALLTYPE Clone(IStream **ppstm) tm_override
    {
        return E_FAIL;
    }

    virtual ComResult STDMETHODCALLTYPE Read(void *pv, TMULong cb, TMULong *pcbRead) tm_override
    {
        return E_FAIL;
    }

    virtual ComResult STDMETHODCALLTYPE Write(const void *pv, TMULong cb, TMULong *pcbWritten) tm_override
    {
        return E_FAIL;
    }

    virtual ComResult STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove, COMDword dwOrigin, ULARGE_INTEGER *plibNewPosition) tm_override
    {
        return E_FAIL;
    }
};


struct LLINT
{
    LARGE_INTEGER m;
    LLINT (TMInt64 n = 0) { m.QuadPart = n; }
    operator LARGE_INTEGER() const { return m; }
    LLINT& operator = (LARGE_INTEGER rhs) { m.QuadPart = rhs.QuadPart; return *this; }
    LLINT& operator = (ULARGE_INTEGER rhs) { m.QuadPart = rhs.QuadPart; return *this; }
    LLINT& operator = (TMInt64 rhs) { m.QuadPart = rhs; return *this; }
    LLINT& operator += (LARGE_INTEGER rhs) { m.QuadPart += rhs.QuadPart; return *this; }
    LLINT& operator += (ULARGE_INTEGER rhs) { m.QuadPart += rhs.QuadPart; return *this; }
    LLINT& operator += (TMInt64 rhs) { m.QuadPart += rhs; return *this; }
    LLINT operator + (LARGE_INTEGER rhs) const { return m.QuadPart + rhs.QuadPart; }
    LLINT operator + (ULARGE_INTEGER rhs) const { return m.QuadPart + rhs.QuadPart; }
    LLINT operator + (TMInt64 rhs) const { return m.QuadPart + rhs; }
    LARGE_INTEGER* operator &() { return &m; }
};

struct ULLINT
{
    ULARGE_INTEGER m;
    ULLINT (TMUInt64 n = 0) { m.QuadPart = n; }
    operator ULARGE_INTEGER() const { return m; }
    ULLINT& operator += (int n) { m.QuadPart += n; return *this; }
    ULARGE_INTEGER* operator &() { return &m; }
};

template <class T>
class CStreamT
    : public IStreamImpl
    , public T
{
public:
//     ComResult STDMETHODCALLTYPE QueryInterface (REFIID iid, void ** ppvObject)
//     {
//         if
//         return E_NOTIMPL;
//     }
// 
//     TM_UNUSED_ADDREF_RELEASE();
// 
//     TM_BGN_INTERFACE_MAP(IStream)
//         TM_INTERFACE_HANDLER(IStream)
//         TM_INTERFACE_HANDLER(ISequentialStream)
//     TM_END_INTERFACE_MAP()

    virtual ComResult STDMETHODCALLTYPE Read(void *pv, TMULong cb, TMULong *pcbRead) tm_override
    {
        int nTmp = T::Read (pv, cb);
        if (pcbRead)
            *pcbRead = nTmp;
        return nTmp >= 0 ? S_OK : E_FAIL;
    }

    virtual ComResult STDMETHODCALLTYPE Write(const void *pv, TMULong cb, TMULong *pcbWritten) tm_override
    {
        int nTmp = T::Write (pv, cb);
        if (pcbWritten)
            *pcbWritten = nTmp;
        return nTmp >= 0 ? S_OK : E_FAIL;
    }

    // STREAM_SEEK_SET, STREAM_SEEK_CUR, STREAM_SEEK_END
    virtual ComResult STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove,
        COMDword dwOrigin, ULARGE_INTEGER *plibNewPosition) tm_override
    {
        int nTmp = T::Seek ((int)dlibMove.QuadPart, (ESeekType)dwOrigin);
        if (plibNewPosition)
            plibNewPosition->QuadPart = nTmp;
        return S_OK;
    }

//     virtual ComResult STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize) tm_override
//     {
//         return E_FAIL;
//     }
// 
//     virtual ComResult STDMETHODCALLTYPE CopyTo(IStream *pstm,
//         ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten) tm_override
//     {
//         return E_FAIL;
//     }
// 
//     virtual ComResult STDMETHODCALLTYPE Commit(COMDword grfCommitFlags) tm_override
//     {
//         return E_FAIL;
//     }
// 
//     virtual ComResult STDMETHODCALLTYPE Revert() tm_override
//     {
//         return E_FAIL;
//     }
// 
//     virtual ComResult STDMETHODCALLTYPE LockRegion(
//         ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, COMDword dwLockType) tm_override
//     {
//         return E_FAIL;
//     }
// 
//     virtual ComResult STDMETHODCALLTYPE UnlockRegion(
//         ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, COMDword dwLockType) tm_override
//     {
//         return E_FAIL;
//     }

    virtual ComResult STDMETHODCALLTYPE Stat(STATSTG *pstatstg, COMDword grfStatFlag) tm_override
    {
        if (pstatstg)
        {
            MemZeroT(*pstatstg);
            pstatstg->type = STGTY_STREAM;
            pstatstg->cbSize.QuadPart = T::GetSize();
            //pstatstg->mtime = fstatus.m_mtime;
            //pstatstg->ctime = fstatus.m_ctime;
            //pstatstg->atime = fstatus.m_atime;
            //pstatstg->grfMode = ;
//             pstatstg->grfLocksSupported = 0;     // no locking supported
            return S_OK;
        }
        return E_FAIL;
    }

//     virtual ComResult STDMETHODCALLTYPE Clone(IStream **ppstm) tm_override
//     {
//         return E_FAIL;
//     }
};


////////////////////////////////////////////////////////////////////////////////
// IEnum* impl
// sample:
//  STDMETHODIMP CBaseFilter2::EnumPins(IEnumPins **ppEnum)
//  {
//     *ppEnum = new CEnumImpl1<IEnumPins, CEnumContInterface<IPin> >(m_pin);
//     return *ppEnum == NULL ? E_OUTOFMEMORY : NOERROR;
//  }
template <class T, class T_IEnumInterface, class T_Val>
class CEnumImplBaseT
    : public TM::CBaseRefT<T, T_IEnumInterface>
    , public TM::CRefObjectRoot<1>
{
public:
    typedef T_Val value_type;

    int m_nPos;

    TM_BGN_INTERFACE_MAP(T_IEnumInterface)
        TM_INTERFACE_HANDLER(T_IEnumInterface)
    TM_END_INTERFACE_MAP()

    CEnumImplBaseT() : m_nPos(0)
    {
    }

    STDMETHODIMP Clone(T_IEnumInterface **ppEnum) tm_override
    {
        *ppEnum = new T(*(static_cast<T*>(this)));
        return S_OK;
    }

    STDMETHODIMP Next(TMULong cPins, value_type* ppPins, TMULong *pcFetched) tm_override
    {
        TMULong nFetched = 0;
        if (cPins >= 1)
        {
            nFetched = (static_cast<T*>(this))->CopyTo(ppPins, cPins);
            m_nPos += nFetched;
        }

        if (pcFetched)
            *pcFetched = nFetched;
        return (nFetched < cPins) ? S_FALSE : S_OK;
    }

    STDMETHODIMP Skip(TMULong cPins) tm_override
    {
        TMULong nFetched = (static_cast<T*>(this))->CopyTo(0, cPins);
        return (nFetched < cPins) ? S_FALSE : S_OK;
    }

    STDMETHODIMP Reset() tm_override
    {
        m_nPos = 0;
        return S_OK;
    }
};

// template <class T> class CEnumContPoint
// {
// public:
//     typedef T value_type;
//     T* m_p;
//     CEnumContPoint(T* p) : m_p (p) {}
//     void CopyTo(value_type*& val) { val = m_p; }
// };
//
// template <class T> class CEnumContInterface
// {
// public:
//     typedef T value_type;
//     CRefPtr<T> m_p;
//     CEnumContInterface(T* p) : m_p (p) {}
//     void CopyTo(value_type*& val) { /*if (val) val->Release();*/ val = m_p; if (val) val->AddRef(); }
// };



///////////////////////////////////////////////////////////////////////////////
// enum 1 object
template <class T, class _IInterface, class _IEnumInterface> class CEnumImpl1BaseT
    : public TM::CBaseRefT<T, _IEnumInterface>
    , public TM::CRefObjectRoot<1>
{
public:
    typedef _IInterface value_type;
    int m_nPos;

    TM_BGN_INTERFACE_MAP(_IEnumInterface)
        TM_INTERFACE_HANDLER(_IEnumInterface)
    TM_END_INTERFACE_MAP()

    CEnumImpl1BaseT()
        : m_nPos(0)
    {
    }

    STDMETHODIMP Clone(_IEnumInterface **ppEnum) tm_override
    {
        T* p = new T(*(static_cast<T*>(this)));
        *ppEnum = p;
        p->Reset();
        return S_OK;
    }

    STDMETHODIMP Next(TMULong cPins, value_type**ppPins, TMULong *pcFetched) tm_override
    {
        if (m_nPos == 0 && cPins >= 1)
        {
            (static_cast<T*>(this))->CopyTo(*ppPins);
            m_nPos = 1;
            if (pcFetched)
                *pcFetched = 1;
            return S_OK;
        }

        if (pcFetched)
            *pcFetched = 0;
        return E_FAIL;
    }

    STDMETHODIMP Skip(TMULong cPins) tm_override
    {
        if ((m_nPos == 0 && cPins == 1) || cPins == 0)
        {
            m_nPos += cPins;
            return S_OK;
        }
        return E_FAIL;
    }

    STDMETHODIMP Reset() tm_override
    {
        m_nPos = 0;
        return S_OK;
    }
};

template <class _IInterface, class _IEnumInterface> class CEnumPoint1
    : public CEnumImpl1BaseT<CEnumPoint1<_IInterface, _IEnumInterface>, _IInterface, _IEnumInterface>
{
public:
    typedef _IInterface value_type;
    value_type* m_p;
    CEnumPoint1(value_type* p) : m_p (p) {}
    CEnumPoint1(const CEnumPoint1& rhs) : m_p (rhs.m_p) {}
    void CopyTo(value_type*& val) { val = m_p; }
};

template <class _IInterface, class _IEnumInterface> class CEnumInterface1
    : public CEnumImpl1BaseT<CEnumInterface1<_IInterface, _IEnumInterface>, _IInterface, _IEnumInterface>
{
public:
    typedef _IInterface value_type;
    CRefPtr<value_type> m_p;
    CEnumInterface1(value_type* p) : m_p (p) {}
    CEnumInterface1(const CEnumInterface1& rhs) : m_p (rhs.m_p) {}
    void CopyTo(value_type*& val) { /*if (val) val->Release();*/ val = m_p; if (val) val->AddRef(); }
};


///////////////////////////////////////////////////////////////////////////////
// enum n object by index
template <class T, class _IInterface, class _IEnumInterface> class CEnumImplNBaseT
    : public TM::CBaseRefT<T, _IEnumInterface>
    , public TM::CRefObjectRoot<1>
{
public:
    typedef _IInterface value_type;
    int m_nPos;

    TM_BGN_INTERFACE_MAP(_IEnumInterface)
        TM_INTERFACE_HANDLER(_IEnumInterface)
    TM_END_INTERFACE_MAP()

    CEnumImplNBaseT()
        : m_nPos(0)
    {
    }

    STDMETHODIMP Clone(_IEnumInterface **ppEnum) tm_override
    {
        T* p = new T(*(static_cast<T*>(this)));
        *ppEnum = p;
        p->Reset();
        return S_OK;
    }

    STDMETHODIMP Next(TMULong cPins, value_type* ppPins, TMULong *pcFetched) tm_override
    {
        DbgAssert(ppPins);

        TMULong n = 0;
        for (; n < cPins; ++n, ++m_nPos)
        {
            ComResult hr = (static_cast<T*>(this))->CopyTo(m_nPos, ppPins[n]);
            if (FAILED(hr))
                break;
        }
        if (pcFetched)
            *pcFetched = n;
        return n ? S_OK : E_FAIL;
    }

    STDMETHODIMP Skip(TMULong cPins) tm_override
    {
        int nCount = (static_cast<T*>(this))->GetCount();
        if (m_nPos + cPins > nCount)
            return S_FALSE;
        return S_OK;
    }

    STDMETHODIMP Reset() tm_override
    {
        m_nPos = 0;
        return S_OK;
    }
};






TM_END_NAMESPACE

/////////////////////////////////////////////////////////////////////////////////////
// unknown
TM_INLINE int DbgUnknownGetRef(IUnknown* p)
{
    int n = p->AddRef();
    p->Release();
    return n -1;
}


/////////////////////////////////////////////////////////////////////////////////////
// stream
// return: S_OK or E_FAIL or other
// output param: IStream **ppstm, void** pMem
TM_CAPI TMResult StreamCreateFromMem(const void* pMem, size_t size, IStream **ppstm);

TM_CAPI TMResult StreamCreateFromStaticMem(const void* pMem, size_t size, IStream **ppstm);

TM_CAPI TMResult StreamCreateFromResource(TMHModule hModule, TMCStr pName, TMCStr pType, IStream **ppstm);

//access: CFile::EAccess or TM_FILE_READ...
TM_CAPI TMResult StreamCreateFromFile(TMCStr pFileName, TMUInt access, IStream **ppstm);

// if size == 0 then call stream->stat to get size
TM_CAPI TMResult StreamCreateCopy(IStream* p, size_t size, IStream **ppstm);

// get sub stream and copied mem buffer
// if size == 0 then call stream->stat to get size
TM_CAPI TMResult StreamCreateReference(IStream* p, size_t size, IStream **ppstm, void** pMem);

TM_INLINE size_t StreamGetSize(IStream* p)
{
    STATSTG stg = {0};
    p->Stat(&stg, 0);
    return stg.cbSize.QuadPart;
}

TM_INLINE TMUInt64 StreamTell(IStream* p)
{
    ULARGE_INTEGER l;
    p->Seek(TM::LLINT(0), SEEK_CUR, &l);
    return l.QuadPart;
}

TM_INLINE size_t StreamSeekSet(IStream* p, TMInt64 n)
{
    ULARGE_INTEGER l;
    p->Seek(TM::LLINT(n), SEEK_SET, &l);
    return l.QuadPart;
}

TM_INLINE size_t StreamSeekCur(IStream* p, TMInt64 n)
{
    ULARGE_INTEGER l;
    p->Seek(TM::LLINT(n), SEEK_CUR, &l);
    return l.QuadPart;
}

TM_INLINE size_t StreamSeekEnd(IStream* p, TMInt64 n)
{
    ULARGE_INTEGER l;
    p->Seek(TM::LLINT(n), SEEK_END, &l);
    return l.QuadPart;
}


#endif // TMCOM_H
