/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021.9.6
 *
 *  Copyright (C) 2021 miragekiller
 */

#include "gxsvgdom.h"
#include "../../tmani.h"
#include "../../tmautoptr.h"


TM_BGN_NAMESPACE


///////////////////////////////////////////////////////////////////////////////
// CSvgAniFuncXXX

typedef float (*PFN_CalcFrameFloat)(CSvgAniFunc& func, float start, float end, float tmRate);

static float CalcFrameFloat_ease(CSvgAniFunc& func, float start, float end, float tmRate)
{
    return start + (end - start) * tmRate;
}

static float CalcFrameFloat_linear(CSvgAniFunc& func, float start, float end, float tmRate)
{
    return start + (end - start) * tmRate;
}

static float CalcFrameFloat_steps(CSvgAniFunc& func, float start, float end, float tmRate)
{
    float fStepTime = 1.0f / (float)func.m_nStep;
    float f = fmod(tmRate, fStepTime);
    tmRate -= f;
    if (func.m_bStart)
        tmRate += fStepTime;
    return CalcFrameFloat_linear(func, start, end, tmRate);
}

static float CalcFrameFloat_bezier(CSvgAniFunc& func, float start, float end, float tmRate)
{
    CSVGCubicBezier cb;
    cb.Init(func.m_x1, func.m_y1, func.m_x2, func.m_y2);
    float x = cb.solve(tmRate);
    return CalcFrameFloat_linear(func, start, end, x);
}

static PFN_CalcFrameFloat g_funcAni[] = 
{
    CalcFrameFloat_bezier,
    CalcFrameFloat_bezier,
    CalcFrameFloat_bezier,
    CalcFrameFloat_bezier,
    CalcFrameFloat_linear,
    CalcFrameFloat_steps,
    CalcFrameFloat_bezier,
};

float CSvgAniFunc::CalcFrameFloat(float start, float end, float tmRate)
{
    DbgAssertS(tm_countof(g_funcAni) == eFuncBezier + 1);
    DbgAssert(m_nFunc >= 0);
    DbgAssert(m_nFunc <= eFuncBezier);
    return g_funcAni[m_nFunc](*this, start, end, tmRate);
}

TMBool CSvgAniFunc::Parse(CXmlTag& val)
{
    val = val.TrimLeft();
    if (val.Left(6) == TM_CL("linear"))
        m_nFunc = eFuncLinear, val = val.Mid(7);
    else if (val.Left(11) == TM_CL("ease-in-out"))
        SetBezier(eFuncEaseInOut, 0.42f, 0.0f, 0.58f, 1.0f), val = val.Mid(12);
    else if (val.Left(8) == TM_CL("ease-out"))
        SetBezier(eFuncEaseOut, 0.0f, 0.0f, 0.58f, 1.0f), val = val.Mid(9);
    else if (val.Left(7) == TM_CL("ease-in"))
        SetBezier(eFuncEaseIn, 0.42f, 0.0f, 1.0f, 1.0f), val = val.Mid(8);
    else if (val.Left(4) == TM_CL("ease"))
        SetEase(), val = val.Mid(5);
    else if (val.Left(10) == TM_CL("step-start"))
        m_nFunc = eFuncLinear, val = val.Mid(11);
    else if (val.Left(8) == TM_CL("step-end"))
        m_nFunc = eFuncLinear, val = val.Mid(9);
    else if (val.Left(5) == TM_CL("steps"))
    {
        val = val.Skip('(');
        CXmlTag param;
        size_t n = val.Split(0, ')', param);
        val = val.Mid(n);
        param.ScanLong(m_nStep);
        tm_setmax(m_nStep, 1);
        param = param.Skip(',');
        param = param.Trim();
        m_bStart = false;
        if (param == TM_CL("start") || param == TM_CL("jump-start"))
            m_bStart = true;
        else if (param != TM_CL("") && param != TM_CL("end") && param != TM_CL("jump-end"))
            DbgTrace("CSvgAniFuncAll::Set() unsupported animation_timing_function: %s", TM_W2A(param));
        m_nFunc = eFuncStep;
    }
    else if (val.Left(12) == TM_CL("cubic-bezier"))
    {
        m_nFunc = eFuncBezier;
        CXmlTag param(val.SubPair('(', ')'));
        val.Assign(param.end() + 1, val.end());
        double f1 = 0, f2 = 0, f3 = 0, f4 = 0;
        param = param.ScanDouble(f1);
        param = param.Skip(',');
        param = param.ScanDouble(f2);
        param = param.Skip(',');
        param = param.ScanDouble(f3);
        param = param.Skip(',');
        param = param.ScanDouble(f4);
        m_x1 = f1;
        m_y1 = f2;
        m_x2 = f3;
        m_y2 = f4;
    }
    else
        return false;
    return true;
}




