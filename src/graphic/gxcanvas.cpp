/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2025.10.20
 *
 *  Copyright (C) 2025 miragekiller
 */

#include "gxcanvas.h"
#include "gxapp_d3d9.h"

#include <d3dx9.h>
#pragma comment (lib, "d3dx9.lib") // for IID_IDirect3DTexture9

// #define uint16 TMUInt16
// #define uint32 TMUInt32
// 
//         /** Converts float in uint32 format to a a half in uint16 format
//         */
//         static inline uint16 floatToHalfI(uint32 i)
//         {
//             int s =  (i >> 16) & 0x00008000;
//             int e = ((i >> 23) & 0x000000ff) - (127 - 15);
//             int m =   i        & 0x007fffff;
//         
//             if (e <= 0)
//             {
//                 if (e < -10)
//                 {
//                     return 0;
//                 }
//                 m = (m | 0x00800000) >> (1 - e);
//         
//                 return static_cast<uint16>(s | (m >> 13));
//             }
//             else if (e == 0xff - (127 - 15))
//             {
//                 if (m == 0) // Inf
//                 {
//                     return static_cast<uint16>(s | 0x7c00);
//                 } 
//                 else    // NAN
//                 {
//                     m >>= 13;
//                     return static_cast<uint16>(s | 0x7c00 | m | (m == 0));
//                 }
//             }
//             else
//             {
//                 if (e > 30) // Overflow
//                 {
//                     return static_cast<uint16>(s | 0x7c00);
//                 }
//         
//                 return static_cast<uint16>(s | (e << 10) | (m >> 13));
//             }
//         }
// 
//         static inline uint16 floatToHalf(float i)
//         {
//             union { float f; uint32 i; } v;
//             v.f = i;
//             return floatToHalfI(v.i);
//         }
// 

TM_BGN_NAMESPACE

///////////////////////////////////////////////////////////////////////////////
// CTmpPath
void CTmpPath::MakeBezier2(float2 p0, float2 cp, float2 ed, float w)
{
//     tm_for (int i = 0; i < 257; ++i)
//         s_X[i] = floatToHalf( (float)i / 256.0f);

    m_nSkip = 0;
    this->push_back(p0);
    int segments = CalculateSegments(p0, cp, ed, w);
    for (int i = 1; i < segments; ++i)
    {
        float2 n0 = CalculateBezier2Point(p0, cp, ed, (float)i / (float)segments);
        this->push_back(n0);
    }
    this->push_back(ed);
}

void CTmpPath::MakeBezier3(float2 p0, float2 cp1, float2 cp2, float2 ed, float w)
{
    m_nSkip = 1;
    this->push_back(p0);
    int segments1 = CalculateSegments(p0,  cp1, cp2, w);
    int segments2 = CalculateSegments(cp1, cp2,  ed, w);
    int segments = (segments1 + segments2) * 3 / 4;
    for (int i = 1; i < segments; ++i)
    {
        float2 n0 = CalculateBezier3Point(p0, cp1, cp2, ed, (float)i / (float)segments);
        this->push_back(n0);
    }
    this->push_back(ed);
}

void CTmpPath::MakeArc(float2 p0, float2 center, float2 angle, float2 r, float2 ed, float w)
{
    m_nSkip = 4;
    this->push_back(p0);
    float segments = ((r.x + w) / 50) * 8 + 16; // (segments1 + segments2) * 3 / 4;
    tm_setmin(segments, 64.0f);
    float da = angle.y;// _ClampAngle(angle.x, angle.y, r.y);
    DbgAssert(da > 0);
    if (r.y)
    {
        da -= CFloat::PI2();
        segments = -(int)(segments * da / CFloat::PI2());
    }
    else
        segments = (int)(segments * da / CFloat::PI2());

    DbgAssert(segments >= 0);
    for (int i = 1; i < segments; ++i)
    {
        float2 n0 = _CaclArcPoint(center, r.x, (da * (float)i / segments) + angle.x);
        this->push_back(n0);
    }
    this->push_back(ed);
}

///////////////////////////////////////////////////////////////////////////////
// CPath
void CPath::_MoveTo(float2 p0, bool bClose)
{
    COnePath* cur = &_cur();
    if(cur->size() > 1)
    {
        cur->m_bClose = bClose; 
        m_aPaths.push_back(COnePath());
        cur = &_cur();
    }
    cur->SetMoveTo(p0);
}

void CPath::_LineTo(float2 p1, EOpt opt)
{
    COnePath& cur = _cur();
    if (cur.size())
    {
        if (p1 != cur.back().pt)
            cur.push_back(PT(p1, opt));
    }
    else
        _MoveTo(p1);
}

