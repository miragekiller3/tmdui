#define TYPEMODE_HLSL_VS
#define TYPEMODE_HLSL_PS
#include "gxvs2d_i.h"

matrix proj : register(c0);

struct VS_OUTPUT
{
    float4 Pos      : POSITION;
    float3 PosCtrl  : TEXCOORD0;
    float2 PosPixel : TEXCOORD1;
};

struct PS_INPUT 
{
    float3 PosCtrl  : TEXCOORD0;    // ctrl pointer x, y, width in stroke mode 
    float2 PosPixel : TEXCOORD1;    // to x, to y, Width
};


///////////////////////////////////////////////////////////////////////////////
// VS
///////////////////////////////////////////////////////////////////////////////
float2 CalculateCapButt(float width, float2 curr, float2 next)
{
    float2 nml = normalize(next - curr);
    float2 nml2 = nml * width;
    float2 nml3 = float2(nml2.y, -nml2.x);
    return curr - nml3;
}

float2 CalculateCapSquare0(float width, float2 curr, float2 next)
{
    float2 nml = normalize(next - curr);
    float2 nml2 = nml * width;
    float2 nml3 = float2(nml2.x + nml2.y, nml2.y - nml2.x);
    return curr - nml3;
}

float2 CalculateCapSquare1(float width, float2 curr, float2 next)
{
    float2 nml = normalize(next - curr);
    float2 nml2 = nml * width;
    float2 nml3 = float2(nml2.x - nml2.y, nml2.y + nml2.x);
    return curr - nml3;
}

float2 CalculateJoinMiterD(float width, float2 dir1,  float2 dir2)
{
    dir1 = normalize(dir1);
    dir2 = normalize(dir2);
    
    float2 perp1 = float2(-dir1.y, dir1.x);
    float2 perp2 = float2(-dir2.y, dir2.x);
    
    float2 tangent = dir1 + dir2;
    tangent = normalize(tangent);
    float2 miter = float2(-tangent.y, tangent.x);
    
    float length = width  / dot(miter, perp1);
    return miter * length;
}

float2 CalculateJoinMiterLeft(float width, float2 prev, float2 curr, float2 next)
{
    float2 dir1 = curr - prev;
    float2 dir2 = next - curr;
    return curr + CalculateJoinMiterD(width, dir1, dir2);
}

float2 CalculateJoinMiterRight(float width, float2 prev, float2 curr, float2 next)
{
    float2 dir1 = curr - prev;
    float2 dir2 = next - curr;
    return curr - CalculateJoinMiterD(width, dir1, dir2);
}

float Cross2D(float2 a, float2 b)
{
    return a.x * b.y - a.y * b.x;
}

float2 CalculateJoinBevel1(float width, float2 prev, float2 curr, float2 next)
{
    float2 dir1 = curr - prev;
    float2 dir2 = next - curr;
    float cross = Cross2D(dir1, dir2);
    if (cross < 0)
        return CalculateCapButt(-width, curr, prev);
    else
        return curr + CalculateJoinMiterD(width, dir1, dir2);
}

float2 CalculateJoinBevel0(float width, float2 prev, float2 curr, float2 next)
{
    float2 dir1 = curr - prev;
    float2 dir2 = next - curr;
    float cross = Cross2D(dir1, dir2);
    if (cross < 0)
        return curr - CalculateJoinMiterD(width, dir1, dir2);
    else
        return CalculateCapButt(width, curr, prev);
}

float2 CalculateJoinRound0(float width, float2 prev, float2 curr, float2 next)
{
    float2 dir1 = curr - prev;
    float2 dir2 = next - curr;
    float cross = Cross2D(dir1, dir2);
    if (cross < 0)
        return CalculateCapButt(-width, curr, prev);
    else
        return CalculateCapButt(width, curr, prev);
}

float2 CalculateJoinRound1(float width, float2 prev, float2 curr, float2 next)
{
    float2 dir1 = curr - prev;
    float2 dir2 = next - curr;
    float cross = Cross2D(dir1, dir2);
    if (cross < 0)
        return curr + CalculateJoinMiterD(width, dir1, dir2);
    else
        return curr - CalculateJoinMiterD(width, dir1, dir2);
}


