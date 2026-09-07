/*
 *  @file
 *  @brief
 *    call back manage
 *    this class can make safe remove callback when for_each(CCallbackSet)
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.6.28
 *
 *  Copyright (C) 2018 miragekiller
 */

// sample code:
// class CHisSinkList : public TM::CCallSetT<TM::CCallProcT<IHisDataSink*> >
// {
// public:
//     ~CHisSinkList() { Clear(); }
//     
//     void done(CServer* ps, int nStep)
//     {
//         CPrepareCallChain prepare(*this);
//         CALLSET_BEGIN_CALL(0);
//         p->OnHisData(ps, nStep);
//         CALLSET_END_CALL();
//     }
// };

#pragma once

#ifndef TMCALLSET_H
#define TMCALLSET_H

#include "tmlist.h"


TM_BGN_NAMESPACE

template <class T> struct CCallProcT
{
    typedef T value_type;
    T m_callback;

    CCallProcT(T p = 0) : m_callback(p) {}
    bool IsDeleted() const { return !m_callback; }
    void SetDeleted() { m_callback = 0; }
    T operator -> () const { return m_callback; }
    operator T () const { return m_callback; }
    bool operator == (const T rhs) const { return m_callback == rhs; }
};


template <class T>
struct CPrepareCallNextT
{
    int n;
    T& callset;
    CPrepareCallNextT(T& cs) : callset(cs) { n = cs.m_nCurrentFrame; }
    ~CPrepareCallNextT() { callset.m_nCurrentFrame = n; }
};

template <class T>
struct CPrepareCallChainT
{
    int n;
    T& callset;
    CPrepareCallChainT(T& cs) : callset(cs) { n = cs.m_nCurrentFrame; cs.m_nCurrentFrame = cs.m_aPorc.size(); }
    ~CPrepareCallChainT() { callset.m_nCurrentFrame = n; callset.TryDelete(); }
};

// P: CCallProcT<X>
template <class P>
class CCallSetT
{
public:
    typedef P                                   PROC;
    typedef typename PROC::value_type           value_type;
    typedef TM::CVector<PROC>                   CProcList;
    typedef typename CProcList::recorder        CProcRec;
    typedef CPrepareCallNextT<CCallSetT<P> >    CPrepareCallNext;
    typedef CPrepareCallChainT<CCallSetT<P> >   CPrepareCallChain;
    friend CPrepareCallNext;
    friend CPrepareCallChain;

public:
    CProcList   m_aPorc;
    int         m_nCurrentFrame; //m_nCurrentFrame == -1 means have no message in process.
    bool        m_bDeleteOne;

protected:
    void SetDelete(PROC& proc)
    {
        proc.SetDeleted();
        m_bDeleteOne = true;
    }
    
    void TryDelete()
    {
        if (m_nCurrentFrame != -1 || !m_bDeleteOne)
            return;
        m_bDeleteOne = false;
        for (CProcRec rec(m_aPorc); rec;)
        {
            PROC& p = *rec;
            if (p.IsDeleted())
                rec.erase();
            else
                ++rec;
        }
    }

public:
    CCallSetT()
        : m_nCurrentFrame (-1)
        , m_bDeleteOne (false)
    {
    }
    
    ~CCallSetT()
    {
        DbgAssert(0 == m_aPorc.size());
    }

    CProcRec GetProcs() { return m_aPorc; }

//     TMBool IsValid() const { return m_aPorc.size(); }

    bool Find(const value_type& proc)
    {
        CProcRec rec(m_aPorc);
        rec.find(proc);
        return rec;
    }

    bool Append(const value_type& proc)
    {
        TMBool b = Erase(proc);
        DbgAssert(!b);
        m_aPorc.push_back(proc);
        return true;
    }

    bool SafeAppend(const value_type& proc)
    {
        if (!Find(proc))
        {
            m_aPorc.push_back(proc);
            return true;
        }
        return false;
    }
    
    bool Erase(const value_type& proc)
    {
        CProcRec rec(m_aPorc);
        rec.find(proc);
        if (!rec)
            return false;
        if (m_nCurrentFrame == -1)
            rec.erase();
        else
            SetDelete(*rec);
        return true;
    }

    int GetSize()
    {
        if (m_bDeleteOne)
        {
            int i = 0;
            for (CProcRec rec(m_aPorc); rec; ++rec)
            {
                PROC& p = *rec;
                if (!p.IsDeleted())
                    ++i;
            }
            return i;
        }
        else
            return m_aPorc.size();
    }

    bool IsCalling() const
    {
        return m_nCurrentFrame != -1;
    }

    void Clear()
    {
        if (IsCalling())
        {
            for (CProcRec rec(m_aPorc); rec; ++rec)
            {
                PROC& p = *rec;
                p.SetDeleted();
            }
            m_bDeleteOne = true;
        }
        else
            m_aPorc.clear();
    }
};

#if TM_CPP >= TM_CPP11
#   define _DECLARE_PROC() using PROC = typename base_class::PROC
#else
#   define _DECLARE_PROC()
#endif

#define CALLSET_BEGIN_CALL(nStop) \
    for (; --this->m_nCurrentFrame >= nStop;) \
    { \
        _DECLARE_PROC(); \
        PROC& p = this->m_aPorc[this->m_nCurrentFrame]; \
        if (!p.IsDeleted()) \
        {

#define CALLSET_END_CALL()  }}; this->m_nCurrentFrame = -1; 


#define CALLSET_BEGIN_CALL_EX(CSET,cset,nStop) \
    CSET::CPrepareCallChain _prepare(cset); \
    for (; --(cset).m_nCurrentFrame >= nStop;) \
    { \
        CSET::PROC& p = (cset).m_aPorc[(cset).m_nCurrentFrame]; \
        if (!p.IsDeleted()) \
        {

#define CALLSET_END_CALL_EX()  }}; //(cset).m_nCurrentFrame = -1; 


TM_END_NAMESPACE

#endif // TMCALLSET_H
