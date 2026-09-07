/*
 *  @file
 *  @brief  tree object
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.01.31
 *
 *  Copyright (C) 2006 miragekiller
 */

#pragma once

#ifndef TMTREE_H
#define TMTREE_H

#include "tmdebug.h"


#ifdef TM_CPP


TM_BGN_NAMESPACE


////////////////////////////////////////////////////////////////////////////////
// standard tree
// T must contain _parent, _prev, _next, _child those 4 members

template <class T, class TDSContainer> struct CTreeItemT;

struct CTreeDataSourceContainer
{
    template <class P> inline static P GetParent(P p)   { return p->_parent; }
    template <class P> inline static P GetNext(P p)     { return p->_next; }
    template <class P> inline static P GetPrev(P p)     { return p->_prev; }
    template <class P> inline static P GetChild(P p)    { return p->_child; }
};

struct FTreeWalkStop
{
    template <class T> TMBool operator () (T _x) const { return true; }
};


template <class T, class TDSContainer = CTreeDataSourceContainer>
struct CTreeItemT : public TDSContainer
{
public:
    T m_pItem;

    typedef TDSContainer                            data_source;

private:
    // comparison with argument
    template <class PFN, bool r> struct CMP1
    {
        PFN pfn;
        TMLParam lp;

        CMP1 (PFN f, TMLParam l)
            : pfn (f)
            , lp (l)
        {
        }

        template <class P>
        bool operator() (P p1, P p2) const
        {
            if (r)
                return 0 != pfn (p2, p1, lp);
            else
                return 0 != pfn (p1, p2, lp);
        }
    };

    // comparison without argument
    template <class PFN, bool r> struct CMP2
    {
        PFN pfn;

        CMP2 (PFN f)
            : pfn (f)
        {
        }

        template <class P>
        bool operator() (P p1, P p2) const
        {
            if (r)
                return 0 != pfn (p2, p1);
            else
                return 0 != pfn (p1, p2);
        }
    };

    void _ParentNext (const T pRoot)
    {
        m_pItem = GetParent();
        T pTmp;

        if (!m_pItem)
            return;
        else if (m_pItem == pRoot)
            m_pItem = 0;
        else
        {
            if (0 != (pTmp = GetNext()))
                m_pItem = pTmp;
            else
                _ParentNext(pRoot);
        }
    }

    // Detach(c) - result:
    // -->next
    // a-b-c-d           a-b-d       c
    // | | |             | |     +   |
    // e f g-h           e f         g-h
//     inline static void _Detach (T pItem)
//     {
//         if (pItem->_prev)
//         {
//             pItem->_prev->_next = pItem->_next;
//         }
//         else if (pItem->_parent)
//         {
//             DbgAssert(pItem->_parent->_child == pItem);
//             pItem->_parent->_child = pItem->_next;
//         }
//         if (pItem->_next)
//         {
//             pItem->_next->_prev = pItem->_prev;
//         }
//     }

    T _GlobalLast(T p)
    {
        for (;;)
        {
            T p1 = TDSContainer::GetNext(p);
            if (p1)
            {
                p = p1;
                continue;
            }
            p1 = TDSContainer::GetChild(p);
            if (p1)
            {
                p = p1;
                continue;
            }
            return p;
        }
    }

    T _ChildGlobalLast(T p)
    {
        T pTmp = TDSContainer::GetChild(p);
        if (pTmp)
            return _GlobalLast(pTmp);
        return 0;
    }

    // enum order: last visible -> last-1 -> last-2 -> ... -> first visible -> 0 -> last visible ...
    T _GlobalPrev(T p, T pRoot)
    {
        DbgAssert(p);

        if (p == pRoot)
            return 0;

        T pTmp = TDSContainer::GetPrev(p);
        if (pTmp)
        {
            T pp = _ChildGlobalLast(pTmp);
            return pp ? pp : pTmp;
        }

        pTmp = TDSContainer::GetParent(p);
        return pTmp; // == pRoot ? 0 : pTmp;
    }


public:
    CTreeItemT (T p, TDSContainer ds = TDSContainer())
        : TDSContainer (ds)
        , m_pItem (p)
    {}

//     CVectorLook GetVectorLook()
//     {
//         return CVectorLook(*this);
//     }

    operator T () const { return m_pItem; }

    T operator -> () const { return m_pItem; }

    T operator * () const { return m_pItem; }

    const T& get_data() const { return m_pItem; }
    T& get_data() { return m_pItem; }

    inline T GetParent()  { DbgAssert(m_pItem); return TDSContainer::GetParent(m_pItem); }
    inline T GetChild()   { DbgAssert(m_pItem); return TDSContainer::GetChild(m_pItem); }
    inline T GetNext()    { DbgAssert(m_pItem); return TDSContainer::GetNext(m_pItem); }
    inline T GetPrev()    { DbgAssert(m_pItem); return TDSContainer::GetPrev(m_pItem); }

    void Child()
    {
        m_pItem = GetChild();
    }

    void Prev ()
    {
        m_pItem = GetPrev();
    }

    void CyclePrev ()
    {
        T p = GetPrev();
        m_pItem = p ? p : GetLast();
    }

    void Next ()
    {
        m_pItem = GetNext();
    }

    // loop traversal in current level
    void CycleNext ()
    {
        DbgAssert(m_pItem);
        T p = GetNext();
        m_pItem = p ? p : GetFirst();
    }

    T GetCycleNext(T pStop)
    {
        DbgAssert(m_pItem);
        T p = GetNext();
        if (p)
            return p != pStop ? p : 0;
        else
        {
            p = GetFirst();
            return (p == m_pItem || p == pStop) ? 0 : p;
        }
    }

    // if current == 0 and pRoot not 0, it will begin a new search
    void GlobalPrev(T pRoot = T())
    {
        DbgAssert(m_pItem || pRoot);
        if (m_pItem)
            m_pItem = _GlobalPrev(m_pItem, pRoot);
        else
        {
            m_pItem = _ChildGlobalLast(pRoot);
            if (!m_pItem)
                m_pItem = pRoot;
        }
    }

