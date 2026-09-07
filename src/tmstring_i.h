/*
 *  @file
 *  @brief  CString inline file
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2024.9.20
 *
 *  Copyright (C) 2024 miragekiller
 */


#if !defined(CharT) || !defined(_CSTRING_MODE)
#   error the file must include from "tmstring.h"
#endif


#if _CSTRING_MODE == _CSTRING_MODE_VIEW
#   define str_constexpr_       tm_constexpr17
#else
#   define str_constexpr_       //tm_constexpr20
#endif

#if _CSTRING_MODE == _CSTRING_MODE_VIEW
#   define _CSTRING_MODE_NOW    _cstring_mode_view
#elif _CSTRING_MODE == _CSTRING_MODE_STD
#   define _CSTRING_MODE_NOW    _cstring_mode_std
#elif _CSTRING_MODE == _CSTRING_MODE_STRONG
#   define _CSTRING_MODE_NOW    _cstring_mode_strong
#elif _CSTRING_MODE == _CSTRING_MODE_WEAK
#   define _CSTRING_MODE_NOW    _cstring_mode_weak
#else
#   error "not support mode"
#endif


TM_BGN_NAMESPACE


////////////////////////////////////////////////////////////////////////////////
// CStringT
template <>
class CStringT<CharT, _CSTRING_MODE_NOW<CharT> > : public _CSTRING_MODE_NOW<CharT>::_base_class
{
public:
    typedef CStringT                                    class_type;
    typedef _CSTRING_MODE_NOW<CharT>::_base_class       base_class;
    typedef CharT                                       value_type;
    typedef base_class::reference                       reference;
    typedef base_class::const_reference                 const_reference;
    typedef CharT*                                      pointer;
    typedef const CharT*                                const_pointer;
    typedef base_class::iterator                        iterator;
    typedef base_class::size_type                       size_type;
    typedef ptrdiff_t                                   difference_type;

    typedef std::basic_string<CharT>                    std_string;
    typedef CStrPtrBaseT<CharT>                         ptr_string;
    typedef std::_tm_string_view<CharT>                 std_view;
    typedef _CSTRING_MODE_NOW<CharT>::_funcs            _funcs;

    enum { bWriteable = true };

    DbgAssertS1((
        std::is_same<CharT, TMCharA>::value ||
        std::is_same<CharT, TMChar8>::value ||
        std::is_same<CharT, TMChar16>::value ||
        std::is_same<CharT, TMChar32>::value));

#if _CSTRING_MODE == _CSTRING_MODE_VIEW || _CSTRING_MODE == _CSTRING_MODE_STD
    typedef CStringT<CharT, _cstring_mode_view<CharT> > view_type;
protected:
    tm_tpl_cexpr11 view_type __make_view(P f, P l)      const tm_noexcept { return __make_view(f, l - f); }
    tm_tpl_cexpr11 view_type __make_view(P f, size_t s) const tm_noexcept { return view_type(&*f, s); }
#else
    typedef CStringT<CharT, _cstring_mode_weak<CharT> > view_type;
protected:
    tm_tpl_cexpr17 view_type __make_view(P f, P l)      const tm_noexcept { return __make_view(f, l - f); }
    tm_tpl_cexpr17 view_type __make_view(P f, size_t s) const tm_noexcept { return view_type(*this, std_view(&*f, s)); }
#endif //_CSTRING_MODE == _CSTRING_MODE_VIEW

    tm_constexpr11 inline void _alloc() { _CSTRING_MODE_NOW<CharT>::alloc(*this); }

#if _CSTRING_MODE == _CSTRING_MODE_WEAK
    tm_constexpr17 explicit CStringT(const ptr_string& r, const std_view& v) tm_noexcept : base_class(r, v) {}
    tm_constexpr17 explicit CStringT(const std_string& r, const std_view& v) tm_noexcept : base_class(v) {}
    friend CStringT<CharT, _cstring_mode_strong<CharT> >;
#endif // _CSTRING_MODE == _CSTRING_MODE_WEAK

public:
    // right value reference
#if _CSTRING_MODE != _CSTRING_MODE_VIEW
#if TM_CPP >= TM_CPP11
    CStringT(std_string&& r) : base_class(std::move(r))             {}
    CStringT(ptr_string&& r) : base_class(std::move(r))             { _alloc(); }
    CStringT& operator =(std_string&& r)                            tm_noexcept { Assign(std::move(r)); return *this; }
    CStringT& operator =(ptr_string&& r)                            tm_noexcept { Assign(std::move(r)); return *this; }
    tm_constexpr20 void Assign(std_string&& s)                      tm_noexcept { this->__assign_r(s); _alloc(); }
    tm_constexpr20 void Assign(ptr_string&& v)                      tm_noexcept { this->__assign_r(v); _alloc(); }
#endif
    tm_tpl_cexpr11 CStringT(rref_obj<P> r)                          { this->__assign_r(*r); _alloc(); }
    tm_tpl_cexpr11 CStringT& operator =(rref_obj<P> r)              { this->__assign_r(*r); _alloc(); return *this; }
#else
#endif //_CSTRING_MODE != _CSTRING_MODE_VIEW