///////////////////////////////////////////////////////////////////////////////
// CSvgAniKeyFrame
enum ESvgAttrib_animation_direction
{
    eSvgAttrib_animation_direction_normal,
    eSvgAttrib_animation_direction_alternate,
};

enum ESvgAttrib_animation_fill_mode
{
    eSvgAttrib_animation_fill_mode_none,
    eSvgAttrib_animation_fill_mode_forwards,
    eSvgAttrib_animation_fill_mode_backwards,
    eSvgAttrib_animation_fill_mode_both,
};

struct CSvgAniKeyFrame : public CSvgAni
{
    typedef TM::CSet<CSvgAniItem> CAniItemList;
    CAniItemList m_frames;

    float GetFrame(float time, CSvgAniItem& itmBgn, CSvgAniItem& itmEnd)
    {
        time = fmod(time, m_dur);
        CAniItemList::recorder rec(m_frames);
        for (const CSvgAniItem* itm1 = &rec.get_data(); ++rec;)
        {
            const CSvgAniItem& itm2 = rec.get_data();
            if (itm2.m_time > time)
            {
                itmBgn = *itm1;
                itmEnd = itm2;
                return (time - itmBgn.m_time) / (itmEnd.m_time - itmBgn.m_time);
            }
            itm1 = &itm2;
        }
        itmBgn = *m_frames.begin();
        itmEnd = itmBgn;
        return 0;
    }

    void Insert(const CSvgAniItem& itm)
    {
        m_frames.insert(itm);
    }
};

struct CA_Del_Attr
{
    void operator ()(CSvgDomAttrib& attr) const { delete attr.m_ani; }
};

CSvgDomNode::~CSvgDomNode()
{
    CChildList::recorder::list_clear(m_childs, TM::CA_Delete());
    CAtrrList::recorder::list_clear(m_attribs, CA_Del_Attr());
}

CSvgDomAttrib& CSvgDomNode::AddAttrib(ESvgAttrib name, CXmlTag val)
{
    DbgAssert(name != eSvgAttrib_unknown);
    
    for (CAtrrList::recorder rec(m_attribs); rec; ++rec)
    {
        CSvgDomAttrib& attr = rec.get_data();
        if (attr.m_eName == name)
            return attr;
    }

    m_attribs.push_back(CSvgDomAttrib());
    CSvgDomAttrib& attr = m_attribs.back();
    attr.m_eName = name;
    attr.m_val = val;
    attr.m_ani = 0;
    return attr;
}


struct CSvgDomParser : public IXmlParser
{
public:
    enum EAniType { eAniTypeNone, eAniTypeAnimate, eAniTypeAnimateTransform, eAniTypeAnimateMotion };

    struct ANIMATION
    {
        enum EMask {
            eMask_none              = 0x00000000,
            eMask_name              = 0x00000001,
            eMask_duration          = 0x00000002,
            eMask_timing_function   = 0x00000004,
            eMask_delay             = 0x00000008,
            eMask_iteration_count   = 0x00000010,
            eMask_direction         = 0x00000020,
            eMask_fill_mode         = 0x00000040
        };
//         TM_ENUM_2_FLAG(CSvgDomParser::ANIMATION::EMask);

        CXmlTag animation_name;
        int animation_iteration_count;
        ESvgAttrib_animation_direction animation_direction;
        ESvgAttrib_animation_fill_mode animation_fill_mode;
        float animation_duration;
        float animation_delay;
//         CXmlTag animation_timing_function;
        CSvgAniFunc animation_timing_function;

