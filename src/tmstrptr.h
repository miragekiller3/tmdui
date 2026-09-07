/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2022.2.24
 *
 *  Copyright (C) 2022 miragekiller
 */

#pragma once

#ifndef TMSTRPTR_H
#define TMSTRPTR_H

#include <string>
#include "tmatomic.h"
#include "tmstrview.h"

#define size_type1 _size_type_all
#define size_type2 _size_type_0_to_max
#define size_type3 _size_type_0_to_src_size
#define size_type4 _size_type_0_to_dst_size


TM_BGN_NAMESPACE

template <class CharT> class CStrPtrBaseT;


TM_BGN_PRIVATE_NAMESPACE

template <class CharT, class T_Str> struct CStrPtrData;

///////////////////////////////////////////////////////////////////////////////
// _CStrPtrHelper
struct _CStrPtrHelper
{
    template <class CharT>
    static std::basic_string<CharT>& _NewString(CStrPtrBaseT<CharT>& str, TMBool bSyncStr)
    {
        typedef CStrPtrData<CharT, std::basic_string<CharT> > std_data;
        std_data* p = new std_data();
        if (bSyncStr)
            p->m_obj.assign(str.data(), str.size());
        str._DataAttach(p);
        return p->m_obj;
    }

    template <class CharT>
    static std::basic_string<CharT>& _NewString(CStrPtrBaseT<CharT>& str, TMBool bSyncStr, TMBool bSyncView)
    {
        std::basic_string<CharT>& s = _NewString(str, bSyncStr);
        if (bSyncView)
            str._assignview(s);
        return s;
    }

    template <class CharT>
    static std::basic_string<CharT>& _GetString(CStrPtrBaseT<CharT>& str, TMBool bSyncStr) // bSyncStr: assign if true
    {
        if (str.m_ptr)
            return str.m_ptr->GetString(str, bSyncStr, false);
        return _NewString(str, bSyncStr);
    }
};

///////////////////////////////////////////////////////////////////////////////
// IStrPtrData
template <class CharT> struct IStrPtrData
{
    typedef TMRefInt (*PfnAddRef)(IStrPtrData* _this);
    typedef TMRefInt (*PfnRelease)(IStrPtrData* _this);
    typedef TMRefInt (*PfnGetRef)(IStrPtrData* _this);
    typedef std::basic_string<CharT>& (*PfnGetString)(IStrPtrData* _this, CStrPtrBaseT<CharT>& str, TMBool bSyncStr, TMBool bSyncView);
    struct IPtrVtbl
    {
        PfnAddRef  AddRef;
        PfnRelease Release;
        PfnGetRef  GetRef;
        PfnGetString GetString;
    };

    const IPtrVtbl* lpVtbl;

    static const IPtrVtbl       s_cexprVtbl;
    static const IStrPtrData    s_cexprData;

    inline TMRefInt GetRef() { return lpVtbl->GetRef(this); }
    inline TMRefInt AddRef() { return lpVtbl->AddRef(this); }
    inline TMRefInt Release() { return lpVtbl->Release(this); }
    inline std::basic_string<CharT>& GetString(CStrPtrBaseT<CharT>& str, TMBool bSyncStr, TMBool bSyncView) { return lpVtbl->GetString(this, str, bSyncStr, bSyncView); }

    static tm_constexpr11 IStrPtrData* GetDataConstexpr() { return const_cast<IStrPtrData*>(&s_cexprData); }

    // IStrPtrData interface
    static TMRefInt FRef(IStrPtrData* _this) { return 1; }
    static std::basic_string<CharT>& FGetString(IStrPtrData* _this, CStrPtrBaseT<CharT>& str, TMBool bSyncStr, TMBool bSyncView)
    {
        return  _CStrPtrHelper::_NewString(str, bSyncStr, bSyncView);
    }
};


///////////////////////////////////////////////////////////////////////////////
// CStrPtrData
template <class CharT, class T_Str>
struct CStrPtrData : public IStrPtrData<CharT>
{
    TM_SELF_ALLOC_F(CStrPtrData);

#ifdef TM_DBG
    int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf) // CStrPtrData
    {
        CStrPtrData* p = (CStrPtrData*)cls->m_pClass;
        if (pbuf)
            StrAssign(pbuf, 255, std::data(p->m_obj), std::size(p->m_obj));
        return (std::size(p->m_obj) + 1) * sizeof(CharT);
    }
