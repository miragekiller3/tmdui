/*
 *  @file
 *  @brief  windows com object
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2007.08.18
 *
 *  Copyright (C) 2007 miragekiller
 */

#pragma once

#ifndef TMCOM_WIN_H
#define TMCOM_WIN_H

#include "tmcom.h"
#include "tmgdi.h"
#include "tmtchar.h"

#if TM_WIN_DESKTOP

#include <Rpcdce.h>
#include <exdisp.h>
#include <exdispid.h>
#include <MSHTMDID.H>
#include <mshtml.h>
#include <mshtmhst.h>

#pragma comment(lib, "Rpcrt4.lib")


TM_BGN_NAMESPACE

////////////////////////////////////////////////////////////////////////////////
// class CCoSTR
struct CCoSTR : public CNoCopyable
{
    LPWSTR p;

    CCoSTR() : p(0) {}
    CCoSTR(const CCoSTR& rhs) : p(rhs.p) { (const_cast<CCoSTR&>(rhs)).p = 0; }
    ~CCoSTR() { ::CoTaskMemFree(p); }
    LPWSTR* operator &() { return &p; }
    operator LPCWSTR() const { return p; }
    LPCWSTR c_str() const { return p; }
    LPCWSTR Detach() { LPWSTR _p = p; p = 0; return _p; }
    void Swap(CCoSTR& rhs) { tm_swap(p, rhs.p); }

    static LPWSTR MakeCoMemStringLen(LPCWSTR source, int nLen)
    {
        if (!nLen)
            return 0;
        LPWSTR p = (LPWSTR)CoTaskMemAlloc((nLen + 1) * sizeof(*source));
        StrCopy(p, nLen + 1, source, nLen);
        return p;
    }

    static LPWSTR MakeCoMemString(LPCWSTR source)
    {
        if (!source)
            return 0;
        return MakeCoMemStringLen(source, StrLen(source));
    }
};

TM_END_NAMESPACE


TM_BGN_NAMESPACE_X(std)

inline TMCStrW data(const TM::CCoSTR& c) tm_noexcept { return c; }
inline size_t size(const TM::CCoSTR& c) { return StrLen(c); }
inline void swap(TM::CCoSTR& c1, TM::CCoSTR& c2, ...) tm_noexcept { tm_swap(c1.p, c2.p); }

TM_END_NAMESPACE_X(std)



TM_BGN_NAMESPACE