        ANIMATION()
            : animation_iteration_count(1)
            , animation_direction(eSvgAttrib_animation_direction_normal)
            , animation_fill_mode(eSvgAttrib_animation_fill_mode_backwards)
            , animation_duration(0)
            , animation_delay(0)
        {
        }

        void CheckRepeat()
        {
            if (animation_iteration_count == 0)
                animation_iteration_count = 1;
        }

        TMBool Parse_fill_mode(CXmlTag val)
        {
            if (val == TM_CL("none"))
                animation_fill_mode = eSvgAttrib_animation_fill_mode_none;
            else if (val == TM_CL("backwards"))
                animation_fill_mode = eSvgAttrib_animation_fill_mode_backwards;
            else if (val == TM_CL("forwards"))
                animation_fill_mode = eSvgAttrib_animation_fill_mode_forwards;
            else if (val == TM_CL("both"))
                animation_fill_mode = eSvgAttrib_animation_fill_mode_both;
            else
                return false;
            return true;
        }

        TMBool Parse_fill_direction(CXmlTag val)
        {
            if (val == TM_CL("alternate"))
                animation_duration = eSvgAttrib_animation_direction_alternate;
            else if (val == TM_CL("normal"))
                animation_duration = eSvgAttrib_animation_direction_normal;
            else
                return false;
            return true;
        }

        void Parse_animation(CXmlTag val)  // animation: name duration timing-function delay iteration-count direction;
        {
            ENumType type;
            val = val.TrimRight();
            for (; !val.IsEmpty();)
            {
                float f = CSvgDomParser::ParseAttrMillisecondOrDouble(val, val, type);
                if (type == CSvgDomParser::eNumTypeMS)
                {
                    if (animation_duration > 0)
                        animation_delay = f;
                    else
                        animation_duration = f;
                }
                else if (type == CSvgDomParser::eNumTypeNum)
                {
                    animation_iteration_count = f;
                    CheckRepeat();
                }
                else if (animation_timing_function.Parse(val))
                    continue;
                else
                {
//                     val = val.TrimLeft();
                    size_t n = val.find_first_of(L" \r\n\t(,;", 0);
                    if (n == val.npos)
                        n = val.length();
                    CXmlTag name = val.Left(n);
//                     if (name == TM_CL("linear" ||
//                         name == TM_CL("ease" ||
//                         name == TM_CL("ease-in" ||
//                         name == TM_CL("ease-out" ||
//                         name == TM_CL("ease-in-out" ||
//                         name == TM_CL("step-start" ||
//                         name == TM_CL("step-end" ||
//                         name == TM_CL("frames")
//                         animation_timing_function = name;
//                     else if (name == TM_CL("cubic-bezier" || name == TM_CL("steps")
//                     {
//                         n = val.find(')', n);
//                         if (n == val.npos)
//                             return;
//                         animation_timing_function = val.Left(n);
//                         val = val.Mid(n + 1);
//                     }
//                     else
                        if (name == TM_CL("infinite"))
                        animation_iteration_count = -1;
                    else if (Parse_fill_mode(name))
                    {
                    }
                    else if (Parse_fill_direction(name))
                    {
                    }
                    else
                    {
                        if (!name.IsEmpty())
                            animation_name = name;
                    }
                    val = val.Mid(n + 1);
                }
            }
        }

        EMask Parse(ESvgAttrib name, CXmlTag val)
        {
            switch (name)
            {
            case eSvgAttrib_animation_name:
                animation_name = val;
                return eMask_name;

            case eSvgAttrib_animation_duration:
                animation_duration = CSvgDomParser::ParseAttrMillisecond(val);
                return eMask_duration;

            case eSvgAttrib_animation_timing_function:
                animation_timing_function.Parse(val);
                return eMask_timing_function;

            case eSvgAttrib_animation_delay:
                animation_delay = CSvgDomParser::ParseAttrMillisecond(val);
                return eMask_delay;

            case eSvgAttrib_animation_iteration_count:
                animation_iteration_count = CSvgDomParser::ParseRepeatCount(val);
                return eMask_iteration_count;

            case eSvgAttrib_animation_direction:
                Parse_fill_direction(val);
                return eMask_direction;

            case eSvgAttrib_animation_fill_mode:
                Parse_fill_mode(val);
                return eMask_fill_mode;
            }
            return eMask_none;
        }

