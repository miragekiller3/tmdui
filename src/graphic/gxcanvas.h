/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2025.10.20
 *
 *  Copyright (C) 2025 miragekiller
 */

#pragma once

#ifndef GXCANVAS_H
#define GXCANVAS_H

#include <d3d9types.h>
#include "gxapp.h"
#include "hlsl\hlsl_iup.h"


TM_BGN_NAMESPACE

struct CBUFFER : public PS_CONST
{
    int nCount;
    IGXTexture* image;
};

class CTmpPath : public CVector<float2>
{
public:
        // out: [0, PI]
    static float CalcTheta(const float2& p0, const float2& p1, const float2& p2)
    {
        float2 v1 = p1 - p0;
        float2 v2 = p1 - p2;
        v1 = normalize(v1);
        v2 = normalize(v2);
        float dt = dot(v1, v2); // -1:180度; 0:90度; 1:直线
        return acos(dt);
    }

    static float Powlize(float n, float nSrcMax, float nDstMax)
    {
        float tmp = 1 - n / nSrcMax; // [0, nSrcMax] -> [1,0]
        tmp = 1 - tmp * tmp * tmp;
        return nDstMax * tmp;
    }

    // 计算曲率因子（0-1范围）
    static float CalculateCurvature(const float2& p0, const float2& p1, const float2& p2)
    {
        float2 v1 = p1 - p0;
        float2 v2 = p1 - p2;
        v1 = normalize(v1);
        v2 = normalize(v2);
        float dt = dot(v1, v2); // -1:180度; 0:90度; 1:直线
        // float x = (1.0f - dot); // 0=直线, 2=180度转折
//         float ret = (1 + dt) / 2;
//         return 1 - ret * ret * ret; 

        float angle = 1 - acos(dt) / 3.1415926536; // 范围 [0, PI] -> [1,0]
        return 1 - angle * angle * angle;
//         
//         // 使用指数函数：在0附近快速上升，在1附近缓慢接近
//         // 这里使用 1 - (1-x)^k 的形式，k>1时在开始阶段增长快
//         return 1.0 - pow(1.0 - normalizedAngle, 3.0);
    }
   
    // 自动计算分段数（基于曲率和线宽）
    static int CalculateSegments(const float2& p0, const float2& p1, 
        const float2& p2, float lineWidth)
    {
        //     if (controlPoints.size() < 3) return 2; // 两点只需1段
        
        const float curvature = CalculateCurvature(p0, p1, p2);
        const float baseLength = distance(p0, p1) + distance(p1, p2);
        
        //      int baseSegments = static_cast<int>((1.0f + curvature) * baseLength / 20.0f);
        
        // 基础分段数（考虑长度和曲率）
        int baseSegments = static_cast<int>(baseLength / 5.0f);
        
        // 线宽调整因子（宽线需要更多分段）
        int widthFactor = static_cast<int>(lineWidth / 10.0f);
        
        //曲率
        int segments = 3 + curvature * (baseSegments + widthFactor);
        
        // 动态范围控制（2-100段）
        return min(segments, 100);
    }

//     static int CalculateSegments(float angle, float baseLength, float lineWidth) // curvature: [0-180]->[0-1]
//     {
//         int baseSegments = static_cast<int>(baseLength / 5.0f);
//         int widthFactor = static_cast<int>(lineWidth / 10.0f);
//         int segments = 3 + curvature * (baseSegments + widthFactor);
//         return min(segments, 100);
//     }


    // 自动计算分段数（基于曲率和线宽）
//     static int CalculateSegments2(const float2& p0, const float2& p1, 
//         const float2& p2, float lineWidth)
//     {
//         //     if (controlPoints.size() < 3) return 2; // 两点只需1段
//         
//         const float theta = CalcTheta(p0, p1, p2);
//         const float baseLength = distance(p0, p1) + distance(p1, p2);
//         float nSrcMax = baseLength + lineWidth;
//         float out = 3.0 + Powlize(theta, 3.1415926536, 97.0);
//         return out;
// //         
// //         //        int baseSegments = static_cast<int>((1.0f + curvature) * baseLength / 20.0f);
// //         
// //         // 基础分段数（考虑长度和曲率）
// //         int baseSegments = static_cast<int>(baseLength / 5.0f);
// //         
// //         // 线宽调整因子（宽线需要更多分段）
// //         int widthFactor = static_cast<int>(lineWidth / 10.0f);
// //         
// //         //曲率
// //         int segments = 3 + curvature * (baseSegments + widthFactor);
// //         
// //         // 动态范围控制（2-100段）
// //         return min(segments, 100);
//     }

