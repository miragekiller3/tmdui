/*
 *  @file
 *  @brief  CRecorderT: common interfaces operated on STL objects
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2002.09.02
 *
 *  Copyright (C) 2002 miragekiller
 */

#pragma once

#ifndef TMRECORDER_H
#define TMRECORDER_H

#include "tmiterator.h"
#include "tmdebug.h"

#ifdef TM_MSC
// identifier was truncated to '255' characters in the browser information
#   pragma warning (disable : 4786)
#endif


///////////////////////////////////////////////////////////////////////////////
// get key type, value type, iterator from the specified types 
// meaning of class names:
//   forword: direction of iterator is forward
//   Reverse: direction of iterator is reverse
//   Set: only a single value;value_type==key_type==data_type
//   Map: value composed of key_type and data_type,used for map and multimap 
TM_BGN_NAMESPACE

// enum ERecorderFlag
// {
//     e_rfAddN    = 0x01, // for vector, list
//     e_rfSort    = 0x02, // for setvector, setlist, set
//     e_rfMap     = 0x04, // for map, multmap, mapvector
//     e_rfReverse = 0x08, 
//     e_rfConst   = 0x10,
// };

enum EListType
{
    eListTypeVector,
    eListTypeSet,
    eListTypeMultiSet,
    eListTypeMap,
    eListTypeMultiMap,
    eListTypeList,
    eListTypeQueue,
    eListTypeDeque,
    eListTypeStack,
};


template <class _List> struct CForward
{
    typedef typename _List::iterator                iterator;
    inline static iterator begin (_List& __ls) { return __ls.begin(); }
    inline static iterator end (_List& __ls) { return __ls.end(); }
};

template <class _List> struct CReverse
{
    typedef typename _List::reverse_iterator        iterator;
    inline static iterator begin (_List& __ls) { return __ls.rbegin(); }
    inline static iterator end (_List& __ls) { return __ls.rend(); }
};

TM_END_NAMESPACE


///////////////////////////////////////////////////////////////////////////////
//  CRecorderT
//  like iterator
TM_BGN_NAMESPACE

template<class _List, class _Fwd, EListType eType>
    class CRecorderT
{
public:
    typedef CRecorderT                              class_type;
    typedef _List                                   cont_type;
    
    typedef typename cont_type::value_type          value_type;
    typedef typename cont_type::const_reference     const_reference;
    typedef typename cont_type::reference           reference;

    typedef typename _Fwd::iterator                 iterator;
    typedef typename _List::key_reference           key_reference;
//     typedef typename _List::const_data_reference    const_data_reference;
    typedef typename _List::data_reference          data_reference;

protected:
    cont_type*  m_aList;
    iterator    m_iter;

protected:
    inline iterator _getbegin () const {
        DbgAssert(m_aList); return _Fwd::begin (*m_aList); }

    inline iterator _getend () const {
        DbgAssert(m_aList); return _Fwd::end (*m_aList); }

    template < class _T1, class _T2 >
    inline TMBool _ret (std::pair<_T1, _T2> __p) {
        m_iter = __p.first; return __p.second; }

    inline TMBool _ret (iterator __it) {
        m_iter = __it; return true; }

public:
    CRecorderT          (cont_type& __list)
        : m_aList       (&__list)
        , m_iter        (_getbegin())
    {
        DbgAssert(m_aList);
    }

    CRecorderT          (const class_type& __rec)
        : m_aList       (__rec.m_aList)
        , m_iter        (__rec.m_iter)
    {
        DbgAssert(m_aList);
    }

    CRecorderT          (cont_type& __list, iterator __iter)
        : m_aList       (&__list)
        , m_iter        (__iter)
    {
        DbgAssert(m_aList);
    }

    CRecorderT          (cont_type& __list, int n)
        : m_aList       (&__list)
        , m_iter        (_getbegin() + n)
    {
        DbgAssert(m_aList);
    }

    class_type& operator = (const class_type& other)
    {
        m_aList = other.m_aList;
        m_iter = other.m_iter;
        DbgAssert(m_aList);
        return *this;
    }

    iterator save() const { return m_iter; }

    void restore(iterator iter) { m_iter = iter; }

    bool bof() const { return m_iter == _getbegin(); }
    
    bool eof() const { return _getend() == m_iter; }

    size_t size() const { return m_aList->size(); }
    
    class_type& begin() { m_iter = _getbegin(); return *this; }
    
    class_type& end() { m_iter = _getend(); return *this; }
    
    operator bool() const { return !eof(); }
    
    bool operator !() const { return eof(); }

    bool operator ==(const CRecorderT& rhs) const {
        return m_iter == rhs.m_iter; }
    
    iterator operator ->() const { return m_iter; }

    class_type& operator ++() {
        DbgAssert(m_iter != _getend());  ++m_iter;  return *this; }