#endif // TM_DBG


    TMAtmInt    m_nRef;
    T_Str       m_obj;
    static const typename IStrPtrData<CharT>::IPtrVtbl s_vtb;

    CStrPtrData()
        : m_nRef(1)
    {
        this->lpVtbl = &s_vtb;
//         *(int*)lpVtbl = 5; // test access denied.
    }

    std::basic_string<CharT>& __GetString(CStrPtrBaseT<CharT>& str, TMBool bSyncStr, TMBool bSyncView, std::basic_string<CharT>*)
    {
        int n = this->m_nRef;
        DbgAssert(n > 0);
        if (n == 1)
        {
            if (bSyncStr && str.size() != this->m_obj.size())
            {
                DbgAssert(str.data() >= this->m_obj.data() && str.data() + str.size() <= this->m_obj.data() + this->m_obj.size());
                this->m_obj.erase(0, str.data() - this->m_obj.data());
                this->m_obj.resize(str.size());
                if (bSyncView)
                    str._assignview(this->m_obj);
            }
            return this->m_obj;
        }
        return _CStrPtrHelper::_NewString(str, bSyncStr, bSyncView);
    }

    std::basic_string<CharT>& __GetString(CStrPtrBaseT<CharT>& str, TMBool bSyncStr, TMBool bSyncView, void*)
    {
        return _CStrPtrHelper::_NewString(str, bSyncStr, bSyncView);
    }

    // IStrPtrData interface
    static TMRefInt FGetRef(IStrPtrData<CharT>* _this) { return static_cast<CStrPtrData*>(_this)->m_nRef; }
    static TMRefInt FAddRef(IStrPtrData<CharT>* _this) { return AtomicInc(&static_cast<CStrPtrData*>(_this)->m_nRef); }
    static TMRefInt FRelease(IStrPtrData<CharT>* _this) { return ref_helper::ReleaseAutoDelete(static_cast<CStrPtrData*>(_this)); }
    static std::basic_string<CharT>& FGetString(IStrPtrData<CharT>* _this, CStrPtrBaseT<CharT>& str, TMBool bSyncStr, TMBool bSyncView)
    {
        CStrPtrData* __this = (CStrPtrData*)(_this);
        return __this->__GetString(str, bSyncStr, bSyncView, &__this->m_obj);
    }
};

///////////////////////////////////////////////////////////////////////////////
// static data
// the s_ptr, s_cexprData, s_vtb must compile in read only data segment.
// template <class CharT>
template <class CharT>
const typename IStrPtrData<CharT>::IPtrVtbl IStrPtrData<CharT>::s_cexprVtbl =
{
    IStrPtrData<CharT>::FRef,
    IStrPtrData<CharT>::FRef,
    IStrPtrData<CharT>::FRef,
    IStrPtrData<CharT>::FGetString,
};

template <class CharT>
const IStrPtrData<CharT> IStrPtrData<CharT>::s_cexprData =
{
    &IStrPtrData<CharT>::s_cexprVtbl
};

template <class CharT, class T_Str>
const typename IStrPtrData<CharT>::IPtrVtbl CStrPtrData<CharT, T_Str>::s_vtb =
{
    CStrPtrData<CharT, T_Str>::FAddRef,
    CStrPtrData<CharT, T_Str>::FRelease,
    CStrPtrData<CharT, T_Str>::FGetRef,
    CStrPtrData<CharT, T_Str>::FGetString,
};

TM_END_PRIVATE_NAMESPACE


///////////////////////////////////////////////////////////////////////////////
// CCharValT
template <class T_Cont> struct CCharValT
{
    typedef typename T_Cont::value_type value_type;
    T_Cont& cont;
    size_t pos;

    CCharValT(T_Cont& c, size_t p) : cont(c), pos(p) { DbgAssert(p <= c.size()); }
    operator const value_type& () const { const T_Cont& _c = cont; return _c.at(pos); }
    value_type operator = (value_type ch) { return cont.begin()[pos] = ch; }
};