    static float2 CalculateBezier2Point(float2 p0, float2 p1, float2 p2, float t) 
    {
        float u = 1 - t;
        return u * u * p0 + 2 * u * t * p1 + t * t * p2;
    }

    // 计算三次贝塞尔曲线上的点
    static float2 CalculateBezier3Point(float2 p0, float2 p1, float2 p2, float2 p3, float t)
    {
        float u = 1.0f - t;
        float u2 = u * u;
        float t2 = t * t;
        return p0 * (u2 * u) + p1 * (3.0f * u2 * t) + p2 * (3.0f * u * t2) + p3 * (t2 * t);
    }

//     function estimateCurvature(p0, p1, p2, p3) {
//         // 计算控制多边形的周长
//         const perimeter = distance(p0, p1) + distance(p1, p2) + distance(p2, p3);
//         
//         if (perimeter === 0) return 0;
//         
//         // 计算二阶差分向量模长
//         const dx = p0.x - 3*p1.x + 3*p2.x - p3.x;
//         const dy = p0.y - 3*p1.y + 3*p2.y - p3.y;
//         const diffMagnitude = Math.sqrt(dx*dx + dy*dy);
//         
//         // 返回曲率估计（归一化）
//         return Math.min(1.0, diffMagnitude / (perimeter * 0.1));
//         }

    static float2 _CosSin(float angle)
    {
        return _float2(cos(angle), sin(angle));
    }
    
    static float2 _CaclArcPoint(float2 center, float r, float angle)
    {
        return center + r * _CosSin(angle);
    }
    
    static float _ClampAngle(float da ) // da = endAngle - startAngle
    {
        // Clamp angles
        float md = fmodf(da, CFloat::PI2());
        if (md < 0)
            md += CFloat::PI2();
        return md;
    }

//     static float _ClampAngle(float start, float end, int dir) //
//     {
//         // Clamp angles
//         float da = end - start;
//         float md = fmod(da, CFloat::PI2());
//         return dir ? -md : md;
//     }

    static float2 _Rotate90R(float2 center, float2 p1)
    {
        float2 p = { -(p1.y - center.y) + center.x,  (p1.x - center.x) + center.y };
        return p;
    }

    static float2 _Rotate90(float2 center, float2 p1)
    {
        float2 p = { (p1.y - center.y) + center.x,  -(p1.x - center.x) + center.y };
        return p;
    }

public:
    int m_nSkip;

    void MakeBezier2(float2 p0, float2 cp, float2 ed, float w);
    void MakeBezier3(float2 p0, float2 cp1, float2 cp2, float2 ed, float w);
    void MakeArc(float2 p0, float2 center, float2 angle, float2 r, float2 ed, float w);
};

