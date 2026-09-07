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

#ifndef GXSVGDOM_H
#define GXSVGDOM_H

#include "tmxml.h"
#include "tmlist.h"


#define svg_trace DbgTrace


TM_BGN_NAMESPACE


// ESvgAttrib
enum ESvgAttrib
{
    eSvgAttrib_unknown = 0,

#   define SVG_STR_ATTRIB(x,y) eSvgAttrib_##y,
#   include "gxsvgstr_i.h"
};

inline const char* _Dbg_ESvgAttrib2Str(ESvgAttrib attr)
{
    switch (attr)
    {
#define SVG_STR_ATTRIB(x,y) case eSvgAttrib_##y: return #x;
#   include "gxsvgstr_i.h"
    }
    return 0;
}

// ESvgTag
enum ESvgTag
{
    eSvgTag_unknown = 0,
        
#   define SVG_STR_TAG(x) eSvgTag_##x,
#   include "gxsvgstr_i.h"
};

inline const char* _Dbg_ESvgTag2Str(ESvgTag tag)
{
    switch (tag)
    {
#define SVG_STR_TAG(x) case eSvgTag_##x: return #x;
#   include "gxsvgstr_i.h"
    }
    return 0;
}

// ESvgUnit
// enum ESvgUnit
// {
//     eSvgUnit_user,
//     eSvgUnit_precent,
//     eSvgUnit_millisecond,
//     eSvgUnit_PX,
//     eSvgUnit_PT,
//     eSvgUnit_PC,
//     eSvgUnit_MM,
//     eSvgUnit_CM,
//     eSvgUnit_IN,
//     eSvgUnit_EM,
//     eSvgUnit_EX,
// };

// ESvgUsage
enum ESvgUsage
{
    eSvgUsage_no_animate = 0x100,

    eSvgUsage_user = 0,
    eSvgUsage_matrix,
    eSvgUsage_translate,
    eSvgUsage_scale,
    eSvgUsage_rotate,
    eSvgUsage_skewX,
    eSvgUsage_skewY,

    // path command, M, m; L, l, H, h, V, v; C, c, S, s; Q, q, T, t; A, a; Z, z;
    // CSvgNum::m_usage == path command: CSvgNum::m_count = data counts;
    // CSvgNum::m_usage == eSvgUsage_user: CSvgNum::m_val = data;
    eSvgUsage_path_mask = 0xff,  
    eSvgUsage_path_M = 'M',
    eSvgUsage_path_m = 'm',
    eSvgUsage_path_L = 'L',
    eSvgUsage_path_l = 'l',
    eSvgUsage_path_H = 'H',
    eSvgUsage_path_h = 'h',
    eSvgUsage_path_V = 'V',
    eSvgUsage_path_v = 'v',
    eSvgUsage_path_C = 'C',
    eSvgUsage_path_c = 'c',
    eSvgUsage_path_S = 'S',
    eSvgUsage_path_s = 's',
    eSvgUsage_path_Q = 'Q',
    eSvgUsage_path_q = 'q',
    eSvgUsage_path_T = 'T',
    eSvgUsage_path_t = 't',
    eSvgUsage_path_A = 'A',
    eSvgUsage_path_a = 'a',
    eSvgUsage_path_Z = 'Z',
    eSvgUsage_path_z = 'z',
};
TM_ENUM_2_FLAG(ESvgUsage);

// CSvgNum
struct CSvgNum
{
    ESvgUsage           m_usage;
    std::wstring_view   m_unit;
    union
    {
        int             m_count; // m_usage == path command
        float           m_val;
    };
};

// CSvgAniFunc
struct CSvgAniFunc
{
    enum EFunc { eFuncEase, eFuncEaseIn, eFuncEaseOut, eFuncEaseInOut, eFuncLinear, eFuncStep, eFuncBezier };
    EFunc m_nFunc;
    union
    {
        struct // eFuncStep
        {
            long    m_nStep;
            bool    m_bStart;
        };
        struct // eFuncBezier
        {
            float   m_x1, m_y1, m_x2, m_y2;
        };
    };

    CSvgAniFunc() { SetEase(); }
    void SetEase() { SetBezier(eFuncEase, 0.25f, 0.1f, 0.25f, 1.0f); }
    void SetBezier(EFunc f, float x1, float y1, float x2, float y2) { m_nFunc = f; m_x1 = x1; m_y1 = y1; m_x2 = x2; m_y2 = y2; }
    float CalcFrameFloat(float start, float end, float tmRate);
    TMBool Parse(CXmlTag& val); // return: is function; val will pointer to next string 
};

// CSvgAniItem
struct CSvgAniItem
{
    float m_time;
    CXmlTag m_val;
    CSvgAniFunc m_func;
    
    CSvgAniItem() {}
    bool operator < (const CSvgAniItem& rhs) const { return m_time < rhs.m_time; }
    float CalcFrameFloat(float start, float end, float tmRate) { return m_func.CalcFrameFloat(start, end, tmRate); } // tmRate: 0 - 1.0f
};

// CSvgAni
struct CSvgAni
{
    TM_SELF_ALLOC(CSvgAni);

    float m_dur;        // ms
    int m_repeatCount;
    bool m_bFillFreeze;

public:
    CSvgAni() : m_dur(0), m_repeatCount(1), m_bFillFreeze(false) {}
    virtual float GetFrame(float time, CSvgAniItem& itmBgn, CSvgAniItem& itmEnd) = 0;
};

// CSvgDomAttrib
struct CSvgDomAttrib
{
    ESvgAttrib m_eName;
    CXmlTag m_val;
    CSvgAni* m_ani;
};

// CSvgDomNode
struct CSvgDomNode : public CNoCopyable
{
    TM_SELF_ALLOC_F(CSvgDomNode);
#ifdef TM_DBG
    int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf)
    {
        return IC_Size(m_attribs) + IC_Size(m_childs) + IC_Size(m_aTransformAni);
    }
#endif // TM_DBG

    typedef CVector<CSvgDomAttrib>  CAtrrList;
    typedef CVector<CSvgDomNode*>   CChildList;

    ESvgTag m_tag;
    CAtrrList m_attribs;
    CChildList m_childs;
    CList<CStringW> m_aTransformAni; // for <animateTransform>

    ~CSvgDomNode();
    CSvgDomAttrib& AddAttrib(ESvgAttrib name, CXmlTag val);
};

// CSvgDomDoc
struct CSvgDomDoc : public CNoCopyable
{
    typedef CMap<CXmlTag, CSvgDomNode*> CDomNodeByIDList;   // <id, CSvgDomNode*>

    CStringW m_strFile;
    CSvgDomNode m_root;
    float m_dur;
    CDomNodeByIDList m_aNodes;

    CSvgDomDoc() : m_dur(0) {}
    void LoadXml(const CStringW& strFile);
    CSvgDomNode* FindNode(CXmlTag tag);
};

TM_END_NAMESPACE



#endif // GXSVGDOM_H