///////////////////////////////////////////////////////////////////////////////
// _CString_Funcs
// T_str: std::string or CStrPtrBaseT
template <class T_str> struct _CString_Funcs
{
public:
    typedef typename T_str::value_type          value_type;
    typedef value_type*                         pointer;
    typedef typename T_str::const_pointer       const_pointer;
    typedef std::basic_string<value_type>       std_string;
    typedef CStrPtrBaseT<value_type>            ptr_string;
    typedef std::_tm_string_view<value_type>    std_view;

    tm_constexpr11 static pointer __buffer(ptr_string& str)
    {
        return str._tidy();
    }

    tm_constexpr11 static pointer __buffer(std_string& str)
    {
        return &str.at(0);
    }

    static pointer __buffer(T_str& str, size_t n)
    {
        str.resize(n);
        return __buffer(str);
    }

    template <class P>
    static void __set(T_str& str, size_t nStart, std::_tm_string_view<P> src)
    {
        DbgAssertS(sizeof(value_type) != sizeof(P));

        if (src.length())
        {
            size_t ns;
            if (sizeof(value_type) >= sizeof(P))            // charX->charX; charA(char8)->charX; char16->char32;
                ns = 1;
            else if (sizeof(TMCharA) == sizeof(value_type)) // charX->charA(char8)
                ns = 6;
            else                                            // char32->char16
            {
                DbgAssert(sizeof(P) == sizeof(TMChar32));
                DbgAssert(sizeof(TMCharW) == sizeof(value_type));
                ns = 2;
            }
            ns *= src.length();
            size_t nCopy = StrFill(__buffer(str, nStart + ns) + nStart, src.data(), src.length());
            DbgAssert((int)nCopy >= 0);
            DbgAssert(nCopy <= ns);
            str.resize(nStart + nCopy);
        }
        else
            str.resize(nStart);
    }

    // assign
    static void __assign(std_string& str, const ptr_string& x)                      { str.assign(x.data(), x.length()); }
    static void __assign(ptr_string& str, const ptr_string& x)                      { str.assign(x); }
    static void __assign(T_str& str, const std::_tm_string_view<value_type>& v)     { __safe_assign(str, v); }
    template <class T_ch>
    static void __assign(T_str& str, const std::_tm_string_view<T_ch>& v, ...)      { __set(str, 0, v); }

    static void __assign(T_str& str, const std::basic_string<value_type>& x)        { str.assign(x); }
    template <class T_ch>
    static void __assign(T_str& str, const std::basic_string<T_ch>& x, ...)         { __set(str, 0, std::_tm_string_view<T_ch>(x)); }

    static void __assign(T_str& str, const_pointer p, size_t s)                     { str.assign(p, s); }
    template <class T_ch>
    static void __assign(T_str& str, const T_ch* p, size_t s, ...)                  { __set(str, 0, StrMakeView(p, s)); }

    // safe assign
    tm_constexpr11 static void __safe_assign(std_view& str, const std_view& v) { str = v; }

    static void __safe_assign(ptr_string& str, const std_view& v)
    {
        if (v.data() >= str.data() && tm_cend_data(v) <= tm_cend_data(str))
            str._assignview(v);
        else
            __assign(str, v.data(), v.length());
    }
    static void __safe_assign(std_string& str, const std_view& v)
    {
        if (v.data() >= str.data() && tm_cend_data(v) <= tm_cend_data(str))
        {
            str.erase(0, v.data() - &*tm_cbegin(str));
            str.erase(v.size());
        }
        else
            __assign(str, v.data(), v.length());
    }

    // append
    static void __append(T_str& str, const_pointer p, size_t s)                     { str.append(p, s); }
    template <class T_ch>
    static void __append(T_str& str, const T_ch* p, size_t s, ...)                  { __set(str, str.length(), StrMakeView(p, s)); }
};