struct CPath
    : public CBaseRefT<CPath, IGXPath2D>
    , public TM::CRefObjectRoot<1>
{
    TM_BGN_INTERFACE_MAP(IUnknown)
    TM_END_INTERFACE_MAP()

    enum EOpt
    { 
        eMoveTo,    // [x,y]
        eLineTo,    // [x,y]
        eBezier2,   // x,y,[cpx,cpy],ex,ey
        eBezier3,   // x,y,[cp1x,cp1y],cp2x,cp2y,ex,ey
        eArc,       // x,y,[cp1x,cp1y],cx,cy,sAngle,da,r,counterclockwise,cp2x,cp2y,ex,ey;
        eMax,
    };

    struct PT
    {
        float2          pt;
        union
        {
            EOpt        opt;
            CTmpPath*   cache;
        };
        
        PT() {}
        PT(float2 _pt, EOpt op = eLineTo) : pt(_pt), opt(op) {}
    };

    struct COnePath : public CVector<PT>
    {
        typedef CList<CTmpPath> CTempsList;

        bool        m_bClose;
        CTempsList  m_aTemps;

        COnePath() : m_bClose(false) {}
        
        float2 LastPt() const
        {
            DbgAssert(this->size());
            return this->back().pt;
        }

        void SetMoveTo(float2 p0)
        {
            this->resize(1);
            *begin() = PT(p0, eMoveTo);
        }
    };

    typedef CList<COnePath> CPathsList;

    CPathsList  m_aPaths;

public:
    CPath()
    {
        m_aPaths.resize(1);
    }

private:
    void _MoveTo(float2 p0, bool bClose = false);
    void _LineTo(float2 p1, EOpt opt = eLineTo);
    void _MoveTo(const CAffine& aff, float x, float y, bool bClose = false);
    void _LineTo(const CAffine& aff, float x, float y, EOpt opt = eLineTo);
    void _AddPath3(float x0, float y0, float x1, float y1, float x2, float y2, EOpt opt);
    COnePath& _cur() { return m_aPaths.back(); }
    const COnePath& _cur() const { return m_aPaths.back(); }
    float2 _CurrPt() const
    {
        DbgAssert(_cur().size());
        return _cur().back().pt;
    }
    
public:
    void Begin(bool bClean);
    void Rect(float x, float y, float w, float h);
    void _Rect(const CAffine& aff, float x, float y, float w, float h);
    void _Arc(float cx, float cy, float r, float sAngle, float dAngle, TMBool counterclockwise);

    ///////////////////////////////////////////////////////////////////////////
    // IGXPath2D
    virtual void AddPath(IGXPath2D* path) tm_override;
    virtual void ClosePath() tm_override;
    virtual void MoveTo(float x, float y) tm_override;
    virtual void LineTo(float x, float y) tm_override;
    virtual void QuadraticCurveTo(float cpx, float cpy, float x, float y) tm_override;
    virtual void BezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y) tm_override;
    virtual void Arc(float cx, float cy, float r, float sAngle, float eAngle, TMBool counterclockwise) tm_override { _Arc(cx, cy, r, sAngle, eAngle - sAngle, counterclockwise); }

    //  void arc() {}
    //  void arcTo() {}
    //     virtual void ellipse() = 0;
    //     virtual void rect() = 0;
//     virtual void roundRect() = 0;
};



#define Vertex VS_INPUT
struct CIndVcts
{
    struct LineCapJoin
    {
        EGXCapJoin          m_eCapJoin;
        float2              m_next; // only for join
        
        LineCapJoin() { }
        LineCapJoin(EGXCapJoin cj) : m_eCapJoin(cj) 
        {
            if (cj >= eGXJoinMiter && cj <= eGXJoinRound)
            {
                int x = 0;
            }
        }
        LineCapJoin(EGXCapJoin cj, float x, float y) : m_eCapJoin(cj) { m_next.x = x; m_next.y = y; }
        LineCapJoin(EGXCapJoin cj, float2 a) : m_eCapJoin(cj), m_next(a) {}
    };

    struct CLast
    {
        TMUInt16 n1;
        TMUInt16 n2;
    };

    typedef CVector<Vertex>     CVctList;
    typedef CVector<TMUInt16>   CIndList;
    enum { eSize = sizeof(Vertex) };

    CVctList        m_aVct;
    CIndList        m_aInd;
    EGXComposite    m_compositeOperation;
    float           m_alpha;
    const CBUFFER*  m_cbuffer;
    
public:
    void _AddVect_4(Vertex* v)
    {
        TMUShort n = m_aVct.size();
        m_aVct.insert(m_aVct.end(), v, v + 4);
        TMUShort u[6] = { n + 0, n + 1, n + 2, n + 2, n + 1, n + 3 };
        m_aInd.insert(m_aInd.end(), u, 6 + &u[0]);
    }

    void _AddVect_3(Vertex* v)
    {
        TMUShort n = m_aVct.size();
        m_aVct.insert(m_aVct.end(), v, v + 3);
        TMUShort u[6] = { n + 0, n + 1, n + 2 };
        m_aInd.insert(m_aInd.end(), u, 3 + &u[0]);
    }