    class_type& operator --() {
        DbgAssert(m_iter != _getbegin());  --m_iter;  return *this; }

    class_type& operator +=(int n) {
        std::advance(m_iter, n); return *this; }

    class_type operator +(int n) const {
        class_type x(*this); x += n; return x;
    }

    class_type operator -(int n) const {
        class_type x(*this); x += -n; return x;
    }

    reference operator*()
    {
        DbgAssert(!eof());
        return *m_iter;
    }

    cont_type* get_list() { return m_aList; }

    iterator get_iter() { return m_iter; }

    key_reference get_key()
    {
        typedef typename _List::traits_key_data tr;
        return tr().key(*m_iter);
    }

    data_reference get_data()
    {
        typedef typename _List::traits_key_data tr;
        return tr().data(*m_iter);
    }
    
    template<class _F, class _K>
    class_type& find(_F __func, const _K& __key)
    {
        iterator __cur = _getbegin();
        iterator __end = _getend();
        
        for (; __cur != __end; ++__cur)
        {
            if (__func(*__cur, __key))
                break;
        }
        
        m_iter = __cur;
        return *this;
    }

    template <class _F, class _K>
    class_type& rfind(_F __func, const _K& __key)
    {
        iterator __bgn = _getbegin();
        m_iter = _getend();
        
        for (; m_iter != __bgn;)
        {
            --m_iter;
            if (__func(*m_iter, __key))
                return *this;
        }
        
        m_iter = _getend();
        return *this;
    }

    template<class _K> class_type& find(const _K& __key)
    {
        m_iter = m_aList->find(__key);
        return *this;
    }
    
    template<class _F> void for_each(_F __func)
    {
        std::for_each(_getbegin(), _getend(), __func);
    }

    void set(const_reference __val)
    {
        if (!eof())
            *m_iter = __val;
        else
            force_insert(__val);
    }

    TMBool insert(const_reference __val) {
        return _ret (m_aList->insert(__val)); }

//     void force_insert (const_reference __val)
//     {
//         if (!insert (__val))
//             *m_iter = __val;
//     }
// 
//     template < class _K, class _D >
//     TMBool insert (const _K& __key, const _D& __data) {
//         return _ret (m_aList->insert (__key, __data)); }
// 
//     template < class _K, class _D >
//     void force_insert (const _K& __key, const _D& __data) {
//         force_insert (value_type (__key, __data)); }

    void erase()
    {
        DbgAssert(m_iter != m_aList->end());
        m_iter = m_aList->erase(m_iter);
    }

//     void safe_erase()
//     {
//         if (*this)
//             erase();
//     }

    // delete the current object using the specified destructor
//     template < class _F >
//         void erase (const _F& __func)
//     {
//         if (*this)
//         {
//             __func (*m_iter);
//             erase ();
//         }
//     }

    private: template<class I> void _list_erase(I itErase, Int2Type<eListTypeVector>)
    {
        int old = m_iter - m_aList->begin();
        int now = itErase - m_aList->begin();
        DbgAssert(old != now);
        if (now <= old)
            --old;
        m_aList->erase(itErase);
        m_iter = m_aList->begin() + old;
    }

    private: template<class I> void _list_erase(I itErase, ...)
    {
        m_aList->erase(itErase);
    }

    public: void list_erase(iterator itErase)
    {
        if (itErase == m_iter)
            erase();
        else
            _list_erase(itErase, Int2Type<eType>());
    }

    static void list_clear(cont_type& aList)
    {
        cont_type __list;
        __list.swap(aList);
    }
    
    // delete all of the objects using the specified destructor
    // purpose for swap is to avoid re-entry
    template <class _F> static void list_clear(cont_type& aList, _F __func)
    {
        cont_type __list;
        __list.swap(aList);
        class_type(__list).for_each(__func);
    }
};

#define RECORDER_FS(x,tp)   TM::CRecorderT<x, TM::CForward<x>, tp>
#define RECORDER_RS(x,tp)   TM::CRecorderT<x, TM::CReverse<x>, tp>
#define RECORDER_FM(x,tp)   TM::CRecorderT<x, TM::CForward<x>, tp>
#define RECORDER_RM(x,tp)   TM::CRecorderT<x, TM::CReverse<x>, tp>

TM_END_NAMESPACE


TM_BGN_NAMESPACE

///////////////////////////////////////////////////////////////////////////////
// CStaticSetRecorderT
// sample:
//  static int s_x[] = {1,2,3,4,8};
//  TM_STATIC_SET_CHECK(s_x); // check s_x order must from min to max.
//  typedef CStaticSetRecorderT<int> Recorder;
//  Recorder rec(s_x); 
//  rec.find(4);
//  if (rec) ...
template <class T>
struct CStaticSetRecorderT
{
private:
    const T* p;
    int n;
    const T* iter;
    
public:
    template <class L>
    CStaticSetRecorderT(const L& x)
        : p(x)
        , n(tm_countof(x))
        , iter(x)
    {}
    
