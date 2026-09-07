/*
 *  @file
 *  @brief  std::string_view for vc6
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2009.02.24
 *
 *  Copyright (C) 2009 miragekiller
 */

#pragma once

#ifndef TMSTRVIEW_H
#define TMSTRVIEW_H

#include "tmtchar.h"
#include "tmrange.h"
#include "tmiterator.h"
#include <string>


#if TM_CPP >= TM_CPP11 && TM_CPP <= TM_CPP14
#   define constexpr_assert(x)
#else
#   define constexpr_assert(x) DbgAssert(x)
#endif

#if TM_CPP < TM_CPP17
TM_BGN_NAMESPACE_X(std)
    template<class T, class _tr, class _a> tm_constexpr11 inline const T* data(const basic_string<T, _tr, _a>& c) { return c.data(); }
    template<class T, class _tr, class _a> tm_constexpr11 inline size_t size(const basic_string<T, _tr, _a>& c) { return c.size(); }
#if !defined(TM_MSC) || TM_MSC < 1800 // swap(basic_string) need c++17, but MSC always have.
    template <class CharT, class _Tr, class _A> tm_constexpr20 void swap(std::basic_string<CharT, _Tr, _A>& l, std::basic_string<CharT, _Tr, _A>& r, ...) { r.swap(l); }
#endif
TM_END_NAMESPACE_X(std)
#endif // TM_CPP < TM_CPP17

#if TM_CPP >= TM_CPP17

#   include <string_view>
#   define _tm_string_view basic_string_view

#else

TM_BGN_NAMESPACE_X(std)

///////////////////////////////////////////////////////////////////////////////
// basic_string_npos
template <class SizeT>
struct basic_string_npos
{
#if TM_CPP >= TM_CPP11
    static constexpr const SizeT npos = SizeT(-1);
};
#else
static const SizeT npos;
};
template <class SizeT>
const SizeT basic_string_npos<SizeT>::npos = SizeT(-1);
#endif // TM_CPP >= TM_CPP11


///////////////////////////////////////////////////////////////////////////////
// _tm_string_view
template<class CharT, class _Traits = char_traits<CharT> >
class _tm_string_view : public basic_string_npos<size_t>
{
public:
    typedef  _Traits                                traits_type;
    typedef CharT                                   value_type;
    typedef const CharT*                            pointer;
    typedef const CharT*                            const_pointer;
    typedef CharT&                                  reference;
    typedef const CharT&                            const_reference;
    typedef const CharT*                            const_iterator;
    typedef const_iterator                          iterator;
    typedef TM::r_iterT<
        const CharT*, const CharT&,
        std::random_access_iterator_tag>            const_reverse_iterator;
    typedef const_reverse_iterator                  reverse_iterator;
    typedef size_t                                  size_type;
    typedef ptrdiff_t                               difference_type;

private:
    const_pointer m_data;
    size_type m_size;

protected:
    tm_constexpr11 int _StrLen(const CharT* p) const { constexpr_assert(p); return _Traits::length(p); }

public:
    // Constructors and assignment
    tm_constexpr11 _tm_string_view() tm_noexcept : m_data(), m_size(0) {}
    tm_constexpr11 _tm_string_view(const _tm_string_view& rhs) tm_noexcept : m_data(rhs.m_data), m_size(rhs.m_size) {}
    tm_constexpr11 _tm_string_view(const CharT* s) tm_noexcept : m_data(s), m_size(_StrLen(s)) {}
    tm_constexpr11 _tm_string_view(const CharT* s, size_type count) tm_noexcept : m_data(s), m_size(count) { constexpr_assert(count == 0 || 0 != s); }
    tm_constexpr11 _tm_string_view(const basic_string<CharT, _Traits>& rhs) tm_noexcept : m_data(rhs.data()), m_size(rhs.length()) {}
    tm_tpl_cexpr11 _tm_string_view(P f, P l) tm_noexcept : m_data(&*f), m_size(l - f) { constexpr_assert(f == l || 0 != f); }
    // tm_constexpr11 _tm_string_view& operator=(const _tm_string_view&) tm_noexcept = default;

    // operator; don't use default string_view::operator <,<=,>=,>
    tm_constexpr11 bool operator !=(_tm_string_view rhs) const tm_noexcept { return !(*this == rhs); }
    tm_constexpr11 bool operator !=(const basic_string<CharT>& rhs) const tm_noexcept { return !(*this == rhs); }
    tm_constexpr11 bool operator == (_tm_string_view rhs) const tm_noexcept
    {
        return length() == rhs.length() && _Traits::compare(data(), rhs.data(), length()) == 0;
    }
    tm_constexpr11 bool operator == (const basic_string<CharT>& rhs) const tm_noexcept { return operator ==(_tm_string_view(rhs)); }

