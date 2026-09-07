/*
 *  @file
 *  @brief  debug function for message
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2015.03.26
 *
 *  Copyright (C) 2015 miragekiller
 */

#pragma once

#ifndef TMFORMATMSG_H
#define TMFORMATMSG_H

#include "tmdef.h"

#if defined(TM_WIN)
#   include "tmformatmsg_win.h"
#else
    inline static TM::CStdStr DbgFormatMessage(...) { return TM::CStdStr(); }
#endif // TM_WIN

#endif // TMFORMATMSG_H
