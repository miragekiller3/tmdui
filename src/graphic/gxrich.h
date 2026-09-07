/**
 *  @file
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2016.10.17
 *
 *  Copyright (C) 2016 miragekiller                               
 */

// rich edit struct:
// sample1: "aa\r\n\r\nbb"
//   line1  "aa" + EOL("\r\n")
//   line2  EOL("\r\n")
//   line3  "bb" + EOL("")
// sample2: wraped "aaaaaa"
//   line1  "aaaa"
//   line2  "aa" + EOL("")
// sample3: "\r\n"
//   line1  EOL("\r\n")
//   line2  EOL("")
// test html: https://www.emirates.com/dz/arabic/

#pragma once

#ifndef GXRICH_H
#define GXRICH_H

#include "gxtext.h"
#include "../tmdom.h"
#include "../tmtree.h"
#include "../tmlist.h"
#include "../tmalloc.h"


//#define TM_DBG_RICH  

#ifdef TM_DBG_RICH
#   define rich_trace DbgTrace
#else
#   define rich_trace DbgTraceNull
#endif 

#define CAutoRichAnalysis CAutoAnalysisNull
//#define CAutoRichAnalysis CAutoAnalysisTuiUser


#define TUI_CLASS_RICHNODE      L"#richitem"
#define RICH_ALLOC_OR_CHECK(x)  TM_SELF_ALLOC_F(x) 


TM_BGN_NAMESPACE


struct CGXRich;
struct CRichLine;



enum EHtmlUsage
{
    eHtmlUsageWeight    = GX_FONT_WEIGHT, 
    eHtmlUsageItalic    = GX_FONT_ITALIC, 
    eHtmlUsageUnderline = GX_FONT_UNDERLINE, 
    eHtmlUsageStrike    = GX_FONT_STRIKE, 
    eHtmlUsageFixed     = GX_FONT_FIXED, 
    eHtmlUsageFace      = GX_FONT_FACE,
    eHtmlUsageHeight    = GX_FONT_HEIGHT,
    eHtmlUsageColor     = 0x0400,
    eHtmlUsageLINK      = 0x0800,
    eHtmlUsageBlock     = 0x1000,
    eHtmlUsageAlignL    = 0x2000,
    eHtmlUsageAlignR    = 0x4000,
    eHtmlUsageAlignC    = 0x8000,
    EHtmlUsageAlignMask = eHtmlUsageAlignL | eHtmlUsageAlignC | eHtmlUsageAlignR,

    eHtmlUsageNull      = 0,
    eHtmlUsageAll       = eHtmlUsageWeight | eHtmlUsageItalic | eHtmlUsageUnderline | eHtmlUsageStrike |
                          eHtmlUsageFixed | eHtmlUsageFace | eHtmlUsageHeight | eHtmlUsageColor | eHtmlUsageLINK |
                          eHtmlUsageBlock | eHtmlUsageAlignL | eHtmlUsageAlignC | eHtmlUsageAlignR,
    eHtmlUsageRewrap    = eHtmlUsageWeight | eHtmlUsageItalic | eHtmlUsageFixed | eHtmlUsageFace | eHtmlUsageHeight,
};



struct CHtmlNode
    : public CDomNodeHelperT<CHtmlNode, IDomNode>
    , public CTreeNodeT<CHtmlNode*>
{
public:
    TM_SELF_ALLOC_F(CHtmlNode);
#ifdef TM_DBG
    int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf)
    {
        CHtmlNode* p = (CHtmlNode*)cls->m_pClass;
        return IC_Size(p->m_aProp);
    }
#endif // TM_DBG
    
protected:
    enum { nDefautTab = 2 };

    typedef CMap<CString, CString> CPropList;
    CString     m_strName;
    CPropList   m_aProp;
    TMUInt32    m_nToken;
    TMColor     m_cssBkColor;
    friend struct CRichDocument_Parse;

