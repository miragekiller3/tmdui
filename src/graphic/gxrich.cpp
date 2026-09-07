/**
 *  @file
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2016.10.17
 *
 *  Copyright (C) 2016 miragekiller                               
 */

#include "gxrich.h"
#include "../tmxmlstring.h"
#include "../tmdebugext.h"


#ifndef WB_MOVEWORDLEFT
#   define WB_MOVEWORDLEFT     4
#   define WB_MOVEWORDRIGHT    5
#endif


/*
// *  WB_CLASSIFY
// *      Returns char class and word break flags of char at start position.
// *
// *  WB_ISDELIMITER
// *      Returns TRUE iff char at start position is analysis delimeter.
// *
// *  WB_LEFT (MOVEUNIT_PREVPROOFWORD)
// *      Finds nearest proof word beginning before start position.
// *
// *  WB_LEFTBREAK (MOVEUNIT_PREVWORDEND)
// *      Finds nearest word end before start position.
// *
// *  WB_MOVEWORDLEFT (MOVEUNIT_PREVWORDBEGIN)
// *      Finds nearest word beginning before start position.
// *      This value is used during CTRL+LEFT key processing.
// *
// *  WB_MOVEWORDRIGHT (MOVEUNIT_NEXTWORDBEGIN)
// *      Finds nearest word beginning after start position.
// *      This value is used during CTRL+RIGHT key processing.
// *
// *  WB_RIGHT (MOVEUNIT_NEXTPROOFWORD)
// *      Finds nearest proof word beginning after start position.
// *
// *  WB_RIGHTEDGE == WB_RIGHTBREAK (MOVEUNIT_NEXTWORDEND)
// *      Finds nearest word end after start position.
// *
// *
// *  NB (t-johnh): WB_MOVEURLLEFT/RIGHT are used for the autodetector to 
// *      determine analysis range of characters that should be checked for being analysis 
// *      URL.  This is no way implies that the given boundary is the boundary
// *      of analysis URL, just that should the tp be positioned in analysis URL, that would 
// *      be the end of it.
// *
// *  WB_MOVEURLLEFT
// *      Finds previous boundary of what could be analysis URL
// *
// *  WB_MOVEURLRIGHT
// *      Finds next boundary of what could be analysis URL
// */

// int CALLBACK gxTextWordBreakProc(TMCStr s, int index, int end, int action)
// {
//     if (!s)
//         return 0;
//     
//     switch (action)
//     {
//     case WB_MOVEWORDLEFT:
//         {
//             int nMask = 0;
//             for (; index > 0 && IsSpaceMask(nMask = GetMask(s[index-1])); --index);
//             if (!IsDelimiterMask(nMask))
//                 return index > 0 ? index - 1 : 0;
//         } // through
//     case WB_LEFT:
//         for (; index > 0 && IsDelimiter(s[index-1]); --index);
//         return index;
//         
//         
//     case WB_RIGHT:
//         for (; index < end && IsDelimiter(s[index]); ++index);
//         return index;
//         
//     case WB_MOVEWORDRIGHT:
//         if (index < end)
//         {
//             int nMask = GetMask(s[index++]);
//             if (IsDelimiterMask(nMask))
//                 for (; index < end && IsDelimiter(s[index]); ++index);
//                 else if (!IsSpaceMask(nMask))
//                     return index;
//                 for (; index < end && IsSpace(s[index]); ++index); // skip space
//         }
//         return index;
//         
//     case WB_ISDELIMITER:
//         return IsDelimiter (s[index]);
//         
//     default:
//         return 0;
//     }
// }

static inline int _IsWSpace(WCHAR ch)
{
    return ch > '\0' && ch <= ' ';
}

int CALLBACK gxTextWordBreakProc(TMCStr s, int index, int end, int action)
{
    if (!s)
        return 0;

    switch (action)
    {
    case WB_ISDELIMITER:
        return _IsWSpace(s[index]);

    case WB_LEFT:
    case WB_MOVEWORDLEFT:
        while (index && _IsWSpace(s[index - 1]))
            --index;
        while (index && !_IsWSpace(s[index - 1]))
            --index;
        return index;

    case WB_RIGHT:
    case WB_MOVEWORDRIGHT:
        while (index < end && !_IsWSpace(s[index]))
            ++index;
        while (index < end && _IsWSpace(s[index]))
            ++index;
        return index;
    }

    return 0;
}

struct _CRichLineList_TxtSize
{
    enum { eCount = 7 };
    static const char cs_aSize[eCount];
    
    static int Height2Size(int nHeight)
    {
        if (nHeight <= cs_aSize[0])
            return 0;
        const char* p = std::lower_bound(&cs_aSize[0], &cs_aSize[eCount], nHeight + 1);
        int n = p - &cs_aSize[0] - 1;
        DbgAssert(n >= 0 && n < eCount);
        return n;
    }
    
    // size: 0 - 6;
    static int Size2Height(int size)
    {
        if (size < 0)
            return cs_aSize[0];
        if (size >= eCount)
            return cs_aSize[eCount - 1];
        return cs_aSize[size];
    }
    
    static int OffsetHeight(int nHeight, int dSize)
    {
        int nSize = Height2Size(nHeight);
        return Size2Height(nSize + dSize);
    }
};
const char _CRichLineList_TxtSize::cs_aSize[eCount] = {8, 10, 12, 14, 18, 24, 36};


static int _CRichLineList_GetHeightUnit(TM::CStrView str, float nDefault, TMBool bFont)
{
    if (str.IsEmpty())
        return nDefault;
    
    const float dpi = TM_DPI_SYSTEM_DEFAULT;
    double x = nDefault;
    TM::CStrView strRet(str.ScanDouble(x));
    switch (strRet.GetLength())
    {
    case 0:
        if (bFont)
            return _CRichLineList_TxtSize::Size2Height((int)(x + 0.5) - 1);
        break;
        
    case 1:
        if (strRet[0] == '%')
            return nDefault * x / 100;
        break;
        
    case 2:
        switch ((strRet[0] << 16) + strRet[1])
        {
        case ('p' << 16) + 'x': break;
        case ('p' << 16) + 't': return x * dpi / 72;
        case ('p' << 16) + 'c': return x * dpi / 6;
        case ('m' << 16) + 'm': return x * dpi / 25.4;
        case ('c' << 16) + 'm': return x * dpi / 2.54;
        case ('i' << 16) + 'n': return x * dpi;
        case ('e' << 16) + 'm': return x * nDefault;
        case ('e' << 16) + 'x': return x * nDefault * 0.52f;
        }
        break;
        
        default:
            break;
    }
    return x;
}



TM_BGN_NAMESPACE

/////////////////////////////////////////////////////////////////////////////
// CRichDocument_Parse
struct CRichDocument_Parse
    : public IDomParser
{
    CHtmlNode m_treeHTML;
    CHtmlNode* m_pCurrent;
    CRichLineList m_doc;
    IRichContext* m_context;
    CGXRich* m_rich;
    CRichLine* m_line;
    int m_nLinePos;
    CStringW m_strTxt;

    friend struct CHtmlNodeBlock_;

private:
    int _AppendText(LPCWSTR pBgn, size_t nSize)
    {
        CAutoRichAnalysis _ama("CRichDocument_Parse::_AppendText");

        int nPos = m_strTxt.GetLength();
        m_strTxt.append(pBgn, nSize);
        int nLen = m_strTxt.GetLength() - nPos;
        if (!m_line)
        {
            m_line = m_doc.InsertLine(nPos);
            m_nLinePos = nLen;
            return 0;
        }
        
        int nOld = m_nLinePos;
        m_nLinePos += nLen;
        return nOld;
    }

    void _EndLine() { m_line = 0; }

public:
    CRichDocument_Parse(CGXRich& r)
        : m_line(0)
        , m_nLinePos(0)
    {
        m_rich = &r;
        m_context = r.m_ptrCreator;
        m_pCurrent = &m_treeHTML;
    }
    
    virtual void AddNode(const CDomStrPtr& tag) tm_override
    {
        DbgAssert(!tag.IsEmpty());
        m_treeHTML.m_strName = tag;
    }

    virtual void EndNode() tm_override
    {
        DbgAssert(m_pCurrent);
        if (m_pCurrent->GetUsage() & eHtmlUsageBlock)
            _EndLine();
        CHtmlNode* p = m_pCurrent->GetParent();
        if (p)
            m_pCurrent = p;
    }
    
    virtual void AddProperty(const CDomStrPtr& name, const CDomStrPtr& val) tm_override
    {
        DbgAssert(m_pCurrent);
        m_treeHTML.m_aProp[name] = val;
    }
    
    virtual void EndPropertys() tm_override;
    
    virtual void AddText(const CDomStrPtr& text) tm_override
    {
        CAutoRichAnalysis _ama("CRichDocument_Parse::AddText");
        DbgAssert(m_pCurrent);
        
        CWeakStr str(text);
//        XmlTextDecode(str, false);
        AddNativeText(str);
    }

    void AddNativeText(const CStrViewW& text)
    {
        if (!text)
            return;

        CAutoRichAnalysis _ama("CRichDocument_Parse::AddNativeText");

        TMCStrW pb = text.data();
        TMCStrW pe = pb + text.GetLength();
        m_strTxt.reserve(m_strTxt.GetLength() + text.GetLength());
        CHtmlNode::CTreeNodeEnd nodeEnd(*m_pCurrent);
        for (TMCStrW pc = pb; ;)
        {
            TMCharW c = pc < pe ? pc[0] : 0;
            if (c != '\t' && c != '\r' && c != '\n' && c != 0)
            {
                ++pc;
                continue;
            }

            if (pc - pb)
            {
                int nPos = _AppendText(pb, pc - pb);
                m_line->InsertTextRunEnd(nPos, pc - pb, *nodeEnd.GetSelf());
            }

            if (c == 0)
                return;
            else if (m_rich->IsFlagSingleLine())
            {
                int nPos = _AppendText(pc, 1);
                m_line->InsertTabRunEnd(nPos, 1, *nodeEnd.GetSelf(), 1);
                pb = ++pc;
            }
            else if (c == '\t')
            {
                int nPos = _AppendText(pc, 1);
                m_line->InsertTabRunEnd(nPos, 1, *nodeEnd.GetSelf(), 4);
                pb = ++pc;
            }
            else
            {
                int n = ((c == '\r' && pc[1] == '\n') || (c == '\n' && pc[1] == '\r')) ? 2 : 1;
                int nPos = _AppendText(pc, n);
                CHtmlNode* pNew = m_line->InsertEOLRun(nPos, n);
                this->_EndLine();
                //m_pCurrent->InsertChildAt(pNew, -1);
                nodeEnd.InsertBefore(pNew);
                pb = pc = pc + n;
            }
        }
    }
};


