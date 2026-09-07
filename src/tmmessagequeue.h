/*
 *  @file
 *  @brief CMessageQueue
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2020.8.14
 *
 *  Copyright (C) 2020 miragekiller
 */

#pragma once

#ifndef TMMESSAGEQUEUE_H
#define TMMESSAGEQUEUE_H

#include "tmatl.h"
#include "tmthread.h"

#ifdef TM_QT
#   include <QApplication>
#   include <QAbstractEventDispatcher>
#endif

#ifdef TM_GTK
#   include <glib.h>
#endif


TM_BGN_NAMESPACE

#ifdef TM_WIN

///////////////////////////////////////////////////////////////////////////////
// CMessageQueueNativeT
template <class T>
struct CMessageQueueWinT
{
protected:
    volatile HWND  m_hMessage;

    static LRESULT WINAPI _WNDPROC(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
    {
        T* p = (T*)CWindowWin(hWnd).GetUserData();
        return p->WndProc(message, wparam, lparam);
    }

    TMResult WndProc(TMUInt uMsg, TMWParam wParam, TMLParam lParam)
    {
        TMResult lr = ::DefWindowProc(m_hMessage, uMsg, wParam, lParam);
        if (uMsg == WM_NCDESTROY)
        {
            m_hMessage = 0;
            ::PostThreadMessage(ThreadSelfId(), WM_NULL, 0, 0); // ::PostQuitMessage(0);
        }
        return lr;
    }

public:
    typedef CMessageQueueWinT    message_queue_class;
        
    CMessageQueueWinT()
        : m_hMessage(0)
    {
    }
    
    ~CMessageQueueWinT()
    {
        ::DestroyWindow(m_hMessage);
    }

    DWORD GetTID() { return ::GetWindowThreadProcessId(m_hMessage, 0); }

    void _CheckTID() { DbgAssert(GetTID() == ThreadSelfId()); }

    operator HWND () const { return m_hMessage; }

    void Create()
    {
        TMCStr p = TM_T("TM message window");
        static ATOM atom = CWindowAT::NativeRegister(p);
        m_hMessage = CWindowAT::NativeCreate2(p, HWND_MESSAGE, &_WNDPROC, static_cast<T*>(this));
    }

    bool IsNull()
    {
        return !::IsWindow(m_hMessage);
    }

//     void Exec()
//     {
//         DbgAssert(m_hMessage);
//         DbgAssert(ThreadSelfId() == ::GetWindowThreadProcessId(m_hMessage, 0));
//         MSG msg;
//         while(m_hMessage && GetMessage(&msg, NULL, 0, 0))
//             DispatchMessage(&msg);
//     }

    void Exit()
    {
        ::PostMessage(m_hMessage, WM_QUIT, 0, 0);
    }

    void SendPostedEvents()
    {
        DbgAssert(m_hMessage);
        _CheckTID();

        MSG msg;
        MSG msgQuit = {0};
        for (; ::PeekMessage(&msg, m_hMessage, 0, 0, PM_REMOVE);)
        {
            if (msg.message == WM_QUIT)
                msgQuit = msg;
            else
                (static_cast<T*>(this))->WndProc(msg.message, msg.wParam, msg.lParam);
        }
        if (msgQuit.message)
            PostQuitMessage(msgQuit.wParam);
    }

    TMBool PostMessage(TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0)
    {
        DbgAssert(m_hMessage);
        return ::PostMessage(m_hMessage, message, wParam, lParam);
    }

    HRESULT SendMessage(TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0)
    {
        DbgAssert(m_hMessage);
        return ::SendMessage(m_hMessage, message, wParam, lParam);
    }
};

template <class T>
class CMessageThreadT : public CThreadID
{
private:
    struct CREATE_PARAM { CState state; T* p; }; 
    struct SENDMSG_PARAM { UINT message; WPARAM wParam; LPARAM lParam; TMResult result; };
    enum { WH_SENDMESSAGE = WM_APP + 1001 }; // wp:CState*; lp:SENDMSG_PARAM*

