/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2025.9.17
 *
 *  Copyright (C) 2025 miragekiller
 */

#include "../gxvg.h"
#include "nanovg/nanovg.c"



// void NVGcontext::Flush()
// {
//     this->params.renderFlush(this->params.userPtr);
// }

void NVGcontext::BeginFrame(float devicePixelRatio)
{
	nvgBeginFrame(this, devicePixelRatio);
}

void NVGcontext::Blt(const GXRect& rcDst, TMUInt32 uFlag)
{
    this->params.renderFlush(this->params.userPtr, rcDst, uFlag);

}