    void Add_CapRound_L(float2 curr, float2 next, float halfWidth)
    {
        Vertex v[4];
        InitCapRound_L(v, curr, next, halfWidth);
        _AddVect_4(v);
    }

    void Add_CapRound_R(float2 curr, float2 next, float halfWidth)
    {
        Vertex v[4];
        InitCapRound_R(v, curr, next, halfWidth);
        _AddVect_4(v);
    }

    void _LastAddQuad_2(CLast& last, Vertex* v)
    {
        TMUShort n = m_aVct.size();
        m_aVct.insert(m_aVct.end(), v, v + 2);
        TMUShort u[6] = { last.n1, last.n2, n + 0, n + 0, last.n2, n + 1 };
        m_aInd.insert(m_aInd.end(), u, 6 + &u[0]);
        last.n1 = n + 0;
        last.n2 = n + 1;
    }

    CLast _AddLineLeft(float2 p0, float2 p1, float w, const LineCapJoin& start);
    void _AddLineRight(CLast& last, float2 p0, float2 p1, float w, const LineCapJoin& end);
    void _StrokeTempPath(CLast& last, const CTmpPath& pc, float width);
    void _FillTempPath(const CTmpPath& pc);
    void _MakeTmpPath(CPath::COnePath& path, CPath::PT* pt, float w);

public:
    CIndVcts() {}
    void Stroke(CPath::COnePath& path, float width, EGXCapJoin cap, EGXCapJoin join);
    void Fill(CPath::COnePath& path, float width);
};


