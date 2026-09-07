/**
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.08.25
 *
 *  Copyright (C) 2018 miragekiller
 */


#ifndef PROXY_H
#define PROXY_H

#include "stdafx.h"
#include <oleacc.h>
//#include "AccSpeak.h"
#include "Helper.h"
#include "Hash.h"

#define ACC_EXPORT_ID_DESC_PRODUCTION 1
//#define ACC_EXPORT_ID_DESC_TA   1
extern bool s_bInLresultFromObject;

///////////////////////////////////////////////////////////////////////////////
// IAccessibleStyle
class IAccessibleStyle : public IUnknown
{
public:
    virtual long _get_accIndex() = 0;
    virtual long _get_accCtrlCount(CAccessibleWnd& sender) = 0; // NC object return 0; other return child window counts.
    virtual long _get_accItemCount(CAccessibleWnd& sender) = 0; // return sub item counts.
    virtual long _get_accItem(CAccessibleWnd& sender, long nIndex, IDispatch** ppRet) = 0; // ppRet used by bind window, combox. return -1 or nIndex - _get_accItemCount().

    virtual HTUI    _get_accParent(CAccessibleWnd& sender) = 0;
    virtual HRESULT _get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name) = 0;
    virtual HRESULT _get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val) = 0;
    virtual HRESULT _get_accDescription(CAccessibleWnd& sender, long nIndex, CComBSTR& description) = 0;
    virtual long    _get_accRole(TM::HTUI hWgt, long nIndex) = 0;
    virtual HRESULT _get_accState(CAccessibleWnd& sender, long nIndex, long& state) = 0;
    virtual HRESULT _get_accHelp(CAccessibleWnd& sender, long nIndex, CComBSTR& helpString) = 0;
    //     virtual HRESULT _get_accHelpTopic(TM::HTUI hWnd, int nIndex, BSTR* helpFile, long& idTopic) { return E_NOTIMPL; }
    virtual HRESULT _get_accKeyboardShortcut(CAccessibleWnd& sender, long nIndex, CComBSTR& szKeyboardShortcut) = 0;
    virtual HRESULT _get_accFocus(CAccessibleWnd& sender, VARIANT* pvarFocusChild) = 0;
    virtual HRESULT _get_accSelection(CAccessibleWnd& sender, long& children) = 0;
    virtual HRESULT _get_accDefaultAction(CAccessibleWnd& sender, long nIndex, CComBSTR& szDefaultAction) = 0;

    virtual HRESULT _accSelect(CAccessibleWnd& sender, long nIndex, long flagsSelect) = 0;
    virtual HRESULT _accItemLocation(CAccessibleWnd& sender, long nIndex, TM::CTuiRect& rc) = 0;
    virtual HRESULT _accNavigate(CAccessibleWnd& sender, long navDir, long varStart, VARIANT* pvarEndUpAt) = 0;
    virtual HRESULT _accItemHitTest(CAccessibleWnd& sender, TM::CTuiPoint& pt, long& child) = 0;
    virtual HRESULT _accDoDefaultAction(CAccessibleWnd& sender, long nIndex) = 0;
    //     virtual HRESULT _put_accName(CAccessibleWrapper& sender, int nIndex, BSTR szName) = 0;
    //     virtual HRESULT _put_accValue(CAccessibleWrapper& sender, int nIndex, BSTR szValue) { return E_NOTIMPL; }

//     virtual HRESULT _get_accEnum(CAccessibleWnd& sender, IEnumVARIANT** ppEnum) { return E_NOTIMPL; }
//     virtual TMULong CopyTo(CAccessibleWnd& sender, int nCurPos, VARIANT* pVars, TMULong cVar) { return 0; }
    virtual HRESULT _QueryService(CAccessibleWnd& sender, REFIID riid, void** ppvObject) { return E_NOINTERFACE; }
    virtual int _get_acc2States(CAccessibleWnd& sender) { return 0; }
};

