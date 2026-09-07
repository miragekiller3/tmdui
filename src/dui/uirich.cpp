/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.10.30
 *
 *  Copyright (C) 2006 miragekiller
 */

#include "uirich.h"
#include "tmclipboard.h"
#include "input.h"
#include "tmkeyboard.h"
#include "view.h"


static int CALLBACK _CGXRich2_PWTextWordBreakProc(int index, int cend, int action)
{
    switch (action)
    {
    case WB_MOVEWORDLEFT:
        return index <= 1 ? 0 : index - 1;

    case WB_LEFT:
        return 0;
        
    case WB_RIGHT:
        return cend;
        
    case WB_MOVEWORDRIGHT:
        return index >= cend - 1 ? cend : index + 1;
        
    case WB_ISDELIMITER:
        return false;
        
    default:
        return 0;
    }
}

void _GetRichRunInfo(GXPText text, TM::CString& str);

#ifdef TUI_EDIT_MODE
TM_API
#endif
void _GetRichRunInfo(TM::HTUI hwnd, TM::CString& str)
{
    GXPText pt = hwnd->m_gxText;
    _GetRichRunInfo(pt, str);
}


TM_BGN_NAMESPACE

///////////////////////////////////////////////////////////////////////////
// CGXRich2
CGXRich2::CGXRich2()
    : m_sel(0, 0)
    , m_chPassWord(0)
    , m_nLimitText(-1)
{
}

CGXRich2::~CGXRich2()
{
}

CTuiPoint CGXRich2::GetViewOrg()
{
    GXDips sl = (IsWordEllipsis() && !IsFocus()) ? 0 : GetScrollLeft();
    GXDips x = m_rcClient.Left() + m_bdPadding.left - sl;
    GXDips y = m_rcClient.Top() + m_bdPadding.top;
    
    if (IsSingleLine()) // GetStyle() & ES_MULTILINE)
        return CTuiPoint(x, y + (m_rcClient.Height() - m_bdPadding.top - m_bdPadding.bottom - GetHeight()) / 2);
    else
        return CTuiPoint(x, y - GetScrollTop());
}

void CGXRich2::AddUndo(const CString& str, int nLen)
{
    CSel0 s0 = Sel1ToSel0(GetSel1());
    s0.Normalize();

    CUndo ud;
    ud.nOldStart = s0.cbegin();
    ud.nNewLength = nLen;
    ud.strOld = str;
    m_aUndo.push_back(ud);
}

void CGXRich2::MoveCharTo(TMSel0 nNewEnd, TMBool bShift)
{
    _check_sel0(nNewEnd, nNewEnd);
    SetSel2(bShift ? GetSel1().cbegin() : nNewEnd, nNewEnd);
}

void CGXRich2::MoveRawPosTo(GXDips x, GXDips y, TMBool bShift)
{
    bool bOld = IsPriAfterWrap();
    int n = CharFromRawPos(x, y, true);
    SetSel2(bShift ? GetSel1().cbegin() : n, n, bOld);
}

void CGXRich2::MovePosTo(GXDips x, GXDips y, TMBool bShift)
{
    CTuiPoint pt(GetViewOrg());
    MoveRawPosTo(x - pt.X(), y - pt.Y(), bShift);
}

void CGXRich2::MoveUpDown_M(int dLine, TMBool bShift)
{
    _check_sel1();
    
    CTuiPoint pt;
    RICHCURSOR pos = { RCFLAG_LINEINFO };
    pos.ptLeftTop = &pt;
    m_textRich->GetInfoFromChar(GetCaret0(), pos, IsPriAfterWrap());
    if (dLine > 0)
        pt.OffsetY(pos.nLineHeight);// pos.ptLeftTop.y += pos.nLineHeight;
    else
        pt.OffsetY(-1);// pos.ptLeftTop.y -= 1;
    MoveRawPosTo(pt.X(), pt.Y(), bShift);
}

void CGXRich2::MovePageM(GXDips nOffset, TMBool bShift)
{
    _check_sel1();
    SetScrollTop(GetScrollTop() + nOffset);
    CTuiPoint pi(RawPosFromChar(GetCaret0()));
    MoveRawPosTo(pi.X(), pi.Y() + nOffset, bShift);
}

int CGXRich2::WordBreakProc(TMCStr s, int index, int cend, int action)
{
    if (IsPassword())
        return _CGXRich2_PWTextWordBreakProc(index, cend, action);
    else
        return gxTextWordBreakProc(s, index, cend, action);
}

CTuiSize CGXRich2::GetMaxSize(TMBool bPaint)
{
    CTuiSize sz;
    sz.SetHeight(m_rcClient.Height() - m_bdPadding.top - m_bdPadding.bottom);
    if (bPaint)
    {
        GXDips nWidth = (!IsWordWrap() || (IsWordEllipsis() && IsFocus()))
            ? GetSafeScrollWidth()
            : GetViewWidth();
        sz.SetWidth(nWidth);
    }
    else
    {
        GXDips nWidth = ((m_dwLayout & (LAYOUT_AUTOWIDTH | LAYOUT_AUTOHSCROLL | LAYOUT_HSCROLL)) || IsStyleWordEllipsis())
            ? GX_MAX_LINE_WIDTH
            : GetViewWidth() - GX_CARET_WIDTH;// m_rcClient.Width() - m_bdPadding.left - m_bdPadding.right - GX_CARET_WIDTH;
        sz.SetWidth(nWidth);
    }
    return sz;
}


#ifdef TUI_EDIT_MODE

CString CGXRich2::GetDebugInfo()
{
    CString str1;
    CTuiRect rc;
    if (GetCaretRect(rc))
    {
        str1.Format(L"cursor rect:%g,%g,%g,%g(%g,%g);\r\n",
            rc.left, rc.top, rc.right, rc.bottom, rc.Width(), rc.Height());
    }

    CString str2;
    _GetRichRunInfo(m_gxText, str2);

    CString strBase(CTuiWgt::GetDebugInfo());
    strBase += L"\r\n\r\n";
    strBase += str1;
    strBase += L"run info:\r\n";
    strBase += str2;
    return strBase;
}

#endif

void CGXRich2::DoSetText(const CString& str, int bRich)
{
#ifdef TUI_EDIT_MODE
    m_textRich.m_str = str;
#endif

    if (IsPriRich() && bRich != TUISETTEXT::eText)
        m_textRich->SetString(str);
    else
        m_textRich->ReplaceTxt(0, -1, TM_T2W(str));

    GetSel1().SetMax(GetTextLength());
    EmptyUndoBuffer();
    Set_WS_PRI_DIRTY();
    SetPriUpdate();
    if (IsSingleLine() && !IsLifeNew()) // (m_dwStyle & ES_MULTILINE))
        Notify(EN_CHANGE);
//     Invalidate ();
}


// nAction : WB_LEFT, WB_RIGHT
void CGXRich2::MoveWordLeftRight(int nAction, TMBool bShift)
{
    _check_sel1();
    
    CString str(m_textRich->GetString());
    int e = WordBreakProc(str, GetCaret0(), str.GetLength(), nAction);
    
    ResetPriAfterWrap();
    MoveCharTo(e, bShift);
}

