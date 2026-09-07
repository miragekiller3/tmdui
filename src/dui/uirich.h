/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.10.30
 *
 *  Copyright (C) 2006 miragekiller
 */

#pragma once

#ifndef UIRICH_H
#define UIRICH_H

#include "txtpic.h"


TM_BGN_NAMESPACE

class CTuiRichBase;


struct CSel2 : public TMSelRange
{
    CSel2()
    {
    }

    CSel2(TMSel2 bgn, TMSel2 end)
    {
        left = bgn;
        right = end;
        _check();
    }

    CSel2(const CSel2& rhs)
    {
        left = rhs.left;
        right = rhs.right;
        _check();
    }
    
    void _check()
    {
        DbgAssert(SelIs2(left));
        DbgAssert(SelIs2(right));
    }

    TMSel1 cbegin() const { return left; }
    TMSel1 cend() const { return right; }
    TMSel1& begin() { return left; }
    TMSel1& end() { return right; }
};

struct CSel1 : public CSel2
{
    CSel1() {}

    CSel1(TMSel1 bgn, TMSel1 end)
        : CSel2(bgn, end)
    {
        _check();
    }
    
    CSel1(const CSel1& rhs)
        : CSel2(rhs)
    {
        _check();
    }

    bool equal(CSel1& rhs) const
    {
        return left == rhs.left && right == rhs.right;
    }

    void SetMax(int nLen)
    {
        tm_setmin(right, nLen);
        tm_setmin(left, nLen);
        _check();
    }

    void _check()
    {
        DbgAssert(SelIs1(left));
        DbgAssert(SelIs1(right));
    }
};

struct CSel0 : public CSel1
{
    CSel0(TMSel0 bgn, TMSel0 end)
        : CSel1(bgn, end)
    {
        _check();
    }
    
    CSel0(const CSel0& rhs)
        : CSel1(rhs)
    {
        _check();
    }
    
    void _check()
    {
        DbgAssert(SelIs0(left));
        DbgAssert(SelIs0(right));
    }

    int length() const { return right - left; }
    bool empty() const { return left == right; }

    void Normalize() { if (left > right) tm_swap(left, right); }
};


////////////////////////////////////////////////////////////////////////////////
// CGXRich2
#define m_textRich m_gxText
typedef int TMSelX;

struct RICHFMT
{
    GXFont font;
    TMColor color;
    int dpi;
};

class TM_NO_VTABLE  CGXRich2
    : public CTxtObject
{
public:
    wchar_t     m_chPassWord;
    TMULong     m_nLimitText;
    CSubFocus   m_subFocus;
    
    struct CUndo 
    {
        int nOldStart;
        int nNewLength;
        CString strOld;
    };
    typedef CList<CUndo>        CUndoList;
    typedef CUndoList::recorder CUndoRec;
    CUndoList   m_aUndo;

    CSel1       m_sel;

#ifdef TM_DBG
    int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf)
    {
        return IC_Size(m_aUndo) + CTxtObject::IC_GetDebugInfo(cls, pbuf);
    }
#endif // TM_DBG

    CGXRich2();
    ~CGXRich2();

protected:
    TMBool IsPassword() const { return m_chPassWord;  }
    TMBool IsReadOnly() const { return m_dwStyle & ES_READONLY; }
    TMBool IsNoCaret() const { return m_dwStyle & ES_NOCARET; }
    TMBool IsFocusPrompt() const { return ES_FOCUSPROMPT & m_dwStyle; }
    TMBool IsWordWrap() const { return !(m_dwLayout & (LAYOUT_AUTOHSCROLL | LAYOUT_HSCROLL)); }
    TMBool IsWordEllipsis() const { return IsStyleWordEllipsis() && IsWordWrap(); }

    TM_DECLARE_NOFLAG   (StyleLinkUnderLine,    m_dwStyle, ES_NOLINKUNDERLINE);
    TM_DECLARE_FLAG     (StyleWordEllipsis,     m_dwStyle, ES_WORD_ELLIPSIS);
    
    TM_DECLARE_FLAG     (PriModify,             m_dwPriStyle, RICH_PRI_MODIFY);
    TM_DECLARE_FLAG     (PriEnsure,             m_dwPriStyle, RICH_PRI_ENSUREVISIBLE);
    TM_DECLARE_FLAG     (PriAfterWrap,          m_dwPriStyle, RICH_PRI_AFTER_WRAP);
    TM_DECLARE_FLAG     (PriUpdate,             m_dwPriStyle, RICH_PRI_UPDATE); // need send EN_UPDATE
    TM_DECLARE_FLAG     (PriRich,               m_dwPriStyle, RICH_PRI_TYPE_HTML);