VS_OUTPUT VS(VS_INPUT input)
{
#define CURR input.Tex0
#define NEXT input.Tex1
#define PREV input.Tex2

    int mode        = input.Set0.x;// / 800.0f;
    float halfWidth = input.Set0.y;// / 800.0f;
	float2 tmpPos;


    if (mode == MODE_CAP_SQR_0 || mode == MODE_CAP_RND_0)
        tmpPos = CalculateCapSquare0(halfWidth, CURR, NEXT);
    else if (mode == MODE_CAP_SQR_1 || mode == MODE_CAP_RND_1)
        tmpPos = CalculateCapSquare1(halfWidth, CURR, NEXT);
    else if (mode == MODE_CAP_BUT_0 || mode == MODE_CAP_RND_2)
        tmpPos = CalculateCapButt(halfWidth, CURR, NEXT);
    else if (mode == MODE_CAP_BUT_1 || mode == MODE_CAP_RND_3)
        tmpPos = CalculateCapButt(-halfWidth, CURR, NEXT);
    else if (mode == MODE_JOIN_MIT_0)
        tmpPos = CalculateJoinMiterRight(halfWidth, PREV, CURR, NEXT);
    else if (mode == MODE_JOIN_MIT_1)
        tmpPos = CalculateJoinMiterLeft(halfWidth, PREV, CURR, NEXT);
    else if (mode == MODE_JOIN_BVL_1)
        tmpPos = CalculateJoinBevel1(halfWidth, PREV, CURR, NEXT);
    else if (mode == MODE_JOIN_BVL_0)
        tmpPos = CalculateJoinBevel0(halfWidth, PREV, CURR, NEXT);
    else if (mode == MODE_JOIN_RND_0)
        tmpPos = CalculateJoinRound0(halfWidth, PREV, CURR, NEXT);
    else if (mode == MODE_JOIN_RND_1)
        tmpPos = CalculateJoinRound1(halfWidth, PREV, CURR, NEXT);
    else // MODE_TRI_NORMAL
        tmpPos = CURR;

    VS_OUTPUT output;
	output.Pos = mul(float4(tmpPos, 1, 1), proj);
    output.PosPixel = tmpPos;
    if (mode >= MODE_CAP_RND_0 && mode <= MODE_CAP_RND_3 || mode == MODE_JOIN_RND_0 || mode == MODE_JOIN_RND_1)
        output.PosCtrl = float3(CURR, halfWidth);
    else
        output.PosCtrl.z = 0;

//         output.Pos.x = CURR.x < 50 ? -1 : 1;
//         output.Pos.y = CURR.y < 50 ? -1 : 1;

    return output;
}


///////////////////////////////////////////////////////////////////////////////
// PS
///////////////////////////////////////////////////////////////////////////////
sampler Tex0 : register(s0); 

float CalcProjectionL(float2 p0, float2 p1, float2 p)
{
    float2 l_vector = p1 - p0;
    float2 p_vector = p  - p0;
    float d1 = dot(p_vector, l_vector);
    float d2 = dot(l_vector, l_vector);
    float t = d1 / d2;
    return t;
}

float4 TexStop(float x, float y)
{
    return tex2D(Tex0, float2((x + 0.5f) / gradient_stopcount, (y + 0.5f) / 2.0f));
}

int BinarySearchStop(float targetValue)
{
    int pos = 0; int tmp;
    tmp = pos + 64; if (gradient_stopcount >= tmp && targetValue > TexStop(tmp, 0).x) pos = tmp;
    tmp = pos + 32; if (gradient_stopcount >= tmp && targetValue > TexStop(tmp, 0).x) pos = tmp;
    tmp = pos + 16; if (gradient_stopcount >= tmp && targetValue > TexStop(tmp, 0).x) pos = tmp;
    tmp = pos +  8; if (gradient_stopcount >= tmp && targetValue > TexStop(tmp, 0).x) pos = tmp;
    tmp = pos +  4; if (gradient_stopcount >= tmp && targetValue > TexStop(tmp, 0).x) pos = tmp;
    tmp = pos +  2; if (                             targetValue > TexStop(tmp, 0).x) pos = tmp; // nCount always >= 3
    tmp = pos +  1; if (                             targetValue > TexStop(tmp, 0).x) pos = tmp;
    return pos;
}