    ///////////////////////////////////////////////////////////////////////
    // construct
    tm_constexpr11 CStringT()                                       tm_noexcept {}
    str_constexpr_ CStringT(const TMRangeT<const CharT*>& rhs)      : base_class(rhs.m_size, rhs.m_data) { _alloc(); }

#if _CSTRING_MODE == _CSTRING_MODE_STD
    tm_tpl_cexpr20 CStringT(string_constant<P> a)                   : base_class(a) {}
#else
    tm_tpl_cexpr11 CStringT(string_constant<P> a)                   tm_noexcept : base_class(a) {}
#endif // CSTRING_MODE != _CSTRING_MODE_STD

#if _CSTRING_MODE != _CSTRING_MODE_VIEW
    tm_tpl_cexpr11 CStringT(const std::_tm_string_view<P>& v)       : base_class(v) { _alloc(); }
    tm_tpl_cexpr20 CStringT(const std::basic_string<P>& s)          : base_class(s) { _alloc(); }
    tm_tpl_cexpr20 CStringT(const CStrPtrBaseT<P>& v)               : base_class(v) { _alloc(); }
//     str_constexpr_ CStringT(size_type n, value_type c)              : base_class(n, c) {}
    str_constexpr_ CStringT(TMCStrA p, size_type n)                 : base_class(n, p) { _alloc(); }
    str_constexpr_ CStringT(TMCStrW p, size_type n)                 : base_class(n, p) { _alloc(); }
    str_constexpr_ CStringT(TMCStrA p)                              : base_class(p) { _alloc(); }
    str_constexpr_ CStringT(TMCStrW p)                              : base_class(p) { _alloc(); }
#else
    tm_constexpr11 CStringT(const std_view& v)                      tm_noexcept : base_class(v) {}
    tm_constexpr11 CStringT(const std_string& s)                    tm_noexcept : base_class(s) {}
    tm_constexpr11 CStringT(const_pointer p, size_type n)           tm_noexcept : base_class(n, p) {}
    tm_constexpr17 CStringT(const_pointer p)                        tm_noexcept : base_class(p) {}
#endif // _CSTRING_MODE != _CSTRING_MODE_VIEW


    ///////////////////////////////////////////////////////////////////////
    // operator =
#if _CSTRING_MODE != _CSTRING_MODE_VIEW
    tm_tpl_cexpr20 CStringT& operator =(const std::_tm_string_view<P>& v)   { Assign(v); return *this; }
    tm_tpl_cexpr20 CStringT& operator =(const std::basic_string<P>& r)      { Assign(r); return *this; }
    tm_tpl_cexpr20 CStringT& operator =(string_constant<P> a)               { Assign(a); return *this; }
    tm_tpl_cexpr20 CStringT& operator =(const CStrPtrBaseT<P>& r)           { Assign(r); return *this; }
    tm_constexpr20 CStringT& operator =(const CStringT& s)                  { Assign(s); return *this; }
    CStringT& operator =(value_type c)                                      { Assign(c); return *this; }
    CStringT& operator =(TMCStrA p)                                         { Assign(p); return *this; }
    tm_constexpr20 CStringT& operator =(TMCStrW p)                          { Assign(p); return *this; }
#else // _CSTRING_MODE != _CSTRING_MODE_VIEW
    tm_constexpr11 CStringT& operator =(const std_view& v)                  tm_noexcept { Assign(v); return *this; }
    tm_constexpr11 CStringT& operator =(const std_string& r)                tm_noexcept { Assign(r); return *this; }
//     tm_tpl_cexpr11 CStringT& operator =(string_constant<P> a)               tm_noexcept { Assign(a); return *this; }
    tm_constexpr11 CStringT& operator =(const CStrPtrBaseT<CharT>& r)       tm_noexcept { Assign(r); return *this; }
    tm_constexpr11 CStringT& operator =(const CStringT& s)                  tm_noexcept { Assign(s); return *this; }
    tm_constexpr17 CStringT& operator =(const_pointer p)                    tm_noexcept { Assign(p); return *this; }
#endif //_CSTRING_MODE != _CSTRING_MODE_VIEW

