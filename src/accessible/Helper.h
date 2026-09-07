/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021.4.14
 *
 *  Copyright (C) 2021 miragekiller
 */




// ;C:\Program Files\Freedom Scientific\JAWS\2021\x86\FsDomSrv.ini
// ;IAccessible2AttribMatch=0x80000 If the AttribName and AttribValue are specified, this flag will cause the rule to match in the IAccessible2 obtained from the window supports the given attribute with the indicated value.
// ; definition for IAText supporting edits
// [IAText]
// DLLName=FSDomNodeIAText
// MappingFlags=0x80000
// AttribName=text-model
// AttribValue=a1
// ; Interaction Mode flags
// ; specified by the key InteractionModeFlags in a given rule's section
// ;defines for which interaction mode the rule is valid
// ; possible values (checked as bit flags)
// ;    VirtualNavigationMode=0x1, virtual document navigation, use the dom server to interact with the application to provide a virtual rendering of the document 
// ;EnhancedTextEditMode=0x2, use the dom node directly to interact with the application for text edits
// ;ObjectBasedJAWSCursorSupport=0x4, uses An accessibility API such as MSAA or UIA to provide JAWS cursor support (only available for certain dom nodes.)
// InteractionModeFlags=0x2

#ifndef ACCIMPL_H
#define ACCIMPL_H

#include "Hash.h"
#include "JawsHook.h"

class IAccessibleStyle;
class AccessibleWrap;

#define ACC2_CHECK_MTUI(func)    FUNC_CHECK_(GetOwner(), func, RPC_E_DISCONNECTED, GetOwner())

#define ACC2_DECLARE_PFUNC(interface) \
    TM::CString PX() { return PF(L"xxxxx "); } \
    TM::CString PF(LPCTSTR pre = L"") { TM::CString str(pre); str += typeid(T).name(); str += L"::"; str += L###interface; str += L"::"; return str; }

#define ACC2_DECLARE_GETFUNC(cls,interface) \
    T* GetSelf() { return static_cast<T*>(this); } \
    CAccessibleWnd& GetOwner() { return GetSelf()->GetOwner(); } \
    IAccessibleStyle* GetStyle() { return GetSelf()->GetStyle(); } \
    TM::CString PX() { return PF(L"xxxxx "); } \
    TM::CString PF(LPCTSTR pre = L"") { TM::CString str(pre); str += typeid(T).name(); str += L"::"; str += L###interface; str += L"::"; return str; }
       


///////////////////////////////////////////////////////////////////////////////
// IAccessible
template <class T, class Interface = IAccessible, const IID* piid = &IID_IAccessible, const GUID* plibid = &LIBID_Accessibility>
class TM_NO_VTABLE CAccessibleT
    : public IDispatchImpl<Interface, piid, plibid>
{
protected:
    typedef CAccessibleT                            CAccessible;
    typedef IDispatchImpl<Interface, piid, plibid>  CDisp;

//     TM_BGN_INTERFACE_MAP        (IAccessible)
//         TM_INTERFACE_HANDLER    (IAccessible)
//         TM_INTERFACE_HANDLER    (IDispatch)
//     TM_END_INTERFACE_MAP        ()

    ACC2_DECLARE_GETFUNC(T, IDispatch);

    //IDispatchImpl
    STDMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames,
                             LCID lcid, DISPID* rgdispid) tm_override
    {
        HRESULT hr = CDisp::GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid);
        ACC_TRACE(PF() + L"GetIDsOfNames(%s); return:*d,%X %s", *rgszNames, *rgdispid, hr, _DINFO());
        return hr;
    }
    
    STDMETHODIMP Invoke(DISPID dispidMember, REFIID riid,
        LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult,
        EXCEPINFO* pexcepinfo, UINT* puArgErr) tm_override
    {
        HRESULT hr = CDisp::Invoke(dispidMember, riid, lcid,
            wFlags, pdispparams, pvarResult, pexcepinfo, puArgErr);
        ACC_TRACE(PF() + L"Invoke(%d); return:%X %s", dispidMember, hr, _DINFO());
        return hr;
    }
};

#endif // ACCIMPL_H



///////////////////////////////////////////////////////////////////////////////
// IAccessible2
#ifdef __ia2_api_all_h__
#ifndef ACC2IMPL_H
#define ACC2IMPL_H

