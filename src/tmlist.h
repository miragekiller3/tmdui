/*
 *  @file
 *  @brief  encapsulation and extension of STL objects
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2005.10.10
 *
 *  Copyright (C) 2005 miragekiller
 */

///////////////////////////////////////////////////////////////////////////////
//  note:!!!!!!
//
//  vc5.0 stl lib have a bug:
//      when use deque, T size= 4*DWORD & deque size == 128, it will crash
//      test code:
//          CAtDeque<T> x;
//          for (int i=0; i<128; i++) x.insert(T());
//          for (CITem y(x); y; y.erase());
//
//  vc5.0 have another bug
//      class AAA;
//      std::deque < AAA* > x; // complie error : use of undefined type 'AAA'
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef TMLIST_H
#define TMLIST_H

#include "tmrecorder.h"
#include "tmio.h"
#include "tmstring.h"

#include <set>
#include <map>
#include <vector>
#include <list>
#include <queue>
#include <stack>

#include <functional>
#include <algorithm>

#if TM_CPP < TM_CPP17
TM_BGN_NAMESPACE_X(std)

template<class T, class _a> tm_constexpr11 inline T* data(vector<T, _a>& c) { return &*c.begin(); }
template<class T, class _a> tm_constexpr11 inline const T* data(const vector<T, _a>& c) { return &*c.begin(); }
template<class T, class _a> tm_constexpr11 inline size_t size(const vector<T, _a>& c) { return c.size(); }

TM_END_NAMESPACE_X(std)
#endif // TM_CPP < TM_CPP17


#ifdef TM_DBG

template <class _Ty, class _A>
inline int IC_Size(const std::vector<_Ty, _A>& a)
{
    return a.capacity() * sizeof(_Ty);
}

template<class _K, class _Pr, class _A>
inline int IC_Size(const std::set<_K, _Pr, _A>& a)
{
    struct SNODE { void* p[3]; _K _k; int clr; };
    return a.size() * sizeof(SNODE);
}

template<class _K, class _Pr, class _A>
inline int IC_Size(const std::multiset<_K, _Pr, _A>& a)
{
    struct MSNODE { void* p[3]; _K _k; int clr; };
    return a.size() * sizeof(MSNODE);
}

template<class _K, class _Ty, class _Pr, class _A>
inline int IC_Size(const std::map<_K, _Ty, _Pr, _A>& a)
{
    typedef typename std::map<_K, _Ty, _Pr, _A>::value_type value_type;
    struct MNODE { void* p[3]; char c[sizeof(value_type)]; int clr; };
    return a.size() * sizeof(MNODE);
}

template<class _K, class _Ty, class _Pr, class _A>
inline int IC_Size(const std::multimap<_K, _Ty, _Pr, _A>& a)
{
    typedef typename std::multimap<_K, _Ty, _Pr, _A>::value_type value_type;
    struct MMNODE { void* p[3]; char c[sizeof(value_type)]; int clr; };
    return a.size() * sizeof(MMNODE);
}

template<class _Ty, class _A>
inline int IC_Size(const std::list<_Ty, _A>& a)
{
    struct LNODE { void* p[2]; char c[sizeof(_Ty)]; };
    return a.size() * sizeof(LNODE);
}

template<class _Ty, class _A>
inline int IC_Size(const std::deque<_Ty, _A>& a)
{
    return a.size() * sizeof(_Ty);
}

#endif // TM_DBG


TM_BGN_NAMESPACE


///////////////////////////////////////////////////////////////////////////////
// vc old version user std::allocate<T> to map;
// new version use std::allocate<std::pair<k,T>> to map;
#if defined(TM_MSC) && TM_MSC < 1300
#   define tm_allocator_map(alc,k,v) alc<v>
#else
#   define tm_allocator_map(alc,k,v) alc<std::pair<k const, v>>
#endif // #if defined(TM_MSC) && TM_MSC < 1300


template <class _Key, class _Data>
struct CKeyData_Set
{
    typedef _Key key_reference;
    typedef _Data data_reference;
    template <class V> _Key key(const V& v) const { return v; }
    template <class V> _Data data(V& v) const { return v; }
};

template <class _Pair>
struct CKeyData_Map
{
    typedef typename _Pair::first_type      key_type;
    typedef typename _Pair::second_type     data_type;
    typedef const key_type&                 key_reference;
    typedef data_type&                      data_reference;
    key_reference key(const _Pair& v) const { return v.first; }
    data_reference data(_Pair& v) const { return v.second; }
};

#define STL_DECLARE_TYPE_1(_reference) \
    typedef typename base_class::value_type             value_type; \
    typedef typename base_class::_reference             reference; \
    typedef typename base_class::const_reference        const_reference; \
    typedef typename base_class::iterator               iterator; \
    typedef typename base_class::const_iterator         const_iterator; \
    typedef typename base_class::reverse_iterator       reverse_iterator; \
    typedef typename base_class::const_reverse_iterator const_reverse_iterator; \
    typedef typename base_class::size_type              size_type; \
    typedef typename base_class::difference_type        difference_type; \
    typedef std::pair<iterator, bool>                   pair_ret;

#define STL_DECLARE_RECORDER(cls,tp) \
    typedef RECORDER_FS(cls,tp)                         recorder; \
    typedef RECORDER_RS(cls,tp)                         reverse_recorder;