        void Merge(const ANIMATION& rhs, TMUInt mask)
        {
            if (mask & eMask_name)
                DbgAssert(0);// animation_name = rhs.animation_name;
            if (mask & eMask_duration)
                animation_duration = rhs.animation_duration;
            if (mask & eMask_timing_function)
                animation_timing_function = rhs.animation_timing_function;
            if (mask & eMask_delay)
                animation_delay = rhs.animation_delay;
            if (mask & eMask_iteration_count)
                animation_iteration_count = rhs.animation_iteration_count;
            if (mask & eMask_direction)
                animation_direction = rhs.animation_direction;
            if (mask & eMask_fill_mode)
                animation_fill_mode = rhs.animation_fill_mode;
        }
    };
    friend ANIMATION;

    struct ANIDOMNODE : public ANIMATION
    {
        CSvgDomNode* node;
    };
    typedef CVector<ANIDOMNODE>         CNodeList;
    typedef CMap<CXmlTag, CNodeList>    CCssAniNodeList;

    struct CSSANI { float time; ESvgAttrib eAttr; CXmlTag val; };

public:
    CSvgDomDoc* m_doc;
    CSvgDomNode* m_pNodesStack[256];
    int m_head;

    CCssAniNodeList m_aAniCSS;
    CXmlTag m_xmlCSS;

//     CSvgDomAttrib m_tempAttr[20]; // for split attr.

private:
    static ESvgAttrib Name2Type(TM::CXmlTag val)
    {
        struct _svg_attrib_CDecodeTable : public TM::CMap<TM::CStrViewW, ESvgAttrib>
        {
            _svg_attrib_CDecodeTable()
            {
#       define SVG_STR_ATTRIB(x,y)   this->insert2(L###x, eSvgAttrib_##y);
#       include "gxsvgstr_i.h"
            }
        };
        static _svg_attrib_CDecodeTable _table;
        _svg_attrib_CDecodeTable::recorder rec(_table);
        rec.find(val);
        if (rec)
            return rec.get_data();
        if (!val.IsEmpty())
            svg_trace("CSvgDomParser::Name2Type() unknown attrib : %s", TM_W2A(val));
        return eSvgAttrib_unknown;
    }

    static ESvgTag Tag2Type(const TM::CXmlTag& val)
    {
        struct _svg_tag_CDecodeTable : public TM::CMap<TM::CStrViewW, ESvgTag>
        {
            _svg_tag_CDecodeTable()
            {
#       define SVG_STR_TAG(x)   this->insert2(L###x, eSvgTag_##x);
#       include "gxsvgstr_i.h"
            }
        };
        static _svg_tag_CDecodeTable _table;
        _svg_tag_CDecodeTable::recorder rec(_table);
        rec.find(val);
        if (rec)
            return rec.get_data();
        if (!val.IsEmpty())
            svg_trace("CSvgDomParser::Tag2Type() unknown tag : %s", TM_W2A(val));
        return eSvgTag_unknown;
    }

    enum ENumType { eNumTypeMS, eNumTypeNum, eNumTypeError };
    static float ParseAttrMillisecondOrDouble(const TM::CXmlTag& val, TM::CXmlTag& ret, ENumType& eType)
    {
        double x = -DBL_MAX;
        ret = val.ScanDouble(x);
        if (x == -DBL_MAX)
            return eType = eNumTypeError, 0;

        switch (ret[0])
        {
        case 's': ret = ret.Mid(1); eType = eNumTypeMS; return x * 1000;
        case 'm':
            if (ret[1] == 's')
                return ret = ret.Mid(1), eType = eNumTypeMS, x;
            else if (ret[1] == 'i' && ret[2] == 'n')
                return ret = ret.Mid(2), eType = eNumTypeMS, x * (float)(1000 * 60);
            else
                return eType = eNumTypeNum, x;
        case 'h': ret = ret.Mid(1); eType = eNumTypeMS; return x * (float)(3600 * 1000);
        case ':': break;
        default: eType = eNumTypeNum; return x; // second;
        }

        eType = eNumTypeMS;
        double n1 = 0;
        ret = ret.Mid(1);
        ret = ret.ScanDouble(n1);
        if (ret[0] != ':')
            return (x * 60.0f + n1) * 1000.0f; // min:second

        double n2 = 0;
        ret = ret.Mid(1);
        ret = ret.ScanDouble(n2);
        return x * (3600.0f + n1 * 60.0f + x) * 1000.0f; // h:m:s
    }