///////////////////////////////////////////////////////////////////////////////
// CAccessibleStyle
class CAccessibleWrapper;
class TM_NO_VTABLE CAccessibleStyle : public IAccessibleStyle
{
public:
    TM_UNUSED_IC_GetDebugInfo();

    TM_BGN_INTERFACE_MAP(CAccessibleStyle)
    TM_END_INTERFACE_MAP()

    TM_UNUSED_ADDREF_RELEASE();

    virtual long _get_accIndex() tm_override { return 0; }
    virtual long _get_accCtrlCount(CAccessibleWnd& sender) tm_override;
    virtual long _get_accItemCount(CAccessibleWnd& sender) tm_override { return 0; }  // child item count; list, tree
    virtual long _get_accItem(CAccessibleWnd& sender, long nIndex, IDispatch** ppRet) tm_override;  // return -1: succeeded; else return _get_accItemCount();

    virtual HTUI    _get_accParent(CAccessibleWnd& sender) tm_override { return sender.GetParent(); }
    virtual HRESULT _get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name) tm_override;
    virtual HRESULT _get_accValue(CAccessibleWnd& sender, long nIndex, CComBSTR& val) tm_override { return DISP_E_MEMBERNOTFOUND; }
    virtual HRESULT _get_accDescription(CAccessibleWnd& sender, long nIndex, CComBSTR& description) tm_override { return S_FALSE; }
    virtual long    _get_accRole(HTUI hWgt, long nIndex) tm_override 
    {
 //       return ROLE_SYSTEM_GROUPING;
//        return ROLE_SYSTEM_CLIENT; 
        return ROLE_SYSTEM_PANE; 
    }
    virtual HRESULT _get_accState(CAccessibleWnd& sender, long nIndex, long& state) tm_override;
    virtual HRESULT _get_accHelp(CAccessibleWnd& sender, long nIndex, CComBSTR& helpString) tm_override;
//     virtual HRESULT _get_accHelp(VARIANT varChild, BSTR* pszHelp) { return E_NOTIMPL; }
//     virtual HRESULT _get_accHelpTopic(HTUI hWnd, int nIndex, BSTR* helpFile, long& idTopic) { return E_NOTIMPL; }
    virtual HRESULT _get_accKeyboardShortcut(CAccessibleWnd& sender, long nIndex, CComBSTR& szKeyboardShortcut) tm_override { return DISP_E_MEMBERNOTFOUND; }
    virtual HRESULT _get_accFocus(CAccessibleWnd& sender, VARIANT* pvarFocusChild) tm_override;
    virtual HRESULT _get_accSelection(CAccessibleWnd& sender, long& children) tm_override { return E_NOTIMPL; }
    virtual HRESULT _get_accDefaultAction(CAccessibleWnd& sender, long nIndex, CComBSTR& szDefaultAction) tm_override { return E_NOTIMPL; }
    
    virtual HRESULT _accSelect(CAccessibleWnd& sender, long nIndex, long flagsSelect) tm_override;
    virtual HRESULT _accItemLocation(CAccessibleWnd& sender, long nIndex, TM::CTuiRect& rc) tm_override { return S_OK; }
    virtual HRESULT _accNavigate(CAccessibleWnd& sender, long navDir, long varStart, VARIANT* pvarEndUpAt) tm_override;
    virtual HRESULT _accItemHitTest(CAccessibleWnd& sender, TM::CTuiPoint& pt, long& child) tm_override { return S_OK; }
    virtual HRESULT _accDoDefaultAction(CAccessibleWnd& sender, long nIndex) tm_override { return E_NOTIMPL; }
//     virtual HRESULT _put_accName(CAccessibleWnd& sender, int nIndex, BSTR szName) = 0;
//     virtual HRESULT _put_accValue(CAccessibleWnd& sender, int nIndex, BSTR szValue) { return E_NOTIMPL; }

    virtual HRESULT _QueryService(CAccessibleWnd& sender, REFIID riid, void** ppvObject) tm_override;
//     virtual int _get_acc2States(CAccessibleWnd& sender) tm_override { return 0; }
};