    template <class F_Walk>
    void GlobalPrev(T pRoot, F_Walk _fWalk)
    {
        for (; GlobalPrev(pRoot), *this;)
        {
            if (_fWalk(*this))
                return;
        }
    }

    template <class F_Walk>
    void GlobalCyclePrev(T pRoot, F_Walk _fWalk)
    {
        GlobalPrev(pRoot, _fWalk);
        if (!pRoot || *this)
            return;
        DbgAssert(!*this);
        GlobalPrev(pRoot, _fWalk);
    }

//     void GlobalPrev(T pRoot = 0)
//     {
//         typedef CTreeItemT<T, CTreeDataSourceContainerReversed<TDSContainer> > CReversedTreeItem;
//         CReversedTreeItem itm(*this);
//         itm.GlobalNext(pRoot);
//         m_pItem = itm;
//     }

    // if rec == 0, it start from root;
    // when search end, it return 0;
    void GlobalNext(T pRoot = T())
    {
        DbgAssert(m_pItem || pRoot);
        if (!m_pItem)
        {
            m_pItem = pRoot;
            return;
        }
        T pTmp = GetChild();
        if (pTmp)
            m_pItem = pTmp;
        else
            GlobalNextSkipChild(pRoot);
    }

    template <class F_Walk>
    void GlobalNext(T pRoot, F_Walk _fWalk)
    {
        for (; GlobalNext(pRoot), *this;)
        {
            if (_fWalk(*this))
                return;
        }
    }

    // if rec == 0, it start from root;
    // if not find, it restart a new search from root.
    // then if not find, it return 0;
    template <class F_Walk>
    void GlobalCycleNext(T pRoot, F_Walk _fWalk)
    {
        GlobalNext(pRoot, _fWalk);
        if (!pRoot || *this)
            return;
        DbgAssert(!*this);
        GlobalNext(pRoot, _fWalk);
    }

    void GlobalWalk(T pRoot, TMBool bReverse)
    {
        if (bReverse)
            GlobalPrev(pRoot);
        else
            GlobalNext(pRoot);
    }

    template <class F_Walk>
    void GlobalWalk(T pRoot, TMBool bReverse, F_Walk _fWalk)
    {
        if (bReverse)
            GlobalPrev(pRoot, _fWalk);
        else
            GlobalNext(pRoot, _fWalk);
    }

    void GlobalCycleWalk(T pRoot, TMBool bReverse)
    {
        if (bReverse)
            GlobalCyclePrev(pRoot, FTreeWalkStop());
        else
            GlobalCycleNext(pRoot, FTreeWalkStop());
    }

    template <class F_Walk>
    void GlobalCycleWalk(T pRoot, TMBool bReverse, F_Walk _fWalk)
    {
        if (bReverse)
            GlobalCyclePrev(pRoot, _fWalk);
        else
            GlobalCycleNext(pRoot, _fWalk);
    }

//     HTREEITEM NextLogicalItem(HTREEITEM ht, BOOL fNext)
//     {
//         HTREEITEM htNext;
//
//         if (fNext)
//         {
//             //
//             // We see if this item has a child.  If so, we are done.  If not,
//             // we get the next sibling.  If that fails, we move back to the parent,
//             // and try the next sibling thing again.  And so on until we reach the
//             // root.
//             //
//             htNext = TreeView_GetChild(m_hwnd, ht);
//             if (htNext)
//                 return(htNext);
//
//             while (ht)
//             {
//                 htNext = TreeView_GetNextSibling(m_hwnd, ht);
//                 if (htNext)
//                     return(htNext);
//
//                 ht = TreeView_GetParent(m_hwnd, ht);
//             }
//         }
//
//         return(NULL);
//     }
//
//     HTREEITEM COutlineView32::PrevLogicalItem(HTREEITEM ht)
//     {
//         HTREEITEM htPrev;
//
//         //
//         // If this item has no previous sibling return the parent.
//         // Then if the so, see if run done the first children.
//         // Then get the previous sibling has no children return that.
//         // Otherwise march down the tre find the last sibling of the last child
//         //
//         htPrev = TreeView_GetPrevSibling(m_hwnd, ht);
//         if (!htPrev)
//         {
//             return TreeView_GetParent(m_hwnd, ht);
//         }
//         else
//         {
//             HTREEITEM htTest = TreeView_GetChild(m_hwnd, htPrev);
//             if (!htTest)
//             {
//                 return htPrev;
//             }
//             else
//             {
//                 htPrev = htTest;
//                 // We are at the first child of the previous sibling
//                 for (;;)
//                 {
//                     htTest = TreeView_GetNextSibling(m_hwnd, htPrev);
//                     if (!htTest)
//                     {
//                         htTest = TreeView_GetChild(m_hwnd, htPrev);
//                         if (!htTest)
//                             break;
//                     }
//
//                     htPrev = htTest;
//                 }
//
//                 return htPrev;
//             }
//         }
// }

    void GlobalNextSkipChild (const T pRoot = T())
    {
        DbgAssert(m_pItem);
        if (pRoot == m_pItem)
            m_pItem = 0;
        else
        {
            T pTmp = GetNext();
            if (pTmp)
                m_pItem = pTmp;
            else
                _ParentNext(pRoot);
        }
    }

//     void GlobalLast()
//     {
//         DbgAssert(m_pItem);
//         m_pItem = _GlobalLast(m_pItem);
//     }

    T GetRoot()
    {
        CTreeItemT itm(*this);
        for (CTreeItemT itmp(itm.GetParent()); itmp; itmp = itm.GetParent())
            itm = itmp;
        return itm;
    }

    T GetFirst ()
    {
        DbgAssert(m_pItem);
        T pTmp = GetParent();
        if (pTmp)
            return TDSContainer::GetChild(pTmp);
        else
        {
            T pItem = m_pItem;
            for (; pTmp = TDSContainer::GetPrev(pItem); pItem = pTmp)
            {
            }
            return pItem;
        }
    }

    T GetLast ()
    {
        DbgAssert(m_pItem);
        T pItem = m_pItem;
        for (T pTmp; pTmp = TDSContainer::GetNext(pItem); pItem = pTmp)
        {
        }
        return pItem;
    }

