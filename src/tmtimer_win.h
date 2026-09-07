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

#ifndef TMTIMER_WIN_H
#define TMTIMER_WIN_H

#include "tmthunk.h"


typedef UINT_PTR TMHTimer;


TM_BGN_NAMESPACE
TM_BGN_PRIVATE_NAMESPACE

///////////////////////////////////////////////////////////////////////////////
// CTimerIDBase, CTimerIDBaseN
class CTimerIDBase
{
public:
    TMHTimer    m_tmID;
    CAtlThunk   m_chunk;

    TM_NO_COPYABLE(CTimerIDBase);
    
protected:
    // T_tmID: TMHTimer; CTimerIDn; 
    template <class T_Callback, class T_tmID> struct PROC
    {
        static void WINAPI _TimerProc(HWND lData, UINT uMsg, UINT_PTR idEvent, DWORD tmNow)
        {
            T_Callback* pthis = reinterpret_cast<T_Callback*>(lData);
            pthis->OnTimer(*(const T_tmID*)&idEvent, tmNow);
        }
    };

    TMHTimer __SetTimer(UINT interval, TIMERPROC proc, void* data)
    {
        if (m_tmID)
            ::KillTimer(0, m_tmID);
        m_chunk.Init((WNDPROC)proc, (void*)data);
        m_tmID = ::SetTimer(0, 0, interval, (TIMERPROC)m_chunk.GetWNDPROC());
        return m_tmID;
    }
    
    template <class T_Callback, class T_tmID>
    static TMHTimer _SetTimer(T_Callback* cb, T_tmID* id, UINT interval)
    {
        return id->__SetTimer(interval, PROC<T_Callback, T_tmID>::_TimerProc, cb);
    }

    template <class T_Callback>
    TMHTimer _SetTimer(T_Callback* cb, UINT interval)
    {
        return __SetTimer(interval, PROC<T_Callback, TMHTimer>::_TimerProc, cb);
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
            ::KillTimer(0, m_tmID);
            m_tmID = 0;
        }
    }

    void KillAni()
    {
        KillTimer(); 
    }

    operator TMHTimer() const { return m_tmID; }
};

typedef CTimerIDBase    CTimerIDBaseN;


TM_END_PRIVATE_NAMESPACE
TM_END_NAMESPACE

#endif // #ifndef TMTIMER_WIN_H
