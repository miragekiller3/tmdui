/*
 *  @file
 *  @brief  time object
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2005.11.07
 *
 *  Copyright (C) 2005 miragekiller
 */

///////////////////////////////////////////////////////////////////////////////
// vc6 tzset have bug:
// putenv("TZ=0");
// tzset(); 
// putenv("TZ=");
// tzset(); 
// putenv("TZ=0");
// tzset();  // tzset failed. 
// because VC6 source code have cache: lastTZ.
// tzset("TZ=0") { set lastTZ = "TZ=0" }
// tzset("TZ=") { return; not reset lastTZ; }
// tzset("TZ=0") { if (new tz == lastTZ) return; then failed. }



///////////////////////////////////////////////////////////////////////////////
//  Format description
//   sample: %Y-%m-%d %X   1980-02-32 19:40:30
//
//  format  descriptions                                             samples 
//  %a      Abbreviations of week day                                Tue 
//  %A      Full week day name                                       Tuesday  
//  %b      Abbreviations of month                                   Jan  
//  %B      Full month name                                          January  
//  %c      date and time                                            Wed Aug 17 19:40:30 2005
//  %C    * the last 2 digits of year
//  %d      month day: [01, 31]                                      14
//  %D    * month/day/year
//  %e    * in two character domain, decimal digits indicate the day of every month  
//  %F    * year-month-day
//  %g    * the last digits of year, year based on week 
//  %G    * year,year based on week
//  %H      hour (24 hours every day): [00, 23]                      19  
//  %I      hour (each 12 hours for am and pm[01, 12]                07  
//  %j      year day: [001, 366]                                     014  
//  %m      month: [01, 12]                                          01  
//  %M      minute: [00, 59]                                         40  
//  %p      A M / P M                                                PM
//  %r    * time of 12 hours 
//  %R    * show hour and minute : hh:mm
//  %S      second:[00, 61]
//  %t    * horizontal tab
//  %T    * show hour minute second : hh:mm:ss
//  %u    * day of every week,Monday is the 1st day (value from 0 to 6, Monday is 0)
//  %U      week of every year,Sunday is the 1st day [00, 53]        02
//  %V    * week of every year,year based on week 
//  %w      week day showed by decimal: [ 0 = Monday, 6 ]            2 
//  %W      week of every year, Monday is the 1st day [00, 53]       02  
//  %x      date                                                     08/17/05 
//  %X      time                                                     19:40:30 
//  %y      non-AD year: [00, 991]                                   05  
//  %Y      AD year                                                  2005 
//  %Z      time zone                                                MST  
///////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef TMTIME_H
#define TMTIME_H

#include <time.h>
#include "tmdef.h"
#include "tmtchar.h"

#ifdef TM_MWERKS
#   include <wtime.h>
#endif

#ifdef TM_WIN
#   include <windows.h>
#   include <mmsystem.h>
#   pragma comment (lib, "Winmm.lib")
#endif

#ifdef TM_GNU
#   include <sys/time.h>
#endif

#ifdef TM_QT
#   include <QElapsedTimer>
#endif

#if TM_CPP >= TM_CPP11
#   include <chrono>
#endif

// #ifdef TM_MSC
// #   pragma warning (push)
// #   pragma warning (disable : 4995) // '_snprintf': name was marked as #pragma deprecated
// #   pragma warning (disable : 4996) // '_vsnprintf': This function or variable may be unsafe. Consider using _vsnprintf_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
// #endif // #ifdef TM_MSC

#define StrFTimeA       strftime
#define StrFTimeW       wcsftime
#define StrFTimeT       TM_SW_UNICODE(StrFTimeW, StrFTimeA)
#define StrFTime        i_str_ftime<tm>::_impl

#ifdef TM_WIN
#   define __tzset _tzset
#else
#   define __tzset tzset
#endif // TM_WIN