    T GetLastChild()
    {
        DbgAssert(m_pItem);
        T item = GetChild();
        if (item)
            return CTreeItemT(item).GetLast();
        return item;
    }

    int GetChildCount ()
    {
        DbgAssert(m_pItem);
        int i = 0;
        for (T pItem = GetChild(); pItem; pItem = TDSContainer::GetNext(pItem), ++i)
        {
        }
        return i;
    }

    int GetAllChildCount ()
    {
        DbgAssert(m_pItem);
        int i = 0;
        for (CTreeItemT ti(GetChild(), *this); ti; ti.Next())
        {
            i += (1 + ti.GetAllChildCount ());
        }
        return i;
    }

    // -1 return last child;
    T GetChildAt(TMUInt32 n)
    {
        CTreeItemT ti(GetChild(), *this);
        for (; ti && (n-- > 0); ti.Next())
        {
        }
        return ti;
    }

    int GetLevel ()
    {
        DbgAssert(m_pItem);
        int i = 0;
        for (T pItem = m_pItem; 0 != (pItem = TDSContainer::GetParent(pItem)); ++i)
        {
        }
        return i;
    }

    bool IsChild (T pChild)
    {
        DbgAssert(pChild);
        DbgAssert(m_pItem);
        for (; 0 != (pChild = TDSContainer::GetParent(pChild));)
        {
            if (pChild == m_pItem)
                return true;
        }
        return false;
    }

    // -1: insert child end
//     void InsertChildAt (T pChild, size_t dwPos)
//     {
//         if (!m_pItem->_child || dwPos == 0)
//         {
//             InsertChildFirst (pChild);
//         }
//         else
//         {
//             T p = m_pItem->_child;
//             for (; p->_next && dwPos > 0; p = p->_next, --dwPos)
//             {
//             }
//
//             DbgAssert(p);
//             CTreeItemT(p, *this).InsertAfter (pChild);
//         }
//     }
//
//     void InsertChildFirst (T pChild)
//     {
//         DbgAssert(m_pItem && pChild);
//         DbgAssert(m_pItem != pChild);
//         DbgAssert(!CTreeItemT(pChild, *this).IsChild(m_pItem));
//
//         _Detach (pChild);
//
//         if (m_pItem->_child)
//         {
//             m_pItem->_child->_prev = pChild;
//         }
//
//         pChild->_next = m_pItem->_child;
//         pChild->_prev = 0;
//         pChild->_parent = m_pItem;
//         m_pItem->_child = pChild;
//     }
//
//     void InsertAfter (T pInsert)
//     {
//         DbgAssert(m_pItem && pInsert);
//         DbgAssert(m_pItem != pInsert);
//         DbgAssert(!CTreeItemT(pInsert, *this).IsChild(m_pItem));
//
//         _Detach (pInsert);
//
//         pInsert->_parent = m_pItem->_parent;
//         pInsert->_next = m_pItem->_next;
//         pInsert->_prev = m_pItem;
//         if (m_pItem->_next)
//         {
//             m_pItem->_next->_prev = pInsert;
//         }
//         m_pItem->_next = pInsert;
//     }
//
//     void InsertBefore (T pInsert)
//     {
//         DbgAssert(m_pItem && pInsert);
//         DbgAssert(m_pItem != pInsert);
//         DbgAssert(!CTreeItemT(pInsert, *this).IsChild(m_pItem));
//
//         _Detach (pInsert);
//
//         pInsert->_parent = m_pItem->_parent;
//         pInsert->_next = m_pItem;
//         pInsert->_prev = m_pItem->_prev;
//         if (m_pItem->_prev)
//             m_pItem->_prev->_next = pInsert;
//         else
//             m_pItem->_parent->_child = pInsert;
//         m_pItem->_prev = pInsert;
//     }
//
//     void Detach ()
//     {
//         DbgAssert(m_pItem);
//
//         _Detach (m_pItem);
//
//         m_pItem->_next = 0;
//         m_pItem->_prev = 0;
//         m_pItem->_parent = 0;
//     }

    template <class F>
    void SortChild (F __f, int nLevel)
    {
        DbgAssert(m_pItem);

        TM::CVector<T> v;

        for (T p1 = m_pItem->_child; NULL != p1; p1 = p1->_next)
        {
            v.binary_insert(p1, __f);
        }

        int sz = v.size();
        if (sz > 1)
        {
            for (int i = sz-2; i >=0; --i)
            {
                v[i]->_next = v[i+1];
                v[i+1]->_prev = v[i];
            }

            v[sz-1]->_next = 0;
            v[0]->_prev = 0;
            m_pItem->_child = v[0];
        }

        if (nLevel != 0)
        {
            for (CTreeItemT ti(m_pItem->_child, *this); ti; ti.Next())
            {
                ti.SortChild (__f, nLevel-1);
            }
        }
    }

    template <class F>
    void SortChildEx (F __f, TMLParam nParam, int nLevel, TMBool bReversed)
    {
        if (bReversed)
        {
            SortChild (CMP1<F, true>(__f, nParam), nLevel);
        }
        else
        {
            SortChild (CMP1<F, false>(__f, nParam), nLevel);
        }
    }

    template <class F>
    void SortChildEx (F __f, int nLevel, TMBool bReversed)
    {
        if (bReversed)
        {
            SortChild (CMP2<F, true>(__f), nLevel);
        }
        else
        {
            SortChild (CMP2<F, false>(__f), nLevel);
        }
    }

    // enum self and all child
    template <class F>
    void Enum (F __f)
    {
        DbgAssert(m_pItem);
        for (CTreeItemT itm(GetChild(), *this); itm; itm.Next())
            itm.Enum(__f);
        __f(*this);
    }

    template <class F, class P>
        void Enum2 (F __f, P param)
    {
        DbgAssert(m_pItem);
        for (CTreeItemT itm(GetChild(), *this); itm; itm.Next())
            itm.Enum2(__f, param);
        __f(*this, param);
    }