#define STL_DECLARE_TYPE_2(tp) \
    typedef typename traits_key_data::key_reference     key_reference; \
    typedef typename traits_key_data::data_reference    data_reference; \
    typedef RECORDER_FS(class_type,tp)                  recorder; \
    typedef RECORDER_RS(class_type,tp)                  reverse_recorder;


// using for CAdvFlatSet, CAdvSet
#define DECALRE_KEY_DATA_MEMBER(_Key,_T,_Member)                    \
    typedef _Key            key_type;                               \
    typedef _T              data_type;                              \
    typedef const key_type& key_reference;                          \
    typedef data_type&      data_reference;                         \
    template<class T> key_reference key(T& _x) const { return _x _Member; } \
    template<class T> key_reference operator()(T& _x) const { return key(_x); } \
    template<class T> data_reference data(T& _x) const { return _x; }


// for CAdvFlatSet; CAdvSet
# define _TM_KVSTRUCT(T,K,m_K)                                      \
    struct TM_LINK_OF(_CPointSet_GetKey_, __LINE__)                 \
    { DECALRE_KEY_DATA_MEMBER(K, T, m_K) }                          \
    TM_LINK_OF(_CPointSet_GetKey_, __LINE__)


////////////////////////////////////////////////////////////////////////////////
// CAdvFlatSet
// sample:
//  struct RIDInfo { TM::CString str; TMInt32 id; };
//  typedef FLATSET(RIDInfo*, TMInt32, ->id) CRIDByIDList;
//  typedef FLATSET(RIDInfo*, TM::CString, ->str) CRIDByStrList;
//  CRIDByIDList    m_aIndexID;
//  CRIDByStrList   m_aIndexStr;
# define FLATSET_LA(T,K,m_K,less,A)                               \
    _TM_KVSTRUCT(T,K,m_K);                                          \
    typedef TM::CAdvFlatSet<K, T, TM_LINK_OF(_CPointSet_GetKey_, __LINE__), less, A >

# define FLATSET_L(T,K,m_K,less)  FLATSET_LA(T,K,m_K,less,std::allocator<T>)
# define FLATSET_A(T,K,m_K,A)     FLATSET_LA(T,K,m_K,std::less<K>,A)
# define FLATSET(T,K,m_K)         FLATSET_L(T,K,m_K,std::less<K>)

template <class K, class T, class T_GetKey, class Pred = std::less<K>, class _A = std::allocator<T> >
class CAdvFlatSet : public std::vector<T, _A>
{
public:
    typedef CAdvFlatSet                               class_type;
    typedef std::vector<T, _A>                          base_class;

    typedef T_GetKey                                    traits_key_data;

    STL_DECLARE_TYPE_1(reference);
    STL_DECLARE_TYPE_2(eListTypeVector);

    struct PredImpl
    {
        bool operator()(const T& __x, key_reference __y) const
        {
            return Pred() (T_GetKey().key(__x), __y);
        }
    };

private:
    // override this function
    using base_class::assign;
    using base_class::push_back;
    using base_class::pop_back;

public:

#if TM_CPP < TM_CPP11
    const value_type* data() const { return &*this->begin(); }
    value_type* data() { return &*this->begin(); }
    const_iterator cbegin() const { return this->begin(); }
    const_iterator cend() const { return this->end(); }
#endif

    pair_ret insert(const_reference x)
    {
        iterator iter = lower_bound(traits_key_data().key(x));
        if (iter == this->end() || Pred()(traits_key_data().key(x), traits_key_data().key(*iter)))
        {
            iter = base_class::insert(iter, x);
            return pair_ret(iter, true);
        }
        return pair_ret(iter, false);
    }

    iterator insert(iterator it, const_reference x)
    {
        it;
        return insert(x);
    }

    template<class _InputIterator>
    void insert(_InputIterator first, _InputIterator last)
    {
        for (; first < last; ++first)
            insert(*first);
    }

    iterator erase(iterator it) { return base_class::erase(it); }
    iterator erase(iterator first, iterator last) { return base_class::erase(first, last); }
    size_type erase(key_reference key)
    {
        iterator iter = lower_bound(key);
        if (iter == this->end() || Pred()(key, traits_key_data().key(*iter)))
            return 0;
        erase(iter);
        return 1;
    }

    const_iterator find(key_reference key) const
    {
        const_iterator iter = lower_bound(key);
        if (iter == this->end() || Pred()(key, traits_key_data().key(*iter)))
            return this->end();
        return iter;
    }

    iterator find(key_reference key)
    {
        iterator iter = lower_bound(key);
        if (iter == this->end() || Pred()(key, traits_key_data().key(*iter)))
            return this->end();
        return iter;
    }

    size_type count(key_reference key) const
    {
        iterator iter = lower_bound(key);
        if (iter == this->end() || Pred()(key, traits_key_data().key(*iter)))
            return 0;
        return 1;
    }

    const_iterator lower_bound(key_reference key) const
    {
        return std::lower_bound(this->begin(), this->end(), key, PredImpl());
    }

    iterator lower_bound(key_reference key)
    {
        return std::lower_bound(this->begin(), this->end(), key, PredImpl());
    }

    const_iterator upper_bound(key_reference key) const
    {
        return std::upper_bound(this->begin(), this->end(), key, PredImpl());
    }

    iterator upper_bound(key_reference key)
    {
        return std::upper_bound(this->begin(), this->end(), key, PredImpl());
    }
};