inline static void TimeGetLocal(struct tm& _tm, time_t tmGmt)
{
    DbgAssert(tmGmt != -1);
#ifdef __STDC_WANT_SECURE_LIB__
    localtime_s(&_tm, &tmGmt);
#elif defined(_POSIX_THREAD_SAFE_FUNCTIONS) && _POSIX_THREAD_SAFE_FUNCTIONS >= 0
    localtime_r(&tmGmt, &_tm);
#else
    _tm = *localtime(&tmGmt);
#endif
}

inline static struct tm TimeGetLocal(time_t tmGmt)
{
    struct tm _tm;
    TimeGetLocal(_tm, tmGmt);
    return _tm;
}

inline static void TimeGetGMT(struct tm& _tm, time_t tmGmt)
{
    DbgAssert(tmGmt != -1);
#ifdef __STDC_WANT_SECURE_LIB__
    gmtime_s(&_tm, &tmGmt);
#elif defined(_POSIX_THREAD_SAFE_FUNCTIONS) && _POSIX_THREAD_SAFE_FUNCTIONS >= 0
    gmtime_r(&tmGmt, &_tm);
#else
    _tm = *gmtime(&tmGmt);
#endif
}

inline static struct tm TimeGetGMT(time_t tmGmt)
{
    struct tm _tm;
    TimeGetGMT(_tm, tmGmt);
    return _tm;
}

#ifdef TM_WIN

inline static void TimeToFileTime(FILETIME& ft, time_t tmGmt)
{
    DbgAssert(tmGmt != -1);
    LONGLONG ll = Int32x32To64(tmGmt, 10000000) + 116444736000000000;
    ft.dwLowDateTime = (DWORD)ll;
    ft.dwHighDateTime = (DWORD)(ll >>32);
}

inline static time_t TimeFromFileTime(const FILETIME& ft)
{
    LONGLONG ui = (((LONGLONG)ft.dwHighDateTime) << 32) + ft.dwLowDateTime;
    time_t tm = (time_t)((LONGLONG)(ui - 116444736000000000) / 10000000);
    return tm;
}

inline static void TimeToSYSTEMTIME(SYSTEMTIME& _tm, const tm& tm)
{
    _tm.wYear = (WORD)(1900 + tm.tm_year);
    _tm.wMonth = (WORD)(1 + tm.tm_mon);
    _tm.wDayOfWeek = (WORD)tm.tm_wday;
    _tm.wDay = (WORD)tm.tm_mday;
    _tm.wHour = (WORD)tm.tm_hour;
    _tm.wMinute = (WORD)tm.tm_min;
    _tm.wSecond = (WORD)tm.tm_sec;
    _tm.wMilliseconds = 0;
}

inline static void TimeToSYSTEMTIME(SYSTEMTIME& _tmGMT, time_t tmGmt)
{
    DbgAssert(tmGmt != -1);
    TimeToSYSTEMTIME(_tmGMT, TimeGetGMT(tmGmt));
}

inline static time_t TimeFromSYSTEMTIME(const SYSTEMTIME& tmGmt)
{
    FILETIME ft;
    if (!SystemTimeToFileTime(&tmGmt, &ft))
        return (time_t)-1;
    return TimeFromFileTime(ft);
}

inline static void TimeToLocalTime(SYSTEMTIME& _tmLocal, time_t tmGmt)
{
    DbgAssert(tmGmt != -1);
    struct tm ptm;
    TimeGetLocal(ptm, tmGmt);
    TimeToSYSTEMTIME(_tmLocal, ptm);
}

// var is system time
inline static void TimeToVariant(VARIANT& varUTC, time_t tmGmt)
{
    VariantClear(&varUTC);
    time_t nday = tmGmt / (24 * 60 * 60);
    time_t nSec = tmGmt - nday * 24 * 60 * 60;
    varUTC.dblVal = nday + ((double)nSec / 86400.) + 25569;
    varUTC.vt = VT_DATE;
}