protected:
    TMBool TransformChild(CGXRich& rich, CStringW& str, int nTab, TMUInt32 nToken);
    void TransformText(CStringW& str, CWeakStrW text, TMUInt32 nToken);
    void SafeInitStyle(CGXRich& rich, CGXFont* font, TMColor* clr, TMUInt32& usage) { if (font || clr) CHtmlNode::InitStyle(rich, font, clr, usage); }
    TMBool GetProp(const CGXStrView& key, CGXStrPtr& val);
    CGXRich* GetRich();

public:
    TM_UNUSED_ADDREF_RELEASE();
    TM_BGN_INTERFACE_MAP(CHtmlNode)
    TM_END_INTERFACE_MAP()

public:
    CHtmlNode(TMColor cssBkColor = cColorInvalid) : m_nToken(0), m_cssBkColor(cssBkColor) {}
    void Mark(int n) { m_nToken = n; }
    TMBool MarkAsStyle(CGXRich& rich, TMUInt32 nToken, TMUInt32& usage, CHtmlNode*& root);

    // self
    virtual ~CHtmlNode();
    virtual void AfterConstruct(CRichDocument_Parse& parser);
    virtual TMBool Transform(CGXRich& rich, CStringW& str, int nTab, TMUInt32 nToken);
    virtual void InitStyle(CGXRich& rich, CGXFont* font, TMColor* clr, TMUInt32& usage);
    virtual CGXFont* GetFont();
    virtual TMColor GetColor(RICHDRAW* rd);
    virtual TMColor GetBkColor(RICHDRAW& rd);
    virtual IRichLink* GetLink() { return GetParent()->GetLink(); }
    virtual TMUInt32 GetUsage() { return 0; } //  return: mask of eHtmlUsage
    virtual void GetPadding(GXRect& rc) { GetParent()->GetPadding(rc); }

    // IDomNode
    virtual CDomStrPtr get_nodeName() tm_override { return TM_CS(TUI_CLASS_RICHNODE); }
    virtual DOMNodeType get_nodeType() tm_override { return NODE_ELEMENT; }
    virtual CDomStrPtr get_nodeValue() tm_override { return TM_CS(TUI_CLASS_RICHNODE); }
    virtual void put_nodeValue(const CDomStrPtr& val) tm_override {}
    virtual void removeChild(IDomNode* node) tm_override {}
};

template<class T>
struct lessRich// : std::binary_function<T, T, bool>
{
    bool operator()(const T& _X, const T& _Y) const
    {
        return (_X->nStart < _Y->nStart);
    };
};

template<class T>
struct lessRichTop// : std::binary_function<T, T, bool>
{
    bool operator()(const T& _X, const T& _Y) const
    {
        return (_X->nTop < _Y->nTop);
    };
};

struct CRichLine;
struct CRichRun;
typedef TM::CFlatSet<CRichRun*, lessRich<CRichRun*> > CRichRunList;
typedef CRichRun GXEdit__;
typedef int CRunIter;
typedef int CLineIter;

TM_DECLARE_UUID(CGXRich,   2EADB1D1, 2756, 4f41, AB, E1, 14, 12, 19, 69, 8A, 80);

enum ERunType
{
    eRunTypeNormal  = 0,
    eRunTypeEOL, // length == 0: wraped line;  != 0: "\r\n";
//     eRunTypeBreak   = 2, // for block object, append first and last EOL.
    eRunTypeImage,
    eRunTypeCustom,
    eRunTypeTab, // tab key
};

enum ERunFlag
{
    GXTEXT_NULL             = 0,
    GXTEXT_COLOR_UPDATE     = 0x40000000,
    GXTEXT_FONT_UPDAET      = 0x80000000,
    GXTEXT_LOADED           = 0x01000000,
};
TM_ENUM_2_FLAG(ERunFlag);

