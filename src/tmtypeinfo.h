/*
 *  @file
 *  @brief  typeinfo
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2002.09.02
 *
 *  Copyright (C) 2002 miragekiller
 */

#pragma once

#ifndef TMTYPEINFO_H
#define TMTYPEINFO_H

#ifdef __MSL__
#   if __MSL__ < 0x00008000
#      // error CTypesInfo do not support CodeWarrior version under 8.0
#   endif
#endif

#include "tmdef.h"

#ifdef TM_CPP

#include "tmselect.h"

#if TM_CPP >= TM_CPP11

#   include <type_traits>

#else

TM_BGN_NAMESPACE_X(std)


///////////////////////////////////////////////////////////////////////////////
// integral_constant
template<class T, T _v> struct integral_constant
{
    enum { value = _v };
    typedef T                           value_type;
    typedef integral_constant<T, _v>    type;
};

typedef integral_constant<bool, true>     true_type;
typedef integral_constant<bool, false>    false_type;


///////////////////////////////////////////////////////////////////////////////
// conditional
TM_BGN_PRIVATE_NAMESPACE
template<int _if> struct _cond_traits
{
    template<class X, class Y> struct t { typedef X type; };
};
TM_TEMPLATE_NULL struct _cond_traits<0>
{
    template<class X, class Y> struct t { typedef Y type; };
};
TM_END_PRIVATE_NAMESPACE

template<int _if, class _then, class _else>
struct conditional
{
    typedef PRI::_cond_traits<_if> traits;
    typedef typename traits::template t<_then, _else>::type type;
};


///////////////////////////////////////////////////////////////////////////////
// enable_if
#ifdef TM_GNUC
    template<int, class T = void> struct enable_if { typedef T type; };
    template<class T> struct enable_if<0, T> {};
#else
    TM_BGN_PRIVATE_NAMESPACE
    template<int _if> struct _enable_if_traits
    {
        template<class T> struct t { typedef T type; };
    };
    TM_TEMPLATE_NULL struct _enable_if_traits<0>
    {
        template<class T> struct t {};
    };
    TM_END_PRIVATE_NAMESPACE
    template<int _if, class T> struct enable_if : public PRI::_enable_if_traits<_if>::t<T> {};
#endif // TM_GNUC else


///////////////////////////////////////////////////////////////////////////////
// is_same
#ifdef TM_GNUC
    template<class T1, class T2> struct is_same : public false_type {};
    template<class T> struct is_same<T, T> : public true_type {};
#else
    TM_BGN_PRIVATE_NAMESPACE
    template<class T1> struct _is_same_traits
    {
        template<class T2> struct t { typedef false_type type; };
        template<> struct t<T1>     { typedef true_type type; };
    };
    TM_END_PRIVATE_NAMESPACE
    template<class T1, class T2> struct is_same : public PRI::_is_same_traits<T1>::t<T2>::type {};
#endif // TM_GNUC else



TM_END_NAMESPACE_X(std)

#endif // #if TM_CPP >= TM_CPP11 else



#ifdef TM_MSC

#   pragma warning (push)

// return type for 'TM::PointerOf<int>::operator ->' is 'int *' (ie; not a UDT
// or reference to a UDT.  Will produce errors if applied using infix notation)
#   pragma warning (disable : 4284)

#endif // #ifdef TM_MSC

namespace TM
{
//    template < class T > struct MakeTHelper { static T Do (); };
//#   define MakeT(T) MakeTHelper<T>::Do()
    
    template < class T > struct Type2Type { typedef T value_type; };
    template < int n >   struct Int2Type { enum { value = n }; };

    //template < class T, class U > struct ConvertHelper
    //{
    //    struct _YE { int __x; };
    //    struct _NO { int __x[4]; };

    //    static _YE Check (U);
    //    static _NO Check (...);

    //    static T MakeT ();
    //};

    template < class T >  struct ConvertHelper
    {
        typedef int                     _YE;
        typedef struct { int __x[2]; }  _NO;

        static _YE Check(T);
        static _NO Check(...);
    };

    template < class V > struct MakeTHelper { static V Do(); };

    // T,U can not be private inheritance 
    template < class T, class U > class CanConvert
    {
    protected:
        typedef ConvertHelper<U>           type_traits;

#       define MakeT(x) MakeTHelper<x>::Do()

        enum { check1 = sizeof(type_traits::Check(MakeT(T))) };
        enum { check2 = sizeof(type_traits::Check(MakeT(U))) };

    public:
        enum { e_bYe = (check1 == check2) };
        enum { e_bNo = !e_bYe };
    };