inline static time_t TimeFromVariant(const VARIANT& varUTC)
{
    double dbn = varUTC.dblVal;
    if (varUTC.vt != VT_DATE)
    {
        VARIANT v2 = { 0 };
        if (SUCCEEDED(VariantChangeType(&v2, (VARIANT*)&varUTC, 0, VT_DATE)))
            dbn = v2.dblVal;
        else
            return -1;
    }
    
    time_t nday = (time_t) (dbn - 25569.);
    time_t nSec = (time_t) ((dbn - (time_t)dbn) * 86400. + 0.5);
    time_t tm = nday * 24 * 60 * 60 + nSec;
    return tm;
}

#endif // TM_WIN

inline static time_t TimeMakeGMT(struct tm& tmGmt)
{
#if defined(TM_MSC) && (TM_MSC < 1300)
    SYSTEMTIME st;
    TimeToSYSTEMTIME(st, tmGmt);
    return TimeFromSYSTEMTIME(st);
#elif defined(TM_MSC) || defined(TM_MINGW)
    return _mkgmtime(&tmGmt);
#else
    return timegm(&tmGmt);
#endif
}


// #define TMClock         TMUInt32    // MS

inline TMClock TimeClock()
{
#if TM_WIN
    return timeGetTime();
#elif defined(TM_GTK)
    return (TMClock)(g_get_monotonic_time() / 1000);
#else
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (TMClock)((TMUInt64)(ts.tv_sec) * 1000 + ts.tv_nsec / 1000000);
#endif 
}

#ifdef __cplusplus

template < class T > struct i_str_ftime
{
    inline static size_t _impl(TMStrA pDest, size_t dwSize, TMCStrA pFmt, tm* tm) {
        return strftime(pDest, dwSize, pFmt, tm); }
    
    inline static size_t _impl(TMStrW pDest, size_t dwSize, TMCStrW pFmt, tm* tm) {
        return wcsftime(pDest, dwSize, pFmt, tm); }
};


TM_BGN_NAMESPACE

class CSeconds
{
private:
    time_t m_tm;

public:
    CSeconds(time_t sec = 0) : m_tm(sec) {}
    CSeconds(time_t nd, time_t nh, time_t nm, time_t ns)
        : m_tm(ns + 60*nm + 60*60*nh + 60*60*24*nd)
    {
    }
    
//        CSeconds(const CSeconds& rhs) : m_tm(rhs.m_tm) {}
//        const CSeconds& operator = (const CSeconds& rhs) {
//            m_tm = rhs.m_tm; return *this; }
    
    const CSeconds& operator = (time_t sec) { m_tm = sec; return *this; }
    operator time_t () const { return m_tm; }

    time_t GetDays() const { return m_tm / (24 * 3600); }
    time_t GetHours() const { return m_tm / 3600; }
    time_t GetHourOfDay() const { return GetHours() - GetDays()*24; }
    time_t GetMinutes() const { return m_tm / 60; }
    time_t GetMinuteOfHour() const { return GetMinutes() - GetHours()*60; }
    time_t GetSeconds() const { return m_tm; }
    time_t GetSecondOfMinute() const { return m_tm % 60; }

    time_t GetMinuteStartTime() const { return (m_tm / 60) * 60; }
    time_t GetHourStartTime() const { return (m_tm / (60 * 60)) * 60 * 60; }
    time_t GetDayStartTime() const { return (m_tm / (60 * 60 * 24)) * 60 * 60 * 24; }
    