struct CRichRun
    : public CHtmlNode
{
    void* operator new(size_t);

    ERunFlag            flag;
    ERunType            eType;
    int                 nStart;     // start character, relative to line
    int                 nLength;    // length of character 
    GXDips              nLeft;
    GXDips              nHeight;
    GXDips              nWidth;
    union
    {
        SCRIPT_ANALYSIS analysis;
        TMUInt32        aInt;
    };
    CRichLine*          m_line;

    TM_DECLARE_FLAG     (FlagColorUpdate,   flag, GXTEXT_COLOR_UPDATE);
    TM_DECLARE_FLAG     (FlagFontUpdate,    flag, GXTEXT_FONT_UPDAET);
    TM_DECLARE_FLAG     (_GXTEXT_LOADED,    flag, GXTEXT_LOADED);


    CRichRun(int nBgn, int nLen, ERunType eType);
    virtual ~CRichRun() tm_override;

    TMBool IsRunRTL() { return analysis.fRTL; }
    TMBool IsRunEOL() const { return eType == eRunTypeEOL; } // || eType == eRunTypeBreak; }
    TMBool IsRunEOF() const { return eType == eRunTypeEOL || nLength == 0; }
    int GetEnd() const { return nStart + nLength; }
    void RenderBk(CGXAppBase* app, GXRect& rc, RICHDRAW& rd);
    void RenderInvertSel(CGXAppBase* app, CRichLine& line, RICHDRAW& td, GXRect& rc); // for image and widget
    CHtmlNode* GetBlockOwner();

    // CRichRun
    virtual int WrapAndAnalyse(CGXRich& rich, CRichLine& line, GXDips w, TMBool bFirstRun);
    virtual int CharFromPos(CGXRich& rich, GXDips nPos) { return 0; } // return relative char to run
    virtual GXDips PosFromChar(CGXRich& rich, int nChar) { return nChar ? nWidth : 0; } // return relative pos to run
    virtual void RenderRun(CGXAppBase* app, CGXRich& rich, GXDips left, GXDips bottom, CRichLine& line, RICHDRAW& td) {}
    virtual TMBool TryMergeRun(CRichRun& runNext) { return false; }
    virtual void Reclaim() {}

    // CHtmlNode
    virtual void InitStyle(CGXRich& rich, CGXFont* font, TMColor* clr, TMUInt32& usage) tm_override;
};


struct CRichLine
{
    TM_SELF_ALLOC_F(CRichLine);
#ifdef TM_DBG
    int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf)
    {
        CRichLine* p = (CRichLine*)cls->m_pClass;
        return IC_Size(p->aRun);
    }
#endif // TM_DBG

    CRichRunList    aRun;
    int             nStart;         // start character
    GXDips          nLeft;       // line margin
    GXDips          nTop;
    GXDips          nWidth;      // width; -1: need merge line; -2: need merge all run.
    GXDips          nHeight;
    CRealloc<int>   logical2visual;
    
    CRichLine(int nBgn);
    ~CRichLine();

    CRichRunList::iterator InsertRun(CRichRun* run);
    TMBool IsNeedWrap() const { return nWidth < 0; }
    TMBool IsNeedRemergeRuns() const { return nWidth == -2; }
    void SetRewrap() { tm_setmin(nWidth, -1); }
    void SetRemergeRuns() { nWidth = -2; }

    void RenderLine(CGXAppBase* app, CGXRich& rich, GXDips left, GXDips top, RICHDRAW& td, IRichLink* link, GXDips nMaxWidth, GXDips nWEllipsis);
    int GetTextLength() const;
    int GetFullLength() const;
    CRichRun* InsertTextRunEnd(int nStart, int nLen, CHtmlNode* pEnd);
    CRichRun* InsertTabRunEnd(int nStart, int nLen, CHtmlNode* pEnd, int nTab);
    CRichRun* InsertImageRun(int nStart);
    CRichRun* InsertEOLRun(int nStart, int nLen);
    CRichRun* InsertCustomRun(int nStart);
    CRunIter SplitRun(CRunIter iterRun, int nCharOfRun); // split run to let nCharOfRun = 0; return next run iter
};

class CRichLineList : public TM::CFlatSet<CRichLine*, lessRich<CRichLine*> >
{
public:
    void Init(CHtmlNode& parent);
    CRichLine* InsertLine(int nLineStart);
};


// struct CRichParagraph : public CRichRun
// {
// private:
//     CRichLineList           m_aEL;
// 
// 
// };