    template <class F>
    void DeleteChild (F __f)
    {
        DbgAssert(m_pItem);
        T pItem;
        for (; pItem = m_pItem->GetChild();)
            CTreeItemT(pItem).DeleteSelf(__f);
    }

//     template <class F>
//     void DeleteNext (F __f)
//     {
//         DbgAssert(m_pItem);
//         CTreeItemT(m_pItem->_next, *this).Enum (__f);
//         m_pItem->_next = 0;
//     }
//
//     template <class F>
//     void DeleteCN (F __f)
//     {
//         DeleteChild (__f);
//         DeleteNext (__f);
//     }

    template <class F>
    void DeleteSelf(F __f)
    {
        DbgAssert(m_pItem);
        m_pItem->Detach();
        DeleteChild(__f);
        __f(m_pItem);
    }
};


// template <class T, class TDSContainer = CTreeDataSourceContainer>
// struct CTreeItemRecorderT : public CTreeItemT<T, TDSContainer>
// {
// public:
//     typedef int (*FTreeFlag) (T p, TMLParam lp);
//     using CTreeItemT<T, TDSContainer>::m_pItem;
//     using CTreeItemT<T, TDSContainer>::GetParent;
//     using CTreeItemT<T, TDSContainer>::GetNext;
//     using CTreeItemT<T, TDSContainer>::GetChild;
//
//     int m_nFlag;
//     FTreeFlag m_pfn;
//     TMLParam m_lp;
//
// private:
//     void _MakeFlag()
//     {
//         m_nFlag = m_pfn(m_pItem, m_lp);
//     }
//
//     TMBool _Set(T p)
//     {
//         if (!p)
//             return false;
//
//         m_pItem = p;
//         _MakeFlag();
//         if (!(m_nFlag & eTreeEnumSelf))
//             GlobalNext();
//
//         return true;
//     }
//
//     void _ParentNext()
//     {
//         m_pItem = GetParent();
//         DbgAssert(m_pItem);
//
//         _MakeFlag();
//         if (IsRoot())
//             return;
//
//         if (_Set(GetNext()))
//             return;
//
//         _ParentNext();
//     }
//
// public:
//     CTreeItemRecorderT(FTreeFlag pfn, TMLParam lp)
//         : m_pfn (pfn)
//         , m_lp (lp)
//         , CTreeItemT<T, TDSContainer>(0)
//     {
//     }
//
//     void Assign(T p)
//     {
//         DbgAssert(p);
//         m_pItem = p;
//         _MakeFlag();
//     }
//
//     void AssignAndTest(T p)
//     {
//         DbgAssert(p);
//         _Set(p);
//     }
//
//     void GlobalNext()
//     {
//         DbgAssert(m_pItem);
//         if (m_nFlag & eTreeEnumChild)
//         {
//             if (_Set(GetChild()))
//                 return;
//         }
//         GlobalNextSkipChild();
//     }
//
//     void GlobalNextSkipChild()
//     {
//         DbgAssert(m_pItem);
//         if (IsRoot())
//             return;
//
//         if (_Set(GetNext()))
//             return;
//
//         _ParentNext();
//     }
//
//     TMBool IsRoot() const
//     {
//         return (m_nFlag & (eTreeEnumRoot | eTreeEnumSelf)) == (eTreeEnumRoot | eTreeEnumSelf);
//     }
//
// };
//
// template <class T, class TDSContainer = CTreeDataSourceContainer>
// struct CTreeItemReserveRecorderT : public CTreeItemT<T, TDSContainer>
// {
// public:
//     typedef CTreeItemReserveRecorderT        class_type;
//     typedef int (*FTreeFlag) (T p, TMLParam lp);
//     using CTreeItemT<T, TDSContainer>::m_pItem;
//
//     int m_nFlag;
//     FTreeFlag m_pfn;
//     TMLParam m_lp;
//
// private:
//     void _MakeFlag()
//     {
//         m_nFlag = m_pfn(m_pItem, m_lp);
//     }
//
//     TMBool _Set(T p)
//     {
//         if (!p)
//             return false;
//
//         m_pItem = p;
//         _MakeFlag();
//         if (!(m_nFlag & eTreeEnumSelf))
//             GlobalNext();
//
//         return true;
//     }
//
//     void _GlobalLast(T p)
//     {
//         if (!p)
//             return;
//
//         // check self
//         int nFlag = m_pfn(p, m_lp);
//         if (nFlag & eTreeEnumSelf)
//         {
//             m_pItem = p;
//             m_nFlag = nFlag;
//         }
//
//         // check next
//         class_type cls(m_pfn, m_lp);
//         cls._GlobalLast(TDSContainer::GetNext(p));
//         if (cls)
//         {
//             p = m_pItem = cls.m_pItem;
//             nFlag = m_nFlag = cls.m_nFlag;
//         }
//
//         if (!(nFlag & eTreeEnumChild))
//             return;
//
//         // check child
//         cls.m_pItem = 0;
//         cls._GlobalLast(TDSContainer::GetChild(p));
//         if (cls)
//         {
//             m_pItem = cls.m_pItem;
//             m_nFlag = cls.m_nFlag;
//         }
//     }
//
//     void _ChildGlobalLast()
//     {
//         DbgAssert(m_pItem);
//         if (m_nFlag & eTreeEnumChild)
//             _GlobalLast(TDSContainer::GetChild(m_pItem));
//     }
//
//     // enum order: last visible -> last-1 -> last-2 -> ... -> first visible -> 0 -> last visible ...
//     void _GlobalPrev()
//     {
//         DbgAssert(!IsRoot());
//
//         // find prev
//         T p = TDSContainer::GetPrev(m_pItem);
//         if (p)
//         {
//             int nFlag = m_pfn(p, m_lp);
//             class_type cls(m_pfn, m_lp);
//             cls.Assign(p);
//             cls._ChildGlobalLast();
//             if (cls != p)
//             {
//                 m_pItem = cls;
//                 m_nFlag = cls.m_nFlag;
//                 return;
//             }
//
//             if (nFlag & eTreeEnumSelf)
//             {
//                 m_pItem = p;
//                 m_nFlag = nFlag;
//                 return;
//             }
//
//             cls._GlobalPrev();
//             if (cls != p)
//             {
//                 m_pItem = cls;
//                 m_nFlag = cls.m_nFlag;
//                 return;
//             }
//         }
//
//         // find parent
//         Assign(TDSContainer::GetParent(m_pItem));
//         if (m_nFlag & (eTreeEnumSelf | eTreeEnumRoot))
//             return;
//
//         _GlobalPrev();
//     }
//
// public:
//     CTreeItemReserveRecorderT(FTreeFlag pfn, TMLParam lp)
//         : m_pfn (pfn)
//         , m_lp (lp)
//         , CTreeItemT<T, TDSContainer>(0)
//     {
//     }
//
//     void Assign(T p)
//     {
//         DbgAssert(p);
//         m_pItem = p;
//         _MakeFlag();
//     }
//
//     void AssignAndTest(T p)
//     {
//         DbgAssert(p);
//         _Set(p);
//     }
//
//     // if current == 0 and pRoot not 0, it will begin a new search
//     void GlobalNext()
//     {
//         DbgAssert(m_pItem);
//         if (IsRoot())
//             _ChildGlobalLast();
//         else
//            _GlobalPrev();
//     }
//
//     TMBool IsRoot() const
//     {
//         return (m_nFlag & (eTreeEnumRoot | eTreeEnumSelf)) == (eTreeEnumRoot | eTreeEnumSelf);
//     }
//
// };
//
//
// template <class T, class TDSContainer>
// struct CVectorLookRecorderByTree
// {
// public:
//     typedef CTreeItemT<T, TDSContainer>                 iterator;
//     typedef CVectorLookRecorderByTree                   class_type;
//
//     iterator m_root;
//     iterator m_item;
//
// public:
//     CVectorLookRecorderByTree (const iterator& itm, const iterator& root)
//         : m_root(root)
//         , m_item(itm)
//     {}
//
//     CVectorLookRecorderByTree (const iterator& itm)
//         : m_root(itm)
//         , m_item(itm)
//     {}
//
//     T operator -> () const { return m_item; }
//     T operator * () const { return m_item; }
//
//     class_type& operator++ ()
//     {
//         m_item.GlobalNext(m_root);
//         return *this;
//     }
//
//     class_type& operator-- ()
//     {
//         m_item.GlobalPrev(m_root);
//         return *this;
//     }
//
//     int operator - (CVectorLookRecorderByTree rhs)
//     {
//         DbgAssert(rhs.m_root == m_root);
//
//         int i = 0;
//         iterator itemTmp = rhs.m_item;
//         for (; rhs.m_item != m_item; ++i)
//         {
//             ++rhs;
//             DbgAssert(itemTmp != rhs.m_item);
//         }
//         return i;
//     }
// };
//
// template <class T, class TDSContainer>
// struct CVectorLookByTree : public CTreeItemT<T, TDSContainer>
// {
// public:
//     typedef CTreeItemT<T, TDSContainer>                 base_class;
//     typedef base_class                                  iterator;
//     typedef CVectorLookRecorderByTree<T, TDSContainer>  recorder;
//     using base_class::m_pItem;
//     using base_class::GetAllChildCount;
//
// public:
//     CVectorLookByTree (const base_class& rhs)
//         : base_class(rhs)
//     {}
//
//     iterator begin()
//     {
//         return m_pItem;
//     }
//
//     iterator end()
//     {
//         iterator iter(*this);
//         iter.GlobalLast();
//         return iter;
//     }
//
//     size_t size() const
//     {
//         return GetAllChildCount();
//     }
//
//     iterator operator [] (int n)
//     {
//         iterator iter(*this);
//         for (int i = 0; i < n; ++i)
//             iter.GlobalNext();
//         return iter;
//     }
// };



