/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2014.2.11
 *
 *  Copyright (C) 2014 miragekiller
 */

// value   round   floor   ceil    trunc
// -----   -----   -----   ----    -----
// 2.3     2.0     2.0     3.0     2.0
// 3.8     4.0     3.0     4.0     3.0
// 5.5     6.0     5.0     6.0     5.0
// -2.3    -2.0    -3.0    -2.0    -2.0
// -3.8    -4.0    -4.0    -3.0    -3.0
// -5.5    -6.0    -6.0    -5.0    -5.0
// 18/1.2  15      14      15      14
// 15      15      15      15      15     

#pragma once

#ifndef TMMATH_H
#define TMMATH_H

#include <float.h>
#include <math.h>
#include <limits>
#include <stdlib.h>
#include "tmdef.h"

#ifdef TM_WIN
#   include <windows.h> // MulDiv
#endif


template <class T>
struct CNumBaseT
{
    inline static T Epsilon() { return std::numeric_limits<T>::epsilon(); }

    inline static bool Equal0(T x, T eps) { DbgAssert(eps >= 0); return (-eps <= x) && (x <= eps); }
    inline static bool Equal0(T x) { return Equal0(x, Epsilon()); }

    inline static bool Equal(T x, T y, T eps) { DbgAssert(eps >= 0); return ((x >= (y - eps)) && (x <= (y + eps))); }
    inline static bool Equal(T x, T y) { return Equal(x, y, Epsilon()); }

//    inline static bool Lesser(T x, T y, T eps) { return x + eps <= y; }
    inline static bool Lesser(T x, T y, T eps) { DbgAssert(eps >= 0); return Equal(x, y, eps) ? false : x < y; }
    inline static bool Lesser(T x, T y) { return Lesser(x, y, Epsilon()); }

//    inline static bool LesserEqual(T x, T y, T eps) { DbgAssert(eps >= 0); return x - eps < y;}
    inline static bool LesserEqual(T x, T y, T eps) { DbgAssert(eps >= 0); return Equal(x, y, eps) ? true : x < y; }
    inline static bool LesserEqual(T x, T y) { return LesserEqual(x, y, Epsilon());}

//    inline static bool Greater(T x, T y, T eps) { DbgAssert(eps >= 0); return x - eps >= y; }
    inline static bool Greater(T x, T y, T eps) { DbgAssert(eps >= 0); return Equal(x, y, eps) ? false : x > y; }
    inline static bool Greater(T x, T y) { return Greater(x, y, Epsilon()); }

//    inline static bool GreaterEqual(T x, T y, T eps) { DbgAssert(eps >= 0); return x + eps > y; }
    inline static bool GreaterEqual(T x, T y, T eps) { DbgAssert(eps >= 0); return Equal(x, y, eps) ? true : x > y; }
    inline static bool GreaterEqual(T x, T y) { return GreaterEqual(x, y, Epsilon()); }

    inline static T Round(T x) { return x >= 0 ? floor(x + (T)0.5f) : ceil(x - (T)0.5f); }
    inline static T Floor(T x) { return floor(x); }
    inline static T Ceiling(T x) { return ceil(x); }
};

// for int/unsigned int(8,16,32,64) types.
template <class T>
struct CNumT : public CNumBaseT<T>
{
    using CNumBaseT<T>::Equal0;
    using CNumBaseT<T>::Equal;
    using CNumBaseT<T>::Epsilon;

    inline static bool Equal0(T x) { return x == 0; }
    inline static bool Equal(T x, T y) { return x == y; }
    inline static bool Lesser(T x, T y, T eps) { return x - eps < y; }
    inline static bool Lesser(T x, T y) { return Lesser(x, y, Epsilon()); }
    inline static bool LesserEqual(T x, T y, T eps) { return x - eps <= y;}
    inline static bool LesserEqual(T x, T y) { return LesserEqual(x, y, Epsilon());}
    inline static bool Greater(T x, T y, T eps) { return x + eps > y; }
    inline static bool Greater(T x, T y) { return Greater(x, y, Epsilon()); }
    inline static bool GreaterEqual(T x, T y, T eps) { return x + eps >= y; }
    inline static bool GreaterEqual(T x, T y) { return GreaterEqual(x, y, Epsilon()); }
    inline static T Round(T x) { return x; }
    inline static T Floor(T x) { return x; }
    inline static T Ceiling(T x) { return x; }
    inline static T Round(float x) { return CNumBaseT<float>::Round(x); }
    inline static T Floor(float x) { return CNumBaseT<float>::Floor(x); }
    inline static T Ceiling(float x) { return CNumBaseT<float>::Ceiling(x); }
    inline static T Round(double x) { return CNumBaseT<double>::Round(x); }
    inline static T Floor(double x) { return CNumBaseT<double>::Floor(x); }
    inline static T Ceiling(double x) { return CNumBaseT<double>::Ceiling(x); }
};


TM_TEMPLATE_NULL struct CNumT<float> : public CNumBaseT<float>
{
//     inline static double EPS() { return FLT_EPSILON; }
    static tm_constexpr11 float PI2() { return 2.0f * 3.14159265358979323846264338327f; }
    static tm_constexpr11 float PI()  { return 3.14159265358979323846264338327f; }
    static tm_constexpr11 float E()   { return 2.7182818284590452354f; }
};