///////////////////////////////////////////////////////////////////////////////
// CStrPtrBaseT
template <class CharT>
class CStrPtrBaseT : public std::_tm_string_view<CharT>
{
public:
    typedef CStrPtrBaseT                            class_type;
    typedef std::_tm_string_view<CharT>             base_class;
    typedef std::_tm_string_view<CharT>             std_view;
    typedef std::basic_string<CharT>                std_string;

    typedef typename base_class::size_type          size_type;
    typedef typename base_class::difference_type    difference_type;
    typedef CharT*                                  pointer;
    typedef typename base_class::const_pointer      const_pointer;
    typedef CharT&                                  reference;
    typedef typename base_class::const_reference    const_reference;
    typedef typename base_class::value_type         value_type;
    typedef pointer                                 iterator;
    typedef const_pointer                           const_iterator;
    typedef TM::r_iterT<
        CharT*, CharT&,
        std::random_access_iterator_tag>            reverse_iterator;
    typedef TM::r_iterT<
        const CharT*, const CharT&,
        std::random_access_iterator_tag>            const_reverse_iterator;

    typedef typename PRI::IStrPtrData<CharT>        IStrPtrData;
    typedef _CString_Funcs<class_type>              _funcs;

    typedef size_type                               _size_type_all;
    typedef size_type                               _size_type_0_to_max;
    typedef size_type                               _size_type_0_to_src_size;
    typedef size_type                               _size_type_0_to_dst_size;

    using base_class::npos;
    using base_class::empty;
    using base_class::data;
    using base_class::length;

    friend PRI::_CStrPtrHelper;
    friend _funcs;

protected:
    IStrPtrData* m_ptr;

    struct _string_auto : public TM::CNoCopyable
    {
        class_type*    pthis;
        std_string&    cstr;
        _string_auto(class_type* pt, TMBool bSyncStr = true)
            : pthis(pt)
            , cstr(PRI::_CStrPtrHelper::_GetString(*pt, bSyncStr))
        {
        }
        ~_string_auto() { pthis->_assignview(cstr); }
        std::basic_string<CharT>* operator->() const { return &cstr; }
    };
    friend _string_auto;

    std_string* _string_native()
    {
        DbgAssert(m_ptr);
        return &m_ptr->GetString(*this, true, true);
    }

    // for CString
    pointer _tidy()
    {
        pointer p = this->size() ? (pointer)_string_auto(this)->data() : 0;
        DbgAssert(use_count() <= 1);
        return p;
    }

    const CharT* _c_str(const CharT* dft) const tm_noexcept
    {
        size_t sz = length();
        if (!sz)
            return dft;
        if (data()[sz]) // this is shared string, not end of '\0';
            _string_auto(tm_const_cast(this))->resize(sz);
        return data();
    }

public:
    void _assignview(const std_view& x)             { *(std_view*)this = x; }

protected:
    // construct, only for CString::__make_view
    tm_constexpr11 explicit CStrPtrBaseT(const CStrPtrBaseT& r, const std_view& v) tm_noexcept
        : base_class(v)
        , m_ptr(r.m_ptr)
    {
        if (m_ptr)
            m_ptr->AddRef();
    }

    tm_constexpr20 ~CStrPtrBaseT()
    {
        if (m_ptr)
            m_ptr->Release();
    }

protected: // only call from CStringT.
    tm_constexpr11 explicit CStrPtrBaseT() tm_noexcept
        : m_ptr(0)
    {}

    tm_constexpr11 explicit CStrPtrBaseT(const_pointer p) tm_noexcept
        : base_class(StrMakeView(p))
        , m_ptr(0)
    {
    }

    tm_constexpr11 explicit CStrPtrBaseT(size_t n, const_pointer p) tm_noexcept
        : base_class(p, n)
        , m_ptr(0)
    {
    }

    tm_constexpr11 explicit CStrPtrBaseT(const std_view& r) tm_noexcept
        : base_class(r)
        , m_ptr(0)
    {
    }

    tm_constexpr11 CStrPtrBaseT(const CStrPtrBaseT& r) tm_noexcept
        : base_class(r)
        , m_ptr(r.m_ptr)
    {
        if (m_ptr)
            m_ptr->AddRef();
    }

    tm_constexpr11 explicit CStrPtrBaseT(const std_string& s) tm_noexcept
        : base_class(s)
        , m_ptr(0)
    {
    }

