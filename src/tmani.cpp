/*
 *  @file
 *  @brief  high-precision timer manage
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2014.1.28
 *
 *  Copyright (C) 2014 miragekiller
 */

#include "tmani.h"
#include "tmtime.h"
#include "tmthread.h"
#include "tmlist.h"

#ifdef TM_WIN
#   include "tmplatform_win.h"
#   include <ddraw.h>
#elif defined(TM_GTK)
#   include <gdk/gdk.h>
#endif


// #define TMANI_TRACEM    DbgTraceMethod
// #define TMANI_TRACE     DbgTrace
#define TMANI_TRACEM    DbgTraceNull
#define TMANI_TRACE     DbgTraceNull


struct CAniBase
{
    union
    {
    void*      volatile m_proc;     // 0: deleted; 1: run but not notify
    PFNVNIPROC volatile m_procV;
    PFNANIPROC volatile m_procA;
    };
    TMLParam   volatile m_data;
    TM_IF_DBG(CAniBase* m_pthis);

    void _Check() const { TM_IF_DBG(DbgAssert(m_pthis == this)); }
    bool IsDeleted() const { return 0 == m_procV; }

    void InitDelay(int) {}
    void InitFrame(int) {}
    void InitClock(int) {}
};

struct TMHAni__ : public CAniBase
{
    TM_CLASS_INSTANCE_CHECK(TMHAni__);

    enum { eError = INT_MAX };

    TM::CClock          m_clock;            // 0: pause;
    TMUInt volatile     m_dwDelay;
    TMUInt volatile     m_nFrame;           // current frame

    void InitDelay(TMUInt n) { m_dwDelay = n; }
    void InitFrame(TMUInt n) { m_nFrame = n; }
    void InitClock(TMClock n) { m_clock = n; }

    static void _call(void* p)
    {
        TMHAni__* ani = (TMHAni__*)p;
        ani->_Check();
        TMANI_TRACEM("TMHAni__::_call", "(hAni:%p, proc:%p, clock:%d, frame:%d)", ani, ani->m_procA, ani->m_clock, ani->m_nFrame);
        if (!ani->IsInvalid())
        {
//            ReplyMessage(true);
            ani->m_procA(ani, ani->m_data, ani->m_clock, ani->m_nFrame);
        }
    }

    TMHAni__() : m_clock(0) {}

    bool IsPaused() const { return 0 == m_clock; }
    bool IsInvalid() const { return IsDeleted() || IsPaused(); }
    int CalcFrame() const { return m_clock.ElapseMS() / m_dwDelay; }

    int ThreadElapse(TM::CCritical& cri) // return: next time, call from thread
    {
        _Check();
        if (IsInvalid())
            return eError;

        DbgAssert(m_dwDelay);
        int nFrame = CalcFrame();
        TMANI_TRACEM("TMHAni__::ThreadElapse", "(hAni:%p, proc:%p, clock:%d, now:%d, frame:%d)", this, m_procA, m_clock, TimeClock(), nFrame);
        if (nFrame != m_nFrame)
        {
            m_nFrame = nFrame;
            cri.Unlock();
            DbgVerify(CallCreate(eCallTypeSync, _call, this));
            cri.Lock();
            if (!m_procA || !m_clock)
                return eError;
        }
        int dNext = (m_nFrame + 1) * m_dwDelay + m_clock;
        TMANI_TRACE("TMHAni__::ThreadElapse(next:%d)", dNext);
        return dNext;
    }

    void Pause()
    {
        TMANI_TRACEM("TMHAni__::Pause", "(hAni:%p)", this);
        m_clock.Assign(0);
    }

    void _Resume()
    {
        m_clock.Reset();
        m_clock.Add(-(int)(m_nFrame * m_dwDelay));
    }

    void Resume()
    {
        TMANI_TRACEM("TMHAni__::Resume", "(hAni:%p)", this);
        if (!m_clock)
            _Resume();
    }

