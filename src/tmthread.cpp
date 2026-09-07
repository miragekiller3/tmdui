/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2019.04.11
 *
 *  Copyright (C) 2019 miragekiller
 */

#include "tmthread.h"
#include "tmmessagequeue.h"

class _CMsgWindow
    : public TM::CStaticBaseRefT2<_CMsgWindow, true, TM::CMessageQueueT<_CMsgWindow> >
{
private:
    enum { WM_SYNC_CALL     = WM_USER + 0x3b7 };    // 951, PFNCall1
    enum { WM_SYNC_CALLEX   = WM_USER + 0x3b8 };    // 952, PFNCallEx sync
    enum { WM_SYNC_CALLEXA  = WM_USER + 0x3b9 };    // 953, PFNCallEx async
    enum { WM_SYNC_LAST     = WM_USER + 0x3ba };

public:
    typedef TM::CMultiMap<void*, ICallBaseAsync*> CTokenList;    // token, ICallBaseAsync
    TM::CCritical   m_critical;
    CTokenList      m_aToken;

    static TMCStr GetWndClassName() { return TM_T("TM window for thread"); }

    TMResult WndProc(TMUInt uMsg, TMWParam wParam, TMLParam lParam)
    {
        if (uMsg == WM_SYNC_CALL)
            return OnCall1(wParam, lParam);
        else if (uMsg == WM_SYNC_CALLEX)
            return OnCallExSync(wParam, lParam);
        else if (uMsg == WM_SYNC_CALLEXA)
            return OnCallExAsync(wParam, lParam);
        else if (uMsg == WM_NCDESTROY)
            Flush(0, false);
        return message_queue_class::WndProc(uMsg, wParam, lParam);
    }

    TMResult OnCall1(TMWParam wParam, TMLParam lParam)
    {
        PFNCall1 pf = (PFNCall1)wParam;
        pf((void*)lParam);
        return true;
    }

    TMResult OnCallExSync(TMWParam wParam, TMLParam lParam)
    {
        ICallBase* pf = (ICallBase*)wParam;
        pf->done();
        return true;
    }

    TMResult OnCallExAsync(TMWParam wParam, TMLParam lParam)
    {
        ICallBaseAsync* pf = (ICallBaseAsync*)wParam;
        if (lParam)
        {
            m_critical.Lock();
            CTokenList::iterator iter = m_aToken.find_by((void*)lParam, pf);
            if (iter == m_aToken.end())
            {
                m_critical.Unlock();
                return true;
            }
            else // have been flush;
            {
                m_aToken.erase(iter);
                m_critical.Unlock();
            }
        }
        pf->done(false);
        delete pf;
        return true;
    }

    void Flush(void* token, TMBool bCancel)
    {
        _CheckTID();

        if (token)
        {
            m_critical.Lock();
            std::pair<CTokenList::iterator, CTokenList::iterator> range = m_aToken.equal_range(token);
            CTokenList tmp;
            tmp.insert(range.first, range.second);
            m_aToken.erase(range.first, range.second);
            for (CTokenList::recorder rec(tmp); rec; ++rec)
            {
                ICallBaseAsync* pf = rec.get_data();
                pf->done(bCancel);
                delete pf;
            }
            m_critical.Unlock();
        }
        else
            this->SendPostedEvents();
    }

public:
    _CMsgWindow()
    {
        this->Create();
        DbgAssert(!this->IsNull());
        DbgAssert(!m_aToken.size());
    }

    ~_CMsgWindow()
    {
    }

    void Construct() {}

    TMBool IsSync(ECallType eType)
    {
        DbgAssert(!this->IsNull());
        return (eType == eCallTypeSync || eType == eCallTypeSyncAsync) && (ThreadSelfId() == GetTID());
    }

    TMBool Call(ECallType eType, PFNCall1 pfn, void* pArg)
    {
        if (IsSync(eType))
            return pfn(pArg), true;
        else if (eType == eCallTypeSync)
            return this->SendMessage(WM_SYNC_CALL, (TMWParam)pfn, (TMLParam)pArg);
        else
        {
            TMBool b = this->PostMessage(WM_SYNC_CALL, (TMWParam)pfn, (TMLParam)pArg);
            DbgAssert(b);
            return b;
        }
    }

    TMBool Call(ECallType eType, ICallBase* pfn, void* token)
    {
        if (IsSync(eType))
            return pfn->done(), true;
        else if (eType == eCallTypeSync)
            return this->SendMessage(WM_SYNC_CALLEX, (TMWParam)pfn);
        else
        {
            ICallBaseAsync* p = pfn->convert();
            if (token)
            {
                m_critical.Lock();
                m_aToken.insert2(token, p);
                m_critical.Unlock();
            }
            return this->PostMessage(WM_SYNC_CALLEXA, (TMWParam)p, (TMWParam)token);
        }
    }
};



void CallInit()
{
    _CMsgWindow::Instance()->AddRef();
}

void CallTerm()
{
    _CMsgWindow::Instance()->Release();
}

TMThreadID CallGetTID()
{
    _CMsgWindow* p = _CMsgWindow::GetInstanceNoAddRef();
    return p ? p->GetTID() : 0;
}

void CallFlush(void* token, TMBool bCancel)
{
    _CMsgWindow* p = _CMsgWindow::GetInstanceNoAddRef();
    if (p)
        p->Flush(token, bCancel);
}

TMBool CallCreate(ECallType eType, PFNCall1 pfn, void* pArg)
{
    return _CMsgWindow::Instance()->Call(eType, pfn, pArg);
}

TMBool CallCreateEx(ECallType eType, ICallBase* pfn, void* token)
{
    return _CMsgWindow::Instance()->Call(eType, pfn, token);
}


