/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.11.11
 *
 *  Copyright (C) 2006 miragekiller
 */

#pragma once

#ifndef MODULE_H
#define MODULE_H

#include "../tmxml.h"
#include "../tmlist.h"
#include "../tmfilename.h"
#include "../tmdom.h"
#include "../tmlocal.h"
#include "object.h"
#include "loader.h"
#include "prepic.h"


TM_BGN_NAMESPACE


class CTuiApp;
class CTuiObject;
class CView;
struct CModule;

// 0. normal
// 1. share;
// 2. sharedpi;


// class type:
// 1. normal: like win, dlg.
// 2. shared: same object id have 1 class instance, like frame.
//    shared object's IsMultiDPI can be 0 or dpi;
//    unshared object's IsMultiDPI always dpi.
// 3. static: only have 1 class instance in process, like pre object.
//
//              bShare  bMultiDPI
//  mgr         false   true
//  widget      false   true
//  frame       true    true
//  pic         true    false
//  preobj      static object, no factory info

struct CFactoryInfo
{
    struct _Class
    {
        CString         strClass;
        EObjType        eType;
        TMUInt8         bShare;
        TMUInt8         bMultiDPI;
        
        virtual CTuiObject* FCreate(CTuiObject* pParent, int nOID, CFactoryInfo* fi, int dpi) const = 0;
        virtual void FDestroy(CTuiObject*) const = 0;
        virtual void FLoadProp(CTuiObject*, const CLoadStream&) const = 0;
        virtual TMResult FDispatch(CTuiObject*, TMUInt msg, TMWParam wp, TMLParam lp) const = 0;
    };

    const _Class* m_pFactory;
//     IDomElement* m_pDomDefault;
    CModule* m_pModule;
    
    CFactoryInfo(_Class* pFactory = 0)
        : m_pFactory(pFactory)
//         , m_pDomDefault(0)
        , m_pModule(0)
    {
    }
    bool operator < (const CFactoryInfo& rhs) const { return m_pFactory->strClass < rhs.m_pFactory->strClass; }
};




#define GetPrePic(x)        ((ITuiPic*)&GetModule()->m_aPrePic.m_picDft_##x)
#define GetPrePic2(mdl,x)   ((ITuiPic*)&(mdl).m_aPrePic.m_picDft_##x)
#define GetPrePicFS(fs,x)   GetPrePic2(*fs->FSGetWin()->GetModule(),x)


////////////////////////////////////////////////////////////////////////////////
// CModule
// module must use ref:
//   module1: create object1
//   module2: use object1.
struct CModule : public CBaseRefT<CModule>
{
    typedef CMultiSet<CTuiObject*, TUIOBJID::Less>  CObjList;
    typedef CMap<int, IDomElement*>                 CXmlList;
    
    typedef CMap<int,CString>                       CStrList;
    typedef CMap<CString,CString>                   CPopList;
    
    typedef CMultiMap<int, TUIFACTORY>              CPreCallbackList;
    typedef FLATSET(CFactoryInfo, CString, .m_pFactory->strClass) CFactoryList;

    friend CBaseRef;

#ifdef TM_DBG
    int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf)
    {
        CModule* p = (CModule*)cls->m_pClass;
        if (pbuf)
            p->m_strXmlFilePath.CopyTo(pbuf);
        return
            IC_Size(p->m_aMessageLang) +
            IC_Size(p->m_aMenuItemLang) +
            IC_Size(p->m_aMenuPopLang) +
            IC_Size(p->m_aFactory) +
            IC_Size(p->m_aObjs) +
            IC_Size(p->m_aXmls) +
            IC_Size(p->m_aCallback);
    }
#endif // TM_DBG

    CView*              m_pOwner;
    TMHModule           m_hRes;
    CLocal              m_local;
    CFileName           m_strXmlFilePath;
    IColorTransform*    m_pColorTransform;

    CStrList            m_aMessageLang;
    CStrList            m_aMenuItemLang;
    CPopList            m_aMenuPopLang;
    
    CFactoryList        m_aFactory;
    CObjList            m_aObjs;
    CXmlList            m_aXmls;
    CPreCallbackList    m_aCallback;
    CPrePicList         m_aPrePic;
    CRefPtr<ITuiPic>    m_ptrFocus;

private:
    ~CModule();

public:
    CModule(CView* pOwner, TMHModule hRes);
    void TermAndRelease();

    CObjList::recorder _FindObj(TMUInt32 id, int dpi = 0);
    void EraseObj(CTuiObject* obj);

    void DispatchObject(TMUInt msg, TMWParam wp, TMLParam lp);
    void DispatchCT();
    
    CComPtr<IDomDocument> XmlToDoc(TMCStr pFile, TM::CString* pfn);

    void _LoadFile (IDomNode* tag);

    void LoadBaseXml (TMCStr pBase);
    void LoadLangXml (TMCStr pLang);

    void LoadXml(TMCStr pBase, TMCStr pLang);
    void ClearXmls();
    void SetColorTransform(IColorTransform* pct);
    EColorVisual GetVisual(); // eColorVisualTransform, eColorVisualHighContrast, eColorVisualNormal