    static float ParseAttrMillisecond(const TM::CXmlTag& val)
    {
        ENumType type;
        TM::CXmlTag ret;
        return ParseAttrMillisecondOrDouble(val, ret, type);
    }

    static int ParseRepeatCount(const TM::CXmlTag& val)
    {
        if (val == TM_CL("indefinite"))
            return -1;
        int repeatCount = val.ToLong();
        return repeatCount == 0 ? 1 : repeatCount;
    }

    void PushAttrib(ESvgAttrib eName, const TM::CXmlTag& val)
    {
        DbgAssert(eName != eSvgAttrib_unknown);

        CSvgDomAttrib attr;
        attr.m_eName = eName;
        attr.m_val = val;
        attr.m_ani = 0;
        if (eName == eSvgAttrib_opacity || eName == eSvgAttrib_display) // for optimize.
            m_pNodesStack[m_head]->m_attribs.insert(m_pNodesStack[m_head]->m_attribs.begin(), attr);
        else
            m_pNodesStack[m_head]->m_attribs.push_back(attr);
    }

    void AddStyle(const TM::CXmlTag& val)
    {
        ANIDOMNODE ani;
        TMUInt mask;
        CVector<ANIDOMNODE> aAni;

        CXmlTag strItem;
        for (size_t n = 0; n = val.Split(n, L';', strItem);)
        {
            CXmlTag _name;
            size_t x = strItem.Split(0, L":", _name);
            CXmlTag _val = strItem.Mid(x).Trim();
            _name = _name.Trim();
            ESvgAttrib name = Name2Type(_name);
            if (name == eSvgAttrib_unknown)
                continue;
            else if (name == eSvgAttrib_animation)
            {
                for (; x = val.Split(x, L",", strItem);)
                {
                    ANIDOMNODE an;
                    an.Parse_animation(strItem);
                    aAni.push_back(an);
                }
            }
            else
            {
                ANIDOMNODE::EMask _mask = ani.Parse(name, _val);
                if (_mask)
                    mask |= _mask;
                else
                    PushAttrib(name, _val);
            }
        }

        for (CVector<ANIDOMNODE>::recorder rec(aAni); rec; ++rec)
        {
            ANIDOMNODE& anode = rec.get_data();
            anode.Merge(ani, mask);
            anode.node = m_pNodesStack[m_head];
            m_aAniCSS[anode.animation_name].push_back(anode);
        }

        if (!aAni.size() && ani.animation_duration > 0)
        {
            ani.node = m_pNodesStack[m_head];
            m_aAniCSS[ani.animation_name].push_back(ani);
        }
    }

    void AddAni(ANIDOMNODE& ani, double tmRate, ESvgAttrib name, const TM::CXmlTag& value)
    {
        DbgAssert(!value.IsEmpty());
        DbgAssert(value[0] != ' ');
        DbgAssert(value[0] != '(');
        DbgAssert(value[0] != ';');
        DbgAssert(value[0] != '\"');
        DbgAssert(value[0] != '\'');
        DbgAssert(value[0] != '=');
        DbgAssert(value[0] != ':');
        DbgAssert(value.end()[-1] != ' ');
//        DbgAssert(value.end()[-1] != ')'); // transform maybe have ')'
        DbgAssert(value.end()[-1] != ';');
        DbgAssert(value.end()[-1] != '\"');
        DbgAssert(value.end()[-1] != '\'');
        DbgAssert(value.end()[-1] != '=');
        DbgAssert(value.end()[-1] != ':');

        CSvgDomNode* node = ani.node;
        DbgAssert(node);
        CSvgDomAttrib& attr = node->AddAttrib(name, value);
        if (!attr.m_ani)
        {
            attr.m_ani = new CSvgAniKeyFrame();
            attr.m_ani->m_dur = ani.animation_duration;
            attr.m_ani->m_repeatCount = ani.animation_iteration_count;
            tm_setmax(m_doc->m_dur, ani.animation_duration);
        }
        CSvgAniKeyFrame* p = (CSvgAniKeyFrame*)attr.m_ani;
        CSvgAniItem itm;
        itm.m_time = tmRate * ani.animation_duration;
        itm.m_func = ani.animation_timing_function;
        itm.m_val = value;
        p->Insert(itm);
    }