float4 CustomLerp(float4 a, float4 b, float n, float n1, float n2)
{
    float t = saturate((n - n1) / (n2 - n1));
    return lerp(a, b, t);
}

float CalcProjectionR(
                            float2 pixelPos,      // 当前像素位置
                            float2 startCenter,   // 起始圆中心
                            float startRadius,    // 起始圆半径
                            float2 endCenter,     // 结束圆中心  
                            float endRadius      // 结束圆半径
                            )
{
    // 计算到两个圆心的距离
    float distToStart = distance(pixelPos, startCenter);
    float distToEnd = distance(pixelPos, endCenter);
    
    // 计算渐变因子
    // 当在起始圆内时为0，在结束圆外时为1
    float t = 0.0;
    
    if (distToStart <= startRadius) {
        t = 0.0;  // 在起始圆内部
    }
    else if (distToEnd >= endRadius) {
        t = 1.0;  // 在结束圆外部
    }
    else {
        // 在两个圆环之间，计算插值因子
        // 这是简化的计算方法，实际 Canvas 使用更复杂的锥形插值
        float numerator = distToStart - startRadius;
        float denominator = (endRadius - startRadius) + distance(endCenter, startCenter);
        t = saturate(numerator / denominator);
    }
    return t;
}

float CalcProjectionR2(
                       float2 pixelPos,      // 当前像素位置
                       float2 startCenter,   // 起始圆中心
                       float startRadius,    // 起始圆半径
                       float2 endCenter,     // 结束圆中心  
                       float endRadius      // 结束圆半径
                            )
{
    // 计算当前像素到两个圆心的向量
    float2 toInnerCenter = pixelPos - startCenter;
    float2 toOuterCenter = pixelPos - endCenter;
    
    // 计算到两个圆心的距离
    float distToInner = length(toInnerCenter);
    float distToOuter = length(toOuterCenter);
    
    // 计算内圆和外圆的边界距离
    float innerBoundary = startRadius;
    float outerBoundary = endRadius;
    
    // 核心渐变计算：基于Canvas径向渐变算法
    // 当在内圆内部时，使用起始颜色
    // 当在外圆外部时，使用结束颜色
    // 在中间区域进行插值
    
    if (distToInner <= innerBoundary) {
        // 在内圆内部，返回起始颜色
        return 0;//colors[0];
    } else if (distToOuter >= outerBoundary) {
        // 在外圆外部，返回结束颜色
        return 1;//colors[colorCount - 1];
    } else {
        // 计算归一化的渐变参数t
        // 基于从内圆边界到外圆边界的距离比例
        float t = 0.0;
        
        // 计算从内圆到当前点的距离比例
        if (innerBoundary > 0) {
            float distFromInner = distToInner - innerBoundary;
            float totalRange = outerBoundary - innerBoundary;
            t = saturate(distFromInner / totalRange);
        }

        return t;
        
//         // 多颜色停止点插值计算
//         for(int i = 0; i < colorCount - 1; i++) {
//             if(t >= stops[i] && t <= stops[i + 1]) {
//                 float localT = (t - stops[i]) / (stops[i + 1] - stops[i]);
//                 return lerp(colors[i], colors[i + 1], localT);
//             }
//         }
    }
    
//     return colors[0];
}