template <class T, class Pred = std::less<T>, class _A = std::allocator<T> >
class CFlatSet : public CAdvFlatSet<T, T, CKeyData_Set<const T&, T&>, Pred, _A>
{
public:
    typedef CFlatSet                                  class_type;
    typedef CAdvFlatSet<
        T,
        T,
        CKeyData_Set<const T&, T&>,
        Pred,
        _A>                                             base_class;
};


////////////////////////////////////////////////////////////////////////////////
// CAdvSet, can find by user default key
// sample:
//  struct _QObj { void* p; DWORD pid; };
//  typedef ADVSET(_QObj, void*, .p) CObjSet;
// for CAdvSet
# define ADVSET_LA(T,K,m_K,less,A)                                  \
    _TM_KVSTRUCT(T,K,m_K);                                          \
    typedef TM::CAdvSet<K, T, TM_LINK_OF(_CPointSet_GetKey_, __LINE__), less, A >

# define ADVSET_L(T,K,m_K,less)     ADVSET_LA(T,K,m_K,less,std::allocator<T>)

# define ADVSET_A(T,K,m_K,A)        ADVSET_LA(T,K,m_K,std::less<K>,A)

# define ADVSET(T,K,m_K)            ADVSET_L(T,K,m_K,std::less<K>)

#if defined(TM_MSC) && TM_MSC < 1300
    template <class K, class T, class T_GetKey, class Pred = std::less<K>, class _A = std::allocator<T>, BOOL bMulti = false>
    struct _tm_rbtree : public std::_Tree<K, T, T_GetKey, Pred, _A>
    {
        typedef std::_Tree<K, T, T_GetKey, Pred, _A> base_class;

        template <class _TI>
        _tm_rbtree(_TI b, _TI e, const _A& _Al = _A()) : base_class(b, e, Pred(), bMulti, _Al) {}
        _tm_rbtree(const _A& _Al = _A()) : base_class(Pred(), bMulti, _Al) {}
        _tm_rbtree(const _tm_rbtree& r) : base_class(r) {}
    };

#elif defined(TM_MSC)

    template <class K, class T, class T_GetKey, class Pred = std::less<K>, class _A = std::allocator<T>, bool bMulti = false>
    struct _tm_rbtree_traits : public std::_Tset_traits<K, Pred, _A, bMulti>
    {
        typedef T           value_type;

#if _HAS_CXX17
       using node_type = std::_Node_handle<
           std::_Tree_node<T, typename std::allocator_traits<_A>::void_pointer>,
           _A, std::_Node_handle_set_base, K>;
#endif // _HAS_CXX17

        static const K& _Kfn(const T& _Val)
        {
            return T_GetKey()(_Val);
        }
    };

    template <class K, class T, class T_GetKey, class Pred = std::less<K>, class _A = std::allocator<T>, BOOL bMulti = false>
    struct _tm_rbtree : public std::_Tree<_tm_rbtree_traits<K, T, T_GetKey, Pred, _A, false>>
    {
        typedef std::_Tree<_tm_rbtree_traits<K, T, T_GetKey, Pred, _A, false>> base_class;

        template <class _TI>
        _tm_rbtree(_TI b, _TI e, const _A& _Al = _A()) : base_class(b, e, Pred(), _Al) {}
        _tm_rbtree(const _A& _Al = _A()) : base_class(Pred(), _Al) {}
        _tm_rbtree(const _tm_rbtree& r) : base_class(r, _A()) {}

        bool operator == (const _tm_rbtree& r) const
        {
            return this->size() == r.size()
                && std::equal(this->begin(), this->end(), r.begin());
        }

#if TM_CPP >= TM_CPP11
        _tm_rbtree(_tm_rbtree&& r) = default;
        _tm_rbtree& operator=(const _tm_rbtree&) = default;
        _tm_rbtree& operator=(_tm_rbtree&&) = default;
#endif
    };