    tm_tpl_cexpr20 explicit CStrPtrBaseT(const P* p, ...) tm_noexcept
        : m_ptr(0)
    {
        _funcs::__assign(*this, StrMakeView(p));
    }

    tm_tpl_cexpr20 explicit CStrPtrBaseT(size_t n, const P* p, ...) tm_noexcept
        : m_ptr(0)
    {
        _funcs::__assign(*this, p, n);
    }

    tm_tpl_cexpr20 explicit CStrPtrBaseT(const std::_tm_string_view<P>& v, ...)
        : m_ptr(0)
    {
        _funcs::__assign(*this, v);
    }

    tm_tpl_cexpr20 explicit CStrPtrBaseT(const CStrPtrBaseT<P>& r, ...)
        : m_ptr(0)
    {
        _funcs::__assign(*this, r);
    }

    tm_tpl_cexpr20 explicit CStrPtrBaseT(const std::basic_string<P>& s, ...)
        : m_ptr(0)
    {
        _funcs::__assign(*this, s);
    }

    tm_tpl_cexpr11 explicit CStrPtrBaseT(string_constant<P> arr) tm_noexcept
        : base_class(arr.data(), arr.length())
        , m_ptr(IStrPtrData::GetDataConstexpr())
    {
    }

    void _DataAttach(IStrPtrData* data)
    {
        IStrPtrData* old = m_ptr;
        m_ptr = data;
        if (old)
            old->Release();
    }

    void _DataAssign(IStrPtrData* data)
    {
        if (data)
            data->AddRef();
        _DataAttach(data);
    }

    void _DataSwap(CStrPtrBaseT& r)
    {
        tm_swap(m_ptr, r.m_ptr);
    }

    // right value reference
#if TM_CPP >= TM_CPP11
    tm_constexpr11 CStrPtrBaseT(std_string&& r)     tm_noexcept : m_ptr(0) { __assign_r(r); }
    tm_constexpr11 CStrPtrBaseT(class_type&& r)     tm_noexcept : m_ptr(r.m_ptr) { r.m_ptr = 0; _assignview(r); }
    CStrPtrBaseT& operator = (std_string&& r)       tm_noexcept { if (r.empty()) clear(); else __assign_r(r); return *this; }
    CStrPtrBaseT& operator = (class_type&& r)       tm_noexcept { assign(r); return *this; }
    tm_constexpr11 void __assign_r(std_string& r)   tm_noexcept { _string_auto(this, false)->assign(std::move(r)); }
#else
    tm_constexpr11 void __assign_r(std_string& r)   tm_noexcept { swap(r); }
#endif // TM_CPP >= TM_CPP11
    tm_tpl_cexpr20 void __assign_r(const P& r, ...) tm_noexcept { assign(r); }

    // const_expr string
    tm_constexpr11 void __assign_c(const_pointer p, size_t n) tm_noexcept
    {
        _DataAssign(IStrPtrData::GetDataConstexpr());
        _assignview(std_view(p, n));
    }

public:
    template <class T_custom> // CCoSTR; CComBSTR; vector<Char>
    void MoveFromCustomString(T_custom& str)
    {
        typedef PRI::CStrPtrData<CharT, T_custom> SPB;
        SPB* p = new SPB();
        std::swap(p->m_obj, str);
        this->_DataAttach(p);
        this->_assignview(std::_tm_string_view<CharT>(std::data(p->m_obj), std::size(p->m_obj)));
    }

public:
    // allocate
    void alloc()
    {
        if (m_ptr || this->empty())
            return;
        std_string str(this->data(), this->size());
        _string_auto(this)->swap(str);
    }

    long use_count() const tm_noexcept
    {
        return m_ptr ? m_ptr->GetRef(): 0;
    }

    bool unique() const tm_noexcept
    {
        return m_ptr ? (1 == m_ptr->GetRef()) : false;
    }

    void freeze()
    {
        if (m_ptr && 1 != m_ptr->GetRef())
        {
            _string_auto sa(this);
        }
        DbgAssert(use_count() <= 1);
    }

