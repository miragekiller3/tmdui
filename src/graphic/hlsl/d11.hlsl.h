/*
 *  @file
 *  @brief  D3D11 VS, PS, NV12 decode
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021.9.22
 *
 *  Copyright (C) 2021 miragekiller
 */

#include "gxvs_i.h"

// Y-U-V-A or Y-UV-A
Texture2D           Tex1;
Texture2D           Tex2;
Texture2D           Tex3;

SamplerState        g_SamLinear;

SamplerState        g_linearfilter;
// {
//     Filter = MIN_MAG_MIP_LINEAR;
// };

// cbuffer ConstantBuffer : register(b0)
// {
//     matrix g_World;
//     matrix g_View;
//     matrix g_Proj;
// }

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Dif : COLOR0;
    float2 Tex : TEXCOORD;
};

VS_OUTPUT VS(VS_INPUT1 input)
{
    VS_OUTPUT Output;
    Output.Pos = mul(float4(input.Pos, 1, 1.0f), proj);
    Output.Dif = input.Dif;
    Output.Tex = input.Tex;
    return Output;
}

float4 PS(VS_OUTPUT In) : SV_Target
{
    return In.Dif;
}

float4 PS_Tex(VS_OUTPUT In) : SV_Target
{
    return Tex1.Sample(g_linearfilter, In.Tex) * In.Dif;
}

static const float3x3 YUVtoRGBCoeffMatrix =
{
    1.164383f,  1.164383f, 1.164383f,
    0.000000f, -0.391762f, 2.017232f,
    1.596027f, -0.812968f, 0.000000f
};

float3 YUV2RGB(float y, float u, float v)
{
    float3 rgb = mul(float3(y - 0.062745f, u - 0.501960f, v - 0.501960f), YUVtoRGBCoeffMatrix);
    return rgb;
}

float3 CalcNV12(float2 pos)
{
    float y    = Tex1.Sample(g_linearfilter, pos).r;
    float2 uv  = Tex2.Sample(g_linearfilter, pos).ra;
    float3 rgb = YUV2RGB(y, uv.x, uv.y);
    return rgb;
}

float4 PS_NV12(VS_OUTPUT input) : SV_Target
{
    return float4(CalcNV12(input.Tex), 1);
}

float4 PS_NV12A(VS_OUTPUT input) : SV_Target // COLOR0
{
    float  a = Tex3.Sample(g_linearfilter, input.Tex).a;
    return float4(CalcNV12(input.Tex) * a, a);
}
