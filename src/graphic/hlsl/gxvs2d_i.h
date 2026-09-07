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

VS_INPUT_BGN                (VS_INPUT)
    VS_INPUT_ITEM           (float2, Set0, TEXCOORD, 0)  // mode, width
    VS_INPUT_ITEM           (float2, Tex0, TEXCOORD, 1)  // curr
    VS_INPUT_ITEM           (float2, Tex1, TEXCOORD, 2)  // next
    VS_INPUT_ITEM           (float2, Tex2, TEXCOORD, 3)  // prev
    // VS_INPUT_ITEM(short2, Set1, TEXCOORD4)  // bezier segments/surrent 
VS_INPUT_END 

//            TEXCOORD0   TEXCOORD1   TEXCOORD2
// cap          curr        next
// join         curr        next        prev
#ifndef MODE_TRI_NORMAL

#   define MODE_TRI_NORMAL  0

#   define MODE_CAP_RND_0   1       //
#   define MODE_CAP_RND_1   2       //  1   3
#   define MODE_CAP_RND_2   3       //      
#   define MODE_CAP_RND_3   4       //  0   2

#   define MODE_CAP_SQR_0   5       //
#   define MODE_CAP_SQR_1   6       //

#   define MODE_CAP_BUT_0   7       //
#   define MODE_CAP_BUT_1   8       //

// miter join                       //   \ \              / / 
#   define MODE_JOIN_MIT_0  9       //  0 > > 1        0 < <  1
#   define MODE_JOIN_MIT_1  10      //   / /              \ \ 

// Bevel join                       //   \ \  0        1  / / 
#   define MODE_JOIN_BVL_0  11      //  1 > |            | <  0                                    
#   define MODE_JOIN_BVL_1  12      //   / /  0        1  \ \ 

// round join cap
//   = Bevel join + round cap       //  \  \  0        0  /  / 
#   define MODE_JOIN_RND_0  13      //   > |> 1        1 <| <  
#   define MODE_JOIN_RND_1  14      //  /  /  0        0  \  \ 

#endif // MODE_TRI_NORMAL




///////////////////////////////////////////////////////////////////////////////
// PS Constant
PS_CONSTANT_BGN             (PS_CONST)
    PS_CONSTANT_ITEM        (float4,    set0, c0);
    PS_CONSTANT_ITEM        (float4,    pos0, c1);
    PS_CONSTANT_ITEM        (float4,    pos1, c2);
    PS_CONSTANT_ITEM        (float4,    clr0, c3);
    PS_CONSTANT_ITEM        (float4,    clr1, c4);
PS_CONSTANT_END

#ifndef TYPE_COLOR

#   define TYPE_COLOR           0   // color
#   define TYPE_LGRADIENT       1   // linear gradient; 2 stops
#   define TYPE_RGRADIENT       2   // radial gradient; 2 stops
#   define TYPE_PATTERN         3
#   define TYPE_STENCIL         4   // stencil test
#   define TYPE_LGRADIENT_N     5   // linear gradient; >2 stops
#   define TYPE_RGRADIENT_N     6   // radial gradient; >2 stops

#   define TYPE_SHADOW1          7
#   define TYPE_SHADOW2          8

#   define PT_REPEAT_NON        0
#   define PT_REPEAT_X          1
#   define PT_REPEAT_Y          2
#   define PT_REPEAT_XY         3

#   define style_type           set0.x
#   define style_alpha          set0.y
#   define style_color          clr0
#   define gradient_stop0       set0.z
#   define gradient_stop1       set0.w
#   define gradient_stopcount   set0.z
#   define gradient_clr0        style_color
#   define gradient_clr1        clr1
#   define gradient_pos0        pos0
#   define gradient_pos1        pos1
#   define rgradient_r0         pos0.z
#   define rgradient_r1         pos1.z
#   define pattern_aff_m11      pos0.x
#   define pattern_aff_m12      pos0.y
#   define pattern_aff_m21      pos0.z
#   define pattern_aff_m22      pos0.w
#   define pattern_aff_dx       pos1.x
#   define pattern_aff_dy       pos1.y
#   define pattern_repeat       set0.w


#endif // TYPE_C0
