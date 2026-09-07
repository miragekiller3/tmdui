/**
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2005.11.11
 *
 *  Copyright (C) 2005 miragekiller
 */

#pragma once

#ifndef TMITERATOR_H
#define TMITERATOR_H

#include <algorithm>
#include <numeric>
#include "tmtypeinfo.h"


TM_BGN_NAMESPACE

template <class _Val>
struct _p2v_default
{
    template <class _Ptr> static _Val done(_Ptr p) { return *p; }
};

// template <class T, class _reference, class _pointer, class _P2V = _p2v_default<_reference> >
// class p_iterT
// {
// private:
//     T m_iter;
// 
//     inline void _inc() { ++m_iter; }
//     inline void _inc(int n) { m_iter -= n; }
//     inline void _dec() { ++m_iter; }
//     inline void _dec(int n) { m_iter += n; }
//     inline TMBool _chk(const r_iterT& rhs) const { return m_iter == rhs.m_iter; }
// 
// public:
//     p_iterT() {}
//     p_iterT(T iter) : m_iter(iter) {}
//     template <class _T, class _R, class _P, class _P2V> p_iterT(const p_iterT<_T,>& rhs) : m_iter (rhs.m_iter) {}
//     _reference operator * () { return *(m_iter - 1); }
//     _reference operator [] (int n) { return *(m_iter - (1 + n)); }
//     _iterator base() const { return m_iter; }
//     
//     typedef std::random_access_iterator_tag iterator_category;
//     TM_DECLARE_CLASS_OPERATOR_EQU(r_iterT);
//     TM_DECLARE_CLASS_OPERATOR_INC(r_iterT);
//     TM_DECLARE_CLASS_OPERATOR_DEC(r_iterT);
// 
// };


template<class _iterator, class _reference, class _category = std::random_access_iterator_tag>
class r_iterT
{
private:
    _iterator   m_iter;

    inline void _inc()      { --m_iter; }
    inline void _inc(int n) { m_iter -= n; }
    inline void _dec()      { ++m_iter; }
    inline void _dec(int n) { m_iter += n; }
    inline TMBool _chk (const r_iterT& rhs) const { return m_iter == rhs.m_iter; }

public:
    r_iterT() {}
    r_iterT(_iterator iter)         : m_iter(iter) {}
    r_iterT(const r_iterT& rhs)     : m_iter(rhs.m_iter) {}
    _reference operator * ()        { return *(m_iter - 1); }
    _reference operator [] (int n)  { return *(m_iter - (1 + n)); }
    _iterator base()                const { return m_iter; }

    typedef _category iterator_category;
    TM_DECLARE_CLASS_OPERATOR_EQU(r_iterT);
    TM_DECLARE_CLASS_OPERATOR_INC(r_iterT);
    TM_DECLARE_CLASS_OPERATOR_DEC(r_iterT);
};

template<TMBool bForward, class _iterator, class _reference, class _pointer, class _P2V = _p2v_default<_reference>, class _category = std::random_access_iterator_tag>
class CIteratorImpl
{
private:
    _iterator   m_iter;
    
    inline void _inc()      { if (bForward) ++m_iter; else --m_iter; }
    inline void _inc(int n) { if (bForward) m_iter += n; else m_iter -= n; }
    inline void _dec()      { if (bForward) --m_iter; else ++m_iter; }
    inline void _dec(int n) { if (bForward) m_iter -= n; else m_iter += n; }
    inline TMBool _chk(const CIteratorImpl& rhs) const { return m_iter == rhs.m_iter; }
    
public:
    CIteratorImpl()                 {}
    CIteratorImpl(_iterator iter)   : m_iter(iter) {}
    template<TMBool _BForward, class _Iter, class _Ref, class _Ptr, class _P2VAdapt, class _Cat> CIteratorImpl(const CIteratorImpl<_BForward, _Iter, _Ref, _Ptr, _P2VAdapt, _Cat>& rhs) : m_iter(rhs.base()) {}
    _reference operator*()          { return _P2V::done(bForward ? m_iter : m_iter - 1); }
    _reference operator[](int n)    { CIteratorImpl iter(*this); iter += n; return *iter; }
    _pointer operator->()           { return &(**this); }
    _iterator base()                const { return m_iter; }
    operator _iterator()            const { return m_iter; }
    
    typedef _category iterator_category;
    TM_DECLARE_CLASS_OPERATOR_EQU(CIteratorImpl);
    TM_DECLARE_CLASS_OPERATOR_INC(CIteratorImpl);
    TM_DECLARE_CLASS_OPERATOR_DEC(CIteratorImpl);
};