protected:
    CTuiPoint GetViewOrg();
    void AddUndo(const CString& str, int nLen);
    void MoveCharTo(TMSel0 nNewEnd, TMBool bShift);
    void MoveRawPosTo(GXDips x, GXDips y, TMBool bShift);
    void MovePosTo(GXDips x, GXDips y, TMBool bShift);
    void MoveUpDown_M(int dLine, TMBool bShift);
    void MovePageM(GXDips nOffset, TMBool bShift);
    void MoveWordLeftRight(int nAction, TMBool bShift);
    void GetCharUnit(ETextUnit eUnit, TMSel2* nBgn, TMSel2* nEnd);
    IRichLink* GetLink(GXDips x, GXDips y);
    void Navigate(const CString& link);
    int GetCharLineEnd(int nPos, TMBool bAfterWrap);
    int WordBreakProc(TMCStr s, int index, int end, int action);
    CTuiSize GetMaxSize(TMBool bPaint);

    void _check_sel1(TMSel1 b, TMSel1 e) const
    {
        TM_IF_DBG(if (b == eSelEnd) b = 0);
        TM_IF_DBG(if (e == eSelEnd) e = 0);
        _check_sel0(b, e);
    }

    void _check_sel1() const
    {
        _check_sel1(GetSel1().cbegin(), GetSel1().cend());
    }

    void _check_sel0(TMSel0 b, TMSel0 e) const
    {
        TM_IF_DBG(int nLen = m_textRich->GetString().GetLength());
        DbgAssert(b <= nLen);
        DbgAssert(e <= nLen);
        DbgAssert(SelIs0(b));
        DbgAssert(SelIs0(e));
    }

#ifdef TUI_EDIT_MODE
    virtual CString GetDebugInfo() tm_override;
#endif
    virtual void DoSetText(const CString& str, int bRich) tm_override;

public:
    CSel1& GetSel1()             { return m_sel; }
    const  CSel1& GetSel1()      const { return m_sel; }
    CSel1  Sel2ToSel1(CSel2 sel) const;
    CSel0  Sel1ToSel0(CSel1 sel) const;
    CSel0  Sel2ToSel0(CSel2 sel) const { return Sel1ToSel0(Sel2ToSel1(sel)); }
    TMSel1 Sel2ToSel1(TMSel2 s2) const { return s2 == eSelCaret ? GetSel1().cend() : s2; }
    TMSel0  Sel1ToSel0(TMSel1 s1) const;
    TMSel0  GetCaret0 ()          const { return Sel1ToSel0(GetSel1().cend()); }
    TMSel0  SelXToSel0(TMSelX sx) const;
    TMSel1 SelXToSel1(TMSelX sx, int nLenMax) const;
    TMSel0  GetTextLength()       const { return m_textRich->GetString().GetLength(); }

    TMBool IsSingleLine() const { return m_textRich->GetFormat() & DT_SINGLELINE; }
    int GetLineCount() const { return m_textRich->GetLineCount(); }
    void EmptyUndoBuffer() { m_aUndo.clear(); }
    int LineFromChar(TMSelX nIndex);
    TMSel0 LineIndex(int nLine) { TMSel0 nStart = 0; m_textRich->GetLineRange(nLine, nStart); return nStart; }
    int LineLength(int nLine) { TMSel0 nStart; return m_textRich->GetLineRange(nLine, nStart); }
    CTuiPoint PosFromChar(TMSel0 nIndex);
    CTuiPoint RawPosFromChar(TMSel0 nIndex);
    TMSel0 CharFromRawPos(GXDips x, GXDips y, TMBool bResetAfterWrap);
    TMSel0 CharFromPos(GXDips x, GXDips y, TMBool bResetAfterWrap);
    int GetLine(int nLine, TMStr pStr, int nBuffer);
    void ReplaceSel(CSel0 s0, const CGXStrView& str, TMBool bUndo, TMBool bHtml);
    void ReplaceSel(const CGXStrView& str, TMBool bUndo, TMBool bHtml) { ReplaceSel(Sel1ToSel0(GetSel1()), str, bUndo, bHtml); }
    TMBool SetTabStops (int nCount, int* pTabs) { return 0; }
    void SetSel1(CSel1 s1, bool bOldAfterWrap);
    void SetSel1(CSel1 s1) { SetSel1(s1, IsPriAfterWrap()); }
    void SetSel2(TMSel2 nBgn, TMSel2 nEnd, bool bOldAfterWrap);
    void SetSel2(int nBgn, int nEnd) { SetSel2(nBgn, nEnd, IsPriAfterWrap()); }
    void OnWMChar(TMCharT c);
    void Copy();
    void Paste();
    void DeletePrev(TMSel0 nPos);
    void ClearSel(CSel0 s0) { ReplaceSel(s0, 0, true, false); }
    void ClearSel() { ClearSel(Sel1ToSel0(GetSel1())); }
    void Cut();
    TMBool Undo();
    TMBool CanUndo ();