    CSeconds operator - (CSeconds rhs) const { return m_tm - rhs.m_tm; }
    CSeconds operator + (CSeconds rhs) const { return m_tm + rhs.m_tm; }
    const CSeconds& operator += (CSeconds rhs) { m_tm += rhs.m_tm; return *this; }
    const CSeconds& operator -= (CSeconds rhs) { m_tm -= rhs.m_tm; return *this; }
    TMBool operator == (CSeconds rhs) const { return m_tm == rhs.m_tm; }
    TMBool operator != (CSeconds rhs) const { return m_tm != rhs.m_tm; }
    TMBool operator <  (CSeconds rhs) const { return m_tm < rhs.m_tm; }
    TMBool operator >  (CSeconds rhs) const { return m_tm > rhs.m_tm; }
    TMBool operator <= (CSeconds rhs) const { return m_tm <= rhs.m_tm; }
    TMBool operator >= (CSeconds rhs) const { return m_tm >= rhs.m_tm; }
};

class CTime
{
public:
    // this time is always GMT time
    time_t m_tm;

//     private:
// #if !(defined (TM_MSC) && (TM_MSC >= 1400)) // 1400: vs2005
//         struct CGmtSet
//         {
//             char cOld[64];
//             CGmtSet ()
//             {
//                 cOld[0] = 'T';
//                 cOld[1] = 'Z';
//                 cOld[2] = '=';
//                 const char* ptz = getenv("TZ");
//                 if (ptz)
//                     StrCopy(&(cOld[3]), 50, ptz);
//                 else
//                     cOld[3] = 0;
//                 // set current tomezone to GMT-0
//                 char env[] = "TZ=0";
//                 putenv(env);
//                 tzset();
//             }
//             ~CGmtSet ()
//             {
//                 // restore old timezome
//                 putenv(cOld);
//                 tzset();
//             }
//         };
// #endif 

private:
    template < class _Str, class _Fmt >
        size_t _fmt(_Str& __str, const _Fmt* pFmt, tm* ptm)
    {
        // ensure fmt and str have the same type,both are char or wchar
        TM_D(const _Fmt* p = __str);

        DbgAssert(ptm && pFmt);

        int i = 512;
        int j;
        
        while (! (j = StrFTime(__str.GetBuffer(i), i, pFmt, ptm)))
        { i *= 2; }

        __str.SetLength(j);
        return j;
    }

public:
    static CTime GetCurrentTime() { return ::time(0); }
    
    static void ResetTimeZone()
    {
#if !TM_WIN_METRO
        __tzset();
#endif
    }
    
    // difference in seconds between GMT and local time 
//      static time_t GetTimeZone() { return timezone; }

//      // offset for Daylight Saving Time
//         static time_t GetDstBias() { return _dstbias; }
    
public:
    CTime(time_t _tm = -1) : m_tm(_tm) {}
    CTime(const CTime& rhs) : m_tm(rhs.m_tm) {}
    
    // convert local time to GMT time
    CTime(tm& time, TMBool bLocal = true)
    {
        if (!bLocal)
            m_tm = TimeMakeGMT(time);
        else
            m_tm = mktime(&time);
    }
    
    // convert local time to GMT time
    // Year, Month, Day, Hour, Min, Sec, bDST
    CTime(int y, int m, int d, int h, int mm, int s, int nDST = -1, TMBool bLocal = true)
    {
        DbgAssert(d >= 1 && d <= 31);
        DbgAssert(m >= 1 && m <= 12);
        DbgAssert(y >= 1970);
        
#ifdef TM_WIN
        if (!bLocal)
        {
            SYSTEMTIME st;
            st.wYear = y;
            st.wMonth = m;
            st.wDay = d;
            st.wHour = h;
            st.wMinute = mm;
            st.wSecond = s;
            st.wMilliseconds = 0;
            *this = CTime(st, false);
        }
        else
#endif
        {
            tm _tm = {s, mm, h, d, m - 1, y - 1900, 0, 0, nDST};
            *this = CTime(_tm, bLocal);
        }
    }
    
#ifdef TM_WIN
    // convert local time to GMT time
    CTime(WORD wDosDate, WORD wDosTime, int nDST = -1)
    {
        *this = CTime(
            (wDosDate >> 9) + 80,
            ((wDosDate & ~0xFE00) >> 5) - 1,
            wDosDate & ~0xFFE0,
            wDosTime >> 11,
            (wDosTime & ~0xF800) >> 5,
            (wDosTime & ~0xFFE0) << 1,
            nDST);
    }
    
