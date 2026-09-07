/*
 *  @file
 *  @brief  fiber
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2020.4.9
 *
 *  Copyright (C) 2020 miragekiller
 */

#pragma once

#ifndef TMFIBER_H
#define TMFIBER_H

#include "tmani.h"
#include "tmthread.h"


TM_BGN_NAMESPACE

// class CFiber
// {
// private:
//     TMBool m_bRun;
// 
//     virtual unsigned ThreadProc() = 0; // tm_override
// 
// public:
//     CFiber()
//         : m_bRun(false)
//     {
//     }
// 
//     operator TMBool () const { return m_bRun; }
// 
//     void Yield()
//     {
//         MSG msg;
//         while(::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
//         {
//             if (msg.message == WM_QUIT)
//                 Stop();
//             ::TranslateMessage(&msg);
//             ::DispatchMessage(&msg);
//         }
//     }
// 
//     void Sleep(int nMS)
//     {
//         CCycle clk;
//         for (int n = clk.ElapseMS(); n < nMS;)
//         {
//             if (WAIT_TIMEOUT == MsgWaitForMultipleObjects(0, 0, false, nMS - n, QS_ALLEVENTS))
//                 return;
//             Yield();
//         }
//     }
// 
//     void Stop()
//     {
//         throw 0;
//     }
// 
//     void Create()
//     {
//         m_bRun = true;
//         try
//         {
//             ThreadProc();
//         }
//         catch (...)
//         {
//         }
//         m_bRun = false;
//     }
// 
// };


// to use CAniFiber, you need set C++/general/debug info format to /Zi£¬don't use /ZI
class CAniFiber : public IThread
{
protected:
    struct CFiberAni : public CAni
    {
        //  CAniProc
        virtual void OnAni(TMHAni tm, TMUInt, TMUInt nFrame, void* data) tm_override
        {
            TM_OTHIS(CAniFiber, m_ani)->ThreadProc();
        }
    };

    CFiberAni m_ani;
    int m_nState;
    friend CFiberAni;
    
protected:
    void Delay(TMUInt ms)
    {
        m_ani.SetDelay(ms < 1 ? 1 : ms);
    }
    
public:
    CAniFiber()
        : m_nState(0)
    {
    }
    
    TMHAni SetAni (TMUInt interval = 1)
    {
        return m_ani.SetAni(interval);
    }
    
    void Stop()
    {
        m_nState = 0;
        m_ani.KillAni();
    }

    // IThread
    // virtual unsigned ThreadProc()  // tm_override
};

#define FiberAniBegin()         Delay(1); switch (m_nState) { case 0:

#define FiberAniEnd()           }

#define FiberAniYield()         do { m_nState=__LINE__; return; case __LINE__:; } while (0)

#define FiberAniSleep(ms)       do { m_nState=__LINE__; Delay(ms); return; case __LINE__:; } while (0)

#define FiberAniStop()          do { Stop(); return; } while (0)

// mark: int, must < 0;
#define FiberAniMark(mark)      { DbgAssertS(mark < 0); } case mark:

// goto 0 means restart.
#define FiberAniGoto(mark)      do { { DbgAssertS(mark <= 0); } m_nState = mark; return; } while (0)


// unused function
// loop:goto the preview FiberAniYield() or FiberAniSleep() or FiberAniMark
#define FiberAniLoop(ms)        do { if (ms) Delay(ms); return; } while (0)
//#define FiberAniGoto(mark,ms)   do { Delay(ms); m_nState = mark; return; } while (0)
//#define FiberAniRestart(ms)       FiberAniGoto(0, ms)
#define FiberAniRestart()       FiberAniGoto(0)


TM_END_NAMESPACE



#endif //TMFIBER_H