template <class T> class CIter
{
public:
    typedef T                                       iterator;
    typedef CIter                                   class_type;
    typedef size_t                                  size_type;
    typedef ptrdiff_t                               difference_type;

protected:
    iterator    m_first;
    iterator    m_last;

public:
    template <class _List>
    CIter           (const _List& __list) 
        : m_first   (__list.begin())
        , m_last    (__list.end())
    {
    }

    CIter           (std::pair<iterator, iterator> __pair)
        : m_first   (__pair.first)
        , m_last    (__pair.second)
    {
    }

    CIter           (iterator first, iterator last)
        : m_first   (first)
        , m_last    (last)
    {
    }

    CIter           (const CIter& rhs)
        : m_first   (rhs.m_first)
        , m_last    (rhs.m_last)
    {
    }

    iterator GetFirst () const { return m_first; }
    iterator GetLast () const { return m_last; }

    operator TMBool () const
    {
        return m_first != m_last;
    }

    TMBool operator ! () const
    {
        return m_first == m_last;
    }

    CIter& operator ++ () { ++m_first; return *this; }
    //CIter& operator -- () { --m_first; return *this; }

    class_type& operator += (size_type n)
    {
        m_first += n;
        return *this;
    }

    class_type& operator = (iterator iter)
    {
        m_first = iter;
        return *this;
    }

    class_type& operator = (const class_type& iter)
    {
        m_first = iter.GetFirst ();
        m_last = iter.GetLast ();
        return *this;
    }

    difference_type distance () const
    {
        return std::distance (m_first, m_last);
    }

    iterator operator -> () const { return m_first; }

    iterator operator * () const { return m_first; }

    template < class T_list >
        class_type& erase (T_list& __lst)
    {
        m_first = __lst.erase (m_first);
    }

    // return _v + 0 + 1 + 2 ... + end
    template <class _V> 
    _V accumulate (_V __v) const
    {
        return std::accumulate (m_first, m_last, __v);
    }

    // return _f(_f(_f(_v, 0), 1)... end);
    template <class _V, class _F> 
        _V accumulate (_V __v, _F __f) const
    {
        return std::accumulate (m_first, m_last, __v, __f);
    }

    // fill difference to a new iterator
    // new0 = 1 - 0; new1 = 2 - 1; new3 = 3 - 2; ...
    template <class _Iter>
    CIter<_Iter> adjacent_difference (_Iter __iter) const
    {
        _Iter it = std::accumulate (m_first, m_last, __iter);
        return CIter<_Iter> (__iter, it);
    }

    // fill difference to another iterator
    // new0 = __f(1 - 0); new1 = __f(2 - 1); new3 = __f(3 - 2); ...
    template <class _Iter, class _F>
    CIter<_Iter> adjacent_difference (_Iter __iter, _F __f) const
    {
        _Iter it = std::accumulate (m_first, m_last, __iter, __f);
        return CIter<_Iter> (__iter, it);
    }

    // if (n - 1 == n) return n - 1; else return end
    CIter adjacent_find () const
    {
        iterator it = std::adjacent_find (m_first, m_last);
        return CIter (it, m_last);
    }

    // if (__f(n - 1, n)) return n - 1; else return end
    template <class _F> 
    CIter adjacent_find (_F& __f) const
    {
        iterator it = std::adjacent_find (m_first, m_last, __f);
        return CIter (it, m_last);
    }

    // if find __v return true;
    template <class _V>
    bool binary_search (const _V& __v) const
    {
        return std::binary_search (m_first, m_last, __v);
    }

    // if find __f(n, __v) != false return true;
    template <class _V, class _F>
    bool binary_search (const _V& __v, _F __f) const
    {
        return std::binary_search (m_first, m_last, __v, __f);
    }

    // copy to another iter
    // *new++ = *old++;
    template <class _Iter>
    CIter<_Iter> copy (_Iter __iter) const
    {
        _Iter it = std::copy (m_first, m_last, __iter);
        return CIter<_Iter> (__iter, it);
    }

    // copy backward to another iter
    // *new-- = *old--;
    template <class _Iter>
    CIter<_Iter> copy_backward (_Iter __iter) const
    {
        _Iter it = std::copy_backward (m_first, m_last, __iter);
        return CIter<_Iter> (it, __iter);
    }

    // return count of (n == __v);
    template <class _V>
    size_t count (const _V& __v) const
    {
        return std::count (m_first, m_last, __v);
    }

    // return count of __f(n) != false;
    template <class _F>
    size_t count_if (_F __f) const
    {
        return std::count_if (m_first, m_last, __f);
    }

    // return 1 == new1 && 2 == new2 && 3 == new3 && ... && n == newn
    template <class _Iter>
    bool equal (_Iter __iter)
    {
        return std::equal (m_first, m_last, __iter);
    }

    // return __f(1, new1) && __f(2, new2) && ... && __f(n, newn)
    template <class _Iter, class _F>
        bool equal (_Iter __iter, _F __f)
    {
        return std::equal (m_first, m_last, __iter, __f);
    }

    // return [lower_bound(first, last, __v), upper_bound(first, last, __v)];
    template <class _V>
    CIter equal_range (const _V& __v) const
    {
        return std::equal_range (m_first, m_last, __v);
    }

    // return [lower_bound(first, last, __v, __f),
    //   upper_bound(first, last, __v, __f)];
    template <class _V, class _F>
    CIter equal_range (const _V& __v, _F __f) const
    {
        return std::equal_range (m_first, m_last, __v, __f);
    }

    // set 0 = 1 = 2 = 3 = ... = end = __v;
    template <class _V>
    void fill (const _V& __v)
    {
        std::fill (m_first, m_last, __v);
    }

    // set 0 = 1 = 2 = 3 = ... = n = __v;
    template <class _SZ, class _V>
    void fill_n (_SZ __size, const _V& __v)
    {
        DbgAssert(distance() >= __size);
        std::fill_n (m_first, __size, __v);
    }

    // return the first of (__v == n);
    template <class _V>
    CIter find (const _V& __v) const
    {
        iterator it = std::find (m_first, m_last, __v);
        return CIter (it, m_last);
    }

    // return the first of (__f(n) != false);
    template <class _F>
    CIter find_if (_F __f) const
    {
        iterator it = std::find_if (m_first, m_last, __f);
        return CIter (it, m_last);
    }

    // find the end of ([n, m] == rhs.[n, m]);
    template <class _Iter>
    CIter find_end (CIter<_Iter> rhs) const
    {
        iterator it = std::find_end (m_first, m_last, rhs.m_first, rhs.m_last);
        return CIter (it, m_last);
    }

    // find the end of (_f([n, m], rhs.[n, m]) != false);
    template <class _Iter, class _F>
    CIter find_end (CIter<_Iter> rhs, _F __f) const
    {
        iterator it = std::find_end (m_first, m_last, rhs.m_first, rhs.m_last, __f);
        return CIter (it, m_last);
    }

    // return the first (n == one of rhs).
    template <class _Iter>
    CIter find_first_of (CIter<_Iter> rhs) const
    {
        iterator it = std::find_first_of (m_first, m_last, rhs.m_first, rhs.m_last);
        return CIter (it, m_last);
    }

    // return the first __f(n, one of rhs) != false;
    template <class _Iter, class _F>
    CIter find_first_of (CIter<_Iter> rhs, _F __f) const
    {
        iterator it = std::find_first_of (m_first, m_last, rhs.m_first, rhs.m_last, __f);
        return CIter (it, m_last);
    }

    // all interators execute __f(n)
    template <class _F>
    void for_each (_F __f) const
    {
        std::for_each (m_first, m_last, __f);
    }

    // use (n = __f()) fill all interators
    template <class _F>
    void generate (_F __f)
    {
        std::generate (m_first, m_last, __f);
    }

    // use (n = __f()) fill first __size interators
    template <class _SZ, class _F>
    void generate_n (_SZ __size, _F __f)
    {
        DbgAssert(distance() >= __size);
        std::generate_n (m_first, __size, __f);
    }

    template <class _Iter>
    bool includes (CIter<_Iter> rhs) const
    {
        return std::includes (m_first, m_last, rhs.m_first, rhs.m_last);
    }




    // search the iterators which make the __func(*iterator,__key) return true 
    template < class _F, class _K >
        void find (_F __func, const _K& __key)
    {
        for (; m_first != m_last; ++m_first)
        {
            if (__func (*m_first, __key))
                break ;
        }
    }

    // search the sub-sequence like sub-string 
    template < class _Iter >
        iterator search (_Iter __f, _Iter __l)
    {
        return m_first = std::search (m_first, m_last, __f, __l);
    }

    template < class _Iter, class _Func >
        iterator search (_Iter __f, _Iter __l, _Func& __func)
    {
        return m_first = std::search (m_first, m_last, __f, __l, __func);
    }

    // search the n adjacent elements  with value __x
    template < class _Tp >
        iterator search_n (size_t n, const _Tp& __x)
    {
        return m_first = std::search_n (m_first, m_last, n, __x);
    }

    // search the n adjacent  elements which make __func(*iterator) return true 
    template < class _Tp, class _Func >
        iterator search_n (size_t n, const _Tp& __x, _Func __func)
    {
        return m_first = std::search_n (m_first, m_last, n, __x, __func);
    }

    // reverse search iterator sub-sequence,return the start address of the last sub-sequence 
    template < class _Iter >
        iterator find_end (_Iter __f, _Iter __l)
    {
        return m_first = std::find_end (m_first, m_last, __f, __l);
    }

    template < class _Iter, class _Func >
        iterator find_end (_Iter __f, _Iter __l, _Func& __func)
    {
        return m_first = 
            std::find_end (m_first, m_last, __f, __l, __func);
    }
    
    // search any value in sequence like strtok()
    template < class _Iter >
        iterator find_first_of (_Iter __f, _Iter __l)
    {
        return m_first = std::find_first_of (m_first, m_last, __f, __l);
    }

    template < class _Iter, class _Func >
        iterator find_first_of (_Iter __f, _Iter __l, _Func __func)
    {
        return m_first = 
            std::find_first_of (m_first, m_last, __f, __l, __func);
    }

    // exchange the values of two sequences,return the pointer to the 2nd sequence's end 
    template < class _Iter >
        _Iter swap_ranges (_Iter __f)
    {
        return std::swap_ranges (m_first, m_last, __f);
    }

    // transform the values in sequence to the 2nd sequence __f using the specified function one by one 
    template < class _Iter, class _Func >
        _Iter transform ( _Iter __f, _Func& __func)
    {
        return std::transform (m_first, m_last, __f, __func);
    }

    // transform the current sequence values  and sequence __f1 values to the 2nd sequence __f using the specified function one by one 
    template < class _Iter1, class _Iter, class _Func >
        _Iter transform ( _Iter1 __f1,  _Iter __f, _Func& __func)
    {
        return std::transform (m_first, m_last, __f1, __f, __func);
    }

    // set all of the __x value objects to __y
    template < class _Tp >
        void replace (const _Tp& __x, const _Tp& __y)
    {
        std::replace (m_first, m_last, __x, __y);
    }

    // set all of the objects which satisfy __func to __y
    template < class _Func, class _Tp >
        void replace_if (_Func& __func, const _Tp& __x)
    {
        std::replace_if (m_first, m_last, __func, __x);
    }

    // replace_copy
    // replace_copy_if
};



