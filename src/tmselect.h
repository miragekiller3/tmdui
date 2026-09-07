/*
 *  @file
 *  @brief  std::conditional for VC6
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2003.04.24
 *
 *  Copyright (C) 2003 miragekiller
 */

#pragma once

#ifndef TMSELECT_H
#define TMSELECT_H

#include "tmdef.h"


#ifdef __cplusplus

namespace TM
{
    namespace PRI
    {
        // partial specialization: true yields first argument
        template < int __x > struct select_traits
        {
            template < class X, class Y > struct t { typedef X Result; };
        };

        // partial specialization: flase yields second argument
        TM_TEMPLATE_NULL struct select_traits < 0 >
        {
            template < class X, class Y > struct t { typedef Y Result; };
        };
    } // end of namespace PRI


    // yield second or third argument depending on first argument
    template< int _if, class _then, class _else >
        struct CSelect
    {
        typedef PRI::select_traits<_if> traits;
        typedef typename traits::template t<_then, _else>::Result Result;
    };

} // end of namespace TM

#define TM_SELECT(x,y,z)      TM::CSelect<x,y,z>::Result


namespace TM
{
    template < class F1, class F2 >
        inline F1 StrSel (F1 f1, F2, TMCStrA) { return f1; }

    template < class F1, class F2 >
        inline F2 StrSel (F1, F2 f2, TMCStrW) { return f2; }

    template < class F1, class F2, class F3, class F4 >
        inline F1 StrSel (F1 f1, F2, F3, F4, TMCStrA, TMCStrA) { return f1; }

    template < class F1, class F2, class F3, class F4 >
        inline F2 StrSel (F1, F2 f2, F3, F4, TMCStrW, TMCStrW) { return f2; }

    template < class F1, class F2, class F3, class F4 >
        inline F3 StrSel (F1, F2, F3 f3, F4, TMCStrA, TMCStrW) { return f3; }

    template < class F1, class F2, class F3, class F4 >
        inline F4 StrSel (F1, F2, F3, F4 f4, TMCStrW, TMCStrA) { return f4; }

} // namespace TM


#endif // end of #ifdef __cplusplus


#endif //TMSELECT_H