    // convert local time to GMT time
    CTime(const SYSTEMTIME& sysTime, TMBool bLocal = true, int nDST = -1)
    {
        if (bLocal)
        {
            *this = CTime(sysTime.wYear, sysTime.wMonth, sysTime.wDay,
                sysTime.wHour, sysTime.wMinute, sysTime.wSecond, nDST);
        }
        else
            m_tm = TimeFromSYSTEMTIME(sysTime);
    }

    // var is system time
    CTime(const VARIANT& var)
    {
        m_tm = TimeFromVariant(var);
    }

    // file time is UTC time
    CTime(const FILETIME& ft)
    {
        m_tm = TimeFromFileTime(ft);
    }
    
#endif // TM_WIN
    
    CTime& operator = (const CTime& rhs) { m_tm = rhs.m_tm; return *this; }
    CTime& operator = (time_t t) { m_tm = t; return *this; }
    
    TMBool IsValid() const { return m_tm != -1; }
    
    void GetGmtTm(tm& _tm) const
    {
        TimeGetGMT(_tm, m_tm);
    }

    void GetLocalTm(tm& _tm) const
    {
        TimeGetLocal(_tm, m_tm);
    }
    
#ifdef TM_WIN
    void GetAsSystemTime(SYSTEMTIME& _tm) const
    {
        TimeToSYSTEMTIME(_tm, m_tm);
    }

    void GetAsLocalTime(SYSTEMTIME& _tm) const
    {
        TimeToLocalTime(_tm, m_tm);
    }

    void GetAsFileTime(FILETIME& ft) const
    {
        TimeToFileTime(ft, m_tm);
    }

    // var is system time
    void GetAsVariant(VARIANT& var) const
    {
        TimeToVariant(var, m_tm);
    }
#endif // TM_WIN
    
    time_t GetTime()    const { return m_tm; }
    operator time_t()   const { return m_tm; }

    int GetYear()       const { return TimeGetLocal(m_tm).tm_year + 1900; }
    int GetMonth()      const { return TimeGetLocal(m_tm).tm_mon + 1; }
    int GetDay()        const { return TimeGetLocal(m_tm).tm_mday; } // day of month
    int GetHour()       const { return TimeGetLocal(m_tm).tm_hour; }
    int GetMinute()     const { return TimeGetLocal(m_tm).tm_min; }
    int GetSecond()     const { return TimeGetLocal(m_tm).tm_sec; }
    int GetDayOfWeek()  const { return TimeGetLocal(m_tm).tm_wday + 1; }  // 1=Sun, 2=Mon, ..., 7=Sat
    int GetDayOfWeek2() const { return TimeGetLocal(m_tm).tm_wday; }        // 0=Sun, 1=Mon, ..., 6=Sat
    int GetDayOfWeek3() const { int wd = TimeGetLocal(m_tm).tm_wday; return wd == 0 ? 7 : wd; }  // 1=Mon, ..., 6=Sat, 7=Sun
    
    // Operations
//         CSeconds operator - (CTime rhs) const { return m_tm - rhs.m_tm; }
//         CTime operator - (CSeconds sec) const { return m_tm - sec.GetSeconds(); }
//         CTime operator + (CSeconds sec) const { return m_tm + sec.GetSeconds(); }
//         const CTime& operator += (CSeconds sec) { m_tm += sec.GetSeconds(); return *this; }
//         const CTime& operator -= (CSeconds sec) { m_tm -= sec.GetSeconds(); return *this; }
//         TMBool operator == (CTime rhs) const { return m_tm == rhs.m_tm; }
//         TMBool operator != (CTime rhs) const { return m_tm != rhs.m_tm; }
//         TMBool operator <  (CTime rhs) const { return m_tm < rhs.m_tm; }
//         TMBool operator >  (CTime rhs) const { return m_tm > rhs.m_tm; }
//         TMBool operator <= (CTime rhs) const { return m_tm <= rhs.m_tm; }
//         TMBool operator >= (CTime rhs) const { return m_tm <= rhs.m_tm; }

