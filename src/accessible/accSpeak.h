/**
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2023.7.13
 *
 *  Copyright (C) 2009-2023 miragekiller
 */

#pragma once

#ifndef ACCSPEAK_H
#define ACCSPEAK_H

#include "Proxy.h"

///////////////////////////////////////////////////////////////////////////////
// CAccSpeak
class CAccSpeak
    : public IDispatchImpl<IAccessible, &IID_IAccessible, &LIBID_Accessibility>
    , public IOleWindow
    , public IServiceProvider
{
public:
    CComBSTR m_strVoice;
    
public:
    TM_UNUSED_ADDREF_RELEASE();
    
    TM_BGN_INTERFACE_MAP    (IAccessible)
        TM_INTERFACE_HANDLER(IAccessible)
        TM_INTERFACE_HANDLER(IOleWindow)
        TM_INTERFACE_HANDLER(IServiceProvider)
    TM_END_INTERFACE_MAP    ()
    
    // IAccessible
    STDMETHODIMP get_accParent(IDispatch** ppdispParent) tm_override;
    STDMETHODIMP get_accChildCount(long* pChildCount) tm_override { *pChildCount = 0; return S_FALSE; };
    STDMETHODIMP get_accChild(VARIANT varChild, IDispatch** ppdispChild) tm_override { return S_FALSE; };
    STDMETHODIMP get_accName(VARIANT varChild, BSTR* pszName) tm_override;
    
    STDMETHODIMP get_accValue(VARIANT varChild, BSTR* pszValue) tm_override;// { return S_FALSE; };
    STDMETHODIMP get_accDescription(VARIANT varChild, BSTR* pszDescription) tm_override { return S_FALSE; };
    STDMETHODIMP get_accRole(VARIANT varChild, VARIANT *pvarRole) tm_override;
    STDMETHODIMP get_accState(VARIANT varChild, VARIANT *pvarState) tm_override;
    
    STDMETHODIMP get_accHelp(VARIANT varChild, BSTR* pszHelp) tm_override { return E_NOTIMPL; }
    STDMETHODIMP get_accHelpTopic(BSTR* pszHelpFile, VARIANT varChild, long* pidTopic) tm_override { return S_FALSE; }
    STDMETHODIMP get_accKeyboardShortcut(VARIANT varChild, BSTR* pszKeyboardShortcut) tm_override { return E_NOTIMPL; }
    STDMETHODIMP get_accFocus(VARIANT* pvarFocusChild) tm_override { return S_FALSE; }
    STDMETHODIMP get_accSelection(VARIANT* pvarSelectedChildren) tm_override { return E_NOTIMPL; }
    STDMETHODIMP get_accDefaultAction(VARIANT varChild, BSTR* pszDefaultAction) tm_override { return E_NOTIMPL; }
    
    STDMETHODIMP accSelect(long flagsSelect, VARIANT varChild) tm_override { return E_NOTIMPL; }
    STDMETHODIMP accLocation(long* pxLeft, long* pyTop, long* pcxWidth, long* pcyHeight, VARIANT varChild) tm_override { return E_NOTIMPL; }
    STDMETHODIMP accNavigate(long navDir, VARIANT varStart, VARIANT* pvarEndUpAt) tm_override { return E_NOTIMPL; }
    STDMETHODIMP accHitTest(long xLeft, long yTop, VARIANT* pvarChildAtPoint) tm_override { return E_NOTIMPL; }
    STDMETHODIMP accDoDefaultAction(VARIANT varChild) tm_override { return E_NOTIMPL; }
    
    STDMETHODIMP put_accName(VARIANT varChild, BSTR szName) tm_override { return E_NOTIMPL; }
    STDMETHODIMP put_accValue(VARIANT varChild, BSTR pszValue) tm_override { return E_NOTIMPL; }
    
    // IOleWindow
    STDMETHODIMP GetWindow(HWND* phwnd) tm_override { *phwnd = NULL; return S_OK; }
    STDMETHODIMP ContextSensitiveHelp(BOOL fEnterMode) { return E_NOTIMPL; }
    
    // IServiceProvider
    STDMETHODIMP QueryService(REFGUID guidService, REFIID riid, void **ppvObject) tm_override { return E_NOTIMPL; };

    void SayString(LPCWSTR str);
};

extern CAccSpeak g_accSpeak;

#endif // ACCSPEAK_H