    void SetDelay(TMUInt dwDelay)
    {
        TMANI_TRACEM("TMHAni__::SetDelay", "(hAni:%p, dwDelay:%d)", this, dwDelay);
        DbgAssert(m_dwDelay != 0);
        m_dwDelay = dwDelay;
        if (!IsInvalid())
            _Resume();
    }

    void SetDelayOnce(TMUInt dwDelay)
    {
        TMANI_TRACEM("TMHAni__::SetDelay", "(hAni:%p, dwDelay:%d)", this, dwDelay);
        DbgAssert(m_dwDelay != 0);
        m_clock.Reset();
        m_clock.Add(-(int)((m_nFrame + 1) * m_dwDelay) + dwDelay);
    }

    void SetFrame(TMUInt nFrame)
    {
        TMANI_TRACEM("TMHAni__::SetFrame", "(hAni:%p, nFrame:%d)", this, nFrame);
        int dFrame = m_nFrame - nFrame;
        m_nFrame = nFrame;
        if (m_clock) // not pause
            m_clock.m_nNow += (dFrame * m_dwDelay);
    }

    TMUInt GetPassed()
    {
        if (IsInvalid())
            return m_clock.ToMillionSecond();
        else
            return m_clock.ElapseMS();
    }
};


///////////////////////////////////////////////////////////////////////////////
// TMHVni__
struct TMHVni__ : public CAniBase
{
    TM_CLASS_INSTANCE_CHECK(TMHVni__);

    struct CALLPARAM
    {
        TMHVni__*   m_ani;
        TMClock     m_tmNow;
        TMClock     m_tmVBlank;
    };

    int m_nSkip;
        
    bool IsPaused() const { return false; }
    bool IsInvalid() const { return IsDeleted(); }

    static void _call(void* p)
    {
        CALLPARAM* cp = (CALLPARAM*)p;
        cp->m_ani->_Check();
        TMANI_TRACEM("TMHVni__::_call", "(hVni:%p, proc:%p, clock:%d)", cp->m_ani, cp->m_ani->m_procV, cp->m_tmNow);
        if (!cp->m_ani->IsInvalid())
            cp->m_ani->m_procV(cp->m_ani, (TMLParam)cp->m_ani->m_data, cp->m_tmNow, cp->m_tmVBlank);
    }

    TMBool ThreadElapse(TM::CCritical& cri, int now, int dVBlank)
    {
        _Check();
        if (IsInvalid())
            return false;
        
        TMANI_TRACEM("TMHVni__::ThreadElapse", "(hAni:%p, proc:%p, now:%d)", this, m_procV, now);
        cri.Unlock();
        CALLPARAM cp = { this, now, dVBlank };
        DbgVerify(CallCreate(eCallTypeSync, _call, &cp));
        cri.Lock();

        return !IsInvalid();
    }
};


///////////////////////////////////////////////////////////////////////////////
// CAniMgr
struct CAniMgr
{
    TM_CLASS_INSTANCE_CHECK(CAniMgr);

    typedef TM::CList<TMHAni__>             CAniList;
    typedef TM::CList<TMHVni__>             CVniList;
    typedef TM::CAutoLockT<TM::CCritical>   CAutoLock;

#ifdef TM_WIN
    struct VBlank
    {
        CComPtr<IDirectDraw> m_ptrDD;
        TMClock m_tmNow;
        TMClock m_tmVBlank;

        void Init()
        {
            if (m_ptrDD)
                return;
            struct dll_ddraw { DECLARE_DLL_LOAD("ddraw.dll", LOAD_LIBRARY_SEARCH_SYSTEM32); };
            DECLARE_DLL_FUNC(dll_ddraw, HRESULT, WINAPI, DirectDrawCreate,(
                GUID FAR *lpGUID,        
                LPDIRECTDRAW FAR *lplpDD,  
                IUnknown FAR *pUnkOuter  
               ));
            _DirectDrawCreate(0, &m_ptrDD, 0);
        }

        TMClock GetVBlankTicket()
        {
            Init();
            DWORD dwF = 60;
            m_ptrDD->GetMonitorFrequency(&dwF);
            m_tmVBlank = 1000 / dwF;
            return m_tmVBlank;
        }

