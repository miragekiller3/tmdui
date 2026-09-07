/*
 *  @file
 *  @brief  subclass widget & window
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2010.11.11
 *
 *  Copyright (C) 2010 miragekiller
 */

#pragma once

#ifndef SUBCLASS_H
#define SUBCLASS_H

#include "../tmcallset.h"


TM_BGN_NAMESPACE

class CSaveStream;
class CLoadStream;
class CEditStream;

template <class T>
struct CMsgProcT : public CMsgDelegateT<T>
{
public:
    typedef CMsgDelegateT<T> delegate_class;
    typedef CMsgProcT<T> value_type;
    CMsgProcT(const delegate_class& rhs)
        : delegate_class (rhs)
    {
    }
    TMUInt8 IsDeleted() const { return !this->pfn; }
    void SetDeleted() { this->pfn = 0; }
};


template <class H>
class CSubClassMgr : public CCallSetT<CMsgProcT<H> >
{
public:
    typedef CMsgDelegateT<H>         delegate_class;
    typedef CCallSetT<CMsgProcT<H> >    base_class;

private:
    void DGSubclass(H hOwner, const delegate_class& dg, HTUI hWnd)
    {
        TMResult hr;
        dg.pfn(dg.pVoid, hOwner, CM_SUBCLASS, (TMLParam)dg.pVoid, (TMWParam)hWnd, hr, 0);
    }

    TMBool Call(H hWnd, TMUInt uMsg, TMWParam wp, TMLParam lp, TMResult& hr, int nStop = 0)
    {
        CALLSET_BEGIN_CALL(nStop);
            TMBool bHandled;
//             DbgAssert(uMsg != CM_SUBCLASS);
            bHandled = p.pfn(p.pVoid, hWnd, uMsg, wp, lp, hr, 0);
            if (uMsg == WM_NCDESTROY)
            {
                if (!p.IsDeleted())
                {
                    DGSubclass(hWnd, p, 0);
                    this->SetDelete(p);
                }
            }
            else if (bHandled)
                return true;
        CALLSET_END_CALL();
        return false;
    }

public:
    TMResult SendProcMessage(H hWnd, TMUInt uMsg, TMWParam wp, TMLParam lp, int nStop = 0)
    {
        typename base_class::CPrepareCallChain prepare(*this);
        TMResult hr = 0;
        Call(hWnd, uMsg, wp, lp, hr, nStop);
        return hr;
    }

    void CallNext(H hWnd, TMUInt uMsg, TMWParam wp, TMLParam lp, TMResult& hr)
    {
        typename base_class::CPrepareCallNext prepare(*this);
        Call(hWnd, uMsg, wp, lp, hr);
    }

    void Subclass(H hWnd, const delegate_class& proc, HTUI hWgt)
    {
        this->Append(proc);
        DGSubclass(hWnd, proc, hWgt);
    }

    bool Unsubclass(H hWnd, const delegate_class& proc)
    {
        bool b;
        if ((b = this->Erase(proc))) // && !proc.IsFunction())
            DGSubclass(hWnd, proc, 0);
        return b;
    }

    void ForceClear(H hWnd)
    {
#ifndef TUI_EDIT_MODE
        DbgAssert(0 == this->m_aPorc.size());
#endif
        DbgAssert(-1 == this->m_nCurrentFrame);
        typename base_class::CProcList pl;
        pl.swap(this->m_aPorc);
        for (typename base_class::CProcRec rec(pl); rec; ++rec)
        {
            typename base_class::PROC& pr = rec.get_data();
            if (!pr.IsDeleted())
                DGSubclass(hWnd, pr, 0);
        }
    }
};

TM_END_NAMESPACE

#endif // SUBCLASS_H