#elif defined(TM_GNUC)
    template <class K, class T, class T_GetKey, class Pred = std::less<K>, class _A = std::allocator<T>, TMBool bMulti = false>
    struct _tm_rbtree : public std::_Rb_tree<K, T, T_GetKey, Pred, _A>
    {
        typedef std::_Rb_tree<K, T, T_GetKey, Pred, _A> base_class;
        STL_DECLARE_TYPE_1(reference);

    private:
        using base_class::_M_emplace_equal;
        using base_class::_M_emplace_unique;
        using base_class::_M_emplace_hint_equal;
        using base_class::_M_emplace_hint_unique;
        using base_class::_M_insert_equal;
        using base_class::_M_insert_unique;
        using base_class::_M_insert_equal_;
        using base_class::_M_insert_unique_;

    public:
        template <class _TI>
        _tm_rbtree(_TI b, _TI e, const _A& _Al = _A()) { insert(b, e); }
        _tm_rbtree() {}
        _tm_rbtree(const _tm_rbtree& r) : base_class(r) {}

#if TM_CPP >= TM_CPP11
        template<typename... _Args>
        std::pair<iterator, bool> emplace(_Args&&... __args)
        {
            return bMulti
                ? this->_M_emplace_equal(std::forward<_Args>(__args)...)
                : this->_M_emplace_unique(std::forward<_Args>(__args)...);
        }

        template<typename... _Args>
        iterator emplace_hint(const_iterator __pos, _Args&&... __args)
        {
            return bMulti
                ? this->_M_emplace_hint_equal(__pos, std::forward<_Args>(__args)...)
                : this->_M_emplace_hint_unique(__pos, std::forward<_Args>(__args)...);
	    }
#endif // TM_CPP >= TM_CPP11

        pair_ret insert(const value_type& __x)
        {
            return bMulti
                ? pair_ret(this->_M_insert_equal(__x), true)
                : this->_M_insert_unique(__x);
        }

        iterator insert(const_iterator __position, const value_type& __x)
        {
            return bMulti
                ? this->_M_insert_equal_(__position, __x)
                : this->_M_insert_unique_(__position, __x);

        }

        template<typename _InputIterator>
        void insert(_InputIterator __first, _InputIterator __last)
	    {
            if (bMulti)
                this->_M_insert_equal(__first, __last);
            else
                this->_M_insert_unique(__first, __last);
        }

#if TM_CPP >= TM_CPP11
        pair_ret insert(value_type&& __x)
        {
            return bMulti
                ? pair_ret(this->_M_insert_equal(std::move(__x)), true)
                : this->_M_insert_unique(std::move(__x));
        }

        iterator insert(const_iterator __position, value_type&& __x)
        {
            return bMulti
                ? this->_M_insert_equal_(__position, std::move(__x))
                : this->_M_insert_unique_(__position, std::move(__x));
        }

        void insert(std::initializer_list<value_type> __l)
        {
            this->insert(__l.begin(), __l.end());
        }

#endif // TM_CPP >= TM_CPP11

        size_type count(const K& __x) const
        {
            return bMulti
                ? this->_M_count_tr(__x)
                : (this->find(__x) == this->end() ? 0 : 1);
        }
    };
#endif // defined(TM_GNUC)


template <class K, class T, class T_GetKey, class Pred = std::less<K>, class _A = std::allocator<T> >
class CAdvSet : public _tm_rbtree<K, T, T_GetKey, Pred, _A>
{
public:
    typedef CAdvSet                                     class_type;
    typedef _tm_rbtree<K, T, T_GetKey, Pred, _A>        base_class;
    typedef T_GetKey                                    traits_key_data;

    STL_DECLARE_TYPE_1(reference);
    STL_DECLARE_TYPE_2(eListTypeMap);

public:
    CAdvSet() {}
    CAdvSet(const CAdvSet& r) : base_class(r) {}
    template<class _TI> CAdvSet(_TI b, _TI e) : base_class(b, e) {}

#if TM_CPP < TM_CPP11
    CAdvSet& operator=(const CAdvSet& r)
    {
        base_class::operator =(r);
        return *this;
    }

    const_iterator cbegin() const tm_noexcept { return this->begin(); }
    const_iterator cend() const tm_noexcept { return this->end(); }
    const_reverse_iterator crbegin() const tm_noexcept { return this->rbegin(); }
    const_reverse_iterator crend() const tm_noexcept { return this->rend(); }

#else
    CAdvSet(CAdvSet&& r) = default;
    CAdvSet(std::initializer_list<value_type> l) { insert(l.begin(), l.end()); }

    CAdvSet& operator=(const CAdvSet&) = default;
    CAdvSet& operator=(CAdvSet&&) = default;
    CAdvSet& operator=(std::initializer_list<value_type> l) { this->clear(); insert(l.begin(), l.end()); return *this; }

#endif // TM_CPP < TM_CPP11

};




////////////////////////////////////////////////////////////////////////////////
// class CVector
template<class T, class A = std::allocator<T> > class CVector : public std::vector<T, A>
{
public:
    typedef CVector                                     class_type;
    typedef std::vector<T, A>                           base_class;

    STL_DECLARE_TYPE_1(reference);
    typedef CKeyData_Set<const_reference, reference>    traits_key_data;
    STL_DECLARE_TYPE_2(eListTypeVector);
    using base_class::insert;

public:

#if TM_CPP < TM_CPP11
    const value_type* data() const { return &*this->begin(); }
    value_type* data() { return &*this->begin(); }
    const_iterator cbegin() const { return this->begin(); }
    const_iterator cend() const { return this->end(); }
#endif

    //binary sorting insert
    iterator binary_insert(const_reference __x = T())
    {
        iterator iter = std::lower_bound(this->begin(), this->end(), __x);
        return insert(iter, __x);
    }

    //binary find
    iterator binary_find(const_reference __x)
    {
        iterator iter = std::lower_bound(this->begin(), this->end(), __x);
        if (iter == this->end())
            return iter;
        if (__x < *iter)
            return this->end();
        return iter;
    }

    template <class F>
    iterator binary_insert(const_reference __x, F __f)
    {
        iterator iter = std::lower_bound(this->begin(), this->end(), __x, __f);
        return insert(iter, __x);
    }

    iterator binary_set_insert(const_reference __x = T())
    {
        iterator iter = std::lower_bound(this->begin(), this->end(), __x);
        if (iter == this->end() || __x < *iter)
            iter = base_class::insert(iter, __x);
        return iter;
    }

    iterator binary_force_set_insert(const_reference __x = T())
    {
        iterator iter = std::lower_bound(this->begin(), this->end(), __x);
        if (iter == this->end() || __x < *iter)
            iter = base_class::insert(iter, __x);
        else
            *iter = __x;
        return iter;
    }

    iterator insert (const_reference __x) {
        return insert (this->end(), __x); }

    template < class _K > iterator find (const _K& __key) {
        return std::find (this->begin(), this->end(), __key); }

    template < class _K > const_iterator find (const _K& __key) const {
        return std::find (this->begin(), this->end(), __key); }

    template < class _K > iterator find (int nBegin, const _K& __key) {
        return  std::find (this->begin()+ nBegin, this->end(), __key); }
};