    void AddAniTransform(ANIDOMNODE& ani, double tmRate, ESvgAttrib name, const TM::CXmlTag& type, const TM::CXmlTag& value)
    {
        CStringW str(type);
        str += L'(';
        str += value;
        str += L')';
        DbgAssert(ani.node);
        ani.node->m_aTransformAni.push_back(str);
        TM::CXmlTag v = ani.node->m_aTransformAni.back();
        AddAni(ani, tmRate, name, v);
    }

    void AddCSSAni(CNodeList& aAnis, double tmRate, ESvgAttrib name, const TM::CXmlTag& value)
    {
        for (CNodeList::recorder rec(aAnis); rec; ++rec)
        {
            ANIDOMNODE& ani = rec.get_data();
            DbgAssert(name != eSvgAttrib_animation);
            if (!ani.Parse(name, value))
                AddAni(ani, tmRate, name, value);
        }
    }

    void AddNodeCSS(const TM::CXmlTag& val)
    {
        // @keyframes covfefe5-_o { 0% { opacity: 0; animation-timing-function: steps(1); } 56.6667% { opacity: 1; animation-timing-function: steps(1); } 100% { opacity: 1; } }
        CXmlTag itm;
        for (size_t n = val.Split(0, L"@keyframes ", itm); n = val.Split(n, L"@keyframes ", itm);)
        {
            // covfefe5-_o { 0% { opacity: 0; animation-timing-function: steps(1); } 56.6667% { opacity: 1; animation-timing-function: steps(1); } 100% { opacity: 1; } }
            CXmlTag tagName;
            size_t nn = itm.Split(0, '{', tagName);
            tagName = tagName.Trim();
            if (tagName.IsEmpty())
                continue;
            if (m_aAniCSS.find(tagName) == m_aAniCSS.end())
                svg_trace("CSvgDomParser::AddNodeCSS() not find animate id: %s", TM_W2A(tagName));
            CNodeList& aAnis = m_aAniCSS[tagName];

            CXmlTag tagFrame;
            for (; nn = itm.Split(nn, '}', tagFrame);)
            {
                //0% { opacity: 0; animation-timing-function: steps(1);
                double x = -DBL_MAX;
                tagFrame = tagFrame.ScanDouble(x);
                if (x != -DBL_MAX)
                {
                    if (tagFrame[0] == '%')
                        x /= 100.0;
                    else
                        continue;
                }
                else if (tagFrame.Left(4) == TM_CL("from"))
                {
                    x = 0.0;
                    tagFrame = tagFrame.Mid(4);
                }
                else if (tagFrame.Left(2) == TM_CL("to"))
                {
                    x = 1.0;
                    tagFrame = tagFrame.Mid(2);
                }
                else
                    continue; //  not keyframe
                tagFrame = tagFrame.Skip('{');

                // opacity: 0; animation-timing-function: steps(1);
                CXmlTag tagItm;
                CNodeList aAnisCopy(aAnis);
                typedef CVector<CSSANI> CAniAttrList;
                CAniAttrList aAttribs;
                tm_for (size_t j = 0; j = tagFrame.Split(j, ';', tagItm);)
                {
                    CXmlTag _n;
                    CXmlTag _v;
                    size_t jj = tagItm.Split(0, ':', _n);
                    _v = tagItm.Mid(jj);
                    _n = _n.Trim();
                    if (_n.IsEmpty())
                        continue;
                    ESvgAttrib _ne = Name2Type(_n);
                    if (_ne == eSvgAttrib_unknown)
                    {
                        DbgTrace("CSvgDomParser::AddNodeCSS() unknown CSS attrib: %s", TM_W2A(_n));
                        continue;
                    }
                    else if (_ne == eSvgAttrib_d) // convert "path(' xxxx ')" to "xxxx";
                    {
                        _v = _v.SubPair('(', ')').Trim();
                        _v = _v.substr(1, _v.length() - 2);
                    }
                    else if (_ne == eSvgAttrib_animation_timing_function)
                    {
                        CSvgAniFunc af;
                        af.Parse(_v);
                        for (CNodeList::recorder rec(aAnisCopy); rec; ++rec)
                        {
                            ANIDOMNODE& ani = rec.get_data();
                            ani.animation_timing_function = af;
                        }
                        continue;
                    }

                    CSSANI ani;
                    ani.eAttr = _ne;
                    ani.time = x;
                    ani.val = _v.Trim(); 
                    aAttribs.push_back(ani);
                }

                for (CAniAttrList::recorder rec(aAttribs); rec; ++rec)
                {
                    CSSANI& num = rec.get_data();
                    AddCSSAni(aAnisCopy, num.time, num.eAttr, num.val);
                }
            }
        }
    }

