/*
 *  @file
 *  @brief  high-precision timer manage
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2011.4.27
 *
 *  Copyright (C) 2011 miragekiller
 */

#pragma once

#ifndef TMANI_H
#define TMANI_H

#include "tmdef.h"
#include "tmcom.h"
#include "tmtime.h"
#include "tmmath.h"


/////////////////////////////////////////////////////////////////////////////
// TMHAni
// TMHAni need call CallInit/CallTerm
// ani can create/kill at any thread, but callback only run in main thread. 
typedef struct TMHAni__*        TMHAni;

typedef void (*PFNANIPROC)(TMHAni hVni, TMLParam lData, TMClock tmNow, TMUInt nFrame);

TM_CAPI TMHAni AniCreate(TMHAni hAni, TMUInt dwDelay, PFNANIPROC proc, TMLParam data = 0) tm_thread_safe;
TM_CAPI void AniKill(TMHAni tm) tm_thread_safe;
TM_CAPI TMUInt AniGetCurrentFrame(TMHAni tm);
TM_CAPI void AniSetCurrentFrame(TMHAni tm, int nCur);
TM_CAPI TMUInt AniGetPassed(TMHAni tm);
TM_CAPI void AniSetDelay(TMHAni tm, TMUInt dwDelay);
TM_CAPI TMUInt AniGetDelay(TMHAni tm);
TM_CAPI void AniPause(TMHAni tm);
TM_CAPI void AniResume(TMHAni tm);
TM_CAPI void AniLock() tm_thread_safe;
TM_CAPI void AniUnlock() tm_thread_safe;


/////////////////////////////////////////////////////////////////////////////
// TMHVni: VBlank ani
// TMHVni need call CallInit/CallTerm
// tmPass: will align to next present time.
typedef struct TMHVni__*        TMHVni;

enum EVniPriority
{
    eVniPriorityLow     = 0,    // use for animate;
    eVniPriorityHigh    = 1,    // use for present;
};

typedef void (*PFNVNIPROC)(TMHVni hVni, TMLParam lData, TMClock tmNow, TMClock tmVBlank);

TM_CAPI TMHVni VniCreate(TMHVni hVni, PFNVNIPROC proc, TMLParam data = 0, EVniPriority ePriority = eVniPriorityLow);
TM_CAPI void VniKill(TMHVni hVni);
TM_CAPI TMClock VniGetClock();
TM_CAPI TMClock VniGetDelay();



///////////////////////////////////////////////////////////////////////////////
// Ease; see https://docs.microsoft.com/en-us/dotnet/framework/wpf/graphics-multimedia/easing-functions
#define AW_ERASEMODEOUT             0x00000000 // default value.
#define AW_ERASEMODEIN              0x10000000
#define AW_ERASEMODEINOUT           0x20000000
#define AW_ERASEMODEMASK            0x30000000

struct IEase : public IUnknown
{
public:
    virtual float Ease(float dTime) = 0; // dTime: 0 - 1.0;
};

TM_CAPI float EaseCalcProgress(IEase* pEase, TMUInt dwMode, float first, float last, float timeAll, float timePassed);

TM_CAPI void EaseCreateBack(IEase** ppEase, float nAmplitude = 1.0f); // nAmplitude: >= 0;
TM_CAPI void EaseCreateCircle(IEase** ppEase);
TM_CAPI void EaseCreateBounce(IEase** ppEase, int nBounces = 3, float nBounciness = 2.0f); // nBounces: >= 0; nBounciness: > 1;
TM_CAPI void EaseCreateElastic(IEase** ppEase, int nOscillations = 3, float nSpringiness = 3.0f); // nOscillations >= 0; nSpringiness >= 0;
TM_CAPI void EaseCreateExponential(IEase** ppEase, float nExponent = 2.0f);  // nExponent == 0: out time = in time;
TM_CAPI void EaseCreatePower(IEase** ppEase, float nPower = 2.0f);  // nPower >= 0; 1:out time = in time;
TM_CAPI void EaseCreateSine(IEase** ppEase);



TM_BGN_NAMESPACE


///////////////////////////////////////////////////////////////////////////////
// CAniAutoLock
struct CAniAutoLock
{
    CAniAutoLock() { AniLock(); }
    ~CAniAutoLock() { AniUnlock(); }
};

///////////////////////////////////////////////////////////////////////////////
// CAniIDBase
TM_BGN_PRIVATE_NAMESPACE

class CAniIDBase
{
public:
    TMHAni      m_tmID;

    TM_NO_COPYABLE(CAniIDBase);

protected:
    TMHAni _SetAni(TMUInt interval, PFNANIPROC proc, TMLParam data = 0)
    {
        m_tmID = AniCreate(m_tmID, interval, proc, data);
        return *this;
    }

public:
    CAniIDBase ()
        : m_tmID(0)
    {
    }