////////////////////////////////////////////////////////////////////////////////
// class CSet
template<class Key, class Pred = std::less<Key>, class A = std::allocator<Key> >
class CSet : public std::set<Key, Pred, A>
{
public:
    typedef CSet                                        class_type;
    typedef std::set<Key, Pred, A>                      base_class;

    STL_DECLARE_TYPE_1(const_reference);
    typedef CKeyData_Set<reference, const_reference>    traits_key_data;
    STL_DECLARE_TYPE_2(eListTypeSet);

#if TM_CPP < TM_CPP11
    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }
#endif
};


template<class Key, class Pred = std::less<Key>, class A = std::allocator<Key> >
class CMultiSet : public std::multiset<Key, Pred, A>
{
public:
    typedef CMultiSet                                   class_type;
    typedef std::multiset<Key, Pred, A>                 base_class;

    STL_DECLARE_TYPE_1(const_reference);
    typedef CKeyData_Set<const_reference, const_reference> traits_key_data;
    STL_DECLARE_TYPE_2(eListTypeMultiSet);

#if TM_CPP < TM_CPP11
    const_iterator cbegin() const { return this->begin(); }
    const_iterator cend() const { return this->end(); }
#endif
};


////////////////////////////////////////////////////////////////////////////////
// class CMap
template<class Key, class T, class Pred = std::less<Key>, class A = TM_TYPENAME std::map<Key,T>::allocator_type>
class CMap : public std::map<Key, T, Pred, A>
{
public:
    typedef CMap                                        class_type;
    typedef std::map<Key, T, Pred, A>                   base_class;

    STL_DECLARE_TYPE_1(reference);
    typedef CKeyData_Map<value_type>                    traits_key_data;
    STL_DECLARE_TYPE_2(eListTypeMap);

public:
    pair_ret insert(const value_type& x)
    { return base_class::insert(x); }

    iterator insert(iterator it, const value_type& x)
    { return base_class::insert(it, x); }

    template<class _TI> void insert(_TI _first, _TI _last)
    {
        for (; _first != _last; ++_first)
            this->insert(*_first); 
    }

    pair_ret insert2(const Key& __k, const T& __d)
    { return base_class::insert(value_type(__k, __d)); }

    // erase on GCC<11 return void
#if !defined(TM_MSC) && (TM_CPP < TM_CPP11)
    iterator erase(iterator it)
    {
        if (it != this->end())
        {
            iterator it2 = it;
            ++it2;
            base_class::erase(it);
            return it2;
        }
        return this->end();
    }
#endif

#if TM_CPP < TM_CPP11
    const_iterator cbegin() const { return this->begin(); }
    const_iterator cend() const { return this->end(); }
#endif

};


////////////////////////////////////////////////////////////////////////////////
// class CMultiMap
template<class Key, class T, class Pred = std::less<Key>, class A = TM_TYPENAME std::map<Key,T>::allocator_type >
class CMultiMap : public std::multimap<Key, T, Pred, A>
{
public:
    typedef CMultiMap                               class_type;
    typedef std::multimap<Key, T, Pred, A>          base_class;

    STL_DECLARE_TYPE_1(reference);
    typedef CKeyData_Map<value_type>                traits_key_data;
    STL_DECLARE_TYPE_2(eListTypeMultiMap);

public:
    using base_class::insert;

    pair_ret insert2(const Key& __k, const T& __d) {
        return pair_ret(base_class::insert(value_type(__k, __d)), true); }
    
    template<class _TI> void insert(_TI _first, _TI _last)
    {
        for (; _first != _last; ++_first)
			this->insert(*_first); 
    }

    iterator find_by(const Key& __k, const T& __d)
    {
        std::pair<iterator, iterator> range = base_class::equal_range(__k);
        for (iterator it = range.first; it != range.second; ++it)
            if ((*it).second == __d)
                return it;
            return this->end();
    }

    // erase on GCC<11 return void
#if !defined(TM_MSC) && (TM_CPP < TM_CPP11)
    iterator erase(iterator it)
    {
        if (it != this->end())
        {
            iterator it2 = it;
            ++it2;
            base_class::erase(it);
            return it2;
        }
        return this->end();
    }
#endif

#if TM_CPP < TM_CPP11
    const_iterator cbegin() const { return this->begin(); }
    const_iterator cend() const { return this->end(); }
#endif
};


////////////////////////////////////////////////////////////////////////////////
// class CList
template<class T>
class CList : public std::list<T>
{
public:
    typedef CList                                       class_type;
    typedef std::list < T >                             base_class;

    STL_DECLARE_TYPE_1(reference);
    typedef CKeyData_Set<const_reference, reference>    traits_key_data;
    STL_DECLARE_TYPE_2(eListTypeList);
    using base_class::insert;

public:
    iterator insert (const_reference __x) {
        return insert (this->end(), __x); }

    template < class _K > iterator find (const _K& __key) {
        return std::find (this->begin(), this->end(), __key); }

