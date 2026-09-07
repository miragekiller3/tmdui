/**
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.08.25
 *
 *  Copyright (C) 2009-2018 miragekiller
 */

#pragma once

#ifndef ACCEDIT_H
#define ACCEDIT_H

#include "Proxy.h"


///////////////////////////////////////////////////////////////////////////////
// CAccEdit
class CAccEdit : public CAccessibleStyle
{
public:
//     virtual long _get_accItemCount(CAccessibleWnd& sender);
//     virtual long _get_accItem(CAccessibleWnd& sender, long nIndex, IDispatch** ppRet);  // return -1: succeeded; else return _get_accItemCount();
    
//     virtual TM::HTUI _get_accParent(CAccessibleWnd& sender);
    virtual HRESULT _get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name) tm_override;
    virtual HRESULT _get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val) tm_override;
    virtual HRESULT _get_accDescription(CAccessibleWnd& sender, long nIndex, CComBSTR& description) { description = L""; return S_OK; }
    virtual long    _get_accRole(HTUI hWgt, long nIndex) tm_override { return ROLE_SYSTEM_TEXT; }
    virtual HRESULT _get_accState(CAccessibleWnd& sender, long nIndex, long& state) tm_override;
//     virtual HRESULT _get_accKeyboardShortcut(CAccessibleWnd& sender, long nIndex, CComBSTR& szKeyboardShortcut);
//     virtual HRESULT _get_accFocus(CAccessibleWnd& sender, VARIANT* pvarFocusChild);
    virtual HRESULT _get_accSelection(CAccessibleWnd& sender, long& children) { children = 0; return S_OK; } // return E_NOTIMPL; }
    virtual HRESULT _get_accDefaultAction(CAccessibleWnd& sender, long nIndex, CComBSTR& szDefaultAction) { szDefaultAction = L"activate"; return S_OK; }

//     virtual HRESULT _accSelect(CAccessibleWnd& sender, long nIndex, long flagsSelect);
//     virtual HRESULT _accItemLocation(CAccessibleWnd& sender, long nIndex, TM::CTuiRect& rc) { return E_NOTIMPL; }
//     virtual HRESULT _accNavigate(CAccessibleWnd& sender, long navDir, long varStart, VARIANT* pvarEndUpAt);
//     virtual void    _accItemHitTest(TM::CTuiPoint& pt, long& child) {}
//     virtual HRESULT _accDoDefaultAction(CAccessibleWnd& sender, long nIndex);

    virtual HRESULT _QueryService(CAccessibleWnd& sender, REFIID riid, void** ppvObject);
    virtual int _get_acc2States(CAccessibleWnd& sender) tm_override;
};

extern CAccEdit g_accEdit;


class CAccessible2TextEdit : public CAccessible2AggBaseT<CAccessible2TextEdit, CAccessible2TextT<CAccessible2TextEdit> >
{
    // IAccessibleText
//     STDMETHODIMP addSelection(long startOffset, long endOffset) tm_override;
    STDMETHODIMP get_attributes(long offset, long *startOffset, long *endOffset, BSTR *textAttributes) tm_override;
    STDMETHODIMP get_caretOffset(long *offset) tm_override;
    STDMETHODIMP get_characterExtents(long offset, enum IA2CoordinateType coordType, long *x, long *y, long *width, long *height) tm_override;
    STDMETHODIMP get_nSelections(long *nSelections) tm_override;
    STDMETHODIMP get_offsetAtPoint(long x, long y, enum IA2CoordinateType coordType, long *offset) tm_override;
    STDMETHODIMP get_selection(long selectionIndex, long *startOffset, long *endOffset) tm_override;
    STDMETHODIMP get_text(long startOffset, long endOffset, BSTR *text) tm_override;
//     STDMETHODIMP get_textBeforeOffset(long offset, enum IA2TextBoundaryType boundaryType, long *startOffset, long *endOffset, BSTR *text) tm_override;
//     STDMETHODIMP get_textAfterOffset(long offset, enum IA2TextBoundaryType boundaryType, long *startOffset, long *endOffset, BSTR *text) tm_override;
    STDMETHODIMP get_textAtOffset(long offset, enum IA2TextBoundaryType boundaryType, long *startOffset, long *endOffset, BSTR *text) tm_override;
//     STDMETHODIMP removeSelection(long selectionIndex) tm_override;
//     STDMETHODIMP setCaretOffset(long offset) tm_override;
//     STDMETHODIMP setSelection(long selectionIndex, long startOffset, long endOffset) tm_override;
    STDMETHODIMP get_nCharacters(long *nCharacters) tm_override;
//     STDMETHODIMP scrollSubstringTo(long startIndex, long endIndex, enum IA2ScrollType scrollType) tm_override;
//     STDMETHODIMP scrollSubstringToPoint(long startIndex, long endIndex, enum IA2CoordinateType coordinateType, long x, long y) tm_override;
//     STDMETHODIMP get_newText(IA2TextSegment *newText) tm_override;
//     STDMETHODIMP get_oldText(IA2TextSegment *oldText) tm_override;
};



#endif