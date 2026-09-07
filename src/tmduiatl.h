/**
 *  @file
 *  @brief  ATL for Template Direct UI Library 
 *
 *  @par attention
 *    the class derived from C***Impl must call CHAIN_MSG_MAP(root_class)
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.12.29
 *
 *  Copyright (C) 2006 miragekiller
 */

#pragma once

#ifndef TMDUIATL_H
#define TMDUIATL_H

#include <float.h>
#include "tmdef.h"
#include "tmdui.h"
#include "tmatl.h"
#include "tmstring.h"
#include "tmsharedptr.h"


TUI_BGN_NAMESPACE

template <class T, class T_Base> class CTuiWidgetImpl;
template <class T, class T_Base> class CTuiDialogImpl;
template <class T> class CTuiPtr;


///////////////////////////////////////////////////////////////////////////////
// CTuiString
class CTuiString : public CString
{
public:
    CTuiString() {}

    explicit CTuiString(TMUInt nid, const CStrView* strXmlProperty = 0)
    {
        LoadString(nid, strXmlProperty);
    }

    explicit CTuiString(TMHModule h, TMUInt nid, const CStrView* strXmlProperty = 0)
    {
        LoadString(h, nid, strXmlProperty);
    }

    void LoadString(TMUInt nid, const CStrView* strXmlProperty = 0)
    {
        LoadString(_ModuleATL.GetResourceInstance(), nid, strXmlProperty);
    }

    void LoadString(TMHModule h, TMUInt nid, const CStrView* strXmlProperty = 0)
    {
        this->CString::operator = (_tuiApp.LoadString(h, nid, strXmlProperty));
    }

    void FormatStringV(TMHModule h, TMUInt nid, va_list va)
    {
        this->FormatV(_tuiApp.LoadString(h, nid).c_str(), va);
    }

    void FormatStringV(TMUInt nid, va_list va)
    {
        FormatStringV(_ModuleATL.GetResourceInstance(), nid, va);
    }

    void FormatString(TMHModule h, TMUInt nid, ...)
    {
        va_list va;
        va_start(va, nid);
        FormatStringV(h, nid, va);
        va_end(va);
    }

    void FormatString(TMUInt nid, ...)
    {
        va_list va;
        va_start(va, nid);
        FormatStringV(nid, va);
        va_end(va);
    }
};


///////////////////////////////////////////////////////////////////////////////
// CTuiPicPtr
struct CTuiPicPtr : public CComPtr<ITuiPic>
{
    typedef CComPtr<ITuiPic> base_class;

    CTuiPicPtr(int nID, TMHModule hm = _ModuleATL.GetResourceInstance(), int dpi = TUI_DPI_SYSTEM_DEFAULT)
        : base_class(_tuiApp.GetPic(hm, nID, dpi))
    {
    }

    CTuiPicPtr(ITuiPic* pic)
        : base_class(pic)
    {
    }

    CTuiPicPtr()
    {
    }

    void Load(int nID, TMHModule hm = _ModuleATL.GetResourceInstance(), int dpi = TUI_DPI_SYSTEM_DEFAULT)
    {
        (base_class&)(*this) = _tuiApp.GetPic(hm, nID, dpi);
    }

    void Load(int nID, int dpi, TMHModule hm = _ModuleATL.GetResourceInstance())
    {
        (base_class&)(*this) = _tuiApp.GetPic(hm, nID, dpi);
    }

    void ChangeDPI(int dpi)
    {
        if (!p)
            return;
        (base_class&)(*this) = p->GetSubPic(dpi);
    }

    using base_class::operator =;
};


////////////////////////////////////////////////////////////////////////////////
// CXXXWindowImpl::WndProc support
template <class T>
struct CDebugRef : public CAtomicInt
{
    ~CDebugRef() { DbgAssert(!Get()); }
};

enum ETuiProcFlag
{
    eTuiProcNoRefSubclass   = 0x00,
    eTuiProcRef             = 0x01,     // entry:++ref; leave:--ref
    eTuiProcSubclass        = 0x02,     // set m_hwnd on OnSubclass
    eTuiProcRefSubclass     = eTuiProcRef | eTuiProcSubclass,
};

template<int> struct _CTuiWidgetProc_Ref
{
    // if crash here, don't inherit T from _EnableSharedFromThis.
    template<class T, class V> static inline void AddRef(T*, _EnableSharedFromThis<V>* ps) { sptrAddRef(*ps); }
    template<class T, class V> static inline void Release(T*, _EnableSharedFromThis<V>* ps) { sptrRelease(*ps); }
    template<class T>          static inline void AddRef(T* p, ...) { p->AddRef(); }
    template<class T>          static inline void Release(T* p, ...) { p->Release(); }
};
template<> struct _CTuiWidgetProc_Ref<0>
{
    template<class T, class V> static inline void AddRef(T*, _EnableSharedFromThis<V>* ps) { sptrAddRef(*ps); }
    template<class T, class V> static inline void Release(T*, _EnableSharedFromThis<V>* ps) { sptrRelease(*ps); }
    template<class T>          static inline void AddRef(T*, ...) {}
    template<class T>          static inline void Release(T*, ...) {}
};