void CGXRich2::GetCharUnit(ETextUnit eUnit, TMSel2* nBgn, TMSel2* nEnd)
{
    if (((eUnit & eTextUnitMaskType)) == eTextUnitDocument)
    {
        if (nBgn)
            *nBgn = 0;
        if (nEnd)
            *nEnd = GetTextLength();
        return;
    }

    CSel0 s0 = Sel2ToSel0(CSel2(nBgn ? *nBgn : 0, nEnd ? *nEnd : 0));
    switch (eUnit & eTextUnitMaskType)
    {
    default:
    case eTextUnitChar:
        if (nBgn)
            *nBgn = PrevChar(s0.cbegin());
        if (nEnd)
            *nEnd = NextChar(s0.cend());
        break;

    case eTextUnitFormat:
    case eTextUnitWord:
        {
            CString str(m_textRich->GetString());
            if (nBgn)
                *nBgn = WordBreakProc(str, s0.cbegin(), str.GetLength(), WB_LEFT);
            if (nEnd)
                *nEnd = WordBreakProc(str, s0.cend(), str.GetLength(), WB_RIGHT);
        }
        break;

    case eTextUnitSentence:
    case eTextUnitLine:
    case eTextUnitParagraph:
        {
            RICHCURSOR line = { RCFLAG_LINEINFO };
            if (nBgn)
            {
                m_textRich->GetInfoFromChar(s0.cbegin(), line, false);
                *nBgn = line.nLineCharStart;
            }
            if (nEnd)
            {
                if (!nBgn || !s0.empty())
                    m_textRich->GetInfoFromChar(s0.cend(), line, false);
                *nEnd = line.nLineCharStart + line.nLineCharLength;
            }
        }
        break;

    case eTextUnitPage:
        {
            GXDips nHeight = m_rcClient.Height();
            CTuiPoint pi;
            if (nBgn)
            {
                pi = RawPosFromChar(s0.cbegin());
                *nBgn = CharFromRawPos(pi.X(), pi.Y() - nHeight, false);
            }
            if (nEnd)
            {
                if (*nBgn || !s0.empty())
                    pi = RawPosFromChar(s0.cend());
                *nEnd = CharFromRawPos(pi.X(), pi.Y() - nHeight, false);
            }
        }
        break;
    }
}

CSel1 CGXRich2::Sel2ToSel1(CSel2 sel) const
{
    TMSel1 b = Sel2ToSel1(sel.cbegin());
    TMSel1 e = Sel2ToSel1(sel.cend());
    return CSel1(b, e);
}

CSel0 CGXRich2::Sel1ToSel0(CSel1 sel) const
{
    TMSel0 b = sel.cbegin();
    TMSel0 e = sel.cend();
    if (b == eSelEnd || e == eSelEnd)
    {
        long nLen = GetTextLength();
        if (b == eSelEnd)
            b = nLen;
        if (e == eSelEnd)
            e = nLen;
    }
    return CSel0(b, e);
}

TMSel0 CGXRich2::Sel1ToSel0(TMSel1 s1) const
{
    _check_sel1(s1, s1);
    if (s1 == eSelEnd)
        s1 = GetTextLength();
    return s1;
}

TMSel0 CGXRich2::SelXToSel0(TMSelX n) const
{
    int nLen = GetTextLength();
    if (n == eSelCaret)
        n = GetSel1().cend();
    if (n == eSelEnd)
        return nLen;
    tm_setmax(n, 0);
    tm_setmin(n, nLen);
    return n;
}

TMSel1 CGXRich2::SelXToSel1(TMSelX n, int nLenMax) const
{
    if (n == eSelCaret)
        return GetSel1().cend();
    if (n == eSelEnd)
        return n;
    tm_setmax(n, 0);
    tm_setmin(n, nLenMax);
    return n;
}

int CGXRich2::LineFromChar(TMSelX nIndex)
{
    TMSel0 sel = SelXToSel0(nIndex);
    RICHCURSOR line = { RCFLAG_LINEINFO };
    m_textRich->GetInfoFromChar(sel, line, true);
    return line.nLineIndex;
}

CTuiPoint CGXRich2::PosFromChar(TMSel0 nIndex)
{
    return RawPosFromChar(nIndex) + GetViewOrg().ToSize();
}

CTuiPoint CGXRich2::RawPosFromChar(TMSel0 nIndex)
{
    DbgAssert(SelIs0(nIndex));
    _check_sel1(nIndex, nIndex);
    CTuiPoint pt;
    RICHCURSOR pos = { 0 };
    pos.ptLeftTop = &pt;
    m_textRich->GetInfoFromChar(nIndex, pos, true);
    return pt;
}

TMSel0 CGXRich2::CharFromRawPos(GXDips x, GXDips y, TMBool bResetAfterWrap)
{
    RICHCURSOR cursor = { RCFLAG_LINEINFO | RCFLAG_RUNINFO };
    m_textRich->GetInfoFromPos(x, y, cursor);
    
    if (bResetAfterWrap)
        SettingPriAfterWrap(cursor.nRunCharStart + cursor.nCharOfRun < cursor.nLineCharLength);
    return cursor.nLineCharStart + cursor.nRunCharStart + cursor.nCharOfRun;
}

TMSel0 CGXRich2::CharFromPos(GXDips x, GXDips y, TMBool bResetAfterWrap)
{
    CTuiPoint pt(GetViewOrg());
    x -= pt.X();
    y -= pt.Y();
    return CharFromRawPos(x, y, bResetAfterWrap);
}

int CGXRich2::GetLine(int nLine, TMStr pStr, int nBuffer)
{
    TMSel0 nStart;
    int nLen = m_textRich->GetLineRange(nLine, nStart);
    return StrCopy(pStr, nBuffer, m_textRich->GetString().data() + nStart, nLen);
}

void CGXRich2::ReplaceSel(CSel0 s0, const CGXStrView& strIn, TMBool bUndo, TMBool bHtml)
{
    _check_sel1();

    if (s0.empty() && strIn.IsEmpty())
        return;
    
    s0.Normalize();
    
    CString strOld(m_textRich->GetString());

    // start replace
    if (bHtml)
        m_textRich->ReplaceHtml(s0.cbegin(), s0.length(), strIn);
    else if (IsSingleLine())
    {
        CGXStrView strInsert;
        strIn.SplitLine(0, strInsert);
        m_textRich->ReplaceTxt(s0.cbegin(), s0.length(), strInsert);
    }
    else
        m_textRich->ReplaceTxt(s0.cbegin(), s0.length(), strIn);

    CGXStrPtr str(m_textRich->GetString());
    int nLenOld = strOld.GetLength();
    int nLen = m_nLimitText;
    int nLenNew = str.GetLength();
    int nSelLen; 
    if (nLen >= 0 && nLenNew > nLen)
    {
        m_textRich->ReplaceTxt(nLen, nLenNew - nLen, CXmlTag());
        nLenNew = nLen;
        nSelLen = nLen - s0.cbegin();
    }
    else
        nSelLen = s0.length();

    nLen = nLenNew - nLenOld + nSelLen;
    if (bUndo)
        AddUndo(strOld.substr(s0.cbegin(), nSelLen), nLen);

    // new selection position
    int nNewPos = s0.cbegin() + nLen;
    Set_WS_PRI_DIRTY();
    SetPriModify();
    SetSel2(nNewPos, nNewPos);
    _check_sel1();

    // notify
    SetPriUpdate();
    Notify(EN_CHANGE);
    Notify(EN_CHANGEEX);
}