enum ERelation
{
    eIA2_RELATION_CONTAINING_APPLICATION, //    (L"containingApplication")
    eIA2_RELATION_CONTAINING_DOCUMENT, //    (L"containingDocument")
    eIA2_RELATION_CONTAINING_TAB_PANE, //    (L"containingTabPane")
    eIA2_RELATION_CONTAINING_WINDOW, //    (L"containingWindow")
    eIA2_RELATION_CONTROLLED_BY, //    (L"controlledBy")
    eIA2_RELATION_CONTROLLER_FOR, //    (L"controllerFor")
    eIA2_RELATION_DESCRIBED_BY, //    (L"describedBy")
    eIA2_RELATION_DESCRIPTION_FOR, //    (L"descriptionFor")
    eIA2_RELATION_EMBEDDED_BY, //    (L"embeddedBy")
    eIA2_RELATION_EMBEDS, //    (L"embeds")
    eIA2_RELATION_FLOWS_FROM, //    (L"flowsFrom")
    eIA2_RELATION_FLOWS_TO, //    (L"flowsTo")
    eIA2_RELATION_LABEL_FOR, //    (L"labelFor")
    eIA2_RELATION_LABELED_BY, //    (L"labelledBy")
    eIA2_RELATION_LABELLED_BY, //    (L"labelledBy")
    eIA2_RELATION_MEMBER_OF, //    (L"memberOf")
    eIA2_RELATION_NEXT_TABBABLE, //    (L"nextTabbable")
    eIA2_RELATION_NODE_CHILD_OF, //    (L"nodeChildOf")
    eIA2_RELATION_NODE_PARENT_OF, //    (L"nodeParentOf")
    eIA2_RELATION_PARENT_WINDOW_OF, //    (L"parentWindowOf")
    eIA2_RELATION_POPUP_FOR, //    (L"popupFor")
    eIA2_RELATION_PREVIOUS_TABBABLE, //    (L"previousTabbable")
    eIA2_RELATION_SUBWINDOW_OF, //    (L"subwindowOf")
    eIA2_RELATION_DETAILS, //    (L"details")
    eIA2_RELATION_DETAILS_FOR, //    (L"detailsFor")
    eIA2_RELATION_ERROR, //    (L"error")
    eIA2_RELATION_ERROR_FOR, //    (L"errorFor")
    eIA2_RELATION_End,
};


// class CIAccessibleRelation : public TM::CBaseRefT<CIAccessibleRelation, 1, IAccessibleRelation>
// {
// protected:
//     typedef TM::CVector<TM::CRefPtr<IUnknown> > CWndList;
//     CWndList m_aWnd;
//     int m_nType;
// 
// public:
//     TM_BGN_INTERFACE_MAP        (CIAccessibleRelation)
//         TM_INTERFACE_HANDLER    (IAccessibleRelation)
//     TM_END_INTERFACE_MAP        ()
// 
//     static LPCTSTR GetRelation(int n)
//     {
//         LPCTSTR p[eIA2_RELATION_End] = 
//         {
//             IA2_RELATION_CONTAINING_APPLICATION, //    (L"containingApplica
//             IA2_RELATION_CONTAINING_DOCUMENT, //    (L"containingDocument")
//             IA2_RELATION_CONTAINING_TAB_PANE, //    (L"containingTabPane")
//             IA2_RELATION_CONTAINING_WINDOW, //    (L"containingWindow")
//             IA2_RELATION_CONTROLLED_BY, //    (L"controlledBy")
//             IA2_RELATION_CONTROLLER_FOR, //    (L"controllerFor")
//             IA2_RELATION_DESCRIBED_BY, //    (L"describedBy")
//             IA2_RELATION_DESCRIPTION_FOR, //    (L"descriptionFor")
//             IA2_RELATION_EMBEDDED_BY, //    (L"embeddedBy")
//             IA2_RELATION_EMBEDS, //    (L"embeds")
//             IA2_RELATION_FLOWS_FROM, //    (L"flowsFrom")
//             IA2_RELATION_FLOWS_TO, //    (L"flowsTo")
//             IA2_RELATION_LABEL_FOR, //    (L"labelFor")
//             IA2_RELATION_LABELED_BY, //    (L"labelledBy")
//             IA2_RELATION_LABELLED_BY, //    (L"labelledBy")
//             IA2_RELATION_MEMBER_OF, //    (L"memberOf")
//             IA2_RELATION_NEXT_TABBABLE, //    (L"nextTabbable")
//             IA2_RELATION_NODE_CHILD_OF, //    (L"nodeChildOf")
//             IA2_RELATION_NODE_PARENT_OF, //    (L"nodeParentOf")
//             IA2_RELATION_PARENT_WINDOW_OF, //    (L"parentWindowOf")
//             IA2_RELATION_POPUP_FOR, //    (L"popupFor")
//             IA2_RELATION_PREVIOUS_TABBABLE, //    (L"previousTabbable")
//             IA2_RELATION_SUBWINDOW_OF, //    (L"subwindowOf")
//             IA2_RELATION_DETAILS, //    (L"details")
//             IA2_RELATION_DETAILS_FOR, //    (L"detailsFor")
//             IA2_RELATION_ERROR, //    (L"error")
//             IA2_RELATION_ERROR_FOR, //    (L"errorFor")
//         };
//         DbgAssert(p[eIA2_RELATION_ERROR_FOR] == IA2_RELATION_ERROR_FOR);
//         DbgAssert(n >= 0 && n < eIA2_RELATION_End);
//         return p[n];
//     }
// 
//     CIAccessibleRelation(int nType)
//         : m_nType(nType)
//     {
//     }
// 
//     void Add(IUnknown* wnd)
//     {
//         m_aWnd.push_back(wnd);
//     }
// 
//     STDMETHODIMP get_relationType(BSTR *relationType)
//     {
//         *relationType = SysAllocString(GetRelation(m_nType));
//         return S_OK;
//     }
//     
//     STDMETHODIMP get_localizedRelationType(BSTR *localizedRelationType) tm_override
//     {
//         return E_NOTIMPL;
//     }
//     
//     STDMETHODIMP get_nTargets(long *nTargets)
//     {
//         DbgAssert(m_aWnd.size());
//         *nTargets = m_aWnd.size();
//         return S_OK;
//     }
//     
//     STDMETHODIMP get_target(long targetIndex, IUnknown **target) tm_override
//     {
//         DbgAssert(m_aWnd.size());
//         if (targetIndex < 0 || targetIndex >= m_aWnd.size())
//             return E_INVALIDARG;
//         *target = m_aWnd[targetIndex];
//         return S_OK;
//     }
//     
//     STDMETHODIMP get_targets(long maxTargets, IUnknown **targets, long *nTargets) tm_override
//     {
//         DbgAssert(m_aWnd.size());
//         if (maxTargets <= 0)
//             return E_INVALIDARG;
//         tm_setmin(maxTargets, m_aWnd.size());
//         for (long i = 0; i < maxTargets; ++i)
//             targets[i] = m_aWnd[i];
//         *nTargets = maxTargets;
//         return S_OK;
//     }
// };


