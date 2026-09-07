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

#ifndef TMTIMER_H
#define TMTIMER_H

#include "tmdef.h"

#ifdef TM_WIN
#   include "tmtimer_win.h"
#elif defined(TM_GTK)
#   include "tmtimer_gtk.h"
#endif


TM_BGN_NAMESPACE

///////////////////////////////////////////////////////////////////////////////
// CTimerImpl
// class AAA : public TM::CTimerImpl<AAA>
// {
//   void Start() { this->SetAni(); }
//   void OnTimer(TMHTimer hTimer, TMClock tmNow) { ... }
// };
template<class T>
class CTimerImpl : public PRI::CTimerIDBase
{
public:
    TMHTimer SetTimer(TMUInt interval)
    {
        T* pthis = static_cast<T*>(this);
        return _SetTimer(pthis, interval);
    }

    TMHTimer SetAni(TMUInt interval)
    {
        return SetTimer(interval);
    }
    
//    void OnTimer(TMHTimer hTimer, TMClock tmNow);
};


///////////////////////////////////////////////////////////////////////////////
// CTimerIDn
// class AAA
// {
//   TM::CTimerID1 ani1;
//   TM::CTimerID2 ani2;
//   TM::CTimerID3 ani3;
//
//   void Start() { ani1.SetAni(this); ani2.SetAni(this); ani3.SetAni(this); }
//
//   void OnTimer(const TM::CTimerID1&, TMClock tmNow) { ... }
//   void OnTimer(const TM::CTimerID2&, TMClock tmNow) { ... }
//   void OnTimer(const TM::CTimerID3&, ...) { ... }
// };
template<int n = 0>
class CTimerIDn : public PRI::CTimerIDBaseN
{
public:
    template<class T> TMHTimer SetTimer(TMUInt interval, T* pthis)
    {
        return _SetTimer(pthis, this, interval);
    }

    template<class T> TMHTimer SetAni(TMUInt interval, T* pthis)
    {
        return SetTimer(interval, pthis);
    }
    
//    void OnTimer(const TM::CTimerIDx&, TMClock tmNow);
//    void OnTimer(TMHTimer hTimer, TMClock tmNow);
//    void OnTimer(...);
};

typedef CTimerIDn<0>      CTimerID;
typedef CTimerIDn<0>      CTimerID0;
typedef CTimerIDn<1>      CTimerID1;
typedef CTimerIDn<2>      CTimerID2;
typedef CTimerIDn<3>      CTimerID3;
typedef CTimerIDn<4>      CTimerID4;
typedef CTimerIDn<5>      CTimerID5;


TM_END_NAMESPACE


#endif // #ifndef TMTIMER_H