////////////////////////////////////////////////////////////////////////////////
// class DispEventUnadviseAll
static void DispEventUnadviseAll(IUnknown* pUnkCP)
{
    CComQIPtr<IConnectionPointContainer> pCPC(pUnkCP);
    CComPtr<IEnumConnectionPoints> pEnum;
    if (S_OK != pCPC->EnumConnectionPoints(&pEnum))
        return;

    for (;;)
    {
        CComPtr<IConnectionPoint> pCP;
        if (S_OK != pEnum->Next(1, &pCP, NULL))
            break;

        for (;;)
        {
            CComPtr<IEnumConnections> pEnumC;
            if (S_OK != pCP->EnumConnections(&pEnumC))
                break;

            CONNECTDATA cd = { 0 };
            if (S_OK != pEnumC->Next(1, &cd, NULL))
                break;
            pCP->Unadvise(cd.dwCookie);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// class CComEventT

template<class T> struct VarGet;

#define DECALRE_VARGET(type,exp,val) \
    template<> struct VarGet<type> { inline static type get(VARIANT& var) { DbgAssert(exp); return var.val; } }

DECALRE_VARGET(BYTE,            var.vt == VT_UI1,                 bVal);
DECALRE_VARGET(FLOAT,           var.vt == VT_R4,                  fltVal);
// DECALRE_VARGET(DOUBLE,          var.vt == VT_R8,                  dblVal);
// DECALRE_VARGET(SHORT,           var.vt == VT_I2,                  iVal);
// DECALRE_VARGET(LONG,            var.vt == VT_I4,                  lVal);
// DECALRE_VARGET(VARIANT_BOOL,    var.vt == VT_BOOL,                boolVal);
// DECALRE_VARGET(SCODE,           var.vt == VT_ERROR,               scode);
DECALRE_VARGET(CY,              var.vt == VT_CY,                  cyVal);
// DECALRE_VARGET(DATE,            var.vt == VT_DATE,                date);
// DECALRE_VARGET(BSTR,            var.vt == VT_BSTR,                bstrVal);
DECALRE_VARGET(DECIMAL*,        var.vt == (VT_BYREF|VT_DECIMAL),  pdecVal);
DECALRE_VARGET(IUnknown*,       var.vt == VT_UNKNOWN,             punkVal);
DECALRE_VARGET(IDispatch*,      var.vt == VT_DISPATCH,            pdispVal);
DECALRE_VARGET(SAFEARRAY*,      var.vt == VT_ARRAY,               parray);
DECALRE_VARGET(BYTE*,           var.vt == (VT_BYREF|VT_UI1),      pbVal);
DECALRE_VARGET(FLOAT*,          var.vt == (VT_BYREF|VT_R4),       pfltVal);
// DECALRE_VARGET(DOUBLE*,         var.vt == (VT_BYREF|VT_R8),         pdblVal);
// DECALRE_VARGET(VARIANT_BOOL*,   var.vt == VT_BYREF|VT_BOOL,       pboolVal);
// DECALRE_VARGET(SCODE*,          var.vt == VT_BYREF|VT_ERROR,      pscode);
DECALRE_VARGET(CY*,             var.vt == (VT_BYREF|VT_CY),       pcyVal);
// DECALRE_VARGET(DATE*,           var.vt == VT_BYREF|VT_DATE,       pdate);
DECALRE_VARGET(BSTR*,           var.vt == (VT_BYREF|VT_BSTR),     pbstrVal);
DECALRE_VARGET(IUnknown**,      var.vt == (VT_BYREF|VT_UNKNOWN),  ppunkVal);
DECALRE_VARGET(IDispatch**,     var.vt == (VT_BYREF|VT_DISPATCH), ppdispVal);
DECALRE_VARGET(SAFEARRAY**,     var.vt == (VT_BYREF|VT_ARRAY),    pparray);
DECALRE_VARGET(VARIANT*,        var.vt == (VT_BYREF|VT_VARIANT),  pvarVal);
DECALRE_VARGET(PVOID,           var.vt == VT_BYREF,               byref);
DECALRE_VARGET(CHAR,            var.vt == VT_I1,                  cVal);
DECALRE_VARGET(USHORT,          var.vt == VT_UI2,                 uiVal);
DECALRE_VARGET(ULONG,           var.vt == VT_UI4,                 ulVal);
DECALRE_VARGET(INT,             var.vt == VT_INT,                 intVal);
DECALRE_VARGET(UINT,            var.vt == VT_UINT,                uintVal);
DECALRE_VARGET(CHAR*,           var.vt == (VT_BYREF|VT_I1),       pcVal);
// DECALRE_VARGET(SHORT*,          var.vt == (VT_BYREF|VT_I2),       piVal);
// DECALRE_VARGET(LONG*,           var.vt == (VT_BYREF|VT_I4),       plVal);
// DECALRE_VARGET(USHORT*,         var.vt == (VT_BYREF|VT_UI2),      puiVal);
DECALRE_VARGET(ULONG*,          var.vt == (VT_BYREF|VT_UI4),      pulVal);
DECALRE_VARGET(INT*,            var.vt == (VT_BYREF|VT_INT),      pintVal);
DECALRE_VARGET(UINT*,           var.vt == (VT_BYREF|VT_UINT),     puintVal);

DECALRE_VARGET(SHORT,           var.vt == VT_I2 || var.vt == VT_BOOL,                       boolVal);
DECALRE_VARGET(LONG,            var.vt == VT_I4 || var.vt == VT_ERROR,                      scode);
DECALRE_VARGET(DOUBLE,          var.vt == VT_R8 || var.vt == VT_DATE,                       date);
DECALRE_VARGET(SHORT*,          var.vt == (VT_BYREF|VT_I2) || var.vt == (VT_BYREF|VT_BOOL), pboolVal);
DECALRE_VARGET(LONG*,           var.vt == (VT_BYREF|VT_I4) || var.vt == (VT_BYREF|VT_ERROR),pscode);
DECALRE_VARGET(DOUBLE*,         var.vt == (VT_BYREF|VT_R8) || var.vt == (VT_BYREF|VT_DATE), pdate);
DECALRE_VARGET(BSTR,            var.vt == (VT_BYREF|VT_UI2) || var.vt == VT_BSTR,           bstrVal);


#undef DECALRE_VARGET




#define TM_BEGIN_SINK_MAP(classname) \
    STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, \
        DISPPARAMS* pdispparams, VARIANT* pvarResult, EXCEPINFO* pexcepinfo, UINT* puArgErr) \
    { \
        /*DbgTrace ("TM_BEGIN_SINK_MAP: %d", dispidMember);*/ \
        typedef classname class_type;


#define TM_SINK_ENTRY(nid, func) \
        if (nid == dispidMember) \
        { \
            TM::CDispInvoke::_Invoke(this, &class_type::func, pdispparams); \
            return S_OK; \
        }

// #define TM_SINK_ENTRY_CLASS(nid, cls, func) \
//         if (nid == dispidMember) \
//         { \
//             TM::CDispInvoke::_Invoke(static_cast<cls*>(this), & cls ::func, pdispparams); \
//             return S_OK; \
//         }

#define TM_CHAIN_SINK_MAP(base_class) \
    base_class::Invoke(dispidMember, riid, lcid, wFlags, \
        pdispparams, pvarResult, pexcepinfo, puArgErr);

#define TM_END_SINK_MAP()    return S_OK; }


///////////////////////////////////////////////////////////////////////////////
// DISP_MAP
#define TM_BEGIN_DISP_MAP(classname) \
    typedef classname _classname_disp; \
    classname* _disp_this() { return static_cast<classname*>(this); } \
    template <int nLine> struct _dp; \
    template <int nLine> HRESULT _Disp_GetID(LPOLESTR pName, _dp<nLine>* pDispID) { return _Disp_GetID(pName, (_dp<nLine+1>*)pDispID); } \
    template <int nLine> HRESULT _Disp_Invoke(DISPID idc, WORD wFlags, DISPPARAMS *pDispParams, _dp<nLine>* pVarResult) { return _Disp_Invoke(idc, wFlags, pDispParams, (_dp<nLine+1>*)pVarResult); } \
    virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames \
        (REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId) tm_override \
    { \
        if (cNames != 1) return E_NOTIMPL; \
        return _Disp_GetID(rgszNames[0], (_dp<__LINE__>*)rgDispId); \
    } \
    HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, \
        WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr) tm_override\
    { \
        return _Disp_Invoke(dispIdMember, wFlags, pDispParams, (_dp<__LINE__>*)pVarResult); \
    }


#define _TM_DISP_FUNCTION_GetIDsOfNames(id,funcname) \
    TM_TEMPLATE_NULL HRESULT _Disp_GetID(LPOLESTR pName, _dp<__LINE__>* pDispID) \
    { \
        if (0 == wcscmp(pName, funcname)) \
        { \
            *(DISPID*)pDispID = id; \
            return S_OK; \
        } \
        return _Disp_GetID(pName, (_dp<__LINE__ + 1>*)pDispID); \
    }


#define TM_DISP_FUNC(id,funcname,func) \
    _TM_DISP_FUNCTION_GetIDsOfNames(id, funcname) \
    TM_TEMPLATE_NULL HRESULT _Disp_Invoke(DISPID idc, WORD wFlags, DISPPARAMS *pDispParams, _dp<__LINE__>* pVarResult) \
    { \
        if (id == idc) \
            return TM::CDispInvoke::_Invoke(_disp_this(), _disp_this()->*func, pDispParams), S_OK; \
        return _Disp_Invoke(idc, wFlags, pDispParams, (_dp<__LINE__+1>*)pVarResult); \
    }

#define TM_DISP_PROP_F(id,propName,getfunc,putfunc) \
    _TM_DISP_FUNCTION_GetIDsOfNames(id,funcname) \
    TM_TEMPLATE_NULL HRESULT _Disp_Invoke(DISPID idc, WORD wFlags, DISPPARAMS *pDispParams, _dp<__LINE__>* pVarResult) \
    { \
        if (id != idc) \
            return _Disp_Invoke(idc, wFlags, pDispParams, (_dp<__LINE__+1>*)pVarResult); \
        if (wFlags & (INVOKE_PROPERTYPUT | INVOKE_PROPERTYPUTREF)) \
        { \
            if (pDispParams->cNamedArgs != 1 || pDispParams->rgdispidNamedArgs[0] != DISPID_PROPERTYPUT) \
                return DISP_E_MEMBERNOTFOUND; \
            return TM::CDispInvoke::_Invoke(_disp_this(), _disp_this()->*putfunc, pDispParams), S_OK; \
        } \
        else if (wFlags & INVOKE_PROPERTYGET)\
        { \
            if (pDispParams->cArgs != 0) return DISP_E_BADPARAMCOUNT; \
            return *((CComVariant*)pVarResult) = _disp_this()->getfunc(), S_OK; \
        } \
        return E_NOTIMPL; \
    }

#define TM_DISP_PROP_GET_F(id,propName,getfunc) \
    _TM_DISP_FUNCTION_GetIDsOfNames(id,propName) \
    TM_TEMPLATE_NULL HRESULT _Disp_Invoke(DISPID idc, WORD wFlags, DISPPARAMS *pDispParams, _dp<__LINE__>* pVarResult) \
    { \
        if (id != idc) \
            return _Disp_Invoke(idc, wFlags, pDispParams, (_dp<__LINE__+1>*)pVarResult); \
        if (wFlags & INVOKE_PROPERTYGET)\
        { \
            if (pDispParams->cArgs != 0) return DISP_E_BADPARAMCOUNT; \
            return *((CComVariant*)pVarResult) = _disp_this()->getfunc(), S_OK; \
        } \
        return E_NOTIMPL; \
    }

#define TM_DISP_PROP(id,propName,member) \
    _TM_DISP_FUNCTION_GetIDsOfNames(id,propName) \
    TM_TEMPLATE_NULL HRESULT _Disp_Invoke(DISPID idc, WORD wFlags, DISPPARAMS *pDispParams, _dp<__LINE__>* pVarResult) \
    { \
        if (id != idc) \
            return _Disp_Invoke(idc, wFlags, pDispParams, (_dp<__LINE__+1>*)pVarResult); \
        if (wFlags & (INVOKE_PROPERTYPUT | INVOKE_PROPERTYPUTREF)) \
        { \
            if (pDispParams->cNamedArgs != 1 || pDispParams->rgdispidNamedArgs[0] != DISPID_PROPERTYPUT) \
                return DISP_E_MEMBERNOTFOUND; \
            return CDispInvoke::Var2T(pDispParams->->rgvarg[0], member), S_OK; \
        } \
        else if (wFlags & INVOKE_PROPERTYGET)\
        { \
            if (pDispParams->cArgs != 0) return DISP_E_BADPARAMCOUNT; \
            return *((CComVariant*)pVarResult) = _disp_this()->member, S_OK; \
        } \
        return E_NOTIMPL; \
    }

#define TM_DISP_PROP_GET(id,propName,member) \
    _TM_DISP_FUNCTION_GetIDsOfNames(id,propName) \
    TM_TEMPLATE_NULL HRESULT _Disp_Invoke(DISPID idc, WORD wFlags, DISPPARAMS *pDispParams, _dp<__LINE__>* pVarResult) \
    { \
        if (id != idc) \
            return _Disp_Invoke(idc, wFlags, pDispParams, (_dp<__LINE__+1>*)pVarResult); \
        if (wFlags & INVOKE_PROPERTYGET)\
        { \
            if (pDispParams->cArgs != 0) return DISP_E_BADPARAMCOUNT; \
            return *((CComVariant*)pVarResult) = _disp_this()->member, S_OK; \
        } \
        return E_NOTIMPL; \
    }

#define TM_END_DISP_MAP() \
    TM_TEMPLATE_NULL HRESULT _Disp_GetID(LPOLESTR pName, _dp<__LINE__>* pDispID) { return DISP_E_MEMBERNOTFOUND; } \
    TM_TEMPLATE_NULL HRESULT _Disp_Invoke(DISPID idc, WORD wFlags, DISPPARAMS *pDispParams, _dp<__LINE__>* pVarResult) { return DISP_E_MEMBERNOTFOUND; }


///////////////////////////////////////////////////////////////////////////////
// CDispInvoke
struct CDispInvoke
{
#define _VA(n) VarGet<T##n>::get(dp->rgvarg[n])

    template <class T> static inline void Var2T(VARIANT& var, T& __x)
    {
        __x = VarGet<T>::get(var);
    }

    template <class Tp, class Tr>
        static inline void _Invoke(Tp* p, Tr (Tp::*fn)(), DISPPARAMS *dp)
    {
        DbgAssert(dp->cArgs == 0);
        (p->*fn)();
    }

    template <class Tp, class Tr, class T0>
        static inline void _Invoke(Tp* p, Tr (Tp::*fn)(T0), DISPPARAMS *dp)
    {
        DbgAssert(dp->cArgs == 1);
        (p->*fn)(_VA(0));
    }

    template <class To, class Tp, class Tr, class T1, class T0>
        static inline void _Invoke(To* p, Tr (Tp::*fn)(T1, T0), DISPPARAMS *dp)
    {
        DbgAssert(dp->cArgs == 2);
        (p->*fn)(_VA(1), _VA(0));
    }

    template <class Tp, class Tr, class T2, class T1, class T0>
        static inline void _Invoke(Tp* p, Tr (Tp::*fn)(T2, T1, T0), DISPPARAMS *dp)
    {
        DbgAssert(dp->cArgs == 3);
        (p->*fn)(_VA(2), _VA(1), _VA(0));
    }

    template <class Tp, class Tr, class T3, class T2, class T1, class T0>
        static inline void _Invoke(Tp* p, Tr (Tp::*fn)(T3, T2, T1, T0), DISPPARAMS *dp)
    {
        DbgAssert(dp->cArgs == 4);
        (p->*fn)(_VA(3), _VA(2), _VA(1), _VA(0));
    }

    template <class Tp, class Tr, class T4, class T3, class T2, class T1, class T0>
        static inline void _Invoke(Tp* p, Tr (Tp::*fn)(T4, T3, T2, T1, T0), DISPPARAMS *dp)
    {
        DbgAssert(dp->cArgs == 5);
        (p->*fn)(_VA(4), _VA(3), _VA(2), _VA(1), _VA(0));
    }

    template <class Tp, class Tr, class T5, class T4, class T3, class T2, class T1, class T0>
        static inline void _Invoke(Tp* p, Tr (Tp::*fn)(T5, T4, T3, T2, T1, T0), DISPPARAMS *dp)
    {
        DbgAssert(dp->cArgs == 6);
        (p->*fn)(_VA(5), _VA(4), _VA(3), _VA(2), _VA(1), _VA(0));
    }

    template <class Tp, class Tr, class T6, class T5, class T4, class T3, class T2, class T1, class T0>
        static inline void _Invoke(Tp* p, Tr (Tp::*fn)(T6, T5, T4, T3, T2, T1, T0), DISPPARAMS *dp)
    {
        DbgAssert(dp->cArgs == 7);
        (p->*fn)(_VA(6), _VA(5), _VA(4), _VA(3), _VA(2), _VA(1), _VA(0));
    }

    template <class Tp, class Tr, class T7, class T6, class T5, class T4, class T3, class T2, class T1, class T0>
        static inline void _Invoke(Tp* p, Tr (Tp::*fn)(T7, T6, T5, T4, T3, T2, T1, T0), DISPPARAMS *dp)
    {
        DbgAssert(dp->cArgs == 8);
        (p->*fn)(_VA(7), _VA(6), _VA(5), _VA(4), _VA(3), _VA(2), _VA(1), _VA(0));
    }

    template <class Tp, class Tr, class T8, class T7, class T6, class T5, class T4, class T3, class T2, class T1, class T0>
        static inline void _Invoke(Tp* p, Tr (Tp::*fn)(T8, T7, T6, T5, T4, T3, T2, T1, T0), DISPPARAMS *dp)
    {
        DbgAssert(dp->cArgs == 9);
        (p->*fn)(_VA(8), _VA(7), _VA(6), _VA(5), _VA(4), _VA(3), _VA(2), _VA(1), _VA(0));
    }

    template <class Tp, class Tr, class T9, class T8, class T7, class T6, class T5, class T4, class T3, class T2, class T1, class T0>
        static inline void _Invoke(Tp* p, Tr (Tp::*fn)(T9, T8, T7, T6, T5, T4, T3, T2, T1, T0), DISPPARAMS *dp)
    {
        DbgAssert(dp->cArgs == 10);
        (p->*fn)(_VA(9), _VA(8), _VA(7), _VA(6), _VA(5), _VA(4), _VA(3), _VA(2), _VA(1), _VA(0));
    }

    template <class Tp, class Tr, class T10, class T9, class T8, class T7, class T6, class T5, class T4, class T3, class T2, class T1, class T0>
        static inline void _Invoke(Tp* p, Tr (Tp::*fn)(T10, T9, T8, T7, T6, T5, T4, T3, T2, T1, T0), DISPPARAMS *dp)
    {
        DbgAssert(dp->cArgs == 11);
        (p->*fn)(_VA(10), _VA(9), _VA(8), _VA(7), _VA(6), _VA(5), _VA(4), _VA(3), _VA(2), _VA(1), _VA(0));
    }

    template <class Tp, class Tr, class T11, class T10, class T9, class T8, class T7, class T6, class T5, class T4, class T3, class T2, class T1, class T0>
        static inline void _Invoke(Tp* p, Tr (Tp::*fn)(T11, T10, T9, T8, T7, T6, T5, T4, T3, T2, T1, T0), DISPPARAMS *dp)
    {
        DbgAssert(dp->cArgs == 12);
        (p->*fn)(_VA(11), _VA(10), _VA(9), _VA(8), _VA(7), _VA(6), _VA(5), _VA(4), _VA(3), _VA(2), _VA(1), _VA(0));
    }

    template <class Tp, class Tr, class T12, class T11, class T10, class T9, class T8, class T7, class T6, class T5, class T4, class T3, class T2, class T1, class T0>
        static inline void _Invoke(Tp* p, Tr (Tp::*fn)(T12, T11, T10, T9, T8, T7, T6, T5, T4, T3, T2, T1, T0), DISPPARAMS *dp)
    {
        DbgAssert(dp->cArgs == 13);
        (p->*fn)(_VA(12), _VA(11), _VA(10), _VA(9), _VA(8), _VA(7), _VA(6), _VA(5), _VA(4), _VA(3), _VA(2), _VA(1), _VA(0));
    }

    template <class Tp, class Tr, class T13, class T12, class T11, class T10, class T9, class T8, class T7, class T6, class T5, class T4, class T3, class T2, class T1, class T0>
        static inline void _Invoke(Tp* p, Tr (Tp::*fn)(T13, T12, T11, T10, T9, T8, T7, T6, T5, T4, T3, T2, T1, T0), DISPPARAMS *dp)
    {
        DbgAssert(dp->cArgs == 14);
        (p->*fn)(_VA(13), _VA(12), _VA(11), _VA(10), _VA(9), _VA(8), _VA(7), _VA(6), _VA(5), _VA(4), _VA(3), _VA(2), _VA(1), _VA(0));
    }

    template <class Tp, class Tr, class T14, class T13, class T12, class T11, class T10, class T9, class T8, class T7, class T6, class T5, class T4, class T3, class T2, class T1, class T0>
        static inline void _Invoke(Tp* p, Tr (Tp::*fn)(T14, T13, T12, T11, T10, T9, T8, T7, T6, T5, T4, T3, T2, T1, T0), DISPPARAMS *dp)
    {
        DbgAssert(dp->cArgs == 15);
        (p->*fn)(_VA(14), _VA(13), _VA(12), _VA(11), _VA(10), _VA(9), _VA(8), _VA(7), _VA(6), _VA(5), _VA(4), _VA(3), _VA(2), _VA(1), _VA(0));
    }

    template <class Tp, class Tr, class T15, class T14, class T13, class T12, class T11, class T10, class T9, class T8, class T7, class T6, class T5, class T4, class T3, class T2, class T1, class T0>
        static inline void _Invoke(Tp* p, Tr (Tp::*fn)(T15, T14, T13, T12, T11, T10, T9, T8, T7, T6, T5, T4, T3, T2, T1, T0), DISPPARAMS *dp)
    {
        DbgAssert(dp->cArgs == 16);
        (p->*fn)(_VA(15), _VA(14), _VA(13), _VA(12), _VA(11), _VA(10), _VA(9), _VA(8), _VA(7), _VA(6), _VA(5), _VA(4), _VA(3), _VA(2), _VA(1), _VA(0));
    }

#undef _VA
};


///////////////////////////////////////////////////////////////////////////////
// CNullDispatchImplT
template <class T>
class CNullDispatchImplT : public T
{
    STDMETHOD(GetTypeInfoCount)(UINT* pctinfo)
    {
        COMTRACE("CComEventT::GetTypeInfoCount");
        return E_NOTIMPL;
    }

    STDMETHOD(GetTypeInfo)(UINT itinfo, LCID lcid, ITypeInfo** pptinfo)
    {
        COMTRACE("CComEventT::GetTypeInfo:%d, %d", itinfo, lcid);
        return E_NOTIMPL;
    }

    STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR* rgszNames, UINT cNames,
        LCID lcid, DISPID* rgdispid)
    {
        COMTRACE("CComEventT::GetIDsOfNames:%d", lcid);
        return E_NOTIMPL;
    }

    STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid,
        LCID lcid, WORD /*wFlags*/, DISPPARAMS* pdispparams, VARIANT* pvarResult,
        EXCEPINFO* /*pexcepinfo*/, UINT* /*puArgErr*/)
    {
        COMTRACE("CWebBrowserEventT::Invoke:(DISPID(%d), LCID(%d))", dispidMember, lcid);
        return S_OK;
    }
};