    ~CAniIDBase()
    {
        KillAni();
    }

    operator TMHAni () const { return m_tmID; }
    
    void KillAni()
    {
        if (m_tmID)
        {
            AniKill(m_tmID);
            m_tmID = 0;
        }
    }

    void SetDelay(TMUInt nInterval)
    {
        DbgAssert(*this);
        AniSetDelay(m_tmID, nInterval);
    }

    TMUInt GetDelay() const
    {
        DbgAssert(*this);
        return AniGetDelay(m_tmID);
    }

    void SetCurrentFrame(int nCur)
    {
        DbgAssert(*this);
        AniSetCurrentFrame(m_tmID, nCur);
    }

    TMUInt GetCurrentFrame()
    {
        return AniGetCurrentFrame(m_tmID);
    }

    TMUInt GetPassed()
    {
        return AniGetPassed(m_tmID);
    }

    void Pause()
    {
        AniPause(m_tmID);
    }

    void Resume()
    {
        AniResume(m_tmID);
    }
};

TM_END_PRIVATE_NAMESPACE


///////////////////////////////////////////////////////////////////////////////
// CAniImpl
// class AAA : public TM::CAniImpl<AAA>
// {
//   void Start() { this->SetAni(); }
//   void OnAni(TMHVni hVni, TMClock tmNow, TMClock tmVBlank) { ... }
// };
template <class T>
class CAniImpl : public PRI::CAniIDBase
{
private:
    static void _AniProc(TMHAni hVni, TMLParam lData, TMClock tmNow, TMUInt nFrame)
    {
        T* pthis = reinterpret_cast<T*>(lData);
        pthis->OnAni(hVni, tmNow, nFrame);
    }

public:
    TMHAni SetAni(TMUInt interval)
    {
        T* pthis = static_cast<T*>(this);
        return this->_SetAni(interval, _AniProc, (TMLParam)pthis);
    }

//     void OnAni(TMHAni hAni, TMClock tmNow, TMUInt nFrame) { ... }
//     void OnAni(TMHAni hAni, ...) { ... }
};


///////////////////////////////////////////////////////////////////////////////
// CAniIDn
// class AAA
// {
//   TM::CAniID1 ani1;
//   TM::CAniID2 ani2;
//   TM::CAniID3 ani3;
//
//   void Start() { ani1.SetAni(this, 5); ani2.SetAni(this, 6); ani3.SetAni(this, 7); }
//
//   void OnAni(const TM::CAniID1&, TMClock tmNow, TMUInt nFrame) { ... }
//   void OnAni(const TM::CAniID2&, TMClock tmNow, TMUInt nFrame) { ... }
//   void OnAni(const TM::CAniID3&, ...) { ... }
// };
template <int n = 0>
class CAniIDn : public PRI::CAniIDBase
{
private:
    template <class T_this, class T_Ani> struct _FuncProxy
    {
        static void _AniProc(TMHAni hAni, TMLParam lData, TMClock tmNow, TMUInt nFrame)
        {
            T_this* pthis = reinterpret_cast<T_this*>(lData);
            pthis->OnAni(*(const T_Ani*)&hAni, tmNow, nFrame);
        }
    };
    
public:
    template <class T> TMHAni SetAni(T* pthis, TMUInt interval)
    {
        return this->_SetAni(interval, _FuncProxy<T, CAniIDn>::_AniProc, (TMLParam)pthis);
    }
    
//    void OnAni(const TM::CAniIDX&, TMClock tmNow, TMUInt nFrame);
//    void OnAni(const TM::CAniIDX&, ...);
//    void OnAni(TMHAni hAni, TMClock tmNow, TMUInt nFrame);
//    void OnAni(...);
};

typedef CAniIDn<0>      CAniID;
typedef CAniIDn<0>      CAniID0;
typedef CAniIDn<1>      CAniID1;
typedef CAniIDn<2>      CAniID2;
typedef CAniIDn<3>      CAniID3;
typedef CAniIDn<4>      CAniID4;
typedef CAniIDn<5>      CAniID5;


TM_BGN_PRIVATE_NAMESPACE
///////////////////////////////////////////////////////////////////////////////
// CVniIDBase
class CVniIDBase
{
public:
    TMHVni      m_tmID;
    
    TM_NO_COPYABLE(CVniIDBase);
    
protected:
    TMHVni _SetAni(PFNVNIPROC pProc, TMLParam data = 0, EVniPriority ePriority = eVniPriorityLow)
    {
        m_tmID = VniCreate(m_tmID, pProc, data, ePriority);
        return m_tmID;
    }

public:
    CVniIDBase() : m_tmID(0) {}
    