#if TM_WIN
    void SetMenuInfo(TMHMenu hMenu, int nPos);
#endif
    
    TMColor ConvertColor(int nObjID, TMColor clr);

    CLocal GetLocal() const { return m_local; }
    CStrPtr LoadString(int nID, const CStrView* strXmlProperty);

    // pclass == 0: Create a object use xml by nIDObj, then load prop.
    // pclass == objid: create a object use xml by objid, then load prop by objid first, and load prop by nIDObj last.
    // pclass == classname: create a object by classname, and set nIDObj. don't load prop.
    CTuiObject* CreateByClass(EObjType eType, CTuiObject* pParent, const CString& pClass, int nID, int dpi = TUI_DPI_SYSTEM_DEFAULT, TMBool bForceMultiDPI = false, TMBool bForceUnshare = false);
    
    CTuiObject* InternalCreateObject(EObjType eType, CTuiObject* pParent, int nIDObj, int dpi, TMBool bForceUnshare);

    BOOL TrackPopupMenu(TMHMenu hMenu, TMUInt uFlags, int x, int y, TMHWnd hWnd)
    {
#ifdef TM_WIN
        if (GetLocal().IsRTL())
            uFlags |= 0x8000L; // TPM_LAYOUTRTL
        return ::TrackPopupMenu(hMenu, uFlags, x, y, 0, hWnd, 0);
#elif defined(TM_GTK)
        if (!hMenu || !GTK_IS_MENU(hMenu))
            return FALSE;
        
        GtkWidget* menu = GTK_WIDGET(hMenu);
        GtkWidget* parent = GTK_WIDGET(hWnd);
        
        // Show the menu and make it grab focus
        gtk_widget_show_all(menu);
        
        // Map uFlags to GdkGravity for proper menu anchor positioning
        // Determine anchor based on TPM_flags (horizontal and vertical)
        // TPM_LEFTALIGN(0x0000), TPM_CENTERALIGN(0x0004), TPM_RIGHTALIGN(0x0008)
        // TPM_TOPALIGN(0x0000), TPM_VCENTERALIGN(0x0010), TPM_BOTTOMALIGN(0x0020)
        
        GdkGravity anchor = GDK_GRAVITY_NORTH_WEST;
        
        // Check vertical alignment
        if (uFlags & 0x0020) {  // TPM_BOTTOMALIGN
            if (uFlags & 0x0008) {  // TPM_RIGHTALIGN
                anchor = GDK_GRAVITY_SOUTH_EAST;
            } else if (uFlags & 0x0004) {  // TPM_CENTERALIGN
                anchor = GDK_GRAVITY_SOUTH;
            } else {  // TPM_LEFTALIGN
                anchor = GDK_GRAVITY_SOUTH_WEST;
            }
        } else if (uFlags & 0x0010) {  // TPM_VCENTERALIGN
            if (uFlags & 0x0008) {  // TPM_RIGHTALIGN
                anchor = GDK_GRAVITY_EAST;
            } else if (uFlags & 0x0004) {  // TPM_CENTERALIGN
                anchor = GDK_GRAVITY_CENTER;
            } else {  // TPM_LEFTALIGN
                anchor = GDK_GRAVITY_WEST;
            }
        } else {  // TPM_TOPALIGN (default)
            if (uFlags & 0x0008) {  // TPM_RIGHTALIGN
                anchor = GDK_GRAVITY_NORTH_EAST;
            } else if (uFlags & 0x0004) {  // TPM_CENTERALIGN
                anchor = GDK_GRAVITY_NORTH;
            } else {  // TPM_LEFTALIGN
                anchor = GDK_GRAVITY_NORTH_WEST;
            }
        }
        
        // Popup the menu at specified coordinates
        if (parent && GTK_IS_WIDGET(parent))
        {
            GdkRectangle rect = {x, y, 1, 1};
            gtk_menu_popup_at_rect(GTK_MENU(menu), 
                                  gtk_widget_get_window(parent),
                                  &rect,
                                  anchor,
                                  GDK_GRAVITY_NORTH_WEST,
                                  NULL);
        }
        else
        {
            gtk_menu_popup_at_pointer(GTK_MENU(menu), NULL);
        }
        
        return TRUE;
#else
        return FALSE;
#endif
    }
};


////////////////////////////////////////////////////////////////////////////////
// manage class registration and object life cycle
// insert the created objects to list and remove the objects from list when destroy them.User can enumerate all of the // // registered classes and objects

class CPreMap
{
public:
//     static void RegisterPre(CTuiObject* p);
//     static CTuiObject* GetPreObject (int nID);
};