template <class T, class Interface = IAccessible2, const IID* piid = &TM_UUID_OF(IAccessible), const GUID* plibid = &LIBID_Accessibility>
class TM_NO_VTABLE CAccessible2T
    : public CAccessibleT<T, Interface, piid, plibid>
{
public:
    typedef CAccessible2T       CAccessible2;
    typedef CAccessible2T       accessible_class;

//     TM_BGN_INTERFACE_MAP        (IAccessible)
// //         TM_INTERFACE_HANDLER    (IAccessible2)
//         TM_INTERFACE_HANDLER    (IAccessible)
//         TM_INTERFACE_HANDLER    (IDispatch)
//     TM_END_INTERFACE_MAP        ()

    ACC2_DECLARE_GETFUNC(T, IAccessible2);

    // IAccessible2
    STDMETHODIMP get_nRelations(long *nRelations) tm_override
    {
        ACC_TRACE(PX() + L"get_nRelations(%d) %s", *nRelations, _DINFO());
        return E_NOTIMPL;
//         *nRelations = 3;
//         ACC_TRACE(PF() + L"get_nRelations() return:%d; %s", *nRelations, _DINFO());
//         return S_OK;
    }
    
    STDMETHODIMP get_relation(long relationIndex, IAccessibleRelation **relation) tm_override
    {
        ACC_TRACE(PX() + L"get_relation(%d) %s", relationIndex, _DINFO());
        return E_NOTIMPL;
//         if (relationIndex < 0 || relationIndex > 2)
//             return E_INVALIDARG;
// 
//         TM::CTuiWidget root(GetOwner().GetWindow(GW_OWNER));
//         CIAccessibleRelation* p = new CIAccessibleRelation(relationIndex);
//         IUnknown* obj = CAccessibleWrapper::GetAccClient(root)->GetUnknown();
//         p->Add(obj);
//         *relation = p;
//         obj->Release();
// 
//         ACC_TRACE(PF() + L"get_relation(%d) return:%p, %s", relationIndex, *relation, _DINFO());
//         return S_OK;
    }
    
    STDMETHODIMP get_relations(long maxRelations, IAccessibleRelation **relations, long *nRelations) tm_override
    {
        ACC_TRACE(PX() + L"get_relations(%d) %s", maxRelations, _DINFO());
        return E_NOTIMPL;
//         if (maxRelations < 0)
//             return E_INVALIDARG;
//         TM::CTuiWidget root(GetOwner().GetWindow(GW_OWNER));
//         IUnknown* obj = CAccessibleWrapper::GetAccClient(root)->GetUnknown();
//         long n = tm_max(maxRelations, 3);
//         for (long i = 0; i < n; ++i)
//         {
//             CIAccessibleRelation* p = new CIAccessibleRelation(i);
//             p->Add(obj);
//             relations[i] = p;
//         }
//         *nRelations = n;
//         obj->Release();
// 
//         ACC_TRACE(PF() + L"get_relations(%d) return:%d; %s", maxRelations, n, _DINFO());
//         return S_OK;
    }
    
    STDMETHODIMP role(long *role) tm_override
    {
        ACC2_CHECK_MTUI("CAccessible2T::IAccessible2::role");
        *role = CAccessibleObj::GetRealRole(GetOwner(), GetStyle(), 0);
//        *role = GetStyle()->_get_accRole(GetOwner(), CHILDID_SELF);
        ACC_TRACE(PF() + L"role() return:%s; %s", DbgFormatRole(*role), _DINFO());
        return S_OK;
    }
    
    STDMETHODIMP scrollTo(enum IA2ScrollType scrollType) tm_override
    {
        ACC_TRACE(PX() + L"scrollTo(%d) %s", scrollType, _DINFO());
        return E_NOTIMPL;
    };
    
    STDMETHODIMP scrollToPoint(enum IA2CoordinateType coordinateType, long x, long y) tm_override
    {
        ACC_TRACE(PX() + L"scrollToPoint(%d,%d,%d) %s", coordinateType, x, y, _DINFO());
        return E_NOTIMPL;
    };
    
    STDMETHODIMP get_groupPosition(long *groupLevel, long *similarItemsInGroup, long *positionInGroup) tm_override
    {
        //have: menuitem, list, list item, outline item, pagetab, check, radio, combo,  
        //no:   titlebar, menubar, scrollbar, grip, window, client, menupop, tooltip, document, pane, chart, grouping, separator, link, properpage,

        CAccessibleWnd& wnd = GetOwner();
        IAccessibleStyle* style = GetStyle();
        HRESULT hr = CAccessibleObj::GetGroupPosition(wnd, style, groupLevel, similarItemsInGroup, positionInGroup);
        ACC_TRACE(PX() + L"get_groupPosition(%d, %d, %d) %s", *groupLevel, *similarItemsInGroup, *positionInGroup, _DINFO());
        return hr;
    };
    
    STDMETHODIMP get_states(AccessibleStates *states) tm_override
    {
//         *states = 0;
//         ACC_TRACE(PX() + L"get_states() return:%X; %s", *states, _DINFO());
//         return S_OK;
        ACC2_CHECK_MTUI("CAccessible2T::IAccessible2::get_states");
        *states = (AccessibleStates)GetStyle()->_get_acc2States(GetOwner());
        ACC_TRACE(L"CAccessibleObj::IAccessible2::get_states() return:%s; %s", DbgFormatState2(*states), _DINFO());
        return S_OK;
    };
    
    STDMETHODIMP get_extendedRole(BSTR *extendedRole) tm_override
    {
        ACC_TRACE(PX() + L"get_extendedRole() %s", _DINFO());
        return E_NOTIMPL;
    };
    
    STDMETHODIMP get_localizedExtendedRole(BSTR *localizedExtendedRole) tm_override
    {
        ACC_TRACE(PX() + L"get_localizedExtendedRole() %s", _DINFO());
        return E_NOTIMPL;
    }
    
    STDMETHODIMP get_nExtendedStates(long *nExtendedStates) tm_override
    {
        ACC_TRACE(PX() + L"get_nExtendedStates() %s", _DINFO());
        return E_NOTIMPL;
    }
    
    STDMETHODIMP get_extendedStates(long maxExtendedStates, BSTR **extendedStates, long *nExtendedStates) tm_override
    {
        ACC_TRACE(PX() + L"get_extendedStates(%d) %s", maxExtendedStates, _DINFO());
        return E_NOTIMPL;
    }
    
    STDMETHODIMP get_localizedExtendedStates(long maxLocalizedExtendedStates, BSTR **localizedExtendedStates, long *nLocalizedExtendedStates) tm_override
    {
        ACC_TRACE(PX() + L"get_localizedExtendedStates(%d) %s", maxLocalizedExtendedStates, _DINFO());
        return E_NOTIMPL;
    }
    
    STDMETHODIMP get_uniqueID(long *uniqueID) tm_override
    {
        ACC2_CHECK_MTUI("CAccessible2T::IAccessible2::get_uniqueID");
        *uniqueID = GetSelf()->m_nHash; // CHash::Find(GetOwner())->m_nHash;
        DbgAssert(*uniqueID);
        ACC_TRACE(L"get_uniqueID() return:%x; %s", *uniqueID, _DINFO());
        return S_OK;
    }
    
    STDMETHODIMP get_windowHandle(HWND *windowHandle) tm_override
    {
        ACC2_CHECK_MTUI("CAccessible2T::IAccessible2::get_windowHandle");
        *windowHandle = GetOwner().GetHWND();
        ACC_TRACE(PF() + L"get_windowHandle() return:%p; %s", *windowHandle, _DINFO());
        return S_OK;
    }
    
    STDMETHODIMP get_indexInParent(long *indexInParent) tm_override
    {
        ACC_TRACE(PX() + L"get_indexInParent() %s", _DINFO());
        return E_NOTIMPL;
    }
    
    STDMETHODIMP get_locale(IA2Locale *locale) tm_override
    {
        ACC_TRACE(PX() + L"get_locale() %s", _DINFO());
        return E_NOTIMPL;
    }
    
    // like: "class:aaa;id:123;tag:ccc"
    // live:off don't get text.
    STDMETHODIMP get_attributes(BSTR *attributes) tm_override
    {
//         *attributes = SysAllocString(L"text-model:a1;");//text-input-type:text;margin-left:0px;text-align:start;text-indent:0px;margin-right:0px;tag:input;line-number:1;margin-top:0px;margin-bottom:0px;display:inline-block;");
//         ACC_TRACE(PX() + L"get_attributes() return:%s; %s", *attributes, _DINFO());
//         return S_OK;
        return E_NOTIMPL;
    }
};