    /*
    template < class T, class U >
        struct CanConvert
    {
        typedef ConvertHelper<T,U>                        helper;
        typedef typename helper::_YE                    _YE;
        enum { e_bYe = sizeof(helper::Check(helper::MakeT())) == sizeof(_YE) };
        enum { e_bNo = !e_bYe };
    };

    template < class T, class U > struct IsSame
    {
        typedef Type2Type<T>                            T1;
        typedef Type2Type<U>                            T2;
        
        enum { e_bYe = CanConvert<T1,T2>::e_bYe };
        enum { e_bNo = !e_bYe };
    };

    
    template < class T > struct IsClass
    {
        template < class U > static _YE Check (U, int U::*);
        template < class U > static _NO Check (U, ...);

        enum { e_bYe = sizeof(Check(MakeT(T),0)) == sizeof(_YE) };
        enum { e_bNo = !e_bYe };
    };
    */

    template < class T > struct IsPointer
    {
        enum { e_bYe = (CanConvert<T,const void*>::e_bYe) };
        enum { e_bNo = !e_bYe };
    };
/*
    template <class T> struct IsBaseType
    {
        enum { e_bYe =
            IsSame<T, char>::e_bYe ||
            IsSame<T, wchar_t>::e_bYe ||
            IsSame<T, short>::e_bYe ||
            IsSame<T, int>::e_bYe ||
            IsSame<T, long>::e_bYe ||
//            IsSame<T, __int64>::e_bYe ||
            IsSame<T, unsigned char>::e_bYe ||
            IsSame<T, unsigned wchar_t>::e_bYe ||
            IsSame<T, unsigned short>::e_bYe ||
            IsSame<T, unsigned int>::e_bYe ||
            IsSame<T, unsigned long>::e_bYe ||
            IsSame<T, unsigned __int64>::e_bYe,
        };

        enum { e_bNo = !e_bYe };
    };

    template <class T> struct IsBasePointer
    {
        enum { e_bYe =
            CanConvert<T, const char*>::e_bYe ||
            CanConvert<T, const wchar_t*>::e_bYe ||
            CanConvert<T, const short*>::e_bYe ||
            CanConvert<T, const int*>::e_bYe ||
            CanConvert<T, const long*>::e_bYe ||
//            CanConvert<T, const __int64*>::e_bYe ||
            CanConvert<T, const unsigned char*>::e_bYe ||
            CanConvert<T, const unsigned wchar_t*>::e_bYe ||
            CanConvert<T, const unsigned short*>::e_bYe ||
            CanConvert<T, const unsigned int*>::e_bYe ||
            CanConvert<T, const unsigned long*>::e_bYe ||
            CanConvert<T, const unsigned __int64*>::e_bYe,
        };
        
        enum { e_bNo = !e_bYe };
    };
*/

} // end of namespace TM

// #define TM_SAMETYPE(T1,T2)      (TM::IsSame<T1,T2>::e_bYe)
#define TM_CAN1TO2(T1,T2)       (TM::CanConvert<T1,T2>::e_bYe)
#define TM_ISPOINTER(T)         (TM::IsPointer<T>::e_bYe)


///////////////////////////////////////////////////////////////////////////////
// check_point
// check special class type is a point

namespace TM
{

#ifndef TM_BORLAND
    template < class T > struct PointerOf
    {
    protected:
        template < class U >
            struct GetReferencePointer
        {
            typedef U*      pointer;
            typedef U&      param_type;
            inline static pointer GetPt (param_type rt) { return &rt; }
        };
        
        template < class U >
            struct GetPointerPointer
        {
            typedef U       pointer;
            typedef U       param_type;
            inline static pointer GetPt (param_type rt) { return rt; }
        };
        
    public:
        enum { e_bPoint = TM_ISPOINTER(T) };
        
    protected:
        typedef typename CSelect < e_bPoint, GetPointerPointer < T >, 
            GetReferencePointer < T > > :: Result pt_traits;
        
    public:
        typedef T                                       value_type;
        typedef typename pt_traits::pointer             pointer;
        typedef typename pt_traits::param_type          param_type;
        
    protected:
        pointer     m_pt;
        
    public:
        explicit PointerOf  (param_type __x)
            : m_pt          (pt_traits::GetPt (__x))
        {
        }

        operator pointer () const { return m_pt; }
        pointer operator-> () const { return m_pt; }
    };

    template < class T >
        PointerOf<T> GetPointer (const T& __x)
    {
        typedef typename PointerOf<T>::param_type        param_type;
        return PointerOf<T> ((param_type) __x);
    }

#define TM_P(x) TM::GetPointer (x)
#define _PointTypeT(T)  TM::PointerOf<T>::pointer

#endif 

}
// check_point
///////////////////////////////////////////////////////////////////////////////

#ifdef TM_MSC
#   pragma warning (pop)
#endif // #ifdef TM_MSC



TM_BGN_NAMESPACE

template <class T> struct t_value_type { typedef typename T::value_type value_type; };

template <class T> struct t_sizeof { enum { eSize = sizeof(T) }; };
TM_TEMPLATE_NULL struct t_sizeof<void> { enum { eSize = 0 }; };


TM_END_NAMESPACE



#endif // TM_CPP


#endif // end of TMTYPEINFO_H