    size_t Format(TMStrA pBuf, size_t nBuf, TMCStrA pFmt = "%c")
    {
        DbgAssert(m_tm != -1);
        return StrFTime(pBuf, nBuf, pFmt, tm_addressof(TimeGetLocal(m_tm)));
    }
    
    size_t Format(TMStrW pBuf, size_t nBuf, TMCStrW pFmt = L"%c")
    {
        DbgAssert(m_tm != -1);
        return StrFTime(pBuf, nBuf, pFmt, tm_addressof(TimeGetLocal(m_tm)));
    }

    size_t FormatGmt(TMStrA pBuf, size_t nBuf, TMCStrA pFmt = "%c")
    {
        DbgAssert(m_tm != -1);
        return StrFTime(pBuf, nBuf, pFmt, tm_addressof(TimeGetGMT(m_tm)));
    }
    
    size_t FormatGmt(TMStrW pBuf, size_t nBuf, TMCStrW pFmt = L"%c")
    {
        DbgAssert(m_tm != -1);
        return StrFTime(pBuf, nBuf, pFmt, tm_addressof(TimeGetGMT(m_tm)));
    }

};


struct CTm : public tm
{
public:
    CTm() {}
    
    CTm(const tm& rhs)
        : tm(rhs)
    {
    }

#ifdef TM_WIN
    CTm(const SYSTEMTIME& sysTime, TMBool isdst = true)
    {
        tm_year = sysTime.wYear - 1900;
        tm_mon = sysTime.wMonth - 1;
        tm_wday = sysTime.wDayOfWeek;
        tm_mday = sysTime.wDay;
        tm_hour = sysTime.wHour;
        tm_min = sysTime.wMinute;
        tm_sec = sysTime.wSecond;
        tm_isdst = isdst;
    }
#endif

    int GetYear() const { return tm_year + 1900; }
    int GetMonth() const { return tm_mon + 1; }
    int GetDay() const { return tm_mday; }
    int GetHour() const { return tm_hour; }
    int GetMinute() const { return tm_min; }
    int GetSecond() const { return tm_sec; }
    int GetDayOfWeek() const { return tm_wday + 1; }  // 1=Sun, 2=Mon, ..., 7=Sat

    size_t Format(TMStrW pBuf, size_t nBuf, TMCStrW pFmt = L"%c")
    {
        return StrFTime(pBuf, nBuf, pFmt, this);
    }

    size_t Format(TMStrA pBuf, size_t nBuf, TMCStrA pFmt = "%c")
    {
        return StrFTime(pBuf, nBuf, pFmt, this);
    }
};


class CClock
{
public:
    typedef TMClock clock_type;
    TMClock m_nNow;

public:
    CClock(TMClock clk) : m_nNow(clk) {}
    CClock() : m_nNow(TimeClock()) {}

    operator clock_type () const { return m_nNow; }

    void Assign(TMClock n) { m_nNow = n; }
    void Add(int n) { m_nNow += n; }

    void Reset() { m_nNow = TimeClock(); }

    int Elapse() const { return TimeClock() - m_nNow; }
    int ElapseMS() const { return Elapse(); }

    TMULong ToMillionSecond() const { return m_nNow; }

    int StepUp()
    {
        TMClock old = m_nNow;
        Reset();
        DbgAssert(m_nNow - old >= 0);
        return m_nNow - old;
    }
};