// T : CTreeItemT
// template <class T, TMBool bReverse>
// class CTreeGlobalRecorder
// {
// private:
//     T m_root;
//     T m_current;
//
// private:
//     void next()
//     {
//         if (bReverse)
//             m_current.GlobalNext(m_root);
//         else
//             m_current.GlobalPrev(m_root);
//     }
//
//     void prev()
//     {
//         if (bReverse)
//             m_current.GlobalPrev(m_root);
//         else
//             m_current.GlobalNext(m_root);
//     }
//
// public:
//     CTreeGlobalRecorder(T current, T root)
//         : m_root (root)
//         , m_current (current)
//     {
//     }
//
//     operator TMBool () const { return !eof(); }
//
//     TMBool operator ! () const { return eof(); }
//
//     iterator operator -> () const { return m_iter; }
//
//     class_type& operator++ () {
//         DbgAssert(m_iter != _getend());  ++m_iter;  return *this; }
//
//     class_type& operator-- () {
//         DbgAssert(m_iter != _getbegin());  --m_iter;  return *this; }
//
//     class_type& operator += (size_t n) {
//         m_iter += n; return *this; }
//
//     reference operator* () { return *m_iter; }
// };

template <class T> struct CTreeNodeEndT;

template <class T> struct CTreeNodeBaseT
{
    typedef T                                       class_type;
    class_type _parent;
    class_type _child;
    class_type _next;
    class_type _prev;

    CTreeNodeBaseT()
        : _parent(0)
        , _child (0)
        , _next (0)
        , _prev (0)
    {
    }
};