    ///////////////////////////////////////////////////////////////////////
    // assign
#if _CSTRING_MODE != _CSTRING_MODE_VIEW
    tm_tpl_cexpr20 void Assign(const std::_tm_string_view<P>& v)    { _funcs::__assign(*this, v); _alloc(); } // safe func.
    tm_tpl_cexpr20 void Assign(const std::basic_string<P>& s)       { _funcs::__assign(*this, s); _alloc(); }
    tm_tpl_cexpr20 void Assign(const CStrPtrBaseT<P>& v)            { _funcs::__assign(*this, v); _alloc(); }
    tm_tpl_cexpr20 void Assign(string_constant<P> a)                { this->__assign_c(a.data(), a.size()); }
    void Assign(value_type c)                                       { this->assign(1, c); _alloc(); }
    void Assign(TMCStrA p)                                          { Assign(StrMakeView(p)); }
    tm_constexpr20 void Assign(TMCStrW p)                           { Assign(StrMakeView(p)); }
    void Assign(TMCStrA p, size_type n)                             { _funcs::__assign(*this, p, n); _alloc(); }
    void Assign(TMCStrW p, size_type n)                             { _funcs::__assign(*this, p, n); _alloc(); }
    tm_tpl_cexpr20 void Assign(P f, P l)                            { this->assign(&*f, l - f); }
#else // _CSTRING_MODE != _CSTRING_MODE_VIEW
    tm_constexpr11 void Assign(const std_view& v)                   tm_noexcept { this->assign(v); }
    tm_constexpr11 void Assign(const std_string& s)                 tm_noexcept { this->assign(s); }
    tm_constexpr11 void Assign(const ptr_string& v)                 tm_noexcept { this->assign(v); }
//     tm_tpl_cexpr11 void Assign(string_constant<P> a)                tm_noexcept { this->assign(a.data(), a.size()); }
    tm_constexpr17 void Assign(const_pointer p)                     tm_noexcept { this->assign(p); }
    tm_constexpr11 void Assign(const_pointer p, size_type n)        tm_noexcept { this->assign(p, n); }
    tm_tpl_cexpr11 void Assign(P f, P l)                            tm_noexcept { this->assign(&*f, l - f); }
#endif // _CSTRING_MODE != _CSTRING_MODE_VIEW


    ///////////////////////////////////////////////////////////////////////
    // SafeAssign
    // for string issue: std::string str(p, 128); str.assign(str.c_str() + 30, 1) will crash.
    void SafeAssign(const CharT* p)                                 { if (!p) this->clear(); else SafeAssign(StrMakeView(p)); }
    void SafeAssign(const CharT* p, size_type n)                    { SafeAssign(StrMakeView(p, n)); }
    void SafeAssign(std::_tm_string_view<CharT> v)                  { _funcs::__assign(*this, v); }


#if _CSTRING_MODE != _CSTRING_MODE_VIEW
    ///////////////////////////////////////////////////////////////////////
    // append
    tm_tpl_cexpr20 void Append(const std::_tm_string_view<P>& v)    { Append(v.data(), v.length()); }
    tm_tpl_cexpr20 void Append(const std::basic_string<P>& s)       { if (this->empty()) Assign(s); else Append(s.data(), s.length()); }
    tm_tpl_cexpr20 void Append(const CStrPtrBaseT<P>& v)            { if (this->empty()) Assign(v); else Append(v.data(), v.length()); }
    void Append(value_type c)                                       { this->append(1, c); }
    tm_constexpr20 void Append(TMCStrA p)                           { Append(StrMakeView(p)); }
    tm_constexpr20 void Append(TMCStrW p)                           { Append(StrMakeView(p)); }
    void Append(TMCStrA p, size_type n)                             { _funcs::__append(*this, p, n); }
    void Append(TMCStrW p, size_type n)                             { _funcs::__append(*this, p, n); }

    ///////////////////////////////////////////////////////////////////////
    // operator +
    tm_tpl_cexpr20 CStringT operator +(const std::_tm_string_view<P>& v)const { return std::move(CStringT(*this) += v); }
    tm_tpl_cexpr20 CStringT operator +(const CStrPtrBaseT<P>& r)        const { return std::move(CStringT(*this) += r); }
    tm_tpl_cexpr20 CStringT operator +(const std::basic_string<P>& s)   const { return std::move(CStringT(*this) += s); }
    CStringT operator +(value_type c)                                   const { return std::move(CStringT(*this) += c); }
//    CStringT operator +(const CStringT& rhs)                            const { return std::move(CStringT(*this) += rhs); }
    CStringT operator +(TMCStrA p)                                      const { return std::move(CStringT(*this) += p); }
    CStringT operator +(TMCStrW p)                                      const { return std::move(CStringT(*this) += p); }

    ///////////////////////////////////////////////////////////////////////
    // operator +=
    tm_tpl_cexpr20 CStringT& operator +=(const std::_tm_string_view<P>& v)  { Append(v); return *this; }
    tm_tpl_cexpr20 CStringT& operator +=(const std::basic_string<P>& r)     { Append(r); return *this; }
    tm_tpl_cexpr20 CStringT& operator +=(const CStrPtrBaseT<P>& r)          { Append(r); return *this; }
    CStringT& operator +=(value_type c)                                     { Append(c); return *this; }
//    tm_constexpr20 CStringT& operator +=(const CStringT& r)             { Append(r); return *this; }
    tm_constexpr20 CStringT& operator +=(TMCStrA p)                         { Append(p); return *this; }
    tm_constexpr20 CStringT& operator +=(TMCStrW p)                         { Append(p); return *this; }

#if TM_CPP >= TM_CPP11  // right operator
    tm_constexpr20 void Append(std_string&& s)                          { if (this->empty()) Assign(std::move(s)); else Append(s.data(), s.length()); }
    tm_constexpr20 void Append(ptr_string&& v)                          { if (this->empty()) Assign(std::move(v)); else Append(v.data(), v.length()); }
    tm_constexpr20 CStringT operator +(std_string&& r)                  const { return std::move(CStringT(*this) += std::move(r)); }
    tm_constexpr20 CStringT operator +(ptr_string&& s)                  const { return std::move(CStringT(*this) += std::move(s)); }
    tm_constexpr20 CStringT& operator +=(std_string&& r)                { Append(std::move(r)); return *this; }
    tm_constexpr20 CStringT& operator +=(ptr_string&& r)                { Append(std::move(r)); return *this; }
#endif

#endif // _CSTRING_MODE != _CSTRING_MODE_VIEW


