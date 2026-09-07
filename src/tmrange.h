/*
 *  @file
 *  @brief  TMRange indicates only a valid memory range and does not manage the life cycle of memory
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2005.10.10
 *
 *  Copyright (C) 2005 miragekiller
 */

#pragma once

#ifndef TMRANGE_H
#define TMRANGE_H

#include "tmdebug.h"


#ifdef TM_CPP

#ifdef TM_MSC
// identifier was truncated to '255' characters in the browser information
#   pragma warning (disable : 4786)
#endif


#include <algorithm>
#include <numeric>

// struct TMRange
// {
//     int m_head;
//     int m_tail;
// };

template <class T>
struct TMRangeT
{
    T m_data;
    size_t m_size;
};


typedef TMRangeT<int>       TMRange;
typedef TMRangeT<TMCStrW>   TMWStrRange;
// typedef TMRangeT<TMCStrA>   TMAStrRange;
// typedef TMRangeT<TMCStr>    TMTStrRange;


// |---------- old range ----------------------|
// |---------- new range width new count--|
template <class T>
tm_constexpr(inline) void RangeLeft(TMRangeT<T>& rg, size_t count)
{
    tm_setmin(rg.m_size, count);
}

// |---------- old range ----------------------|
//          |------ new range width new count--|
template <class T>
tm_constexpr(inline) void RangeRight(TMRangeT<T>& rg, size_t count)
{
    if (count < rg.m_size)
    {
        rg.m_data = rg.m_data + rg.m_size - count;
        rg.m_size = count;
    }
}

// |---------- old range ----------------------|
//      |---- new range width new count--|
// pos: 0 from rangesz.m_data;
template <class T>
tm_constexpr(inline) void RangeMid(TMRangeT<T>& rg, size_t pos, size_t count = -1)
{
    if (pos > rg.m_size)
    {
        rg.m_data += rg.m_size;
        rg.m_size = 0;
    }
    else
    {
        RangeRight(rg, rg.m_size - pos);
        RangeLeft(rg, count);
    }
}

#endif // TM_CPP


#endif // end of #ifndef TMRANGE_H