void CGXRich2::SetSel1(CSel1 s1, bool bOldAfterWrap)
{
    GetSel1() = s1;
    _check_sel1();
    
    if (Is_WS_PRI_DIRTY())
        SetPriEnsure();
    else
        EnsureVisible();
    
    Invalidate();
    g_input.CaretReset(this);
    
    Notify(EN_SELCHANGE);
}

void CGXRich2::SetSel2(TMSel2 nBgn, TMSel2 nEnd, bool bOldAfterWrap)
{
    CSel1 s1(Sel2ToSel1(CSel2(nBgn, nEnd)));
    if (s1.equal(GetSel1()) && (IsPriAfterWrap() == bOldAfterWrap))
        return;
    SetSel1(s1);
}

void CGXRich2::OnWMChar(TMCharT c)
{
    TMUInt32 dwStyle = GetStyle();
    if (dwStyle & ES_READONLY)
    {
        if (c == 0x01)
            SetSel2(0, -1);
        return;
    }
    switch (c)
    {
    case L'\t':
        if (!IsSingleLine()) //  dwStyle & ES_MULTILINE)
            ReplaceSel(TM_CL("\t"), true, false);
        break;
        
    case VK_BACK:
        if (!g_input.m_key.IsModifiersControl())
        {
            _check_sel1();
            CSel0 s0 = Sel1ToSel0(GetSel1());
            if (s0.empty())
                DeletePrev(s0.cbegin());
            else
                ClearSel(s0);
        }
        break;
                
    default:
        /*If Edit control style is ES_NUMBER allow users to key in only numeric values*/
        if ((dwStyle & ES_NUMBER) && !(c >= TM_T('0') && c <= TM_T('9')))
            break;
        
        if ((c >= ' ') && (c != 127))
        {
            TMCharT str[2] = { c, 0 };
            ReplaceSel(CGXStrView(str, 1), true, false);
        }
        break;
    }
}

void CGXRich2::Copy()
{
    _check_sel1();

    if (IsPassword())
        return;
    
    CSel0 s0 = Sel1ToSel0(GetSel1());
    if (s0.empty())
        return;

    s0.Normalize();
    int nLen = s0.length();
    
    CClipboard clipboard;
    clipboard.EmptyClipboard();
    clipboard.AddString(m_textRich->GetString().data() + s0.cbegin(), nLen);
    if (IsPriRich())
    {
        CGXStrPtr str(m_textRich->GetHtml(s0.cbegin(), nLen));
        clipboard.AddHTML(str);
    }
}

void CGXRich2::Paste()
{
    if (IsReadOnly())
        return;

    CClipboard clipboard;
    CString str;
    if (IsPriRich() && clipboard.GetHTML(str))
        ReplaceSel(str, true, true);
    else if (clipboard.GetText(str))
        ReplaceSel(str, true, false);
}

void CGXRich2::DeletePrev(TMSel0 nPos)
{
    if (!nPos)
        return;
    CSel0 s0 = CSel0(PrevChar(nPos), nPos);
    ClearSel(s0);
}

void CGXRich2::Cut()
{
    Copy();
    if (!IsReadOnly())
        ClearSel();
}

TMBool CGXRich2::Undo()
{
    TMUInt32 dwStyle = GetStyle();
    
    // for a single-line edit control, the return value is always TMTrue
    if ((dwStyle & ES_READONLY) || (m_aUndo.size() == 0))
        return IsSingleLine(); // (dwStyle & ES_MULTILINE);
    
    CUndo& ud = m_aUndo.back();
    SetSel2(ud.nOldStart, ud.nOldStart + ud.nNewLength);
    ReplaceSel(ud.strOld, false, false);
    m_aUndo.pop_back();
    return true;
}

TMBool CGXRich2::CanUndo ()
{
    return m_aUndo.size() != 0;
}

// void CGXRich2::OnCreate()
// {
// }

void CGXRich2::OnWMCommand(int id)
{
    switch (id)
    {
    case WM_UNDO:
    case EM_UNDO:
        Undo();
        break;
        
    case WM_CUT:
        Cut();
        break;
        
    case WM_COPY:
        Copy();
        break;
        
    case WM_PASTE:
        Paste();
        break;
        
    case WM_CLEAR:
        ClearSel();
        break;
        
    case EM_SETSEL: // Select All
        SetSel2(0, -1);
        break;
        
    default:
        break;
    }
}

void CGXRich2::EnableMenuItem(TMHMenu hMenu, TMUInt nID, TMBool bEnable)
{
#ifdef TM_WIN
    TMUInt uCmd = MF_BYCOMMAND | (bEnable ? MF_ENABLED : MF_GRAYED);
    DbgVerify(::EnableMenuItem(hMenu, nID, uCmd));
#endif
}

void CGXRich2::OnWMContextMenu(int x, int y)
{
    _check_sel1();

#ifndef TM_QT
    
    TMHModule hUser = dll_user32::Load();
    TMHMenu hMenu = LoadMenu(hUser, MAKEINTRESOURCE(1));
    TMHMenu hPop = GetSubMenu(hMenu, 0);
    TMUInt32 dwStyle = GetStyle ();
    CSel0 s0 = Sel1ToSel0(GetSel1());
    s0.Normalize();
    bool bEmpty = s0.empty();
    
    TMBool bUndo = CanUndo() && ! (dwStyle & ES_READONLY);
    EnableMenuItem(hPop, WM_UNDO, bUndo);

    TMBool bCopy = !bEmpty && !IsPassword();// (dwStyle & ES_PASSWORD);
    EnableMenuItem(hPop, WM_COPY, bCopy);
    
    TMBool bCut = bCopy && ! (dwStyle & ES_READONLY);
    EnableMenuItem(hPop, WM_CUT, bCut);
    
    TMBool bPaste = IsClipboardFormatAvailable (TM_SW_UNICODE(CF_UNICODETEXT, CF_TEXT)) && !(dwStyle & ES_READONLY);
    EnableMenuItem(hPop, WM_PASTE, bPaste);
    
    TMBool bDelete = !bEmpty && !(dwStyle & ES_READONLY);
    EnableMenuItem(hPop, WM_CLEAR, bDelete);
    
    CString str(m_textRich->GetString());
    int nTextLen = str.GetLength();
    TMBool bSelectAll = (nTextLen && (s0.length() != nTextLen));
    EnableMenuItem(hPop, EM_SETSEL, bSelectAll);
    
    DeleteMenu(hPop, 11,    MF_BYPOSITION);    // delete menu item : Insert Unicode control character
    DeleteMenu(hPop, 32769, MF_BYCOMMAND);  // delete menu item : Show Unicode control characters
    DeleteMenu(hPop, 32768, MF_BYCOMMAND);  // delete menu item : Right to left Reading order
    DeleteMenu(hPop, 8,     MF_BYPOSITION);     // delete the separator below menu item "Selete All"
    
    int nCmd = GetModule()->TrackPopupMenu(hPop, TPM_LEFTALIGN | TPM_RETURNCMD, x, y, GetHWND());
    DbgTrace("TrackPopupMenu(%d, %d) ret:%d", x, y, nCmd);
    
    DestroyMenu(hMenu);
    
    SendMessage(WM_COMMAND, nCmd, 0);

#endif
}

