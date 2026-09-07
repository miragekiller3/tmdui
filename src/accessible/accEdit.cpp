/**
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.08.25
 *
 *  Copyright (C) 2009-2018 miragekiller
 */


#include "stdafx.h"
#include "accEdit.h"
#include "Proxy.h"

using namespace TM;


///////////////////////////////////////////////////////////////////////////////
// CAccEdit
CAccEdit g_accEdit;

HRESULT CAccEdit::_get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name)
{
    return CAccessibleObj::GetAccNameOrValue(sender, name, eGetCustom | eGetPrevStatic | eGetAccText |eGetHint);
}

HRESULT CAccEdit::_get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val)
{
    TM::CTuiEdit wnd(sender);
    TCHAR bPassword = wnd.GetPasswordChar();
    if (bPassword)
        return E_ACCESSDENIED;

    return CAccessibleObj::GetAccNameOrValue(sender, val, eGetWindowText);
}

HRESULT CAccEdit::_get_accState(CAccessibleWnd& sender, long nIndex, long& state)
{
    HRESULT hr = CAccessibleStyle::_get_accState(sender, nIndex, state);
    if (FAILED(hr))
        return hr;
    
    TM::CTuiEdit wnd(sender);
    TMUInt32 dwStyle  = wnd.GetStyle();
    TMBool bReadOnly = (dwStyle & ES_READONLY);
    TCHAR bPassword = wnd.GetPasswordChar();
    if (bReadOnly)
        state |= STATE_SYSTEM_READONLY;
    if (bPassword)
        state |= STATE_SYSTEM_PROTECTED;
    
    return S_OK;
}

int CAccEdit::_get_acc2States(CAccessibleWnd& sender)
{
    AccessibleStates ia2_state = IA2_STATE_OPAQUE;// | IA2_STATE_ACTIVE;
    ia2_state |= IA2_STATE_EDITABLE;
    
    TM::CString str;
    sender.GetClassName(str);
    if (str == TUI_CLASS_EDIT || (str == TUI_CLASS_RICH && CTuiRich(sender).GetLineCount() < 2))
        ia2_state |= IA2_STATE_SINGLE_LINE;
    else
        ia2_state |= IA2_STATE_MULTI_LINE;
    
    ia2_state |= IA2_STATE_SELECTABLE_TEXT;
    
    return ia2_state;
}

HRESULT CAccEdit::_QueryService(CAccessibleWnd& sender, REFIID riid, void** ppvObject)
{
    if (riid == TM_UUID_OF(IAccessibleText))
    {
        *ppvObject = static_cast<IAccessibleText*>(CAccessible2TextEdit::_Inst(sender));
        return S_OK;
    }
    return E_NOINTERFACE;
}


// IAccessibleText
STDMETHODIMP CAccessible2TextEdit::get_attributes(long offset, long *startOffset, long *endOffset, BSTR *textAttributes)
{
    ACC2_CHECK_MTUI("CAccessible2TextEdit::IAccessibleText::get_attributes");
    if (!startOffset || !endOffset || !textAttributes)
        return E_INVALIDARG;
//     ACC_TRACE(L"CAccessible2Text::IAccessibleText::get_attributes(%d) return:%d,%d,%s; %s",
//         offset, *startOffset, *endOffset, *textAttributes, _DINFO());
//     return E_NOTIMPL;
    *startOffset = 0;
    *endOffset = GetOwner().GetWindowTextLength();
//    *textAttributes = SysAllocString(L"NumberingLevel:0;") ;
    *textAttributes = SysAllocString(L"text-model:a1;");//;font-size:10pt;font-style:normal;font-weight:400;background-color:rgb(255\\, 255\\, 255);color:rgb(0\\, 0\\, 0);font-family:Tahoma;text-position:baseline;");
    //*textAttributes = SysAllocString(L"text-position:baseline; language:zh; font-style:normal; font-weight:400; background-color:rgb(255, 255, 255); font-family:Microsoft YaHei; font-size:12pt; color:rgb(0, 0, 0);");

    ACC_TRACE(L"CAccessible2TextEdit::IAccessibleText::get_attributes(%d) return:%d,%d,%s; %s",
        offset, *startOffset, *endOffset, *textAttributes, _DINFO());
    return S_OK;
}

STDMETHODIMP CAccessible2TextEdit::get_caretOffset(long *offset)
{
    ACC2_CHECK_MTUI("CAccessible2TextEdit::IAccessibleText::get_caretOffset");
    TM::CTuiEdit edit(GetOwner());
    TMSel0 bgn, end;
    edit.GetSel(bgn, end);
    *offset = end;
    ACC_TRACE(L"CAccessible2TextEdit::IAccessibleText::get_caretOffset() return:%d; %s", end, _DINFO());
    return S_OK;
}