template <class TEvent, const IID* piid = &__uuidof(TEvent) >
class CComEventT : public CNullDispatchImplT<TEvent> //, public CFuncInvoke
{
private:
    DWORD m_dwEventCookie;

protected:
//     inline to(VARIANT& v,)

// #define _VA(n) VarGet<T##n>::get(rgvarg[n])
//
//     template <class Tp, class Tr>
//     void _Invoke(Tp* p, Tr (Tp::*fn)(), VARIANT *rgvarg)
//     {
//         (p->*fn)();
//     }
//
//     template <class Tp, class Tr, class T0>
//     void _Invoke(Tp* p, Tr (Tp::*fn)(T0), VARIANT *rgvarg)
//     {
//         (p->*fn)(_VA(0));
//     }
//
//     template <class Tp, class Tr, class T1, class T0>
//         void _Invoke(Tp* p, Tr (Tp::*fn)(T1, T0), VARIANT *rgvarg)
//     {
//         (p->*fn)(_VA(1), _VA(0));
//     }
//
//     template <class Tp, class Tr, class T2, class T1, class T0>
//         void _Invoke(Tp* p, Tr (Tp::*fn)(T2, T1, T0), VARIANT *rgvarg)
//     {
//         (p->*fn)(_VA(2), _VA(1), _VA(0));
//     }
//
//     template <class Tp, class Tr, class T3, class T2, class T1, class T0>
//         void _Invoke(Tp* p, Tr (Tp::*fn)(T3, T2, T1, T0), VARIANT *rgvarg)
//     {
//         (p->*fn)(_VA(3), _VA(2), _VA(1), _VA(0));
//     }
//
//     template <class Tp, class Tr, class T4, class T3, class T2, class T1, class T0>
//         void _Invoke(Tp* p, Tr (Tp::*fn)(T4, T3, T2, T1, T0), VARIANT *rgvarg)
//     {
//         (p->*fn)(_VA(4), _VA(3), _VA(2), _VA(1), _VA(0));
//     }
//
//     template <class Tp, class Tr, class T5, class T4, class T3, class T2, class T1, class T0>
//         void _Invoke(Tp* p, Tr (Tp::*fn)(T5, T4, T3, T2, T1, T0), VARIANT *rgvarg)
//     {
//         (p->*fn)(_VA(5), _VA(4), _VA(3), _VA(2), _VA(1), _VA(0));
//     }
//
//     template <class Tp, class Tr, class T6, class T5, class T4, class T3, class T2, class T1, class T0>
//         void _Invoke(Tp* p, Tr (Tp::*fn)(T6, T5, T4, T3, T2, T1, T0), VARIANT *rgvarg)
//     {
//         (p->*fn)(_VA(6), _VA(5), _VA(4), _VA(3), _VA(2), _VA(1), _VA(0));
//     }
//
//     template <class Tp, class Tr, class T7, class T6, class T5, class T4, class T3, class T2, class T1, class T0>
//         void _Invoke(Tp* p, Tr (Tp::*fn)(T7, T6, T5, T4, T3, T2, T1, T0), VARIANT *rgvarg)
//     {
//         (p->*fn)(_VA(7), _VA(6), _VA(5), _VA(4), _VA(3), _VA(2), _VA(1), _VA(0));
//     }
//
//     template <class Tp, class Tr, class T8, class T7, class T6, class T5, class T4, class T3, class T2, class T1, class T0>
//         void _Invoke(Tp* p, Tr (Tp::*fn)(T8, T7, T6, T5, T4, T3, T2, T1, T0), VARIANT *rgvarg)
//     {
//         (p->*fn)(_VA(8), _VA(7), _VA(6), _VA(5), _VA(4), _VA(3), _VA(2), _VA(1), _VA(0));
//     }
//
//     template <class Tp, class Tr, class T9, class T8, class T7, class T6, class T5, class T4, class T3, class T2, class T1, class T0>
//         void _Invoke(Tp* p, Tr (Tp::*fn)(T9, T8, T7, T6, T5, T4, T3, T2, T1, T0), VARIANT *rgvarg)
//     {
//         (p->*fn)(_VA(9), _VA(8), _VA(7), _VA(6), _VA(5), _VA(4), _VA(3), _VA(2), _VA(1), _VA(0));
//     }
//
//     template <class Tp, class Tr, class T10, class T9, class T8, class T7, class T6, class T5, class T4, class T3, class T2, class T1, class T0>
//         void _Invoke(Tp* p, Tr (Tp::*fn)(T10, T9, T8, T7, T6, T5, T4, T3, T2, T1, T0), VARIANT *rgvarg)
//     {
//         (p->*fn)(_VA(10), _VA(9), _VA(8), _VA(7), _VA(6), _VA(5), _VA(4), _VA(3), _VA(2), _VA(1), _VA(0));
//     }
//
// #undef _VA

public:
    TM_UNUSED_ADDREF_RELEASE    ();