    // Iterators
    tm_constexpr11 const_iterator begin() const tm_noexcept { return m_data; }
    tm_constexpr11 const_iterator end() const tm_noexcept { return m_data + m_size; }
    tm_constexpr11 const_iterator cbegin() const tm_noexcept { return begin(); }
    tm_constexpr11 const_iterator cend() const tm_noexcept { return end(); }
    tm_constexpr11 const_reverse_iterator rbegin() const tm_noexcept { return const_reverse_iterator(end()); }
    tm_constexpr11 const_reverse_iterator rend() const tm_noexcept { return const_reverse_iterator(begin()); }
    tm_constexpr11 const_reverse_iterator crbegin() const tm_noexcept { return rbegin(); }
    tm_constexpr11 const_reverse_iterator crend() const tm_noexcept { return rend(); }

    // Capacity
    tm_constexpr11 size_type size() const tm_noexcept { return m_size; }
    tm_constexpr11 size_type length() const tm_noexcept { return m_size; }
    tm_constexpr11 bool empty() const tm_noexcept { return m_size == 0; }
    tm_constexpr11 size_type max_size() const tm_noexcept { return tm_min(static_cast<size_t>(TM_UINT_MAX), static_cast<size_t>(-1) / sizeof(CharT)); }

    // Element access
    tm_constexpr11 const_reference operator[](size_type pos) const tm_noexcept { return at(pos); }
    tm_constexpr11 const_reference at(size_type pos) const { constexpr_assert(pos <= m_size); return m_data[pos]; }
    tm_constexpr11 const_reference front() const tm_noexcept { constexpr_assert(size()); return m_data[0]; }
    tm_constexpr11 const_reference back() const tm_noexcept { constexpr_assert(size()); return m_data[m_size - 1]; }
    tm_constexpr11 const_pointer data() const tm_noexcept { return m_data; }

    // Modifiers
    tm_constexpr11 void remove_prefix(size_type count) tm_noexcept { constexpr_assert(m_size >= count); m_data = m_data + count; m_size = m_size - count; }
    tm_constexpr11 void remove_suffix(size_type count) tm_noexcept { constexpr_assert(m_size >= count); m_size = m_size - count; }
    tm_constexpr11 void swap(_tm_string_view& _Other) tm_noexcept { tm_swap(*this, _Other); }

    // Operations
    tm_constexpr11 size_type copy(CharT* dest, size_type count, size_type pos = 0) const
    {
        constexpr_assert(m_size >= pos);
        const size_type real = tm_min(m_size - pos, count);
        _Traits::copy(dest, m_data + pos, real);
        return real;
    }

    tm_constexpr11 _tm_string_view substr(size_type pos = 0, size_type count = npos) const
    {
        constexpr_assert(m_size >= pos);
        return _tm_string_view(m_data + pos, tm_min(m_size - pos, count));
    }

    tm_constexpr11 bool starts_with(_tm_string_view sv) const tm_noexcept
    {
        return (m_size >= sv.m_size && _Traits::compare(m_data, sv.m_data, sv.m_size) == 0);
    }

    // compare
    tm_constexpr14 int compare(_tm_string_view v) const tm_noexcept
    {
        const int _Ans = _Traits::compare(data(), v.data(), tm_min(length(), v.length()));
        if (_Ans != 0)
            return _Ans;
        if (length() < v.length())
            return -1;
        if (length() > v.length())
            return 1;
        return 0;
    }

    tm_constexpr14 int compare(size_type pos, size_type count1, _tm_string_view v) const
    {
        return substr(pos, count1).compare(v);
    }

    tm_constexpr14 int compare(size_type pos, size_type count1, _tm_string_view v, size_type pos2, size_type count2) const
    {
        return substr(pos, count1).compare(v.substr(pos2, count2));
    }

    tm_constexpr14 int compare(const CharT* s) const
    {
        return compare(_tm_string_view(s));
    }

    tm_constexpr14 int compare(size_type pos, size_type count1, const CharT* s) const
    {
        return substr(pos, count1).compare(_tm_string_view(s));
    }

    tm_constexpr14 int compare(size_type pos, size_type count1, const CharT* s, size_type count2) const
    {
        return substr(pos, count1).compare(_tm_string_view(s, count2));
    }

    // find
    tm_constexpr11 size_type find(_tm_string_view v, size_type pos = 0) const tm_noexcept
    {
        return find(v.data(), pos, v.length());
    }