float4 PS(PS_INPUT input) : COLOR0
{
    int type = style_type;
    float4 clr;
    if (type == TYPE_COLOR)
        clr = style_color;
    else if (type == TYPE_STENCIL)
        clr = float4(1,1,1,1);
    else if (type == TYPE_LGRADIENT)
    {
        float n = CalcProjectionL(gradient_pos0.xy, gradient_pos1.xy, input.PosPixel);
        clr = CustomLerp(gradient_clr0, gradient_clr1, n, gradient_stop0, gradient_stop1);
    }
    else if (type == TYPE_LGRADIENT_N)
    {
         float n = CalcProjectionL(gradient_pos0.xy, gradient_pos1.xy, input.PosPixel);
         int ind = BinarySearchStop(n);
         float4 cf = TexStop(ind, 1);
         if (ind < gradient_stopcount)
         {
             float4 cto = TexStop(ind + 1, 1);
             clr = CustomLerp(cf, cto, n, TexStop(ind, 0).x, TexStop(ind + 1, 0).x);
         }
         else
             clr = TexStop(gradient_stopcount - 1, 1);
    }
    else if (type == TYPE_RGRADIENT)
    {
        float n = CalcProjectionR2(input.PosPixel, gradient_pos0.xy, rgradient_r0, gradient_pos1.xy, rgradient_r1);
        clr = CustomLerp(gradient_clr0, gradient_clr1, n, gradient_stop0, gradient_stop1);
    }
    else if (type == TYPE_RGRADIENT_N)
    {
        float n = CalcProjectionR2(input.PosPixel, gradient_pos0.xy, rgradient_r0, gradient_pos1.xy, rgradient_r1);
        int ind = BinarySearchStop(n);
        float4 cf = TexStop(ind, 1);
        if (ind < gradient_stopcount)
        {
            float4 cto = TexStop(ind + 1, 1);
            clr = CustomLerp(cf, cto, n, TexStop(ind, 0).x, TexStop(ind + 1, 0).x);
        }
        else
            clr = TexStop(gradient_stopcount - 1, 1);
    }
    else if (type == TYPE_PATTERN)
    {
        float3x3 aff = {
            pattern_aff_m11, pattern_aff_m12, 0,
            pattern_aff_m21, pattern_aff_m22, 0,
            pattern_aff_dx,  pattern_aff_dy,  1};
        float2 uv = mul(float3(input.PosPixel, 1), aff).xy;
        bool bDiscard = 
            ((pattern_repeat == PT_REPEAT_NON) && ((uv.y < 0 || uv.y > 1 || uv.x < 0 || uv.x > 1))) ||
            ((pattern_repeat == PT_REPEAT_X)   && (uv.y < 0 || uv.y > 1)) ||
            ((pattern_repeat == PT_REPEAT_Y)   && (uv.x < 0 || uv.x > 1));
        if(bDiscard)
            discard;
        uv = frac(uv);
        clr = tex2D(Tex0, uv);
    }


    // 计算到中心线的距离（归一化）
//    float dist = length(input.Tex /*- float2(0.5, 0.5)*/) * 2.0;
//      float width = 50.0f;
//      float dist = min(distance(PREV, input.Tex), width) / width;
	float alpha = 1.0;
	if (input.PosCtrl.z != 0)
	{
		float dist = distance(input.PosPixel, input.PosCtrl.xy) / input.PosCtrl.z;
//      float dist =(input.Pos.xy / input.Pos.w).x;
  
    // 圆角处理（使用smoothstep平滑过渡）
		alpha -= smoothstep(0.99, 1.0, dist);
//        alpha = dist;
	}
    
    // 线宽控制
//     if (dist > 1.0) discard;
    
    // 颜色输出（红色带透明度过渡）
    //return float4(1.0, 0, 0.0, alpha);
    return clr * alpha * style_alpha; 
//    return float4(1.0, alpha, 0.0, 1.0);
//    return float4(1.0, 0, PREV.y, 1.0);
}

technique WideLine {
    pass P0 {
        VertexShader = compile vs_3_0 VS();
        PixelShader = compile ps_3_0 PS();
        AlphaBlendEnable = TRUE;
        SrcBlend = SRCALPHA;
        DestBlend = INVSRCALPHA;
    }
}