template <class T, class Interface = IAccessible2_2, const IID* piid = &TM_UUID_OF(IAccessible), const GUID* plibid = &LIBID_Accessibility>
class TM_NO_VTABLE CAccessible2_2T
    : public CAccessible2T<T, Interface, piid, plibid>
{
public:
    typedef CAccessible2_2T     CAccessible2_2;
    typedef CAccessible2_2T     accessible_class;

//     TM_BGN_INTERFACE_MAP        (IAccessible)
// //         TM_INTERFACE_HANDLER    (IAccessible2)
//         TM_INTERFACE_HANDLER    (IAccessible)
//         TM_INTERFACE_HANDLER    (IDispatch)
//     TM_END_INTERFACE_MAP        ()

    ACC2_DECLARE_GETFUNC(T, IAccessible2_2);

    // IAccessible2_2
    STDMETHODIMP get_attribute(BSTR name, VARIANT *attribute) tm_override
    {
        ACC_TRACE(PX() + L"get_attribute(%s) %s", name, _DINFO());
        return E_NOTIMPL;
    }
        
    STDMETHODIMP get_accessibleWithCaret(IUnknown **accessible, long *caretOffset) tm_override
    {
        ACC_TRACE(PX() + L"get_accessibleWithCaret() %s", _DINFO());
        return E_NOTIMPL;
    }
        
    STDMETHODIMP get_relationTargetsOfType(BSTR type, long maxTargets, IUnknown ***targets, long *nTargets) tm_override
    {
        ACC_TRACE(PX() + L"get_relationTargetsOfType(%s, %d) %s", type, maxTargets, _DINFO());
        return E_NOTIMPL;
    }
};



