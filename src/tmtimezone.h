/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2015.12.1
 *
 *  Copyright (C) 2015 miragekiller
 */

#pragma once

#ifndef TMTIMEZONE_H
#define TMTIMEZONE_H

#include "tmtime.h"
#include "tmstrview.h"

#ifndef TM_WIN
#   error only support windows
#endif


#if (TM_MSC < 1300) && (_WIN32_WINNT < 0x0501)

    DECLARE_DLL_FUNC(dll_kernel32, BOOL, WINAPI, TzSpecificLocalTimeToSystemTime,
        (LPTIME_ZONE_INFORMATION lpTimeZoneInformation, LPSYSTEMTIME lpLocalTime, LPSYSTEMTIME lpUniversalTime));
#   define TzSpecificLocalTimeToSystemTime _TzSpecificLocalTimeToSystemTime

#endif

TM_BGN_NAMESPACE

//                    SYSTEMTIME:                               tm
// wYear:             current year                              current year - 1900
// wMonth:            January = 1, February = 2, ...            0 – 11; January = 0;

// wDayOfWeek:                    Sunday = 0, Monday = 1, ....              
struct CTimeZone : TIME_ZONE_INFORMATION
{
    CTimeZone()
    {
        MemZeroT(*(TIME_ZONE_INFORMATION*)this);
    }

    void SetLocalAM()
    {
        this->Bias = 300;
        this->StandardBias = 0;
        this->StandardDate.wMonth = 11;
        this->StandardDate.wDay = 1;
        this->StandardDate.wHour = 2;
        StrCopy(this->StandardName, 32, L"东部标准时间");
        this->DaylightBias = -60;
        this->DaylightDate.wMonth = 3;
        this->DaylightDate.wDay = 2;
        this->DaylightDate.wHour = 2;
        StrCopy(this->DaylightName,32, L"东部夏季时间");
    }

    TMBool SystemTimeToLocalTime(SYSTEMTIME& lpLocalTime, const SYSTEMTIME& lpUniversalTime) const
    {
        return SystemTimeToTzSpecificLocalTime(
            (LPTIME_ZONE_INFORMATION)(this),
            (LPSYSTEMTIME)(&lpUniversalTime),
            &lpLocalTime);
    }

    TMBool SystemTimeToLocalTime(SYSTEMTIME& lpLocalTime, time_t tmUniversalTime) const
    {
        CTime tm(tmUniversalTime);
        SYSTEMTIME systm;
        tm.GetAsSystemTime(systm);
        return SystemTimeToLocalTime(lpLocalTime, systm);
    }

    TMBool SystemTimeToLocalTime(tm& _tm, time_t time) const
    {
        SYSTEMTIME st;
        if (!SystemTimeToLocalTime(st, time))
            return false;
        TM::CTm tim(st);;
        _tm = tim;
        return true;
    }
    
    // y,m,d,h,m,s use SYSTEMTIME format
    TMBool LocalTimeToSystemTime(time_t& tmUniversalTime, int y, int m, int d, int h, int M, int s) const
    {
        SYSTEMTIME st;
        st.wYear = y;
        st.wMonth = m;
        st.wDay = d;
        st.wHour = h;
        st.wMinute = M;
        st.wSecond = s;
        st.wMilliseconds = 0;
        return LocalTimeToSystemTime(tmUniversalTime, st);
    }
    
    TMBool LocalTimeToSystemTime(SYSTEMTIME& lpUniversalTime, const SYSTEMTIME& lpLocalTime) const
    {
        return TzSpecificLocalTimeToSystemTime(
            (LPTIME_ZONE_INFORMATION)this,
            (LPSYSTEMTIME)&lpLocalTime,
            &lpUniversalTime);
    }
    
    TMBool LocalTimeToSystemTime(time_t& tmUniversalTime, const SYSTEMTIME& lpLocalTime) const
    {
        SYSTEMTIME systm;
        if (LocalTimeToSystemTime(systm, lpLocalTime))
        {
            TM::CTime tm(systm, false);
            tmUniversalTime = tm;
            return true;
        }
        return false;
    }

    //yyyymmdd HH:MM:SS
    TMBool LocalTimeToSystemTime(time_t& tmUniversalTime, LPCTSTR strTm) const
    {
        LPCTSTR p = strTm;
        long y = TM::CStrView(p, 4).ToLong(0, 10);
        long m = TM::CStrView(p + 4, 2).ToLong(0, 10);
        long d = TM::CStrView(p + 6, 2).ToLong(0, 10);
        if (p[8] == 0)
            return LocalTimeToSystemTime(tmUniversalTime, y, m, d, 0, 0, 0);
        long h = TM::CStrView(p + 9, 2).ToLong(0, 10);
        long mm = TM::CStrView(p + 12, 2).ToLong(0, 10);
        long s = TM::CStrView(p + 15, 2).ToLong(0, 10);
        return LocalTimeToSystemTime(tmUniversalTime, y, m, d, h, mm, s);
    }

    size_t FormatTime(time_t time, TMStrW pBuf, size_t nBuf, TMCStrW pFmt = L"%c") const
    {
        TM::CTm tm;
        SystemTimeToLocalTime(tm, time);
        return tm.Format(pBuf, nBuf, pFmt);
    }

    




//     void GetTM(struct tm * _tm)
//     {
//         _tm.tm_isdst .tm_hour = 
//     }
// 
//     size_t Format (TMStrA pBuf, size_t nBuf, TMCStrA pFmt = "%c")
//     {
//         DbgAssert(m_tm != -1);
//         return StrFTime (pBuf, nBuf, pFmt, localtime(&m_tm));
//     }
//     
//     size_t Format (TMStrW pBuf, size_t nBuf, TMCStrW pFmt = L"%c")
//     {
//         DbgAssert(m_tm != -1);
//         return StrFTime (pBuf, nBuf, pFmt, localtime(&m_tm));
//     }
};


TM_END_NAMESPACE



#endif // TMTIMEZONE_H