    TM_BGN_INTERFACE_MAP        (IUnknown)
        TM_INTERFACE_HANDLER    (TEvent)
        TM_INTERFACE_HANDLER    (IDispatch)
    TM_END_INTERFACE_MAP        ()

public:
    CComEventT()
        : m_dwEventCookie(0xFEFEFEFE)
    {
    }

    ~CComEventT()
    {
        DbgAssert(m_dwEventCookie == 0xFEFEFEFE);
    }

//     STDMETHOD(GetTypeInfoCount)(UINT* pctinfo)
//     {
//         COMTRACE("CComEventT::GetTypeInfoCount");
//         return E_NOTIMPL;
//     }
//
//     STDMETHOD(GetTypeInfo)(UINT itinfo, LCID lcid, ITypeInfo** pptinfo)
//     {
//         COMTRACE("CComEventT::GetTypeInfo:%d, %d", itinfo, lcid);
//         return E_NOTIMPL;
//     }
//
//     STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR* rgszNames, UINT cNames,
//         LCID lcid, DISPID* rgdispid)
//     {
//         COMTRACE("CComEventT::GetIDsOfNames:%d", lcid);
//         return E_NOTIMPL;
//     }

    HRESULT DispEventAdvise (IUnknown* pUnk)
    {
        if (m_dwEventCookie == 0xFEFEFEFE)
            return AtlAdvise (pUnk, this, *piid, &m_dwEventCookie);
        else
            return S_OK;
    }

    HRESULT DispEventUnadvise (IUnknown* pUnk)
    {
        HRESULT hr = AtlUnadvise (pUnk, *piid, m_dwEventCookie);
        m_dwEventCookie = 0xFEFEFEFE;
        return hr;
    }
};