TMSel0 CGXRich2::NextChar(TMSel0 nPos)
{
    _check_sel0(nPos, nPos);
    CGXStrPtr p = m_textRich->GetString();
    int n = p.length();
    if (n == 0 || nPos >= n - 1)
        return n;
    
    ++nPos;
    if (p[nPos - 1] == L'\r' && !IsPassword())
    {
        while (p[nPos] == L'\r')
            ++nPos;
        ++nPos;
    }
    _check_sel0(nPos, nPos);
    return nPos;
}

TMSel0 CGXRich2::PrevChar(TMSel0 nPos)
{
    _check_sel0(nPos, nPos);
    if (nPos < 1)
        return 0;
    --nPos;
    CGXStrPtr p = m_textRich->GetString();
    if (!IsPassword())
    {
        while (nPos > 0 && p[nPos-1] == L'\r')
            --nPos;
    }
    _check_sel0(nPos, nPos);
    return nPos;
}

TMBool CGXRich2::DoExec()
{
    if (!IsReadOnly())
        return false;
    TMCharW c[PATH_MAX];
    if (!m_textRich->GetLinks(m_subFocus.GetCurrentTab(), c))
        return false;
    Navigate(c);
    return true;
}

TMBool CGXRich2::OnWMKeyDown(int key, LPARAM lp)
{
//     if (CKeyboard::IsKeyDown(VK_TAB))
//         return false;
    
    _check_sel1();
    CKeyInput ki(key, lp);
    TMUInt16 nModifiers = ki.GetModifiersFast();
    if (nModifiers & (MOD_ALT | MOD_WIN))
        return false;

    key = GetModule()->GetLocal().ConvertKey(key);

    switch (key)
    {
    case VK_TAB:
        if (nModifiers & MOD_CONTROL)
            return false;
        if (IsPriRich() && IsReadOnly())
        {
            int nSize = m_textRich->GetLinks(0, 0);
            return m_subFocus.OnTabKey(this, nSize, ki);
        }
        if (!(GetStyle() & ES_WANTTAB))
            return false;
        OnWMChar(key);
        break;

    case VK_UP:
        if (!IsSingleLine()) //  pEdit->GetStyle() & ES_MULTILINE)
            MoveUpDown_M(-1, nModifiers & MOD_SHIFT);
        else
            return false;
        break;

    case VK_LEFT:
        //m_bSelCur = false;
        SetPriAfterWrap();
        if (nModifiers & MOD_CONTROL)
            MoveWordLeftRight(WB_MOVEWORDLEFT, nModifiers & MOD_SHIFT);
        else
            MoveCharTo(PrevChar(GetCaret0()), nModifiers & MOD_SHIFT);
        break;

    case VK_DOWN:
        if (!IsSingleLine()) //  pEdit->GetStyle() & ES_MULTILINE)
            MoveUpDown_M(1, nModifiers & MOD_SHIFT);
        else
            return false;
        break;

    case VK_RIGHT:
//        m_bSelCur = false;
        SetPriAfterWrap();
        if (nModifiers & MOD_CONTROL)
            MoveWordLeftRight(WB_MOVEWORDRIGHT, nModifiers & MOD_SHIFT);
        else
            MoveCharTo(NextChar(GetCaret0()), nModifiers & MOD_SHIFT);
        break;

    case VK_HOME:
        {
            int nPos = 0;
            if (!(nModifiers & MOD_CONTROL))
            {
                RICHCURSOR line = { RCFLAG_LINEINFO };
                m_textRich->GetInfoFromChar(GetCaret0(), line, IsPriAfterWrap());
                nPos = line.nLineCharStart;
            }
            SetPriAfterWrap();
            MoveCharTo(nPos, nModifiers & MOD_SHIFT);
        }
        break;

    case VK_END:
        {
            int nPos;
            if (nModifiers & MOD_CONTROL)
                nPos = GetTextLength();
            else
                nPos = GetCharLineEnd(GetCaret0(), IsPriAfterWrap());
            ResetPriAfterWrap();
            MoveCharTo(nPos, nModifiers & MOD_SHIFT);
        }
        break;

    case VK_PRIOR:
        if (!IsSingleLine()) //  pEdit->GetStyle() & ES_MULTILINE)
        {
            GXDips nHeight = m_rcClient.Bottom() - m_rcClient.Top();
            MovePageM(-nHeight, nModifiers & MOD_SHIFT);
        }
        else
            return false;
        break;

    case VK_NEXT:
        if (!IsSingleLine()) //  pEdit->GetStyle() & ES_MULTILINE)
        {
            GXDips nHeight = m_rcClient.Bottom() - m_rcClient.Top();
            MovePageM(nHeight, nModifiers & MOD_SHIFT);
        }
        else
            return false;
        break;

    case VK_DELETE:
        if (!(GetStyle() & ES_READONLY) && (nModifiers != (MOD_SHIFT | MOD_CONTROL)))
        {
            CSel0 s0 = Sel1ToSel0(GetSel1());
            if (!s0.empty())
            {
                if (nModifiers & MOD_SHIFT)
                    Cut();
                else
                    ClearSel(s0);
            }
            else
            {
                if (nModifiers & MOD_SHIFT) // delete character left of caret 
                    DeletePrev(s0.cbegin());
                else if (nModifiers & MOD_CONTROL) // delete to cend of line
                {
                    s0.end() = GetCharLineEnd(s0.cend(), true);
                    ClearSel(s0);
                }
                else // delete character right of caret
                {
                    int nEnd = s0.cend();
                    int nLen = GetTextLength(); 
                    if (nEnd < nLen)
                    {
                        s0.end() = NextChar(nEnd);
                        ClearSel(s0);
                    }
                }
            }
        }
        break;

    case VK_INSERT:
        if (nModifiers == MOD_SHIFT)
            Paste();
        else if (nModifiers == MOD_CONTROL)
            Copy();
        else
            break;
        return true;

    case L'A':
        if (nModifiers == MOD_CONTROL)
            return SetSel2(0, -1), true;
        return false;

    case L'C':
        return (nModifiers == MOD_CONTROL) ? (SendMessage(WM_COPY, 0, 0), true) : false;
        
    case L'V':
        return (nModifiers == MOD_CONTROL) ? (SendMessage(WM_PASTE, 0, 0), true) : false;

    case L'X':
        return (nModifiers == MOD_CONTROL) ? (SendMessage(WM_CUT, 0, 0), true) : false;

    case L'Z': 
        return (nModifiers == MOD_CONTROL) ? (SendMessage(WM_UNDO, 0, 0), true) : false;

    case VK_RETURN:
        if (DoExec())
            return true;
        if (!IsSingleLine() && !IsReadOnly()) // (pEdit->m_dwStyle & (ES_MULTILINE | ES_READONLY)) == ES_MULTILINE)
        {
            SetPriAfterWrap();
            ReplaceSel(TM_CL("\r\n"), true, false);
            return true;
        }
        return false;

    case ' ':
        return DoExec();


//            return (pEdit->m_dwStyle & ES_MULTILINE) ? false : true;

//         case VK_ESCAPE:
//             pEdit->SendNotifyMessage (WM_COMMAND, IDCANCEL, (TMLParam)pEdit);
//             break;

    default:
        return false;
    }
    return true;
}

void CGXRich2::OnWMLButtonDblClk()
{
    _check_sel1();
    
    CGXStrPtr p(m_textRich->GetString());
    int nLen =  p.GetLength();
    TMSel0 s0 = GetCaret0();
    int b = WordBreakProc(p.data(), s0, nLen, WB_LEFT);
    int e = WordBreakProc(p.data(), s0, nLen, WB_RIGHT);
    SetSel2(b, e);
}