    static TM_DECALRE_THREAD_PROC(_ThreadProc)
    {
        MSG msg;
        PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE);
        CREATE_PARAM* p = (CREATE_PARAM*)pArg;
        T* pthis = p->p;
        p->state.Signal();

        for (;;)
        {
            pthis->WaitMessage();
            while(::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
            {
                switch (msg.message)
                {
                case WH_SENDMESSAGE: // send message
                    {
                        SENDMSG_PARAM* pr = (SENDMSG_PARAM*)msg.lParam;
                        pr->result = pthis->WndProc(pr->message, pr->wParam, pr->lParam);
                        ((CState*)msg.wParam)->Signal();
                    }
                    continue;
                    
                default: // post message
                    pthis->WndProc(msg.message, msg.wParam, msg.lParam);
                    continue;
                    
                case WM_QUIT:
                    return msg.wParam;
                }
            }
        }

        return 0;
    }

public:
    void WaitMessage() { ::WaitMessage(); }

    TMBool PostMessage(TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0)
    {
        DbgAssert(this->IsValid());
        DbgAssert(message != WH_SENDMESSAGE);
        return PostThreadMessage(m_obj.dwID, message, wParam, lParam);
    }
    
    TMResult SendMessage(TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0)
    {
        DbgAssert(this->IsValid());
        DebugCheckNoMessageLoop();
        SENDMSG_PARAM sp = { message, wParam, lParam };
        CState state;
        if (::PostThreadMessage(m_obj.dwID, WH_SENDMESSAGE, (WPARAM)&state, (LPARAM)&sp))
            state.Lock();
        return sp.result;
    }

    void Create()
    {
        DbgAssert(!IsValid());
        CREATE_PARAM cp;
        cp.p = static_cast<T*>(this);
        CreateBy(&_ThreadProc, &cp);
        cp.state.Lock();
    }

    void Exit()
    {
        PostThreadMessage(m_obj.dwID, WM_QUIT, 0, 0);
    }

public:
//     TMResult WndProc(TMUInt uMsg, TMWParam wParam, TMLParam lParam)
//     {
//         return 0;
//     }
};


#endif // TM_WIN


#ifdef TM_QT

///////////////////////////////////////////////////////////////////////////////
// CMessageEvent
//  SendMessage or PostMessage to a QObject
struct CMessageEvent : public QEvent
{
    QSemaphore* m_semaphone;
    TMResult* m_lr;
    TMUInt m_message;
    TMWParam m_wParam;
    TMLParam m_lParam;
    
    CMessageEvent(TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0, QSemaphore* s = 0, TMResult* lr = 0)
        : QEvent(GetEventType())
        , m_semaphone(s)
        , m_lr(lr)
        , m_message(message)
        , m_wParam(wParam)
        , m_lParam(lParam)
    {} 
    
    constexpr static QEvent::Type GetEventType() { return static_cast<QEvent::Type>(QEvent::User + 100); }

    static TMResult SendMessage(QObject* pObj, TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0)
    {
        TMResult lr = 0;
        if (QThread::currentThread() == pObj->thread())
        {
            CMessageEvent me(message, wParam, lParam, 0, &lr);
            if (QApplication::sendEvent(pObj, &me))
                return lr;
            return false;
        }
        
        QSemaphore semaphore;
        CMessageEvent* pEvent = new CMessageEvent(message, wParam, lParam, &semaphore, &lr);
        QApplication::postEvent(pObj, pEvent, Qt::HighEventPriority);
        semaphore.acquire();
        
        return lr;
    }
    
    static TMBool PostMessage(QObject* pObj, TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0)
    {
        QApplication::postEvent(pObj, new CMessageEvent(message, wParam, lParam));
        return true;
    }
    