    // operator ==, !=, <
    tm_constexpr17 bool operator ==(const_pointer p)        const tm_noexcept { return 0 == Compare(p); }
    tm_constexpr17 bool operator ==(const std_string& s)    const tm_noexcept { return 0 == Compare(s); }
    tm_constexpr17 bool operator ==(const std_view& v)      const tm_noexcept { return 0 == Compare(v); }
    tm_constexpr17 bool operator !=(const_pointer p)        const tm_noexcept { return 0 != Compare(p); }
    tm_constexpr17 bool operator !=(const std_view& v)      const tm_noexcept { return !operator == (v); }
    tm_constexpr17 bool operator !=(const std_string& s)    const tm_noexcept { return !operator == (s); }
    tm_constexpr17 bool operator <(const_pointer p)         const tm_noexcept { return Compare(p) < 0; }
    tm_constexpr17 bool operator <(const std_string& s)     const tm_noexcept { return Compare(s) < 0; }
    tm_constexpr17 bool operator <(const std_view& v)       const tm_noexcept { return Compare(v) < 0; }


#if _CSTRING_MODE != _CSTRING_MODE_VIEW

    // if change the position of the terminator need to reset the length by calling ReleaseBuffer()
    tm_constexpr20 pointer GetBuffer() { return _funcs::__buffer(*this); }
    pointer GetBuffer(size_t n) { return _funcs::__buffer(*this, n); }
    void ReleaseBuffer()
    {
        size_type n = StrLen(this->data(), this->length());
        DbgAssert(n <= capacity());
        this->resize(n);
//         CHECKMEMORY();
    }

#ifdef TM_WIN
    void Assign(const VARIANT& var, TMBool bAutoChangeType)
    {
        if (VT_BSTR == var.vt)
            Assign((TMCStrW)var.bstrVal);
        else if (bAutoChangeType)
        {
            VARIANT varNew = {0};
            VariantChangeType(&varNew, const_cast<VARIANT*>(&var), 0, VT_BSTR);
            if (VT_BSTR == var.vt)
                Assign((TMCStrW)varNew.bstrVal);
            else
                clear();
            ::VariantClear(&varNew);
        }
        else
            clear();
    }
#endif // #ifdef TM_WIN

    void FormatV(size_t nMaxSize, size_t nPos, const_pointer pFormat, va_list vl)
    {
        DbgAssert(nPos <= GetLength());
        for (int l = tm_min(512, nMaxSize); ;)
        {
            int n = StrVPrintf(GetBuffer(l+ nPos) + nPos, l, pFormat, vl);
            if (n >= 0)
            {
                SetLength(nPos + n);
                return;
            }
            if (l >= nMaxSize)
                break;
            l *= 2;
            tm_setmin(l, nMaxSize);
        }
        ReleaseBuffer();
    }

    void FormatV(const_pointer pFormat, va_list vl)
    {
        this->FormatV(INT_MAX, 0, pFormat, vl);
    }

    void Format(const_pointer pFormat, ...)
    {
        va_list va;
        va_start(va, pFormat);
        this->FormatV(pFormat, va);
        va_end(va);
    }

    // maxSize must < 32767 when 32bit
    void Format(size_t nMaxSize, size_t nPos, const_pointer pFormat, ...)
    {
        va_list va;
        va_start(va, pFormat);
        this->FormatV(nMaxSize, nPos, pFormat, va);
        va_end(va);
    }

    CStringT& AppendFormat(const_pointer pFormat, ...)
    {
        va_list va;
        va_start(va, pFormat);
        this->FormatV(INT_MAX, this->length(), pFormat, va);
        va_end(va);
        return *this;
    }

    size_t FormatMessage(const_pointer lpszFormat, ...)
    {
#ifdef TM_WIN
        // format message into temporary buffer lpszTemp
        va_list argList;
        va_start(argList, lpszFormat);
        pointer lpszTemp;
        size_t sz = StrSwitch(lpszFormat, ::FormatMessageA, ::FormatMessageW)(
            FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER,
            lpszFormat, 0, 0, (pointer)&lpszTemp, 0, &argList);
        Assign(*lpszTemp);
        LocalFree(lpszTemp);
        va_end(argList);
        return sz;
#else
        tm_unused(lpszFormat); return 0;
#endif // TM_WIN
    }

    void Delete(size_t pos, size_t nCount = base_class::npos)
    {
        this->erase(pos, nCount);
    }

    // reverse string
    void MakeReverse() { std::reverse(this->begin(), this->end()); }

    void MakeLower() { strLower(GetBuffer(), GetLength()); }