    tm_constexpr11 bool is_constexpr() const tm_noexcept
    {
        return m_ptr == IStrPtrData::GetDataConstexpr();
    }

    // append
    void append(const std_string& rhs) { _string_auto(this)->append(rhs); }
    void append(const std_string& rhs, size_type3 _pos, size_type1 _count) { _string_auto(this)->append(rhs, _pos, _count); }
    void append(const std_view& rhs) { if(length()) append(rhs.data(), rhs.length()); else assign(rhs); }
    void append(const_pointer p, size_type2 _count) { _string_auto(this)->append(p, _count); }
    void append(const_pointer p) { _string_auto(this)->append(p); }
    void append(size_type2 _count, value_type _ch) { _string_auto(this)->append(_count, _ch); }
    void append(const_iterator _first, const_iterator _last) { _string_auto(this)->append(_first, _last); }

    // assign
    void assign(const CStrPtrBaseT& x, size_type pos, size_type n) { assign(x.substr(pos, n)); }
    void assign(const CStrPtrBaseT& x) { _DataAssign(x.m_ptr); _assignview(x); }
    void assign(const std_view& x) { _DataAttach(0); _assignview(x); }
    void assign(const std_string& rhs) { assign(rhs.data(), rhs.length()); }
    void assign(const_pointer p, size_type2 _count) { assign(std_view(p, _count)); }
    void assign(const_pointer p) { assign(StrMakeView(p)); }
    void assign(size_type2 _count, value_type _ch) { _string_auto(this, false)->assign(_count, _ch); }
    void assign(const_iterator _first, const_iterator _last) { _string_auto(this, false)->assign(_first, _last); }

    // element access
    tm_constexpr11 const_reference at(size_type pos) const { DbgAssert(pos <= length()); return this->data()[pos]; }
    CCharValT<class_type> at(size_type pos) { return CCharValT<class_type>(*this, pos); }
    CCharValT<class_type> front() { return at(0); }
    const CharT& front() const { return at(0); }
    CCharValT<class_type> back() { return at(length() - 1); }
    const CharT& back() const { return at(length() - 1); }
    CCharValT<class_type> operator[](int pos) { return this->at(pos); }
    tm_constexpr11 const_reference operator[](int pos) const { return this->at(pos); }

    const CharT* c_str() const tm_noexcept { return _c_str((const CharT*)L"\0"); }
    operator const_pointer() const tm_noexcept { return _c_str(0); }

    iterator begin() tm_noexcept { alloc(); return &*_string_native()->begin(); }
    tm_constexpr11 const_iterator begin() const tm_noexcept { return data(); }
    tm_constexpr11 const_iterator cbegin() const tm_noexcept { return data(); }
    iterator end() tm_noexcept { alloc(); return begin() + length(); }
    tm_constexpr11 const_iterator end() const tm_noexcept { return data() + length(); }
    tm_constexpr11 const_iterator cend() const tm_noexcept { return data() + length(); }

    reverse_iterator rbegin() tm_noexcept { return end(); }
    tm_constexpr11 const_reverse_iterator rbegin() const tm_noexcept { return end(); }
    tm_constexpr11 const_reverse_iterator crbegin() const tm_noexcept { return cend(); }
    reverse_iterator rend() tm_noexcept { return begin(); }
    tm_constexpr11 const_reverse_iterator rend() const tm_noexcept { return begin(); }
    tm_constexpr11 const_reverse_iterator crend() const tm_noexcept { return cbegin(); }

    void reserve(size_type new_cap = 0) { _string_auto(this)->reserve(new_cap); }
    size_type capacity() const tm_noexcept { return m_ptr ? tm_const_cast(this)->_string_native()->capacity() : 0; }

    void clear() tm_noexcept { _DataAttach(0); _assignview(std_view()); }
    void resize(size_type count) { if (count <= length()) _assignview(std_view(data(), count)); else _string_auto(this)->resize(count); }
    void resize(size_type count, value_type ch) { if (count <= length()) _assignview(std_view(data(), count)); else _string_auto(this)->resize(count, ch); }
    void swap(class_type& rhs) { _DataSwap(rhs); ((std_view*)this)->swap(rhs); }
    void swap(std_string& rhs) { if (!rhs.empty() || !empty()) _string_auto(this)->swap(rhs); }