    // sample:
    // virtual void customEvent(QEvent *event) tm_override { DoCustomEvent(event, this); }
    template <class T_sender>
    static void DoCustomEvent(QEvent *event, T_sender* p)
    {
        if (event->type() == CMessageEvent::GetEventType())
        {
            CMessageEvent* pEvent = static_cast<CMessageEvent*>(event);
            TMResult hr = p->WndProc(pEvent->m_message, pEvent->m_wParam, pEvent->m_lParam);
            if (pEvent->m_lr)
                *pEvent->m_lr = hr;
            if (pEvent->m_semaphone)
                pEvent->m_semaphone->release();
        }
    }
};


///////////////////////////////////////////////////////////////////////////////
// CMessagedObjectT
//  T_Base: QObject, ...
template <class T, class T_Base>
class CMessagedObjectT : public T_Base
{
public:
    void customEvent(QEvent *event) tm_override
    {
        CMessageEvent::DoCustomEvent(event, static_cast<T*>(this));
    }
};

///////////////////////////////////////////////////////////////////////////////
// CMessageQueueQtT
template <class T>
struct CMessageQueueQtT
{
protected:
    struct CMsgObject : public CMessagedObjectT<CMsgObject, QObject>
    {
        TMResult WndProc(TMUInt uMsg, TMWParam wParam, TMLParam lParam)
        {
            CMessageQueueQtT* p = TM_OTHIS(CMessageQueueQtT, m_nBuffer);
            return static_cast<T*>(p)->WndProc(uMsg, wParam, lParam);
        }
    };

    TMUInt8 m_nBuffer[sizeof(CMsgObject)];
    CMsgObject* m_pObj;
    QEventLoop* m_pLoop;
    friend CMsgObject;

protected:
    TMResult WndProc(TMUInt uMsg, TMWParam wParam, TMLParam lParam)
    {
        return 0;
    }

public:
    typedef CMessageQueueQtT    message_queue_class;

    CMessageQueueQtT()
        : m_pObj(0)
        , m_pLoop(0)
    {
    }
    
    ~CMessageQueueQtT()
    {
        m_pObj->~CMsgObject();
    }

    void Create()
    {
        DbgAssert(!m_pObj);
        //DbgAssert(QThread::currentThread() == qApp->thread());
        m_pObj = new(m_nBuffer)CMsgObject();
        //DbgAssert(m_pObj->thread() == qApp->thread());
    }

    bool IsNull()
    {
        return !m_pObj;
    }
    
    void Exec()
    {
        DbgAssert(this->m_pObj);
        DbgAssert(m_pObj->thread() == QThread::currentThread());
        QEventLoop loop;
        QEventLoop* pOld = m_pLoop;
        m_pLoop = &loop;
        loop.exec();
        m_pLoop = pOld;
    }

    void Exit()
    {
        DbgAssert(this->m_pObj);
        if (m_pLoop)
            m_pLoop->quit();
//        QApplication::postEvent(this->m_pObj, new QEvent(QEvent::Quit));
    }
    
    void SendPostedEvents()
    {
        DbgAssert(this->m_pObj);
        DbgAssert(m_pObj->thread() == QThread::currentThread());
        QApplication::sendPostedEvents(this->m_pObj, CMessageEvent::GetEventType());
    }
    
    TMBool PostMessage(TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0)
    {
        DbgAssert(m_pObj);
        return CMessageEvent::PostMessage(m_pObj, message, wParam, lParam);
    }
    
    TMResult SendMessage(TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0)
    {
        DbgAssert(m_pObj);
        return CMessageEvent::SendMessage(m_pObj, message, wParam, lParam);
    }
};

#endif // #ifdef TM_QT else


#ifdef TM_GTK

///////////////////////////////////////////////////////////////////////////////
// CMessageQueueGtkT
//  Uses GLib main context + idle sources for PostMessage/SendMessage
template <class T>
struct CMessageQueueGtkT
{
protected:
    GMainContext* m_pContext;
    GMainLoop*    m_pLoop;
    TMThreadID    m_tid;

    struct CPostParam
    {
        T*       p;
        TMUInt   message;
        TMWParam wParam;
        TMLParam lParam;
    };

    struct CSendParam
    {
        T*       p;
        TMUInt   message;
        TMWParam wParam;
        TMLParam lParam;
        TMResult result;
        GMutex   mutex;
        GCond    cond;
        bool     done;
    };

    static gboolean _PostCallback(gpointer data)
    {
        CPostParam* pp = (CPostParam*)data;
        pp->p->WndProc(pp->message, pp->wParam, pp->lParam);
        delete pp;
        return G_SOURCE_REMOVE;
    }