void CPath::_MoveTo(const CAffine& aff, float x, float y, bool bClose)
{
    float _x = aff.MapX(x, y);
    float _y = aff.MapY(x, y);
    _MoveTo(_float2(_x, _y), bClose);
}

void CPath::_LineTo(const CAffine& aff, float x, float y, EOpt opt)
{
    float _x = aff.MapX(x, y);
    float _y = aff.MapY(x, y);
    _LineTo(_float2(_x, _y), opt);
}


void CPath::_AddPath3(float x0, float y0, float x1, float y1, float x2, float y2, EOpt opt)
{
    _LineTo(_float2(x0, y0), opt);
    _LineTo(_float2(x1, y1), opt);
    _LineTo(_float2(x2, y2), eLineTo);
}

void CPath::Begin(bool bClean)
{
    if (m_aPaths.size() > 1)
    {
        (*m_aPaths.begin()).swap(m_aPaths.back());
        m_aPaths.resize(1);
    }
    
    COnePath& cur = _cur();
    cur.m_bClose = false;
    if (bClean)
    {
        cur.clear();
        cur.m_aTemps.clear();
    }
    else if (cur.size() > 1)
    {
        (*cur.begin()) = cur.back();
        cur.resize(1);
        cur.back().opt = eMoveTo;
    }
}

void CPath::Rect(float x, float y, float w, float h)
{
    _MoveTo(_float2(x,     y),     false);
    _LineTo(_float2(x,     y + h), eLineTo);
    _LineTo(_float2(x + w, y + h), eLineTo);
    _LineTo(_float2(x + w, y),     eLineTo);
    ClosePath();
}

void CPath::_Rect(const CAffine& aff, float x, float y, float w, float h)
{
    _MoveTo(aff, x,     y,     false);
    _LineTo(aff, x,     y + h, eLineTo);
    _LineTo(aff, x + w, y + h, eLineTo);
    _LineTo(aff, x + w, y,     eLineTo);
    ClosePath();
}


void CPath::AddPath(IGXPath2D* path)
{
    DbgAssert(m_aPaths.size());
    CPath* r = static_cast<CPath*>(path);
    CPathsList::iterator iter = m_aPaths.end();
    m_aPaths.insert(--iter, r->m_aPaths.begin(), r->m_aPaths.end());
}

void CPath::ClosePath()
{
    if (_cur().size() > 1)
        _MoveTo(_CurrPt(), true);
}

void CPath::MoveTo(float x, float y)
{
    _MoveTo(_float2(x, y));
}

void CPath::LineTo(float x, float y)
{
    _LineTo(_float2(x, y));
}

void CPath::QuadraticCurveTo(float cpx, float cpy, float x, float y)
{
    _LineTo(_float2(cpx, cpy), eBezier2);
    _LineTo(_float2(x,   y  ), eLineTo);
}

void CPath::BezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y)
{
    _AddPath3(cp1x, cp1y, cp2x, cp2y, x, y, eBezier3);
}

void CPath::_Arc(float cx, float cy, float r, float sAngle, float dAngle, TMBool counterclockwise)
{
    float2 center = { cx, cy };
    float da = CTmpPath::_ClampAngle(dAngle);
    float2 b = CTmpPath::_CaclArcPoint(center, r, sAngle);
    if (da == 0)
    {
        _LineTo(b, eLineTo);
        return;
    }

    float2 e = CTmpPath::_CaclArcPoint(center, r, sAngle + da);
    float2 cp1, cp2;
    if (counterclockwise)
    {// 
        cp1 = CTmpPath::_Rotate90R(b, center);
        cp2 = CTmpPath::_Rotate90 (e, center);
    }
    else
    {
        cp1 = CTmpPath::_Rotate90 (b, center);
        cp2 = CTmpPath::_Rotate90R(e, center);
    }
    _LineTo(b, eLineTo);
    _LineTo(cp1, eArc);
    _AddPath3(cx, cy, sAngle, da, r, counterclockwise, eLineTo);
//     _LineTo(cp1, eLineTo);
    _LineTo(cp2, eLineTo);
    _LineTo(e, eLineTo);
}


