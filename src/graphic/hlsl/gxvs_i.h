/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2025.9.6
 *
 *  Copyright (C) 2025 miragekiller
 */


#include "../../gxsl.h"


///////////////////////////////////////////////////////////////////////////////
// VS
VS_INPUT_BGN                (VS_INPUT1)
    VS_INPUT_ITEM           (float2, Pos,   POSITION, 0)
    VS_INPUT_ITEM           (float4, Dif,   COLOR,    0)
    VS_INPUT_ITEM           (float2, Tex,   TEXCOORD, 0)
VS_INPUT_END 

///////////////////////////////////////////////////////////////////////////////
// VS Constant
VS_CONSTANT_BGN             (CBuffer)
    VS_CONSTANT_ITEM        (matrix,        proj,     c0);
VS_CONSTANT_END
