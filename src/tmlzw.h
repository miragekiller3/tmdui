/*
 *  @file
 *  @brief  LZW encode, decode
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2012.10.11
 *
 *  Copyright (C) 2012 miragekiller
 */

#pragma once

#ifndef TMLZW_H
#define TMLZW_H

#include "tmio.h"

struct ISequentialStream;
TM_CAPI TMBool LzwDecode(ISequentialStream* pIn, ISequentialStream* pOut);

#endif // TMLZW_H