///////////////////////////////////////////////////////////////////////////////
// CIndVcts
CIndVcts::CLast CIndVcts::_AddLineLeft(float2 p0, float2 p1, float w, const LineCapJoin& start)
{
    DbgAssert(p0 != p1);

    EGXCapJoin join = start.m_eCapJoin;
    if (join == eGXCapRound)
    {
        Add_CapRound_L(p0, p1, w);
        join = eGXCapButt;
    }
    
    Vertex x[4];
    if (join == eGXCapSquare)
        InitCapSquare_L(x, p0, p1, w);
    else if (join == eGXCapButt)
        InitCapButt_L(x, p0, p1, w);
    else if (join == eGXJoinMiter)
        InitJoinMiter_L(x, start.m_next, p0, p1, w);
    else if (join == eGXJoinBevel || join == eGXJoinRound)
        InitJoinBevel_L(x, start.m_next, p0, p1, w);
    CLast last;
    last.n1 = m_aVct.size();
    last.n2 = last.n1 + 1;
    m_aVct.push_back(x[0]);
    m_aVct.push_back(x[1]);
    return last;

}

void CIndVcts::_AddLineRight(CLast& last, float2 p0, float2 p1, float w, const LineCapJoin& end)
{
    DbgAssert(p0 != p1);
    Vertex x[4];
    if (end.m_eCapJoin == eGXCapSquare)
        InitCapSquare_R(x, p0, p1, w);
    else if (end.m_eCapJoin == eGXCapButt || end.m_eCapJoin == eGXCapRound)
        InitCapButt_R(x, p0, p1, w);
    else if (end.m_eCapJoin == eGXJoinMiter)
        InitJoinMiter_R(x, p0, p1, end.m_next, w);
    else if (end.m_eCapJoin == eGXJoinBevel || end.m_eCapJoin == eGXJoinRound)
        InitJoinBevel_R(x, p0, p1, end.m_next, w);
    
    _LastAddQuad_2(last, &x[2]);
    
    
    // add extend tri
    if (end.m_eCapJoin == eGXJoinBevel || end.m_eCapJoin == eGXJoinRound)
    {
        InitJoinBevel_L(x, p0, p1, end.m_next, w); 
        _LastAddQuad_2(last, &x[0]);
        if (end.m_eCapJoin == eGXJoinRound)
        {
            InitJoinRound_Cap(x, p0, p1, end.m_next, w);
            _AddVect_3(x);
        }
    }
    else if (end.m_eCapJoin == eGXCapRound)
    {
        Add_CapRound_R(p0, p1, w);
    }
}

void CIndVcts::_StrokeTempPath(CLast& last, const CTmpPath& pc, float width)
{
    for (int i = 1; i < pc.size() - 1; ++i)
    {
        LineCapJoin lcj(eGXJoinMiter, pc[i + 1]);
        _AddLineRight(last, pc[i - 1], pc[i], width, lcj);
    }
}

void CIndVcts::_FillTempPath(const CTmpPath& pc)
{
    for (int i = 1; i < pc.size() - 1; ++i)
    {
        VS_INPUT vi;
        InitCap(vi, MODE_TRI_NORMAL, 0, pc[i]);
        m_aVct.push_back(vi);
    }
}

void CIndVcts::_MakeTmpPath(CPath::COnePath& path, CPath::PT* pt, float w)
{
    path.m_aTemps.push_back(CTmpPath());
    CTmpPath& tp = path.m_aTemps.back();
    switch(pt[0].opt)
    {
    case CPath::eBezier2: // body::bezier2
        tp.MakeBezier2(pt[-1].pt, pt->pt, pt[1].pt, w);
        break;
        
    case CPath::eBezier3: // body::bezier3
        tp.MakeBezier3(pt[-1].pt, pt->pt, pt[1].pt, pt[2].pt, w);
        break;
        
    case CPath::eArc: // body::eArc
        tp.MakeArc(pt[-1].pt, pt[1].pt, pt[2].pt, pt[3].pt, pt[5].pt, w);
        break;
        
    default:
        DbgAssert(0);
        break;
    }
    pt[0].cache = &tp;
}

