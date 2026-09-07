/*
 *  @file
 *  @brief  timer
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2004.02.20
 *
 *  Copyright (C) 2004 miragekiller
 *
 *  attention: CTimer*** must work in threads which have message queue
 */

#pragma once

#ifndef TMTIMER_GTK_H
#define TMTIMER_GTK_H

#include <glib.h>

typedef guint TMHTimer;


TM_BGN_NAMESPACE
TM_BGN_PRIVATE_NAMESPACE

///////////////////////////////////////////////////////////////////////////////
// CTimerIDBase
class CTimerIDBase
{
public:
    TMHTimer    m_tmID;

    TM_NO_COPYABLE(CTimerIDBase);
    
protected:
    template <class T_Callback> struct PROC
    {
        static gboolean _TimerProc(gpointer userData)
        {
            T_Callback* pthis = reinterpret_cast<T_Callback*>(userData);
            if (!pthis->m_tmID)
                return G_SOURCE_REMOVE;
            pthis->OnTimer(pthis->m_tmID, TimeClock());
            return G_SOURCE_CONTINUE;
        }
    };
    
    TMHTimer __SetTimer(guint interval, GSourceFunc proc, gpointer data)
    {
        DbgAssert(proc);
        if (m_tmID)
            g_source_remove(m_tmID);
        m_tmID = g_timeout_add(interval, proc, data);
        return m_tmID;
    }

    template <class T_Callback>
    TMHTimer _SetTimer(T_Callback* cb, guint interval)
    {
        return __SetTimer(interval, PROC<T_Callback>::_TimerProc, cb);
    }

    CTimerIDBase() : m_tmID(0) {}
    
public:
    ~CTimerIDBase()
    {
        KillTimer();
    }

    void KillTimer()
    {
        if (m_tmID)
        {
            g_source_remove(m_tmID);
            m_tmID = 0;
        }
    }

    void KillAni()
    {
        KillTimer(); 
    }
    
    operator TMHTimer() const { return m_tmID; }
};


///////////////////////////////////////////////////////////////////////////////
// CTimerIDBaseN
class CTimerIDBaseN : public CTimerIDBase
{
public:
    void* m_pTarget;

protected:
    // T_tmID: CTimerIDn; 
    template <class T_Callback, class T_tmID> struct PROC
    {
        static gboolean _TimerProc(gpointer userData)
        {
            T_tmID* pthis = reinterpret_cast<T_tmID*>(userData);
            if (!pthis->m_tmID)
                return G_SOURCE_REMOVE;
            T_Callback* target = reinterpret_cast<T_Callback*>(pthis->m_pTarget);
            target->OnTimer(*pthis, TimeClock());
            return G_SOURCE_CONTINUE;
        }
    };

    template <class T_Callback, class T_tmID>
    TMHTimer _SetTimer(T_Callback* cb, T_tmID* id, guint interval)
    {
        m_pTarget = cb;
        return id->__SetTimer(interval, PROC<T_Callback, T_tmID>::_TimerProc, id);
    }
};



TM_END_PRIVATE_NAMESPACE
TM_END_NAMESPACE

#endif // #ifndef TMTIMER_WIN_H
