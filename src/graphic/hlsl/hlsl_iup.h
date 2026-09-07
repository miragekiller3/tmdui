#ifndef HLSL_H
#define HLSL_H

#define TYPEMODE_C_VERTEX
#include "gxvs2d_i.h"

inline void InitCap(VS_INPUT& x, float id, float w, float2 cur)
{
    x.Set0.x = id;
    x.Set0.y = w;
    x.Tex0   = cur;
}
inline void InitCap(VS_INPUT& x, float id, float w, float2 cur, float2 next)
{
    DbgAssert(cur != next);
    InitCap(x, id, w, cur);
    x.Tex1   = next;
}
inline void InitCap(VS_INPUT& x, float id, float w, float2 prev, float2 cur, float2 next)
{
    DbgAssert(cur != prev);
    InitCap(x, id, w, cur, next);
    x.Tex2 = prev;
}

inline VS_INPUT* InitCapRound_L(VS_INPUT* x, float2 curr, float2 next, float w)
{
    InitCap(x[0], MODE_CAP_RND_0, w, curr, next);
    InitCap(x[1], MODE_CAP_RND_1, w, curr, next);
    InitCap(x[2], MODE_CAP_RND_2, w, curr, next);
    InitCap(x[3], MODE_CAP_RND_3, w, curr, next);
    return x + 4;
}
inline VS_INPUT* InitCapRound_R(VS_INPUT* x, float2 curr, float2 next, float w)
{
    InitCap(x[0], MODE_CAP_RND_3, w, next, curr);
    InitCap(x[1], MODE_CAP_RND_2, w, next, curr);
    InitCap(x[2], MODE_CAP_RND_1, w, next, curr);
    InitCap(x[3], MODE_CAP_RND_0, w, next, curr);
    return x + 4;
}

inline void InitCapSquare_L(VS_INPUT* x, float2 curr, float2 next, float w)
{
    InitCap(x[0], MODE_CAP_SQR_0, w, curr, next);
    InitCap(x[1], MODE_CAP_SQR_1, w, curr, next);
}
inline void InitCapSquare_R(VS_INPUT* x, float2 curr, float2 next, float w)
{
    InitCap(x[2], MODE_CAP_SQR_1, w, next, curr);
    InitCap(x[3], MODE_CAP_SQR_0, w, next, curr);
}
inline void InitCapButt_L(VS_INPUT* x, float2 curr, float2 next, float w)
{
    InitCap(x[0], MODE_CAP_BUT_0, w, curr, next);
    InitCap(x[1], MODE_CAP_BUT_1, w, curr, next);
}
inline void InitCapButt_R(VS_INPUT* x, float2 curr, float2 next, float w)
{
    InitCap(x[2], MODE_CAP_BUT_1, w, next, curr);
    InitCap(x[3], MODE_CAP_BUT_0, w, next, curr);
}

inline void InitJoinMiter_L(VS_INPUT* x, float2 prev, float2 cur, float2 next, float w)
{
    InitCap(x[0], MODE_JOIN_MIT_1, w, prev, cur, next);
    InitCap(x[1], MODE_JOIN_MIT_0, w, prev, cur, next);
}
inline void InitJoinMiter_R(VS_INPUT* x, float2 prev, float2 cur, float2 next, float w)
{
    InitCap(x[2], MODE_JOIN_MIT_1, w, prev, cur, next);
    InitCap(x[3], MODE_JOIN_MIT_0, w, prev, cur, next);
}
inline void InitJoinBevel_L(VS_INPUT* x, float2 prev, float2 cur, float2 next, float w)
{
    InitCap(x[0], MODE_JOIN_BVL_0, w, next, cur, prev);
    InitCap(x[1], MODE_JOIN_BVL_1, w, next, cur, prev);
}
inline void InitJoinBevel_R(VS_INPUT* x, float2 prev, float2 cur, float2 next, float w)
{
    InitCap(x[2], MODE_JOIN_BVL_1, w, prev, cur, next);
    InitCap(x[3], MODE_JOIN_BVL_0, w, prev, cur, next);
}
inline void InitJoinRound_Cap(VS_INPUT* x, float2 prev, float2 cur, float2 next, float w)
{
    InitCap(x[0], MODE_JOIN_RND_0, w, prev, cur, next);
    InitCap(x[1], MODE_JOIN_RND_0, w, next, cur, prev);
    InitCap(x[2], MODE_JOIN_RND_1, w, prev, cur, next);
}


#endif // HLSL_H
