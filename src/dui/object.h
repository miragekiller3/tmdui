/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.09.14
 *
 *  Copyright (C) 2006 miragekiller
 */

#pragma once

#ifndef OBJECT_H
#define OBJECT_H

#include "../tmgdi.h"
#include "../tmdui.h"
#include "../tmdom.h"
#include "../tmxmlstring.h"
#include "../tmdebugext.h"
#include "../design/Design.h"


class CObjData;
struct GXFont;


TM_BGN_NAMESPACE


// EditProp&SaveProp only used in edit mode,removed in Release for optimization
#ifdef TUI_EDIT_MODE

#define BGN_OBJ_PROP_MAP() \
    public: \
    void EditProp(class CEditStream& __stm)  { Serialize(__stm); } \
    void SaveProp(class CSaveStream& __stm)  { Serialize(__stm); } \
    void ResetIDProp(class CIdStream& __stm) { Serialize(__stm);} \
    template<class S> TMBool Serialize(S& __stream) \
    { \
        __stream.BeginSerialize(this); \
        TMBool b = _Serialize(__stream); \
        __stream.EndSerialize(); \
        return b; \
    } \
    template<class S> TMBool _Serialize(S& __stream) {

#else

#define BGN_OBJ_PROP_MAP() \
    public: \
    template<class S> TMBool Serialize(S& __stream) \
        { \
            __stream.BeginSerialize(this); \
            TMBool b = _Serialize(__stream); \
            __stream.EndSerialize(); \
            return b; \
        } \
    template<class S> TMBool _Serialize(S& __stream) {

#endif

// basical objects
#define OBJ_PROP_CHILDS()                   __stream.ExecChilds(this);

#define OBJ_PROP_FRAME(name, member)        __stream.ExecFrame(TM_CT(name), member);
#define OBJ_PROP_TEXT(name, member)         __stream.ExecText(TM_CT(name), member);
#define OBJ_PROP_INT(name, member)          __stream.ExecInt(TM_CT(name), member);
#define OBJ_PROP_CHAR(name, member)         __stream.ExecChar(TM_CT(name), member);
#define OBJ_PROP_ID(name, member)           __stream.ExecID(TM_CT(name), member);
#define OBJ_PROP_ENUM(name, member, type)   __stream.ExecEnum(TM_CT(name), member, (const type*)0);
#define OBJ_PROP_MASK(name, member, type)   __stream.ExecMask(TM_CT(name), member, (const type*)0);
#define OBJ_PROP_OLE(name, member)          __stream.ExecOle(TM_CT(name), member);
#define OBJ_PROP_FILE(name, member)         __stream.ExecFile(TM_CT(name), member);
#define OBJ_PROP_CLASS(name, member)        __stream.ExecClass(TM_CT(name), member);
// #define OBJ_PROP_REF(name, member)          __stream.ExecRef(TM_CT(name), member);
#define OBJ_PROP_PIC(name, member)          __stream.ExecPic (TM_CT(name), member);
#define OBJ_PROP_DLG(name, member)          __stream.ExecDlg (TM_CT(name), member);
#define OBJ_PROP_WIN(name, member)          __stream.ExecWin (TM_CT(name), member);

#define CHAIN_OBJ_PROP(x)                   x::_Serialize(__stream);
#define CHAIN_OBJ_PROP_MEMBER(x)            x._Serialize(__stream);

#define END_OBJ_PROP_MAP()                  return false; }



struct CModule;
struct CFactoryInfo;
class  CTuiObject;
class  CLoadStream;
class  CTuiWgt;
class  CTuiWnd;
class  CView;


inline CString _Debug_MakeDbgInfo(const TMDbgClassInfo* pinfo)
{
    CStringA str;
    str.Format("class info:%s:%d; this:0x%p; classsize:%d; thread:%d",
        pinfo->m_pTypeName, pinfo->m_id, pinfo->m_pClass, pinfo->m_szClass, pinfo->m_idThread);
    return str;
}

template <class T>
inline CString _Debug_MakeDelegateInfo(T& del)
{
    CString str;
    
//     if (del.IsFunction())
        str.Format(L"\r\n  function:0x%p(0x%p); ", del.pfn, del.pVoid);
//     else
//         str.Format(L"\r\n  callback:0x%p; ", del.pProc);
    
    const TMDbgClassInfo* pinfo = DbgLeakOpt((TMDbgClassInfo*)del.pVoid, eDbgLeakOptTypeFindByMem);
    if (pinfo)
        str += _Debug_MakeDbgInfo(pinfo);
    else
        str += L"\r\n";
    
    return str;
}

template <class T>
inline CString _Debug_MakeDelegatesInfo(T& aDel)
{
    CString str;
    for (typename T::CProcList::recorder rec(aDel.GetProcs()); rec; ++rec)
    {
        typename T::PROC& proc = rec.get_data();
        str += _Debug_MakeDelegateInfo(proc);
    }
    return str;
}


inline int GetDomObjectID(IDomNode* p)
{
    DbgAssert(p);
    return p->get_nodeName().Mid(1).ToLong();
}

struct TUIOBJID;
struct TUIOBJID
{
    TMUInt32    m_dwObjID;
    int         m_nDPI;
    
    struct Less
    {
        bool operator()(const TUIOBJID* _x, const TUIOBJID* _y) const
        {
            if (_x->m_dwObjID != _y->m_dwObjID)
                return _x->m_dwObjID < _y->m_dwObjID;
            if (_x->m_nDPI == 0 || _y->m_nDPI == 0) // ==0 means skip dpi.
                return false;
            return _x->m_nDPI < _y->m_nDPI;
        };
    };

    TMUInt32 GetObjectID () const { return m_dwObjID; }
    CTuiZoom GetDPI () const { return m_nDPI ? m_nDPI : TM_DPI_SYSTEM_DEFAULT; }
    GXDips GetEps() const
    {
        return (GXDips)0.5 / (GXDips)GetDPI();
    }

//     int WP(int x) const { return CTuiZoom(m_nDPI).LPtoWP(x); }
};


////////////////////////////////////////////////////////////////////////////////
// class register 
// shared object: only one instance in one process
// unshared object: create a new object and use old object to init it
// purpose for design:
//  1.quick registration class.
//  2.ensure new and delete are in the same module
//  3.clone object
//  4.remove virtual function in CTuiObject
class TM_NO_VTABLE CTuiObject : public TUIOBJID
{
private:
    TM_NO_COPYABLE(CTuiObject);

public:
    CFactoryInfo*   m_pFactoryInfo;
    TMLParam        m_dwTag;
    TMAtmInt        m_nRef;
    
#ifdef TUI_EDIT_MODE
    CObjData*       m_pEditData;
#endif

    TM_SELF_ALLOC(CTuiObject);
    TM_UNUSED_IC_GetDebugInfo();
    TM_IF_DBG(virtual void CreateVTable() {} );

protected:
    CTuiObject();
    ~CTuiObject();

public:
    CFactoryInfo* GetFactory() { return m_pFactoryInfo; }

    CModule* GetModule() const;
    CView* GetView() const;

    void FinalConstruct(CTuiObject* pParent) {}
    void FinalRelease() {}

    template<class S> TMBool _Serialize(S& __stream)
    {
        OBJ_PROP_INT ("Tag", m_dwTag);
        return false; 
    }

//     bool IsNativeMulitDPI() const { return false; }

    
    CStrPtr GetClassName();

    EObjType GetType() const;

//    virtual TMResult Dispatch(void* pKey, HTUI hWgt, TMUInt msg, TMWParam wp, TMLParam lp) { return 0; }
    TMResult Dispatch(TMUInt msg, TMWParam wp, TMLParam lp) { return 0; }

    TM_REFAPI AddRef();
    
    TM_REFAPI Release();

    void LoadAll (IDomElement* xml, TMUInt32 dwFlag);

#ifdef TUI_EDIT_MODE
    virtual void OnLoadProp (IDomElement* pXml) {}

    virtual void SaveProp(class CSaveStream& __stm) = 0;
    virtual void EditProp(class CEditStream& __stm) = 0;
    virtual void ResetIDProp(class CIdStream& __stm) = 0;

    virtual CString GetDebugInfo();

    virtual int EditorRender(IGXApp* gapp, int nState, const CTuiRect& rc) { return 0; } // -1: render as edit

    virtual TMBool EditorMatchID(TMUInt32 n) { return false; }

#else

    void OnLoadProp (void* pXml) {}

#endif // TUI_EDIT_MODE


    // helper function
    TMHModule GetModuleHandle() const;
    TMBool IsHighContrast() const;

};

// spy mode / doc mode / release mode;
#ifdef TUI_EDIT_MODE

#   define Edit_IsObjectInDocMode(x) ((x)->GetView() == AppGetDoc())
#   define Edit_IsObjectInSpyMode(x) ((x)->GetView() == &AppGetTester()->GetView())
    inline TMBool Edit_IsObjectInSpyOrDocMode(const CTuiObject* x) { return 0 != x->m_pEditData; }
    void Edit_ConvertXmlVersion(CModule& mdl, CStringW& strXml);
    TMBool Edit_IsShowHide(TMBool bEdit);
    TMBool Edit_IsKeepMenuPop(TMBool bEdit);
    TMBool Edit_IsDisableGhost();
    void Edit_SaveTexture(TMBool bEdit, HTUI hWnd, GXPTexture texture, int nAlpha);
    void Edit_SaveFlip(TMBool bEdit, int nObjid, HWND hWnd, GXPTexture texture, const TMRect& rcFlip, UINT* msg);
    TMBool Edit_IsPlugInModeEditor();

#   define SPY_INVALIDATE_PARAM(x,y)  x, y

    void Edit_SpyInvalidate(HWND hWnd, HTUI h, int nObjID, const CString& cname, const TMRect* rc, const TMRectF* rcDirty = 0);
    template <class T> inline void Edit_SpyInvalidate(HWND hWnd, T* h, const TMRect* rc, const TMRectF* rcDirty = 0)
    {
        if (h && Edit_IsObjectInSpyMode(h))
            Edit_SpyInvalidate(hWnd, h, h->GetObjectID(), h->GetClassName().c_str(), rc, rcDirty);
    }

    class CDisableSpyFunc
    {
    public:
        static TMBool& IsDisable() { static TMBool b = 0; return b; }
        CDisableSpyFunc() { IsDisable() = true; }
        ~CDisableSpyFunc() { IsDisable() = false; }
//         static void Disable() { IsDisable() = true; }
    };
#   define DISABLE_SPY() CDisableSpyFunc TM_LINK_OF(ds,__LINE__)

#   define TUI_IF_EDIT(x)   x

#else
    
#   define Edit_IsObjectInDocMode(x)          false
#   define Edit_IsObjectInSpyMode(x)          true
#   define Edit_IsObjectInSpyOrDocMode(x)     false
#   define Edit_ConvertXmlVersion(m,x)           false
#   define Edit_IsShowHide(b)               false
#   define Edit_IsKeepMenuPop(b)            false 
#   define Edit_IsDisableGhost()            false
#   define Edit_SaveTexture(b,h,t,a) 
#   define Edit_SaveFlip(b,h,t,a,r,m) 
#   define Edit_IsPlugInModeEditor()        false

#   define Edit_SpyInvalidate(hw,h,rc,rc2)       {}
#   define SPY_INVALIDATE_PARAM(x,y)        x

#   define DISABLE_SPY()

#   define TUI_IF_EDIT(x)

#endif

// for test case project
#ifdef TUI_TEST_MODE
#   undef Edit_IsObjectInSpyMode
#   define Edit_IsObjectInSpyMode(x)          true
#endif






// int StrDec (CStringW& strDest, const CXmlTag& strSource);


TM_END_NAMESPACE


#ifdef TUI_EDIT_MODE
#   include "../design/Panel/EditStream.h"
#   include "../design/Panel/SaveStream.h"
#   include "../design/Panel/NullStream.h"
#endif


#endif // OBJECT_H