    template < class _K > const_iterator find (const _K& __key) const {
        return std::find (this->begin(), this->end(), __key); }

//     static iterator T2Iterator(const T& _vol)
//     {
//         return TM_OTHIS(base_class::_Node, _Value);
//     }

#if TM_CPP < TM_CPP11
    const_iterator cbegin() const { return this->begin(); }
    const_iterator cend() const { return this->end(); }
#endif
};


////////////////////////////////////////////////////////////////////////////////
// class CDeque
template<class T> class CDeque : public std::deque<T>
{
public:
    typedef CDeque                                      class_type;
    typedef std::deque<T>                               base_class;

    STL_DECLARE_TYPE_1(reference);
    typedef CKeyData_Set<const_reference, reference>    traits_key_data;
    STL_DECLARE_TYPE_2(eListTypeDeque);

public:
    iterator insert (const_reference __x) {
        this->push_back (__x); return this->end () - 1; }

    template < class _K > iterator find (const _K& __key) {
        return std::find (this->begin(), this->end(), __key); }

    template < class _K > const_iterator find (const _K& __key) const {
        return std::find (this->begin(), this->end(), __key); }

#if TM_CPP < TM_CPP11
    const_iterator cbegin() const { return this->begin(); }
    const_iterator cend() const { return this->end(); }
#endif
};


class CAnyStack : public std::vector<char>
{
public:
    ~CAnyStack()
    {
        size_t sz = size();
        DbgAssert(sz == 0);
    }

    template <class T> void push(const T& __t)
    {
#ifdef TM_DBG
        TMUInt32 x = 0x12345678;
        insert(end(), (char*)&x, (char*)(&x + 1));
#endif
        insert(end(), (char*)&__t, (char*)(&__t + 1));
    }

    template <class T> void pop(T& __t)
    {
        size_t n = sizeof(T);
        iterator ite = end();
        iterator itb = ite - n;
        std::copy(itb, ite, (char*)&__t);
        erase(itb, ite);

#ifdef TM_DBG
        TMUInt32 x;
        n = sizeof(TMUInt32);
        iterator it = end() - n;
        MemCpy(&x, &*it, n);
        erase(it, end());
        DbgAssert(x == 0x12345678);
#endif
    }

    template <class T> size_t get(size_t pos, T& __t) const
    {
#ifdef TM_DBG
        TMUInt32 nc;
        size_t x = sizeof(x);
        const_iterator it = begin() + pos;
        MemCpy(&nc, &*it, x);
        DbgAssert(nc == 0x12345678);
        pos += x;
#endif

        size_t n = sizeof(T);
        const_iterator itb = begin() + pos;
        const_iterator ite = itb + n;
        std::copy(itb, ite, (char*)&__t);
        return pos + n;
    }
};

// class CAnyVector : public std::vector<char>
// {
// private:
//     mutable int m_nPos;
// 
// public:
//     typedef std::vector<char>::difference_type difference_type;
// 
//     CAnyVector() : m_nPos(0) {}
// 
//     template <class T> void operator << (const T& __t)
//     {
//         insert(end(), (char*)&__t, (char*)(&__t + 1));
//     }
// 
//     template <class T> void operator >> (T& __t) const
//     {
//         DbgAssert(m_nPos >= 0 && m_nPos < this->length());
//         __t = *(T*)(std::data(*this) + m_nPos);
//         m_nPos += sizeof(T);
//         DbgAssert(m_nPos <= this->length());
//     }
// 
//     size_type GetSize () const{ return this->length(); }
//     difference_type Tell() const { return m_nPos; }
//     TMBool Eof() const { return Tell() >= GetSize(); }
//     difference_type SeekSet(difference_type lOffset) { m_nPos = lOffset; return Tell(); }
//     difference_type SeekCur(difference_type lOffset) { m_nPos += lOffset; return Tell(); }
//     difference_type SeekEnd(difference_type lOffset) { m_nPos = m_dwSize + lOffset; return Tell(); }
//     difference_type Skip(difference_type n) {difference_type pos = Tell(); SeekCur(n); return Tell(); }
// 
// };




// template <class T> struct std_life : public std::allocate<T>
// {
//     void transfer(T* dst, T* src) { construct(dst, *src); destroy(src); }
// };
//
// template <class T> struct fast_life : public std::allocate<T>
// {
//     typedef TMUInt8[sizeof(T)] mem_type;
//
//     void transfer(T* dst, T* src) { *(mem_type*)dst = *(mem_type*)src; }
// };