void CIndVcts::Stroke(CPath::COnePath& path, float width, EGXCapJoin cap, EGXCapJoin join)
{
    int nSize = path.size();
    if (nSize < 2)
        return;
    bool bClose = nSize > 2 ? path.m_bClose : false;
    bool bSameHT = path.LastPt() == path[0].pt;
    CLast last;
    
    // head
    if (bClose)
    {
        if (bSameHT)
            last = _AddLineLeft(path[0].pt, path[1].pt, width, LineCapJoin(join, path[path.size() - 2].pt));
        else
            last = _AddLineLeft(path[0].pt, path[1].pt, width, LineCapJoin(join, path.LastPt()));
    }
    else
        last = _AddLineLeft(path[0].pt, path[1].pt, width, cap);
    
    // body
    int i = 1;
    for (; ; ++i)
    {
        CPath::PT* pt = &path[i];
        if (i >= nSize - 1) //tail
        {
            DbgAssert(pt->opt != CPath::eBezier2);
            if (bClose) // closed
            {
                if (bSameHT)
                    _AddLineRight(last, pt[-1].pt, pt->pt, width, LineCapJoin(join, path[1].pt));
                else
                {
                    _AddLineRight(last, pt[-1].pt, pt->pt, width, LineCapJoin(join, path[0].pt));
                    _AddLineRight(last, pt->pt, path[0].pt, width, LineCapJoin(join, path[1].pt));
                }
                break;
            }
            else // not closed
                _AddLineRight(last, pt[-1].pt, pt->pt, width, cap);
            break;
        }
        
        if (pt->opt == CPath::eLineTo)
        {
            LineCapJoin lcj(join, pt[1].pt);
            _AddLineRight(last, pt[-1].pt, pt->pt, width, lcj);
            continue;
        }
        
        if (pt->opt < CPath::eMax)
            _MakeTmpPath(path, pt, width);
        
        DbgAssert(pt->opt > CPath::eMax);
        const CTmpPath& pc = *pt->cache;
        _StrokeTempPath(last, pc, width);
        i += pc.m_nSkip;
    }
}

void CIndVcts::Fill(CPath::COnePath& path, float width)
{
    for (int i = 0; i < path.size(); ++i)
    {
        CPath::PT pt = path[i];
        if (pt.opt == CPath::eLineTo || pt.opt == CPath::eMoveTo)
        {
            VS_INPUT vi;
            InitCap(vi, MODE_TRI_NORMAL, 0, path[i].pt);
            m_aVct.push_back(vi);
            continue;
        }
        
        if (pt.opt < CPath::eMax)
            _MakeTmpPath(path, &path[i], width);
        
        DbgAssert(path[i].opt > CPath::eMax);
        const CTmpPath& pc = *path[i].cache;
        _FillTempPath(pc);
        i += pc.m_nSkip;
    }
}


///////////////////////////////////////////////////////////////////////////////
// CGXCanvas
CGXCanvas::~CGXCanvas()
{
    for (CCBufferList::recorder rec(m_aCBuffer); rec; ++rec)
    {
        CBUFFER& cb = rec.get_data();
        if (cb.style_type == TYPE_PATTERN)
        {
            DbgAssert(cb.image);
            cb.image->Release();
        }
    }
}

void CGXCanvas::_Stroke(CPath& path)
{
    const CBUFFER& buf = _MakeCBufferS();
    CIndVcts* vcts = &m_aVcts.back();
    if (vcts->m_cbuffer != &buf)
    {
        m_aVcts.push_back(CIndVcts());
        vcts = &m_aVcts.back();
        vcts->m_cbuffer = &buf;
    }
    for (CPath::CPathsList::recorder rec(path.m_aPaths); rec; ++rec)
        vcts->Stroke(rec.get_data(), m_stateCur.strokeWidth, m_stateCur.lineCap, m_stateCur.lineJoin);
}

void CGXCanvas::_Fill(CPath& path, float width)
{
    // fill need stencil buffer, so need render one by one.
    if (m_aVcts.back().m_aVct.size())
        m_aVcts.push_back(CIndVcts());
    const CBUFFER& buf = _MakeCBufferF();
    for (CPath::CPathsList::recorder rec(path.m_aPaths); rec; ++rec)
    {
        CIndVcts& vcts = m_aVcts.back();
        vcts.m_cbuffer = &buf;
        vcts.Fill(rec.get_data(), width);
        m_aVcts.push_back(CIndVcts());
    }
}

bool CGXCanvas::_MakeCStop(CBUFFER& pc, int nStop, const CGXStyle::ColorStop* cs)
{
    IGXTexture* tex = 0;
    _gxApp->TextureCreateIndirect(&tex, nStop, 2, eGXFormatA32B32G32R32F);
    if (!tex)
        return false;
    tm_setmin(nStop, CGXStyle::eMaxStop);
    pc.image                    = tex;
    pc.gradient_stopcount       = nStop;
    pc.nCount                   = CB_COUNT(gradient_pos1);
    GXDibData dib;
    dib.wb = nStop * sizeof(float4);
    float4* p = (float4*)_alloca(2 * dib.wb);
    dib.bits = p;
    for (int i = 0; i < nStop; ++i)
    {
        p[i].x = cs[i].stop;
        _MakeClr(p[i + nStop], cs[i].color);
    }
    tex->ResetAsObject(&dib, eGXResetCopyDib);
    return true;
}