    void DiscardNode()
    {
        CSvgDomNode* p = m_pNodesStack[m_head];
        DbgAssert(p->m_tag != eSvgTag_defs);
        delete p;
        p = m_pNodesStack[m_head - 1];
        p->m_childs.pop_back();
    }

    void AddTagAnimate(EAniType nType)
    {
        CXmlTag keytimes, values, from, to;
        ESvgAttrib name;
        ANIDOMNODE ani;
        CXmlTag type;

        CSvgDomNode* p = m_pNodesStack[m_head];
        for (CSvgDomNode::CAtrrList::recorder rec(p->m_attribs); rec; ++rec)
        {
            CSvgDomAttrib& attr = rec.get_data();
            switch (attr.m_eName)
            {
            case eSvgAttrib_attributeName:  name = Name2Type(attr.m_val); break;
            case eSvgAttrib_type:           type = attr.m_val; break; // for transform
            case eSvgAttrib_keyTimes:       keytimes = attr.m_val; break;
            case eSvgAttrib_values:         values = attr.m_val; break;
            case eSvgAttrib_from:           from = attr.m_val; break;
            case eSvgAttrib_to:             to = attr.m_val; break;
            case eSvgAttrib_dur:            ani.animation_duration = ParseAttrMillisecond(attr.m_val); break;
            case eSvgAttrib_repeatCount:    ani.animation_iteration_count = ParseRepeatCount(attr.m_val); break;
            case eSvgAttrib_attributeType:  tm_fallthrough;
            case eSvgAttrib_fill:           tm_fallthrough; // freeze, remove(default)
            case eSvgAttrib_path:           tm_fallthrough;
            case eSvgAttrib_href:           tm_fallthrough;
            case eSvgAttrib_xlink_href:
                break;
            default: svg_trace("CSvgDomParser::AddTagAnimate() unknown attrib : %s", _Dbg_ESvgAttrib2Str(attr.m_eName)); break;
            }
        }

        ani.node = m_pNodesStack[m_head - 1];
        if (!from.IsEmpty() && !to.IsEmpty())
        {
            if (nType == eAniTypeAnimateTransform)
            {
                AddAniTransform(ani, 0.0, name, type, from);
                AddAniTransform(ani, 1.0, name, type, to);
            }
            else if (nType == eAniTypeAnimate)
            {
                AddAni(ani, 0.0, name, from);
                AddAni(ani, 1.0, name, to);
            }
            else if (nType == eAniTypeAnimateMotion)
            {
            }
            else
                DbgAssert(0);
        }
        else if (!keytimes.IsEmpty() && !values.IsEmpty())
        {
            size_t n1 = 0, n2 = 0;
            for (;;)
            {
                CXmlTag keytime, value;
                n1 = keytimes.Split(n1, ';', keytime);
                n2 = values.Split(n2, ';', value);
                if (n1 == 0 || n2 == 0)
                    break;
                double _tm = keytime.ToDouble();
                value = value.Trim();
                if (nType == eAniTypeAnimateTransform)
                    AddAniTransform(ani, _tm, name, type, value);
                else if (nType == eAniTypeAnimate)
                    AddAni(ani, _tm, name, value);
            }
        }

        DiscardNode();
    }
    
public:
    CSvgDomParser(CSvgDomDoc* doc)
        : m_doc(doc)
        , m_head(-1)
    {
    }