///////////////////////////////////////////////////////////////////////
// High-precision timer ,return clock cycle
// T_Impl: _CycleImplWin, _CycleImplQt, _CycleImplGtk, _CycleImplCpp, _CycleImplGnu
template <class T_Impl> class CCycleT : public T_Impl
{
private:
    TMInt64 m_now;

public:
    CCycleT(TMInt64 u) : m_now(u) { DbgAssert(u >= 0); }
    CCycleT() { Reset(); }

    operator TMInt64() const { return m_now; }

    CCycleT& operator += (TMInt64 n) { m_now += n; return *this; }

    double   ToSecondF()        const { return (double)m_now / (double)(T_Impl::Frequency()); }
    double   ToMillionSecondF() const { return (double)m_now / (double)(T_Impl::Frequency() / 1000); }
    TMUInt32 ToMillionSecond()  const { return (TMUInt32)(m_now / (T_Impl::Frequency() / 1000)); }
    TMUInt32 ToMicroSecond()    const { return (TMUInt32)(m_now / (T_Impl::Frequency() / 1000000)); }
    TMUInt32 ToNanoSecond()     const { return (TMUInt32)(m_now / (T_Impl::Frequency() / 1000000000)); }
    TMUInt32 ToTicket()         const { return (TMUInt32)(m_now % T_Impl::Frequency()); }

    void Reset() { m_now = T_Impl::Now(); DbgAssert(m_now >= 0); }

    CCycleT Elapse() const { TMInt64 dtm = T_Impl::Now() - m_now; return dtm; }

    TMUInt32 ElapseMS() const { return Elapse().ToMillionSecond(); }

    CCycleT StepUp()
    {
        TMInt64 now = m_now;
        Reset();
        TMInt64 dtm = m_now - now;
        return dtm;
    }
};

#ifdef TM_WIN
    struct _CycleBaseWin
    {
        static TMInt64 Frequency()
        {
            LARGE_INTEGER fre;
            QueryPerformanceFrequency(&fre);
            return fre.QuadPart;
        }
        static TMInt64 Now()
        {
            LARGE_INTEGER now;
            QueryPerformanceCounter(&now);
            return now.QuadPart;
        }

        static void BindCPU()
        {
    #if TM_WIN_DESKTOP
            //         SetThreadAffinityMask(GetCurrentThread(), 0x00000001);
    #endif
        }
    };

    typedef CCycleT<_CycleBaseWin>  CCycleWin;

#endif // TM_WIN

#ifdef TM_QT
    struct _CycleBaseQt
    {
        static tm_constexpr11 TMInt64 Frequency() { return 1000000000LL; }
        static TMInt64 Now()
        {
            static QElapsedTimer s_timer;
            static TMBool s_inited = (s_timer.start(), true);
            (void)s_inited;
#if QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)
            return (TMInt64)s_timer.nsecsElapsed();
#else
            return (TMInt64)s_timer.elapsed() * 1000000LL;
#endif
        }
    };
    typedef CCycleT<_CycleBaseQt>  CCycleQt;
#endif // TM_QT

// #ifdef TM_GTK
//     struct _CycleBaseGtk
//     {
//         static tm_constexpr11 TMInt64 Frequency() { return 1000000LL; }
//         static TMInt64 Now() { return (TMInt64)g_get_monotonic_time(); }
//     };
//     typedef CCycleT<_CycleBaseGtk>  CCycleGtk;
// #endif // TM_GTK

#ifdef TM_GNU
    struct _CycleBaseGnu
    {
        static tm_constexpr11 TMInt64 Frequency() { return 1000000000LL; }
        static TMInt64 Now()
        {
            timespec ts;
            clock_gettime(CLOCK_MONOTONIC, &ts);
            return (TMInt64)ts.tv_sec * 1000000000LL + (TMInt64)ts.tv_nsec;
        }
    };
    typedef CCycleT<_CycleBaseGnu>  CCycleGnu;
#endif // TM_GNU
    