///////////////////////////////////////////////////////////////////////////////
// CAccessibleStyleButtonBase
class TM_NO_VTABLE CAccessibleStyleButtonBase : public CAccessibleStyle
{
public:
    virtual HRESULT _get_accName(CAccessibleWnd& sender, long nIndex, CComBSTR& name);
    virtual HRESULT _get_accState(CAccessibleWnd& sender, long nIndex, long& state);
//     virtual HRESULT _get_accKeyboardShortcut(CAccessibleWnd& sender, long nIndex, CComBSTR& szKeyboardShortcut);
    virtual HRESULT _accDoDefaultAction(CAccessibleWnd& sender, long nIndex);
};

///////////////////////////////////////////////////////////////////////////////
// CAccessibleStyleButtonBase
class TM_NO_VTABLE CAccessibleStyleItemBase : public CAccessibleStyle
{
protected:
    long m_sel;
    CAccessibleStyle* m_style;

public:
    void SelSel(CAccessibleStyle* style, long n) { m_style = style; m_sel = n; }

public:
    // IAccessibleStyle
    virtual long _get_accIndex() tm_final { return m_sel; }
    virtual HTUI _get_accParent(CAccessibleWnd& sender) tm_final { return sender; }
    virtual HRESULT _get_accFocus(CAccessibleWnd& sender, VARIANT* pvarFocusChild) tm_final
    {
        pvarFocusChild->vt = VT_I4;
        pvarFocusChild->lVal = CHILDID_SELF;
        return S_OK;
    }
    virtual HRESULT _get_accSelection(CAccessibleWnd& sender, long& children) tm_final
    {
        children = 0;
        return S_OK;
    }
};

///////////////////////////////////////////////////////////////////////////////
// CAccessibleWnd
// for GetAccNameOrValue
enum EGetNOV
{
    eGetNull            = 0,
    eGetPrevStatic      = 0x02,
    eGetAccText         = 0x04,
    eGetHint            = 0x08,
    eGetWindowText      = 0x10,
    eGetCustom          = eGetAccText, // get text by IAccessible::put_accName
};
TM_ENUM_2_FLAG(EGetNOV);

enum ECombTest
{
    eCombNull           = 0,
    eCombEdit           = 0x01,
    eCombButton         = 0x02,
    eCombList           = 0x04,
    eCombAll            = 0xff,
};
TM_ENUM_2_FLAG(ECombTest);


class CAccessibleObj_wrap;

class CAccessibleObj
    : public CAccessibleWnd
    , public CAccessible2T<CAccessibleObj> // IDispatchImpl<IAccessible, &IID_IAccessible, &LIBID_Accessibility>
//    , public CAccessible2_2T<CAccessibleObj>
    , public IOleWindow // for win32 SDK: WindowFromAccessibleObject.
    , public IServiceProvider