///////////////////////////////////////////////////////////////////////////////
// CAccessible2Application
// must provide for support JAWS.
//IAccessible2::get_appName(Firefox), ret:0
//IAccessible2::get_appVersion(128.0.3), ret : 0
//IAccessible2::get_toolkitName(Gecko), ret : 0
//IAccessible2::get_toolkitVersion(128.0.3), ret : 0
template <class T>
class TM_NO_VTABLE CAccessible2ApplicationT
    : public IAccessibleApplication
{
protected:
    ACC2_DECLARE_GETFUNC(T, IAccessibleApplication);

    // IAccessibleApplication
    virtual STDMETHODIMP get_appName(BSTR *name) tm_override
    {
        *name = CComBSTR(s_cAppName).Detach();
        ACC_TRACE(PX() + L"get_appName() return:%s; %s", *name, _DINFO());
        return S_OK; //E_NOTIMPL;
    }

    //virtual STDMETHODIMP get_appName(BSTR* name) tm_override
    //{
    //    TM::CString str;
    //    TM::CTuiWidget(GetOwner().GetWindow(GW_OWNER)).GetWindowText(str);
    //    *name = CComBSTR(str).Detach();
    //    return S_OK; //E_NOTIMPL;
    //}

    virtual STDMETHODIMP get_appVersion(BSTR *version) tm_override
    {
//        *version = CComBSTR(L"128.0.3").Detach();
        ACC_TRACE(PX() + L"get_appVersion() return:%s; %s", *version, _DINFO());
//        return S_OK; //E_NOTIMPL;
        return E_NOTIMPL;
    }

    virtual STDMETHODIMP get_toolkitName(BSTR *name) tm_override
    {
        ACC_TRACE(PX() + L"get_toolkitName() return:%s; %s", *name, _DINFO());
        return E_NOTIMPL;
    }

    virtual STDMETHODIMP get_toolkitVersion(BSTR *version) tm_override
    {
        ACC_TRACE(PX() + L"get_toolkitVersion() return:%s; %s", *version, _DINFO());
        return E_NOTIMPL;
    }
};


///////////////////////////////////////////////////////////////////////////////
// CAccessible2Text
template <class T>
class TM_NO_VTABLE CAccessible2TextT : public IAccessibleText
{
public:
    typedef CAccessible2TextT       CAccessible2Text;
    ACC2_DECLARE_GETFUNC(T, IAccessibleText);
//    ACC2_DECLARE_PFUNC(IAccessibleText);