///////////////////////////////////////////////////////////////////////////////
// CGXCanvas
class CGXCanvas
    : public CBaseRefT<CGXCanvas, IGXCanvas>
    , public TM::CRefObjectRoot<1>
{
public:
    TM_BGN_INTERFACE_MAP(IUnknown)
    TM_END_INTERFACE_MAP()

    struct CState //: public CFontAppBase
    {
        EGXComposite compositeOperation;
        CGXStyle fill;
        CGXStyle stroke;
        float strokeWidth;
        float miterLimit;
        EGXCapJoin lineJoin;
        EGXCapJoin lineCap;
        float alpha;
        CAffine aff;
        float fontSize;
        float letterSpacing;
        float lineHeight;
        float fontBlur;
        int textAlign;
        int fontId;

        CState()
            : compositeOperation(eGXCompositeSourceOver)
            , fill(TMRGB(0,0,0))
            , stroke(TMRGB(0,0,0))
            , strokeWidth(1)
            , miterLimit(10)
            , lineJoin(eGXJoinMiter)
            , lineCap(eGXCapButt)
            , alpha(1)
        {
            aff.Reset();
        }
    };

public:
    typedef CList<CIndVcts> CVctsList;
    typedef CList<CBUFFER>  CCBufferList;

    CVctsList       m_aVcts;
    CCBufferList    m_aCBuffer;
    CVector<CState> m_aState;
    CPath           m_path;
    CState          m_stateCur;
    const CBUFFER*  m_pOldFill;
    const CBUFFER*  m_pOldStroke;

private:
    void _Stroke(CPath& path);
    void _Fill(CPath& path, float width);

    void _map(float& x, float& y)
    {
        float _x = m_stateCur.aff.MapX(x, y);
        float _y = m_stateCur.aff.MapY(x, y);
        x = _x;
        y = _y;
    }

    void _mapwh(float& w, float& h)
    {
        float _x = m_stateCur.aff.MapW(w, h);
        float _y = m_stateCur.aff.MapH(w, h);
        w = _x;
        h = _y;
    }

    float _mapA(float x)
    {
        return m_stateCur.aff.MapRad(x);
    }

//     static void _MakeClr(half4& f, TMColor clr)
//     {
//         f.x = 0x3800;// s_tbl_clr2half[255];//ColorR(clr)];
//         f.y = s_tbl_clr2half[ColorG(clr)];
//         f.z = s_tbl_clr2half[ColorB(clr)];
//         f.w = s_tbl_clr2half[ColorA(clr)];
// //         f.x = ((TMUInt16)ColorR(clr)) << 8;
// //         f.y = ((TMUInt16)ColorG(clr)) << 8;
// //         f.z = ((TMUInt16)ColorB(clr)) << 8;
// //         f.w = ((TMUInt16)ColorA(clr)) << 8;
//     }

    static void _MakeClr(float4& f, TMColor clr)
    {
        f.x = ((float)ColorR(clr)) / 255.0f;
        f.y = ((float)ColorG(clr)) / 255.0f;
        f.z = ((float)ColorB(clr)) / 255.0f;
        f.w = ((float)ColorA(clr)) / 255.0f;
    }

    static void _MakeClr(float* r, TMColor clr)
    {
        r[0] = ((float)ColorR(clr)) / 255.0f;
        r[1] = ((float)ColorG(clr)) / 255.0f;
        r[2] = ((float)ColorB(clr)) / 255.0f;
        r[3] = ((float)ColorA(clr)) / 255.0f;
    }

#define CB_COUNT(member)  (offsetof(PS_CONST, member) / sizeof(float4) + 1)
    static void _MakeColor(CBUFFER& pc, TMColor clr)
    {
        pc.style_type = TYPE_COLOR;
        _MakeClr(pc.style_color, clr);
        pc.nCount = CB_COUNT(style_color);
    }

    static bool _MakeCStop(CBUFFER& pc, int nStop, const CGXStyle::ColorStop* cs);
    const CBUFFER& _MakeCBuffer(const CGXStyle& s);

    const CBUFFER& _MakeCBufferF()
    {
        if (m_pOldFill)
            return *m_pOldFill;
        return _MakeCBuffer(m_stateCur.fill);
   }

    const CBUFFER& _MakeCBufferS()
    {
        if (m_pOldStroke)
            return *m_pOldStroke;
        return _MakeCBuffer(m_stateCur.stroke);
    }

public:
    CGXCanvas() : m_pOldFill(0), m_pOldStroke(0) { m_aVcts.resize(1); }
    ~CGXCanvas();
//    void ClearCache() { m_aVcts.clear(); m_aVcts.resize(1); m_path.Begin(true); }

public:
    virtual void SetGlobalCompositeOperation(EGXComposite op) tm_override { m_stateCur.compositeOperation = op; }
    virtual void SetGlobalAlpha(float alpha) tm_override { m_stateCur.alpha = alpha; }
    virtual EGXComposite GetGlobalCompositeOperation() tm_override { return m_stateCur.compositeOperation; }
    virtual float GetGlobalAlpha() tm_override { return m_stateCur.alpha; }

    virtual void Save() tm_override { m_aState.push_back(m_stateCur); }
    virtual void Restore() tm_override { m_pOldFill = m_pOldStroke = 0; m_stateCur = m_aState.back(); m_aState.pop_back(); }

    // style
//     CGXStyle CreateLinearGradient(float x0, float y0, float x1, float y1) {}
//     CGXStyle CreateRadialGradient(float x0, float y0, float r0, float x1, float y1, float r1) {}
//     CGXStyle AddColorStop(CGXStyle& style, float stop, TMColor color) {}
//     CGXStyle CreatePattern(int image, CGXStyle::EFlags flag) {}
    
    virtual void SetFillStyle(CGXStyle style) tm_override { m_pOldFill = 0; m_stateCur.fill = style; }
    CGXStyle GetFillStyle() {}
    virtual void SetStrokeStyle(CGXStyle style) tm_override { m_pOldStroke = 0; m_stateCur.stroke = style; }
    CGXStyle GetStrokeStyle() {}

    virtual void SetLineCap(EGXCapJoin cap) tm_override { m_stateCur.lineCap = cap; }
    virtual void SetLineJoin(EGXCapJoin join) tm_override { m_stateCur.lineJoin = join; }
    virtual void SetLineWidth(float w) tm_override { m_stateCur.strokeWidth = w / 2.0f; }
    virtual void SetMiterLimit(float l) tm_override { m_stateCur.miterLimit = l; }

    // path
    virtual void Rect(float x, float y, float w, float h) tm_override { m_path._Rect(m_stateCur.aff, x, y, w, h); }
    virtual void FillRect(float x, float y, float w, float h) tm_override { CPath p; p._Rect(m_stateCur.aff, x, y, w, h); _Fill(p, m_stateCur.strokeWidth); }
    virtual void StrokeRect(float x, float y, float w, float h) tm_override { CPath p; p._Rect(m_stateCur.aff, x, y, w, h); _Stroke(p); }
    void clearRect() {}
    virtual void Fill() tm_override { this->_Fill(m_path, m_stateCur.strokeWidth); }
    virtual void Stroke() tm_override { this->_Stroke(m_path); }
    virtual void BeginPath() tm_override { m_path.Begin(true); }
    virtual void MoveTo(float x, float y) tm_override { _map(x, y); m_path.MoveTo(x, y); }
    virtual void ClosePath() tm_override { m_path.ClosePath(); }
    virtual void LineTo(float x, float y) tm_override { _map(x, y); m_path.LineTo(x, y); }
    void clip() {}
    virtual void QuadraticCurveTo(float cpx, float cpy, float x, float y) tm_override { _map(cpx, cpy); _map(x, y); m_path.QuadraticCurveTo(cpx, cpy, x, y); }
    virtual void BezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y) tm_override { _map(cp1x, cp1y); _map(cp2x, cp2y); _map(x, y); m_path.BezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y); }
    virtual void Arc(float x, float y, float r, float sAngle, float eAngle, TMBool counterclockwise) tm_override { _map(x, y); m_path._Arc(x, y, r, _mapA(sAngle), eAngle - sAngle, counterclockwise); }
    void arcTo() {}
    void isPointInPath() {}
    
    // transform
    virtual void Scale(float x, float y) tm_override { m_stateCur.aff.Scale(x, y); }
    virtual void Rotate(float angle) tm_override { m_stateCur.aff.RotateR(angle); }
    virtual void Translate(float dx, float dy) tm_override { m_stateCur.aff.Translate(dx, dy); }
    virtual void Transform(float a, float b, float c, float d, float e, float f) { CAffine aff(a, b, c, d, e, f); m_stateCur.aff *= aff; }
    virtual void SetTransform(float a, float b, float c, float d, float e, float f) tm_override { m_stateCur.aff.Set(a, b, c, d, e, f); }

    // text
    virtual void SetFont(const GXFont& font) {}
    virtual void FillText(IGXText* text, float x, float y, float nMaxWidth = -1) {}

    // extern
    virtual void RoundRect(float x, float y, float w, float h, float r) tm_override;
    virtual void Ellipse(float cx, float cy, float rx, float ry) tm_override;

};