    void MakeUpper() { StrUpper(GetBuffer(), GetLength()); }

#if defined (TM_WIN)
    ////////////////////////////////////////////////////////////////////////
    // for windows
    static int _LoadString(HINSTANCE hRes, int nID, TMStrA pBuf, int nBuf)
    {
        return ::LoadStringA(hRes, nID, pBuf, nBuf);
    }

    static int _LoadString(HINSTANCE hRes, int nID, TMStrW pBuf, int nBuf)
    {
        return ::LoadStringW(hRes, nID, pBuf, nBuf);
    }

    void LoadString(HINSTANCE hRes, UINT nID)
    {
        int nLen = 0;
        for (int i = 512; ; i *= 2)
        {
            nLen = _LoadString(hRes, nID, GetBuffer(i), i);
            if (nLen < i)
                break;
        }
        SetLength(nLen);
    }

    static string_constant<TMCharW> FastLoadString(HINSTANCE hRes, UINT nID)
    {
        HRSRC hrsrc = FindResource(hRes, MAKEINTRESOURCE((LOWORD(nID) >> 4) + 1), (LPCTSTR)RT_STRING);
        if (!hrsrc)
            return string_constant<TMCharW>(0, 0);
        HGLOBAL hmem = LoadResource(hRes, hrsrc);
        if (!hmem)
            return string_constant<TMCharW>(0, 0);
        const WCHAR* p = (const WCHAR*)LockResource(hmem);
        int string_num = nID & 0x000f;
        for (int i = 0; i < string_num; i++)
            p += *p + 1;
        return string_constant<TMCharW>(p + 1, *p);
    }

#endif // TM_WIN

#ifdef __ATLBASE_H__
    void LoadString(UINT nID)
    {
#   if (_ATL_VER >= 0x0700)
        LoadString(ATL::_AtlBaseModule.GetResourceInstance(), nID);
#   else // !(_ATL_VER >= 0x0700)
        LoadString(ATL::_pModule->GetResourceInstance(), nID);
#   endif // !(_ATL_VER >= 0x0700)
    }
#endif
    
#define _SS(x) StrSwitch(const_pointer(0), x, L##x)

    void XMLEncode()
    {
        Replace(_SS("&"), _SS("&amp;"));
        Replace(_SS("\""), _SS("&quot;"));
        Replace(_SS("\'"), _SS("&apos;"));
        Replace(_SS("<"), _SS("&lt;"));
    }

    void XMLDecode()
    {
        Replace(_SS("&lt;"), _SS("<"));
        Replace(_SS("&gt;"), _SS(">"));
        Replace(_SS("&quot;"), _SS("\""));
        Replace(_SS("&apos;"), _SS("\'"));
        Replace(_SS("&amp;"), _SS("&"));
    }

#undef _SS

    void Replace(value_type cOld, value_type cNew, int nStart = 0)
    {
        std::replace(this->begin() + nStart, this->end(), cOld, cNew);
    }

    int Replace(const_pointer pOld, const_pointer pNew)
    {
        if (!pOld || !pOld[0])
            return 0;

        size_type pos = 0;
        int no = StrLen(pOld);
        int nn = pNew ? StrLen(pNew) : 0;

        int i = 0;
        for (; (pos = this->find(pOld, pos, no)) != this->npos; pos += nn, ++i)
        {
            TM_IF_DBG(int oldlen = this->length());
            replace(pos, no, pNew, nn);
            DbgAssert(this->length() - oldlen == nn - no);
        }
        return i;
    }

    int ReplaceI(const_pointer pOld, const_pointer pNew)
    {
        if (!pOld || !pOld[0])
            return 0;

        size_type pos = 0;
        int no = StrLen(pOld);
        int nn = pNew ? StrLen(pNew) : 0;

        int i = 0;
        for (; (pos = FindI(pOld, pos)) != this->npos; pos += nn, ++i)
            replace(pos, no, pNew, nn);
        return i;
    }

private:
    static bool _IsWord(int c)
    {
        return c == '_' || isalnum(c);
    }

    void _ReplaceWholeWord(size_type& pos, size_type no, const_pointer pNew, size_type nn, int& count)
    {
        if ((pos != 0 && _IsWord(this->data()[pos - 1])) || ((pos + no) < this->length() && _IsWord(this->data()[pos + no])))
            pos += no;
        else
        {
            TM_IF_DBG(int oldlen = this->length());
            replace(pos, no, pNew, nn);
            DbgAssert(this->length() - oldlen == nn - no);
            pos += nn;
            ++count;
        }
    }

public:
    int ReplaceWholeWord(const_pointer pOld, const_pointer pNew)
    {
        if (!pOld || !pOld[0])
            return 0;

        size_type pos = 0;
        int no = StrLen(pOld);
        int nn = pNew ? StrLen(pNew) : 0;

        int i = 0;
        for (; (pos = this->find(pOld, pos, no)) != this->npos; )
            _ReplaceWholeWord(pos, no, pNew, nn, i);
        return i;
    }