    CAccessible2TextT()
    {
        ACC_TRACE(L"CAccessible2Text::CAccessible2Text()");
    }
    
    ~CAccessible2TextT()
    {
        ACC_TRACE(L"CAccessible2Text::~CAccessible2Text()");
    }
    
private:
    TM_BGN_INTERFACE_MAP        (IAccessibleText)
        TM_INTERFACE_HANDLER    (IAccessibleText)
    TM_END_INTERFACE_MAP        ()
        
    // IAccessibleText
    STDMETHODIMP addSelection(long startOffset, long endOffset) tm_override
    {
        ACC_TRACE(PX() + L"addSelection(%d, %d) %s", startOffset, endOffset, _DINFO());
        return E_NOTIMPL;
    }

    STDMETHODIMP get_attributes(long offset, long *startOffset, long *endOffset, BSTR *textAttributes) tm_override
    {
//        *textAttributes = CComBSTR(L"xml - roles:slider; valuetext : 82; tag:div; class :md - slider - wrapper md - focused; display:block;").Detach();
        ACC_TRACE(PX() + L"get_attributes(%d) %s", offset, _DINFO());
//        return S_OK;
        return E_NOTIMPL;
    }

    STDMETHODIMP get_caretOffset(long *offset) tm_override
    {
        ACC_TRACE(PX() + L"get_caretOffset() %s", _DINFO());
        return E_NOTIMPL;
    }

    STDMETHODIMP get_characterExtents(long offset, enum IA2CoordinateType coordType, long *x, long *y, long *width, long *height) tm_override
    {
        ACC_TRACE(PX() + L"get_characterExtents(%d, %d) %s", offset, coordType, _DINFO());
        return E_NOTIMPL;
    }

    STDMETHODIMP get_nSelections(long *nSelections) tm_override
    {
        ACC_TRACE(PF() + L"get_nSelections() %s", _DINFO());
        *nSelections = 0;
        return S_OK;
    }

    STDMETHODIMP get_offsetAtPoint(long x, long y, enum IA2CoordinateType coordType, long *offset) tm_override
    {
        ACC_TRACE(PX() + L"get_offsetAtPoint(%d, %d, %d) %s", x, y, coordType, _DINFO());
        return E_NOTIMPL;
    }

    STDMETHODIMP get_selection(long selectionIndex, long *startOffset, long *endOffset) tm_override
    {
        ACC_TRACE(PX() + L"get_selection(%d) %s", selectionIndex, _DINFO());
        return E_NOTIMPL;
    }

    STDMETHODIMP get_text(long startOffset, long endOffset, BSTR *text) tm_override
    {
        ACC2_CHECK_MTUI("CAccessible2TextT::IAccessibleText::get_text");
        ACC_TRACE(PF() + L"(%d,%d) %s", startOffset, endOffset, _DINFO());
        CAccessibleObj::GetText(GetOwner(), startOffset, endOffset, text);
        return S_OK;
    }

    STDMETHODIMP get_textBeforeOffset(long offset, enum IA2TextBoundaryType boundaryType, long *startOffset, long *endOffset, BSTR *text) tm_override
    {
        ACC_TRACE(PX() + L"get_textBeforeOffset(%d, %d) %s", offset, boundaryType, _DINFO());
        return E_NOTIMPL;
    }

    STDMETHODIMP get_textAfterOffset(long offset, enum IA2TextBoundaryType boundaryType, long *startOffset, long *endOffset, BSTR *text) tm_override
    {
        ACC_TRACE(PX() + L"get_textAfterOffset(%d, %d) %s", offset, boundaryType, _DINFO());
        return E_NOTIMPL;
    }
    
    STDMETHODIMP get_textAtOffset(long offset, enum IA2TextBoundaryType boundaryType, long *startOffset, long *endOffset, BSTR *text) tm_override
    {
        ACC_TRACE(PX() + L"get_textAtOffset(%d, %d) %s", offset, boundaryType, _DINFO());
        return E_NOTIMPL;
    }

    STDMETHODIMP removeSelection(long selectionIndex) tm_override
    {
        ACC_TRACE(PX() + L"removeSelection(%d) %s", selectionIndex, _DINFO());
        return E_NOTIMPL;
    }

    STDMETHODIMP setCaretOffset(long offset) tm_override
    {
        ACC_TRACE(PX() + L"setCaretOffset(%d) %s", offset, _DINFO());
        return E_NOTIMPL;
    }

    STDMETHODIMP setSelection(long selectionIndex, long startOffset, long endOffset) tm_override
    {
        ACC_TRACE(PX() + L"setSelection(%d, %d, %d) %s", selectionIndex, startOffset, endOffset, _DINFO());
        return E_NOTIMPL;
    }