        void WaitForVerticalBlank()
        {
            if (_DwmFlush && SUCCEEDED(_DwmFlush()))
            {
            }
            else
            {
                Init();
                m_ptrDD->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, 0);
            }
            TMClock now = TimeClock();
//             DbgTrace("CAniMgr::VBlank::WaitForVerticalBlank() %d", now - m_tmNow);
            m_tmNow = now;
        }
    };

#elif defined TM_GTK
    struct VBlank
    {
        TMClock m_tmNow;
        TMClock m_tmVBlank;

        void Init()
        {
            if (m_tmVBlank <= 0)
            {
                int hz = 60;
                GdkDisplay* pDisplay = gdk_display_get_default();
                if (pDisplay)
                {
                    GdkMonitor* pMonitor = gdk_display_get_primary_monitor(pDisplay);
                    if (!pMonitor && gdk_display_get_n_monitors(pDisplay) > 0)
                        pMonitor = gdk_display_get_monitor(pDisplay, 0);
                    if (pMonitor)
                    {
                        const int nRefreshMilliHz = gdk_monitor_get_refresh_rate(pMonitor);
                        if (nRefreshMilliHz > 0)
                            hz = (nRefreshMilliHz + 500) / 1000;
                    }
                }
                if (hz <= 0)
                    hz = 60;
                m_tmVBlank = (1000 + hz / 2) / hz;
                if (m_tmVBlank <= 0)
                    m_tmVBlank = 1000 / 60;
            }
            if (m_tmNow <= 0)
                m_tmNow = TimeClock();
        }

        TMClock GetVBlankTicket()
        {
            Init();
            return m_tmVBlank;
        }

        void WaitForVerticalBlank()
        {
            Init();
            TMClock now = TimeClock();
            TMClock next = m_tmNow + m_tmVBlank;
            if (now < next)
                g_usleep((gulong)(next - now) * 1000);
            m_tmNow = TimeClock();
        }
    };

