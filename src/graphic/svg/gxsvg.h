/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021.9.6
 *
 *  Copyright (C) 2021 miragekiller
 */

#pragma once

#ifndef GXSVG_H
#define GXSVG_H

#include "../gxanisrc.h"

TM_BGN_NAMESPACE

const float SVG_FRAME_RATE = 30;
const float SVG_FRAME_INTERVALS = 1000.0f / SVG_FRAME_RATE;

IAniSource* svgLoad(EImageFormat fmt, const char* data, int nSize, TM::IColorTransform* pct, TMLParam context, TM::IImageSizeDecide* isd);

TM_END_NAMESPACE


#endif // GXSVG_H