    STDMETHODIMP get_nCharacters(long *nCharacters) tm_override
    {
        ACC2_CHECK_MTUI("CAccessible2TextT::IAccessibleText::get_nCharacters");
        *nCharacters = GetOwner().GetWindowTextLength();
        ACC_TRACE(PF() + L"get_nCharacters() return:%d; %s", *nCharacters, _DINFO());
        return S_OK;
    }

    STDMETHODIMP scrollSubstringTo(long startIndex, long endIndex, enum IA2ScrollType scrollType) tm_override
    {
        ACC_TRACE(PX() + L"scrollSubstringTo(%d, %d, %d) %s", startIndex, endIndex, scrollType, _DINFO());
        return E_NOTIMPL;
    }

    STDMETHODIMP scrollSubstringToPoint(long startIndex, long endIndex, enum IA2CoordinateType coordinateType, long x, long y) tm_override
    {
        ACC_TRACE(PX() + L"scrollSubstringToPoint(%d, %d, %d, %d, %d) %s", startIndex, endIndex, coordinateType, x, y, _DINFO());
        return E_NOTIMPL;
    }

    STDMETHODIMP get_newText(IA2TextSegment *newText) tm_override
    {
        ACC_TRACE(PX() + L"get_newText() %s", _DINFO());
        return E_NOTIMPL;
    }

    STDMETHODIMP get_oldText(IA2TextSegment *oldText) tm_override
    {
        ACC_TRACE(PX() + L"get_oldText() %s", _DINFO());
        return E_NOTIMPL;
    }
};


#endif // #ifndef ACC2IMPL_H
#endif //__ia2_api_all_h__


#ifdef __uiautomationcore_h__
#ifndef UIAIMPL_H
#define UIAIMPL_H

///////////////////////////////////////////////////////////////////////////////
// CAccessibleExT
template <class T>
class TM_NO_VTABLE CAccessibleExT : public IAccessibleEx
{
protected:
    typedef CAccessibleExT      CAccessibleEx;

    ACC2_DECLARE_GETFUNC        (T, IAccessibleEx);

    TM_BGN_INTERFACE_MAP        (IAccessibleEx)
        TM_INTERFACE_HANDLER    (IAccessibleEx)
    TM_END_INTERFACE_MAP        ()
    
    // IAccessibleEx
    STDMETHODIMP GetObjectForChild(long aIdChild, IAccessibleEx** aAccEx) tm_override
    {
        ACC_TRACE(PX() + L"GetObjectForChild(%d) %s", aIdChild, _DINFO()); 
        return  E_NOTIMPL;
    }
    
    STDMETHODIMP GetIAccessiblePair(IAccessible** aAcc, long* aIdChild) tm_override
    {
        ACC_TRACE(PX() + L"GetIAccessiblePair() %s", _DINFO()); 
        return  E_NOTIMPL;
    }
    
    STDMETHODIMP GetRuntimeId(SAFEARRAY** aRuntimeIds) tm_override
    {
        ACC_TRACE(PX() + L"GetRuntimeId() %s", _DINFO());
        return E_NOTIMPL; 
    }
    
    STDMETHODIMP ConvertReturnedElement(IRawElementProviderSimple* aRawElmProvider, IAccessibleEx** aAccEx) tm_override
    {
        ACC_TRACE(PX() + L"ConvertReturnedElement() %s", _DINFO()); 
        return E_NOTIMPL; 
    }
};

///////////////////////////////////////////////////////////////////////////////
// CRawElementProviderSimpleT
template <class T>
class TM_NO_VTABLE CRawElementProviderSimpleT : public IRawElementProviderSimple
{
protected:
    typedef CRawElementProviderSimpleT  CRawElementProviderSimple;
    
    ACC2_DECLARE_GETFUNC        (T, IRawElementProviderSimple);
    
    TM_BGN_INTERFACE_MAP        (IRawElementProviderSimple)
        TM_INTERFACE_HANDLER    (IRawElementProviderSimple)
    TM_END_INTERFACE_MAP        ()

    // IRawElementProviderSimple
    STDMETHODIMP get_ProviderOptions(enum ProviderOptions* aOptions) tm_override
    {
        ACC_TRACE(PF() + L"get_ProviderOptions() %s", _DINFO()); 
        if (!aOptions)
            return E_INVALIDARG;
        
        // This method is not used with IAccessibleEx implementations.
//        *aOptions = ProviderOptions_ServerSideProvider | ProviderOptions_HasNativeIAccessible | ProviderOptions_UseComThreading;
        *aOptions = ProviderOptions_ServerSideProvider;// | ProviderOptions_HasNativeIAccessible | ProviderOptions_UseComThreading;
        return S_OK;
    }

    STDMETHODIMP GetPatternProvider(PATTERNID aPatternId, IUnknown** aPatternProvider) tm_override
    {
        ACC_TRACE(PX() + L"GetPatternProvider() %s", _DINFO()); 
        *aPatternProvider = NULL;
        return S_OK;
    }