struct CGXRich
    : public GXTextBase__
    , public CHtmlNode
{
public:
    typedef TM::CFlatSet<CRichLine*, lessRichTop<CRichLine*> > CLinePositionList;
    typedef TM::CFlatSet<IRichLink*> CLinkList;

    // the cursor have 2 mode:
    // nCharOfRun < cursor->nLength
    struct CURSOR
    {
        CLineIter   iterLine;
        CRunIter    iterRun;
        int         nCharOfRun;
    };

#ifdef TM_DBG
    int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf)
    {
        CGXRich* p = (CGXRich*)cls->m_pClass;
        return IC_Size(p->m_aEL) + IC_Size(p->m_aLink) + IC_Size(p->m_aDx) + IC_Size(p->m_aProp);
    }
#endif // TM_DBG

protected:
    CRichLineList           m_aEL;
    GXDips                  m_nWidth;       // calc wordbreak;
    CRefPtr<IRichContext>   m_ptrCreator;
    int                     m_nToken;
    SCRIPT_CONTROL          m_control;
    SCRIPT_STATE            m_state;
    CLocal                  m_local;
    TMColor                 m_color;

    friend CRichRun;
    friend struct CRichDocument_Parse;

public:
    CLinkList               m_aLink;
    CVector<int>            m_aDx;
    CTextCache              m_txtEllipsis;
    wchar_t                 m_chPassWord;

protected:
    CLineIter SplitLineAtRun(CLineIter iterLine, CRunIter iterRun, TMBool bSetRewap); // move all next run to next line
    int SplitLineAtChar(int nChar); // return: next line index

    GXDips RewrapLine(int nLine, GXDips nTop, GXDips nMaxWidth);
    int BeginReplaceText(int nBgn, int nEnd, CRichRun** ppStyle, EZOrder& zorder);
    void AfterReplaceText(int nBgn, int nEnd, int nInsert, int nToken); // nToken: return of BeginReplaceText
    void MergeParagraph(int nLine, GXDips nMaxWidth);
    GXDips GetStyleOffset(CRichLine* line);
    GXDips GetEllipsisWidth(CGXAppBase* app, RICHDRAW& td);

    void _check_size()
    {
        DbgAssert(m_aEL.size() > 0);
    }

    void _check_line(CLineIter iter)
    {
        DbgAssert(iter < m_aEL.size());
        DbgAssert(iter >= 0);
    }

    void _check_cursor(CURSOR& cursor)
    {
        _check_line(cursor.iterLine);
        DbgAssert(cursor.iterRun < m_aEL[cursor.iterLine]->aRun.size());
        if (cursor.iterRun != m_aEL[cursor.iterLine]->aRun.size())
        {
            DbgAssert(cursor.nCharOfRun >= 0);
            DbgAssert(cursor.nCharOfRun <= m_aEL[cursor.iterLine]->aRun[cursor.iterRun]->nLength);
        }
    }

public:
    void _TraceAllLine(LPCTSTR pInfo);

    void _GetRunsString(CString& str)
    {
        str.clear();
        
        for (CRichLineList::recorder rec(m_aEL); rec; ++rec)
        {
            CRichLine* line = rec.get_data();
            int j = 0;
            for (CRichRunList::recorder rec2(line->aRun); rec2; ++rec2, ++j)
            {
                CRichRun* run = rec2.get_data();
                CStringW strRun;
                if (run->eType == eRunTypeEOL)
                    strRun.Format(L"{EOL%d}", run->nLength);
//                 else if (run->IsBreakRun())
//                     strRun.Format(L"{EOB%d}", run->nLength);
                else if (run->eType == eRunTypeImage)
                    strRun = L"{IMG}";
                else if (run->eType == eRunTypeCustom)
                    strRun = L"{CUSTOM}";
                else
                {
                    strRun = L"[";
                    strRun.append(m_strTxt.data() + line->nStart + run->nStart, run->nLength);
                    CStringW strA;
                    strA.Format(L"{%d", run->analysis.s.uBidiLevel);
                    if (run->analysis.fLayoutRTL)
                        strA += L",LayoutRTL";
                    if (run->analysis.fRTL)
                        strA += L",RTL";
                    strA += L"}";
                    strRun += strA;
                    strRun += L"]";
                }
                
                str += strRun;
            }
            str += L"\r\n";
        }
    }