    int ReplaceWholeWordI(const_pointer pOld, const_pointer pNew)
    {
        if (!pOld || !pOld[0])
            return 0;
        
        size_type pos = 0;
        int no = StrLen(pOld);
        int nn = pNew ? StrLen(pNew) : 0;
        
        int i = 0;
        for (; (pos = FindI(pOld, pos)) != this->npos; )
            _ReplaceWholeWord(pos, no, pNew, nn, i);
        return i;
    }

    int Remove(value_type ch)
    {
        iterator iter = this->begin();
        size_t len = this->length();
        int n = std::remove(iter, iter + len, ch) - iter;
        this->resize(n);
        return len - n;
    }

    template <class _F>
    int RemoveIf(_F __func)
    {
        iterator iter = this->begin();
        size_t len = this->length();
        int n = std::remove_if(iter, iter + len, __func) - iter;
        this->resize(n);
        return len - n;
    }

    int RemoveBlanks()
    {
        return RemoveIf(StrIsSpace);
    }

    void SetAt(int n, value_type ch) { this->at(n) = ch; }

    void Empty() { this->clear(); }

#endif // _CSTRING_MODE != _CSTRING_MODE_VIEW

    tm_constexpr17 int Compare(const_pointer p)         const tm_noexcept { return p ? ToView().compare(p) : ToView().compare(std_view()); }
    tm_constexpr17 int Compare(const std_string& s)     const tm_noexcept { return Compare(std_view(s)); }
    tm_constexpr17 int Compare(const std_view& v)       const tm_noexcept { return ToView().compare(v); }
    int CompareNoCase(const_pointer p)                  const tm_noexcept { return StrCompareI(this->data(), this->length(), p, -1); }
    int CompareNoCase(const std_string& s)              const tm_noexcept { return CompareNoCase(std_view(s)); }
    int CompareNoCase(const std_view& v)                const tm_noexcept { return StrCompareI(this->data(), this->length(), v.data(), v.length()); }

    str_constexpr_ value_type GetAt(int n) const { return this->at(n); }
    str_constexpr_ TMBool IsEmpty() const { return this->empty(); }

    // length not include '\0'
    str_constexpr_ size_type GetLength () const { return this->length(); }
    void SetLength(size_type n) { this->resize(n); }

    ////////////////////////////////////////////////////////////////////////
    // find function
    // if not find return -1; else return pos of p
    str_constexpr_ int Find(std_view v, size_t pos = 0) const tm_noexcept
    {
        size_type n = this->find(v.data(), pos, v.length());
        return n == this->npos ? -1 : static_cast<int>(n);
    }

    str_constexpr_ int Find(value_type ch, size_t pos = 0) const tm_noexcept
    {
        size_type n = this->find(ch, pos);
        return n == this->npos ? -1 : static_cast<int>(n);
    }

    int FindI(std_view v, size_t pos = 0) const tm_noexcept
    {
        if (!v.length())
            return pos;
        size_t s = StrSplitStrI(this->data(), this->length(), pos, v.data(), v.length());
        return (pos - s != v.length()) ? -1 : s;
    }

    int FindI(value_type ch, size_t pos = 0) const tm_noexcept
    {
        size_t s = StrSplitChrI(this->data(), this->length(), pos, ch);
        return (pos - s != 1) ? -1 : s;
    }

    template <class PFN>
    int FindF(PFN __pfn, size_t pos = 0) const tm_noexcept
    {
        size_t len = this->length();
        DbgAssert(pos >= 0 && pos <= len);
        size_t i = pos;
        for (; i < len; ++i)
        {
            if (__pfn(this->data()[i]))
                return i;
        }
        return -1;
    }

    // find: return 0 - length-1; not find:return -1
    int FindOneOf(const_pointer pSet, size_t pos = 0) const tm_noexcept
    {
        return (int)base_class::find_first_of(pSet, pos);
    }