    STDMETHODIMP GetPropertyValue(PROPERTYID aPropertyId, VARIANT* aPropertyValue) tm_override
    {
        ACC_TRACE(PF() + L"GetPropertyValue(%s) %s",
            DbgFormatOropertyId(aPropertyId), _DINFO()); 
        aPropertyValue->vt = VT_EMPTY;
        return S_OK;
    }

    STDMETHODIMP get_HostRawElementProvider(IRawElementProviderSimple** aRawElmProvider) tm_override
    {
        ACC_TRACE(PF() + L"get_HostRawElementProvider() %s", _DINFO()); 
        if (!aRawElmProvider)
            return E_INVALIDARG;
        
        // This method is not used with IAccessibleEx implementations.
        *aRawElmProvider = 0;
        return S_OK;
    }
};

///////////////////////////////////////////////////////////////////////////////
// CRawElementProviderFragmentT
template <class T>
class TM_NO_VTABLE CRawElementProviderFragmentT : public IRawElementProviderFragment
{
protected:
    typedef CRawElementProviderFragmentT  CRawElementProviderFragment;
    
    ACC2_DECLARE_GETFUNC        (T, IRawElementProviderFragment);
    
    TM_BGN_INTERFACE_MAP        (IRawElementProviderFragment)
        TM_INTERFACE_HANDLER    (IRawElementProviderFragment)
    TM_END_INTERFACE_MAP        ()

    //IRawElementProviderFragment
    STDMETHODIMP Navigate(NavigateDirection direction, IRawElementProviderFragment** retVal) tm_override
    {
        ACC_TRACE(PF() + L"::Navigate(%d) %s", direction, _DINFO());
        *retVal = NULL;
        return S_OK;
    }

    STDMETHODIMP GetRuntimeId(SAFEARRAY** retVal) tm_override
    {
        ACC_TRACE(PX() + L"::GetRuntimeId() %s", _DINFO());
        return E_NOTIMPL;
    }

    STDMETHODIMP get_BoundingRectangle(UiaRect* retVal) tm_override
    {
        ACC2_CHECK_MTUI("CRawElmProvider::IRawElementProviderFragment::get_BoundingRectangle");
        
        TM::CTuiRect rect;
        CAccessibleWnd& wnd = GetOwner();
        wnd.GetWindowRect(&rect);
        TM::CRect rcScreen;
        wnd.ClientToScreen(rect, rcScreen);
        retVal->left = rcScreen.Left();
        retVal->top = rcScreen.Top();
        retVal->width = rcScreen.Width();
        retVal->height = rcScreen.Height();
        
        ACC_TRACE(PF(L"") + "get_BoundingRectangle(%d,%d,%d,%d) %s", rcScreen, _DINFO()); 
        return S_OK;
    }

    STDMETHODIMP GetEmbeddedFragmentRoots(SAFEARRAY** retVal) tm_override
    {
        ACC_TRACE(PX() + L"GetEmbeddedFragmentRoots() %s", _DINFO()); 
        *retVal = NULL;
        return S_OK;
    }

    STDMETHODIMP SetFocus() tm_override
    {
        ACC_TRACE(PX() + L"SetFocus() %s", _DINFO()); 
        return S_OK;
    }

    STDMETHODIMP get_FragmentRoot(IRawElementProviderFragmentRoot** retVal) tm_override
    {
        ACC_TRACE(PX() + L"get_FragmentRoot() %s", _DINFO()); 
        *retVal = 0;
        return S_OK;
    }
};


///////////////////////////////////////////////////////////////////////////////
// CRawElementProviderFragmentRootT
template <class T>
class TM_NO_VTABLE CRawElementProviderFragmentRootT : public IRawElementProviderFragmentRoot
{
protected:
    typedef CRawElementProviderFragmentRootT  CRawElementProviderFragmentRoot;
    
    ACC2_DECLARE_GETFUNC        (T, IRawElementProviderFragmentRoot);
    
    TM_BGN_INTERFACE_MAP        (IRawElementProviderFragmentRoot)
        TM_INTERFACE_HANDLER    (IRawElementProviderFragmentRoot)
    TM_END_INTERFACE_MAP        ()

    //IRawElementProviderFragmentRoot
    HRESULT STDMETHODCALLTYPE ElementProviderFromPoint(double x, double y, IRawElementProviderFragment** retVal)
    {
        ACC_TRACE(PX() + L"ElementProviderFromPoint(%g,%g) %s", x, y, _DINFO()); 
        return QueryInterface(TM_UUID_OF(IRawElementProviderFragment), (void**)retVal);
    }

    HRESULT STDMETHODCALLTYPE GetFocus(IRawElementProviderFragment** retVal)
    {
        ACC_TRACE(PX() + L"GetFocus() %s", _DINFO()); 
        *retVal = NULL;
        return S_OK;
    }
};

#endif // #ifndef UIAIMPL_H
#endif  // __uiautomationcore_h__