template <class T>
struct CAmbientPropImpl
{
public:
    BOOL GetAmbientProp(DISPID dispid, VARIANT* pvarResult)
    {
        switch (dispid)
        {
        case DISPID_AMBIENT_AUTOCLIP:
        case DISPID_AMBIENT_MESSAGEREFLECT:
        case DISPID_AMBIENT_SUPPORTSMNEMONICS:
        case DISPID_AMBIENT_USERMODE:
            V_VT(pvarResult) = VT_BOOL;
            V_BOOL(pvarResult) = VARIANT_TRUE;
            return TRUE;

        case DISPID_AMBIENT_SHOWGRABHANDLES:
        case DISPID_AMBIENT_SHOWHATCHING:
        case DISPID_AMBIENT_UIDEAD:
            V_VT(pvarResult) = VT_BOOL;
            V_BOOL(pvarResult) = 0;
            return TRUE;

        case DISPID_AMBIENT_APPEARANCE:     // ambient appearance is 3D
            V_VT(pvarResult) = VT_I2;
            V_I2(pvarResult) = 1;
            return TRUE;

//         case DISPID_AMBIENT_BACKCOLOR:
//         case DISPID_AMBIENT_FORECOLOR:
//             if (m_crBack == (COLORREF)-1)   // ambient colors not initialized
//             {
//                 CWindowDC dc(m_pWnd);
//                 m_pWnd->SendMessage(WM_CTLCOLORSTATIC, (WPARAM)dc.m_hDC,
//                     (LPARAM)m_pWnd->m_hWnd);
//                 m_crBack = dc.GetBkColor();
//                 m_crFore = dc.GetTextColor();
//             }
//
//             V_VT(pvarResult) = VT_COLOR;
//             V_I4(pvarResult) = (dispid == DISPID_AMBIENT_BACKCOLOR) ?
// m_crBack : m_crFore;
//             return TRUE;

//         case DISPID_AMBIENT_FONT:
//             if (m_pOleFont == NULL)         // ambient font not initialized
//                 CreateOleFont(m_pWnd->GetFont());
//
//             ASSERT(m_pOleFont != NULL);
//             if (m_pOleFont == NULL)         // failed to create font
//                 return FALSE;
//
//             V_VT(pvarResult) = VT_FONT;
//             m_pOleFont->AddRef();
//             V_DISPATCH(pvarResult) = m_pOleFont;
//             return TRUE;
//
//         case DISPID_AMBIENT_DISPLAYASDEFAULT:
//             V_VT(pvarResult) = VT_BOOL;
//             V_BOOL(pvarResult) = (VARIANT_BOOL)(pSite->IsDefaultButton() ? -1 : 0);
//             return TRUE;

        case DISPID_AMBIENT_LOCALEID:
            V_VT(pvarResult) = VT_I4;
            V_I4(pvarResult) = GetThreadLocale();
            return TRUE;

        case DISPID_AMBIENT_DISPLAYNAME:
            {
                V_VT(pvarResult) = VT_BSTR;
                V_BSTR(pvarResult) = CComBSTR(L"").Detach(); // return blank string
            }
            return TRUE;

        case DISPID_AMBIENT_SCALEUNITS:
            {
                V_VT(pvarResult) = VT_BSTR;
                V_BSTR(pvarResult) = CComBSTR(L"pixels").Detach(); // str.AllocSysString();
            }
            return TRUE;
        }

        return FALSE;
    }
};

////////////////////////////////////////////////////////////////////////////////
// class CWebBrowserEventT
template <class T>
    class CWebBrowserEventT : public CComEventT <DWebBrowserEvents2>
{
public:

    STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid,
        LCID lcid, WORD /*wFlags*/, DISPPARAMS* pdispparams, VARIANT* pvarResult,
        EXCEPINFO* /*pexcepinfo*/, UINT* /*puArgErr*/)
    {
        COMTRACE("CWebBrowserEventT::Invoke:(DISPID(%d), LCID(%d))", dispidMember, lcid);
        switch (dispidMember)
        {
        case DISPID_AMBIENT_DLCONTROL:
            if (pvarResult->vt == VT_EMPTY)
            {
                pvarResult->vt = VT_I4;
                pvarResult->lVal = DLCTL_DLIMAGES | DLCTL_VIDEOS | DLCTL_BGSOUNDS;
                (static_cast < T* > (this))->DLControl (&pvarResult->lVal);
            }
            break;

        case DISPID_AMBIENT_USERAGENT:
            return (static_cast<T*>(this))->UserAgent(pvarResult);

        case DISPID_BEFORENAVIGATE2:
            // The parameters for this DISPID are as follows:
            // [0]: Cancel flag - VT_BYREF|VT_BOOL
            // [1]: HTTP headers - VT_BYREF|VT_VARIANT
            // [2]: Address of HTTP POST data - VT_BYREF|VT_VARIANT
            // [3]: Target frame name - VT_BYREF|VT_VARIANT
            // [4]: Option flags - VT_BYREF|VT_VARIANT
            // [5]: URL to navigate to - VT_BYREF|VT_VARIANT
            // [6]: An object that evaluates to the top-level or frame
            (static_cast < T* > (this))->BeforeNavigate2 (
                pdispparams->rgvarg[6].pdispVal,
                pdispparams->rgvarg[5].pvarVal,
                pdispparams->rgvarg[4].pvarVal,
                pdispparams->rgvarg[3].pvarVal,
                pdispparams->rgvarg[2].pvarVal,
                pdispparams->rgvarg[1].pvarVal,
                pdispparams->rgvarg[0].pboolVal
               );
            break;

        case 268: //DISPID_CLIENTTOHOSTWINDOW
            (static_cast < T* > (this))->ClientToHostWindow (
                pdispparams->rgvarg[1].plVal,
                pdispparams->rgvarg[0].plVal
               );
            break;

        case DISPID_COMMANDSTATECHANGE:
            (static_cast < T* > (this))->CommandStateChange (
                pdispparams->rgvarg[1].lVal,
                pdispparams->rgvarg[0].boolVal
               );
            break;

        case DISPID_DOCUMENTCOMPLETE:
            (static_cast < T* > (this))->DocumentComplete (
                pdispparams->rgvarg[1].pdispVal,
                pdispparams->rgvarg[0].pvarVal
               );
            break;

        case DISPID_DOWNLOADBEGIN:
            (static_cast < T* > (this))->DownloadBegin ();
            break;

        case DISPID_DOWNLOADCOMPLETE:
            (static_cast < T* > (this))->DownloadComplete ();
            break;

//        case 269: // DISPID_SETSECURELOCKICON
        case 270: //DISPID_FILEDOWNLOAD:
            (static_cast < T* > (this))->FileDownload (
                pdispparams->rgvarg[0].pboolVal
               );
            break;

        case DISPID_NAVIGATECOMPLETE2:
            (static_cast < T* > (this))->NavigateComplete2 (
                pdispparams->rgvarg[1].pdispVal,
                pdispparams->rgvarg[0].pvarVal
               );
            break;

        case 271: //DISPID_NAVIGATEERROR:
            (static_cast < T* > (this))->NavigateError (
                pdispparams->rgvarg[4].pdispVal,
                pdispparams->rgvarg[3].pvarVal,
                pdispparams->rgvarg[2].pvarVal,
                pdispparams->rgvarg[1].pvarVal,
                pdispparams->rgvarg[0].pboolVal
               );
            break;

        case DISPID_NEWWINDOW2:
            (static_cast < T* > (this))->NewWindow2 (
                pdispparams->rgvarg[1].ppdispVal,
                pdispparams->rgvarg[0].pboolVal
               );
            break;

        case DISPID_ONFULLSCREEN:
            (static_cast < T* > (this))->OnFullScreen (
                pdispparams->rgvarg[0].boolVal
               );
            break;

        case DISPID_ONMENUBAR:
            (static_cast < T* > (this))->OnMenuBar (
                pdispparams->rgvarg[0].boolVal
               );
            break;

        case DISPID_ONQUIT:
            (static_cast < T* > (this))->OnQuit ();
            break;

        case DISPID_ONSTATUSBAR:
            (static_cast < T* > (this))->OnStatusBar (
                pdispparams->rgvarg[0].boolVal
               );
            break;

        case DISPID_ONTHEATERMODE:
            (static_cast < T* > (this))->OnTheaterMode (
                pdispparams->rgvarg[0].boolVal
               );
            break;

        case DISPID_ONTOOLBAR:
            (static_cast < T* > (this))->OnToolBar (
                pdispparams->rgvarg[0].boolVal
               );
            break;

        case DISPID_ONVISIBLE:
            (static_cast < T* > (this))->OnVisible (
                pdispparams->rgvarg[0].boolVal
               );
            break;

        case DISPID_PROGRESSCHANGE:
            (static_cast < T* > (this))->ProgressChange (
                pdispparams->rgvarg[1].lVal,
                pdispparams->rgvarg[0].lVal
               );
            break;

        case DISPID_PROPERTYCHANGE:
            (static_cast < T* > (this))->PropertyChange (
                pdispparams->rgvarg[0].bstrVal
               );
            break;

        case DISPID_STATUSTEXTCHANGE:
            (static_cast < T* > (this))->StatusTextChange (
                pdispparams->rgvarg[0].bstrVal
               );
            break;

        case DISPID_TITLECHANGE:
            (static_cast < T* > (this))->TitleChange (
                pdispparams->rgvarg[0].bstrVal
               );
            break;

        case 263: //DISPID_WINDOWCLOSING:
            (static_cast < T* > (this))->WindowClosing (
                pdispparams->rgvarg[1].boolVal,
                pdispparams->rgvarg[0].pboolVal
               );
            break;

        case 267: //DISPID_WINDOWSETHEIGHT:
            (static_cast < T* > (this))->WindowSetHeight (
                pdispparams->rgvarg[0].lVal
               );
            break;

        case 264: //DISPID_WINDOWSETLEFT:
            (static_cast < T* > (this))->WindowSetLeft (
                pdispparams->rgvarg[0].lVal
               );
            break;

        case 262: //DISPID_WINDOWSETRESIZABLE:
            (static_cast < T* > (this))->WindowSetResizable (
                pdispparams->rgvarg[0].boolVal
               );
            break;

        case 265: //DISPID_WINDOWSETTOP:
            (static_cast < T* > (this))->WindowSetTop (
                pdispparams->rgvarg[0].lVal
               );
            break;

        case 266: //DISPID_WINDOWSETWIDTH:
            (static_cast < T* > (this))->WindowSetWidth (
                pdispparams->rgvarg[0].lVal
               );
            break;

        case 273: // DISPID_NEWWINDOW3
            (static_cast<T*>(this))->NewWindow3 (
                pdispparams->rgvarg[4].ppdispVal,
                pdispparams->rgvarg[3].pboolVal,
                pdispparams->rgvarg[2].lVal,
                pdispparams->rgvarg[1].bstrVal,
                pdispparams->rgvarg[0].bstrVal
               );
            break;

        default:
            DbgTrace("unknown dispid:%d", dispidMember);
            return DISP_E_MEMBERNOTFOUND;
        }

        return S_OK;
    }

    void DLControl (long *pFalg)
    {
//         DLCTL_DLIMAGES, DLCTL_VIDEOS, and DLCTL_BGSOUNDS: Images, videos, and background sounds will be downloaded from the server and displayed or played if these flags are set. They will not be downloaded and displayed if the flags are not set.
//         DLCTL_NO_SCRIPTS and DLCTL_NO_JAVA: Scripts and Java applets will not be executed.
//         DLCTL_NO_DLACTIVEXCTLS and DLCTL_NO_RUNACTIVEXCTLS : ActiveX controls will not be downloaded or will not be executed.
//         DLCTL_DOWNLOADONLY: The page will only be downloaded, not displayed.
//         DLCTL_NO_FRAMEDOWNLOAD: The WebBrowser Control will download and parse a frameSet, but not the individual frame objects within the frameSet.
//         DLCTL_RESYNCHRONIZE and DLCTL_PRAGMA_NO_CACHE: These flags cause cache refreshes. With DLCTL_RESYNCHRONIZE, the server will be asked for update status. Cached files will be used if the server indicates that the cached information is up-to-date. With DLCTL_PRAGMA_NO_CACHE, files will be re-downloaded from the server regardless of the update status of the files.
//         DLCTL_NO_BEHAVIORS: Behaviors are not downloaded and are disabled in the document.
//         DLCTL_NO_METACHARSET_HTML: Character sets specified in meta elements are suppressed.
//         DLCTL_URL_ENCODING_DISABLE_UTF8 and DLCTL_URL_ENCODING_ENABLE_UTF8: These flags function similarly to the DOCHOSTUIFLAG_URL_ENCODING_DISABLE_UTF8 and DOCHOSTUIFLAG_URL_ENCODING_ENABLE_UTF8 flags used with IDocHostUIHandler::GetHostInfo. The difference is that the DOCHOSTUIFLAG flags are checked only when the WebBrowser Control is first instantiated. The download flags here for the ambient property change are checked whenever the WebBrowser Control needs to perform a download.
//         DLCTL_NO_CLIENTPULL: No client pull operations will be performed.
//         DLCTL_SILENT: No user interface will be displayed during downloads.
//         DLCTL_FORCEOFFLINE: The WebBrowser Control always operates in offline mode.
//         DLCTL_OFFLINEIFNOTCONNECTED and DLCTL_OFFLINE: These flags are the same. The WebBrowser Control will operate in offline mode if not connected to the Internet.
    }

    HRESULT UserAgent(VARIANT* pvarResult)
    {
        return DISP_E_MEMBERNOTFOUND;
    }

    void BeforeNavigate2 (
        IDispatch *pDisp, VARIANT *URL, VARIANT *Flags,
        VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers,
        VARIANT_BOOL *Cancel)
    {
    }

    void ClientToHostWindow (long *CX, long *CY) {}

    void CommandStateChange (long Command, VARIANT_BOOL Enable) {}

    void DocumentComplete (IDispatch *pDisp, VARIANT *URL) {}

    void DownloadBegin (VOID) {}

    void DownloadComplete (VOID) {}

    void FileDownload (VARIANT_BOOL *Cancel) {}

    void NavigateComplete2 (IDispatch *pDisp, VARIANT *URL) {}

    void NavigateError (
        IDispatch *pDisp, VARIANT *URL, VARIANT *TargetFrameName,
        VARIANT *StatusCode, VARIANT_BOOL *Cancel)
    {
    }

    void NewWindow2 (IDispatch **ppDisp, VARIANT_BOOL *Cancel) {}

    void OnFullScreen (VARIANT_BOOL FullScreen) {}

    void OnMenuBar (VARIANT_BOOL MenuBar) {}

    void OnQuit(VOID) {}

    void OnStatusBar (VARIANT_BOOL StatusBar) {}

    void OnTheaterMode (VARIANT_BOOL TheaterMode) {}

    void OnToolBar (VARIANT_BOOL ToolBar) {}

    void OnVisible (VARIANT_BOOL Visible) {}

    void ProgressChange (long Progress, long ProgressMax) {}

    void PropertyChange (BSTR szProperty) {}

    void StatusTextChange (BSTR Text) {}

    void TitleChange (BSTR Text) {}

    void WindowClosing (VARIANT_BOOL IsChildWindow, VARIANT_BOOL *Cancel) {}

    void WindowSetHeight (long Height) {}

    void WindowSetLeft (long Left) {}

    void WindowSetResizable (VARIANT_BOOL Resizable) {}

    void WindowSetTop (long Top) {}

    void WindowSetWidth (long Width) {}

    void NewWindow3(IDispatch** ppDisp, VARIANT_BOOL* Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl) {}
};


// IDocHostUIHander2, ICustomDoc, IHostDialogHelper

#ifndef DOCHOSTUIFLAG_DPI_AWARE
#   define DOCHOSTUIFLAG_DPI_AWARE                         0x40000000
#endif

class CDocHostUIHandler : public IDocHostUIHandler
{
public:
    // IDocHostUIHandler
    // MSHTML requests to display its context menu
    STDMETHOD(ShowContextMenu)(DWORD dwID, TMPoint* pptPosition, IUnknown* pCommandTarget, IDispatch* pDispatchObjectHit)
    {
        COMTRACE(L"CDocHostUIHandler::ShowContextMenu()");
        return S_FALSE;
    }

    // Called at initialisation to find UI styles from container
    STDMETHOD(GetHostInfo)(DOCHOSTUIINFO* pInfo)
    {
        COMTRACE(L"CDocHostUIHandler::GetHostInfo()");
        pInfo->cbSize= sizeof(DOCHOSTUIINFO);
        pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
        pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_DISABLE_SCRIPT_INACTIVE | DOCHOSTUIFLAG_DPI_AWARE;
        return S_OK;
    }

    // Allows the host to replace the IE4/MSHTML menus and toolbars.
    STDMETHOD(ShowUI)(DWORD dwID, IOleInPlaceActiveObject* pActiveObject, IOleCommandTarget* pCommandTarget, IOleInPlaceFrame* pFrame, IOleInPlaceUIWindow* pDoc)
    {
        COMTRACE(L"CDocHostUIHandler::ShowUI()");
        return S_FALSE;
    }

    // Called when IE4/MSHTML removes its menus and toolbars.
    STDMETHOD(HideUI)()
    {
        COMTRACE(L"CDocHostUIHandler::HideUI()");
        return S_FALSE;
    }

    // Notifies the host that the command state has changed.
    STDMETHOD(UpdateUI)()
    {
        COMTRACE(L"CDocHostUIHandler::UpdateUI()");
        return S_FALSE;
    }