public:
    CGXRich();
    ~CGXRich();

    GXDips GetWidth() const { return m_nWidth; }
    GXDips GetHeight() const;

    void ReplaceBy(int nBgn, int nSize, const CGXStrView& str, EXmlLoadFlag flag);

    void TryRewrap(CGXAppBase* app);
    void _Paint(CGXAppBase* gapp, const GXPoint& ptOrg, RICHDRAW& td);

    int LineIndex(int nLine);
    int LineLength(int nLine);
    CZoom GetDPI() { return m_font->m_dpi; }

    // cursor option
    CLineIter LineFromChar(int nIndex);
    CLineIter _LineFromPos(GXDips y);

    void CursorFromLinePos(CURSOR& cursor, CLineIter& line, GXDips x);
    void ConvertCursorWrap(CURSOR& cursor, TMBool bAfterWrap);

    void CursorFromChar(CURSOR& cursor, int nIndex);
    void CursorFromPos(CURSOR& cursor, GXDips x, GXDips y);
    
    GXPoint PosFromCursor(CURSOR& cursor);
    void _Cursor2Info(CGXRich::CURSOR& cursor, RICHCURSOR& info);
    void Paint(const GXPoint& ptOrg, RICHDRAW* td, IGXApp* gapp);
    void SetMaxSize(const GXSize& sz);

public:
    // IUnknown
    TM_BGN_INTERFACE_MAP(IGXText)
    TM_END_INTERFACE_MAP()

    // CHtmlNode
    virtual CGXFont* GetFont() tm_override { return m_font; }
    virtual TMColor GetColor(RICHDRAW* rd) tm_override { /*DbgAssert(rd);*/ return m_color; }
    virtual TMColor GetBkColor(RICHDRAW& rd) tm_override { return 0; }
    virtual TMBool Transform(CGXRich& rich, CStringW& str, int nTab, TMUInt32 nToken) tm_override { DbgAssert(!GetNext()); return TransformChild(rich, str, nTab, nToken); }
    virtual IRichLink* GetLink() tm_override { return 0; }
    virtual void GetPadding(GXRect& rc) tm_override {}

    // IGXText::text
    virtual void SetString(const CGXStrPtr& str) tm_override;
    virtual CGXStrPtr GetHtml(TMSel0 nBgn, int nSize) tm_override;
    virtual void SetFormat(TMUInt32 uFormat) tm_override;
    virtual TMBool GetSize(GXSize& sz) tm_override;
    virtual void Paint(const GXRect& rc, RICHDRAW* td, GXRect* rcReal, IGXApp* gapp) tm_override
    {
        this->SetMaxSize(rc.Size());
        this->Paint(rc.TopLeft(), td, gapp);
    }
    virtual void CalcSize(GXSize& sz, IGXApp* gapp) tm_override;
    virtual void Reclaim() tm_override;

    // IGXText::rich
    virtual void SetPasswordChar(wchar_t chPassword) tm_override;
    virtual wchar_t GetPasswordChar() tm_override { return m_chPassWord; }
    virtual void SetContext(IRichContext* p) tm_override { m_ptrCreator = p; }
    virtual int GetLineCount() tm_override { return m_aEL.size(); }
    virtual int GetLineRange(int nLine, TMSel0& nStart) tm_override;
    virtual void GetInfoFromChar(TMSel0 nIndex, RICHCURSOR& info, TMBool bAfterWrap) tm_override;
    virtual TMBool GetInfoFromPos(GXDips x, GXDips y, RICHCURSOR& info) tm_override;
    virtual void ReplaceHtml(TMSel0 nBgn, int nSize, const CGXStrView& str) tm_override;
    virtual void ReplaceTxt(TMSel0 nBgn, int nSize, const CGXStrView& str) tm_override;
    virtual int GetLinks(int n, wchar_t* p) tm_override;
};



TM_END_NAMESPACE

#endif // GXRICH_H