//     , public IAccessibleText
    , public CAccessible2ApplicationT<CAccessibleObj>
    //, public IAccIdentity
    //, public ISupportErrorInfo
{
public:
    TM::CRefPtr<IAccessibleStyle>    m_ptr;
    CAccessibleObj_wrap*                m_wrap;
    AccHash32                           m_nHash;

    TM_UNUSED_IC_GetDebugInfo();
    TM_UNUSED_ADDREF_RELEASE();

public:
    CAccessibleObj() : m_wrap(0), m_nHash(0) {}
    ~CAccessibleObj();
    CAccessibleWnd& GetOwner() { return *this; }
    IAccessibleStyle* GetStyle() { DbgAssert(this->m_ptr); return this->m_ptr; }
    IUnknown* GetUnknown() { return (IAccessible*)this; }
    // IUnknown
    STDMETHODIMP QueryInterface(REFIID iid, void** ppvObject) tm_override
    {
        return _InternalQueryInterface(iid, ppvObject);
    }

    // IAccessible
    STDMETHODIMP get_accParent(IDispatch** ppdispParent) tm_override;
    STDMETHODIMP get_accChildCount(long* pChildCount) tm_override;
    STDMETHODIMP get_accChild(VARIANT varChild, IDispatch** ppdispChild) tm_override;
    
    STDMETHODIMP get_accName(VARIANT varChild, BSTR* pszName) tm_override;
    STDMETHODIMP get_accValue(VARIANT varChild, BSTR* pszValue) tm_override;
    STDMETHODIMP get_accDescription(VARIANT varChild, BSTR* pszDescription) tm_override;
    STDMETHODIMP get_accRole(VARIANT varChild, VARIANT *pvarRole) tm_override;
    STDMETHODIMP get_accState(VARIANT varChild, VARIANT *pvarState) tm_override;
    STDMETHODIMP get_accHelp(VARIANT varChild, BSTR* pszHelp) tm_override;// { return E_NOTIMPL; }
    STDMETHODIMP get_accHelpTopic(BSTR* pszHelpFile, VARIANT varChild, long* pidTopic) tm_override { return S_FALSE; }
    STDMETHODIMP get_accKeyboardShortcut(VARIANT varChild, BSTR* pszKeyboardShortcut) tm_override;
    STDMETHODIMP get_accFocus(VARIANT* pvarFocusChild) tm_override;
    STDMETHODIMP get_accSelection(VARIANT* pvarSelectedChildren) tm_override;
    STDMETHODIMP get_accDefaultAction(VARIANT varChild, BSTR* pszDefaultAction) tm_override;
    
    STDMETHODIMP accSelect(long flagsSelect, VARIANT varChild) tm_override;
    STDMETHODIMP accLocation(long* pxLeft, long* pyTop, long* pcxWidth, long* pcyHeight, VARIANT varChild) tm_override;
    STDMETHODIMP accNavigate(long navDir, VARIANT varStart, VARIANT* pvarEndUpAt) tm_override;
    STDMETHODIMP accHitTest(long xLeft, long yTop, VARIANT* pvarChildAtPoint) tm_override;
    STDMETHODIMP accDoDefaultAction(VARIANT varChild) tm_override;
    
    STDMETHODIMP put_accName(VARIANT varChild, BSTR szName) tm_override;
    STDMETHODIMP put_accValue(VARIANT varChild, BSTR pszValue) tm_override;

    // IOleWindow
    STDMETHODIMP GetWindow(HWND* phwnd) tm_override;
    STDMETHODIMP ContextSensitiveHelp(BOOL fEnterMode) { return E_NOTIMPL; }

    // IServiceProvider
    STDMETHODIMP QueryService(REFGUID guidService, REFIID riid, void **ppvObject) tm_override;

    //// IAccIdentity
    //STDMETHODIMP GetIdentityString(DWORD dwIDChild, BYTE** ppIDString,DWORD* pdwIDStringLen) tm_final;

    //// ISupportErrorInfo
    //STDMETHODIMP InterfaceSupportsErrorInfo(REFIID riid) tm_final;

    // IAccessible2
//     STDMETHODIMP get_states(AccessibleStates *states) tm_override;
//     STDMETHODIMP get_attributes(BSTR *attributes) tm_override
//     {
//         *attributes = SysAllocString(L"text-model:a1;");//text-input-type:text;margin-left:0px;text-align:start;text-indent:0px;margin-right:0px;tag:input;line-number:1;margin-top:0px;margin-bottom:0px;display:inline-block;");
//         ACC_TRACE(PX() + L"get_attributes() return:%s; %s", *attributes, _DINFO());
//         return S_OK;
//         return E_NOTIMPL;
//     }



    // IEnumVARIANT
//     virtual STDMETHODIMP Clone(IEnumVARIANT **ppEnum) tm_override
//     {
//         DbgAssert(0);
//         return E_NOTIMPL;
//     }
//     
//     virtual STDMETHODIMP Next(TMULong cPins, VARIANT* ppPins, TMULong *pcFetched) tm_override
//     {
//         ACC_TRACE(L"CAccessibleObj::Next()");
//         TMULong nFetched = 0;
//         if (cPins >= 1)
//         {
//             nFetched = m_ptr->CopyTo(GetOwner(), m_nPos, ppPins, cPins);
//             m_nPos += nFetched;
//         }
//         
//         if (pcFetched)
//             *pcFetched = nFetched;
//         return (nFetched < cPins) ? S_FALSE : S_OK;
//     }
//     
//     virtual STDMETHODIMP Skip(TMULong cPins) tm_override
//     {
//         ACC_TRACE(L"CAccessibleObj::Skip()");
//         TMULong nFetched = m_ptr->CopyTo(GetOwner(), m_nPos, 0, cPins);
//         return (nFetched < cPins) ? S_FALSE : S_OK;
//     }
//     
//     virtual STDMETHODIMP Reset() tm_override
//     {
//         ACC_TRACE(L"CAccessibleObj::Reset()");
//         m_nPos = 0;
//         return S_OK;
//     }

    // public functions
    HRESULT _InternalQueryInterface(REFIID iid, void ** ppvObject);

    static CAccessibleObj* MakeAccClient(HTUI hWgt); // have been add ref;
    static CAccessibleObj* GetAccProxyBy(AccHash32 hash); // have been add ref;
    static HRESULT MakeAccKeyboardShortcut(const TM::CString& str, CComBSTR& szKeyboardShortcut);
    static EGetNOV GetAccNameOrValue(HTUI hWgt, TM::CString& str, EGetNOV nMask);
    static HRESULT GetAccNameOrValue(HTUI hWgt, CComBSTR& str, EGetNOV nMask);
    static bool IsContainCombobboxBtn(CAccessibleObj& obj);
    static bool IsComboList(HTUI hWgt);
    static void VirtualMouseClick(HTUI hWgt, TM::CTuiPoint pt, TMBool bDoubleClick);
    static void GetText(HTUI hWgt, long startOffset, long endOffset, BSTR *text);
    static void CalcRect(HTUI hWgt, const TM::CTuiRect& rc, TMBool bScreen, long *x, long *y, long *width, long *height);
    static long GetRealRole(HTUI hWgt, IAccessibleStyle* style, long idChild);
    static long GetState(CAccessibleWnd& wnd);
    static HRESULT GetGroupPosition(CAccessibleWnd& wnd, IAccessibleStyle* style, long* groupLevel, long* similarItemsInGroup, long* positionInGroup);
    static bool CombUtilIsFocus(HTUI hCombUtil, ECombTest eSrc, HTUI hFocus, ECombTest eTest = eCombAll);
    bool IsMgr() { return !GetParent(); }

    template <class T> static int GetPercentageValue(T& wnd)
    {
        int nMin = 0, nMax = 0;
        wnd.GetRange(nMin, nMax);
        int nPos = wnd.GetPos();
        if (nMin == nMax)
            return 0;
        return ((nPos - nMin) * 100) / (nMax - nMin);
    }
};
typedef TM::CRefObjectT<CAccessibleObj> CRefCAccessibleObj;


///////////////////////////////////////////////////////////////////////////////
template <class T, class TBase>
class TM_NO_VTABLE CAccessible2AggBaseT : public TBase
{
protected:
    TM::CRefPtr<CRefCAccessibleObj> m_ptrOwner;
    
public:
    TM_UNUSED_IC_GetDebugInfo();

    ~CAccessible2AggBaseT()
    {
        DbgAssert(m_ptrOwner);
//         m_ptrOwner->GetUnknown()->Release();
    }

    CAccessibleObj& GetOwner() { return *m_ptrOwner; }
    IAccessibleStyle* GetStyle() { return GetOwner().m_ptr; }
    
    static TBase* _Inst(CAccessibleWnd& sender)
    {
        CAccessible2AggBaseT* p = new TM::CRefObjectT<T>(1);
        p->m_ptrOwner = static_cast<CRefCAccessibleObj*>(static_cast<CAccessibleObj*>(&sender));
        return p;
    }
};



#endif // PROXY_H