TM_END_NAMESPACE


/*
///////////////////////////////////////////////////////////////////////////////
//    CIterator     
//    like iterator
//        iter_traits_forward    normal iterator
//        iter_traits_reverse    reverse iterator

namespace TM
{

    struct iter_traits_forward  // forward iterator
    {
        template < class T > 
            struct traits
        {
            typedef typename T::iterator                iterator;
            typedef typename T::const_iterator          const_iterator;
            
            inline static void begin (T& lst, iterator& iter)
            { 
                iter = lst.begin(); 
            }
            
            inline static void end (T& lst, iterator& iter)
            { 
                iter = lst.end(); 
            }
            
            template < class Iter >
                inline static TMBool bof (T& lst, Iter& iter)
            { 
                return iter == lst.begin(); 
            }
            
            template < class Iter >
                inline static TMBool eof (T& lst, Iter& iter)
            { 
                return iter == lst.end(); 
            }
            
        };
    };
    
    struct iter_traits_reverse // reverse iterator
    {
        template < class T > 
            struct traits
        {
            typedef typename T::reverse_iterator        iterator;
            typedef typename T::const_reverse_iterator  const_iterator;
            
            inline static void begin (T& lst, iterator& iter)
            { 
                iter = lst.rbegin(); 
            }
            
            inline static void end (T& lst, iterator& iter)
            { 
                item = lst.rend(); 
            }
            
            template < class Iter >
                inline static TMBool bof (T& lst, Iter& iter)
            { 
                return iter == lst.rbegin(); 
            }
            
            template < class Iter >
                inline static TMBool eof (T& lst, Iter& iter)
            { 
                return item == lst.rend(); 
            }
        };            
    };
    
    template < class _List, class Iter_traits = iter_traits_forward >
        class CIter
    {
    public:
        typedef CIter < _List, Iter_traits >            class_type;
        typedef typename iter_traits_forward::traits<_List>    traits;
        
        typedef typename traits::iterator                iterator;
        typedef typename traits::const_iterator            const_iterator;
        typedef typename _List::value_type                value_type;
        typedef typename _List::pointer                    pointer;
        typedef typename _List::const_pointer            const_pointer;
        typedef typename _List::reference                reference;
        typedef typename _List::const_reference            const_reference;
        typedef typename _PointTypeT(value_type)        always_pointer;
        
    protected:
        iterator        m_pointer;
        const_iterator    m_end;
        
    public:
        explicit CIter    (_List& __lst) 
            : m_pointer    (__lst.begin())
            , m_end        (__lst.end())
        { 
        }
        
        CIter            (_List& __lst, const iterator& __it) 
            : m_pointer    (__it)
            , m_end        (__lst.end())
        { 
        }
        
        inline CIter& operator = (const CIter& __it)
        {
            m_pointer = __it.m_pointer;
            m_end = __it.m_end;
            return *this;
        }
        
        inline CIter& operator = (const iterator& __it)
        {
            _ASSERTE (__it <= m_end); 
            m_pointer = __it;
            return *this;
        }

        operator TMBool ()              { return !eof(); }
        
        operator TMBool () const         { return !eof(); }
        
        TMBool operator ! () const    { return eof(); }
        
        operator iterator& ()         { return m_pointer; }

        TMBool eof ()    const throw ()    { return m_pointer == m_end; }

        CIter& operator ++ ()        { DbgAssert(*this); ++m_pointer; return *this; }

        CIter& operator -- ()        { --m_pointer; return *this; }

        reference operator * ()        { return value(); }

        reference value()            { DbgAssert(*this); return *m_pointer; }
        
        always_pointer pvalue ()    { return TM_P(value()); }

        pointer operator -> ()        { return & (value()); }
    };

} // end of namespace TM

#define CIter1(_List)        TM::CIter< _List, TM::iter_traits_forward >
#define CIter2(_List)        TM::CIter< _List, TM::iter_traits_reverse >

//    CIter    
///////////////////////////////////////////////////////////////////////////////
*/