    tm_constexpr14 size_type find(CharT ch, size_type pos = 0) const tm_noexcept
    {
        if (pos < length())
        {
            const CharT * const _Found_at = _Traits::find(data() + pos, length() - pos, ch);
            if (_Found_at)
                return static_cast<size_type>(_Found_at - data());
        }
        return npos;
    }

    tm_constexpr14 size_type find(const CharT* s, size_type pos, size_type count) const tm_noexcept
    {
        if (count > m_size || pos > m_size - count)
            return this->npos;

        if (count == 0)
            return pos;

        const CharT * _Possible_matches_end = m_data + (m_size - count) + 1;
        for (const CharT * _Match_try = m_data + pos; ; ++_Match_try)
        {
            _Match_try = _Traits::find(_Match_try, static_cast<size_t>(_Possible_matches_end - _Match_try), *s);
            if (!_Match_try)
                return this->npos;

            if (_Traits::compare(_Match_try, s, count) == 0)
                return static_cast<size_t>(_Match_try - m_data);
        }
    }

    tm_constexpr11 size_type find(const CharT* s, size_type pos = 0) const tm_noexcept
    {
        return find(s, pos, _StrLen(s));
    }

    // rfind
    tm_constexpr14 size_type rfind(_tm_string_view v, size_type pos = npos) const tm_noexcept
    {
        return rfind(v.data(), pos, v.length());
    }

    tm_constexpr14 size_type rfind(CharT ch, size_type pos = npos) const tm_noexcept
    {
        if (m_size != 0)
        {
            for (const CharT * _Match_try = m_data + tm_min(pos, m_size - 1); ; --_Match_try)
            {
                if (_Traits::eq(*_Match_try, ch))
                    return (static_cast<size_t>(_Match_try - m_data));    // found a match
                if (_Match_try == m_data)
                    break;    // at beginning, no more chance for match
            }
        }

        return this->npos;
    }

    tm_constexpr14 size_type rfind(const CharT* s, size_type pos, size_type count) const tm_noexcept
    {
        if (count == 0)
            return tm_min(pos, length());    // empty string always matches

        if (count <= length())
        {
            for (const CharT * _Match_try = m_data + tm_min(pos, length() - count); ; --_Match_try)
            {
                if (_Traits::eq(*_Match_try, *s) && _Traits::compare(_Match_try, s, count) == 0)
                    return static_cast<size_t>(_Match_try - m_data);
                if (_Match_try == m_data)
                    break;
            }
        }

        return this->npos;
    }

    tm_constexpr11 size_type rfind(const CharT* s, size_type pos = npos) const tm_noexcept
    {
        return rfind(s, pos, _StrLen(s));
    }

    // find first one of
    tm_constexpr11 size_type find_first_of(_tm_string_view v, size_type pos = 0) const tm_noexcept
    {
        return find_first_of(v.data(), pos, v.length());
    }

    tm_constexpr11 size_type find_first_of(CharT ch, size_type pos = 0) const tm_noexcept
    {
        return find(ch, pos);
    }

    tm_constexpr14 size_type find_first_of(const CharT* s, size_type pos, size_type count) const tm_noexcept
    {
        constexpr_assert(pos >= 0);
        constexpr_assert(pos <= length());
        const_pointer b = data();
        const_pointer e = b + length();
        const_pointer p = std::find_first_of(b + pos, e, s, s + count);
        if (p == e)
            return this->npos;
        return static_cast<size_t>(p - b);
    }

    tm_constexpr11 size_type find_first_of(const CharT* s, size_type pos = 0) const tm_noexcept
    {
        return find_first_of(s, pos, _StrLen(s));
    }

    // find last one of
    tm_constexpr11 size_type find_last_of(_tm_string_view v, size_type pos = npos) const tm_noexcept
    {
        return find_last_of(v.data(), pos, v.length());
    }

    tm_constexpr11 size_type find_last_of(CharT ch, size_type pos = npos) const tm_noexcept
    {
        return rfind(ch, pos);
    }

    tm_constexpr14 size_type find_last_of(const CharT* s, size_type pos, size_type count) const tm_noexcept
    {
        size_type len = this->length();
        if (len && count)
        {
            if (--len > pos)
                len = pos;
            do
            {
                if (traits_type::find(s, count, this->data()[len]))
                    return len;
            }
            while (len-- != 0);
        }
        return npos;
    }

    tm_constexpr11 size_type find_last_of(const CharT* s, size_type pos = npos) const tm_noexcept
    {
        return find_last_of(s, pos, _StrLen(s));
    }

    // find_first_not_of
    tm_constexpr14 size_type find_first_not_of(_tm_string_view v, size_type pos = 0) const tm_noexcept
    {
        return find_first_not_of(v.data(), pos, v.length());
    }