//     void OnCreate();
    void OnWMCommand(int id);
    void EnableMenuItem(TMHMenu hMenu, TMUInt nID, TMBool bEnable);
    void OnWMContextMenu(int x, int y);
    TMSel0 NextChar(TMSel0 nPos);
    TMSel0 PrevChar(TMSel0 nPos);
    TMBool DoExec();
    TMBool OnWMKeyDown(int key, LPARAM lp);
    void OnWMLButtonDblClk();
    void OnWMLButtonDown(TMULong dwKeys, GXDips x, GXDips y);
    void OnWMLButtonUp(TMULong dwKeys, GXDips x, GXDips y);
    void OnSetCursor();
    void OnWMPaint(IGXApp* gapp, const CTuiRect& rcValid, EButState ebs);
    TMBool GetCaretRect(TUIRect& rc);
    void DrawCaret(IGXApp* gapp);
    void EnsureVisible();
    void GetSize(GXSize& sz) { m_textRich->GetSize(sz); }
    int GetHeight() { GXSize sz(0,0); GetSize(sz); return sz.Height(); }
    int LineFromPos(GXDips y);
    TMUInt32 GetPromptFormat();
    TMUInt32 GetFormat();
};


////////////////////////////////////////////////////////////////////////////////
// CTuiRichBase
class TM_NO_VTABLE CTuiRichBase : public CGXRich2
{
    TM_NO_COPYABLE(CTuiRichBase)

protected:
    CPropColor  m_clrTxt;
    CPropColor  m_clrTxtFocus;
    CPropColor  m_clrTxtGray;
    CText       m_gxPrompt;

    friend CGXRich2;

public: 
    TM_BGN_MASK_SYMBOL2 (CSymbol_Style, CSymbol_WsStyle)
        TM_MASK_SYMBOL2 (ES_CENTER, ES_CENTER | ES_RIGHT)
        TM_MASK_SYMBOL2 (ES_RIGHT, ES_CENTER | ES_RIGHT)
//         TM_MASK_SYMBOL  (ES_UPPERCASE)
//         TM_MASK_SYMBOL  (ES_LOWERCASE)
        TM_MASK_SYMBOL  (ES_NOHIDESEL)
//         TM_MASK_SYMBOL  (ES_OEMCONVERT)
        TM_MASK_SYMBOL  (ES_READONLY)
        TM_MASK_SYMBOL  (ES_WANTRETURN)
        TM_MASK_SYMBOL  (ES_WANTTAB)
        TM_MASK_SYMBOL  (ES_NUMBER)
        TM_MASK_SYMBOL  (ES_NOPOPUPMENU)
        TM_MASK_SYMBOL  (ES_NOAUTOSELECT)
        TM_MASK_SYMBOL  (ES_NOCARET)
        TM_MASK_SYMBOL  (ES_FOCUSPROMPT)
        TM_MASK_SYMBOL  (ES_WORD_ELLIPSIS)
        TM_MASK_BREAK   ()
    TM_END_MASK_SYMBOL  ()