    // match with wildcard
    TMBool Match(const_pointer pKey, value_type cSplit, TMBool bMatchCase) const tm_noexcept
    {
        if (!IsEmpty() && pKey && pKey[0])
            return StrMatch(this->data(), pKey, cSplit, bMatchCase);
        return false;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Split.
    //   return 0 means end split
    //
    // CString str; // or CXmlTag str;
    // for (size_t nStart = 0; nStart = strFile.SplitLine(nStart, str);) { printf(str); }
    // "aaa\r\nbbb" -> "aaa" + "bbb"
    template <class R>
    size_t SplitLine(size_t pos, R& ret) const tm_noexcept
    {
        size_t head = pos;
        size_t tail = StrSplitLine(this->data(), this->length(), pos);
        if (pos)
            ret = __make_view(this->data() + head, tail - head);
        return pos;
    }

    //L: CVector<CString>; CSet<CString>;
    template <class L, class C>
    void SplitTo(L& aList, const C& ch) const tm_noexcept
    {
        class_type str;
        for (size_t i = 0; 0 != (i = Split(i, ch, str));)
            aList.insert(str);
    }

    // return 0 means end split
    // sample:
    // CString str; // or CXmlTag str;
    // for (size_t nStart = 0; nStart = strFile.Split(nStart, '\r', str);)
    // { print(str); }
    template <class R>
    size_t Split(size_t pos, std_view v, R& ret) const tm_noexcept
    {
        DbgAssert(v.length() > 0);
        size_t head = pos;
        size_t tail = StrSplitStr(this->data(), this->length(), pos, v.data(), v.length());
        if (pos)
            ret = __make_view(this->data() + head, tail - head);
        return pos;
    }

    template <class R>
    size_t Split(size_t pos, value_type ch, R& ret) const tm_noexcept
    {
        size_t head = pos;
        size_t tail = StrSplitChr(this->data(), this->length(), pos, ch);
        if (pos)
            ret = __make_view(this->data() + head, tail - head);
        return pos;
    }

    template <class R>
    size_t SplitI(size_t pos, std_view p, R& ret) const tm_noexcept
    {
        DbgAssert(p.size() > 0);
        size_t head = pos;
        size_t tail = StrSplitStrI(this->data(), this->length(), pos, p.data(), p.size());
        if (pos)
            ret = __make_view(this->data() + head, tail - head);
        return pos;
    }

    template <class R>
    size_t SplitI(size_t pos, value_type ch, R& ret) const tm_noexcept
    {
        size_t head = pos;
        size_t tail = StrSplitChrI(this->data(), this->length(), pos, ch);
        if (pos)
            ret = __make_view(this->data() + head, tail - head);
        return pos;
    }

    template <class R>
    size_t SplitOneOf(size_t pos, std_view p, R& ret) const tm_noexcept
    {
        DbgAssert(p.size() > 0);
        size_t head = pos;
        size_t tail = StrSplitOne(this->data(), this->length(), pos, p.data(), p.length());
        if (pos)
            ret = __make_view(this->data() + head, tail - head);
        return pos;
    }

    // F: PfnSpliteA, PfnSpliteW
    template <class R, class F>
    size_t SplitIf(size_t pos, F f, R& ret) const tm_noexcept
    {
        size_t head = pos;
        size_t tail = StrSplitIf(this->data(), this->length(), pos, f);
        if (pos)
            ret = __make_view(this->data() + head, tail - head);
        return pos;
    }

    str_constexpr_ view_type Mid(size_t pos, size_t count = base_class::npos) const tm_noexcept
    {
        TMRangeT<size_t> rg = {0, this->length()};
        RangeMid(rg, pos, count);
        return __make_view(this->data() + rg.m_data, rg.m_size);
    }

    str_constexpr_ view_type Left(size_t count) const tm_noexcept
    {
        TMRangeT<size_t> rg = {0, this->length()};
        RangeLeft(rg, count);
        return __make_view(this->data() + rg.m_data, rg.m_size);
    }

    str_constexpr_ view_type Right(size_t count) const tm_noexcept
    {
        TMRangeT<size_t> rg = {0, this->length()};
        RangeRight(rg, count);
        return __make_view(this->data() + rg.m_data, rg.m_size);
    }

    str_constexpr_ view_type substr(size_t pos, size_t count = base_class::npos) const tm_noexcept
    {
        return Mid(pos, count);
    }

    str_constexpr_ view_type SpanIncluding(const_pointer lpSet) const tm_noexcept   // strspn equivalent
    {
        DbgAssert(lpSet);
        size_type n = find_first_not_of(lpSet);
        return Left(n == this->npos ? 0 : n);
        //         _cstrspn(c_str(), lpSet));
    }

    str_constexpr_ view_type SpanExcluding(const_pointer lpSet) const tm_noexcept   // strcspn equivalent
    {
        DbgAssert(lpSet);
        size_type n = find_first_of(lpSet);
        return Left(n == this->npos ? 0 : n);
    }


    str_constexpr_ view_type TrimLeft() const tm_noexcept
    {
        const_pointer _b = this->data();
        const_pointer _e = _b + this->length();
        for (; _b != _e && StrIsSpace(*_b); ++_b)
        {
        }
        return __make_view(_b, _e);
    }

    str_constexpr_ view_type TrimLeft(value_type chTarget) const tm_noexcept
    {
        size_type n = this->find_first_not_of(chTarget);
        return Mid(n == this->npos ? 0 : n);
    }

    str_constexpr_ view_type TrimLeft(const_pointer pFilter) const tm_noexcept
    {
        size_type n = this->find_first_not_of(pFilter);
        return Mid(n == this->npos ? 0 : n);
    }

    str_constexpr_ view_type TrimRight() const tm_noexcept
    {
        const_pointer _b = this->data();
        const_pointer _e = _b + this->length();
        for (; _e != _b && StrIsSpace(_e[-1]); --_e)
        {
        }
        return __make_view(_b, _e);
    }

	str_constexpr_ view_type TrimRight(value_type chTarget) const tm_noexcept
    {
        size_type n = find_last_not_of(chTarget);
        return Left(n + 1);
    }

    str_constexpr_ view_type TrimRight(const_pointer pFilter) const tm_noexcept
    {
        size_type n = find_last_not_of(pFilter);
        return Left(n + 1);
    }

    str_constexpr_ view_type Trim() const tm_noexcept
    {
        return TrimLeft().TrimRight();
    }

    str_constexpr_ view_type Trim(const_pointer pFilter) const tm_noexcept
    {
        return TrimLeft(pFilter).TrimRight(pFilter);
    }

    std_view ToView() const tm_noexcept { return std_view(this->data(), this->length()); }

    // stop by char or end of string
    template <class V>
    view_type ScanByChar(value_type chStop, V& ret) const tm_noexcept
    {
        const_pointer _b = this->data();
        const_pointer _e = _b + this->length();
        const_pointer _c = std::find(_b, _e, chStop);
        ret = view_type(__make_view(_b, _c));
        if (_c == _e)
            return view_type();
        else
            return __make_view(_c + 1, _e);
    }

    view_type ScanByCharC(value_type chStop, CharT* ret) const tm_noexcept
    {
        const_pointer _b = this->data();
        const_pointer _e = _b + this->length();
        const_pointer _c = std::find(_b, _e, chStop);
        __make_view(_b, _c).CopyTo(ret);
        if (_c == _e)
            return view_type();
        else
            return __make_view(_c + 1, _e);
    }

    template <class V> // CharT is long, int, char, ...
    view_type ScanLong(V& nRet, int nBase = 0) const tm_noexcept
    {
        long x = nRet;
        size_t pos = StrScanLong(this->data(), this->length(), x, nBase);
        nRet = x;
        return __make_view(this->data() + pos, this->length() - pos);
    }

    view_type ScanDouble(double& dRet) const tm_noexcept
    {
        size_t n = StrScanDouble(this->data(), this->length(), dRet);
        return __make_view(this->data() + n, this->length() - n);
    }

    // return dft if pointer is invalid, otherwise return content
    long ToLong(long dft = 0, int base = 0) const tm_noexcept
    {
        StrScanLong(this->data(), this->length(), dft, base);
        return dft;
    }

    TMULong ToULong(long dft = 0, int base = 10) const tm_noexcept
    {
        return ToLong(dft, base);
    }

    double ToDouble(double dft = 0.0) const tm_noexcept
    {
        StrScanDouble(this->data(), this->length(), dft);
        return dft;
    }

    size_t CopyTo(TMStrA p, int nDstSize = -1) const
    {
        return StrAssign(p, nDstSize, this->data(), this->length());
    }

    size_t CopyTo(TMStrW p, int nDstSize = -1) const
    {
        return StrAssign(p, nDstSize, this->data(), this->length());
    }

    size_t CopyTo(TMStrA first, TMStrA last) const
    {
        return CopyTo(first, last - first);
    }

//     size_t CopyTo(TMStrW first, TMStrW last) const
//     {
//         return CopyTo(first, last - first);
//     }

    template <class V> void CopyTo(std::basic_string<V>& str) const
    {
        CStringT<V, _cstring_mode_std<V> >(*this).swap(str);
    }

    template <class V> void CopyTo(CStringT<V, _cstring_mode_strong<V> >& str) const
    {
        str = *this;
    }

#ifdef TM_WIN
    void CopyToBSTR(BSTR* bstrText) const
    {
        DbgAssert(!*bstrText);
        *bstrText = Str2BSTR(this->data(), this->length());
    }
#endif

    void MoveTo(base_class& str)
    {
        this->swap(str);
    }

    void MoveTo(CStringT& str)
    {
        str.swap(*this);
    }

    str_constexpr_ view_type SubPair(wchar_t left, wchar_t right) const tm_noexcept
    {
        const_pointer pb = this->data();
        const_pointer pe = pb + this->length();
        int nLevel = 0;
        bool bInString = false;
        view_type ret(__make_view(pb, pb));
        for (; pb != pe; ++pb)
        {
            if (pb[0] == '\"')
                bInString = !bInString;
            if (bInString)
                continue;
            if (pb[0] == left)
            {
                if (nLevel == 0)
                    ret = __make_view(pb + 1, pe); // .assign(pb + 1, 0);
                ++nLevel;
            }
            else if (pb[0] == right)
            {
                if (nLevel > 0)
                {
                    --nLevel;
                    if (nLevel == 0)
                    {
                        ret = __make_view(ret.data(), pb);
                        break;
                    }
                }
            }
        }
        return ret;
    }

    tm_constexpr20 view_type Skip(wchar_t _ch, size_type _pos = 0) const tm_noexcept
    {
        constexpr_assert(_pos >= 0);
        constexpr_assert(_pos <= this->length());
        const_pointer pb = this->data();
        const_pointer pe = pb + this->length();
        const_pointer p = std::find(pb + _pos, pe, _ch);
        return __make_view(p == pe ? p : p + 1, pe);
    }

    tm_constexpr20 view_type Skip(const std_view& _rhs, size_type _pos = 0) const tm_noexcept
    {
        constexpr_assert(_pos >= 0);
        constexpr_assert(_pos <= this->length());
        const_pointer pb = this->data();
        const_pointer pe = pb + this->length();
        const_pointer pc = std::search(pb + _pos, pe, _rhs.begin(), _rhs.end());
        return __make_view(pc == pe ? pc : pc + 1, pe);
    }
};

TM_END_NAMESPACE



#undef CharT
#undef _CSTRING_MODE
#undef _CSTRING_MODE_NOW
#undef str_constexpr_