const CBUFFER& CGXCanvas::_MakeCBuffer(const CGXStyle& s)
{
    CBUFFER pc;
    pc.style_alpha = m_stateCur.alpha;

    switch (s.type)
    {
    case s.eTypeColor:
        _MakeColor(pc, s.clr);
        break;
        
    case s.eTypeLinearGradient:
    case s.eTypeRadialGradient:
        if (s.lg->nStop < 2)
        {
            _MakeColor(pc, TMRGB(0,0,0));
            break;
        }

        pc.gradient_pos0.x      = s.rg->x0;
        pc.gradient_pos0.y      = s.rg->y0;
        pc.gradient_pos1.x      = s.rg->x1;
        pc.gradient_pos1.y      = s.rg->y1;
        _map(pc.gradient_pos0.x, pc.gradient_pos0.y);
        _map(pc.gradient_pos1.x, pc.gradient_pos1.y);
        if (s.type == s.eTypeRadialGradient)
        {
            pc.rgradient_r0     = s.rg->r0;
            pc.rgradient_r1     = s.rg->r1;
        }

        if (s.lg->nStop == 2)
        {
            DbgAssertS1(TYPE_LGRADIENT == s.eTypeLinearGradient);
            DbgAssertS2(TYPE_RGRADIENT == s.eTypeRadialGradient);

            pc.nCount           = CB_COUNT(gradient_clr1);
            pc.style_type       = s.type;
            pc.gradient_stop0   = s.rg->cs[0].stop;
            pc.gradient_stop1   = s.rg->cs[1].stop;
            _MakeClr(pc.gradient_clr0, s.rg->cs[0].color);
            _MakeClr(pc.gradient_clr1, s.rg->cs[1].color);
        }
        else
        {
            pc.style_type       = s.type + (TYPE_LGRADIENT_N - TYPE_LGRADIENT);
            _MakeCStop(pc, s.rg->nStop, s.rg->cs);
        }
        break;

    case s.eTypePattern:
        if (s.pattern->image)
        {
            DbgAssertS1(PT_REPEAT_NON == CGXStyle::eRepeatNon);
            DbgAssertS2(PT_REPEAT_X   == CGXStyle::eRepeatX);
            DbgAssertS3(PT_REPEAT_Y   == CGXStyle::eRepeatY);
            DbgAssertS4(PT_REPEAT_XY  == CGXStyle::eRepeatXY);

            pc.image                = s.pattern->image;
            pc.nCount               = CB_COUNT(gradient_pos1);
            pc.style_type           = TYPE_PATTERN;
            pc.pattern_repeat       = s.pattern->repeat;
            pc.image->AddRef();
            CAffine& aff = ((CAffine&)pc.pattern_aff_m11);
            aff = m_stateCur.aff;
            float w = pc.image->GetWidth();
            float h = pc.image->GetHeight();
            aff.Invert();
            aff.Scale(1.0f/w,  1.0f/h);
        }
        break;
    }
    m_aCBuffer.push_back(pc);
    return m_aCBuffer.back();
}

void CGXCanvas::RoundRect(float x, float y, float w, float h, float r)
{
    tm_setmin(r, w / 2.0f);
    tm_setmin(r, h / 2.0f);
    MoveTo(x + r, y);
    Arc(x + w - r, y + r, r, CFloat::PI() * 3.0 / 2.0, 0, false);
    Arc(x + w - r, y + h - r, r, 0, CFloat::PI() / 2.0, false);
    Arc(x + r, y + h - r, r, CFloat::PI() / 2.0, CFloat::PI(), false);
    Arc(x + r, y + r, r, CFloat::PI(), CFloat::PI() * 3.0 / 2.0, false);
    ClosePath();
}

void CGXCanvas::Ellipse(float cx, float cy, float rx, float ry) tm_override
{
#   define NVG_KAPPA90 0.5522847493f
    MoveTo(cx-rx, cy);
    BezierCurveTo(cx-rx, cy+ry*NVG_KAPPA90, cx-rx*NVG_KAPPA90, cy+ry, cx, cy+ry);
    BezierCurveTo(cx+rx*NVG_KAPPA90, cy+ry, cx+rx, cy+ry*NVG_KAPPA90, cx+rx, cy);
    BezierCurveTo(cx+rx, cy-ry*NVG_KAPPA90, cx+rx*NVG_KAPPA90, cy-ry, cx, cy-ry);
    BezierCurveTo(cx-rx*NVG_KAPPA90, cy-ry, cx-rx, cy-ry*NVG_KAPPA90, cx-rx, cy);
    ClosePath();
}

TM_END_NAMESPACE