#else
#   error not support
#endif

    struct CThr : public TM::CThreadID, public TM::CState
    {
        void Signal()
        {
            TMANI_TRACE("CAniMgr::CThread::Signal()");
            typedef TM::CState base_class;
            base_class::Signal();
        }
    };

    CThr            m_thrAni;
    CThr            m_thrVni;
    CAniList        m_aAni;         // need lock m_critical
    CVniList        m_aVni[2];      // need lock m_critical
    TM::CCritical   m_critical;
    volatile TMBool m_bExit;
    VBlank          m_vBlank;
    static CAniMgr  s_aniMgr;

    CAniMgr()
        : m_bExit(false)
    {
        TMANI_TRACEM("CAniMgr::CAniMgr");
    }
    
    ~CAniMgr()
    {
        m_bExit = true;
        m_thrAni.Signal();
        m_thrAni.Join();
        m_thrVni.Signal();
        m_thrVni.Join();
//         DbgAssert(0 == m_aAni.size());
        DbgTrace("CAniMgr::~CAniMgr()");
    }

    template <class T_aAni, class T_Ani, class T_fnThread>
    T_Ani* Create(CThr& thr, T_aAni& aAni, T_Ani* hAni, TMUInt dwDelay, void* pCallback, TMLParam data, T_fnThread fn)
    {
        DbgAssert(!m_bExit);
        DbgAssert(DbgIsPtr(pCallback));

        TM::CClock now; // lock maybe use more times.
        CAutoLock lock(m_critical);

        if (!thr.IsValid())
            thr.CreateBy(fn, 0);

        if (!hAni)
        {
            aAni.push_back(T_Ani());
            TMANI_TRACE("CAniMgr::Create(hAni:%p, proc:%p) size:%d, return:%p", hAni, pCallback, m_aAni.size(), &m_aAni.back());
            hAni = &aAni.back();
        }
        else
            TMANI_TRACE("CAniMgr::ReCreate(hAni:%p, proc:%p) size:%d", hAni, pCallback, m_aAni.size());

        hAni->InitDelay(dwDelay);
        hAni->InitFrame(0);
        hAni->m_proc = pCallback;
        hAni->m_data = data;
        hAni->InitClock(now);
        TM_IF_DBG(hAni->m_pthis = hAni);
        thr.Signal();
        return hAni;
    }

    void Kill(CAniBase* tm, CThr& thr)
    {
        if (m_bExit)
            return;
        TMANI_TRACEM("CAniMgr::Kill", "(hAni:%p)", tm);
        CAutoLock lock(m_critical);
        tm->m_proc = 0; // the timer manage will delete tm at ThreadElapse;
        TMANI_TRACE("CAniMgr::kill(hAni:%p, proc:%p), size:%d", tm, tm->m_proc, m_aAni.size());
        thr.Signal();
    }

    ///////////////////////////////////////////////////////////////////////////
    // Ani function
    static TM_DECALRE_THREAD_PROC(_AniThreadProc)
    {
        TMANI_TRACEM("CAniMgr::_ThreadProc()");
        
        for (; !s_aniMgr.m_bExit;)
        {
            int nNext;
            {
                CAutoLock lock(s_aniMgr.m_critical);
                nNext = s_aniMgr.AniThreadElapse();
#ifdef TM_DBG
                if (!s_aniMgr.m_aAni.size())
                    DbgAssert(nNext == TMHAni__::eError);
                else if (nNext == TMHAni__::eError)
                {
                    for (CAniList::recorder rec(s_aniMgr.m_aAni); rec; ++rec)
                        DbgAssert(rec.get_data().IsInvalid());
                }
#endif
                s_aniMgr.m_thrAni.Unsignal();
            }
            s_aniMgr.AniLockTo(nNext);
        }
        
        return 0;
    }
    
    int AniThreadElapse() // return: next frame time
    {
        TMANI_TRACEM("CAniMgr::AniThreadElapse", "(count:%d)", m_aAni.size());
        int dNext = TMHAni__::eError;
        for (CAniList::recorder rec(m_aAni); rec;)
        {
            TMHAni__& ani = rec.get_data();
            int tmd = ani.ThreadElapse(m_critical);
            if (tmd != TMHAni__::eError)
                tm_setmin(dNext, tmd);
            if (ani.IsDeleted())
                rec.erase();
            else
                ++rec;
        }
        return dNext;
    }
    
    void AniLockTo(int nNext)
    {
        if (nNext == TMHAni__::eError)
        {
            TMANI_TRACE("CAniMgr::Lock(-1)");
            m_thrAni.Lock(-1);
            TMANI_TRACE("CAniMgr::Lock(-1) end");
        }
        else
        {
            int nNow = TimeClock();
            if (nNow < nNext)
            {
                TMANI_TRACE("CAniMgr::Lock(%d)", nNext - nNow);
                DbgAssert(nNext - nNow < 65535);
                m_thrAni.Lock(nNext - nNow);
                TMANI_TRACE("CAniMgr::Lock(%d) end", nNext - nNow);
            }
        }
    }

    void AniResume(TMHAni tm)
    {
        TMANI_TRACEM("CAniMgr::Resume", "(hAni:%p)", tm);
        DbgAssert(!m_bExit);
        CAutoLock lock(m_critical);
        tm->Resume();
        m_thrAni.Signal();
    }

    ///////////////////////////////////////////////////////////////////////////
    // Vni function
    static TM_DECALRE_THREAD_PROC(_VniThreadProc)
    {
        TMANI_TRACEM("CVniMgr::_ThreadProc()");
        
        for (; !s_aniMgr.m_bExit;)
        {
            s_aniMgr.m_vBlank.WaitForVerticalBlank();
            TMClock now = s_aniMgr.m_vBlank.m_tmNow;
            int dtime = s_aniMgr.m_vBlank.GetVBlankTicket();
            {
                CAutoLock lock(s_aniMgr.m_critical);
                s_aniMgr.VniThreadElapse(s_aniMgr.m_aVni[1], now, dtime);
                s_aniMgr.VniThreadElapse(s_aniMgr.m_aVni[0], now, dtime);
                TMBool b = s_aniMgr.m_aVni[0].empty() && s_aniMgr.m_aVni[1].empty();
                if (!b)
                    continue;
                s_aniMgr.m_thrVni.Unsignal();
            }
            s_aniMgr.m_thrVni.Lock(-1);
        }
        
        return 0;
    }

    void VniThreadElapse(CVniList& aList, int now, int dVBlank)
    {
        TMANI_TRACEM("CAniMgr::VniThreadElapse", "(count:%d)", aList.size());
        for (CVniList::recorder rec(aList); rec;)
        {
            TMHVni__& ani = rec.get_data();
            if (!ani.ThreadElapse(m_critical, now, dVBlank))
                rec.erase();
            else
                ++rec;
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // friend ani,vni api
    friend TMHAni AniCreate(TMHAni hAni, TMUInt dwDelay, PFNANIPROC proc, TMLParam data)
    {
        DbgAssert(dwDelay);
        return CAniMgr::s_aniMgr.Create(CAniMgr::s_aniMgr.m_thrAni, CAniMgr::s_aniMgr.m_aAni,
            hAni, dwDelay, (void*)proc, data, CAniMgr::_AniThreadProc);
    }

    friend void AniKill(TMHAni hAni)
    {
        CAniMgr::s_aniMgr.Kill(hAni, CAniMgr::s_aniMgr.m_thrAni);
    }

    friend TMHVni VniCreate(TMHVni hVni, PFNVNIPROC proc, TMLParam data, EVniPriority nPriority)
    {
        return CAniMgr::s_aniMgr.Create(CAniMgr::s_aniMgr.m_thrVni, CAniMgr::s_aniMgr.m_aVni[nPriority != 0],
            hVni, 0, (void*)proc, data, CAniMgr::_VniThreadProc);
    }
    
    friend void VniKill(TMHVni hVni)
    {
        CAniMgr::s_aniMgr.Kill(hVni, CAniMgr::s_aniMgr.m_thrVni);
    }

    friend TMClock VniGetClock()
    {
        return CAniMgr::s_aniMgr.m_vBlank.m_tmNow;
    }

    friend TMClock VniGetDelay()
    {
        return CAniMgr::s_aniMgr.m_vBlank.m_tmVBlank;
    }
};

CAniMgr CAniMgr::s_aniMgr;






void AniSetCurrentFrame(TMHAni tm, int nCur)
{
    if (tm)
        tm->SetFrame(nCur);
}

TMUInt AniGetCurrentFrame(TMHAni tm)
{
    return tm ? tm->m_nFrame : 0;
}

TMUInt AniGetPassed(TMHAni tm)
{
    return tm ? tm->GetPassed() : 0;
}

// void* AniGetData(TMHAni tm)
// {
//     return tm ? tm->m_data : 0;
// }
// 
// void AniSetData(TMHAni tm, void* data)
// {
//     DbgAssert(tm);
//     tm->m_data = data;
// }

void AniPause(TMHAni tm)
{
    if (tm) tm->Pause();
}

// TMBool AniIsRunning(TMHAni tm)
// {
//     return tm ? !tm->IsInvalid() : false;
// }

void AniResume(TMHAni tm)
{
    DbgAssert(tm);  
    CAniMgr::s_aniMgr.AniResume(tm);
}

void AniSetDelay(TMHAni tm, TMUInt dwDelay)
{
    DbgAssert(tm);
    tm->SetDelay(dwDelay);
}

TMUInt AniGetDelay(TMHAni tm)
{
    DbgAssert(tm);
    return tm->m_dwDelay;
}

void AniLock()
{
    CAniMgr::s_aniMgr.m_critical.Lock();
}

void AniUnlock()
{
    CAniMgr::s_aniMgr.m_critical.Unlock();
}


// void AniSetDelayOnce(TMHAni tm, TMUInt dwDelay)
// {
//     DbgAssert(tm);
//     tm->SetDelayOnce(dwDelay);
// }



///////////////////////////////////////////////////////////////////////////////
// CEaseBase
class TM_NO_VTABLE CEaseBase : public IEase
{
public:
    TM_UNUSED_IC_GetDebugInfo();

protected:
    TM_BGN_INTERFACE_MAP(IEase)
    TM_END_INTERFACE_MAP()
};

float EaseCalcProgress(IEase* pEase, TMUInt dwMode, float first, float last, float timeAll, float timePassed)
{
    DbgAssert(pEase);
    float dTime = timePassed / timeAll;

    tm_setmax(dTime, 0.0f);
    tm_setmin(dTime, 1.0f);

    switch (dwMode & AW_ERASEMODEMASK)
    {
    case AW_ERASEMODEIN: dTime =  pEase->Ease(dTime); break;
    case AW_ERASEMODEOUT: dTime = 1.0f - pEase->Ease(1.0f - dTime); break;
    default:
        if (dTime < 0.5f)
            dTime = pEase->Ease(dTime  * 2.0f) * 0.5f;
        else
            dTime = (1.0f - pEase->Ease((1.0f - dTime) * 2.0f)) * 0.5f + 0.5f;
    }

    return first + (last - first) * dTime;
}


///////////////////////////////////////////////////////////////////////////////
// CEaseBack
class TM_NO_VTABLE CEaseBack : public CEaseBase
{
public:
    double m_nAmplitude;

protected:
    virtual float Ease(float dTime) tm_override
    {
        DbgAssert(m_nAmplitude >= 0);
        return pow(dTime, 3.0f) - dTime * m_nAmplitude * sin(CFloat::PI() * dTime);
    }
};


TM_CAPI void EaseCreateBack(IEase** ppEase, float nAmplitude)
{
    DbgAssert(!*ppEase);
    DbgAssert(nAmplitude >= 0);
    CEaseBack* p = new TM::CRefObjectT<CEaseBack>();
    p->m_nAmplitude = nAmplitude;
    *ppEase = p;
}

///////////////////////////////////////////////////////////////////////////////
// CEaseCircle
class TM_NO_VTABLE CEaseCircle : public CEaseBase
{
protected:
    virtual float Ease(float dTime) tm_override
    {
        return 1.0f - sqrt(1.0f - dTime * dTime);
    }
};


TM_CAPI void EaseCreateCircle(IEase** ppEase)
{
    DbgAssert(!*ppEase);
    CEaseCircle* p = new TM::CRefObjectT<CEaseCircle>();
    *ppEase = p;
}


///////////////////////////////////////////////////////////////////////////////
// CEaseBounce
class TM_NO_VTABLE CEaseBounce : public CEaseBase
{
public:
    int m_nBounces;
    float m_nBounciness;

protected:
    virtual float Ease(float dTime) tm_override
    {
        DbgAssert(m_nBounces >= 0);
        DbgAssert(m_nBounciness > 1.0f);

        float dPow = pow(m_nBounciness, m_nBounces);
        float oneMinusBounciness = 1.0f - m_nBounciness;

        // 'unit' space calculations.
        // Our m_nBounces grow in the x axis exponentially.  we define the first bounce as having a 'unit' width of 1.0 and compute
        // the total number of 'units' using a geometric series.
        // We then compute which 'unit' the current time is in.
        float sumOfUnits = (1.0f - dPow) / oneMinusBounciness + dPow * 0.5f; // geometric series with only half the last sum
        float unitAtT = dTime * sumOfUnits;

        // 'bounce' space calculations.
        // Now that we know which 'unit' the current time is in, we can determine which bounce we're in by solving the geometric equation:
        // unitAtT = (1 - m_nBounciness^bounce) / (1 - m_nBounciness), for bounce.
        float bounceAtT = log(-unitAtT * (1.0f-m_nBounciness) + 1.0f) / log(m_nBounciness); //log(-unitAtT * (1.0-m_nBounciness) + 1.0, m_nBounciness);
        float start = floor(bounceAtT);
        float end = start + 1.0f;

        // 'time' space calculations.
        // We then project the start and end of the bounce into 'time' space
        float startTime = (1.0f - pow(m_nBounciness, start)) / (oneMinusBounciness * sumOfUnits);
        float endTime = (1.0f - pow(m_nBounciness, end)) / (oneMinusBounciness * sumOfUnits);

        // Curve fitting for bounce.
        float midTime = (startTime + endTime) * 0.5f;
        float timeRelativeToPeak = dTime - midTime;
        float radius = midTime - startTime;
        float amplitude = pow(1.0f / m_nBounciness, (m_nBounces - start));

        // Evaluate a quadratic that hits (startTime,0), (endTime, 0), and peaks at amplitude.
        return (-amplitude / (radius * radius)) * (timeRelativeToPeak - radius) * (timeRelativeToPeak + radius);
    }
};

void EaseCreateBounce(IEase** ppEase, int nBounces, float nBounciness)
{
    DbgAssert(!*ppEase);
    DbgAssert(nBounces >= 0);
    DbgAssert(nBounciness > 1.0f);
    CEaseBounce* p = new TM::CRefObjectT<CEaseBounce>();
    p->m_nBounces = nBounces;
    p->m_nBounciness = nBounciness;
    *ppEase = p;
}


///////////////////////////////////////////////////////////////////////////////
// CEaseElastic
class TM_NO_VTABLE CEaseElastic : public CEaseBase
{
public:
    int m_nOscillations;
    float m_nSpringiness;

protected:
    virtual float Ease(float dTime) tm_override
    {
        DbgAssert(m_nSpringiness >= 0);
        DbgAssert(m_nOscillations >= 0);

        float expo;
        if (CMath::Equal0(m_nSpringiness))
            expo = dTime;
        else
            expo = (exp(m_nSpringiness * dTime) - 1.0f) / (exp(m_nSpringiness) - 1.0f);

        return expo * (sin((CFloat::PI() * 2.0f * m_nOscillations + CFloat::PI() * 0.5f) * dTime));
    }
};

void EaseCreateElastic(IEase** ppEase, int nOscillations, float nSpringiness)
{
    DbgAssert(!*ppEase);
    DbgAssert(nSpringiness >= 0);
    DbgAssert(nOscillations >= 0);
    CEaseElastic* p = new TM::CRefObjectT<CEaseElastic>();
    p->m_nOscillations = nOscillations;
    p->m_nSpringiness = nSpringiness;
    *ppEase = p;
}


///////////////////////////////////////////////////////////////////////////////
// CEaseExponential
class TM_NO_VTABLE CEaseExponential : public CEaseBase
{
public:
    float m_nExponent;

protected:
    virtual float Ease(float dTime) tm_override
    {
        if (CMath::Equal0(m_nExponent))
            return dTime;
        else
            return (exp(m_nExponent * dTime) - 1.0f) / (exp(m_nExponent) - 1.0f);
    }
};


TM_CAPI void EaseCreateExponential(IEase** ppEase, float nExponent)
{
    DbgAssert(!*ppEase);
    CEaseExponential* p = new TM::CRefObjectT<CEaseExponential>();
    p->m_nExponent = nExponent;
    *ppEase = p;
}


///////////////////////////////////////////////////////////////////////////////
// CEasePower
class TM_NO_VTABLE CEasePower : public CEaseBase
{
public:
    float m_nPower;

protected:
    virtual float Ease(float dTime) tm_override
    {
        DbgAssert(m_nPower >= 0);
        return pow(dTime, m_nPower);
    }
};


TM_CAPI void EaseCreatePower(IEase** ppEase, float nPower)
{
    DbgAssert(!*ppEase);
    DbgAssert(nPower >= 0);
    CEasePower* p = new TM::CRefObjectT<CEasePower>();
    p->m_nPower = nPower;
    *ppEase = p;
}


///////////////////////////////////////////////////////////////////////////////
// CEaseSine
class TM_NO_VTABLE CEaseSine : public CEaseBase
{
protected:
    virtual float Ease(float dTime) tm_override
    {
        return 1.0 - sin(CFloat::PI() * 0.5f  * (1.0f - dTime));
    }
};


TM_CAPI void EaseCreateSine(IEase** ppEase)
{
    DbgAssert(!*ppEase);
    CEaseSine* p = new TM::CRefObjectT<CEaseSine>();
    *ppEase = p;
}