    tm_constexpr11 operator bool() const { return iter != p + n; }
    tm_constexpr11 void begin() { iter = p; }
    tm_constexpr11 void end() { iter = p + n; }
    tm_constexpr11 const T* operator -> () const { DbgAssert(iter >= p && iter < p + n); return iter; }
    
    template <class _Key>
    CStaticSetRecorderT& find(const _Key& key)
    {
        iter = std::lower_bound(p, p + n, key);
        if (iter != p + n && key < *iter)
            iter = p + n;
        return *this;
    }
};

#ifdef TM_DBG 

    template <class L>
    void _static_set_check(L& x)
    {
        for (int i = 1; i < tm_countof(x); ++i)
            DbgAssert(x[i - 1] < x[i]);
    }

#   define TM_STATIC_SET_CHECK(x) static const int s_static_set_check_##x = (TM::_static_set_check(x), 0)
#else
#   define TM_STATIC_SET_CHECK(x)
#endif

TM_END_NAMESPACE


TM_BGN_NAMESPACE

template < class _List, class _Func, class _Param >
inline typename _List::recorder ListFind (_List& __lst, _Func __func, 
                                 const _Param& __param)
{
    typename _List::recorder __iter (__lst);

    for (; __iter; ++__iter)
    {
        if (__func (*__iter, __param))
            break;
    }

    return __iter;
}


///////////////////////////////////////////////////////////////////////////
//    sample :
//    class  AAA 
//    {
//        void aaa();
//    };
//    CVector<AAA> list;
//    ListEnum (list, AAA::aaa);
//    it will call aaa() in all list member;

template < class _List, class _Func>
inline void ListEnum (_List& __lst, _Func __func)
{
    typedef typename _List::value_type              value_type;
    typedef typename PointerOf<value_type>::pointer pointer;

    typename _List::recorder __iter (__lst);

    for (; __iter; ++__iter)
    {
        pointer pt = TM_P(__iter.get_data());
        (pt ->* __func)();
        //            (TM_P(__iter.value()) ->* __func)();
    }
}

///////////////////////////////////////////////////////////////////////////
//    sample :
//    class  AAA 
//    {
//        void aaa(int x);
//    };
//    CVector<AAA> list;
//    ListEnum (list, AAA::aaa, 5);
//    it will call aaa(5) in all list member;

template < class _List, class _Func, class _Param >
inline void ListEnum (_List& __lst, _Func __func, _Param __param)
{
    typedef typename _List::value_type              value_type;
    typedef typename PointerOf<value_type>::pointer pointer;

    typename _List::recorder __iter (__lst);

    for (; __iter; ++__iter)
    {
        pointer pt = TM_P(__iter.get_data());
        (pt ->* __func)(__param);

        //            (TM_P(__iter.value()) ->* __func)(__param);
    }
}


///////////////////////////////////////////////////////////////////////////
//    sample :
//    class  AAA 
//    {
//        void aaa(int x, int y);
//    };
//    CVector<AAA> list;
//    ListEnum (list, AAA::aaa, 5, 6);
//    it will call aaa(5, 6) in all list member;

template < class _List, class _Func, class _Param1, class _Param2 >
inline void ListEnum (_List& __lst, _Func __func, _Param1 __param1, 
                      _Param2 __param2)
{
    typedef typename _List::value_type              value_type;
    typedef typename PointerOf<value_type>::pointer pointer;

    typename _List::recorder __iter (__lst);

    for (; __iter; ++__iter)
    {
        pointer pt = TM_P(__iter.get_data());
        (pt ->* __func)(__param1, __param2);
        //            (TM_P(__iter.value()) ->* __func)(__param1, __param2);
    }
}


//In powerpoint file, we may have two UCF objects are in the same page, and
// they are overlap, the early version makes the UCF objects order reversed.
//Add this function to make the UCF object order in the page is the same as
// the order in the powerpoint.
//bug 164122,walker 
template < class _List, class _Func>
inline void ListEnumReverse (_List& __lst, _Func __func)
{
    typedef typename _List::value_type              value_type;
    typedef typename PointerOf<value_type>::pointer pointer;

    typename _List::recorder __iter (__lst);

    int nSize = __lst.size();
    for (; __iter; ++__iter)
    {
        nSize--;
        if (nSize == 0)
            break;
    }
    nSize=0;
    for (; (nSize<__lst.size()); --__iter)
    {
        pointer pt = TM_P(__iter.get_data());
        if (pt)
            (pt ->* __func)();
        nSize++;
    }
}

TM_END_NAMESPACE


#endif