template <class T, class T_Base = CTreeNodeBaseT<T> > struct CTreeNodeT : public T_Base
{
public:
    typedef T                                       class_type;
    typedef CTreeItemT<T>                           iterator;
    typedef CTreeNodeEndT<T>                        CTreeNodeEnd;

    using T_Base::_parent;
    using T_Base::_child;
    using T_Base::_next;
    using T_Base::_prev;

    class_type GetChild()
    {
        return _child;
    }

    class_type GetThis()
    {
        return static_cast<T>(this);
    }

    iterator GetSelf()
    {
        return static_cast<T>(this);
    }

    class_type GetParent()
    {
        DbgAssert(DbgIsPtr(_parent));
        return _parent;
    }

    class_type GetNext()
    {
        DbgAssert(DbgIsPtr(_next));
        return _next;
    }

    class_type GetPrev()
    {
        DbgAssert(DbgIsPtr(_prev));
        return _prev;
    }

    // break trees -> iterator; don't break iterator -> tree.
    // Detach(c,e) - result:
    // c keep old parent, prev, next.
    // p                 p              p
    // |                 |              ^
    // a-b-c-d-e         a-b-e        b<c-d>e
    // | | |             | |     +      |
    // e f g-h           e f            g-h
    static void _Detach(class_type pBgn, class_type pEnd)
    {
        DbgAssert(pBgn);
        if (pBgn->_prev)
        {
            pBgn->_prev->_next = pEnd;
        }
        else if (pBgn->_parent && pBgn->_parent->_child == pBgn)
        {
//            DbgAssert(pBgn->_parent->_child == pBgn);
            pBgn->_parent->_child = pEnd;
        }
        if (pEnd)
        {
            pEnd->_prev = pBgn->_prev;
        }
    }

    // Detach(c) - result:
    // -->next
    // p                 p             p
    // |                 |             ^
    // a-b-c-d           a-b-d       b<c>d
    // | | |             | |     +     |
    // e f g-h           e f          g-h
    static void _Detach(class_type pItem)
    {
        _Detach(pItem, pItem->_next);
        if (pItem->_parent)
            DbgAssert(pItem->_parent->_child != pItem);
    }

    void Detach()
    {
        _Detach(GetThis());
        this->_next = 0;
        this->_prev = 0;
        this->_parent = 0;
    }

    void DetachSimple()
    {
        _Detach(GetThis());
    }

    // new parent must have no childs
    // Split(c) - result:
    // -->next
    // a-b-c-d           a-b         c-d
    // | | |             | |     +   |
    // e f g-h           e f         g-h
    void Split(T newParent = 0)
    {
        if (_prev)
            _prev->_next = 0;
        else if (_parent && _parent->_child == this)
            _parent->_child = 0;
        _prev = 0;
        for (T p = GetThis(); p; p = p->_next)
            p->_parent = newParent;

        if (newParent)
        {
            DbgAssert(!newParent->_child);
            newParent->_child = GetThis();
        }
    }

    // b.Appends(c)
    // -->next
    // a-b         c-d           a-b-c-d
    // | |     +   |       =     | | |
    // e f         g-h           e f g-h
//     void Appends(class_type pInserts)
//     {
//         DbgAssert(!_next);
//         DbgAssert(!pInsert->_prev);
//         _next = pInserts;
//         pInserts->_prev = this;
//     }

    // c.SetChilds(g)
    // -->next
    // a-b-c-d     g-h          a-b-c-d
    // | |     +          =     | | |
    // e f                      e f g-h
//     void SetChilds(class_type pChilds)
//     {
//         DbgAssert(!_child);
//         DbgAssert(!pChilds->_parent);
//         DbgAssert(!pChilds->_prev);
//         _child = pChilds;
//         pChilds->_parent = GetThis();
//     }

    void InsertBefore(class_type pInsert)
    {
        DbgAssert(pInsert);
        DbgAssert(this != pInsert);
        DbgAssert((!pInsert->GetSelf().IsChild(GetThis())));

        _Detach(pInsert);

        pInsert->_parent = this->_parent;
        pInsert->_next = GetThis();
        pInsert->_prev = this->_prev;
        if (this->_prev)
            this->_prev->_next = pInsert;
        else
            this->_parent->_child = pInsert;
        this->_prev = pInsert;
    }

    class_type _MakeParentOnInserts(class_type pBegin, class_type pEnd)
    {
        DbgAssert(pBegin != pEnd);

        class_type pParent2 = pBegin->_parent;
        class_type pLast;
        for (; ;)
        {
            DbgAssert(pBegin->_parent == pParent2);
            pBegin->_parent = _parent;
            pLast = pBegin;
            pBegin = pBegin->_next;
            if (pBegin == pEnd)
                return pLast;
        }
    }

    void InsertBefores(class_type pBegin, class_type pEnd)
    {
        if (pBegin == pEnd || !pBegin)
            return;

        DbgAssert(this != pBegin);
        DbgAssert((!pBegin->GetSelf().IsChild(GetThis())));

        _Detach(pBegin, pEnd);

        class_type pLast = _MakeParentOnInserts(pBegin, pEnd);
        pLast->_next = GetThis();
        pBegin->_prev = this->_prev;
        if (this->_prev)
            this->_prev->_next = pBegin;
        else
            this->_parent->_child = pBegin;
        this->_prev = pLast;
    }

    void InsertAfter(class_type pInsert)
    {
        DbgAssert(pInsert);
        DbgAssert(this != pInsert);
        DbgAssert(!pInsert->GetSelf().IsChild(GetThis()));

        _Detach(pInsert);

        pInsert->_parent = this->_parent;
        pInsert->_next = this->_next;
        pInsert->_prev = GetThis();
        if (this->_next)
            this->_next->_prev = pInsert;
        this->_next = pInsert;
    }

    // pBegin and pEnd must have same parent
    // c + g-h-j
    // p           q            p             q
    // |           |            |             |
    // a-b-c-d     g-h-j        a-b-c-g-h-d   j
    // | |     +   |       =    | |   |
    // e f         i            e f   i
    void InsertAfters(class_type pBegin, class_type pEnd)
    {
        if (pBegin == pEnd || !pBegin)
            return;

        DbgAssert(this != pBegin);
        DbgAssert(!pBegin->GetSelf().IsChild(GetThis()));

        _Detach(pBegin, pEnd);

        class_type pLast = _MakeParentOnInserts(pBegin, pEnd);
        pLast->_next = this->_next;
        pBegin->_prev = GetThis();
        if (this->_next)
            this->_next->_prev = pLast;
        this->_next = pBegin;
    }

    void InsertChildFirst(class_type pChild)
    {
        DbgAssert(pChild);
        DbgAssert(this != pChild);
        DbgAssert(!pChild->GetSelf().IsChild(GetThis()));

        _Detach(pChild);

        if (this->_child)
            this->_child->_prev = pChild;

        pChild->_next = this->_child;
        pChild->_prev = 0;
        pChild->_parent = GetThis();
        this->_child = pChild;
    }

    void InsertChildAt(class_type pChild, size_t dwPos)
    {
        if (!this->_child || dwPos == 0)
        {
            this->InsertChildFirst(pChild);
        }
        else
        {
            class_type p = this->_child;
            for (--dwPos; p->_next && dwPos > 0; p = p->_next, --dwPos)
            {
            }

            DbgAssert(p);
            if (p != pChild)
                p->InsertAfter(pChild);
        }
    }
};