TM_TEMPLATE_NULL struct CNumT<double> : public CNumBaseT<double>
{
//     inline static double EPS() { return DBL_EPSILON; }
    static tm_constexpr11 double PI2() { return 2.0 * 3.14159265358979323846264338327; }
    static tm_constexpr11 double PI()  { return 3.14159265358979323846264338327; }
    static tm_constexpr11 double E()   { return 2.7182818284590452354; }
};

typedef CNumT<float>    CFloat;
typedef CNumT<double>   CDouble;
typedef CNumT<int>      CInt;


class CMath
{
public:
    template<class T> inline static bool Equal0(T x) { return CNumT<T>::Equal0(x); }
    template<class T> inline static bool Equal0(T x, T eps) { return CNumT<T>::Equal0(x, eps); }
    
    template<class T> inline static bool Equal(T x, T y) { return CNumT<T>::Equal(x, y); }
    template<class T> inline static bool Equal(T x, T y, T eps) { return CNumT<T>::Equal(x, y, eps); }

    template<class T> inline static bool Lesser(T x, T y) { return CNumT<T>::Lesser(x, y); }
    template<class T> inline static bool Lesser(T x, T y, T eps) { return CNumT<T>::Lesser(x, y, eps); }

    template<class T> inline static bool LesserEqual(T x, T y) { return CNumT<T>::LesserEqual(x, y); }
    template<class T> inline static bool LesserEqual(T x, T y, T eps) { return CNumT<T>::LesserEqual(x, y, eps); }

    template<class T> inline static bool Greater(T x, T y, T eps = 0) { return CNumT<T>::Greater(x, y); }
    template<class T> inline static bool GreaterEqual(T x, T y, T eps = 0) { return CNumT<T>::GreaterEqual(x, y, eps); }

    template<class T> inline static T Round(T x) { return CNumT<T>::Round(x); }
    template<class T> inline static T Floor(T x) { return CNumT<T>::Floor(x); }
    template<class T> inline static T Ceiling(T x) { return CNumT<T>::Ceiling(x); }

    template<class T> inline static bool IsNaN(T x) { return x != x; }  // is not a number
    template<class T> inline static T Abs(T x) { return abs(x); }
    template<class T> inline static T Max(T x, T y) { return tm_max(x, y); }
    template<class T> inline static T Min(T x, T y) { return tm_min(x, y); }
    template<class T> inline static int Sign(T x)
    {
        if (Lesser(x, (T)0))
            return -1;
        else if (Greater(x, (T)0))
            return 1;
        else
            return 0;
    }

    inline static float   ACos(float x) { return acosf(x); }
    inline static float   Asin(float x) { return asinf(x); }
    inline static float   Atan(float x) { return atanf(x); }
    inline static float   Atan2(float x, float y) { return atan2f(x, y); }
    inline static float   Cos(float x) { return cosf(x); }
    inline static float   Cosh(float x) { return coshf(x); }
    inline static float   Exp(float x) { return expf(x); }
    inline static float   Log(float x) { return logf(x); }                     // base e
    inline static float   Log(float x, float y) { return logf(x) / logf(y); }   // base y
    inline static float   Log10(float x) { return log10f(x); }
    inline static float   Pow(float x, float y) { return powf(x, y); }
    inline static float   Sin(float x) { return sinf(x); }
    inline static float   Sinh(float x) { return sinhf(x); }
    inline static float   Sqrt(float x) { return sqrtf(x); }
    inline static float   Tan(float x) { return tanf(x); }
    inline static float   Tanh(float x) { return tanhf(x); }

    inline static double  ACos(double x) { return acos(x); }
    inline static double  Asin(double x) { return asin(x); }
    inline static double  Atan(double x) { return atan(x); }
    inline static double  Atan2(double x, double y) { return atan2(x, y); }
    inline static double  Cos(double x) { return cos(x); }
    inline static double  Cosh(double x) { return cosh(x); }
    inline static double  Exp(double x) { return exp(x); }
    inline static double  Log(double x) { return log(x); }                        // base e
    inline static double  Log(double x, double y) { return log(x) / log(y); }     // base y
    inline static double  Log10(double x) { return log10(x); }
    inline static double  Pow(double x, int y) { return pow(x, y); }
    inline static double  Sin(double x) { return sin(x); }
    inline static double  Sinh(double x) { return sinh(x); }
    inline static double  Sqrt(double x) { return sqrt(x); }
    inline static double  Tan(double x) { return tan(x); }
    inline static double  Tanh(double x) { return tanh(x); }

    inline static int MulDiv(int n1, int n2, int div)
    {
#ifdef TM_WIN
        return ::MulDiv(n1, n2, div);
#else
        return static_cast<int>((TMLong)n1 * n2 / div); 
#endif
    }


    inline static int Rand(int nMax) // return: [0,nMax]
    {
        DbgAssert(nMax > 0 && nMax <= RAND_MAX);
        int n = (1 + (int)RAND_MAX) / (nMax + 1);
        for (;;)
        {
            int ret = rand() / n;
            if (ret <= nMax)
                return ret;
        }
    }

    inline static TMUInt32 Rand32()
    {
        return (rand()<<17)|(rand()<<3)|(rand());
    }


//     inline static template<class T> IEEERemainder(T x) { return exp(x); }



};


#endif // TMMATH_H