    // Called from the IE4/MSHTML implementation of IOleInPlaceActiveObject::EnableModeless
    STDMETHOD(EnableModeless)(TMBool fEnable)
    {
        COMTRACE(L"CDocHostUIHandler::EnableModeless()");
        return E_NOTIMPL;
    }

    // Called from the IE4/MSHTML implementation of IOleInPlaceActiveObject::OnDocWindowActivate
    STDMETHOD(OnDocWindowActivate)(TMBool fActivate)
    {
        COMTRACE(L"CDocHostUIHandler::OnDocWindowActivate()");
        return E_NOTIMPL;
    }

    // Called from the IE4/MSHTML implementation of IOleInPlaceActiveObject::OnFrameWindowActivate.
    STDMETHOD(OnFrameWindowActivate)(TMBool fActivate)
    {
        COMTRACE(L"CDocHostUIHandler::OnFrameWindowActivate()");
        return E_NOTIMPL;
    }

    // Called from the IE4/MSHTML implementation of IOleInPlaceActiveObject::ResizeBorder.
    STDMETHOD(ResizeBorder)(const TMRect* prcBorder, IOleInPlaceUIWindow* pUIWindow, TMBool fFrameWindow)
    {
        COMTRACE(L"CDocHostUIHandler::ResizeBorder()");
        return E_NOTIMPL;
    }

    // Called by IE4/MSHTML when IOleInPlaceActiveObject::TranslateAccelerator or IOleControlSite::TranslateAccelerator is called.
    STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, const GUID* pguidCmdGroup, DWORD nCmdID)
    {
        COMTRACE(L"CDocHostUIHandler::TranslateAccelerator()");
        return E_NOTIMPL;
    }

    // Returns the registry key under which IE4/MSHTML stores user preferences.
    // Returns S_OK if successful, or S_FALSE otherwise. If S_FALSE, IE4/MSHTML will default to its own user options.
    STDMETHOD(GetOptionKeyPath)(BSTR* pbstrKey, DWORD dwReserved)
    {
        COMTRACE(L"CDocHostUIHandler::GetOptionKeyPath()");
        return S_FALSE;
    }

    // Called by IE4/MSHTML when it is being used as a drop target to allow the host to supply an alternative IDropTarget
    STDMETHOD(GetDropTarget)(IDropTarget* pDropTarget, IDropTarget** ppDropTarget)
    {
        COMTRACE(L"CDocHostUIHandler::GetDropTarget()");
        return S_FALSE;
    }

    // Called by IE4/MSHTML to obtain the host's IDispatch interface
    STDMETHOD(GetExternal)(IDispatch** ppDispatch)
    {
        COMTRACE(L"CDocHostUIHandler::GetExternal()");
        return S_FALSE;
    }

    // Called by IE4/MSHTML to allow the host an opportunity to modify the URL to be loaded
    STDMETHOD(TranslateUrl)(DWORD dwTranslate, OLECHAR* pchURLIn, OLECHAR** ppchURLOut)
    {
        COMTRACE(L"CDocHostUIHandler::TranslateUrl()");
        return E_NOTIMPL;
    }

    // Called on the host by IE4/MSHTML to allow the host to replace IE4/MSHTML's data object.
    // This allows the host to block certain clipboard formats or support additional clipboard formats.
    STDMETHOD(FilterDataObject)(IDataObject* pDO, IDataObject** ppDORet)
    {
        COMTRACE(L"CDocHostUIHandler::FilterDataObject()");
        return S_FALSE;
    }
};

class CDocHostShowUI : public IDocHostShowUI
{
public:
    // IDocHostShowUI
    STDMETHOD(ShowMessage)(TMHWnd hwnd, LPOLESTR lpstrText, LPOLESTR lpstrCaption,
        DWORD dwType, LPOLESTR lpstrHelpFile, DWORD dwHelpContext,
        TMResult __RPC_FAR *plResult)
    {
        COMTRACE(L"CDocHostUIHandler::ShowMessage()");
        return S_FALSE;
    }

    STDMETHOD(ShowHelp)(TMHWnd hwnd, LPOLESTR pszHelpFile, UINT uCommand,
        DWORD dwData, TMPoint ptMouse, IDispatch __RPC_FAR *pDispatchObjectHit)
    {
        COMTRACE(L"CDocHostUIHandler::ShowHelp()");
        return S_FALSE;
    }
};


#if defined(TM_MSC) && TM_MSC <= 1200

#define DECARE_WEBCLASS_CONSTRUCT_EX(name,cls) \
    name() {} \
    name(cls* lp) : base_class (lp) {} \
    name(IUnknown* lp) : base_class (lp) {} \
    template <class P> name(const CComQIPtr<P>& rhs) : base_class(rhs.p) {}

#else

#define DECARE_WEBCLASS_CONSTRUCT_EX(name,cls) \
    name() {} \
    name(cls* lp) : base_class (lp) {} \
    name(IUnknown* lp) : base_class (lp) {} \

#endif

#define DECARE_WEBCLASS_CONSTRUCT(name,cls) \
    typedef CComQIPtr<cls> base_class; \
    DECARE_WEBCLASS_CONSTRUCT_EX(name,cls)


// \
// name(IUnknown* lp) : base_class (lp) {} \
// name(_PtrClass* lp) : base_class (lp) {} \
// name(const base_class& lp) : base_class (lp) {} \
// name(const name& lp) : base_class (lp) {} \
// _PtrClass* operator=(_PtrClass* lp) { return base_class::operator=(lp); } \
// _PtrClass* operator=(const base_class& lp) { return base_class::operator=(lp); } \
// _PtrClass* operator=(IUnknown* lp) { return base_class::operator=(lp); } \
// operator _PtrClass*() { return p; } \


//name(_PtrClass* lp) : base_class (lp) {} \
//operator _PtrClass* () const { return p; } \
//name(_PtrClass* lp) : base_class (lp) {} \
//    operator _PtrClass* () const { return p; }

//    name(const base_class& rhs) : base_class(rhs.p) {} \
//    template <class P> name(const CComQIPtr<P>& rhs) : base_class(rhs.p) {}

//name(const base_class& lp) : base_class (lp) {} \