class CPfnMap
{
public:
    static TMBool RegisterPfn(CFactoryInfo::_Class* p);
};


template <class T>
struct CFactoryImpl : public CFactoryInfo::_Class
{
public:
    class _ObjImpl : public T
    {
    public:
        TM_SELF_ALLOC_F(T);

        void OnLoadProp (IDomElement* pXml)
        {
            CAutoMsgAnalysis ama("CFactoryImpl::OnLoadProp");
            T::OnLoadProp(pXml);
        }
    };

    template <class T_pfnmap>
    CFactoryImpl(const CString& str, T_pfnmap _map, bool bShared, bool bMDPI)
    {
        bShare = bShared;
        strClass = str;
        bMultiDPI = bMDPI;
        eType = (EObjType)T::eObjType;
        _map.RegisterPfn(this);
    }

    // for manager
    CFactoryImpl(const CString& str, TMBool bShared, bool bMDPI)
    {
        bShare = bShared;
        strClass = str;
        bMultiDPI = bMDPI;
        eType = (EObjType)T::eObjType;
    }

    virtual CTuiObject* FCreate(CTuiObject* pParent, int nOID, CFactoryInfo* fi, int dpi) const
    {
        _ObjImpl* p = new _ObjImpl();
        TM_IF_DBG(TMDbgClassInfo_SetID(p, nOID));
        p->m_pFactoryInfo = fi;
        p->m_dwObjID = nOID;
        p->m_nDPI = dpi;
//         p->m_pModule = &mdl;
        p->FinalConstruct(pParent);
        return p;
    }

    virtual void FDestroy(CTuiObject* p) const
    {
        DbgAssert(p);
        _ObjImpl* pt = static_cast<_ObjImpl*>(p);
        pt->FinalRelease();
        delete pt;
    }
    
    virtual void FLoadProp(CTuiObject* p, const CLoadStream& __stm) const
    {
//         CAutoMsgAnalysis ama("CFactoryImpl::LoadProp", p->GetObjectID());
        DbgAssert(p);
        T* pt = static_cast<T*>(p);
        pt->Serialize(__stm);
        pt->OnLoadProp(__stm.GetXml());
    }

    virtual TMResult FDispatch(CTuiObject* p, TMUInt msg, TMWParam wp, TMLParam lp) const
    {
        DbgAssert(p);
        T* pt = static_cast<T*>(p);
        return pt->Dispatch(msg, wp, lp);
    }
};

#define TM_REGISTER_CLASS4(classname,classnamestr,bShare,bMultiDPI) \
    static CFactoryImpl<classname> reg##classname (classnamestr,CPfnMap(),bShare,bMultiDPI);

#define TM_REGISTER_CLASS2(classname,classnamestr) \
    TM_REGISTER_CLASS4(classname,classnamestr,false,true)

#define TM_REGISTER_CLASS1(classname) \
    TM_REGISTER_CLASS2(classname##Impl,classname::GetWndClassName())



////////////////////////////////////////////////////////////////////////////
// CObjPtr
template <class T, class T_Real = T> struct CObjPtr : public CRefPtr<T>
{
public:
    inline static T_Real* CreateNative(CModule* pm, CTuiObject* pParent, int nID, int dpi, TMBool bForceUnshare = false)
    {
        DbgAssert(pm);
        CTuiObject* p = pm->InternalCreateObject((EObjType)eObjType, pParent, nID, dpi, bForceUnshare);
        DbgAssert(!p || !p->GetFactory() || (p->GetType() == eObjType));
        return static_cast<T_Real*> (p);
    }

public:
    enum { eObjType = T_Real::eObjType };

//     void GetPre(int nID)
//     {
//         T_Real* pp = static_cast<T_Real*>(CPreMap::GetPreObject (nID));
//         DbgAssert(!pp || !pp->GetFactory() || (pp->GetType() == eObjType));
//         this->Attach(pp);
//     }

    void Create(CModule& mdl, int nID, int dpi)
    {
        T_Real* pp = CreateNative(&mdl, 0, nID, dpi);
        this->Attach(pp);
    }

    void ChangeDPI(int dpi)
    {
        if (!*this)
            return;
        T* pold = this->p;
        this->p = CreateNative(pold->GetModule(), 0, pold->GetObjectID(), dpi);
        pold->Release();
    }

    TMUInt32 GetObjectID()
    {
        return *this ? (*this)->GetObjectID() : 0;
    }

    template <class _Tp>
    TMBool IsSameObject(_Tp& ptr, TMHModule hModule, int nID)
    {
        if (ptr.p)
        {
            if (ptr.p->m_dwObjID == nID)
            {
                if (!hModule || hModule == ptr.p->GetModuleHandle())
                    return true;
            }
        }
        else if (!nID)
            return true;
        return false;
    }
};




TM_END_NAMESPACE

#endif // #ifndef MODULE_H