    static gboolean _SendCallback(gpointer data)
    {
        CSendParam* sp = (CSendParam*)data;
        sp->result = sp->p->WndProc(sp->message, sp->wParam, sp->lParam);
        g_mutex_lock(&sp->mutex);
        sp->done = true;
        g_cond_signal(&sp->cond);
        g_mutex_unlock(&sp->mutex);
        return G_SOURCE_REMOVE;
    }

    TMResult WndProc(TMUInt /*uMsg*/, TMWParam /*wParam*/, TMLParam /*lParam*/)
    {
        return 0;
    }

public:
    typedef CMessageQueueGtkT message_queue_class;

    CMessageQueueGtkT()
        : m_pContext(0)
        , m_pLoop(0)
        , m_tid(0)
    {
    }

    ~CMessageQueueGtkT()
    {
        if (m_pContext)
        {
            g_main_context_unref(m_pContext);
            m_pContext = 0;
        }
    }

    TMThreadID GetTID() { return m_tid; }

    void _CheckTID() { DbgAssert(GetTID() == ThreadSelfId()); }

    void Create()
    {
        DbgAssert(!m_pContext);
        m_tid = ThreadSelfId();
        m_pContext = g_main_context_new();
    }

    bool IsNull()
    {
        return !m_pContext;
    }

    void Exec()
    {
        DbgAssert(m_pContext);
        g_main_context_push_thread_default(m_pContext);
        m_pLoop = g_main_loop_new(m_pContext, FALSE);
        g_main_loop_run(m_pLoop);
        g_main_loop_unref(m_pLoop);
        m_pLoop = 0;
        g_main_context_pop_thread_default(m_pContext);
    }

    void Exit()
    {
        if (m_pLoop)
            g_main_loop_quit(m_pLoop);
    }

    void SendPostedEvents()
    {
        DbgAssert(m_pContext);
        while (g_main_context_iteration(m_pContext, FALSE))
            ;
    }

    TMBool PostMessage(TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0)
    {
        DbgAssert(m_pContext);
        CPostParam* pp = new CPostParam();
        pp->p       = static_cast<T*>(this);
        pp->message = message;
        pp->wParam  = wParam;
        pp->lParam  = lParam;
        GSource* src = g_idle_source_new();
        g_source_set_callback(src, _PostCallback, pp, 0);
        g_source_attach(src, m_pContext);
        g_source_unref(src);
        return TRUE;
    }

    TMResult SendMessage(TMUInt message, TMWParam wParam = 0, TMLParam lParam = 0)
    {
        DbgAssert(m_pContext);
        if (g_main_context_is_owner(m_pContext))
        {
            // Called from the same thread that owns the context: dispatch directly
            return static_cast<T*>(this)->WndProc(message, wParam, lParam);
        }

        CSendParam sp;
        sp.p       = static_cast<T*>(this);
        sp.message = message;
        sp.wParam  = wParam;
        sp.lParam  = lParam;
        sp.result  = 0;
        sp.done    = false;
        g_mutex_init(&sp.mutex);
        g_cond_init(&sp.cond);

        GSource* src = g_idle_source_new();
        g_source_set_callback(src, _SendCallback, &sp, 0);
        g_source_attach(src, m_pContext);
        g_source_unref(src);

        g_mutex_lock(&sp.mutex);
        while (!sp.done)
            g_cond_wait(&sp.cond, &sp.mutex);
        g_mutex_unlock(&sp.mutex);

        g_mutex_clear(&sp.mutex);
        g_cond_clear(&sp.cond);
        return sp.result;
    }
};

#endif // TM_GTK


#ifdef TM_QT
#   define  CMessageQueueT CMessageQueueQtT
#elif defined(TM_WIN)
#   define  CMessageQueueT CMessageQueueWinT
#elif defined(TM_GTK)
#   define  CMessageQueueT CMessageQueueGtkT
#else
#   error "not support"
#endif // #ifdef TM_QT else


TM_END_NAMESPACE



#endif //TMMESSAGEQUEUE_H