    // insert
    void insert(size_type index, size_type count, CharT ch) { _string_auto(this)->insert(index, count, ch); }
    void insert(size_type index, const CharT* s) { _string_auto(this)->insert(index, s);  }
    void insert(size_type index, const CharT* s, size_type count) { _string_auto(this)->insert(index, s, count); }
    void insert(size_type index, std_view str) { insert(index, str.data(), str.length()); }
    void insert(size_type index, std_view str, size_type s_index, size_type count = npos) { insert(index, str.Left(s_index, count)); }
    iterator insert(const_iterator pos, CharT ch) { size_type p = pos - data(); _string_auto(this)->insert(p, 1, ch); return tm_const_cast(data()) + p +1; }
    void insert(const_iterator pos, size_type count, CharT ch) { _string_auto(this)->insert(pos - data(), count, ch); }
    template<class InputIt>
    void insert(const_iterator pos, InputIt first, InputIt last) { _string_auto s(this); s->insert(s->begin() + (pos - s->data()), first, last); }

    // erase
    void erase(size_type index = 0, size_type count = npos) { _string_auto(this)->erase(index, count); }
    iterator erase(const_iterator pos) { _string_auto s(this); iterator iter = (s->erase(s->begin() + (pos - s->data())) - s->begin()) + tm_const_cast(s->data()); return iter; }
    iterator erase(const_iterator first, const_iterator last) { _string_auto s(this); iterator iter = (s->erase(s->begin() + (first - s->data()), s->begin() + (last - s->data())) - s->begin()) + tm_const_cast(s->data()); return iter; }

    /*
    size_type copy(value_type *p, size_type _count, size_type _pos0 = 0) const
        {if (_Len < _pos0)
            _Xran();
        if (_Len - _pos0 < _count)
            _count = _Len - _pos0;
        if (0 < _count)
            _Tr::copy(p, _postr + _pos0, _count);
        return (_count); }
*/

    // replace
    void replace(size_type pos, size_type count, std_view str) { replace(pos, count, str.data(), str.length()); }
    void replace(const_iterator first, const_iterator last, std_view str) { replace(first, last, str.data(), str.length()); }
    void replace(size_type pos, size_type count, std_view str, size_type pos2, size_type count2 = npos) { replace(pos, count, str.Left(pos2, count2)); }
    void replace(size_type pos, size_type count, const CharT* cstr, size_type count2) { _string_auto(this)->replace(pos, count, cstr, count2); }
    void replace(const_iterator first, const_iterator last, const CharT* cstr, size_type count2) { _string_auto s(this); s->replace(first - s->data(), last - first, cstr, count2); }
    void replace(size_type pos, size_type count, const CharT* cstr) { _string_auto(this)->replace(pos, count, cstr); }
    void replace(const_iterator first, const_iterator last, const CharT* cstr) { _string_auto s(this); s->replace(first - s->data(), last - first, cstr); }
    void replace(size_type pos, size_type count, size_type count2, CharT ch) { _string_auto(this)->replace(pos, count, count2, ch); }
    void replace(const_iterator first, const_iterator last, size_type count2, CharT ch) { _string_auto s(this); s->replace(first - s->data(), last - first, count2, ch); }

    // use std::string_view::compare, find, rfind, find first one of, find last one of, find_first_not_of, find_last_not_of
};

TM_END_NAMESPACE


TM_BGN_NAMESPACE_X(std)
template <class CharT> void swap(TM::CStrPtrBaseT<CharT>& l, TM::CStrPtrBaseT<CharT>& r, ...) { l.swap(r); }
template <class CharT, class _Tr, class _A> void swap(std::basic_string<CharT, _Tr, _A>& l, TM::CStrPtrBaseT<CharT>& r, ...) { r.swap(l); }
template <class CharT, class _Tr, class _A> void swap(TM::CStrPtrBaseT<CharT>& l, std::basic_string<CharT, _Tr, _A>& r, ...) { l.swap(r); }
TM_END_NAMESPACE_X(std)



#endif //TMSTRPTR_H