    virtual void AddNode(XmlRange tag) tm_final
    {
        CSvgDomNode* p;
        ESvgTag eTag = Tag2Type(m_doc->m_strFile.Mid(tag.m_data, tag.m_size));
        if (m_head < 0)
            p = &m_doc->m_root;
        else
        {
            p = new CSvgDomNode();
            if (eTag == eSvgTag_defs)
                m_pNodesStack[m_head]->m_childs.insert(m_pNodesStack[m_head]->m_childs.begin(), p); // <defs> need bring to front
            else
                m_pNodesStack[m_head]->m_childs.push_back(p);
        }
        p->m_tag = eTag;
        m_pNodesStack[++m_head] = p;
    }
    
    virtual void EndNode(XmlRange tag) tm_final
    {
        DbgAssert(m_head >= 0);
        DbgAssert(Tag2Type(m_doc->m_strFile.Mid(tag.m_data, tag.m_size)) == m_pNodesStack[m_head]->m_tag);

        // check animation
        switch (m_pNodesStack[m_head]->m_tag)
        {
        case eSvgTag_animate: AddTagAnimate(eAniTypeAnimate); break;
        case eSvgTag_animateTransform: AddTagAnimate(eAniTypeAnimateTransform); break;
        case eSvgTag_style: DiscardNode(); break;
        }

        if (m_head)
            --m_head;
    }
    
    virtual void AddProperty(XmlRange name, XmlRange _val) tm_final
    {
        DbgAssert(m_head >= 0);
        
        ESvgAttrib eName = Name2Type(m_doc->m_strFile.Mid(name.m_data, name.m_size));
        CXmlTag val(m_doc->m_strFile.substr(_val.m_data, _val.m_size));
        switch (eName)
        {
        case eSvgAttrib_unknown:
            return;

        case eSvgAttrib_id:
            m_doc->m_aNodes.insert2(val, m_pNodesStack[m_head]);
            PushAttrib(eName, val);
            break;

        case eSvgAttrib_style:
            AddStyle(val);
            return;
        }

//         int n = SplitAttr(eName, val);
//         for (int i = 0; i < n; ++i)
//             PushAttrib(m_tempAttr[i].m_eName, m_tempAttr[i].m_val);
        PushAttrib(eName, val);
    }
    
    virtual void EndPropertys(XmlRange tag, XmlRange props) tm_final {}

    virtual void AddText(size_t endPrevTag, size_t beforeText, size_t endText, size_t beforeNextTag) tm_override
    {
        if (m_head > 0 && m_pNodesStack[m_head]->m_tag == eSvgTag_style)
            m_xmlCSS = StrMakeView(m_doc->m_strFile.data() + beforeText, endText - beforeText);
    }

    virtual void AddProcessingInstruction(XmlRange target, XmlRange data) tm_final {}

    virtual void AddComment(XmlRange commnet) tm_override {}

    virtual void EndDoc() tm_final
    {
        AddNodeCSS(m_xmlCSS);
        m_aAniCSS.clear();
    }
};

void CSvgDomDoc::LoadXml(const CStringW& strFile)
{
    m_strFile = strFile;
    CSvgDomParser parser(this);
    XmlParse(m_strFile, &parser);
}

CSvgDomNode* CSvgDomDoc::FindNode(CXmlTag tag)
{
    CDomNodeByIDList::recorder rec(m_aNodes);
    rec.find(tag);
    if (rec)
        return rec.get_data();
    return 0;
}


TM_END_NAMESPACE