template <class T>
struct CTreeNodeEndT : public CTreeNodeT<T>
{
    using CTreeNodeT<T>::GetSelf;
    using CTreeNodeT<T>::_next;
    using CTreeNodeT<T>::_child;
    using CTreeNodeT<T>::Detach;

    CTreeNodeEndT(CTreeNodeT<T>& parent)
    {
        parent.InsertChildAt(GetSelf(), -1);
    }

    ~CTreeNodeEndT()
    {
        DbgAssert(!_next);
        DbgAssert(!_child);
        Detach();
    }
};

// template <class T> struct CTreeNode2T
// {
// public:
//     typedef T                                       class_type;
//     typedef CTreeItemT<T>                           iterator;
//
// public:
//     class_type _parent;
//     class_type _child;
//     class_type _next;
//     class_type _prev;
//
// public:
//     CTreeNode2T()
//         : _parent(0)
//         , _child (0)
//         , _next (0)
//         , _prev (0)
//     {
//     }
//
//     class_type GetChild()
//     {
//         return _child;
//     }
//
//     class_type GetThis()
//     {
//         return static_cast<T>(this);
//     }
//
//     iterator GetSelf()
//     {
//         return static_cast<T>(this);
//     }
//
//     class_type GetParent()
//     {
//         return _parent;
//     }
//
//     class_type GetNext()
//     {
//         return _next;
//     }
//
//     class_type GetPrev()
//     {
//         return _prev;
//     }
//
//     // Detach(c) - result:
//     // -->next
//     // a-b-c-d           a-b-d       c
//     // | | |             | |     +   |
//     // e f g-h           e f         g-h
//     static void _Detach(CTreeNodeT* pItem)
//     {
//         if (pItem->_prev)
//         {
//             pItem->_prev->_next = pItem->_next;
//         }
//         else if (pItem->_parent && pItem->_parent->_child == pItem)
//         {
//             DbgAssert(pItem->_parent->_child == pItem);
//             pItem->_parent->_child = pItem->_next;
//         }
//         if (pItem->_next)
//         {
//             pItem->_next->_prev = pItem->_prev;
//         }
//     }
//
//     void Detach()
//     {
//         _Detach(this);
//         this->_next = 0;
//         this->_prev = 0;
//         this->_parent = 0;
//     }
//
//     void InsertBefore(class_type pInsert)
//     {
//         DbgAssert(this && pInsert);
//         DbgAssert(this != pInsert);
//         DbgAssert((!pInsert->GetSelf().IsChild(GetThis())));
//
//         _Detach(pInsert);
//
//         pInsert->_parent = this->_parent;
//         pInsert->_next = GetThis();
//         pInsert->_prev = this->_prev;
//         if (this->_prev)
//             this->_prev->_next = pInsert;
//         else
//             this->_parent->_child = pInsert;
//         this->_prev = pInsert;
//     }
//
//     void InsertAfter(class_type pInsert)
//     {
//         DbgAssert(this && pInsert);
//         DbgAssert(this != pInsert);
//         DbgAssert(!pInsert->GetSelf().IsChild(GetThis()));
//
//         _Detach(pInsert);
//
//         pInsert->_parent = this->_parent;
//         pInsert->_next = this->_next;
//         pInsert->_prev = GetThis();
//         if (this->_next)
//             this->_next->_prev = pInsert;
//         this->_next = pInsert;
//     }
//
//     void InsertChildFirst(class_type pChild)
//     {
//         DbgAssert(this && pChild);
//         DbgAssert(this != pChild);
//         DbgAssert(!pChild->GetSelf().IsChild(GetThis()));
//
//         _Detach(pChild);
//
//         if (this->_child)
//             this->_child->_prev = pChild;
//
//         pChild->_next = this->_child;
//         pChild->_prev = 0;
//         pChild->_parent = GetThis();
//         this->_child = pChild;
//     }
//
//     void InsertChildAt(class_type pChild, size_t dwPos)
//     {
//         if (!this->_child || dwPos == 0)
//         {
//             this->InsertChildFirst(pChild);
//         }
//         else
//         {
//             class_type* p = this->_child;
//             for (--dwPos; p->_next && dwPos > 0; p = p->_next, --dwPos)
//             {
//             }
//
//             DbgAssert(p);
//             p->InsertAfter (pChild);
//         }
//     }
// };