///////////////////////////////////////////////////////////////////////////////
//    atom function

namespace TM
{
#   define MATH_METHOD(ret) \
        template < class _T1 , class _T2 > \
            ret operator () (const _T1& __x, const _T2& __y) const

    struct Plus         { MATH_METHOD (_T1) { return __x + __y; } };
    struct Minus        { MATH_METHOD (_T1) { return __x - __y; } };
    struct Multiplies   { MATH_METHOD (_T1) { return __x * __y; } };
    struct Divides      { MATH_METHOD (_T1) { return __x / __y; } };
    struct Modulus      { MATH_METHOD (_T1) { return __x % __y; } };
    struct EqualTo      { MATH_METHOD (TMBool) { return __x == __y; } };
    struct NotEqualTo   { MATH_METHOD (TMBool) { return __x != __y; } };
    struct Greater      { MATH_METHOD (TMBool) { return __x > __y; } };
    struct Less         { MATH_METHOD (TMBool) { return __x < __y; } };
    struct GreaterEqual { MATH_METHOD (TMBool) { return __x >= __y; } };
    struct LessEqual    { MATH_METHOD (TMBool) { return __x <= __y; } };
    struct LogicalAnd   { MATH_METHOD (TMBool) { return __x && __y; } };
    struct LogicalOr    { MATH_METHOD (TMBool) { return __x || __y; } };
    

#define FIND_OP_FULL(_Func) \
    template < class _Tp1, class _Tp2 > \
        TMBool operator () (const _Tp1& __X, const _Tp2& __Y) const \
    { \
        return _Func; \
    } 

#define FIND_OP(_Func) \
    template < class _Tp1, class _Tp2 > \
    TMBool operator () (const _Tp1& __X, const _Tp2& __Y) const \
    { \
    return ((__X) _Func) == __Y; \
    } 

#define DECLARE_FIND_CLASS(_Class,_Func) \
    struct _Class { FIND_OP(_Func) }


    template < class _Value > 
        inline void Swap (_Value& __x, _Value& __y)
    {
        _Value __tmp = __x;
        __x = __y;
        __y = __tmp; 
    }
}


//    atom function
///////////////////////////////////////////////////////////////////////////////



#endif