STDMETHODIMP CAccessible2TextEdit::get_characterExtents(long offset, enum IA2CoordinateType coordType, long *x, long *y, long *width, long *height)
{
    ACC2_CHECK_MTUI("CAccessible2TextEdit::IAccessibleText::get_characterExtents");
    CTuiWidget& owner = GetOwner();
    TM::CTuiEdit& edit = static_cast<TM::CTuiEdit&>(owner);

    RICHCURSOR info = { RCFLAG_ALL };
    GXPoint pt;
    info.ptLeftTop = &pt;
    edit.GetCharInfo(offset, info);
    if (!edit)
        return RPC_E_DISCONNECTED;

    GXRect rc;
    rc.SetRect(pt.X(), pt.Y(), pt.X(), pt.Y() + info.nRunHeight);
    CAccessibleObj::CalcRect(owner, rc, coordType == IA2_COORDTYPE_SCREEN_RELATIVE, x, y, width, height);
    
    ACC_TRACE(L"CAccessible2TextEdit::IAccessibleText::get_characterExtents(%d,%d) return:%d,%d,%d,%d; %s",
        offset, coordType, *x, *y, *width, *height, _DINFO());
    return S_OK;
}

STDMETHODIMP CAccessible2TextEdit::get_nSelections(long *nSelections)
{
    ACC2_CHECK_MTUI("CAccessible2TextEdit::IAccessibleText::get_nSelections");
    ACC_TRACE(L"CAccessible2TextEdit::IAccessibleText::get_nSelections() %s", _DINFO());
    *nSelections = 0;
    return S_OK;
}

STDMETHODIMP CAccessible2TextEdit::get_offsetAtPoint(long x, long y, enum IA2CoordinateType coordType, long *offset)
{
    ACC2_CHECK_MTUI("CAccessible2TextEdit::IAccessibleText::get_offsetAtPoint");
    TM::CTuiEdit edit(GetOwner());
    TM::CPoint pt(x,y);
    GXPoint ptL;
    if (coordType == IA2_COORDTYPE_SCREEN_RELATIVE)
        edit.ScreenToClient(&pt, 1, &ptL);
    else
        edit.GetDPI().DPtoLP(&pt, 1, &ptL);
    *offset = edit.CharFromPos(ptL);
    ACC_TRACE(L"CAccessible2Text::IAccessibleText::get_offsetAtPoint(%d,%d%d) return:%d; %s",
        x, y, coordType, *offset, _DINFO());
    return S_OK;
}

STDMETHODIMP CAccessible2TextEdit::get_selection(long selectionIndex, long *startOffset, long *endOffset)
{
    ACC2_CHECK_MTUI("CAccessible2Text::IAccessibleText::get_selection");
    TM::CTuiEdit edit(GetOwner());
    TMSel0 bgn, end;
    edit.GetSel(bgn, end);
    *startOffset = bgn;
    *endOffset = end;
    ACC_TRACE(L"CAccessible2TextEdit::IAccessibleText::get_selection(%d) return:%d,%d; %s",
        selectionIndex, bgn, end, _DINFO());
    return S_OK;
}

STDMETHODIMP CAccessible2TextEdit::get_text(long startOffset, long endOffset, BSTR *text)
{
    ACC2_CHECK_MTUI("CAccessible2Text::IAccessibleText::get_text");
    ACC_TRACE(L"CAccessible2Text::IAccessibleText::get_text(%d,%d) %s",
        startOffset, endOffset, _DINFO());
    CAccessibleObj::GetText(GetOwner(), startOffset, endOffset, text);
    return S_OK;
}

// STDMETHODIMP CAccessible2TextEdit::get_textBeforeOffset(long offset, enum IA2TextBoundaryType boundaryType, long *startOffset, long *endOffset, BSTR *text)
// {
//     ACC2_CHECK_MTUI("CAccessible2Text::IAccessibleText::get_textBeforeOffset");
//     ACC_TRACE(L"xxxxx  CAccessible2Text::IAccessibleText::get_textBeforeOffset(%d,%d) %s",
//         offset, boundaryType, _DINFO());
//     return E_NOTIMPL;
// }
// 
// STDMETHODIMP CAccessible2TextEdit::get_textAfterOffset(long offset, enum IA2TextBoundaryType boundaryType, long *startOffset, long *endOffset, BSTR *text)
// {
//     ACC2_CHECK_MTUI("CAccessible2Text::IAccessibleText::get_textAfterOffset");
//     ACC_TRACE(L"xxxxx  CAccessible2Text::IAccessibleText::get_textAfterOffset(%d,%d) %s",
//         offset, boundaryType, _DINFO());
//     return E_NOTIMPL;
// }

