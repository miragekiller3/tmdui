/**
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2023.7.13
 *
 *  Copyright (C) 2009-2023 miragekiller
 */


#include "stdafx.h"
#include "accSpeak.h"

using namespace TM;
using namespace TM::PRI;

///////////////////////////////////////////////////////////////////////////////
// CAccSpeak
CAccSpeak g_accSpeak;


// IAccessible
STDMETHODIMP CAccSpeak::get_accParent(IDispatch** ppdispParent)
{
    HWND hOwner = GetForegroundWindow();
    return AccessibleObjectFromWindow(hOwner, OBJID_WINDOW, IID_IDispatch, (void **)ppdispParent);
};

STDMETHODIMP CAccSpeak::get_accName(VARIANT varChild, BSTR* pszName)
{
    *pszName = CComBSTR(m_strVoice).Detach();
    return S_OK;
};

STDMETHODIMP CAccSpeak::get_accValue(VARIANT varChild, BSTR* pszValue)
{
    *pszValue = 0;//CComBSTR(m_strVoice).Detach();
    return S_OK;
};

STDMETHODIMP CAccSpeak::get_accRole(VARIANT varChild, VARIANT *pvarRole)
{
    pvarRole->lVal = ROLE_SYSTEM_STATICTEXT;
    pvarRole->vt   = VT_I4;
    return S_OK;
};

STDMETHODIMP CAccSpeak::get_accState(VARIANT varChild, VARIANT *pvarState)
{
    pvarState->lVal = STATE_SYSTEM_FOCUSED;
    pvarState->vt   = VT_I4;
    return S_OK;
};

void CAccSpeak::SayString(LPCWSTR str)
{
    g_accSpeak.m_strVoice = str;
//    ::NotifyWinEvent(EVENT_OBJECT_VALUECHANGE, GetForegroundWindow(), OBJID_SOUND, INDEXID_CONTAINER);
    ::NotifyWinEvent(EVENT_OBJECT_FOCUS, GetForegroundWindow(), OBJID_SOUND, CHILDID_SELF);
}