// class CCanvasCmd
//     : public CBaseRefT<CCanvasCmd, IGXCanvas>
//     , public TM::CRefObjectRoot<1>
//     {
// public:
//     TM_BGN_INTERFACE_MAP(IUnknown)
//     TM_END_INTERFACE_MAP()
// 
//     enum ECmd
//     {
//         eCmdSetGlobalComposit = 1,      // EGXComposite;
//         eCmdSetGlobalAlpha,             // float;
//         eCmdSave,
//         eCmdRestore,
// 
//         eCmdSetFill,
//         eCmdSetFillColor            = eCmdSetFill + CGXStyle::eTypeColor,           // TMColor
//         eCmdSetFillLinearGradient   = eCmdSetFill + CGXStyle::eTypeLinearGradient,  // StyleLinearGradient + ColorStop[n];
//         eCmdSetFillRadialGradient   = eCmdSetFill + CGXStyle::eTypeRadialGradient,  // StyleRadialGradient + ColorStop[n];
//         eCmdSetFillPattern          = eCmdSetFill + CGXStyle::eTypePattern,         // StylePattern;
// 
//         eCmdSetStroke,
//         eCmdSetStrokeColor          = eCmdSetStroke + CGXStyle::eTypeColor,           // TMColor
//         eCmdSetStrokeLinearGradient = eCmdSetStroke + CGXStyle::eTypeLinearGradient,  // StyleLinearGradient + ColorStop[n];
//         eCmdSetStrokeRadialGradient = eCmdSetStroke + CGXStyle::eTypeRadialGradient,  // StyleRadialGradient + ColorStop[n];
//         eCmdSetStrokePattern        = eCmdSetStroke + CGXStyle::eTypePattern,         // StylePattern;
// 
//         eCmdSetLineCap,                 // EGXCapJoin
//         eCmdSetLineJoin,                // EGXCapJoin
//         eCmdSetLineWidth,               // float
//         eCmdSetMiterLimit,              // float
// 
//         eCmdRect,                       // Rect
//         eCmdFillRect,                   // Rect
//         eCmdStrokeRect,                 // Rect
//         eCmdClearRect,                  // Rect
// 
//         eCmdFill,
//         eCmdStroke,
//         eCmdBeginPath,
//         eCmdMoveTo,                     // Point
//         eCmdClosePath,
//         eCmdLineTo,                     // Point
//         eCmdQuadraticCurveTo,           // Bezier2
//         eCmdBezierCurveTo,              // Bezier3
//         eCmdArc,                        // Arc
//         eCmdArcTo,                      // _ArcTo
// 
//         eCmdScale,                      // Point
//         eCmdRotate,                     // float
//         eCmdTranslate,                  // Point
//         eCmdTransform,                  // CAff
//         eCmdSetTransform,               // CAff
// 
//         eCmdSetFont,                    // GXFont
//         eCmdFillText,                   // _FillText
//     };
// 
//     struct _StyleLinearGradient { int nStop; float x0, y0, x1, y1; };
//     struct _StyleRadialGradient { int nStop; float x0, y0, x1, y1, r0, r1; };
//     struct _StylePattern        { IGXTexture* img; CGXStyle::ERepeat repeat; };
//     struct _Rect                { float l, t, w, h; };
//     struct _Point               { float l, t; };
//     struct _Bezier2             { float cpx, cpy, x, y; };
//     struct _Bezier3             { float cp1x, cp1y, cp2x, cp2y, x, y; };
//     struct _Arc                 { float cx, cy, r, sAngle, eAngle, TMBool; };
//     struct _ArcTo               { float x1, y1, x2, y2, radius; };
//     struct _FillText            { IGXText* text; float x, y, nMaxWidth; };
//     
// public:
//     CAnyStack m_aCmd;
// 
// private:
//     void SetStyle(ECmd eBase, CGXStyle style)
//     {
//         m_aCmd.push((ECmd)(eBase + style.type));
//         switch(style.type)
//         {
//         case CGXStyle::eTypeColor: m_aCmd.push(style.clr); break;
//         case CGXStyle::eTypeLinearGradient:
//             {
//                 _StyleLinearGradient s = { style.lg->nStop, style.lg->x0, style.lg->y0, style.lg->x1, style.lg->y1 };
//                 m_aCmd.push(s);
//                 for (int i = 0; i < style.lg->nStop; ++i)
//                     m_aCmd.push(style.lg->cs[i]);
//             }
//             break;
// 
//         case CGXStyle::eTypeRadialGradient:
//             {
//                 _StyleRadialGradient s = { style.rg->nStop, style.rg->x0, style.rg->y0, style.rg->x1, style.rg->y1, style.rg->r0, style.rg->r1 };
//                 m_aCmd.push(s);
//                 for (int i = 0; i < style.rg->nStop; ++i)
//                     m_aCmd.push(style.rg->cs[i]);
//             }
//             break;
// 
//         case CGXStyle::eTypePattern:
//             style.pattern->image->AddRef();
//             m_aCmd.push(*style.pattern);
//             break;
// 
//         default:
//             DbgAssert(0);
//             break;
//         }
//     }
// 
// public:
//     // composite
//     virtual void SetGlobalCompositeOperation(EGXComposite op) tm_override { m_aCmd.push(eCmdSetGlobalComposit); m_aCmd.push(op); }
//     virtual void SetGlobalAlpha(float alpha) tm_override { m_aCmd.push(eCmdSetGlobalAlpha); m_aCmd.push(alpha); }
//     
//     // other
//     virtual void Save() tm_override { m_aCmd.push(eCmdSave); }
//     virtual void Restore() tm_override { m_aCmd.push(eCmdRestore); }
// 
//     virtual void SetFillStyle(CGXStyle style) tm_override { SetStyle(eCmdSetFill, style); }
//     virtual void SetStrokeStyle(CGXStyle style) tm_override { SetStyle(eCmdSetStroke, style); }
// 
// 
// };


TM_END_NAMESPACE


#endif  // GXCANVAS_H