    tm_constexpr14 size_type find_first_not_of(CharT ch, size_type pos = 0) const tm_noexcept
    {
        if (pos < m_size)
        {
            const CharT * _End = m_data + m_size;
            for (const CharT * _Match_try = m_data + pos; _Match_try < _End; ++_Match_try)
            {
                if (!_Traits::eq(*_Match_try, ch))
                    return (static_cast<size_t>(_Match_try - m_data));    // found a match
            }
        }
        return this->npos;    // no match
    }

    tm_constexpr14 size_type find_first_not_of(const CharT* s, size_type pos, size_type count) const tm_noexcept
    {
        for (size_type n = pos; n < length(); ++n)
        {
            if (std::find(s, s + count, data()[n]) == s + count)
                return n;
        }
        return this->npos;
    }

    tm_constexpr11 size_type find_first_not_of(const CharT* s, size_type pos = 0) const tm_noexcept
    {
        return find_first_not_of(s, pos, _StrLen(s));
    }

    // find_last_not_of
    tm_constexpr11 size_type find_last_not_of(_tm_string_view v, size_type pos = npos) const tm_noexcept
    {
        return find_last_not_of(v.data(), pos, v.length());
    }

    tm_constexpr14 size_type find_last_not_of(CharT ch, size_type pos = npos) const tm_noexcept
    {
        if (m_size != 0)
        {
            for (const CharT * _Match_try = m_data + tm_min(pos, m_size - 1); ; --_Match_try)
            {
                if (!_Traits::eq(*_Match_try, ch))
                    return (static_cast<size_t>(_Match_try - m_data));    // found a match
                if (_Match_try == m_data)
                    break;
            }
        }
        return this->npos;
    }

    tm_constexpr14 size_type find_last_not_of(const CharT* s, size_type pos, size_type count) const tm_noexcept
    {
        size_type len = length();
        if (len)
        {
            if (--len > pos)
                len = pos;
            do
            {
                if (!traits_type::find(s, count, data()[len]))
                    return len;
            }
            while (len--);
        }
        return npos;
    }

    tm_constexpr11 size_type find_last_not_of(const CharT* s, size_type pos = npos) const tm_noexcept
    {
        return find_last_not_of(s, pos, _StrLen(s));
    }
};

typedef _tm_string_view<char>       string_view;
typedef _tm_string_view<wchar_t>    wstring_view;
typedef _tm_string_view<TMChar8>    u8string_view;
typedef _tm_string_view<TM_CHAR16>  u16string_view;
typedef _tm_string_view<TM_CHAR32>  u32string_view;


template<class T, class _tr> tm_constexpr11 inline const T* data(const _tm_string_view<T, _tr>& c) { return c.data(); }
template<class T, class _tr> tm_constexpr11 inline size_t size(const _tm_string_view<T, _tr>& c) { return c.size(); }


TM_END_NAMESPACE_X(std)

#endif // #if TM_CPP < TM_CPP17


///////////////////////////////////////////////////////////////////////////////
// StrMakeView
TM_BGN_NAMESPACE

template<class CharT>
tm_constexpr11 inline std::_tm_string_view<CharT> StrMakeView(const CharT* p)
{
    return p ? std::_tm_string_view<CharT>(p) : std::_tm_string_view<CharT>();
}

template<class CharT>
tm_constexpr11 inline std::_tm_string_view<CharT> StrMakeView(const CharT* p, size_t s)
{
    return std::_tm_string_view<CharT>(p, s);
}

template<class CharT>
tm_constexpr11 inline std::_tm_string_view<CharT> StrMakeView(const CharT* p1, const CharT* p2)
{
    return std::_tm_string_view<CharT>(p1, p2 - p1);
}



TM_END_NAMESPACE


///////////////////////////////////////////////////////////////////////////////
//string_constant
template <class T> struct string_constant : public std::_tm_string_view<T>
{
    typedef std::_tm_string_view<T> base_class;
    explicit string_constant(const T* p, size_t n) : base_class(p, n) {}
};

#if defined(TM_MSC) && TM_MSC < 1300

    template <class T>
    tm_constexpr11 string_constant<T> __mk_string_constant(const T* p, int n){ return string_constant<T>(p, n - 1); }
    #define StrMakeConstant(x) __mk_string_constant(x, tm_countof(x))

#else

    template <class T, int n>
    tm_constexpr11 string_constant<T> StrMakeConstant(const T(&p)[n]) { return string_constant<T>(p, n - 1); }

#endif // TM_MSC


#endif // end of TMSTRVIEW_H