////////////////////////////////////////////////////////////////////////////////
// binary tree
// struct binaryTreeHelper
// {
// protected:
//     template < class T >
//     inline static T* GetFirst (T* pItem)
//     {
//         DbgAssert(pItem);
//
//         for (; ;)
//         {
//             T* pp = pItem->_parent;
//             if (!pp)
//                 break;
//             if (pp->_parent->_next == pItem)
//                 continue;
//             pItem = pp;
//             break;
//         }
//         return pItem;
//     }
//
//     template < class T >
//     inline static T* GetLast (T* pItem)
//     {
//         DbgAssert(pItem);
//
//         for (; pItem->_next; pItem = pItem->_next);
//         return pItem;
//     }
//
//     template < class T >
//         inline static int GetChildCount (T* pItem)
//     {
//         DbgAssert(pItem);
//
//         pItem = pItem->_child;
//         int i = 0;
//         for (; pItem; pItem = pItem->_next)
//             ++i;
//         return i;
//     }
//
//     template < class T >
//         inline static int GetAllChildCount (T* pItem)
//     {
//         DbgAssert(pItem);
//
//         pItem = pItem->_child;
//         int i = 0;
//         for (; pItem; pItem = pItem->_next)
//             i += (1 + GetAllChildCount (pItem));
//         return i;
//     }
//
//     template < class T >
//         inline static int GetLevel (T* pItem)
//     {
//         DbgAssert(pItem);
//         int i = 0;
//         for (; pItem = pItem->_parent; ++i);
//         return i;
//     }
//
//     template < class T >
//     inline static void InsertChildFirst (T* pParent, T* pInsert)
//     {
//         DbgAssert(pParent && pInsert);
//
//         if (pParent->_child)
//             pParent->_child->_prev = pInsert;
//         pInsert->_next = pParent->_child;
//         pInsert->_prev = 0;
//         pInsert->_parent = pParent;
//         pParent->_child = pInsert;
//     }
//
//     template < class T >
//     inline static void InsertChildLast (T* pParent, T* pInsert)
//     {
//         DbgAssert(pParent && pInsert);
//
//         if (!pParent->_child)
//             InsertChildFirst (pParent, pInsert);
//         else
//         {
//             pParent = GetLast (pParent->_child);
//             InsertAfter (pParent, pInsert);
//         }
//     }
//
//     template < class T >
//     inline static void InsertAfter (T* pItem, T* pInsert)
//     {
//         DbgAssert(pItem && pInsert);
//
//         pInsert->_parent = pItem->_parent;
//         pInsert->_next = pItem->_next;
//         pInsert->_prev = pItem;
//         if (pItem->_next)
//             pItem->_next->_prev = pInsert;
//         pItem->_next = pInsert;
//     }
//
//     template < class T >
//     inline static void InsertBefore (T* pItem, T* pInsert)
//     {
//         DbgAssert(pItem && pInsert);
//
//         pInsert->_parent = pItem->_parent;
//         pInsert->_next = pItem;
//         pInsert->_prev = pItem->_prev;
//         if (pItem->_prev)
//             pItem->_prev->_next = pInsert;
//         else
//             pItem->_parent->_child = pInsert;
//         pItem->_prev = pInsert;
//     }
//
//     template < class T >
//     inline static void Detach (T* pItem)
//     {
//         DbgAssert(pItem);
//
//         if (pItem->_prev)
//             pItem->_prev->_next = pItem->_next;
//         else if (pItem->_parent)
//         {
//             DbgAssert(pItem->_parent->_child == pItem);
//             pItem->_parent->_child = pItem->_next;
//         }
//         if (pItem->_next)
//             pItem->_next->_prev = pItem->_prev;
//
//         pItem->_next = 0;
//         pItem->_prev = 0;
//         pItem->_parent = 0;
//     }
// };

// root item: 0 - prev - self - last - 0;
// other item: last - self - next - last;
// template <class T>
// class CDomTreeNodeT
// {
// public:
//     T* m_pNodeParent;
//     T* m_pNodeChild;
//     T* m_pNodePrev;
//     T* m_pNodeNext;
//
// private:
//     void InsertChildFirst(T* pChild)
//     {
//         DbgAssert(pChild);
//         DbgAssert(this != pChild);
//         DbgAssert(!pChild->get_self().IsChild(get_self()));
//
//         _Detach(pChild);
//
//         if (this->m_pNodeChild)
//             this->m_pNodeChild->_prev = pChild;
//
//         pChild->m_pNodeNext = this->m_pNodeChild;
//         pChild->m_pNodePrev = 0;
//         pChild->m_pNodeParent = get_self();
//         this->m_pNodeChild = pChild;
//     }
//
//     void InsertChildAt(class_type pChild, size_t dwPos)
//     {
//         if (!this->_child || dwPos == 0)
//         {
//             this->InsertChildFirst(pChild);
//         }
//         else
//         {
//             class_type p = this->_child;
//             for (--dwPos; p->_next && dwPos > 0; p = p->_next, --dwPos)
//             {
//             }
//
//             DbgAssert(p);
//             if (p != pChild)
//                 p->InsertAfter(pChild);
//         }
//     }
//
//     TMBool _IsLastNode() const
//     {
//         return !m_pNodeNext || (DbgAssert(m_pNodeParent), m_pNodeParent->m_pNodeChild == m_pNodeNext);
//     }
//
//     TMBool _IsFirstNode() const
//     {
//         return !m_pNodePrev || (DbgAssert(m_pNodeParent), m_pNodeParent->m_pNodeChild == this);
//     }
//
// private:
//     T* get_self() { return static_cast<T*>(this); }
//     T* get_parentNode() { return m_pNodeParent; }
//     T* get_firstChild() { return m_pNodeChild; }
//     T* get_lastChild() { return (m_pNodeChild && m_pNodeChild != m_pNodeChild->m_pNodePrev) ? m_pNodeChild->m_pNodePrev : 0; }
//     T* get_nextSibling() { return _IsLastNode() ? 0 : (DbgAssert(m_pNodeNext), m_pNodeNext); }
//     T* get_previousSibling() { return _IsFirstNode() ? 0 : (DbgAssert(m_pNodePrev), m_pNodePrev); }
//     void insertBefore(T* pInsert)
//     {
//         DbgAssert(this && pInsert);
//         DbgAssert(this != pInsert);
//         DbgAssert((!pInsert->get_self().IsChild(get_self())));
//
//         _Detach(pInsert);
//
//         pInsert->m_pNodeParent = this->m_pNodeParent;
//         pInsert->m_pNodeNext = get_self();
//         pInsert->m_pNodePrev = this->m_pNodePrev;
//         if (this->m_pNodePrev)
//             this->m_pNodePrev->m_pNodeNext = pInsert;
//         else
//             this->m_pNodeParent->m_pNodeChild = pInsert;
//         this->m_pNodePrev = pInsert;
//     }
//
//     T* appendChild(T* child)
//     {
//
//     }
//
// };
//
//
// template <class T>
// class CDomTreeItemT
// {
//     typedef T* m_pItem;
//
//     CDomTreeItemT(T* pItem = 0) : m_pItem(pItem) {}
//     T* get_firstChild() { return m_pItem->m_pNodeChild; }
//     T* get_lastChild() { return m_pItem->m_pNodeChild; }
// };


TM_END_NAMESPACE

#endif // TM_CPP

#endif // TMTREE_H