static TMBool _CHtmlNode_IsGroup(CHtmlNode* p, CHtmlNode* pParent)
{
    if (!p)
        return false;
    if (p->GetParent() != pParent)
        return false;
    return true;
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlNode
CHtmlNode::~CHtmlNode()
{
    CAutoRichAnalysis _ama("CHtmlNode::~CHtmlNode");

    CHtmlNode* pParent = GetParent();
    if (!pParent)
        return;
    CHtmlNode* pNext = GetNext();
    CHtmlNode* pPrev = GetPrev();
    Detach();
    if (_CHtmlNode_IsGroup(pNext, pParent) || _CHtmlNode_IsGroup(pPrev, pParent))
        return;
    
    // this is alone run; remove parent node;
    if (pParent->GetParent()) // skip root item.
        delete pParent;
}

TMBool CHtmlNode::MarkAsStyle(CGXRich& rich, TMUInt32 nToken, TMUInt32& usage, CHtmlNode*& root)
{
    CAutoRichAnalysis _ama("CHtmlNode::MarkAsStyle");

    if (nToken == m_nToken)
    {
        root = 0;
        return true;
    }

    TMUInt32 usg = GetUsage();
    if ((usg | usage) != usage)
    {
        root = this;
        usage |= usg;
        m_nToken = nToken;
        if (usage == eHtmlUsageAll)
            return true;
    }
    else
        usg = 0;

    CHtmlNode* pp = GetParent();
    if (!pp)
        return false;
    if (pp->MarkAsStyle(rich, nToken, usage, root))
    {
        m_nToken = nToken;
        return true;
    }

    return usg;
}


TMBool CHtmlNode::TransformChild(CGXRich& rich, CStringW& str, int nTab, TMUInt32 nToken)
{
    CAutoRichAnalysis _ama("CHtmlNode::TransformChild");
    TMBool b = false;
    CHtmlNode* pc = GetChild();
    for (CHtmlNode* p = pc; p; p = p->GetNext())
        b |= p->Transform(rich, str, nTab + nDefautTab, nToken);

    return b;
}

void CHtmlNode::TransformText(CStringW& str, CWeakStrW text, TMUInt32 nToken)
{
    CAutoRichAnalysis _ama("CHtmlNode::TransformText");
    if (nToken == 0 || nToken == m_nToken)
    {
        XmlTextEncode(text, false);
        str += text;
    }
}

TMBool CHtmlNode::GetProp(const CGXStrView& key, CGXStrPtr& val)
{
    CAutoRichAnalysis _ama("CHtmlNode::GetProp");
    CPropList::recorder rec(m_aProp);
    rec.find(key);
    if (!rec)
        return false;
    val = rec.get_data();
    return true;
}

CGXRich* CHtmlNode::GetRich()
{
    CHtmlNode* p = GetSelf().GetRoot();
    DbgAssert(p!= this);
    return static_cast<CGXRich*>(p);
}

// CHtmlNode
void CHtmlNode::AfterConstruct(CRichDocument_Parse& parser)
{
    CAutoRichAnalysis _ama("CHtmlNode::AfterConstruct");
    CGXStrPtr strCSS;
    if (GetProp(L"style", strCSS))
    {
        CStrPtr strKV, strKey, strVal;
        for (int i = 0; i = strCSS.Split(i, ';', strKV);)
        {
            int j = strKV.Split(0, ':', strKey);
            strVal = strKV.substr(j, -1);
            strKey = strKey.Trim();
            if (strKey == L"background" || strKey == L"background-color")
            {
                strVal = strVal.Trim();
                m_cssBkColor = XmlParseColor(strVal);
            }
        }
    }
}

TMBool CHtmlNode::Transform(CGXRich& rich, CStringW& str, int nTab, TMUInt32 nToken)
{
    CAutoRichAnalysis _ama("CHtmlNode::Transform");

    if (nToken != 0 && nToken != m_nToken)
        return false;

    CString strc;
    TransformChild(rich, strc, nTab, nToken);

    // write tab
    str += "\r\n";
    str.append(nTab, ' ');
    
    // write name
    str += L'<';
    str += m_strName;
    
    // write prop
    //    CHECKMEMORY();
    for (CPropList::recorder rec(m_aProp); rec; ++rec)
    {
        str += L' ';
        str += rec.get_key();
        str += L"=\"";
        str += rec.get_data();
        str += L"\"";
    }
    str += L'>';
    
    // write child
    str += strc;
    
    // write end node
    str += L"</";
    str += m_strName;
    str += L">";
    return true;
}

void CHtmlNode::InitStyle(CGXRich& rich, CGXFont* font, TMColor* clr, TMUInt32& usage)
{
    CAutoRichAnalysis _ama("CHtmlNode::InitStyle");

    DbgAssert(font || clr);
    for (CHtmlNode* p = GetChild(); p; p = p->GetNext())
        p->InitStyle(rich, font, clr, usage);
}

CGXFont* CHtmlNode::GetFont()
{
    DbgAssert(_parent);
    return GetParent()->GetFont();
}

static TMColor _CHtmlNode_ConvertColor(RICHDRAW* rd, TMColor clr)
{
    if (rd)
        return CColorTransformPtr(rd->pct).Convert(rd->lct, clr);
    else
        return clr;
}

TMColor CHtmlNode::GetColor(RICHDRAW* rd)
{
    DbgAssert(_parent);
    return GetParent()->GetColor(rd);
}

TMColor CHtmlNode::GetBkColor(RICHDRAW& rd)
{
    DbgAssert(_parent);
    if (m_cssBkColor)
        return CColorTransformPtr(rd.pct).Convert(rd.lct, m_cssBkColor);
    return GetParent()->GetBkColor(rd);
}

// <mark>
struct CHtmlNodeMark : public CHtmlNode
{
public:
    CHtmlNodeMark() : CHtmlNode(ColorFromSys(eColorWebMark)) {}

    // CHtmlNode
    virtual TMColor GetColor(RICHDRAW* rd) tm_override
    {
        return _CHtmlNode_ConvertColor(rd, ColorFromSys(eColorWebMarkText));
    }
};


/////////////////////////////////////////////////////////////////////////////
// CHtmlNodeFontBaseT
struct _CRichPropFont : public TM::CRefPtr<CGXFont>
{
public:
    void CheckFont(CGXFont*& font, const GXFont& gxFont, TMUInt32& usage)
    {
        CAutoRichAnalysis _ama("CRichPropFont::CheckFont");
        
        DbgAssert(font);
        if (p)
        {
            GXFont ft(p->GetKey());
            TMUInt32 usg = ft.ReplaceFromEx(gxFont);
            if (!usg && p->m_dpi == font->m_dpi)
            {
                font  = 0;
                return;
            }
            usage |= usg;
        }
        else
            usage = eHtmlUsageAll;
        
        Assign(font->CloneFont(gxFont, font->m_dpi));
        font = p;
    }
};

template <class T = CHtmlNode>
struct TM_NO_VTABLE CHtmlNodeFontBaseT : public T
{
    _CRichPropFont m_ptrFont;
    
    // CHtmlNode
    virtual CGXFont* GetFont() tm_override
    {
        CAutoRichAnalysis _ama("CHtmlNodeFontBaseT::GetFont");
        if (m_ptrFont)
            return m_ptrFont;
        return T::GetFont();
    }
};

// <a>
struct CHtmlNodeLink
    : public CHtmlNodeFontBaseT<CHtmlNode>
    , public IRichLink
{
    RICH_ALLOC_OR_CHECK(CHtmlNodeLink);

    CHtmlNodeLink()
    {
    }

    ~CHtmlNodeLink()
    {
        CGXRich* p = GetRich();
        p->m_aLink.erase(this);
    }

    // CHtmlNode
    virtual void AfterConstruct(CRichDocument_Parse& parser) tm_override
    {
        parser.m_rich->m_aLink.insert(this);
        CHtmlNodeFontBaseT<CHtmlNode>::AfterConstruct(parser);
    }

    virtual TMColor GetColor(RICHDRAW* rd) tm_override
    {
        return _CHtmlNode_ConvertColor(rd, ColorFromSys(COLOR_HOTLIGHT));
    }

    virtual IRichLink* GetLink() tm_override
    {
        return this;
    }

    virtual void InitStyle(CGXRich& rich, CGXFont* font, TMColor* clr, TMUInt32& usage) tm_override
    {
        CAutoRichAnalysis _ama("CHtmlNodeLink::InitStyle");
        DbgAssert(font || clr);
        if (font)
        {
            GXFont ft(font->GetKey());
            if (rich.IsFlagUnderLink())
                ft.SetUnderline(true);
            m_ptrFont.CheckFont(font, ft, usage);
        }

        SafeInitStyle(rich, font, clr, usage);
    }

    virtual TMUInt32 GetUsage() tm_override
    {
        return eHtmlUsageLINK;
    }

    // IRichLink
    virtual void GetStringRef(TMStr pBuffer) tm_override
    {
        CAutoRichAnalysis _ama("CHtmlNodeLink::GetStringRef");
        DbgAssert(pBuffer);
        CGXStrPtr strVal;
        if (GetProp(L"href", strVal))
            strVal.CopyTo(pBuffer, MAX_PATH);
        else if (GetProp(L"id", strVal))
            strVal.CopyTo(pBuffer, MAX_PATH);
        else
            pBuffer[0] = 0;
    }
};

// <font>
struct CHtmlNodeFont : public CHtmlNodeFontBaseT<CHtmlNode>
{
    RICH_ALLOC_OR_CHECK(CHtmlNodeFont);

    GXFont m_font;
    TMColor m_color; // 0: use parent color; 

    CHtmlNodeFont()
        : m_color(cColorInvalid)
    {
    }

    // CHtmlNode
    virtual void AfterConstruct(CRichDocument_Parse& parser) tm_override
    {
        CAutoRichAnalysis _ama("CHtmlNodeFont::AfterConstruct");
        CGXStrPtr strVal;
        if (GetProp(L"color", strVal))
            XmlStrToColor(strVal, m_color);
        if (GetProp(L"face", strVal))
            m_font.SetName(strVal.data(), strVal.length());
        if (GetProp(L"size", strVal))
            m_font.height = _CRichLineList_GetHeightUnit(strVal, 16, true);
        CHtmlNodeFontBaseT<CHtmlNode>::AfterConstruct(parser);
    }

    virtual void InitStyle(CGXRich& rich, CGXFont* font, TMColor* clr, TMUInt32& usage) tm_override
    {
        CAutoRichAnalysis _ama("CHtmlNodeFont::InitStyle");
        DbgAssert(font || clr);
        if (font)
        {
            GXFont ft(font->GetKey());
            ft.ReplaceFrom(m_font);
            m_ptrFont.CheckFont(font, ft, usage);
        }

        if (clr)
        {
            if (!m_color && m_color != *clr)
            {
                clr = &m_color;
                usage |= eHtmlUsageColor;
            }
            else
                clr = 0;
        }

        SafeInitStyle(rich, font, clr, usage);
    }

    virtual TMColor GetColor(RICHDRAW* rd) tm_override
    {
        if (m_color)
            return _CHtmlNode_ConvertColor(rd, m_color);
        return CHtmlNode::GetColor(rd);
    }

    virtual TMUInt32 GetUsage() tm_override
    {
        TMUInt32 n = 0;
        if (m_font.name[0])
            n |= eHtmlUsageFace;
        if (m_font.height)
            n |= eHtmlUsageHeight;
        if (m_color)
            n |= eHtmlUsageColor;
        return n;
    }
};

// <em, strong, b, u, strike, del, s, i>
struct CHtmlNodeFontStyle : public CHtmlNodeFontBaseT<CHtmlNode>
{
    RICH_ALLOC_OR_CHECK(CHtmlNodeFontStyle);

    virtual TMUInt32 GetUsage() tm_override
    {
        switch (m_strName[0])
        {
        case 'b': return eHtmlUsageWeight;
        case 'd': return eHtmlUsageStrike; // "del"
        case 'e': return eHtmlUsageItalic; // "em"
        case 'i': return eHtmlUsageItalic;
        case 's': return m_strName[3] == 'o' ? eHtmlUsageWeight : eHtmlUsageStrike; // "strong", "strike"
        case 'u': return eHtmlUsageUnderline;
        }
        DbgAssert(0);
        return 0;
    }

    virtual void InitStyle(CGXRich& rich, CGXFont* font, TMColor* clr, TMUInt32& usage) tm_override
    {
        CAutoRichAnalysis _ama("CHtmlNodeFontStyle::InitStyle");
        DbgAssert(font || clr);
        if (font)
        {
            GXFont ft(font->GetKey());
            TMUInt8 usg = (TMUInt8)GetUsage();
            ft.SetStyle(true, usg);
            m_ptrFont.CheckFont(font, ft, usage);
        }

        SafeInitStyle(rich, font, clr, usage);
    }
};

// <big>
struct CHtmlNodeBig : public CHtmlNodeFontBaseT<CHtmlNode>
{
    RICH_ALLOC_OR_CHECK(CHtmlNodeBig);

    virtual void InitStyle(CGXRich& rich, CGXFont* font, TMColor* clr, TMUInt32& usage) tm_override
    {
        CAutoRichAnalysis _ama("CHtmlNodeBig::InitStyle");
        DbgAssert(font || clr);
        if (font)
        {
            GXFont ft(font->GetKey());
            ft.height = _CRichLineList_TxtSize::OffsetHeight(ft.height, 1);
            m_ptrFont.CheckFont(font, ft, usage);
        }

        SafeInitStyle(rich, font, clr, usage);
    }

    virtual TMUInt32 GetUsage() tm_override { return eHtmlUsageHeight; }
};

// <small>
struct CHtmlNodeSmall : public CHtmlNodeFontBaseT<CHtmlNode>
{
    RICH_ALLOC_OR_CHECK(CHtmlNodeSmall);

    virtual void InitStyle(CGXRich& rich, CGXFont* font, TMColor* clr, TMUInt32& usage) tm_override
    {
        CAutoRichAnalysis _ama("CHtmlNodeSmall::InitStyle");
        DbgAssert(font || clr);
        if (font)
        {
            GXFont ft(font->GetKey());
            ft.height = _CRichLineList_TxtSize::OffsetHeight(ft.height, -1);
            m_ptrFont.CheckFont(font, ft, usage);
        }

        SafeInitStyle(rich, font, clr, usage);
    }

    virtual TMUInt32 GetUsage() tm_override { return eHtmlUsageHeight; }
};

// <p>, <div>
struct CHtmlNodeBlock_ : public CHtmlNode
{
    EHtmlUsage m_nAlign;

    CHtmlNodeBlock_() : m_nAlign(eHtmlUsageNull) {}

    // CHtmlNode
    virtual void AfterConstruct(CRichDocument_Parse& parser) tm_override
    {
        CAutoRichAnalysis _ama("CHtmlNodeBlock_::AfterConstruct");

        CGXStrPtr strVal;
        if (GetProp(L"align", strVal))
        {
            if (strVal == L"right")
                m_nAlign = eHtmlUsageAlignR;
            else if (strVal == L"center")
                m_nAlign = eHtmlUsageAlignC;
            else if (strVal == L"left")
                m_nAlign = eHtmlUsageAlignL;
        }
        CHtmlNode::AfterConstruct(parser);
    }

    virtual TMUInt32 GetUsage() tm_override { return eHtmlUsageBlock | m_nAlign; }
};

struct CHtmlNodeP : public CHtmlNodeBlock_ 
{
    RICH_ALLOC_OR_CHECK(CHtmlNodeP);
};

// <h1> - <h6>
struct CHtmlNodeHN : public CHtmlNodeFontBaseT<CHtmlNodeBlock_>
{
    RICH_ALLOC_OR_CHECK(CHtmlNodeHN);

    // CHtmlNode
    virtual void InitStyle(CGXRich& rich, CGXFont* font, TMColor* clr, TMUInt32& usage) tm_override
    {
        CAutoRichAnalysis _ama("CHtmlNodeHN::InitStyle");
        DbgAssert(font || clr);
        if (font)
        {
            GXFont ft(font->GetKey());
            ft.height = _CRichLineList_TxtSize::Size2Height('6' - m_strName[1]);
            m_ptrFont.CheckFont(font, ft, usage);
        }

        SafeInitStyle(rich, font, clr, usage);
    }

    virtual TMUInt32 GetUsage() tm_override { return eHtmlUsageHeight | eHtmlUsageBlock; }
};

// <li>
struct CHtmlNodeLI : public CHtmlNode
{
    RICH_ALLOC_OR_CHECK(CHtmlNodeLI);

    // CHtmlNode
    virtual TMUInt32 GetUsage() tm_override { return eHtmlUsageBlock; }

    virtual void GetPadding(GXRect& rc) tm_override
    {
        GetParent()->GetPadding(rc);
        rc.left += 20;
    }
};

/////////////////////////////////////////////////////////////////////////////
// CRichRun
CRichRun::CRichRun(int nBgn, int nLen, ERunType eType)
    : nStart(nBgn)
    , nLength(nLen)
    , nLeft(0)
    , nHeight(0)
    , nWidth(0)
    , eType(eType)
    , aInt(0)
    , flag(GXTEXT_NULL)
{
    DbgAssertS(sizeof(TMUInt32) == sizeof(SCRIPT_ANALYSIS));
    DbgAssert(nLen >= 0);
}

CRichRun::~CRichRun()
{
    DbgAssert(!GetChild());
}

void CRichRun::RenderBk(CGXAppBase* app, GXRect& rc, RICHDRAW& rd)
{
    TMColor clr = GetBkColor(rd);
    if (clr)
        app->FillRect(rc, clr);
}

void CRichRun::RenderInvertSel(CGXAppBase* app, CRichLine& line, RICHDRAW& td, GXRect& rc)
{
    if (td.pSel && td.pSel->nSelBegin - line.nStart <= nStart && td.pSel->nSelEnd - line.nStart >= nStart + nLength)
    {
        DbgAssert(td.pSel->nSelBegin < td.pSel->nSelEnd);
        app->InvertRect(rc);
    }
}

CHtmlNode* CRichRun::GetBlockOwner()
{
    for (CHtmlNode* node = this; node; node = node->GetParent())
    {
        if (node->GetUsage() & eHtmlUsageBlock)
            return node;
    }
    return 0;
}


int CRichRun::WrapAndAnalyse(CGXRich& rich, CRichLine& line, GXDips w, TMBool bFirstRun)
{
    TMBool bWrap = (w < nWidth);
    return (!bWrap || bFirstRun) ? nLength : 0;
}


// CHtmlNode
void CRichRun::InitStyle(CGXRich& rich, CGXFont* font, TMColor* clr, TMUInt32& usage)
{
    DbgAssert(font || clr);
    if (font)
        ResetFlagFontUpdate();
    if (clr)
        ResetFlagColorUpdate();
}


/////////////////////////////////////////////////////////////////////////////
// CRichRunTxtBase; base class of CRichRunTxt && CRichRunTab
struct CRichRunTxtBase
    : public CRichRun
{
    struct RAWDRAWTXTPARAM
    {
        CGXRich* rich;
        CRichRunTxtBase* run;
    };

    CTextCache m_cache;

    CRichRunTxtBase(int nBgn, int nLen, ERunType eType)
        : CRichRun(nBgn, nLen, eType)
//         , m_line (line)
    {
        DbgAssert(nLen > 0);
    }

    int* GetDx(CGXRich& rich)
    {
        DbgAssert(m_line);
        return rich.m_aDx.data() + m_line->nStart + nStart;
    }

    void Draw(CGXAppBase* app, CGXRich& rich, CRichLine& line, GXDips _nLeft, GXDips nTop, TMColor* clr, RICHDRAW& rd)
    {
        CAutoRichAnalysis _ama("CRichRunTxtBase::Draw");

        TMColor color = clr ? *clr : GetColor(&rd);
        CGXFont* font = GetFont();
        if (!IsFlagFontUpdate() || !m_cache || !m_cache->CacheTestApp(app))
        {
            SetFlagFontUpdate();
            m_cache.Release();
        }

        SetFlagColorUpdate();

        TMUInt fmt = DT_SINGLELINE | DT_NOPREFIX;
        if (this->IsRunRTL())
            fmt |= DT_RTLREADING;
        GXPoint pt(_nLeft, nTop);
        GXSize sz((float)GX_MAX_LINE_WIDTH, (float)GX_MAX_LINE_WIDTH);
        if (rich.m_chPassWord)
        {
            tm_alloca(TMCharW, c, this->nLength);
            std::fill(c, &c[this->nLength], rich.m_chPassWord);
            font->FontRenderText(app, StrMakeView(c, this->nLength), fmt, color, &pt, &sz, m_cache, 0);
        }
        else
        {
            LPCWSTR c = rich.m_strTxt.data() + line.nStart + this->nStart;
            font->FontRenderText(app, StrMakeView(c, this->nLength), fmt, color, &pt, &sz, m_cache, 0);
//            target->RawDraw(c, run->nLength, rc.Left(), rc.Top(), run->IsRunRTL());
        }
    }

    void RenderTextByColor(CGXAppBase* app, CGXRich& rich, CRichLine& line, TMColor clrNew, GXDips left, GXDips top, RICHDRAW& rd)
    {
        Draw(app, rich, line, left, top, &clrNew, rd);
        ResetFlagColorUpdate();
    }

    virtual void RenderRun(CGXAppBase* app, CGXRich& rich, GXDips left, GXDips bottom, CRichLine& line, RICHDRAW& td) tm_override
    {
        RenderRun(app, rich, left, bottom, line, td, 0);
    }

    void RenderRun(CGXAppBase* app, CGXRich& rich, GXDips left, GXDips bottom, CRichLine& line, RICHDRAW& td, IRichLink* link)
    {
        CAutoRichAnalysis _ama("CRichRunTxtBase::Render");

        TMColor clr = GetBkColor(td);
        if (clr)
        {
            GXRect rc(left + nLeft, bottom - nHeight, left + nWidth + nLeft, bottom);
            RenderBk(app, rc, td);
        }

        int nDrawMode = 0; // 0: only draw normal; 1:draw normal + draw selected; 2: only draw selected;
        GXRect rcx;

        if (td.pSel && !td.bDisable)
        {
            DbgAssert(td.pSel->nSelBegin < td.pSel->nSelEnd);
            int b = td.pSel->nSelBegin - line.nStart - nStart;
            int e = td.pSel->nSelEnd - line.nStart - nStart;
            tm_setmax(b, 0);
            tm_setmin(b, nLength);
            tm_setmax(e, 0);
            tm_setmin(e, nLength);

            if (b < e)
            {
                nDrawMode = (b == 0 && e == nLength) ? 2 : 1;
                // fill background
                rcx.SetLeft(nLeft + left + PosFromChar(rich, b));
                rcx.SetRight(nLeft + left + PosFromChar(rich, e));
                rcx.SetTop(bottom - nHeight);
                rcx.SetBottom(bottom);
                rcx.NormalizeRect(); // right to left text
            }
        }

        if (nDrawMode == 0 || nDrawMode == 1)
        {
            if (td.eVisual & eColorVisualHighContrast)
            {
                int n = td.bDisable ? COLOR_GRAYTEXT : (link ? COLOR_HOTLIGHT : COLOR_WINDOWTEXT);
                TMColor clr = ColorFromSys(n);
                RenderTextByColor(app, rich, line, clr, left + nLeft, bottom - nHeight, td);
            }
            else
                Draw(app, rich, line, left + nLeft, bottom - nHeight, 0, td);
        }

        if (nDrawMode == 1 || nDrawMode == 2)
        {
            DbgAssert(td.pSel);
            if (app->ClipAndPush(rcx, true, eGXPropClip))
            {
                app->FillRect(rcx, td.pSel->clrSelBackground);
                RenderTextByColor(app, rich, line, td.pSel->clrSelText, left + nLeft, bottom - nHeight, td);
                app->Pop();
            }
        }
    }

    virtual void Reclaim() tm_override
    {
        m_cache.Release();
    }

    // CRichRun
    int MakeSize(CGXRich& rich, CGXFont* font, LPCTSTR pStr, int nLen, int nMaxSize, TMBool bFirstRun, TMSize& size) // return char of 
    {
        CAutoRichAnalysis _ama("CRichRunTxtBase::MakeSize");
        DbgAssert(nLen > 0);
        
        int* piDx = GetDx(rich);
        int nCount = 0;
        font->FontGetExtentPoint(StrMakeView(pStr, nLen), nMaxSize, &nCount, piDx, size);
//        DbgTrace("GetTextExtentExPoint:len:%d, maxsize:%d, ncount:%d", nLen, nMaxSize, nCount);
        if (nCount == nLen)
            return nLen; // needn't re-wrap
        
        int nPosSpace = 0;
        if (nCount)
        {
            CAutoRichAnalysis _ama("CRichRunTxtBase::MakeSize.ScriptBreak");
            // need word wrap.
            // search white space.
            tm_alloca(SCRIPT_LOGATTR, logattrs, nCount);
            ScriptBreak(pStr, nCount, &analysis, logattrs);
            
            for (int i = 0; i < nCount; ++i)
            {
                if (/*logattrs[i].fWhiteSpace ||*/ logattrs[i].fSoftBreak)
                    nPosSpace = i;
            }
        }

        if (bFirstRun)
        {
            if (!nPosSpace)
                nPosSpace = nCount ? nCount : 1;
        }
        else if (!nPosSpace)
            return 0;

        CAutoRichAnalysis _ama2("CRichRunTxtBase::MakeSize.GetTextExtentExPoint");
        DbgAssert(nPosSpace);
        font->FontGetExtentPoint(StrMakeView(pStr, nPosSpace), INT_MAX, &nCount, piDx, size);
        DbgAssert(nCount == nPosSpace);
        return nPosSpace;
    }

    virtual int WrapAndAnalyse(CGXRich& rich, CRichLine& line, GXDips w, TMBool bFirstRun) tm_override
    {
        CAutoRichAnalysis _ama("CRichRunTxtBase::WrapAndAnalyse");
        m_cache.Release();

        m_line = &line;
//         CAutoDCFont aft(this);
        CGXFont* font = GetFont();
        CZoom zoom(font->m_dpi);

        CSize sz;
        int n;
        int nLen = nLength;
        DbgAssert(nLength >= 1);
        tm_setmin(nLen, w);
        tm_setmax(nLen, 1);
        if (rich.m_chPassWord)
        {
            tm_alloca(TMCharW, c, nLen);
            std::fill(c, &c[nLen], rich.m_chPassWord);
            n = MakeSize(rich, font, c, nLen, zoom.LPtoDP(w), bFirstRun, sz);
        }
        else
            n = MakeSize(rich, font, rich.m_strTxt.data() + line.nStart + nStart, nLen, zoom.LPtoDP(w), bFirstRun, sz);

        if (n == 0)
            ;// will rewrap, so needn't set nWidth = nHeight = 0;
        else
        {
            GXSize szg;
            zoom.DPtoLP(sz, szg);
            nWidth = szg.Width();
            nHeight = szg.Height();
        }
        return n;
    }

    // CHtmlNode
    virtual TMBool Transform(CGXRich& rich, CStringW& str, int nTab, TMUInt32 nToken) tm_override
    {
        CAutoRichAnalysis _ama("CRichRunTxtBase::Transform");
        TransformText(str, rich.m_strTxt.substr(m_line->nStart + nStart, nLength), nToken);
        return true;
    }
};

/////////////////////////////////////////////////////////////////////////////
// CRichRunTxt
struct CRichRunTxt
    : public CRichRunTxtBase
{
    RICH_ALLOC_OR_CHECK(CRichRunTxt);

    CRichRunTxt(int nBgn, int nLen)
        : CRichRunTxtBase(nBgn, nLen, eRunTypeNormal)
    {
        DbgAssert(nLen > 0);
    }

    ~CRichRunTxt()
    {
    }

    GXDips CPtoX(CGXRich& rich, int nCP)
    {
        CAutoRichAnalysis _ama("CRichRunTxt::CPtoX");
        DbgAssert(m_line);
//         rich_trace("CRichRunTxt::CPtoX: nCP(%d), bTrail(%d)", nCP, bTrail);
        int y;
        if (nCP == 0)
            y = 0;
        else if (nCP > nLength)
            y = GetDx(rich)[nLength - 1];
        else
            y = GetDx(rich)[nCP - 1];
        GXDips x = rich.GetDPI().DPtoLP(y);
        return (!rich.IsFlagR2L() != !IsRunRTL()) ? nWidth - x : x;
    }

    int XtoCP(CGXRich& rich, GXDips X)
    {
        CAutoRichAnalysis _ama("CRichRunTxt::XtoCP");

        if ((!rich.IsFlagR2L() != !IsRunRTL()))
            X = nWidth - X;
        int nX = rich.GetDPI().LPtoDP(X);

        int* b = GetDx(rich);
        int* e = b + nLength;
        int* p = std::lower_bound(b, e, nX);
        if (p == b)
            return 0;
        int nPos = p - b;
        if (p == e)
            return nPos;
        if (nX > ((b[nPos] + b[nPos - 1]) / 2))
            ++nPos;
        DbgAssert(nPos <= nLength);
        return nPos;
    }

    virtual int CharFromPos(CGXRich& rich, GXDips x) tm_override
    {
        return XtoCP(rich, x);
    }

    virtual GXDips PosFromChar(CGXRich& rich, int nIndex) tm_override
    {
        return CPtoX(rich, nIndex);
    }

    TMBool CanMerge(const CRichRun& rhs)
    {
        return rhs.eType == eRunTypeNormal && rhs._parent == _parent;
    }

    virtual TMBool TryMergeRun(CRichRun& runNext) tm_override
    {
        CAutoRichAnalysis _ama("CRichRunTxt::TryMergeRun");
        if (CanMerge(runNext))
        {
            nLength += runNext.nLength;
            return true;
        }
        return false;
    }
};


// \t or \r\n in single line;
struct CRichRunTab
    : public CRichRunTxtBase
{
    RICH_ALLOC_OR_CHECK(CRichRunTab);

    int                 m_nTab;

    CRichRunTab(int nBgn, int nLen, int nTab)
        : CRichRunTxtBase(nBgn, nLen, eRunTypeTab)
        , m_nTab(nTab)
    {
    }
    
    // CRichRun
    virtual int WrapAndAnalyse(CGXRich& rich, CRichLine& line, GXDips w, TMBool bFirstRun) tm_override
    {
        CAutoRichAnalysis _ama("CRichRunTab::WrapAndAnalyse");

        if (rich.m_chPassWord)
            return CRichRunTxtBase::WrapAndAnalyse(rich, line, w, bFirstRun);

        DbgAssert(!rich.m_chPassWord);
        m_line = &line;
        CGXFont* font = GetFont();
        DbgAssert(font);
        CZoom zoom(font->m_dpi);
        
        nWidth = zoom.DPtoLP(m_nTab * font->FontGetMetrics().tmAveCharWidth);
        nHeight = zoom.DPtoLP(font->FontGetMetrics().tmHeight);
        
        return CRichRun::WrapAndAnalyse(rich, line, w, bFirstRun);
    }

    virtual void RenderRun(CGXAppBase* app, CGXRich& rich, GXDips left, GXDips bottom, CRichLine& line, RICHDRAW& td) tm_override
    {
        CAutoRichAnalysis _ama("CRichRunTab::RenderRun");

        if (rich.m_chPassWord)
            CRichRunTxtBase::RenderRun(app, rich, left, bottom, line, td);
        else if (td.pSel && td.pSel->nSelBegin - line.nStart <= nStart && td.pSel->nSelEnd - line.nStart >= nStart + nLength)
        {
            DbgAssert(td.pSel->nSelBegin < td.pSel->nSelEnd);
            GXRect rc(left + nLeft, bottom - nHeight, left + nWidth + nLeft, bottom);
            app->FillRect(rc, td.pSel->clrSelBackground);
        }
    }
    
    // CHtmlNode
//     virtual TMBool Transform(CGXRich& rich, CStringW& str, int nTab, TMUInt32 nToken) tm_override
//     {
//         if (nLength)
//         {
//             TransformText(str, rich.m_strTxt.substr(m_line->nStart + nStart, nLength), nToken);
// //            TransformText(str, L"\t", nToken);
//             return true;
//         }
//         return false;
//     }
};

struct CRichRunEOL : public CRichRun
{
    RICH_ALLOC_OR_CHECK(CRichRunEOL);
    
    CRichRunEOL(int nBgn, int nLen)
        : CRichRun(nBgn, nLen, eRunTypeEOL)
    {
    }

    // CRichRun
    int WrapAndAnalyse(CGXRich& rich, CRichLine& line, GXDips w, TMBool bFirstRun) tm_override
    {
        CAutoRichAnalysis _ama("CRichRunEOL::WrapAndAnalyse");

        CGXFont* font = GetFont();
        DbgAssert(font);
        CZoom zoom(font->m_dpi);

        nWidth = 0;
        nHeight = zoom.DPtoLP(font->FontGetMetrics().tmHeight);

        return nLength;
    }

    // CHtmlNode
    virtual TMBool Transform(CGXRich& rich, CStringW& str, int nTab, TMUInt32 nToken) tm_override
    {
        CAutoRichAnalysis _ama("CRichRunEOL::Transform");
        
        if (nLength)
        {
            TransformText(str, L"\r\n", nToken);
            return true;
        }
        return false;
    }
};

// for block object auto break line.
// struct CRichRunEOLBlock : public CRichRun
// {
//     TM_CLASS_INSTANCE_CHECK(CRichRunEOLBlock);
// //     TM_CLASS_INSTANCE_COUNT(CRichRunEOLBlock);
// 
//     CRichRunEOLBlock(int nBgn, int nLen)
//         : CRichRun(nBgn, nLen, eRunTypeBreak)
//     {
//     }
// 
//     // CRichRun
//     int WrapAndAnalyse(CGXRich& rich, CRichLine& line, GXDips w, TMBool bFirstRun) tm_override
//     {
//         DbgAssert(line.aRun.size() > 1);
//         CRichRun* beforeRun = *(line.aRun.end() - 2);
//         nHeight = beforeRun->nHeight;
//         return nLength;
//     }
// 
//     // CHtmlNode
//     virtual TMBool Transform(CGXRich& rich, CStringW& str, int nTab, TMUInt32 nToken)
//     {
//         return true;
//     }
// };

struct CRichRunImage
    : public CRichRun
{
    RICH_ALLOC_OR_CHECK(CRichRunImage);

    CTexture texture;
    
    CRichRunImage(int nBgn, int nLen)
        : CRichRun(nBgn, nLen, eRunTypeImage)
    {
    }

    // CRichRun
    virtual void RenderRun(CGXAppBase* app, CGXRich& rich, GXDips left, GXDips bottom, CRichLine& line, RICHDRAW& td) tm_override
    {
        CAutoRichAnalysis _ama("CRichRunImage::RenderRun");

        GXRect rc(left + nLeft, bottom - nHeight, left + nWidth + nLeft, bottom);
        RenderBk(app, rc, td);

        ReloadImage();
        if (texture)
            texture->Blt(rc, CRect(0,0,0,0), GX_BLT_DFTSRCWH);
        RenderInvertSel(app, line, td, rc);
    }

    virtual void Reclaim() tm_override
    {
        texture.Release();
        Reset_GXTEXT_LOADED();
    }

    // CHtmlNode
    void ReloadImage()
    {
        if (Is_GXTEXT_LOADED())
            return;

        CGXStrPtr strVal;
        if (!GetProp(L"src", strVal))
            return;

        CTexture tex;
        if (strVal.Left(8).CompareNoCase(CStrViewW(L"file:///", 8)) == 0) //   StrCompareI(strVal.c_str(), L"file:///", 8)
            tex.CreateFromFile(strVal.data() + 8);
        else
            tex.CreateFromFile(strVal.data());
        
        if (tex)
        {
            if (!nWidth)
                nWidth = tex->GetWidth();
            if (!nHeight)
                nHeight = tex->GetHeight();
            tex.Swap(texture);
        }

        Set_GXTEXT_LOADED();
    }

    virtual void AfterConstruct(CRichDocument_Parse& parser) tm_override
    {
        CAutoRichAnalysis _ama("CRichRunImage::AfterConstruct");

        CGXStrPtr strVal;
        if (GetProp(L"width", strVal))
            nWidth = strVal.ToLong();
        if (GetProp(L"height", strVal))
            nHeight = strVal.ToLong();

        ReloadImage();

        CRichRun::AfterConstruct(parser);
    }
};

struct CRichRunCustom
    : public CRichRun
    , public IRichLink
    , public IRichInlinecontainer
{
    RICH_ALLOC_OR_CHECK(CRichRunCustom);

    CRefPtr<IRichInlineObject> m_ptrObj;

    CRichRunCustom(int nBgn, int nLen)
        : CRichRun(nBgn, nLen, eRunTypeCustom)
    {
    }
    
    ~CRichRunCustom()
    {
        if (m_ptrObj)
        {
            m_ptrObj->Destroy();
            m_ptrObj.Release();
        }
    }

    // IRichLink
    virtual void GetStringRef(TMStr pBuffer) tm_override
    {
    }
    
    void SyncSize()
    {
        if (m_ptrObj)
        {
            CAutoRichAnalysis _ama("CRichRunCustom::SyncSize");
            GXSize sz;
            m_ptrObj->GetMetrics(sz);
            nWidth = sz.Width();
            nHeight = sz.Height();
        }
    }


    // IRichInlinecontainer
    virtual TMBool GetAttribute(const CGXStrView& key, CGXStrPtr& val) tm_override
    {
        return GetProp(key, val);
    }

    // CHtmlNode
    virtual void AfterConstruct(CRichDocument_Parse& parser) tm_override
    {
        CAutoRichAnalysis _ama("CRichRunCustom::AfterConstruct");
//         DbgAssert(parser.m_context);
        if (parser.m_context)
        {
            parser.m_context->CreateCustomObject(this, &m_ptrObj);
            SyncSize();
        }

        CRichRun::AfterConstruct(parser);
    }

    // CRichRun
    virtual int WrapAndAnalyse(CGXRich& rich, CRichLine& line, GXDips w, TMBool bFirstRun) tm_override
    {
        CAutoRichAnalysis _ama("CRichRunCustom::WrapAndAnalyse");
        SyncSize();
        return CRichRun::WrapAndAnalyse(rich, line, w, bFirstRun);
    }

    virtual void RenderRun(CGXAppBase* app, CGXRich& rich, GXDips left, GXDips bottom, CRichLine& line, RICHDRAW& td) tm_override
    {
        CAutoRichAnalysis _ama("CRichRunCustom::RenderRun");
        GXRect rc(left + nLeft, bottom - nHeight, left + nWidth + nLeft, bottom);
        RenderBk(app, rc, td);
        if (m_ptrObj)
            m_ptrObj->Render(app, rc, td);
        RenderInvertSel(app, line, td, rc);
    }
};

void CRichDocument_Parse::EndPropertys()
{
    CAutoRichAnalysis _ama("CRichDocument_Parse::EndProperty");
    DbgAssert(m_pCurrent);
    
    CHtmlNode* pNew;
    
    CString& name = m_treeHTML.m_strName;
    if (name == L"a")
        pNew = new CHtmlNodeLink();
    else if (name == L"mark")
        pNew = new CHtmlNodeMark();
    else if (name == L"font")
        pNew = new CHtmlNodeFont();
    else if (name == L"big")
        pNew = new CHtmlNodeBig();
    else if (name == L"small")
        pNew = new CHtmlNodeSmall();
    else if (name == L"em")
        pNew = new CHtmlNodeFontStyle();
    else if (name == L"strong" || name == L"b")
        pNew = new CHtmlNodeFontStyle();
    else if (name == L"u")
        pNew = new CHtmlNodeFontStyle();
    else if (name == L"strike" || name == L"del" || name == L"s")
        pNew = new CHtmlNodeFontStyle();
    else if (name == L"i")
        pNew = new CHtmlNodeFontStyle();
    else if (name == L"p" || name == L"div" || name == L"pre")
    {
        this->_EndLine();
        pNew = new CHtmlNodeP();
    }
    else if (name[0] == 'h' && (name[1] >= '1' && name[1] <= '6') && name.GetLength() == 2)
    {
        this->_EndLine();
        pNew = new CHtmlNodeHN();
    }
//     else if (name == L"li")
//     {
//         this->_EndLine();
//         pNew = new CHtmlNodeLI();
//     }
    else if (name == L"img")
    {
        int nPos = _AppendText(L" ", 1);
        pNew = m_line->InsertImageRun(nPos);
    }
    else if (name == L"br" && !m_rich->IsFlagSingleLine())
    {
        int nPos = _AppendText(L"\r\n", 2);
        pNew = m_line->InsertEOLRun(nPos, 2);
        this->_EndLine();
    }
    else if (name == L"widget")
    {
        int nPos = _AppendText(L" ", 1);
        pNew = m_line->InsertCustomRun(nPos);
    }
    else 
    {
        if (name != L"span")
            DbgTrace(L"CRichLineList::AppendHtmlNode: unkown html tag - %s", name.c_str());
        pNew = new CHtmlNode(); 
    }

    pNew->m_strName.swap(name);
    pNew->m_aProp.swap(m_treeHTML.m_aProp);
    m_pCurrent->InsertChildAt(pNew, -1);
    m_pCurrent = pNew;
    pNew->AfterConstruct(*this);
}


/////////////////////////////////////////////////////////////////////////////
// CRichLine
CRichLine::CRichLine(int nBgn)
    : nStart(nBgn)
    , nLeft(0)
    , nTop(0)
    , nWidth(-1)
    , nHeight(0)
    , logical2visual(1)
{
    logical2visual[0] = 0;
}

CRichLine::~CRichLine()
{
    CRichRunList::recorder::list_clear(aRun, TM::CA_Delete());
}

void CRichLine::RenderLine(CGXAppBase* app, CGXRich& rich, GXDips left, GXDips top, RICHDRAW& td, IRichLink* focus, GXDips nMaxWidth, GXDips nWEllipsis)
{
    CAutoRichAnalysis _ama("CRichLine::Render");

    left += nLeft;
    for (CRichRunList::recorder rec(aRun); rec; ++rec)
    {
        CRichRun* run = rec.get_data(); //aRun[pOrder->nDisplayOrder];
        if ((nWEllipsis < 1) || (this->nWidth <= nMaxWidth + 1) || (run->nLeft + run->nWidth + nWEllipsis <= nMaxWidth))
            run->RenderRun(app, rich, left, top + nHeight, *this, td);
        else
        {
            DbgAssert(rich.m_txtEllipsis);
            int nChar = run->CharFromPos(rich, nMaxWidth - run->nLeft - nWEllipsis);
            GXDips nRight = run->PosFromChar(rich, nChar) + run->nLeft + left;
            GXRect rc(left, top, nRight, top + nHeight);
            if (app->ClipAndPush(rc, true, eGXPropClip))
            {
                run->RenderRun(app, rich, left, top + nHeight, *this, td);
                app->Pop();
            }
            CGXFont* font = rich.GetFont();
            GXDips fh = font->GetDPI().DPtoLP(rich.m_txtEllipsis->GetHeight());
            GXPoint pt(nRight, top + nHeight - fh);
//            GXRect rcx(nRight, top + nHeight - fh, 0, 0);
            //rich.m_txtEllipsis->DrawToDevice(app, rcx, GX_BLT_DFTDSTSRCWH | GX_BLT_TEXTMONO, rich.GetColor(), &rich);
//            font->FontRenderText(app, rich.GetColor(), &rcx, GX_BLT_DFTDSTSRCWH | GX_BLT_TEXTMONO, &rich, CSize(), &rich.m_txtEllipsis.p);
            font->FontRenderText(app, CGXStrView(TM_CL("...")), DT_SINGLELINE, rich.GetColor(&td), &pt, 0, rich.m_txtEllipsis, 0);
            rich.Set_DT_STATE_ELLIPSIS();
            break;
        }
    }
}

static void _check_line_runs(const CRichRunList& aRunc)
{
#ifdef TM_DBG_RICH
    CRichRunList& aRun = const_cast<CRichRunList&>(aRunc);
    DbgAssert(aRun.size());
    int x = 0;
    int i = 0;
    for (CRichRunList::recorder rec(aRun); rec; ++rec, ++i)
    {
        CRichRun* run = rec.get_data();
        DbgAssert(run->nStart == x);
        x += run->nLength;
    }
#endif
}

CRichRunList::iterator CRichLine::InsertRun(CRichRun* run)
{
    CAutoRichAnalysis _ama("CRichLine::InsertRun");
    CRichRunList::pair_ret ret = aRun.insert(run);
    DbgAssert(ret.second);
    run->m_line = this;
//     if (run->IsRunTxt())
//         (static_cast<CRichRunTxt*>(run))->m_line = this;
    return ret.first;
}

int CRichLine::GetTextLength() const
{
    _check_line_runs(aRun);
    const CRichRun* p = *aRun.rbegin();
    return p->IsRunEOL() ? p->nStart : p->nStart + p->nLength;
}

int CRichLine::GetFullLength() const
{
    _check_line_runs(aRun);
    const CRichRun* p = *aRun.rbegin();
    return p->nStart + p->nLength;
}

static void _CRichLine_check_insert(CRichLine* line, int nStart)
{
#ifdef TM_DBG_RICH
    if (nStart == 0)
        DbgAssert(line->aRun.size() == 0);
    else
    {
        DbgAssert(line->aRun.size() != 0);
        DbgAssert(nStart == (*line->aRun.rbegin())->GetEnd());
    }
#endif
}

CRichRun* CRichLine::InsertTextRunEnd(int _nStart, int nLen, CHtmlNode* pEnd)
{
    CAutoRichAnalysis _ama("CRichLine::InsertTextRun");
    _CRichLine_check_insert(this, _nStart);
    DbgAssert(nLen);
    CRichRunTxt* p = new CRichRunTxt(_nStart, nLen);
    CRichRunList::iterator ret = InsertRun(p);
    pEnd->InsertBefore(*ret);
    return p;
}

CRichRun* CRichLine::InsertTabRunEnd(int _nStart, int nLen, CHtmlNode* pEnd, int nTab)
{
    CAutoRichAnalysis _ama("CRichLine::InsertTabRun");
    _CRichLine_check_insert(this, _nStart);
    DbgAssert(nLen);
    CRichRunTab* p = new CRichRunTab(_nStart, nLen, nTab);
    CRichRunList::iterator ret = InsertRun(p);
    pEnd->InsertBefore(*ret);
    return p;
}

CRichRun* CRichLine::InsertImageRun(int _nStart)
{
    CAutoRichAnalysis _ama("CRichLine::InsertImageRun");
    _CRichLine_check_insert(this, _nStart);
    CRichRunImage* p = new CRichRunImage(_nStart, 1);
    InsertRun(p);
    return p;
}

CRichRun* CRichLine::InsertEOLRun(int _nStart, int nLen)
{
    CAutoRichAnalysis _ama("CRichLine::InsertEOLRun");
    _CRichLine_check_insert(this, _nStart);
    CRichRun* p = new CRichRunEOL(_nStart, nLen);
    InsertRun(p);
    return p;
}

// CRichRun* CRichLine::InsertEOLBlockRun(int _nStart, int nLen)
// {
//     CAutoRichAnalysis _ama("CRichLine::InsertEOLBlockRun");
//     _CRichLine_check_insert(this, _nStart);
//     CRichRun* p = new CRichRunEOLBlock(_nStart, nLen);
//     InsertRun(p);
//     return p;
// }


CRichRun* CRichLine::InsertCustomRun(int _nStart)
{
    CAutoRichAnalysis _ama("CRichLine::InsertCustomRun");
    _CRichLine_check_insert(this, _nStart);
    CRichRunCustom* p = new CRichRunCustom(_nStart, 1);
    InsertRun(p);
    return p;
}


// "aab" + 0;                           return "aab";
// "aab" + 2 -> "aa" + "b";             return "b";
// "aab" + end() + 3 -> "aab" + end();  return end();
CRunIter CRichLine::SplitRun(CRunIter iterRun, int nCharOfRun)
{
    CAutoRichAnalysis _ama("CRichLine::SplitRun");
    if (nCharOfRun == 0)
        return iterRun;
    DbgAssert(iterRun < aRun.size());
    DbgAssert(iterRun == aRun.size() ? true : nCharOfRun < GetFullLength());
    CRichRun* run = aRun[iterRun];
    CRichRun* pNew = new CRichRunTxt(run->nStart + nCharOfRun, run->nLength - nCharOfRun);
    run->nLength = nCharOfRun;
    // don't call "CRunIter iter = InsertRun(pNew) - aRun.begin()";
    // because CPU maybe call aRun.begin() first, then call Insert.
    // insert will change vector's begin pointer.
    CRichRunList::iterator iterNew = InsertRun(pNew);
    CRunIter iter = iterNew - aRun.begin();
    run->InsertAfter(pNew);
    DbgAssert(iter == iterRun + 1);
    return iter;
}


/////////////////////////////////////////////////////////////////////////////
// CRichLineList
CRichLine* CRichLineList::InsertLine(int nLineStart)
{
    CAutoRichAnalysis _ama("CRichLine::InsertLine");
    CRichLine* p = new CRichLine(nLineStart);
    CRichLineList::pair_ret ret = this->insert(p);
    if (ret.second)
        return p;
    delete p;
    return 0;
}

void CRichLineList::Init(CHtmlNode& parent)
{
    CAutoRichAnalysis _ama("CRichLine::Init");
    CRichLine* line = InsertLine(0);
    CRichRun* p = line->InsertEOLRun(0, 0);//, fmt, eZOrderDown, &parent);
    parent.InsertChildAt(p, -1);
}

/////////////////////////////////////////////////////////////////////////////
// CGXRich
CGXRich::CGXRich()
    : GXTextBase__(DT_RICH)
    , m_nWidth (1)
    , m_chPassWord (0)
    , m_nToken(0)
    , m_local (-1)
    , m_color(cColorInvalid)
{
    m_aEL.Init(*this);
    SetFlagDirty();

    MemZeroT(m_state);
    MemZeroT(m_control);
    m_control.uDefaultLanguage = LANG_USER_DEFAULT;
}

CGXRich::~CGXRich()
{
    CRichLineList::recorder::list_clear(m_aEL, TM::CA_Delete());
    DbgAssert(!m_aEL.size());
}

void CGXRich::_TraceAllLine(LPCTSTR pInfo)
{
#ifdef TM_DBG_RICH
    DbgTrace(L"start trace all line - %s", pInfo);
    int i = 0;
    for (CRichLineList::recorder rec(m_aEL); rec; ++rec, ++i)
    {
        CRichLine* line = rec.get_data();
        DbgTrace(L"line%d(%x): start:%d; top:%g; width:%g; height:%g, childs:%d",
            i, line, line->nStart, line->nTop, line->nWidth, line->nHeight, line->aRun.size());
        int j = 0;
        for (CRichRunList::recorder rec2(line->aRun); rec2; ++rec2, ++j)
        {
            CRichRun* run = rec2.get_data();
            DbgTrace(L"  run%d(%x): type:%d; start:%d; length: %d, left:%g, width:%g, height:%g, analysis:%d, eScript:%d, bidiLevel:%d, RTL:%d",
                j, run, run->eType, run->nStart, run->nLength, run->nLeft, run->nWidth, run->nHeight, run->aInt, run->analysis.eScript, run->analysis.s.uBidiLevel, run->analysis.fRTL);
        }
    }
    DbgTrace(L"end trace all line - %s\r\n", pInfo);
#endif
}

// line1(aaa + bbb) + 0 -> line1(aaa + bbb)
// line1(aaa + bbb) + 1 -> line1(aaa) + line2(bbb)
// line1(aaa + bbb) + 2 -> line1(aaa + bbb)
CLineIter CGXRich::SplitLineAtRun(CLineIter iterLine, CRunIter iterRun, TMBool bSetRewap)
{
    CAutoRichAnalysis _ama("CGXRich::SplitLineAtRun");

    // check first run
    CRichLine* line = m_aEL[iterLine];
    if (iterRun == 0 || iterRun >= line->aRun.size())
        return iterLine;

    // start
//    DbgAssert(iterRun < line->aRun.size());
    CRichRunList::iterator itRun = line->aRun.begin() + iterRun;
    CRichRun* run = *itRun;
    int nPos = run->nStart;

    // create new line
    CRichLine* p = new CRichLine(line->nStart + nPos);
    if (bSetRewap)
        line->SetRewrap();
    CRichLineList::pair_ret ret = m_aEL.insert(p);
    DbgVerify(ret.second);
    DbgAssert(*(ret.first-1) == line);

    // move all next run to next line
    tm_for (CRichRunList::iterator iter = itRun; iter != line->aRun.end(); ++iter)
    {
        run = *iter;
        run->nStart -= nPos;
        p->InsertRun(run);
    }
    line->aRun.erase(itRun, line->aRun.end());

    DbgAssert(p->IsNeedWrap());

    return iterLine + 1; //ret.first;
}

void CGXRich::MergeParagraph(int nLine, GXDips nMaxWidth)
{
    CAutoRichAnalysis _ama("CGXRich::MergeParagraph");

    CRichLine* line = m_aEL[nLine];
    int nStart = line->nStart;
    
    CRichRun* runEnd = *line->aRun.rbegin();
    if (!runEnd->IsRunEOL())
    {
        // move all runs in paragraph to current line;
        for (; ;)
        {
            CRichLineList::iterator iterLine = m_aEL.begin() + (nLine + 1);
            CRichLine* line2 = *iterLine;
            CRichRun* runBgn = *line2->aRun.begin();
            if (!IsFlagSingleLine() && (runEnd->GetBlockOwner() != runBgn->GetBlockOwner())) // block object, don't merge.
                break;
            int dStart = line2->nStart - nStart;
            for (; line2->aRun.size();)
            {
                CRichRunList::iterator iterRun = line2->aRun.begin();
                runEnd = *iterRun;
                runEnd->nStart += dStart;
                line->aRun.insert(runEnd);
                line2->aRun.erase(iterRun);
            }
            m_aEL.erase(iterLine);
            delete line2;
            if (runEnd->IsRunEOL())
                break;
        }
    }
    int nLength = runEnd->nStart;
    if (nLength == 0)
        return; // empty line.

    // ScriptItemize
    CAutoRichAnalysis _ama2("CGXRich::MergeParagraph.ScriptItemize");
    int nMaxLen = nMaxWidth;
    tm_setmin(nMaxLen, nLength);
    CVector<SCRIPT_ITEM> aItems;
    aItems.resize(32);
    int nItems;
    HRESULT hr;
    for (;;)
    {
        hr = ScriptItemize(m_strTxt.data() + nStart, nMaxLen, aItems.size(), &m_control, &m_state, &aItems[0], &nItems);
        if (hr != E_OUTOFMEMORY)
            break;
        aItems.resize(aItems.size() * 2);
    }
    if (nMaxLen != nLength)
        aItems[nItems].iCharPos = nLength;
    
    DbgAssert(aItems[nItems].iCharPos == nLength);
    DbgAssert(SUCCEEDED(hr));
    if (FAILED(hr))
        return;

#ifdef TM_DBG_RICH
    tm_for (int i = 0; i <= nItems; ++i)
    {
        SCRIPT_ITEM& itm = aItems[i];
        DbgTrace(L"CGXRich::MergeParagraph run%d; charpos:%d, analysis:%d, eScript:%d, bidiLevel:%d, RTL:%d, LayoutRTL:%d",
            i, itm.iCharPos, *(int*)&itm.a, itm.a.eScript, itm.a.s.uBidiLevel, itm.a.fRTL, itm.a.fLayoutRTL);
    }
#endif

    // merge style runs
    {
        CAutoRichAnalysis _ama3("CGXRich::MergeParagraph.MergeStyleRuns");

        int i = 0, j = 0, k = 1;
        tm_for (; j < nItems; ++i)
        {
            SCRIPT_ITEM a0 = aItems[j];
            for (; ++j < nItems;)
            {
                SCRIPT_ANALYSIS a1 = aItems[j].a;
                if (a0.a.fLayoutRTL != a1.fLayoutRTL || a0.a.fRTL != a1.fRTL || a0.a.s.uBidiLevel != a1.s.uBidiLevel)
                    break;
                rich_trace(L"CGXRich::MergeParagraph merget run:%d", j);
                a0.a.eScript = SCRIPT_UNDEFINED;
            }
            aItems[i] = a0;
        }
        aItems[i] = aItems[nItems];
        nItems = i;
    }
    DbgAssert(aItems[nItems].iCharPos == nLength);

    // split run by analysis info.
    CAutoRichAnalysis _ama4("CGXRich::MergeParagraph.SpliteRun");
    int iRun = 0;
    int iItm = 0;
    for (; iItm < nItems;)
    {
        CRichRun* run = line->aRun[iRun];
        SCRIPT_ITEM& itm = aItems[iItm];
//         DbgAssert(*(int*)&itm.a != *(int*)&aItems[iItm + 1].a);
        DbgAssert(run->nStart == itm.iCharPos);
        int nLenRun = run->nLength;
        int nLenItm = aItems[iItm + 1].iCharPos - itm.iCharPos;
        DbgAssert(nLenRun > 0);
        DbgAssert(nLenItm > 0);
        if (nLenRun == nLenItm)
        {
            run->analysis = itm.a;
            ++iItm;
            ++iRun;
        }
        else if (nLenRun > nLenItm)
        {
            run->analysis = itm.a;
            iRun = line->SplitRun(iRun, nLenItm);
            ++iItm;
        }
        else //  nLenRun < nLenItm
        {
            // try merge run first;
            CRichRunList::iterator iter = line->aRun.begin() + (iRun + 1);
            CRichRun* runNext = *iter;
            if (run->TryMergeRun(*runNext))
            {
                delete runNext;
                line->aRun.erase(iter);
            }
            else
            {
                run->analysis = itm.a;
//                 run->analysis.s.uBidiLevel = 0;
                itm.iCharPos += nLenRun;
                ++iRun;
            }
        }
    }
}

GXDips CGXRich::RewrapLine(int nLine, GXDips nTop, GXDips nMaxWidth)
{
    CAutoRichAnalysis _ama("CGXRich::RewrapLine");

    CRichLine& line = *m_aEL[nLine];

    // this is analysis empty line, remove it.
    DbgAssert(line.aRun.size());

    GXRect rcPading(0,0,0,0);
    line.aRun[0]->GetPadding(rcPading);
    nMaxWidth -= (rcPading.Left() + rcPading.Right());

    // check is need wrap.
    line.nTop = nTop;
//     if (nMaxWidth >0)
         line.SetRewrap();
    if (line.IsNeedWrap() || IsFlagDirty())
    {
        // merge next line runs
        MergeParagraph(nLine, nMaxWidth);

        // start wrap
        line.nLeft = rcPading.Left();
        line.nWidth = 0;
        line.nHeight = 0;
        TMBool bFirstRun = true;
        BYTE *BidiLevel = (BYTE*)alloca(line.aRun.size() * sizeof(BYTE));
        tm_for (CRunIter iterRun = 0; iterRun < line.aRun.size(); ++iterRun, bFirstRun = false)
        {
            DbgAssert(nMaxWidth > 0);
            GXDips w = nMaxWidth - line.nWidth;
            if (w <= 0)
            {
                DbgAssert(!bFirstRun);
            }
            else
            {
                CRichRun* pRun = line.aRun[iterRun];
                DbgAssert(pRun);
                BidiLevel[iterRun] = pRun->analysis.s.uBidiLevel;
                pRun->nLeft = line.nWidth;

                int nLen = pRun->WrapAndAnalyse(*this, line, w, bFirstRun);
                TMBool bWrap = (nLen != pRun->nLength);// && !bFirstRun;//  false;
                
                if (nLen)
                {
                    line.nWidth += pRun->nWidth;
                    tm_setmax(line.nHeight, pRun->nHeight);
                    if (bWrap)
                        iterRun = line.SplitRun(iterRun, nLen);
                    else
                        continue;
                }
                else if (!bWrap)
                {
                    DbgAssert(pRun->IsRunEOF());
                    DbgAssert(iterRun == line.aRun.size() - 1);
                    tm_setmax(line.nHeight, pRun->nHeight);
                    break;
                }
                DbgAssert(bWrap);
            }

            // out of line width, re-wrap analysis new line
            SplitLineAtRun(nLine, iterRun, false);
            break;
        }

        // reset logic order, rebuild run position. skip EOL run.
        int nNumItems = line.aRun.size();
        line.logical2visual.resize(nNumItems);
//         line.logical2visual[nNumItems - 1] = nNumItems - 1;
//         if (nNumItems > 1)
        {
//             --nNumItems; // don't layout EOL
            int* V2L = (int*)alloca(sizeof(int) * nNumItems);
            DbgVerifyHResult(ScriptLayout(nNumItems, BidiLevel, V2L, line.logical2visual));
            int nLeft = 0;
            if (IsFlagR2L())
            {
                std::reverse(line.logical2visual.begin(), line.logical2visual.end(nNumItems));
                std::reverse(V2L, V2L + nNumItems);
            }
            tm_for (int i = 0; i < nNumItems; ++i)
            {
                CRichRun* p = line.aRun[V2L[i]];
                p->nLeft = nLeft;
                nLeft += p->nWidth;
            }
        }
    }

    tm_setmax(m_nWidth, line.nWidth);
    return line.nTop + line.nHeight;
}

CLineIter CGXRich::LineFromChar(int nIndex)
{
    CAutoRichAnalysis _ama("CGXRich::LineFromChar");

    DbgAssert(nIndex >= 0);
    _check_size();

    char c[sizeof(CRichLine)];
    CRichLine* paraTmp = (CRichLine*)c;
//    if (!bSelCurLine)
        ++nIndex;
    paraTmp->nStart = nIndex;
    CRichLineList::iterator iter = m_aEL.lower_bound(paraTmp);
    CLineIter n = iter - m_aEL.begin();
    if (n)
        --n;
    _check_line(n);
    return n;
}

void CGXRich::CursorFromChar(CURSOR& cursor, int nIndex)
{
    CAutoRichAnalysis _ama("CGXRich::CursorFromChar");
    DbgAssert(nIndex >= 0);

//     if (nIndex == 7)
//     {
//         int x = 0;
//     }
    // find line
    CLineIter iterLine = LineFromChar(nIndex);
    _check_line(iterLine);
    CRichLine* line = m_aEL[iterLine];
    
    // find run
    nIndex -= line->nStart;
    DbgAssert(nIndex >= 0);
    char c[sizeof(CRichRun)];
    CRichRun* runTmp = (CRichRun*)c;
    runTmp->nStart = nIndex + 1;
    CRichRunList::iterator iterRun = line->aRun.lower_bound(runTmp);
    DbgAssert(iterRun != line->aRun.begin());
    --iterRun;
    runTmp = *iterRun;
    nIndex -= runTmp->nStart;
    
    // return
    cursor.iterLine = iterLine;
    cursor.iterRun = iterRun - line->aRun.begin();
    cursor.nCharOfRun = nIndex;
    _check_cursor(cursor);
}

struct lessRichLeft// : std::binary_function<int, GXDips, bool>
{
    CRichLine* line;
    bool operator()(int _X, GXDips _Y) const
    {
        CRichRun* run1 = line->aRun[_X];
        return (run1->nLeft < _Y);
    };
};

void CGXRich::CursorFromLinePos(CURSOR& cursor, CLineIter& iterLine, GXDips x)
{
    CAutoRichAnalysis _ama("CGXRich::CursorFromLinePos");
    _check_line(iterLine);
    CRichLine* line = m_aEL[iterLine];
    
    // find run
    x -= GetStyleOffset(line);
    lessRichLeft less;
    less.line = line;
    int* b = line->logical2visual.begin();
    int* e = b + line->aRun.size();
    int* pIndex = std::lower_bound(b, e, x + 1, less);
    
    // get position from run
    int nPos = 0;
    if (pIndex != b)
    {
        --pIndex;
        CRichRun* runTmp = line->aRun[*pIndex]; // *iterRun;
        nPos = runTmp->CharFromPos(*this, x - runTmp->nLeft);
    }
    
    // return
    cursor.iterLine = iterLine;
    cursor.iterRun = *pIndex;//iterRun - line->aRun.begin();
    cursor.nCharOfRun = nPos;
    _check_cursor(cursor);

//     DbgTrace("std::lower_bound(%g, %g)", x, line->aRun[*pIndex]->nLeft);
}

void CGXRich::ConvertCursorWrap(CURSOR& cursor, TMBool bAfterWrap)
{
    CAutoRichAnalysis _ama("CGXRich::ConvertCursorWrap");
    if (bAfterWrap)
        return;
    if (cursor.nCharOfRun != 0 || cursor.iterRun != 0 || cursor.iterLine == 0)
        return;
    CLineIter iterLine = cursor.iterLine;
    --iterLine;
    CRichLine* pLine = m_aEL[iterLine];
    CRunIter iterRun = pLine->aRun.size();
    --iterRun;
    CRichRun* run = pLine->aRun[iterRun];
    if (run->IsRunEOL())
        return;
    cursor.iterLine = iterLine;
    cursor.iterRun = iterRun;
    cursor.nCharOfRun = run->nLength;
}

void CGXRich::CursorFromPos(CURSOR& cursor, GXDips x, GXDips y)
{
    CAutoRichAnalysis _ama("CGXRich::CursorFromPos");

    // find line
    CLineIter iterLine = _LineFromPos(y);

    // get cursor
    CursorFromLinePos(cursor, iterLine, x);
}

int CGXRich::SplitLineAtChar(int nChar)
{
    CAutoRichAnalysis _ama("CGXRich::SplitLineAtChar");
    CURSOR cursor;
    CursorFromChar(cursor, nChar);
    cursor.iterRun = m_aEL[cursor.iterLine]->SplitRun(cursor.iterRun, cursor.nCharOfRun);
    CLineIter iterLine = SplitLineAtRun(cursor.iterLine, cursor.iterRun, true);
    return iterLine; //LineIterToLineIndex(iterLine);
}

// void CGXRich::CheckPrevBlock(CURSOR cursor, int& n)
// {
//     if (cursor.nCharOfRun != 0)
//         return;
// 
//     if (cursor.iterRun != 0)
//     {
//         --cursor.iterRun;
//         return true;
//     }
//     if (cursor.iterRun)
// 
// }
// TMBool GetNextCursor(CURSOR& cursor);

int CGXRich::BeginReplaceText(int nBgn, int nEnd, CRichRun** ppStyle, EZOrder& zorder)
{
    CAutoRichAnalysis _ama("CGXRich::BeginReplaceText");
    DbgAssert(nEnd >= nBgn);
    
    // split line at char position
    int nLine1 = SplitLineAtChar(nBgn);
    if (nBgn != nEnd)
    {
        int nLine2 = SplitLineAtChar(nEnd);
        
        // remove all select line
        for (; nLine1 < nLine2; --nLine2)
        {
            CRichLine* line = m_aEL[nLine1];
            delete line;
            m_aEL.erase(m_aEL.begin() + nLine1);
        }
    }
    
    {
        CRichRun* runStyle;
        if (nLine1 != 0)
        {
            zorder = eZOrderDown;
            runStyle = *(m_aEL[nLine1 - 1]->aRun.rbegin());
        }
        else
        {
            zorder = eZOrderUp;
            runStyle = *(m_aEL[nLine1]->aRun.begin());
        }
//         DbgAssert(runStyle->ptrNode);
        *ppStyle = runStyle;
    }
    
    return nLine1;
}

void CGXRich::AfterReplaceText(int nBgn, int nEnd, int nInsert, int nToken)
{
    CAutoRichAnalysis _ama("CGXRich::AfterReplaceText");
    // reset all line nStart param
    int nLength = nInsert;
    int dPos = nLength + nBgn - nEnd;
    tm_for (CRichLineList::iterator iter = m_aEL.begin() + nToken; iter != m_aEL.end(); ++iter)
    {
        CRichLine* line = *iter;
        line->nStart += dPos;
    }
}

void CGXRich::ReplaceBy(int nBgn, int nSize, const CGXStrView& str, EXmlLoadFlag flag)
{
    // don't use self to replace self.
//     DbgAssert(str.size() == 0 || str.begin() > m_strTxt.c_str() + m_strTxt.length() || str.end() < m_strTxt.c_str());

    CAutoRichAnalysis _ama("CGXRich::ReplaceBy");
    DbgAssert(GetLineCount());
    DbgAssert(m_aDx.size() == m_strTxt.size());

    DbgAssert(nBgn >= 0);
    DbgAssert(nBgn <= m_strTxt.GetLength());
    int nEnd =  (nSize < 0) ? m_strTxt.GetLength() : nBgn + nSize;
    CRichRun* pStyle = 0;
    EZOrder zorder;
    int nLine = BeginReplaceText(nBgn, nEnd, &pStyle, zorder);
    _TraceAllLine(L"CGXRich::ReplaceBy - BeginReplaceText");

    CRichDocument_Parse parser(*this);
    if (flag == eXmlLoadFlagText)
        parser.AddNativeText(str);
    else
        DomParser(str, &parser, flag);
    CGXFont* ft = pStyle->GetFont();
    TMColor clr = pStyle->GetColor(0);
    TMUInt32 usg;
    parser.m_treeHTML.InitStyle(*this, ft, &clr, usg);
    if (zorder == eZOrderDown)
        pStyle->InsertAfters(parser.m_treeHTML.GetChild(), 0);
    else
        pStyle->InsertBefores(parser.m_treeHTML.GetChild(), 0);

    int nLength = parser.m_strTxt.GetLength();
    AfterReplaceText(nBgn, nEnd, nLength, nLine);

    m_strTxt.replace(nBgn, nEnd - nBgn, parser.m_strTxt.ToView());
    if (nLength > nEnd - nBgn)
        m_aDx.insert(m_aDx.begin() + nEnd, nLength - (nEnd - nBgn), 0);
    else
        m_aDx.erase(m_aDx.begin() + nBgn, m_aDx.begin() + nEnd - nLength);
    DbgAssert(m_aDx.size() == m_strTxt.size());

    //move node to rich document
    if (nLength)
    {
        for (CRichLineList::recorder rec(parser.m_doc); rec; ++rec)
        {
            CRichLine* line = rec.get_data();
            line->nStart += nBgn;
            m_aEL.insert(line);
        }
        parser.m_doc.clear();
    }

    SetFlagDirtyPart();

    _TraceAllLine(L"CGXRich::ReplaceBy - AfterReplaceText");
}

void CGXRich::TryRewrap(CGXAppBase* app)
{
    CAutoRichAnalysis _ama("CGXRich::TryRewrap");
    DbgAssert(app->IsPainting());

    {
        CGXFont* font = 0;
        TMColor* clr = 0;
        TMUInt32 usage = 0;
        if (m_font != app->m_font)
        {
//            if (m_font && m_font->m_dpi != app->m_font->m_dpi)
            usage = eHtmlUsageRewrap;
            font = app->m_font, m_font = app->m_font;
            m_txtEllipsis.Release();
        }
        if (m_color != app->m_clrText)
        {
            m_color = app->m_clrText, clr = &m_color;
//             if (!m_txtEllipsis.IsClear())
//                 m_txtEllipsis.MakeTextColor(m_color);
        }
        CAutoRichAnalysis _ama("CGXRich::TryRewrap.SafeInitStyle");
        this->SafeInitStyle(*this, font, clr, usage);
        if (usage & eHtmlUsageRewrap)
            SetFlagDirty();
    }

    if (m_local != app->m_local || IsFlagR2L() != IsNeedRTL(app))
    {
        CAutoRichAnalysis _ama("CGXRich::TryRewrap.ScriptRecordDigitSubstitution");
        m_local = app->m_local;
        SettingFlagR2L(IsNeedRTL(app));
        SetFlagDirty();

        // Script Init
        SCRIPT_DIGITSUBSTITUTE sd;
        SCRIPT_CONTROL control = { LANG_USER_DEFAULT };
//        CGXAppBase* app = gxGetAppBase();
        ScriptRecordDigitSubstitution(m_local, &sd);
        ScriptApplyDigitSubstitution(&sd, &control, &m_state);
        bool bR2L = IsFlagR2L();
        m_state.uBidiLevel = bR2L;
        if (!bR2L)
            m_state.fDigitSubstitute = false;
    }
    
    if (!IsFlagDirty() && !IsFlagDirtyPart())
        return;

    CAutoRichAnalysis _ama2("CGXRich::TryRewrap.RewrapLine");
    m_nWidth = 1;
    GXDips nNextTop = 0;
    GXDips nMaxWidth = GetMaxWidth();
    DbgAssert(nMaxWidth >= 0);
    tm_for (int i = 0; i < m_aEL.size(); ++i)
        nNextTop = RewrapLine(i, nNextTop, nMaxWidth);
//     tm_setmax(m_nWidth, nMaxWidth);
    _TraceAllLine(L"CGXRich::Rewrap");
    ResetFlagDirty();
    ResetFlagDirtyPart();
}

GXDips CGXRich::GetStyleOffset(CRichLine* line)
{
    CRichRun* pRun = *line->aRun.begin();
    DbgAssert(pRun);
    CHtmlNode* pNode = pRun->GetBlockOwner();
    TMUInt flag = m_flag;
    if (pNode)
    {
        TMUInt32 usage = pNode->GetUsage();
        if (usage & eHtmlUsageAlignL)
            flag = DT_LEFT;
        else if (usage & eHtmlUsageAlignR)
            flag = DT_RIGHT;
        else if (usage & eHtmlUsageAlignC)
            flag = DT_CENTER;
    }

    if ((flag & (DT_WORD_ELLIPSIS | DT_END_ELLIPSIS)) && (line->nWidth > m_szMax.Width()))
        return 0;
//     if (flag & DT_RIGHT)
//         return ((IsWordEllipsis() || IsAutoWrapRich()) ? m_szMax.Width() : m_nWidth) - line->nWidth;
//     else if (flag & DT_CENTER)
//         return (((IsWordEllipsis() || IsAutoWrapRich()) ? m_szMax.Width() : m_nWidth) - line->nWidth) / 2;
//     else
//         return 0;

    if (flag & DT_RIGHT)
        return m_szMax.Width() - line->nWidth;
    else if (flag & DT_CENTER)
        return (m_szMax.Width() - line->nWidth) / (GXDips)2;
    else
        return 0;
}

// static void _CGXRich__RawDrawEllipsis(CGXAppBase* app, const CRect& rc, void* param)
// {
//     CTextCache* pEngine = (CTextCache*)param;
//     pEngine->RawDraw(L"...", 3, rc, DT_SINGLELINE);
// }

GXDips CGXRich::GetEllipsisWidth(CGXAppBase* app, RICHDRAW& td)
{
    if (!m_txtEllipsis)
    {
//        CSizeI szReal(0, 0);
//        m_font->FontCalcRect(TM_T("..."), 3, szReal, DT_SINGLELINE);
//        m_font->CreateTextBuffer(app, app->m_clrText, szReal, this, &m_txtEllipsis);
//        m_font->FontRenderText(app, app->m_clrText, 0, GX_BLT_NONE, this, szReal, &m_txtEllipsis);
        GXSize sz((GXDips)GX_MAX_LINE_WIDTH, (GXDips)GX_MAX_LINE_WIDTH);
        m_font->FontRenderText(app, CGXStrView(TM_CL("...")), DT_SINGLELINE, GetColor(&td), 0, &sz, m_txtEllipsis, 0);
//        return m_font->GetDPI().DPtoLP(szReal.Width());
    }
    return m_font->GetDPI().DPtoLP(m_txtEllipsis->GetWidth());
}

int CGXRich::LineIndex(int nLine)
{
    if (nLine >= m_aEL.size() || nLine < 0)
        return -1;
    
    return m_aEL[nLine]->nStart;
}

int CGXRich::LineLength(int nLine)
{
    if (nLine < 0 || nLine >= m_aEL.size())
        return 0;
    
    CRichLine& el = *m_aEL[nLine];
    return el.GetTextLength();
}

GXPoint CGXRich::PosFromCursor(CURSOR& cursor)
{
    CAutoRichAnalysis _ama("CGXRich::PosFromCursor");
    _check_cursor(cursor);
    CRichLine* line = m_aEL[cursor.iterLine];
    CRichRun* run;
    int nCharOfRun;
    if (cursor.nCharOfRun == 0 && cursor.iterRun > 0)
    {
        run = line->aRun[cursor.iterRun - 1];
        nCharOfRun = run->nLength;
    }
    else
    {
        run = line->aRun[cursor.iterRun];
        nCharOfRun = cursor.nCharOfRun;
    }
    GXDips nLeft = run->nLeft;
    nLeft += run->PosFromChar(*this, nCharOfRun);
    nLeft += GetStyleOffset(line);
    return GXPoint(nLeft, line->nTop);
}

CLineIter CGXRich::_LineFromPos(GXDips y)
{
    CAutoRichAnalysis _ama("CGXRich::_LineFromPos");
    char c[sizeof(CRichLine)];
    CRichLine* paraTmp = (CRichLine*)c;
    paraTmp->nTop =  y + 1;

    CLinePositionList& aLine = *(CLinePositionList*)&m_aEL;
    CLineIter iter = aLine.lower_bound(paraTmp) - m_aEL.begin();
    if (iter != 0)
        --iter;
    return iter;
}

GXDips CGXRich::GetHeight() const
{
    DbgAssert(m_aEL.size() > 0);
    CRichLine* line = *m_aEL.rbegin();
    return line->nTop + line->nHeight;
}

void CGXRich::_Paint(CGXAppBase* gapp, const GXPoint& ptOrg, RICHDRAW& td)
{
    CAutoRichAnalysis _ama("CGXRich::_Paint");
//     DbgAssert(!(m_fmt & DT_CALCRECT));
    
    GXRect rcClip;
    gapp->GetClip(rcClip);
 
    if (td.pSel)
    {
        if (td.pSel->nSelBegin > td.pSel->nSelEnd)
            tm_swap(td.pSel->nSelBegin, td.pSel->nSelEnd);
        else if (td.pSel->nSelBegin == td.pSel->nSelEnd)
            td.pSel = 0;
    }

    IRichLink* pFocus = (td.nCurrentTab < m_aLink.size() && td.nCurrentTab >= 0) ? m_aLink[td.nCurrentTab] : 0;
    GXDips nMaxWidth = GX_MAX_LINE_WIDTH;// (m_flag & DT_WORDBREAK) ? m_szMax.Width() : m_nWidth;// (m_flag & (DT_WORD_ELLIPSIS | DT_END_ELLIPSIS)) ? GetEllipsisWidth() : 0;
    GXDips nWEllipsis = 0;
    if (m_flag & (DT_WORD_ELLIPSIS | DT_END_ELLIPSIS))
    {
        nWEllipsis = GetEllipsisWidth(gapp, td);
        nMaxWidth = td.nWidth > 0.5 ? td.nWidth : m_szMax.Width();
        DbgAssert(m_txtEllipsis);
    }

    for (CRichLineList::recorder rec(m_aEL); rec; ++rec)
    {
        CRichLineList::iterator iter = rec.get_iter();
        CRichLine* line = *iter;
        if (ptOrg.Y() + line->nTop + line->nHeight < rcClip.Top())
            continue;
        if (ptOrg.Y() + line->nTop > rcClip.Bottom())
            break;
        GXPoint pt(ptOrg.X() + GetStyleOffset(line), ptOrg.Y() + line->nTop);
        line->RenderLine(gapp, *this, pt.X(), pt.Y(), td, pFocus, nMaxWidth, nWEllipsis);
    }
}

///////////////////////////////////////////////////////////////////////////////
// IGXText
void CGXRich::SetPasswordChar(wchar_t chPassword)
{
    if (m_chPassWord == chPassword)
        return;
    m_chPassWord = chPassword;
    SetFlagDirty();
}

void CGXRich::SetFormat(TMUInt32 uFormat)
{
    enum { fMask = DT_SINGLELINE | DT_RIGHT | DT_CENTER | DT_WORDBREAK | DT_WORD_ELLIPSIS | DT_L2RREADING | DT_NOLINKUNDERLINE };
    if ((m_flag & fMask) != (uFormat & fMask))
    {
        SetFlagDirty();
        m_flag &= ~fMask;
        m_flag |= (uFormat & fMask);
    }
}

int CGXRich::GetLineRange(int nLine, TMSel0& nStart)
{
    nStart = LineIndex(nLine);
    return LineLength(nLine);
}

TMBool CGXRich::GetSize(GXSize& sz)
{
    if (IsFlagDirty())
        return false;
    sz.SetSize(GetWidth(), GetHeight());
    return true;
}

void CGXRich::SetMaxSize(const GXSize& sz)
{
    TMBool bDirty = TrySetMaxSize_Rich(this, sz);
    if (bDirty)
        SetFlagDirty();
}

void CGXRich::Reclaim()
{
    for (CRichLineList::recorder rec(m_aEL); rec; ++rec)
    {
        CRichLine* line = rec.get_data();
        for (CRichRunList::recorder recRun(line->aRun); recRun; ++recRun)
        {
            CRichRun* run = recRun.get_data();
            run->Reclaim();
        }
    }
    m_txtEllipsis.Release();
}

CGXStrPtr CGXRich::GetHtml(TMSel0 nBgn, int nSize)
{
    CAutoRichAnalysis _ama("CGXRich::GetHtml");

    DbgAssert(nBgn >= 0);
    DbgAssert(nBgn <= m_strTxt.GetLength());

    if (nSize < 0)
        nSize = m_strTxt.GetLength() - nBgn;
    if (nSize <= 0)
        return CGXStrPtr();

    CURSOR cBgn, cEnd;
    CursorFromChar(cBgn, nBgn);
    CursorFromChar(cEnd, nBgn + nSize);
    CRichRun* run1 = m_aEL[cBgn.iterLine]->aRun[cBgn.iterRun];
    CRichRun* run2 = m_aEL[cEnd.iterLine]->aRun[cEnd.iterRun];

    // make token
    ++m_nToken;
    if (m_nToken == 0)
        ++m_nToken;

    typedef CVector<CHtmlNode*> CNodeList;
    CNodeList aNode;
    for (CTreeItemT<CHtmlNode*> node(run1); ; node.GlobalNext())
    {
        CHtmlNode* pRoot = node;
        TMUInt32 usage = 0;
        pRoot->MarkAsStyle(*this, m_nToken, usage, pRoot);
        if (pRoot)
        {
            pRoot->Mark(m_nToken);
            aNode.push_back(pRoot);
        }
        if (node == run2)
            break;
    }

    CString str;
    int n1 = run1->nStart;
    int n2 = run1->nLength;
    int n3 = run2->nLength;
    run1->nStart += cBgn.nCharOfRun;
    if (run1 == run2)
        run1->nLength = cEnd.nCharOfRun - cBgn.nCharOfRun;
    else
    {
        run1->nLength -= cBgn.nCharOfRun;
        run2->nLength = cEnd.nCharOfRun;
    }
    for (CNodeList::recorder rec(aNode); rec; ++rec)
    {
        CHtmlNode* p = rec.get_data();
        p->Transform(*this, str, 0, m_nToken);
    }
    run1->nStart = n1;
    run1->nLength = n2;
    run2->nLength = n3;

    return std::move(str);
}

void CGXRich::_Cursor2Info(CGXRich::CURSOR& cursor, RICHCURSOR& info)
{
    CAutoRichAnalysis _ama("CGXRich::_Cursor2Info");

    CRichLine* l = m_aEL[cursor.iterLine];
    
    if (info.nFlag & RCFLAG_LINEINFO)
    {
        info.nLineIndex = cursor.iterLine;//this->LineIterToLineIndex(cursor.iterLine);
        info.nLineCharStart = l->nStart;
        info.nLineCharLength = l->GetTextLength();
        info.nLineWidth = l->nWidth; // GetStyleOffset(l);
        info.nLineTop = l->nTop;
        info.nLineHeight = l->nHeight;
    }

    if (info.nFlag & RCFLAG_LINELEFT)
        info.nLineLeft = GetStyleOffset(l);

    if (info.nFlag & RCFLAG_RUNINFO)
    {
        CRichRun* r = l->aRun[cursor.iterRun];
        info.nRunIndex = cursor.iterRun;
        info.nCharOfRun = cursor.nCharOfRun;
        info.nRunCharStart = r->nStart;
        info.nRunCharLength = r->nLength;
        info.pLink = r->GetLink();
        if (r->eType == eRunTypeCustom)
            info.pObj = (static_cast<CRichRunCustom*>(r))->m_ptrObj;
        else
            info.pObj = 0;
        info.nRunLeft = r->nLeft;
        info.nRunWidth = r->nWidth;
        info.nRunHeight = r->nHeight;
    }

    if (info.ptLeftTop)
    {
        if (IsFlagDirty() || IsFlagDirtyPart())
            info.ptLeftTop = 0; //->SetPoint(0, 0);
        else
            *info.ptLeftTop = PosFromCursor(cursor);
    }
}

void CGXRich::GetInfoFromChar(TMSel0 nIndex, RICHCURSOR& info, TMBool bAfterWrap)
{
    CAutoRichAnalysis _ama("CGXRich::GetInfoFromChar");
    DbgAssert(nIndex >= 0);
    DbgAssert(nIndex <= m_strTxt.GetLength());
    CGXRich::CURSOR cursor;
    CursorFromChar(cursor, nIndex);
    ConvertCursorWrap(cursor, bAfterWrap);
    _Cursor2Info(cursor, info);
}

TMBool CGXRich::GetInfoFromPos(GXDips x, GXDips y, RICHCURSOR& info)
{
    CAutoRichAnalysis _ama("CGXRich::GetInfoFromPos");
    if (IsFlagDirty() || IsFlagDirtyPart())
        return false;

    if ((info.nFlag & RCFLAG_RUNINFO) || info.ptLeftTop)
    {
        CURSOR cursor;
        CursorFromPos(cursor, x, y);
        _Cursor2Info(cursor, info);
    }
    else
    {
        CURSOR cursor = { _LineFromPos(y), 0, 0 };
        _Cursor2Info(cursor, info);
    }
    return true;
}

void CGXRich::Paint(const GXPoint& ptOrg, RICHDRAW* td, IGXApp* gapp)
{
    CAutoRichAnalysis _ama("CGXRich::Paint");
    CGXAppBase* app = static_cast<CGXAppBase*>(gapp);
    TryRewrap(app);
    RICHDRAW rd = {0};
    if (!td)
        td = &rd;
    _Paint(app, ptOrg, *td);
}

void CGXRich::CalcSize(GXSize& sz, IGXApp* gapp)
{
    CAutoRichAnalysis _ama("CGXRich::CalcSize");
    SetMaxSize(sz);
    TryRewrap(static_cast<CGXAppBase*>(gapp));
    MakeEllipsisSize(false, sz, GXSize(GetWidth(), GetHeight()));
}

void CGXRich::SetString(const CGXStrPtr& str)
{
    CAutoRichAnalysis _ama("CGXRich::SetString");
    ReplaceHtml(0, m_strTxt.GetLength(), str);
}

void CGXRich::ReplaceHtml(TMSel0 nBgn, int nSize, const CGXStrView& str)
{
    CAutoRichAnalysis _ama("CGXRich::ReplaceHtml");
    ReplaceBy(nBgn, nSize, str, eXmlLoadFlagHTML);
}

void CGXRich::ReplaceTxt(TMSel0 nBgn, int nSize, const CGXStrView& str)
{
    CAutoRichAnalysis _ama("CGXRich::ReplaceTxt");
    ReplaceBy(nBgn, nSize, str, eXmlLoadFlagText);
}

int CGXRich::GetLinks(int n, wchar_t* p)
{
    int ns = m_aLink.size();
    if (p && n < ns && n >= 0)
        m_aLink[n]->GetStringRef(p);
    return ns;
}

TM_END_NAMESPACE

void gxTextCreate(IGXText** ppText, TMBool bRich)
{
    if (bRich)
        *ppText = new TM::CRefObjectT<TM::CGXRich>(1);
    else
        *ppText = new TM::CRefObjectT<TM::GXText__>(1);
}

void _GetRichRunInfo(GXPText text, TM::CString& str)
{
    if (text->GetFormat() & DT_RICH)
    {
        TM::CGXRich* pr = static_cast<TM::CGXRich*>(text);
        pr->_GetRunsString(str);
    }
}