void CGXRich2::OnWMLButtonDown(TMULong dwKeys, GXDips x, GXDips y)
{
    _check_sel1();

    if (!IsFocus() && !(m_dwStyle & ES_NOAUTOSELECT))
        SetFocus(eFocusReasonMouse);
    else
    {
        SetFocus(eFocusReasonMouse);
        SetCapture();
        MovePosTo(x, y, dwKeys & MK_SHIFT);
    }
}

void CGXRich2::Navigate(const CString& link)
{
    long id = link.ToLong(-1);
    if (0 == id || link.IsEmpty())
    {
        if (m_dwID)
            Notify(BN_CLICKED);
    }
    else if (-1 != id)
        Notify(id, BN_CLICKED, false);
    else
    {
#ifdef TM_WIN
        TMBool bRet;
#ifndef _WIN32_WCE
        DWORD_PTR dwRet = (DWORD_PTR)::ShellExecute(0, TM_T("open"), link, 0, 0, SW_SHOWNORMAL);
#else // CE specific
        SHELLEXECUTEINFO shExeInfo = { sizeof(SHELLEXECUTEINFO), 0, 0, L"open", link, 0, 0, SW_SHOWNORMAL, 0, 0, 0, 0, 0, 0, 0 };
        DbgVerify(::ShellExecuteEx(&shExeInfo));
        DWORD_PTR dwRet = (DWORD_PTR)shExeInfo.hInstApp;
#endif // _WIN32_WCE
        bRet = (dwRet == 0) || (dwRet > 32);
        DbgVerify(bRet);
#endif
    }

//     if (!bNotify && GetDlgCtrlID())
//         Notify(GetDlgCtrlID());
}

int CGXRich2::GetCharLineEnd(int nPos, TMBool bAfterWrap)
{
    RICHCURSOR line = { RCFLAG_LINEINFO };
    m_textRich->GetInfoFromChar(nPos, line, bAfterWrap);
    return line.nLineCharStart + line.nLineCharLength;
}

void CGXRich2::OnWMLButtonUp(TMULong dwKeys, GXDips x, GXDips y)
{
    CView* view = GetView();
    view->ReleaseCapture();
    TUIHITINFO ht;
    view->GetMouseLDownState(ht);
    if (!CMath::Equal(PointX(ht.pt), x, (GXDips)1) || !CMath::Equal(PointY(ht.pt), y, (GXDips)1))
        return;
    IRichLink* pLink = GetLink(x, y);
    if (pLink)
    {
        TMCharT c[1024] = {0};
        pLink->GetStringRef(c);
        Navigate(c);
    }
}

IRichLink* CGXRich2::GetLink(GXDips x, GXDips y)
{
//     GXRich__::CURSOR cursor;
    CTuiPoint pt(GetViewOrg());
    x -= pt.X();
    y -= pt.Y();
    RICHCURSOR info = { RCFLAG_RUNINFO | RCFLAG_LINEINFO | RCFLAG_LINELEFT};
    m_textRich->GetInfoFromPos(x, y, info);
    if (x < (info.nRunLeft + info.nLineLeft) || (y > info.nLineTop + info.nLineHeight))
        return 0;
    return info.pLink;
}

void CGXRich2::OnSetCursor()
{
    TUIHITINFO ht;
    GetView()->GetMouseHotState(ht);
    IRichLink* pLink = GetLink(PointX(ht.pt), PointY(ht.pt));
    CTuiWgt::OSSetCursor(pLink ? CCursorAT::cursor_HAND() : CCursorAT::cursor_IBEAM());
}

static CSel0& _RICHSEL_GetSel0(RICHSEL& rs)
{
    DbgAssertS(offsetof(RICHSEL, clrSelText) - offsetof(RICHSEL, nSelBegin) == sizeof(CSel1));
    return *(CSel0*)&rs.nSelBegin;
}

void CGXRich2::OnWMPaint(IGXApp* gapp, const CTuiRect& rcValid, EButState ebs)
{
    _check_sel1();
    
    //         SetBkMode (hDC, TRANSPARENT);
    TMBool bFocus = IsFocus();
    TMUInt32 dwStyle = GetStyle();
    
    CTuiPoint ptOrg(GetViewOrg());
    RICHDRAW rd;
    RICHSEL td;
    rd.bDisable = ebs == e_bsDisable;
    rd.eVisual = GetModule()->GetVisual();//  .bHighContrast = GetView()->IsHighContrast();
    rd.pct = GetModule()->m_pColorTransform;
    rd.lct = GetObjectID();
    rd.nWidth = IsFocus() ? (GXDips)0 : rcValid.Width();
    TMBool bDrawSel =
        (((dwStyle & ES_NOHIDESEL) && !bFocus) || bFocus) &&
        !(_RICHSEL_GetSel0(td) = Sel1ToSel0(GetSel1())).empty();

    if (bDrawSel)
    {
        rd.pSel = &td;
        if (GetView()->IsHighContrast())
        {
            td.clrSelBackground = ColorFromSys(COLOR_HIGHLIGHT);
            td.clrSelText = ColorFromSys(COLOR_HIGHLIGHTTEXT);
        }
        else
        {
            CView* pView = GetView();
            td.clrSelBackground = pView->m_clrEditSelBackground;
            td.clrSelText = pView->m_clrColorEditSelText;
        }
    }
    else
        rd.pSel = 0;

    rd.nCurrentTab = bFocus ? m_subFocus.GetCurrentTab() : -1;
//     gxTextSetMaxSize(m_textRich, GetMaxSize(true));
    GXSize sz;
    m_textRich->GetSize(sz);
    m_textRich->Paint(GXRect(ptOrg, sz), &rd, 0, gapp);
}

TMBool CGXRich2::GetCaretRect(TUIRect& rc)
{
    _check_sel1();

    CTuiPoint pt;
    RICHCURSOR pos = { RCFLAG_ALL };
    pos.ptLeftTop = &pt;
    m_textRich->GetInfoFromChar(GetCaret0(), pos, IsPriAfterWrap());
    if (!pos.ptLeftTop)// (pos.nFlag & RCFLAG_CHARPOS))
        return false;
    pt += GetViewOrg();
    (static_cast<CTuiRect&>(rc)).SetLTWH(
        pt.X() - (GX_CARET_WIDTH / 2),
        pt.Y() + pos.nLineHeight - pos.nRunHeight,
        GX_CARET_WIDTH,
        pos.nRunHeight);
    return true;
}

void CGXRich2::DrawCaret(IGXApp* gapp)
{
    CTuiRect rc;
    if (GetCaretRect(rc))
        g_input.CaretDraw(gapp, this, rc);
}

void CGXRich2::EnsureVisible()
{
    CTuiRect rc;
    if (GetCaretRect(rc))
        CTuiWgt::ScrollToVisible(rc, true, true);
}

int CGXRich2::LineFromPos(GXDips y)
{
    RICHCURSOR cursor = { RCFLAG_LINEINFO };
    m_textRich->GetInfoFromPos(0, y, cursor);
    return cursor.nLineIndex;

//     return gxRichLineFromPos(m_textRich, y);
//     GXRich__::CLineIter iter = m_textRich->LineFromPos(y);
//     return m_textRich->LineIterToLineIndex(iter);
}