///////////////////////////////////////////////////////////////////////////
// class CGrid
//
//          x nx
//       1  | 2 |  3
//  y  -----     -----
//  ny   4    5    6
//     -----     -----
//       7  | 8 |  9
/*
template <class T>
struct GridMem
{
    typedef typename Int2MemType<sizeof(T)>::mem_type   mem_type;

    T* m_pt;
    int m_cx, m_cy;

    GridMem()
        : m_pt(0)
        , m_cx (0)
        , m_cy (0)
    {
    }

    void _free()
    {
        if (m_pt)
            free(m_pt);
    }

    void _construct(int x, int y, int nx, int ny, const T& val)
    {
        mem_type* pt = (mem_type*)(m_pt + x);
        for (int j = 0; j < ny; ++j, pt += m_cx)
            std::fill(pt, pt + nx, (const mem_type&)(val));
    }

    template <class T_delete>
    void _destroy(int x, int y, int nx, int ny, T_delete del)
    {
        T* pt = m_pt + x;
        for (int j = 0; j < ny; ++j, pt += m_cx)
            std::for_each(pt, pt + nx, del);
    }

    void _move_big_to_small(int dx, int dy, GridMem& src, int sx, int sy, int sw, int sh)
    {
        mem_type* pd = (mem_type*)(m_pt + m_cx * dy + dx);
        mem_type* ps = (mem_type*)(src.m_pt + src.m_cx * sy + sx);
        for (int y = 0; y < sh; ++y, pd += m_cx, ps += src.m_cx)
            std::copy(ps, ps + sw, pd);
    }

    void _move_small_to_big(int dx, int dy, GridMem& src, int sx, int sy, int sw, int sh)
    {
        for (int y = sh - 1; y >= 0; --y)
        {
            mem_type* pd = (mem_type*)(m_pt + m_cx * (dy + y) + dx);
            mem_type* ps = (mem_type*)(src.m_pt + src.m_cx * (sy + y) + sx);
            std::copy_backward(ps, ps + sw, pd);
        }
    }

    void _reserve_and_move(int cx, int cy, int x, int y)
    {
        if (cx > m_cx || cy > m_cy)
        {
            GridMem m(*this);

            m_pt = malloc(cx * cy * sizeof(T));
            m_cx = cx;
            m_cy = cy;

            // move memory
            _move_big_to_small(0, 0, m, 0, 0, x, y);
        }
    }
};
 */
/*
template <class T, class T_delete>
class CGridPtrVector : private GridMem<T>
{
public:
    typedef CGridPtrVector                              class_type;
    typedef T                                           value_type;
    typedef T*                                          pointer;
    typedef T&                                          reference;
    typedef const T&                                    const_reference;
    typedef size_t                                      size_type;
    typedef typename Int2MemType<sizeof(T)>::mem_type   mem_type;
    typedef GridMem<T>                                  base_class;

private:
    int m_x, m_y;

// private:
//     void construct(int x, int y, int nx, int ny, const T& val)
//     {
//         for (int j = 0; j < ny; ++j)
//             for (int i = 0; i < nx; ++i)
//                 (mem_type&)(m_pt[(y + j) * m_capacityx + i + x]) = val;
//     }
//
//     void destroy(pointer p, int n)
//     {
//         for (pointer pe = p + n; p < pe; ++p)
//         {
//             T_delete()(*p);
//         }
//     }
//
//     void destroy(int x, int y, int nx, int ny)
//     {
//         pointer pt = m_pt + m_capacityx * y + x;
//         for (int yy = 0; yy < ny; ++yy, pt += m_capacityx)
//             destroy (pt, nx);
//     }
//
//     void move_to(pointer pto, pointer pfrom, int n)
//     {
//         memcpy (pto, pfrom, n * sizeof(value_type));
//     }
//
//     void move_big_to_small(int dx, int dy, int sx, int sy, int sw, int sh)
//     {
//         pointer ptd = m_pt + m_capacityx * dy + sx;
//         pointer pts = m_pt + m_capacityx * sy + dy;
//         for (int y = 0; y < sh; ++y, ptd += m_capacityx, pts += m_capacityx)
//             move_to (pts, pts + sw,  ptd);
//     }
//
//     void move_small_to_big(pointer pd, int cd, pointer ps, int w, int h)
//     {
//         for (int y = h - 1; y >= 0; --y)
//         {
//             pointer ptd = pd + y * cd;
//             pointer pts = ps + y * m_capacityx;
//             memmove(ptd, pts, w * sizeof(value_type));
//         }
//     }
//
//     pointer reserve_and_move(size_type cx, size_type cy, int x, int y)
//     {
//         pointer pd = m_pt;
//         if (cx > m_capacityx || cy > m_capacityy)
//         {
//             pd = allocate(cx * cy, (void *)0);
//             // move 1 to 1
//             move_big_to_small(pd, cx, m_pt, x, y);
//         }
//         return pd;
//     }
//
//     void reserve_for_insert(size_type cx, size_type cy, int x, int nx, int y, int ny, const T& val)
//     {
//         pointer ps = m_pt;
//         pointer pd = reserve_and_move(cx, cy, x, y);
//
//         // move 2,5,8 to 3,6,9
//         move_small_to_big(pd + x + nx, cx, ps + x, m_x - x, m_y);
//         // move 4,5,6 to 7,8,9
//         move_small_to_big(pd + (y + cy) * cx, cx, ps + y * m_cx, m_x + nx, m_y - y);
//
//         // construct 2, 4,5,6, 8
//
//         move_small_to_big(pd + (y + cy) * cx + x + nx, cx, ps + y * m_cx + x, m_x + nx, m_y - y); //move 5 to 9
//
//         if (pd != ps)
//         {
//             m_pt = pd;
//             _A::deallocate(ps, m_capacityx * m_capacityy);
//         }
//         m_capacityx = cx;
//         m_capacityy = cy;
//
//         construct(construct)
//
//
//
//
//         for (size_t i = 0; i < m_y; ++i, po += m_capacityx; pn += cx)
//             move_to(pn, po, m_x);
//         deallocate(m_pt, m_capacityx * m_capacityy);
//
//         m_pt = p;
//         m_capacityx = cx;
//         m_capacityy = cy;
//     }

public:
    CGridPtrVector()
        : m_x(0)
        , m_y(0)
    {
    }

    ~CGridPtrVector()
    {
        clear();
        base_class::Free();
    }

    void clear()
    {
        int x = m_x, y = m_y;
        m_x = m_y = 0;
        _destroy(0, 0, x, y, T_delete());
    }

    void reserve(size_type cx, size_type cy)
    {
        base_class m(*this);
        base_class::_reserve_and_move(cx, cy, m_x, m_y);
        if (m.m_pt != this->m_pt)
            m._free();
    }

    void resize(size_t x, size_t y, const T& val = T())
    {
        if (x == m_x && y == m_y)
            return;

        if (x < m_x)
        {
            _destroy(x, 0, m_x - x, m_y);
            m_x = x;
        }

        if (y < m_y)
        {
            _destroy(0, y, m_x, m_y - y);
            m_y = y;
        }

        insert (m_x, x - m_x, m_y, y - m_y, val);
    }

    void erase(int x, int nx, int y, int ny)
    {
        if (x > m_x)
            x = m_x;
        if (x + nx > m_x)
            nx = m_x - x;
        if (y > m_y)
            y = m_y;
        if (y + ny)
            ny = m_y - y;

        // move 7,8,9 to 4,5,6
        _destroy (0, y, m_x, ny, T_delete());
        _move_big_to_small(0, y, *this, 0, y + ny, m_x, m_y - y - ny);
        m_y -= ny;

        // move 3,6 to 2,5
        _destroy(x, 0, nx, m_y, T_delete());
        _move_big_to_small(x, 0, *this, x + nx, 9, m_x - x - nx, m_y);
        m_x -= nx;
    }

    void insert(int x, int nx, int y, int ny, const T& val = T())
    {
        base_class m(*this);

        if (x > m_x)
            x = m_x;
        if (y > m_y)
            y = m_y;

        {
            int cx = x + nx, cy = y + ny;
            if (cx > m_cx || cy > m_cy)
            {
                if (cx > m_cx)
                    cx = 4 + cx * 3 / 2;
                if (cy > m_cy)
                    cy = 4 + cy * 3 / 2;
                _reserve_and_move(cx, cy, x, y);
            }
        }

        //move 4,5,6 to 7,8,9
        _move_small_to_big(0, y + ny, m, 0, y, m_x, m_y - y);
        _construct(0, y, m_x, ny, val);
        m_y += ny;

        // move 2 to 3, 8 to 9
        _move_small_to_big(x + nx, 0, m, x, 0, m_x - x, y);
        _move_small_to_big(x + nx, y + ny, m, x, y, m_x - x, m_y - y - ny);
        _construct(x, 0, nx, m_y, val);
    }

    size_type size() const { return m_x * m_y; }

    bool empty() const { return m_x == 0 && m_y == 0; }

    size_type rows() const { return m_y; }

    size_type columns() const { return m_x; }
};
*/