    ~CVniIDBase() { KillAni(); }

    operator TMHVni() const { return m_tmID; }

    static TMClock GetClock()
    {
        return VniGetClock();
    }

    static TMClock GetDelay()
    {
        return VniGetDelay();
    }
    
    void KillAni()
    {
        if (m_tmID)
        {
            VniKill(m_tmID);
            m_tmID = 0;
        }
    }
};

TM_END_PRIVATE_NAMESPACE


///////////////////////////////////////////////////////////////////////////////
// CVniImpl
// class AAA : public TM::CVniImpl<AAA>
// {
//   void Start() { this->SetAni(); }
//   void OnAni(TMHVni hVni, TMClock tmNow, TMClock tmVBlank) { ... }
// };
template <class T>
class CVniImpl : public PRI::CVniIDBase
{
private:
    static void _VniProc(TMHVni hVni, TMLParam lData, TMClock tmNow, TMClock tmVBlank)
    {
        T* pthis = reinterpret_cast<T*>(lData);
        pthis->OnAni(hVni, tmNow, tmVBlank);
    }

public:
    TMHVni SetAni(EVniPriority ePriority = eVniPriorityLow)
    {
        T* pthis = static_cast<T*>(this);
        return this->_SetAni(_VniProc, (TMLParam)pthis, ePriority);
    }

//     void OnAni(TMHVni hVni, TMClock tmNow, TMClock tmVBlank);
//     void OnAni(TMHAni hVni, ...) { ... }
};


///////////////////////////////////////////////////////////////////////////////
// CVniIDn
// class AAA
// {
//   TM::CVniID1 ani1;
//   TM::CVniID2 ani2;
//   TM::CVniID3 ani3;
//
//   void Start() { ani1.SetAni(this); ani2.SetAni(this); ani3.SetAni(this); }
//
//   void OnAni(const TM::CVniID1&, TMClock tmNow, TMClock tmVBlank) { ... }
//   void OnAni(const TM::CVniID2&, TMClock tmNow, TMClock tmVBlank) { ... }
//   void OnAni(const TM::CVniID3&, ...) { ... }
// };
template <int n = 0>
class CVniIDn : public PRI::CVniIDBase
{
private:
    template <class T_this, class T_Vni> struct _FuncProxy
    {
        static void _VniProc(TMHVni hVni, TMLParam lData, TMClock tmNow, TMClock tmVBlank)
        {
            T_this* pthis = reinterpret_cast<T_this*>(lData);
            pthis->OnAni(*(const T_Vni*)&hVni, tmNow, tmVBlank);
        }
    };

public:
    // SetAni(T* pthis, EVniPriority ePriority = xxx) build error C2587 on VC6.
    template <class T> TMHVni SetAni(T* pthis, EVniPriority ePriority)
    {
        return this->_SetAni(_FuncProxy<T, CVniIDn>::_VniProc, (TMLParam)pthis, ePriority);
    }

    template <class T> TMHVni SetAni(T* pthis)
    {
        return this->_SetAni(_FuncProxy<T, CVniIDn>::_VniProc, (TMLParam)pthis, eVniPriorityLow);
    }

//    void OnAni(const TM::CVniIDX&, TMClock tmNow, TMClock tmVBlank);
//    void OnAni(const TM::CVniIDX&, ...);
//    void OnAni(TMHVni hVni, TMClock tmNow, TMClock tmVBlank);
//    void OnAni(...);
};

typedef CVniIDn<0>      CVniID;
typedef CVniIDn<0>      CVniID0;
typedef CVniIDn<1>      CVniID1;
typedef CVniIDn<2>      CVniID2;
typedef CVniIDn<3>      CVniID3;
typedef CVniIDn<4>      CVniID4;
typedef CVniIDn<5>      CVniID5;


// https://github.com/MrRaindrop/cubicbezier
struct CSVGCubicBezier
{
    float ax,bx,cx;
    float ay,by,cy;
    
    void Init(float p1x, float p1y, float p2x, float p2y)
    {
        // Calculate the polynomial coefficients,
        // implicit first and last control points are (0,0) and (1,1).
        ax = 3 * p1x - 3 * p2x + 1;
        bx = 3 * p2x - 6 * p1x;
        cx = 3 * p1x;
    
        ay = 3 * p1y - 3 * p2y + 1;
        by = 3 * p2y - 6 * p1y;
        cy = 3 * p1y;
    }
    
    float sampleCurveDerivativeX(float t)
    {
        // `ax t^3 + bx t^2 + cx t' expanded using Horner 's rule.
        return (3 * ax * t + 2 * bx) * t + cx;
    }
    