TMUInt32 CGXRich2::GetPromptFormat()
{
    TMUInt32 dwStyle = GetStyle();
    TMUInt32 dwFormat = DT_NOPREFIX | DT_EXPANDTABS;
    if (dwStyle & ES_CENTER)
        dwFormat |= DT_CENTER;
    else if (dwStyle & ES_RIGHT)
        dwFormat |= DT_RIGHT;
    if (IsSingleLine()) // (dwStyle & ES_MULTILINE))
        dwFormat |= (DT_VCENTER | DT_SINGLELINE);
    dwFormat |= (m_uFormat & DT_L2RREADING);
    return dwFormat;
}

TMUInt32 CGXRich2::GetFormat()
{
    TMUInt32 dwStyle = GetStyle();
    TMUInt32 dwFormat = 0;
    if (dwStyle & ES_CENTER)
        dwFormat |= DT_CENTER;
    else if (dwStyle & ES_RIGHT)
        dwFormat |= DT_RIGHT;
    if (IsSingleLine()) // (dwStyle & ES_MULTILINE))
        dwFormat |= DT_SINGLELINE;
    else if (IsWordWrap())
        dwFormat |= DT_WORDBREAK;
    if (!IsStyleLinkUnderLine())
        dwFormat |= DT_NOLINKUNDERLINE;
    if (dwStyle & ES_WORD_ELLIPSIS)
        dwFormat |= DT_WORD_ELLIPSIS;
    dwFormat |= (m_uFormat & DT_L2RREADING);
    return dwFormat;
}


////////////////////////////////////////////////////////////////////////////////
// CTuiRichBase
CTuiRichBase::CTuiRichBase()
//     : m_clrTxt(cColorInvalid)       //  (TMRGB(0,0,0))
//     , m_clrTxtFocus(cColorInvalid)  //  (TMRGB(0,0,0))
//     , m_clrTxtGray(cColorInvalid)   //  (TMRGB(128,128,128))
{
    m_dwStyle |= WS_TABSTOP;
    m_dwAdvStyle |= WS_EX_CLIENTEDGE;

    m_textRich.Create(true);
}

CTuiRichBase::~CTuiRichBase()
{
}

void CTuiRichBase::FinalConstruct(CTuiObject* pParent)
{
    CGXRich2::FinalConstruct(pParent);
    if (IsPriRich())
    {
        CRefPtr<IRichContext> ptr;
        DISABLE_SPY();
        CTuiWidget(this).GetRichContext(&ptr);
        m_textRich->SetContext(ptr);
    }
}


// int CTuiRichBase::GetScrollWidth () const
// {
//     return m_nWidthP + CARET_WIDTH; // + m_bdPadding.left + m_bdPadding.right
// }
// 
// int CTuiRichBase::GetScrollHeight () const
// {
//     return m_aEL.size() * m_nLineHeightP; // + m_bdPadding.top + m_bdPadding.right;
// }

// int CTuiRichBase::GetVScrollLine () const
// {
//     return GetLineHeight();
// }

// TMBool CTuiRichBase::AfterScroll ()
// {
// //     MoveCaret (this, false);
//     return true;
// }