    TM_BGN_MASK_SYMBOL  (CSymbol_Format)
//         TM_MASK_SYMBOL2 (DT_CENTER, DT_CENTER | DT_RIGHT)
//         TM_MASK_SYMBOL2 (DT_RIGHT, DT_CENTER | DT_RIGHT)
//         TM_MASK_SYMBOL2 (DT_VCENTER, DT_VCENTER | DT_BOTTOM)
//         TM_MASK_SYMBOL2 (DT_BOTTOM, DT_VCENTER | DT_BOTTOM)
//         TM_MASK_SYMBOL  (DT_WORD_ELLIPSIS)
        TM_MASK_SYMBOL  (DT_L2RREADING)
    TM_END_MASK_SYMBOL  ()

    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_MASK   ("Style",           m_dwStyle, CSymbol_Style)
        OBJ_PROP_MASK   ("TextFormat",      m_uFormat, CSymbol_Format);
        OBJ_PROP_INT    ("TextLimit",       m_nLimitText)
        OBJ_PROP_CLASS  ("TextColor",       m_clrTxt)
        OBJ_PROP_CLASS  ("FocusTextColor",  m_clrTxtFocus)
        OBJ_PROP_CLASS  ("GrayTextColor",   m_clrTxtGray)
        OBJ_PROP_CLASS  ("Prompt",          m_gxPrompt.p)
        CHAIN_OBJ_PROP  (CTuiWgt)
    END_OBJ_PROP_MAP    ()
    
    TMBool ProcessWindowMessage (CTuiWgt* hWnd, TMUInt uMsg, TMWParam wParam,
        TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID = 0);

    // CTuiWgt
    virtual void DoPrintClient(IGXApp* gapp, TMLParam lParam);
    virtual void Reclaim();

    void OnLoadProp(IDomElement* pXml);
    TMBool OnBindModeKeyDown(int key, TMLParam lParam);

private:
    int GetFirstVisibleLine();

// public:
//     TMBool AfterScroll ();

public:
    CTuiRichBase();
    ~CTuiRichBase();
    void FinalConstruct(CTuiObject* pParent);
};


////////////////////////////////////////////////////////////////////////////////
// CTuiRichImpl: multi line rich edit
class TM_NO_VTABLE CTuiRichImpl : public CTuiRichBase
{
public:
    TM_BGN_MASK_SYMBOL2 (CSymbol_Style_Rich, CSymbol_Style)
        TM_MASK_SYMBOL  (ES_HYPERLINK)
        TM_MASK_SYMBOL  (ES_NOLINKUNDERLINE)
        TM_MASK_BREAK   ()
    TM_END_MASK_SYMBOL  ()

public:
    BGN_OBJ_PROP_MAP    ()
//         OBJ_PROP_CHAR    ("PasswordChar", m_chPassWord)
        OBJ_PROP_MASK   ("Style", m_dwStyle, CSymbol_Style_Rich)
        CHAIN_OBJ_PROP  (CTuiRichBase)
    END_OBJ_PROP_MAP    ()

//     BEGIN_TUIMSG_MAP    ()
//         MESSAGE_HANDLER (WM_NCCREATE,          MessageHandler)
//     END_TUIMSG_MAP      ()

public:
    CTuiRichImpl ();

};


////////////////////////////////////////////////////////////////////////////////
// CTuiEditImpl: single line edit
class TM_NO_VTABLE CTuiEditImpl : public CTuiRichBase
{
public:
    BGN_OBJ_PROP_MAP    ()
        OBJ_PROP_CHAR   ("PasswordChar", m_chPassWord)
        CHAIN_OBJ_PROP  (CTuiRichBase)
    END_OBJ_PROP_MAP    ()

public:
    CTuiEditImpl();
};


// ////////////////////////////////////////////////////////////////////////////////
// CTuiMemoImpl: multi line edit
class TM_NO_VTABLE CTuiMemoImpl : public CTuiRichBase
{
public:
    CTuiMemoImpl();
};

TM_END_NAMESPACE


#endif // UIRICH_H