    float sampleCurveX(float t)
    {
        return ((ax * t + bx) * t + cx) * t;
    }
    
    float sampleCurveY(float t)
    {
        return ((ay * t + by) * t + cy) * t;
    }
    
    // Given an x value, find a parametric value it came from.
    float solveCurveX(float x)
    {
        float ZERO_LIMIT = 1e-4f;// CFloat::Epsilon();

        float t2 = x;
        
        // https://trac.webkit.org/browser/trunk/Source/WebCore/platform/animation
        // First try a few iterations of Newton's method -- normally very fast.
        // http://en.wikipedia.org/wiki/Newton's_method
        for (float i = 0; i < 8; i++)
        {
            // f(t)-x=0
            float x2 = sampleCurveX(t2) - x;
            if (CFloat::Equal0(x2, ZERO_LIMIT))// fabs(x2) < ZERO_LIMIT)
                return t2;
            float derivative = sampleCurveDerivativeX(t2);
            // == 0, failure
            if (CFloat::Equal0(derivative, ZERO_LIMIT)) // fabs(derivative) < ZERO_LIMIT)
                break;
            t2 -= x2 / derivative;
        }
        
        // Fall back to the bisection method for reliability.
        // bisection
        // http://en.wikipedia.org/wiki/Bisection_method
        float t1 = 1, t0 = 0;
        while (t1 > t0)
        {
            float x2 = sampleCurveX(t2) - x;
            if (CFloat::Equal0(x2, ZERO_LIMIT)) // fabs(x2) < ZERO_LIMIT)
                return t2;
            if (x2 > 0)
                t1 = t2;
            else
                t0 = t2;
            t2 = (t1 + t0) / 2;
        }
        
        // Failure
        return t2;
    }
    
    float solve(float x) // 0.0 - 1.0f.
    {
        return sampleCurveY(solveCurveX(x));
    }
};

// struct CSVGCubicBezier
// {
//     float ax_,bx_,cx_;
//     float ay_,by_,cy_;
//     
//     void Init(float p1x, float p1y, float p2x, float p2y)
//     {
//         // Calculate the polynomial coefficients,
//         // implicit first and last control points are (0,0) and (1,1).
//         cx_ = 3.0 * p1x;
//         bx_ = 3.0 * (p2x - p1x) - cx_;
//         ax_ = 1.0 - cx_ - bx_;
//         
//         cy_ = 3.0 * p1y;
//         by_ = 3.0 * (p2y - p1y) - cy_;
//         ay_ = 1.0 - cy_ - by_;
//     }
//     
//     float sampleCurveDerivativeX(float t)
//     {
//         // `ax t^3 + bx t^2 + cx t' expanded using Horner 's rule.
//         return (3 * ax * t + 2 * bx) * t + cx;
//     }
//     
//     float sampleCurveX(float t)
//     {
//         return ((ax * t + bx) * t + cx) * t;
//     }
//     
//     float sampleCurveY(float t)
//     {
//         return ((ay * t + by) * t + cy) * t;
//     }
//     
//     // Given an x value, find a parametric value it came from.
//     float solveCurveX(float x)
//     {
//         float ZERO_LIMIT = CFloat::Epsilon();
//         
//         float t2 = x;
//         
//         // https://trac.webkit.org/browser/trunk/Source/WebCore/platform/animation
//         // First try a few iterations of Newton's method -- normally very fast.
//         // http://en.wikipedia.org/wiki/Newton's_method
//         for (float i = 0; i < 8; i++)
//         {
//             // f(t)-x=0
//             float x2 = sampleCurveX(t2) - x;
//             if (fabs(x2) < ZERO_LIMIT)
//                 return t2;
//             float derivative = sampleCurveDerivativeX(t2);
//             // == 0, failure
//             if (fabs(derivative) < ZERO_LIMIT)
//                 break;
//             t2 -= x2 / derivative;
//         }
//         
//         // Fall back to the bisection method for reliability.
//         // bisection
//         // http://en.wikipedia.org/wiki/Bisection_method
//         float t1 = 1, t0 = 0;
//         while (t1 > t0)
//         {
//             float x2 = sampleCurveX(t2) - x;
//             if (fabs(x2) < ZERO_LIMIT)
//                 return t2;
//             if (x2 > 0)
//                 t1 = t2;
//             else
//                 t0 = t2;
//             t2 = (t1 + t0) / 2;
//         }
//         
//         // Failure
//         return t2;
//     }
//     
//     float solve(float x) // 0.0 - 1.0f.
//     {
//         return sampleCurveY(solveCurveX(x));
//     }
// };



TM_END_NAMESPACE


#endif // TMANI_H