#if TM_CPP >= TM_CPP11
    struct _CycleBaseCpp
    {
        static tm_constexpr11 TMInt64 Frequency() { return 1000000000LL; }
        static TMInt64 Now()
        {
            // Generic C++ fallback, also in nanoseconds.
            typedef std::chrono::steady_clock steady_clock_t;
            return (TMInt64)std::chrono::duration_cast<std::chrono::nanoseconds>(
                steady_clock_t::now().time_since_epoch()).count();
        }
    };
    typedef CCycleT<_CycleBaseCpp>  CCycleCpp;
#endif // TM_CPP


#ifdef TM_WIN
    typedef CCycleWin CCycle;
#elif defined TM_QT
    typedef CCycleQt CCycle;
// #elif defined TM_GTK
//     typedef CCycleGtk CCycle;
#elif defined TM_GNU
    typedef CCycleGnu CCycle;
#else
    typedef CCycleCpp CCycle;
#endif // TM_WIN



///////////////////////////////////////////////////////////////////////
// calc all times in a function by some calls
// sample:
// void aaa() { CTimesSum<101> s; ... } // call 3'rd, every call 1s
// void bbb() { CTimesSum<202> s; ... } // call 2'nd, every call 2s
// after process end; trace timesum 101: (1*3)=3s; trace timesum 202: (2*2)=4s
template <int n>
struct CTimesSum
{
    CCycle m_cyc;

    struct CTm
    {
        TMUInt64 m_cyc;
        int m_num;
        CTm() : m_cyc(0), m_num(0) {}
        ~CTm()
        {
            TMULong ms = CCycle(m_cyc).ToMillionSecond();
            DbgTrace("timesum id:%d - num:%d - %dms - avg:%d - avg:%dms", n, m_num, ms, m_cyc / m_num, ms / m_num);
        }
    };

    CTimesSum() {}

    ~CTimesSum()
    {
        static CTm tm;
        tm.m_cyc += m_cyc.Elapse();
        ++tm.m_num;
    }
};

template <int n> struct CNullTimesSum {};

#define CTimesSumDbg TM_SW_DBG(CTimesSum, CNullTimesSum)


////////////////////////////////////////////////////////////////////////////
// calc fps
// sample:
// void render()
// {
//     ...
//     static CFps fps;
//     fps.StepUp();
//     DbgTrace("fps:%g", fps.GetFPS());
// }
template <int nAvgNum = 30>
class CFpsT
{
private:
    CCycle m_cyc;
    CCycle m_AvgDTime;
    
public:
    CFpsT() : m_AvgDTime(1) {}

    void Reset()
    {
        m_cyc.Reset();
    }

    TMInt64 StepUp()
    {
        TMInt64 x = m_cyc.StepUp();
        m_AvgDTime = (m_AvgDTime * (nAvgNum - 1) + x) / nAvgNum;
        return x;
    }

    TMInt64 StepUp(TMInt64 nExtTime)
    {
        TMInt64 x = m_cyc.StepUp();
        m_AvgDTime = (m_AvgDTime * (nAvgNum - 1) + x + nExtTime) / nAvgNum;
        return x;
    }

    float GetFPS() const
    {
        return 1000.0 / m_AvgDTime.ToMillionSecondF() ;
    }

    CCycle GetAvgDTime() const
    {
        return m_AvgDTime;
    }
};

typedef CFpsT<30> CFps;

template <class T>
struct CAutoFpsT
{
    T& fps;

    CAutoFpsT(T& _fps)
        : fps(_fps)
    {
        fps.Reset();
    }

    ~CAutoFpsT()
    {
        fps.StepUp();
    }
};

typedef CAutoFpsT<CFps> CAutoFps;



TM_END_NAMESPACE



#endif // #ifdef __cplusplus


// #ifdef TM_MSC
// #   pragma warning (pop)
// #endif // #ifdef TM_MSC

#endif // TMTIME_H