TMBool CTuiRichBase::ProcessWindowMessage (CTuiWgt* hWnd, TMUInt uMsg,
                                      TMWParam wParam, TMLParam lParam,
                                      TMResult& lResult, TMULong dwMsgMapID)
{
    TMBool bHandled = true;
    
    switch (uMsg)
    {
    case CM_WINDOWPOSCHANGED:
        if ((((TUIWINDOWPOS*)lParam)->flags & (SWP_NOSIZE | SWP_FRAMECHANGED)) != SWP_NOSIZE)
        {
            SetDirtyAndInvalidate();
        }
        bHandled = false;
        break;

    case CM_ZOOMED:
        Set_WS_PRI_DIRTY();
//         SetDpi(wParam);
        bHandled = false;
        break;

    case CM_SETFONT:
        CTuiWgt::ProcessWindowMessage (this, uMsg, wParam, lParam, lResult);
        Set_WS_PRI_DIRTY();
        break;
        
    case WM_GETTEXT:
        lResult = m_textRich->GetString().CopyTo((TMStr)lParam, wParam);
        break;

    case WM_GETTEXTLENGTH:
        lResult = GetTextLength();
        break;

    case CM_SETTEXTCOLOR:
        switch (wParam)
        {
        case e_bsNormal:
            m_clrTxt = lParam;
//             SetDefaultColor(m_clrTxt);
            break;
        case e_bsFocus:     m_clrTxtFocus = lParam; break;
        case e_bsDisable:   m_clrTxtGray = lParam; break;
        default:
//             SetSelColor((TMColor)lParam);
//             SetDirty();
            break;
        }
        Invalidate();
        break;

    case EM_GETSEL:
        {
            CSel0 s0 = Sel1ToSel0(GetSel1());
            s0.Normalize();
            
            if (wParam)
                *((TMSel0*)wParam) = s0.cbegin();
            if (lParam)
                *((TMSel0*)lParam) = s0.cend();
            lResult = MAKELONG(s0.cbegin(), s0.cend());
        }
        break;
    
    case EM_SETSEL:
        {
            int n = GetTextLength();
            TMSel1 b = SelXToSel1(wParam, n);
            TMSel1 e = SelXToSel1(lParam, n);
            SetSel1(CSel1(b, e));
        }
        break;

    case EM_GETRECT:
    case EM_SETRECT:
    case EM_SETRECTNP:
    case EM_SCROLL:
    case EM_LINESCROLL:
    case EM_SCROLLCARET:
    case EM_SETHANDLE:
    case EM_GETHANDLE:
    case EM_GETTHUMB:
    case EM_FMTLINES:
    case EM_SETWORDBREAKPROC:
    case EM_GETWORDBREAKPROC:
    case EM_SETMARGINS:
    case EM_GETMARGINS:
        DbgAssert(0);
        break;

    case EM_GETMODIFY:
        lResult = IsPriModify();
        break;

    case EM_SETMODIFY:
        SettingPriModify(wParam);
        break;

    case EM_GETLINECOUNT:
        lResult = IsSingleLine() ? 1 : GetLineCount();
        break;

    case EM_LINEINDEX:
        lResult = LineIndex(wParam);
        break;

    case EM_LINELENGTH:
        lResult = LineLength(wParam);
        break;
        
    case EM_REPLACESEL:
        ReplaceSel((TMCStr)lParam, wParam, false);
        break;

    case CEM_REPLACESELHTML:
        ReplaceSel((TMCStr)lParam, wParam, true);
//        InsertHtml (this, (TMCStr)lParam); //, wParam);
        break;

    case CEM_GETSELHTML:
        {
            DbgAssert(wParam);
            CStrPtr* str = (CStrPtr*)wParam;
            CSel1 s1;
            if (lParam)
            {
                int nMax = GetTextLength();
                TMSelRange* rg = (TMSelRange*)lParam;
                s1 = CSel1(SelXToSel1(rg->left, nMax), SelXToSel1(rg->right, nMax));
            }
            else
                s1 = GetSel1();
            CSel0 s0 = Sel1ToSel0(s1);
            s0.Normalize();
            *str = m_textRich->GetHtml(s0.cbegin(), s0.length());
        }
        break;

    case CEM_GETCHARINFO:
        {
            DbgAssert(lParam);
            TMSel0 nPos = SelXToSel0(wParam);
            RICHCURSOR& info = *(RICHCURSOR*)lParam;
            m_textRich->GetInfoFromChar(nPos, info, info.nFlag & RCFLAG_PRI_AFTERWARP);
            CTuiPoint pt = GetViewOrg();
            info.nLineTop += pt.Y();
            info.nLineLeft += pt.X();
            if (info.ptLeftTop)
                (*info.ptLeftTop) += GetViewOrg();
        }
        break;

    case CEM_GETCHARUNIT:
        GetCharUnit(
            (ETextUnit)wParam,
            (wParam & eTextUnitLeft) ? &((TMSelRange*)lParam)->left : 0,
            (wParam & eTextUnitRight) ? &((TMSelRange*)lParam)->right : 0);
        break;

    case EM_GETLINE:
        lResult = GetLine(wParam, (TMStr)lParam, *(TMUInt16*)lParam);
        break;

    case EM_SETLIMITTEXT: 
        m_nLimitText = wParam;
        break;

    case EM_CANUNDO:
        lResult = CanUndo ();
        break;

    case EM_UNDO:
    case WM_UNDO:
        lResult = Undo();
        break;

    case EM_LINEFROMCHAR:
        lResult = LineFromChar(wParam);
        break;

    case EM_SETTABSTOPS:
        lResult = SetTabStops (wParam, (int*)lParam);
        break;

    case EM_SETPASSWORDCHAR:
        {
//             TMUInt32 dwStyle = GetStyle();
            if (!IsSingleLine()) // dwStyle & ES_MULTILINE)
                break;
            
            TMCharT c = (TMCharT)wParam;
            if (m_chPassWord == c)
                break;
            
//             TMULong dwNewStyle =
//                 c ? (dwStyle | ES_PASSWORD) : (dwStyle & ~ES_PASSWORD);
//             if (dwNewStyle != dwStyle)
//                 SetStyle (dwNewStyle);
//             
//             SetPasswordChar(c);
            m_chPassWord = c;

            SetDirtyAndInvalidate();
            SetPriUpdate();
            Notify (EN_CHANGE);
        }
        break;

    case EM_EMPTYUNDOBUFFER:
        EmptyUndoBuffer();
        break;

    case EM_GETFIRSTVISIBLELINE:
        lResult = GetFirstVisibleLine();
        break;

    case EM_SETREADONLY:
        {
            TMUInt32 dwStyle = GetStyle();
            TMULong dwNew = wParam ? (dwStyle | ES_READONLY) : (dwStyle & ~ES_READONLY);
            if (dwStyle != dwNew)
                SetStyle (dwNew);
//             EnableDisableIME (this);
        }
        lResult = true;
        break;

    case EM_GETPASSWORDCHAR:
        lResult = m_chPassWord;// GetPasswordChar();
        break;

    case EM_GETLIMITTEXT:
        lResult = m_nLimitText;
        break;

    case EM_POSFROMCHAR:
        *(TUIPoint*)wParam = PosFromChar(SelXToSel0(lParam));
        break;

    case EM_CHARFROMPOS:
        lResult = CharFromPos((short)LOWORD(lParam), (short)HIWORD(lParam), true);
        break;

//     case WM_GETDLGCODE:
//         {
//             lResult = DLGC_HASSETSEL | DLGC_WANTCHARS; // | DLGC_WANTARROWS;
//             MSG* pMsg = (MSG*)lParam;
//             if (GetStyle() & ES_MULTILINE)
//                 lResult |= DLGC_WANTALLKEYS;
//             else
//             {
//                 if (pMsg->wParam == VK_LEFT || pMsg->wParam == VK_LEFT)
// 
//             }
//         }
//         break;

    case WM_CHAR:
        OnWMChar(wParam);
        break;

    case WM_CLEAR:
        ClearSel();
        break;

    case WM_COMMAND:
        if (HIWORD(wParam) || lParam)
        {
            bHandled = false;
            break;
        }
        OnWMCommand(LOWORD(wParam));
        break;

    case WM_CONTEXTMENU:
        if (! (m_dwStyle & ES_NOPOPUPMENU))
        {
            if (lParam == KEYBOARD_MENU)
            {
                CTuiRect rc;
                PRI::tuiCaretGetInfo(rc);
                CPoint pti;
                ClientToScreen(tm_addressof(rc.BottomRight()), 1, &pti);
                OnWMContextMenu(pti.X(), pti.Y());
            }
            else
                OnWMContextMenu((short)LOWORD(lParam), (short)HIWORD(lParam));
        }
        break;

    case WM_COPY:
        Copy();
        break;
        
    case WM_CUT:
        Cut();
        break;
        
    case WM_ENABLE:
        Invalidate ();
        break;

    case WM_KEYDOWN:
        bHandled = lResult = OnWMKeyDown(wParam, lParam);
        break;

    case WM_LBUTTONDBLCLK:
        OnWMLButtonDblClk();
        break;
 
    case WM_LBUTTONDOWN:
        OnWMLButtonDown(wParam, (short)LOWORD(lParam), (short)HIWORD(lParam));
        break;

    case WM_LBUTTONUP:
        OnWMLButtonUp(wParam, (short)LOWORD(lParam), (short)HIWORD(lParam));
        break;

    case CM_MOUSESWITCH:
        Invalidate();
        bHandled = false;
        break;

    case WM_MOUSEACTIVATE:
        lResult = MA_ACTIVATE;
        break;
    case WM_RBUTTONUP:
        {
            SetFocus(eFocusReasonMouse);
            bHandled = false;
        }
        break;

    case WM_MOUSEMOVE:
        OnSetCursor();
        if (IsCapture())
            MovePosTo((short)LOWORD(lParam), (short)HIWORD(lParam), true);
        break;

    case WM_SETCURSOR:
        //SetCursor (LoadCursor(0, IDC_IBEAM));
        OnSetCursor();
        lResult = true;
        break;


    case WM_PASTE:
        Paste();
        break;

//     case WM_IME_NOTIFY:
//         if (wParam == IMN_SETOPENSTATUS)
//         {
//             return 0;
//         }
//         break;

    case WM_SETFOCUS:
        g_input.CaretCreate(this);// CARET_WIDTH, m_nLineHeightP);
        g_input.ImeEnable(this, !(GetStyle() & (ES_READONLY | WS_DISABLED)) && !IsPassword());
        Invalidate();
        Notify (EN_SETFOCUS);
        if (!(m_dwStyle & ES_NOAUTOSELECT))
            SetSel2(0, -1);
        break;

    case WM_KILLFOCUS:
        g_input.CaretDestroy(this);
        g_input.ImeEnable(this, false);
        Invalidate();
        Notify (EN_KILLFOCUS);
        break;

    case WM_STYLECHANGED:
        {
            const TMSTYLESTRUCT* pStyle = (const TMSTYLESTRUCT *)lParam;
            const TMULong dwMask = ES_UPPERCASE | ES_LOWERCASE;// | ES_MULTILINE;// | ES_PASSWORD;
            if  ((GWL_STYLE == wParam) &&
                ((pStyle->styleNew & dwMask) != (pStyle->styleOld & dwMask)))
            {
                SetDirtyAndInvalidate();
            }
        }
        break;

//     case CM_HITTEST:
//         CTuiWgt::ProcessWindowMessage(this, uMsg, wParam, lParam, lResult);
//         if (((TUIHITINFO*)lParam)->bNC)
//             break;
//         OnCMHitTest(this, wParam, lParam);
//         break;

    case WM_DESTROY:
        g_input.CaretDestroy(this);
        bHandled = false;
        break;

    case CEM_SETPROMPT:
        {
            CString str;
            if (StringMessageToString(lParam, str))
            {
                m_gxPrompt.ReCreate(str, wParam);
                Invalidate();
            }
        }
        lResult = true;
        break;

    case CM_MSGBUBBLE:
        if (IsReadOnly() && IsPriRich())
            bHandled = CWidgetHelper::OnBindModeBubble(this, (TUIMSG*)(lParam), lResult);
        else
            bHandled = false;
        break;

//     case CM_SCROLLTO:
//         if (IsStyleWordEllipsis() && IsWordWrap() && wParam)
//             *(GXDips*)wParam = 0; // always scroll to 0;
//         bHandled = false;
//         break;

    default:
        bHandled = false;
        break;
    }

    if (bHandled)
        return TMTrue;

//    CHAIN_MSG_MAP (scrollimpl)
    CHAIN_MSG_MAP (CTxtObject)

    return false;
}