#define DECALRE_PROC_TRAITS(name, flag) \
    struct _CTuiWidgetProc_Traits_##name : public _CTuiWidgetProc_Ref<flag & eTuiProcRef>  \
    { \
        template<class T, class H> \
        static inline TMBool WindowProc(T* p, H hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID) \
        { \
            return p->ProcessWindowMessage_##name(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID); \
        } \
        template<int> class _sel; \
        template<class T> static void _OnSubclass(T*, HTUI, _sel<0>*) {} \
        template<class T> static void _OnSubclass(T* p, HTUI hWnd, _sel<1>*) { DbgAssert(hWnd ? !p->name : !!p->name); p->name = (HTUI)hWnd; } \
        template<class T> static void  OnSubclass(T* p, HTUI hWnd) { _OnSubclass(p, hWnd, (_sel<0 != (flag & eTuiProcSubclass)>*)0); } \
        TM_IF_DBG(template <class T> static CAtomicInt& GetDebugSubclassRef(T* p) { return p->m_refDbgSubclass_##name; }) \
    }

template<ETuiProcFlag eFlag = eTuiProcSubclass>
struct _CTuiWidgetProc_Traits : public _CTuiWidgetProc_Ref<eFlag & eTuiProcRef>
{
    template<class T, class H>
    static inline TMBool WindowProc(T* p, H hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)
    {
        return p->ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
    }
    template<class T, class H> static void OnSubclass(T* p, H hWnd) { p->OnSubclass(hWnd); }
    TM_IF_DBG(template<class T> static CAtomicInt& GetDebugSubclassRef(T* p) { return p->m_refDbgSubclass; })
};

template<class T, class T_hwnd, class T_traits> 
struct _CTuiWidgetProc
{
    static TMBool _WindowProc(T* p, T_hwnd hWnd, TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID)
    {
        DbgAssert(p);
        
        if (uMsg != CM_SUBCLASS)
        {
            T_traits::AddRef(p, p);
            TMBool b = T_traits::WindowProc(p, hWnd, uMsg, wParam, lParam, lResult, dwMsgMapID);
            T_traits::Release(p, p);
            return b;
        }
        
        if (p != (T*)wParam)
            return false;
        
        if (lParam)
        {
            T_traits::AddRef(p, p);
            TM_IF_DBG(int n = ++T_traits::GetDebugSubclassRef(p));
            T_traits::OnSubclass(p, (HTUI)lParam);
        }
        else
        {
            T_traits::OnSubclass(p, (HTUI)0);
            TM_IF_DBG(int n = --T_traits::GetDebugSubclassRef(p));
            DbgAssert(n >= 0);
            T_traits::Release(p, p);
        }
        return false;
    }
};


/////////////////////////////////////////////////////////////////////////////
// class CTuiPtr, auto ptr support
// T: CTuiWidgetImpl or CTuiDialogImpl
template <class T> class CTuiPtr : public CNoCopyable
{
public:
    typedef T _PtrClass;

    // nRef:
    // create class:0; create or subclass widget:+1; destroy or Unsubclass:-1;
    // entry message:+1; leave message:-1;  when ref == 0; delete this;

    template <class V>
    class CImplT : public CBaseRefT<CImplT<V>, V>, public CRefObjectRoot<1>
    {
    protected:
        typedef CBaseRefT<V>                base_class;
        typedef typename V::handle_type     handle_type;
        friend CTuiPtr<T>;
        friend base_class;

        template<class A> static void _CheckSharedFromThis(A* p, ...) {}
        template<class A, class B> static void _CheckSharedFromThis(A*, _EnableSharedFromThis<B>* ps)
        {
            // CTuiPtr not support std::enable_shared_from_this;
            // use std::make_shared to support shared_ptr
            DbgAssertS(0);
        }

    public:
        CImplT()
        {
            _CheckSharedFromThis(this, this);
        }

        ~CImplT()
        {
            DbgAssert(!this->m_hWnd);
        }

        // debug function;
        // if error count of m_nRef, you can break on _DebugAddRef and _DebugRelease.
        /*/
        AT_REFAPI AddRef()
        {
            _DebugAddRef(m_nRef);
            return base_class::AddRef();
        }

        AT_REFAPI Release()
        {
            _DebugRelease(m_nRef);
            return base_class::Release();
        }
        /*/

        //*/

        virtual CMsgDelegateT<handle_type> GetDelegate() tm_override
        {
            return CMsgDelegateT<handle_type>(this, _CTuiWidgetProc<CImpl, handle_type, _CTuiWidgetProc_Traits<eTuiProcRefSubclass> >::_WindowProc);
        }
    };

public:
    typedef CImplT<T> CImpl;
    CImpl* p;

public:
    // for CXXXImpl::RegisterAutoCreate
    static void _Creator(HTUI hWgt, void* param1, void* param2)
    {
        CImpl* p = new CImpl();
        DbgAssert(!param1);
        p->SubclassWindow(hWgt);
        p->FinalConstruct();
        p->Release();
    }

public:
    CTuiPtr() : p(0)
    {
    }

    CTuiPtr(const CTuiPtr& lp)
    {
        DbgAssert(&lp != this);
        if ((p = lp.p) != NULL)
            p->AddRef();
    }

    ~CTuiPtr()
    {
        Release();
    }

    CImpl* CreateInstance()
    {
        if (p)
            return p;
        p = new CImpl();
        p->FinalConstruct();
        return p;
    }

    operator _PtrClass* () { return p; }

    _CRefPtr_NoAddRefRelease<_PtrClass>* operator->() const
    {
        _PtrClass* _p = p;
        return (_CRefPtr_NoAddRefRelease<_PtrClass>*)(p);
    }

    TMBool IsWindow() const
    {
        return p && p->m_hWnd;
    }

    void Assign(const CTuiPtr& rhs)
    {
        if (rhs.p == p)
            return;

        CImpl* pp = p;
        p = rhs.p;
        if (p)
            p->AddRef();
        if (pp)
            CTuiWndPtr_Release(pp);
    }

    CTuiPtr& operator =(const CTuiPtr& rhs)
    {
        Assign(rhs);
        return *this;
    }

    void Release() tm_noexcept
    {
        if (p)
        {
            CImpl* pp = p;
            p = 0;
            CTuiWndPtr_Release(pp);
        }
    }

    // std interface
    void reset() tm_noexcept { Release(); }
    void swap(CTuiPtr& r) tm_noexcept { tm_swap(p, r.p); }
    T* get() const tm_noexcept { return p; }
    long use_count() const tm_noexcept { return p ? p->GetRef() : 0; }
};

template <class T> void CTuiWndPtr_Release(T*);


////////////////////////////////////////////////////////////////////////////////
// T_Base: TM::CTuiWidget, TM::CTuiEdit, TM::CTuiXXX, CWindow, CEdit,...
// T_H: HTUI, HWND
template <class T, class T_Base, class T_H>
struct TM_NO_VTABLE CTuiWidgetBaseT
    : public T_Base
{
private:
    TM_NO_COPYABLE (CTuiWidgetBaseT);

public:
    TM_SELF_ALLOC_F(T);
    TM_UNUSED_IC_GetDebugInfo();
    TM_UNUSED_ADDREF_RELEASE_NO_OVERRIDE();

    typedef CTuiPtr<T>                               CPtr;
    typedef CTuiPtr<T>                               CShared;

protected:
    typedef T_H                                         value_type;
    typedef typename CMsgDelegateT<value_type>::_PFNProc _PFNProc;

    // CAAA a;
    // CAAA b;
    // a.Create(ID1);
    // b.Create(ID1);  // not assert;
    // b.Sublclass(a); // assert;
    template <class V>
    inline void _Debug_DontUseSameClassSubclassSameWindowTwice(V* p, HTUI hWnd)
    {
// #if defined(TM_DBG) && !defined(TUI_TEST_MODE)
//         if (hWnd)
//         {
//             CStdStrA str;
//             str.Format("id:%d; HTUI:0x%X",
//                 CTuiWidget(hWnd).GetDlgCtrlObjectID(), hWnd);
//             TM_CLASS_INSTANCE_SET_INFO(p, str); //m_dbg_instanceCheck.ResetT(p, str);
// 
//             // don't use same class subclass same widget twice:
//             // CAAA a; a.Subclass(wnd); CAAA b; b.Subclass(wnd);
//             DbgAssert(!m_dbg_instanceCheck.HaveOther());
//         }
//         else
//             TM_CLASS_INSTANCE_SET_INFO(p, ""); //m_dbg_instanceCheck.ResetT(p, "");
// #endif
    }

private:
    // hide this CTuiWidget(CWindow)'s function
    void Detach();
    void Attach();
    void operator = (TMHWnd);
    void operator = (HTUI);

protected:
    CTuiWidgetBaseT()
    {
    }

    ~CTuiWidgetBaseT()
    {
        DbgAssert(!this->m_hWnd);
    }

    TUIFACTORY GetFactory()
    {
        return GetDelegate().ToFactory();
    }

    virtual CMsgDelegateT<value_type> GetDelegate()
    {
//        return *(CMsgDelegateT<value_type>*)0;
        return CMsgDelegateT<value_type>(static_cast<T*>(this), _CTuiWidgetProc<T, value_type, _CTuiWidgetProc_Traits<eTuiProcSubclass> >::_WindowProc);
    }

public:
    void FinalConstruct() {}

    /**
     * @attention
     * don't eat WM_NCDESTROY, WM_DESTROY, WM_CREATE message,
     * these msg is system used to free res.
     * user can use "CHAIN_MSG_MAP       (root_class)" to call this func.
     */
    TMBool ProcessWindowMessage(value_type hWnd, TMUInt uMsg, TMWParam wParam,
        TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID = 0)
    {
        // system will auto unSubclass
        return false;
    }

    static void RegisterAutoCreate(int nObjectID = T::IDD, TMHModule hInstance = _ModuleATL.GetResourceInstance())
    {
        TUIFACTORY f = { CPtr::_Creator };
        _tuiApp.RegisterCallbackFactory(hInstance, nObjectID, f);
    }

    friend void CTuiWndPtr_Release(typename CPtr::CImpl* p)
    {
        p->Release();
    }


// don't call T_Base::DestroyWindow direct;
// reason:
//  T_Base::DestroyWindow
//  {
//    OnSubclass(0)
//      m_hWnd = 0;
//    delete this;
//    T_Base::m_hWnd = 0; // crash when this have deleted
//  }

};


/**
 *  @class CTuiWidgetImpl
 *  @brief
 *  encapsulate widget proc of CTuiWidget<br>
 *  this class like ATL::CWindowImpl, see CWindowImpl in MSDN.
 *  @par sample:
 *  @code
    struct CMyWindow :: public CTuiWidgetImpl<CMyWindow>
    {
        BEGIN_TUIMSG_MAP(CMyWindow)
            MESSAGE_HANDLER     (WM_MOUSEMOVE,          MessageHandler)
            COMMAND_HANDLER     (IDC_EDIT1, EN_CHANGE,  CommandHandler)
            ROUTE_HANDLER       (WM_CREATE,             RouteHandle)
//            CHAIN_MSG_MAP       (root_class)
        END_TUIMSG_MAP()

        TMResult MessageHandler(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
        TMResult CommandHandler(WORD wNotifyCode, WORD wID, void* hWndCtl, TMBool& bHandled);
        TMResult RouteHandle(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    };
 *  @endcode
 */
template <class T, class T_Base = CTuiWidget>
class TM_NO_VTABLE CTuiWidgetImpl
    : public CTuiWidgetBaseT<T, T_Base, HTUI>
{
public:
    typedef CTuiWidgetBaseT<T, T_Base, HTUI>            base_class;
    typedef CTuiWidgetImpl<T, T_Base>                   root_class;
    typedef HTUI                                        handle_type;

    enum { IDD = 0 };

    // _CTuiWidgetProc
    TM_IF_DBG(CDebugRef<HTUI> m_refDbgSubclass);
    TM_IF_DBG(CAtomicInt& GetDebugSubclassRef(HTUI) { return m_refDbgSubclass; })
    void OnSubclass(HTUI hWnd)
    {
        DbgAssert(!hWnd != !this->m_hWnd);
        this->m_hWnd = hWnd;
        this->_Debug_DontUseSameClassSubclassSameWindowTwice(static_cast<T*>(this), hWnd);
    }

public:
    CTuiWidgetImpl()
    {
        // if build error here, means T is not valid parent class.
        // class CAAA : public CTuiWidgetImpl<CAAA> // success
        // class CAAA : public CTuiWidgetImpl<CBBB> // fail
         if (0) { T* p = 0; root_class* pc = p; ; }
         DbgTrace("%s::%s()", typeid(T).name(), typeid(T).name());
    }

    ~CTuiWidgetImpl()
    {
        DbgTrace("%s::~%s()", typeid(T).name(), typeid(T).name());
        if (this->m_hWnd)
            UnsubclassWindow();
        DbgAssert(!this->m_hWnd);
    }

    TMBool DestroyWindow()
    {
        TMBool b = CTuiWidget(this->m_hWnd).DestroyWindow();
        // after destroy, this maybe destroyed, so don't call DbgAssert(!m_hWnd);
        return b;
    }

    /** */
    void SubclassWindow(HTUI hWnd)
    {
        DbgAssert(!this->m_hWnd);
        DbgAssert(hWnd);
        CTuiWidget(hWnd).SubclassWindow(this->GetDelegate());
        DbgAssert(this->m_hWnd);
    }

    /** */
    void UnsubclassWindow()
    {
        DbgAssert(this->m_hWnd);
        CTuiWidget::UnsubclassWindow(this->GetDelegate());
        // after UnsubclassWindow, this maybe destroyed, so don't call DbgAssert(!m_hWnd);
    }

    // hInstance== 0: use hWndParent's instance
    /** this function will create a child window use XML res template */
    HTUI Create(
        HTUI hWndParent, void* dwInitParam = NULL,
        TMUInt idd = T::IDD, TMHModule hInstance = 0)
    {
        //return T_Base::Create(hWndParent, idd, hInstance, &this->GetFactory(), dwInitParam);
        DbgProtect(!hWndParent, 0);
        DbgAssert(!this->m_hWnd)("oid:%d", idd);
        DbgAssert(idd);
        CTuiWidget(hWndParent)._Debug_WidgetMustValidAndSameThread();
        HTUI hWnd = PRI::tuiCreateByObjectID(0, hWndParent, hInstance, idd, tm_addressof(this->GetFactory()), dwInitParam);
        DbgAssert(hWnd == this->m_hWnd);
        this->_Debug_WidgetMustBeNotMgr();
        return hWnd;
    }

    /** modeless dialogs, see CDialogImpl::Create in MSDN */
    HTUI Create(
        CTuiApp& app, TMHWnd hWndParent = 0, void* dwInitParam = NULL,
        TMUInt idd = T::IDD, TMHModule hInstance = _ModuleATL.GetResourceInstance())
    {
        DbgAssert(!this->m_hWnd);
        HTUI hWnd = app.CreateDlgEx(hInstance, idd, hWndParent, tm_addressof(this->GetFactory()), dwInitParam);
        CTuiWidget(hWnd)._Debug_WidgetMustBeMgr();
        return hWnd;
    }

    HTUI Create(
        TMHWnd hWndParent = 0, void* dwInitParam = NULL,
        TMUInt idd = T::IDD, TMHModule hInstance = _ModuleATL.GetResourceInstance())
    {
        return this->Create(_tuiApp, hWndParent, dwInitParam, idd, hInstance);
    }

    HTUI CreateByHWND(TMHWnd hWnd, void* dwInitParam = NULL,
        TMUInt idd = T::IDD, TMHModule hInstance = _ModuleATL.GetResourceInstance())
    {
        DbgAssert(!this->m_hWnd);
        HTUI hWgt = _tuiApp.CreateDlgByHWND(hInstance, idd, hWnd, tm_addressof(this->GetFactory()), dwInitParam);
        CTuiWidget(hWgt)._Debug_WidgetMustBeMgr();
        return hWgt;
    }

    int DoModal(
        CTuiApp& app, TMHWnd hWndParent = 0/*::GetActiveWindow()*/, void* dwInitParam = NULL,
        TMUInt idd = T::IDD, TMHModule hInstance = _ModuleATL.GetResourceInstance())
    {
        if (this->Create(app, hWndParent, dwInitParam, idd, hInstance))
            return CTuiWindow(this->m_hWnd).DoDialogBox(hWndParent);
        else
            return -1;
    }

    int DoModal(
        TMHWnd hWndParent = 0/*::GetActiveWindow()*/, void* dwInitParam = NULL,
        TMUInt idd = T::IDD, TMHModule hInstance = _ModuleATL.GetResourceInstance())
    {
        return this->DoModal(_tuiApp, hWndParent, dwInitParam, idd, hInstance);
    }

    /** */
//     TMResult DefWindowProc (TMUInt uMsg, TMWParam wParam, TMLParam lParam)
//     {
//         return DefSubclassProc(uMsg, wParam, lParam);
//     }
};


/**
 *  @class CTuiDialogImpl
 *  @brief
 *  generate dialog using the ID in xml configuration file
 *  this class like ATL::CDialogImpl, see CDialogImpl in MSDN.
 *  @par sample:
 *  @code
    struct CMyDlg :: public CTuiDialogImpl<CMyDlg>
    {
        enum { IDD = IDD_DLG1 };

        BEGIN_TUIMSG_MAP(CMyDlg)
            MESSAGE_HANDLER     (WM_MOUSEMOVE,          MessageHandler)
            COMMAND_HANDLER     (IDC_EDIT1, EN_CHANGE,  CommandHandler)
            ROUTE_HANDLER       (WM_CREATE,             RouteHandle)
            CHAIN_MSG_MAP       (root_class)
        END_TUIMSG_MAP()

        TMResult MessageHandler(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
        TMResult CommandHandler(WORD wNotifyCode, WORD wID, void* hWndCtl, TMBool& bHandled);
        TMResult RouteHandle(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    };

    CMyDlg dlg;
    dlg.DoModal (0);
 *  @endcode
 */
template <class T, class T_Base = CWindowAT>
class TM_NO_VTABLE CTuiDialogImpl
    : public CTuiWidgetBaseT<T, T_Base, TMHWnd>
{
    TM_NO_COPYABLE (CTuiDialogImpl);

public:
    typedef CTuiDialogImpl                              root_class;
    typedef TMHWnd                                      handle_type;

    CTuiWindow m_tuiWindow;

    // _CTuiWidgetProc
    TM_IF_DBG(CDebugRef<HTUI> m_refDbgSubclass);
    TM_IF_DBG(CAtomicInt& GetDebugSubclassRef(TMHWnd) { return m_refDbgSubclass; })
    void OnSubclass(HTUI hWnd)
    {
        DbgAssert(!hWnd != !*this);
        m_tuiWindow = hWnd;
        *((T_Base*)this) = (hWnd ? m_tuiWindow.GetHWND() : 0);
        this->_Debug_DontUseSameClassSubclassSameWindowTwice(static_cast<T*>(this), hWnd);
    }

public:
    enum { IDD = 0 };

    CTuiDialogImpl()
    {
//         TM_CLASS_INSTANCE_SET_INFO2(T, this, T::IDD);

        // if build error here, means T is not valid parent class.
        // class CAAA : public CTuiWidgetImpl<CAAA> // success
        // class CAAA : public CTuiWidgetImpl<CBBB> // fail
        if (0) { T* p = 0; root_class* pc = p; ; }
        DbgTrace("%s::%s()", typeid(T).name(), typeid(T).name());
    }

    ~CTuiDialogImpl()
    {
        DbgTrace("%s::~%s()", typeid(T).name(), typeid(T).name());
        if (this->m_hWnd)
            UnsubclassWindow();
        DbgAssert(!this->IsWindow());
    }

    TMBool DestroyWindow()
    {
        TMBool b = CWindowAT(this->m_hWnd).DestroyWindow();
        // after DestroyWindow, this maybe destroyed, so don't call DbgAssert(!m_hWnd);
        return b;
    }

    /** */
    TMBool SubclassWindow(HTUI hWnd)
    {
        DbgAssert(!this->m_hWnd);
        DbgAssert(!m_tuiWindow);
        DbgAssert(hWnd);
        return CTuiWindow(hWnd).SubclassMgr(this->GetDelegate());
    }

    void SubclassWindow(TMHWnd hWnd)
    {
        DbgAssert(CWindowAT(hWnd).IsWindow());
        HTUI hWgt = (HTUI)CWindowAT(hWnd).SendMessage(CM_GET_WIDGET, 0, 0);
        if (hWgt)
            SubclassWindow(hWgt);
    }

    /** */
    void UnsubclassWindow()
    {
        m_tuiWindow.UnsubclassMgr(this->GetDelegate());
        // after UnsubclassWindow, this maybe destroyed, so don't call DbgAssert(!m_hWnd);
    }

    TMHWnd GetHWND() const { return this->m_hWnd; }

    /**
     *  modal dialogs, see CDialogImpl::DoModal in MSDN
     *  if call "DestroyWindow" to destroy a modal dialog,
     *  the modal dialog proc will return 0;
     */
    int DoModal(
        CTuiApp& app, TMHWnd hWndParent = 0/*::GetActiveWindow()*/, void* dwInitParam = NULL,
        TMUInt idd = T::IDD, TMHModule hInstance = _ModuleATL.GetResourceInstance())
    {
        if (this->Create(app, hWndParent, dwInitParam, idd, hInstance))
            return m_tuiWindow.DoDialogBox(hWndParent);
        else
            return -1;
    }

    int DoModal(
        TMHWnd hWndParent = 0/*::GetActiveWindow()*/, void* dwInitParam = NULL,
        TMUInt idd = T::IDD, TMHModule hInstance = _ModuleATL.GetResourceInstance())
    {
        return this->DoModal(_tuiApp, hWndParent, dwInitParam, idd, hInstance);
    }

    /** modeless dialogs, see CDialogImpl::Create in MSDN */
    TMHWnd Create(
        CTuiApp& app, TMHWnd hWndParent, void* dwInitParam = NULL,
        TMUInt idd = T::IDD, TMHModule hInstance = _ModuleATL.GetResourceInstance())
    {
        DbgAssert(!this->m_hWnd);
        app.CreateDlgEx(hInstance, idd, hWndParent, tm_addressof(this->GetFactory()), dwInitParam);
        return this->m_hWnd;
    }

    TMHWnd Create(
        TMHWnd hWndParent, void* dwInitParam = NULL,
        TMUInt idd = T::IDD, TMHModule hInstance = _ModuleATL.GetResourceInstance())
    {
        return this->Create(_tuiApp, hWndParent, dwInitParam, idd, hInstance);
    }

    void CreateByHWND(TMHWnd hWnd, void* dwInitParam = NULL,
        TMUInt idd = T::IDD, TMHModule hInstance = _ModuleATL.GetResourceInstance())
    {
        _tuiApp.CreateDlgByHWND(hInstance, idd, hWnd, &this->GetFactory(), dwInitParam);
    }

    /** see CDialogImpl::EndDialog in MSDN */
    TMBool EndDialog(int nRetCode)
    {
        DbgAssert(this->IsWindow());
        return m_tuiWindow.EndDialog(nRetCode);
    }

    /** Get child widget(nCtlID), ID can be control id or object id */
    CTuiWidget Item(int nCtlID)
    {
        return m_tuiWindow.GetDlgItem(nCtlID);
    }

    /** see CDialogImpl::DefWindowProc in MSDN */
    TMResult DefWindowProc(TMUInt uMsg, TMWParam wParam, TMLParam lParam)
    {
        return m_tuiWindow.DefSubclassMgrProc(uMsg, wParam, lParam);
    }
};


///////////////////////////////////////////////////////////////////////////////
// interface functions for handling message map
// sample:
//     BEGIN_TUIMSG_MAP (CDlgMemAnalysis)
//         MESSAGE_HANDLER     (WM_INITDIALOG,                     OnInitDialog)
//         ROUTE_ID_HANDLER    (ID_OK, WM_PRINTCLIENT,             OnPrintClient)
//         COMMAND_ID_HANDLER  (IDOK,                              OnOk)
//         CMNOTIFY_HANDLER    (IDC_LISTBOX_30395, HDN_ITEMCLICK,  OnHeaderClicked)
//     END_MSG_MAP()
//     TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
//     TMResult OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled);
//     TMResult OnHeaderClicked(int idCtrl, TM::NMBASE* pnmh, TMBool& bHandled);
#define BEGIN_TUIMSG_MAP(theClass) \
public: \
    TMBool ProcessWindowMessage(TM::HTUI hWnd, TMUInt uMsg, TMWParam wParam, \
        TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID = 0) \
    { \
        TMBool bHandled = TMTrue; \
        hWnd; \
        uMsg; \
        wParam; \
        lParam; \
        lResult; \
        bHandled; \
        switch (dwMsgMapID) \
        { \
        case 0:

// flag: ETuiProcFlag
#define BEGIN_TUIMSG_MAP_H(name,flag) \
public: \
    DECALRE_PROC_TRAITS(name,flag); \
    friend _CTuiWidgetProc_Traits_##name; \
    TM_IF_DBG(CDebugRef<_CTuiWidgetProc_Traits_##name> m_refDbgSubclass_##name); \
    TMBool Subclass_##name(TM::HTUI wnd) { return TM::CTuiWidget(wnd).SubclassWindow(this, _CTuiWidgetProc_Traits_##name()); } \
    TMBool Unsubclass_##name(TM::HTUI wnd) { return TM::CTuiWidget(wnd).UnsubclassWindow(this, _CTuiWidgetProc_Traits_##name()); } \
    TMBool ProcessWindowMessage_##name(TM::HTUI hWnd, TMUInt uMsg, TMWParam wParam, \
        TMLParam lParam, TMResult& lResult, TMULong dwMsgMapID = 0) \
    { \
        TMBool bHandled = TMTrue; \
        uMsg; \
        wParam; \
        lParam; \
        lResult; \
        bHandled; \
        switch (dwMsgMapID) \
        { \
        case 0:


#define END_TUIMSG_MAP END_MSG_MAP

///////////////////////////////////////////////////////////////////////////////
// ROUTE_HANDLER
#define ROUTE_HANDLER_EXP(exp, func, expmsg) \
    if (uMsg == CM_MSGROUTE && (exp)) \
    { \
        TMBool& bhandle = *(TMBool*)wParam; \
        bhandle = TMTrue; \
        lResult = func(((TM::TUIMSG*)lParam)->uID, (TM::TUIMSG*)lParam, bhandle); \
        if (expmsg == WM_PRINTCLIENT) \
            return lResult = bhandle, true; \
        if (bhandle) \
            return TMTrue; \
    }
//((TM::TUIMSG*)lParam)->lResult = lResult;

#define ROUTE_HANDLER(msg, func) \
    ROUTE_HANDLER_EXP (((TM::TUIMSG*)lParam)->message == (msg), func, msg)

#define ROUTE_RANGE_HANDLER(msgFirst, msgLast, func) \
    ROUTE_HANDLER_EXP (((TM::TUIMSG*)lParam)->message >= (msgFirst) && ((TM::TUIMSG*)lParam)->message <= (msgLast), func, 0)

#define ROUTE_RANGE_ID_HANDLER(id, msgFirst, msgLast, func) \
    ROUTE_HANDLER_EXP (((TM::TUIMSG*)lParam)->message >= (msgFirst) && ((TM::TUIMSG*)lParam)->message <= (msgLast) && (id) == ((TM::TUIMSG*)lParam)->uID, func, 0)

#define ROUTE_RANGE_OID_HANDLER(id, msgFirst, msgLast, func) \
    ROUTE_HANDLER_EXP (((TM::TUIMSG*)lParam)->message >= (msgFirst) && ((TM::TUIMSG*)lParam)->message <= (msgLast) && (id) == ((TM::TUIMSG*)lParam)->uObjectID, func, 0)

#define ROUTE_ID_HANDLER(id, msg, func) \
    ROUTE_HANDLER_EXP (((TM::TUIMSG*)lParam)->message == (msg) && (id) == ((TM::TUIMSG*)lParam)->uID, func, msg)

#define ROUTE_OID_HANDLER(oid, msg, func) \
    ROUTE_HANDLER_EXP (((TM::TUIMSG*)lParam)->message == (msg) && (oid) == ((TM::TUIMSG*)lParam)->uObjectID, func, msg)

#define ROUTE_ID_RANGE_HANDLER(idFirst, idLast, msg, func) \
    ROUTE_HANDLER_EXP (((TM::TUIMSG*)lParam)->message == (msg) && ((TM::TUIMSG*)lParam)->uID >= (idFirst) && ((TM::TUIMSG*)lParam)->uID <= (idLast), func, 0)

#define ROUTE_OID_RANGE_HANDLER(oidFirst, oidLast, msg, func) \
    ROUTE_HANDLER_EXP (((TM::TUIMSG*)lParam)->message == (msg) && ((TM::TUIMSG*)lParam)->uObjectID >= (oidFirst) && ((TM::TUIMSG*)lParam)->uObjectID <= (oidLast), func, 0)

#define ROUTE_TAG_HANDLER(tag, msg, func) \
    ROUTE_HANDLER_EXP (((TM::TUIMSG*)lParam)->message == (msg) && (((TM::TUIMSG*)lParam)->uTag) == tag, func, msg)

#define ROUTE_TAG_RANGE_HANDLER(TagFirst, TagLast, msg, func) \
    ROUTE_HANDLER_EXP (((TM::TUIMSG*)lParam)->message == (msg) && ((TM::TUIMSG*)lParam)->uTag >= (TagFirst) && ((TM::TUIMSG*)lParam)->uTag <= (TagLast), func, 0)

//TMResult RouteHandler(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);

///////////////////////////////////////////////////////////////////////////////
// CMNOTIFY_HANDLER
#define CMNOTIFY_HANDLER_EXP(func, exp) \
    if (uMsg == CM_NOTIFY && (exp)) \
    { \
        bHandled = true; \
        lResult = func((int)wParam, (TM::NMBASE*)lParam, bHandled); \
        if(bHandled) \
            return true; \
    }

#define CMNOTIFY_HANDLER(id, cd, func) \
    CMNOTIFY_HANDLER_EXP(func, id == ((TM::NMBASE*)lParam)->idFrom && cd == ((TM::NMBASE*)lParam)->code)
    
#define CMNOTIFY_ID_HANDLER(id, func) \
    CMNOTIFY_HANDLER_EXP(func, id == ((TM::NMBASE*)lParam)->idFrom)

#define CMNOTIFY_OID_HANDLER(oid, func) \
    CMNOTIFY_HANDLER_EXP(func, oid == ((TM::NMBASE*)lParam)->oidFrom)

#define CMNOTIFY_CODE_HANDLER(cd, func) \
    CMNOTIFY_HANDLER_EXP(func, cd == ((TM::NMBASE*)lParam)->code)

#define CMNOTIFY_RANGE_HANDLER(idFirst, idLast, func) \
    CMNOTIFY_HANDLER_EXP(func, ((TM::NMBASE*)lParam)->idFrom >= idFirst && ((TM::NMBASE*)lParam)->idFrom <= idLast)


////////////////////////////////////////////////////////////////////////////////
// DDX impl
struct TUI_DDX_LOAD
{
    void DDX_Text(TM::CTuiWidget win, int nID, TMStr p, TMULong dwLen)
    { win = win.GetDlgItem(nID); win.GetWindowText(p, dwLen); }

    void DDX_Text(TM::CTuiWidget win, int nID, CString& str, TMULong dwLen)
    {
        win = win.GetDlgItem(nID);
        if (win)
            win.GetWindowText(str);
    }

#if defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)
    void DDX_Text(TM::CTuiWidget win, int nID, _CSTRING_NS::CString& str, TMULong dwLen)
    {
        win =  win.GetDlgItem(nID);
        int n = win.GetWindowTextLength();
        win.GetWindowText(str.GetBufferSetLength(n), n);
    }
#endif

#if defined(TM_WIN_DESKTOP) && defined(__ATLBASE_H__)
    void DDX_Text(TM::CTuiWidget win, int nID, CComBSTR& str, TMULong dwLen)
    {
        win = win.GetDlgItem(nID);
        int n = win.GetWindowTextLength();
        TMStr p = (TMStr)alloca(sizeof(TMCharT) * (n + 1));
        win.GetWindowText(p, n + 1);
        str = p;
    }
#endif

    template<class T> void DDX_Int(TM::CTuiWidget win, int nID, T& n)
    { n = win.GetDlgItemInt(nID); }

    template<class T> void DDX_UInt(TM::CTuiWidget win, int nID, T& n)
    { n = win.GetDlgItemInt (nID, 0, false); }

    void DDX_Float(TM::CTuiWidget win, int nID, float& n, int np)
    { double x = n; TMCharT c[32]; DDX_Text(win, nID, c, 32); StrScanDouble(c, tm_countof(c), x); n = x; }

    template<class T> void DDX_Check (TM::CTuiWidget win, TMUInt nID, T& nValue)
    { nValue = (T) CTuiButton(win.GetDlgItem(nID)).GetCheck(); }

    template<class T> void DDX_Radio(TM::CTuiWidget win, TMUInt nID, T& nValue)
    {
        CTuiRadioButton but(win.GetDlgItem(nID));
        win = but.GetCheckedButton();
        if (!win)
            win = but;
        nValue = win.GetDlgCtrlObjectID();
    }

    void DDX_LBAll(TM::CTuiWidget win, TMUInt nID, CString& nValue)
    {
        CTuiListBox lb(win.GetDlgItem(nID));
        int n = lb.GetCount();
        TMCharT c[1024];
        for (int i = 0; i < n; ++i)
        {
            lb.GetText(i, c);
            nValue.append(c);
            nValue.append(TM_T(","));
        }
    }

#ifdef TM_WIN
    void DDX_WindowRect(TM::CTuiWidget win, TMRect& rc)
    {
        WINDOWPLACEMENT wp = { sizeof(wp) };
        ::GetWindowPlacement(win.GetHWND(), &wp);
        rc.left = wp.rcNormalPosition.left;
        rc.top = wp.rcNormalPosition.top;
        rc.right = wp.rcNormalPosition.right;
        rc.bottom = wp.rcNormalPosition.bottom;
    }
#endif // TM_WIN
};

struct TUI_DDX_SAVE
{
    void DDX_Text(TM::CTuiWidget win, int nID, TMCStrA p, TMULong dwLen)
    { win.SetDlgItemText(nID, TM_A2T(p)); }

    void DDX_Text(TM::CTuiWidget win, int nID, TMCStrW p, TMULong dwLen)
    { win.SetDlgItemText(nID, TM_W2T(p)); }

    void DDX_Int(TM::CTuiWidget win, int nID, int n)
    { win.SetDlgItemInt (nID, n); }

    void DDX_UInt(TM::CTuiWidget win, int nID, TMUInt n)
    { win.SetDlgItemInt (nID, n, false); }

    void DDX_Float(TM::CTuiWidget win, int nID, float n, int np = FLT_DIG)
    { TMCharT c[32]; StrFromF(n, c, np); DDX_Text(win, nID, c, 0); }

    void DDX_Check(TM::CTuiWidget win, TMUInt nID, int nValue)
    { CTuiButton(win.GetDlgItem(nID)).SetCheck (nValue); }

    void DDX_Radio(TM::CTuiWidget win, TMUInt nID, int nValue)
    { CTuiButton(win.GetDlgItem(nValue)).SetCheck (true); }

    void DDX_LBAll(TM::CTuiWidget win, TMUInt nID, const CString& nValue)
    {
        CTuiListBox lb(win.GetDlgItem(nID));
        lb.ResetContent();
        CString& str = (TM::CString&)nValue;
        CStrPtr strItem;
        for (int n = 0; n = str.Split(n, ',', strItem);)
            lb.AddString(strItem);
    }

    void DDX_WindowRect(TM::CTuiWidget win, const TMRect& rc)
    {
        CWindowAT(win.GetHWND()).MoveWindow(&rc);
    }
};

#define BGN_TUI_DDX_MAP(thisClass,hParent) \
    virtual TMBool TuiExchangeLoad(int nID = 0) { return DoTuiExchange(TM::TUI_DDX_LOAD(), hParent, nID); } \
    virtual TMBool TuiExchangeSave(int nID = 0) { return DoTuiExchange(TM::TUI_DDX_SAVE(), hParent, nID); } \
    template <class T> TMBool DoTuiExchange(T& pd, TM::CTuiWidget wnd, int nID) {

#define BGN_TUI_DDX_MAP_EX(thisClass) \
    virtual TMBool TuiExchangeLoad(TM::HTUI hParent, int nID = 0) { return DoTuiExchange(TM::TUI_DDX_LOAD(), hParent, nID); } \
    virtual TMBool TuiExchangeSave(TM::HTUI hParent, int nID = 0) { return DoTuiExchange(TM::TUI_DDX_SAVE(), hParent, nID); } \
    template <class T> TMBool DoTuiExchange(T& pd, TM::CTuiWidget wnd, int nID) {

#define _TUI_DDX_T(id)           if (nID == id || nID == 0) pd

#define TUI_DDX_TEXT(id,var)     _TUI_DDX_T(id).DDX_Text (wnd, id, var, sizeof(var));
#define TUI_DDX_INT(id,var)      _TUI_DDX_T(id).DDX_Int (wnd, id, var);
#define TUI_DDX_UINT(id,var)     _TUI_DDX_T(id).DDX_UInt (wnd, id, var);
#define TUI_DDX_FLOAT(id,var)    _TUI_DDX_T(id).DDX_Float (wnd, id, var);
#define TUI_DDX_CHECK(id,var)    _TUI_DDX_T(id).DDX_Check (wnd, id, var);
#define TUI_DDX_RADIO(id,var)    _TUI_DDX_T(id).DDX_Radio (wnd, id, var);
#define TUI_DDX_LBALL(id,var)    _TUI_DDX_T(id).DDX_LBAll (wnd, id, var);
#define TUI_DDX_WINDOWRECT(var)  pd.DDX_WindowRect (wnd, var);
#define TUI_DDX_CHAIN_MAP(x)     x::DoTuiExchange(pd, wnd, nID);


#define END_TUI_DDX_MAP()            return TRUE; }


TUI_END_NAMESPACE


#endif // TMDUIATL_H