STDMETHODIMP CAccessible2TextEdit::get_textAtOffset(long offset, enum IA2TextBoundaryType boundaryType, long *startOffset, long *endOffset, BSTR *text)
{
    ACC2_CHECK_MTUI("CAccessible2Text::IAccessibleText::get_textAtOffset");

    TM::ETextUnit unit;
    switch (boundaryType)
    {
    case IA2_TEXT_BOUNDARY_WORD: unit = eTextUnitLeft | eTextUnitRight | eTextUnitWord; break;
    case IA2_TEXT_BOUNDARY_SENTENCE: unit = eTextUnitLeft | eTextUnitRight | eTextUnitSentence; break;
    case IA2_TEXT_BOUNDARY_PARAGRAPH: unit = eTextUnitLeft | eTextUnitRight | eTextUnitParagraph; break;
    case IA2_TEXT_BOUNDARY_LINE: unit = eTextUnitLeft | eTextUnitRight | eTextUnitLine; break;
    case IA2_TEXT_BOUNDARY_ALL: unit = eTextUnitLeft | eTextUnitRight | eTextUnitDocument; break;
    case IA2_TEXT_BOUNDARY_CHAR: tm_fallthrough;
    default: unit = eTextUnitRight | eTextUnitChar; break;
    }

    TM::CTuiEdit edit(GetOwner());
    TMSelRange rg = { offset, offset };
    edit.GetCharUnit(unit, rg);
    *startOffset = rg.left;
    *endOffset = rg.right;

    CAccessibleObj::GetText(GetOwner(), rg.left, rg.right, text);

    ACC_TRACE(L"CAccessible2TextEdit::IAccessibleText::get_textAtOffset(%d,%d) return:%d,%d,%s; %s",
        offset, boundaryType, rg.left, rg.right, *text, _DINFO());

    return S_OK;
}

// STDMETHODIMP CAccessible2TextEdit::removeSelection(long selectionIndex)
// {
//     ACC2_CHECK_MTUI("CAccessible2TextEdit::IAccessibleText::removeSelection");
//     ACC_TRACE(L"xxxxx  CAccessible2TextEdit::IAccessibleText::removeSelection(%d) %s",
//         selectionIndex, _DINFO());
//     return E_NOTIMPL;
// }
// 
// STDMETHODIMP CAccessible2TextEdit::setCaretOffset(long offset)
// {
//     ACC2_CHECK_MTUI("CAccessible2TextEdit::IAccessibleText::setCaretOffset");
//     ACC_TRACE(L"xxxxx  CAccessible2TextEdit::IAccessibleText::setCaretOffset(%d) %s", offset, _DINFO());
//     return E_NOTIMPL;
// }
// 
// STDMETHODIMP CAccessible2TextEdit::setSelection(long selectionIndex, long startOffset, long endOffset)
// {
//     ACC2_CHECK_MTUI("CAccessible2TextEdit::IAccessibleText::setSelection");
//     ACC_TRACE(L"xxxxx  CAccessible2TextEdit::IAccessibleText::setSelection(%d,%d,%d) %s",
//         selectionIndex, startOffset, endOffset, _DINFO());
//     return E_NOTIMPL;
// }

STDMETHODIMP CAccessible2TextEdit::get_nCharacters(long *nCharacters)
{
    ACC2_CHECK_MTUI("CAccessible2TextEdit::IAccessibleText::get_nCharacters");
    *nCharacters = GetOwner().GetWindowTextLength();
    ACC_TRACE(L"CAccessible2TextEdit::IAccessibleText::get_nCharacters() return:%d; %s",
        *nCharacters, _DINFO());
    return S_OK;
}

// STDMETHODIMP CAccessible2TextEdit::scrollSubstringTo(long startIndex, long endIndex, enum IA2ScrollType scrollType)
// {
//     ACC2_CHECK_MTUI("CAccessible2TextEdit::IAccessibleText::scrollSubstringTo");
//     ACC_TRACE(L"xxxxx  CAccessible2TextEdit::IAccessibleText::scrollSubstringTo(%d,%d,%d) %s",
//         startIndex, endIndex, scrollType, _DINFO());
//     return E_NOTIMPL;
// }
// 
// STDMETHODIMP CAccessible2TextEdit::scrollSubstringToPoint(long startIndex, long endIndex, enum IA2CoordinateType coordinateType, long x, long y)
// {
//     ACC2_CHECK_MTUI("CAccessible2TextEdit::IAccessibleText::scrollSubstringToPoint");
//     ACC_TRACE(L"xxxxx  CAccessible2TextEdit::IAccessibleText::scrollSubstringToPoint(%d,%d,%d,%d,%d) %s",
//         startIndex, endIndex, coordinateType, x, y, _DINFO());
//     return E_NOTIMPL;
// }
// 
// STDMETHODIMP CAccessible2TextEdit::get_newText(IA2TextSegment *newText)
// {
//     ACC2_CHECK_MTUI("CAccessible2TextEdit::IAccessibleText::get_newText");
//     ACC_TRACE(L"xxxxx  CAccessible2TextEdit::IAccessibleText::get_newText() %s", _DINFO());
//     return E_NOTIMPL;
// }
// 
// STDMETHODIMP CAccessible2TextEdit::get_oldText(IA2TextSegment *oldText)
// {
//     ACC2_CHECK_MTUI("CAccessible2TextEdit::IAccessibleText::get_oldText");
//     ACC_TRACE(L"xxxxx  CAccessible2TextEdit::IAccessibleText::get_oldText() %s", _DINFO());
//     return E_NOTIMPL;
// }