// CTuiWgt
void CTuiRichBase::DoPrintClient(IGXApp* gapp, TMLParam lParam)
{
    // update
    if (IsPriUpdate())
    {
        ResetPriUpdate();
        Notify(EN_UPDATE);
        if (IsLifeInvalid())
            return;
    }
    DbgAssert(!IsLifeDestroyed());

    // dirty
    if (Is_WS_PRI_DIRTY())
    {
        m_textRich->SetPasswordChar(m_chPassWord);
        m_textRich->SetFormat(GetFormat());
        GXSize sz(GetMaxSize(false));
//        gxTextSetMaxSize(m_textRich, sz);// GXSize(nWidth, nHeight));
        m_textRich->CalcSize(sz, gapp);
        sz.OffsetWidth(GX_CARET_WIDTH);
        SendScrollRangeMessage(&GeometryNative0(sz), &GeometryNative1(sz));
        if (IsLifeInvalid())
            return;
   
        if (IsPriEnsure())
        {
            EnsureVisible();
            ResetPriEnsure();
            if (IsLifeInvalid())
                return;
        }

        Reset_WS_PRI_DIRTY();
    }
    DbgAssert(!IsLifeDestroyed());

    // render
//     CView* pView = GetView();
    {
        EButState ebs = GetButtonState();
        DbgAssert(ebs >= 0 && ebs < e_bsEnd);
        
        // render bk
        CTuiRect rcValid;
        if (m_pBk)
        {
            if (IsHighContrast())// pView->IsHighContrast())
                gapp->FillRect(m_rcClient, CAppColor::GetColorWindowBk(0)); //HC_RenderBkWnd(m_rcClient);
            else
                m_pBk->Render(ebs, 0, m_rcClient, 0, gapp);
        }
        
        // clip rect, don't draw text to margin area;
        GetPaddingRect (&rcValid);
        if (gapp->ClipAndPush(rcValid, true, eGXPropClip))
        {
            TMBool bFocus = IsFocusPrompt() ? false : IsFocus();
            if (!bFocus && !GetTextLength())
            {
                if (m_gxPrompt)
                {
                    gapp->Push(eGXPropTextColor);
                    TMColor clrNewTxt = IsHighContrast() //(eVisual & eColorVisualHighContrast)
                        ? CAppColor::GetColorEditText(e_bsDisable)// ColorFromSys(COLOR_GRAYTEXT)
                        : GetModule()->ConvertColor(GetObjectID(), GetView()->m_clrColorEditPrompt);
                    gapp->SetTextColor(clrNewTxt);
                    m_gxPrompt->SetFormat(GetPromptFormat() | DT_WORDBREAK | DT_END_ELLIPSIS | DT_EDITCONTROL);  // for prompt can show the multi-line 
                    m_gxPrompt->Paint(rcValid, 0, 0, gapp);
                    gapp->Pop();
                }
            }
            else
            {
                CPropColor clrs1[] = { m_clrTxt, m_clrTxt, m_clrTxt, m_clrTxtGray, m_clrTxtFocus };
                CPropColor clrs2[] = { m_clrTxt, m_clrTxt, m_clrTxt, GetView()->m_clrDisableText, m_clrTxtFocus };
                CPropColor* clrss[] = { clrs1, clrs2, 0 };
                RenderSetTextColor(gapp, ebs, clrss, CAppColor::GetColorEditText);
                OnWMPaint(gapp, rcValid, ebs);
            }
            
            if (!IsNoCaret() && IsFocus())
                DrawCaret(gapp);
            
            gapp->Pop(); // restore old clip
        }
    }
}

void CTuiRichBase::Reclaim()
{
    m_gxPrompt.Reclaim();
    CGXRich2::Reclaim();
}

void CTuiRichBase::OnLoadProp (IDomElement* pXml)
{
//     SetFlagTextChanged();
    Set_WS_PRI_DIRTY();
    CTuiWgt::OnLoadProp (pXml);
}

// TUISize CTuiRichBase::GetClientSize()
// {
//     TUIRect rc;
//     GetPaddingRect (&rc);
//     TUISize sz = {rc.right - rc.left, rc.bottom - rc.top};
//     return sz;
// }

TMBool CTuiRichBase::OnBindModeKeyDown(int key, TMLParam lParam)
{
    if (key != VK_TAB)
        return false;
    
    if (IsFocus()) // focus on hyper link
    {
        TMBool b = m_subFocus.OnTabKey(this, m_textRich->GetLinks(0, 0), CKeyInput(key, lParam));
        if (!b)
            return false;
    }

//     TMBool bReverse = OSGetKeyState(VK_SHIFT) & 0x8000;
//     HTUI hWnd = CTuiWidget(this).GetNextDlgTabItem(bReverse);
//     if (IsChild(hWnd))
// 
// 
//     if (hWnd)
//         hWnd->SetFocus();
    return false;
}

int CTuiRichBase::GetFirstVisibleLine ()
{
    if (IsSingleLine())
        return CharFromPos(m_rcClient.Left() + m_bdPadding.left, m_rcClient.Top(), true);// GetScrollLeftByES(GetStyle());
    else
        return LineFromPos(GetScrollTop());
}

////////////////////////////////////////////////////////////////////////////////
// CTuiRichBase
TM_REGISTER_CLASS2(CTuiRichImpl, TM_CS(TUI_CLASS_RICH));

CTuiRichImpl::CTuiRichImpl()
{
    m_dwStyle |= (ES_NOAUTOSELECT | ES_WANTTAB);
    SetPriRich();
    SetAutoHScroll();
    SetAutoVScroll();
}


//////////////////////////////////////////////////////////////////////////////
TM_REGISTER_CLASS2(CTuiEditImpl, TM_CS(TUI_CLASS_EDIT));

CTuiEditImpl::CTuiEditImpl()
{
//    SetEdit();
    m_textRich->SetFormat(DT_SINGLELINE | DT_NOPREFIX);
}

////////////////////////////////////////////////////////////////////////////////
// CTuiRichBase
TM_REGISTER_CLASS2(CTuiMemoImpl, TM_CS(TUI_CLASS_MEMO));

CTuiMemoImpl::CTuiMemoImpl ()
{
    m_dwStyle |= (ES_NOAUTOSELECT | ES_WANTTAB);
//     SetMemo();
    SetAutoHScroll();
    SetAutoVScroll();
}



TM_END_NAMESPACE