/*
class WDocument;
//class WFrames;
class WBrowser;
class WWindow;


// class WItem : public CComQIPtr<IHTMLElement>
// {
// public:
//     typedef CComQIPtr<_PtrClass> base_class;
//     WItem() {}
//     WItem(_PtrClass* lp) : base_class (lp) {}
//     WItem(const base_class& lp) : base_class (lp) {}
//     WItem(IUnknown* lp) : base_class (lp) {}
//     template <class P> WItem(const CComQIPtr<P>& rhs)
//         : base_class(rhs.p)
//     {
//
//     }
//
// };


template <class T, class T_Ret>
class WAllT : public CComQIPtr<T>
{
public:
    DECARE_WEBCLASS_CONSTRUCT(WAllT, T);

    long length() { DbgAssert(this->p); long x = 0; this->p->get_length(&x); return x; }
    T_Ret item(int n) { DbgAssert(this->p); CComPtr<IDispatch> dsp; this->p->item(CComVariant(n), CComVariant(), &dsp); return dsp; }
    T_Ret item(LPCWSTR name, int n = 0) { DbgAssert(this->p); CComPtr<IDispatch> dsp; this->p->item(CComVariant(name), CComVariant(n), &dsp); return dsp; }
};

class WAll : public WAllT<IHTMLElementCollection, CComQIPtr<IHTMLElement> >
{
public:
    typedef WAllT<IHTMLElementCollection, CComQIPtr<IHTMLElement> > base_class;
    DECARE_WEBCLASS_CONSTRUCT_EX(WAll, IHTMLElement);

    WAll tags(const CComVariant& tagName) { DbgAssert(this->p); CComPtr<IDispatch> dsp; this->p->tags(tagName, &dsp); return WAll(dsp); }
};

class WItem : public CComQIPtr<IHTMLElement>
{
public:
    DECARE_WEBCLASS_CONSTRUCT(WItem, IHTMLElement);

    WItem parentElement() {DbgAssert(p);  WItem itm; p->get_parentElement(&itm); return itm; }
    WAll children() { DbgAssert(p); CComPtr<IDispatch> dsp; p->get_children(&dsp); return WAll(dsp); }
};

// typedef WAllT<IHTMLElementCollection, CComQIPtr<IDispatch> > WAll;
typedef WAllT<IHTMLFramesCollection2, WWindow > WFrames;
typedef WAllT<IHTMLFormElement, CComQIPtr<IDispatch> > WForm;


class WWindow : public CComQIPtr<IHTMLWindow2>
{
public:
    DECARE_WEBCLASS_CONSTRUCT(WWindow, IHTMLWindow2);

    WBrowser browser();
    WDocument document(); // { WDocument doc; p->get_document(&doc); return doc; }
    WFrames frames(); // { WFrames frm; p->get_frames(&frm); return frm; }
    WWindow self()
    {
        DbgAssert(p);
        WWindow ww;
        (*this)->get_self(&ww);
        return ww;
    }
    WWindow window()
    {
        DbgAssert(p);
        WWindow ww;
        p->get_window(&ww);
        return ww;
    }
};

// class WFrames : public CComQIPtr<IHTMLFramesCollection2>
// {
// public:
//     DECARE_WEBCLASS_CONSTRUCT(WFrames);
//
//     long length() {DbgAssert(p);  long x = 0; p->get_length(&x); return x; }
//     WWindow item(LPCWSTR name) {DbgAssert(p);  CComVariant varf; p->item(&CComVariant(name), &varf); return varf.pdispVal; }
//     WWindow item(int n) {DbgAssert(p);  CComVariant varf; p->item(&CComVariant(n), &varf); return varf.pdispVal; }
// };


class WBody : public CComQIPtr<IHTMLBodyElement>
{
public:
    DECARE_WEBCLASS_CONSTRUCT(WBody, IHTMLBodyElement);
};

class WDocument : public CComQIPtr<IHTMLDocument2>
{
public:
    DECARE_WEBCLASS_CONSTRUCT(WDocument, IHTMLDocument2);

    WAll all() { DbgAssert(p); WAll wa; p->get_all(&wa); return wa; }
    WFrames frames() { DbgAssert(p); WFrames f; p->get_frames(&f); return f; }
    WItem body() { DbgAssert(p); WItem bdy; p->get_body(&bdy); return bdy; }
    WItem CreateScript(BSTR str)
    {
        DbgAssert(p);
        WItem itm;
        p->createElement((BSTR)L"script", &itm);
        itm->setAttribute((BSTR)L"type", CComVariant(L"text/javascript"), 0);
        itm->setAttribute((BSTR)L"text", CComVariant(str), 0);
        return itm;
    }
    WWindow parentWindow()
    {
        DbgAssert(p);
        WWindow ww;
        p->get_parentWindow(&ww);
        return ww;
    }
    WAll links()
    {
        WAll wall;
        p->get_links(&wall);
        return wall;
    }
    WAll images()
    {
        WAll wall;
        p->get_images(&wall);
        return wall;
    }
    WAll forms()
    {
        WAll wall;
        p->get_forms(&wall);
        return wall;
    }

    WItem elementFromPoint(POINT pt)
    {
        WItem itm;
        p->elementFromPoint(pt.x, pt.y, &itm);
        return itm;
    }
};

class WBrowser : public CComQIPtr<IWebBrowser2>
{
public:
    DECARE_WEBCLASS_CONSTRUCT(WBrowser, IWebBrowser2);

    WDocument Document()
    {
        DbgAssert(p);
        CComPtr<IDispatch> dsp;
        p->get_Document(&dsp);
        return WDocument(dsp);
    }

    HRESULT Navigate2(const CComVariant& url, int Flags = navNoHistory | navNoReadFromCache | navNoWriteToCache,
        VARIANT *TargetFrameName = 0, VARIANT* PostData = 0, VARIANT*Headers = 0)
    {
        DbgAssert(p);
        return p->Navigate2(tm_const_cast(&url), tm_addressof(CComVariant(Flags)), TargetFrameName, PostData, Headers);
    }
};


inline WBrowser WWindow::browser()
{
    DbgAssert(p);
    CComQIPtr<IServiceProvider> spServiceProvider(p);
    WBrowser spWebBrws;
    spServiceProvider->QueryService(IID_IWebBrowserApp, IID_IWebBrowser2, (void**)&spWebBrws);
    return spWebBrws;
}

inline WDocument WWindow::document()
{
    DbgAssert(p);
    WDocument doc;
    HRESULT hr = p->get_document(&doc);
    if (!doc)
    {
        // cross frame; When IHTMLWindow2::get_document returns E_ACCESSDENIED
        // http://codecentrix.blogspot.com/2007/10/when-ihtmlwindow2getdocument-returns.html
        WBrowser br = browser();
        return br.Document();
    }
    return doc;
}

inline WFrames WWindow::frames() { DbgAssert(p); WFrames frm; p->get_frames(&frm); return frm; }

class WTableCell : public CComQIPtr<IHTMLTableCell>
{
public:
    DECARE_WEBCLASS_CONSTRUCT(WTableCell, IHTMLTableCell);
};
typedef WAllT<IHTMLElementCollection, WTableCell> WTableCells;


class WTableRow : public CComQIPtr<IHTMLTableRow>
{
public:
    DECARE_WEBCLASS_CONSTRUCT(WTableRow, IHTMLTableRow);

    WTableCells cells() { DbgAssert(p); WTableCells wc; p->get_cells(&wc); return wc; }
};
typedef WAllT<IHTMLElementCollection, WTableRow> WTableRows;

class WTableHead : public CComQIPtr<IHTMLTableSection>
{
public:
    DECARE_WEBCLASS_CONSTRUCT(WTableHead, IHTMLTableSection);

    WTableRows rows() { DbgAssert(p); WTableRows wa; p->get_rows(&wa); return wa; }

};

class WTable : public CComQIPtr<IHTMLTable>
{
public:
    DECARE_WEBCLASS_CONSTRUCT(WTable, IHTMLTable);

    WTableRows rows() { DbgAssert(p); WTableRows wa; p->get_rows(&wa); return wa; }
    WTableHead tHead() { DbgAssert(p); WTableHead head; p->get_tHead(&head); return head; }

};



// typedef WAllT<WItem> WAll;
//
// class WForm : public CComQIPtr<IHTMLFormElement>
// {
// public:
//     DECARE_WEBCLASS_CONSTRUCT(WForm);
//
// }
*/

TM_END_NAMESPACE


/////////////////////////////////////////////////////////////////////////////////////
// moniker
TM_INLINE TMBool MonikerGetFriendlyName(IMoniker* pMoniker, BSTR* str)
{
    DbgAssert(pMoniker);
    CComPtr<IPropertyBag> pBag;
    DbgVerifyHResult(pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pBag));
    if (pBag)
    {
        VARIANT var = {0};
        var.vt = VT_BSTR;
        HRESULT hr = pBag->Read(L"FriendlyName", &var, NULL);
        if (SUCCEEDED(hr))
        {
            *str = var.bstrVal;
            return true;
        }
    }
    return false;
}

// pName sample: L"@device:*:{86DBB310-5D01-11D0-BD3B-00A0C911CE86}";
static TMBool MonikerFromDisplayName(LPCOLESTR pName, IMoniker **ppMoniker)
{
    IBindCtx *pBindCtx;
    HRESULT hr = CreateBindCtx(0, &pBindCtx);
    ULONG chEaten = 0;
    hr = MkParseDisplayName(pBindCtx, pName, &chEaten, ppMoniker);
    pBindCtx->Release();
    return SUCCEEDED(hr);
}

TM_INLINE void VariantToString(CComVariant& var)
{
    if ((var.vt & VT_ARRAY) && var.parray)
    {
        UINT dims = SafeArrayGetDim(var.parray);
        if (dims != 1)
        {
            DbgAssert(0 && "Only 1-dimensional arrays are supported");
            return;
        }
        std::wstring tmp;
        tmp = L"[";
        LONG lBound = 0, uBound = -1;
        SafeArrayGetLBound(var.parray, 1, &lBound);
        SafeArrayGetUBound(var.parray, 1, &uBound);
        for (LONG i = lBound; i <= uBound; ++i)
        {
            CComVariant varTmp;
            varTmp.vt = (var.vt & ~VT_ARRAY);
            SafeArrayGetElement(var.parray, &i, &varTmp.byref);
            if (SUCCEEDED(varTmp.ChangeType(VT_BSTR)))
                tmp += (TMCStrW)varTmp.bstrVal;
            else
                tmp += L"??";
            if (i < uBound)
                tmp += L",";
        }
        tmp += L"]";
        var = tmp.c_str();
        return;
    }
    if (SUCCEEDED(var.ChangeType(VT_BSTR)))
        return;
    DbgAssert(0);
}



#endif // TM_WIN_DESKTOP

#endif // TMCOM_WIN_H