// CSingletonListT:
//  1. auto create list when insert
//  2. auto release when erase
// this function must use static init
//
// sample:
// struct CGdiFont
// {
//     typedef CMap<X, Y> CFontList;
//     typedef CSingletonListT<CFontList> CAutoFontList;
//     static CAutoFontList s_aFont;
//     void erase(X x) { s_aFont->erase(x); s_aFont.check_release(); }
// }
// CGdiFont::CAutoFontList CGdiFont::s_aFont;

// template <class T>
// class CSingletonListT
// {
// public:
//     typedef T element_type;
//     CInstanceCheckClass<element_type>* m_pList;
//
// public:
//     T& instance()
//     {
//         if (!m_pList)
//         {
//             m_pList = new CInstanceCheckClass<element_type>();
//         }
//         return *m_pList;
//     }
//
//     T* get()
//     {
//         DbgAssert(m_pList);
//         return m_pList;
//     }
//
//     T* operator ->()
//     {
//         return get();
//     }
//
//     T& operator * ()
//     {
//         return instance();
//     }
//
//     void check_release()
//     {
//         DbgAssert(m_pList);
//         if (!m_pList->size())
//         {
//             delete m_pList;
//             m_pList = 0;
//         }
//     }
// };



TM_END_NAMESPACE

// sample:
// static CStaticMapPair<int, LPCTSTR> map[] = { 0, "1", 1, "2" };
// TMBool c = StaticMapFind(map, 0, ret); return true; ret = "1"
// TMBool c = StaticMapFind(map, 4, ret); return false;
template <class K, class V> struct CStaticMapPair
{
    typedef K first_type;
    typedef V second_type;
    K first; V second;
};

template <class T, class K, class V>
inline static TMBool StaticMapFind(T& _map, K key, V& ret)
{
    for (int i = 0; i < tm_countof(_map); ++i)
    {
        if (key == _map[i].first)
        {
            ret = _map[i].second;
            return true;
        }
    }
    return false;
}

template <class T, class K, class V>
inline static TMBool StaticMapFindBy(T& _map, K key, V& ret)
{
    for (int i = 0; i < tm_countof(_map); ++i)
    {
        if (key == _map[i].second)
        {
            ret = _map[i].first;
            return true;
        }
    }
    return false;
}

template <class T, class K, class V>
inline static TMBool StaticMapFindByStrI(T& _map, K key, V& ret)
{
    for (int i = 0; i < tm_countof(_map); ++i)
    {
        if (StrCompareI(key, _map[i].second) == 0)
        {
            ret = _map[i].first;
            return true;
        }
    }
    return false;
}

#endif // #ifndef TMLIST_H
