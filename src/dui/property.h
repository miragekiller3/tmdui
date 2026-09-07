/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2008.08.11
 *
 *  Copyright (C) 2008 miragekiller
 */

#pragma once

#ifndef PROPERTY_H
#define PROPERTY_H

#include "tmdui.h"
#include "tmsymbol.h"
#include "tmxmlstring.h"

class IProjMgr;


TM_BGN_NAMESPACE

struct CModule;
struct CPicPtr;
struct CFrmPtr;
class CPropBox2;
typedef CBorder BORDER;


///////////////////////////////////////////////////////////////////////////////
// IPropItemLoaderT
struct ILoaderEnv
{
public:
    virtual CModule& GetModule() const = 0;
    virtual int GetDPI() const { return TUI_DPI_SYSTEM_DEFAULT; }
};

template <class T> struct IPropItemLoaderT
{
public:
    IPropItemLoaderT<void>* face() { return (IPropItemLoaderT<void>*)this; }
    virtual void Str2T(const ILoaderEnv* env, const CWeakStr& s, T* p) = 0;
};
typedef IPropItemLoaderT<void> IPropItemLoader;


#ifdef TUI_EDIT_MODE

///////////////////////////////////////////////////////////////////////////////
// IPropItemSaverT
template <class T>
struct IPropItemSaverT
{
public:
    IPropItemSaverT<void>* face() { return (IPropItemSaverT<void>*)this; }
    virtual int  GetType() = 0;
    virtual CString T2Str(const T* obj) = 0;
    virtual bool EqualString(const T* p, const CWeakStr& s) = 0;
};
typedef IPropItemSaverT<void> IPropItemSaver;


///////////////////////////////////////////////////////////////////////////////
// IPropItemEditT
struct IEditEnv : public ILoaderEnv
{
public:
};

enum EPopable
{
    ePopableNone, ePopableButton, ePopableComb
};

template <class T>
struct IPropItemEditT
{
public:
    IPropItemEditT<void>* face() { return (IPropItemEditT<void>*)this; }
    virtual void Edit_String2T(CPropBox2* pb, const CWeakStr& s, T* p) = 0;
    virtual CString Edit_T2String(CPropBox2* pb, const T* p) = 0;
    virtual bool Edit_Equal(const T* v1, const T* v2) = 0;
    virtual EPopable Edit_InitPopable(CPropBox2* pb, TMBool bSelect) { return ePopableNone; }
    virtual TMBool Edit_DoDialog(IProjMgr* mgr, TM::CString& str) { return false; }
    virtual TMUInt32 Obj2ID(const T* p) { return 0; }
};
typedef IPropItemEditT<void> IPropItemEdit;


///////////////////////////////////////////////////////////////////////////////
// IPropItemLoaderSaverT
template <class T>
struct TM_NO_VTABLE IPropItemLoaderSaverT : public IPropItemLoaderT<T>, public IPropItemSaverT<T>
{
    IPropItemLoaderSaverT<void>* face() { return (IPropItemLoaderSaverT<void>*)this; }
};
template <class T>
struct TM_NO_VTABLE IPropItemLoaderSaverEditT : public IPropItemLoaderT<T>, public IPropItemSaverT<T>, public IPropItemEditT<T>
{
    IPropItemLoaderSaverEditT<void>* face() { return (IPropItemLoaderSaverEditT<void>*)this; }
    virtual void Edit_String2T(CPropBox2* pb, const CWeakStr& s, T* p) tm_override { this->Str2T(0, s, p); }
    virtual CString Edit_T2String(CPropBox2* pb, const T* p) tm_override { return std::move(this->T2Str(p)); }
};

#else // TUI_EDIT_MODE

#   define IPropItemLoaderSaverT        IPropItemLoaderT
#   define IPropItemLoaderSaverEditT    IPropItemLoaderT

#endif // TUI_EDIT_MODE

typedef IPropItemLoaderSaverT<void>     IPropItemLoaderSaver;
typedef IPropItemLoaderSaverEditT<void> IPropItemLoaderSaverEdit;



////////////////////////////////////////////////////////////////////////////////
// Serialization

#define SS_FILE         0x00000001
#define SS_COLOR        0x00000002
#define SS_SIZE         0x00000004
#define SS_ID           0x00000008
#define SS_PIC          0x00000010
#define SS_FONT         0x00000020
#define SS_FRAME        0x00000040
#define SS_ENUM         0x00000080
#define SS_MASK         0x00000100
#define SS_TEXT         0x00000200
#define SS_INT          0x00000400
#define SS_MENU         0x00000800
#define SS_ACCEL        0x00001000
#define SS_HTML         0x00002000
#define SS_CLASSNAME    0x00004000
#define SS_CHILD        0x00008000 // load or save child item
#define SS_OLE          0x00010000
#define SS_ALL          0x000fffff


// subobject is put below parent object not in it.
// besides delete the objects  whose count attribute is 0 and not record the class attributes 
#define SS_SKINMODE     0x10000000

// check the attribute default values and delete it if same
#define SS_REMOVESAME   0x20000000 

// SS_CHECKCHILD only for load mode:
//   SS_CHILD: create child.
//   SS_CHILD && SS_CHECKCHILD: create child, but skip same object_id's child
//   SS_CHECKCHILD: don't create child, but reload child's xml
#define SS_CHECKCHILD   0x40000000 // don't load exists child object

// save id macro to XML.
// <L1234 .. macro_oid="IDW_WINDOWS1234" id=0 macro_id="IDOK"></L1234>
// #define SS_OIDMACRO     0x80000000 


/////////////////////////////////////////////////////////////////////////////
// TSIZE
template <class T> struct TSIZE
{
    typedef T value_type;
    T cx;
    T cy;
    
    TSIZE(T x = 0, T y = 0) : cx (x), cy (y) {}
    bool operator == (const TSIZE& r) const { return cx == r.cx && cy == r.cy; }
};

/////////////////////////////////////////////////////////////////////////////
// CPropMenu
struct CPropMenu
{
    TMHMenu hMenu;
    TMHModule hRes;
    int nID;

    CPropMenu();
    ~CPropMenu();

    bool operator ==(const CPropMenu& r) const { return nID == r.nID; }

    void Set(TMHMenu h);
    void Set(TMHModule h, int n);   // call when WM_SETMENU and WM_NCCREATE
    void ReLoad();                  // call when change language
};

enum EPropText { ePropTextNormal = 0x01, ePropTextMemo = 0x02, ePropTextRich = 0x04 };

struct CPropText : public GX::CText
{
#ifdef TUI_EDIT_MODE
    CString m_str;
#endif

    void PropSet(const CString& xml);
    void PropSetByEditor(const CXmlTag& xml);
    CString PropGet() const;
    CPropText* operator& () { return this; }

    void ReCreate(const CString& str, TMBool bRich)
    {
#ifdef TUI_EDIT_MODE
        m_str = str;
#endif
        CText::ReCreate(str, bRich);
    }
};


struct CPropHotKey
{
    TM_BGN_MASK_SYMBOL  (CSymbol_Modifiers)
        TM_MASK_SYMBOL  (MOD_ALT)
        TM_MASK_SYMBOL  (MOD_CONTROL)
        TM_MASK_SYMBOL  (MOD_SHIFT)
        TM_MASK_SYMBOL  (MOD_WIN)
        TM_MASK_SYMBOL  (MOD_LEFT)
        TM_MASK_SYMBOL  (MOD_RIGHT)
        TM_MASK_SYMBOL  (MOD_ON_KEYUP)
        TM_MASK_SYMBOL  (MOD_IGNORE_ALL_MODIFIER)
        TM_MASK_SYMBOL2 (MOD_APP, MOD_APP | MOD_SYS)
        TM_MASK_SYMBOL2 (MOD_SYS, MOD_APP | MOD_SYS)
        TM_MASK_SYMBOL  (MOD_DISABLE)
        TM_MASK_SYMBOL  (MOD_THROUGH)
    TM_END_MASK_SYMBOL  ()
    
    enum { eMaskKeys = MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_WIN };
    enum { eMaskSide = MOD_LEFT | MOD_RIGHT };
    enum { eMaskKeysPress = eMaskKeys | MOD_ON_KEYUP };
    
    union
    {
        TMUInt32        m_key;
        struct
        {
            TMUInt16    m_modifiers;
            TMUInt16    m_vk;
        };
    };
    
    CPropHotKey(TMUInt32 vk = 0)
        : m_vk(vk)
        , m_modifiers(0)
    {
    }

    CPropHotKey(TMUInt16 vk, TMUInt16 modifiers)
        : m_vk(vk)
        , m_modifiers(modifiers)
    {
    }

    bool operator == (const CPropHotKey& r) const { return m_key == r.m_key; }

    TMUInt GetID() const { return 0; }

    void Set(const TUIHOTKEY& hk)
    {
        m_vk = hk.vkey;
        m_modifiers = hk.modifiers;
    }

    void Normalize()
    {
        if (0 == (m_modifiers & eMaskSide))
            m_modifiers |= eMaskSide;
    }

    void CheckNormalize() const
    {
        DbgAssert(0 != (m_modifiers & eMaskSide));
    }
};

struct CPropHotKeyID : public CPropHotKey
{
public:
    TMUInt m_id;
    
    CPropHotKeyID(TMUInt32 key = 0, TMUInt32 id = 0)
        : CPropHotKey(key)
        , m_id(id)
    {
    }
    
    CPropHotKeyID(const CPropHotKey& key, TMUInt32 id)
        : CPropHotKey(key)
        , m_id(id)
    {
    }

    TMUInt GetID() const { return m_id; }

    void Set(const TUIHOTKEY& hk)
    {
        CPropHotKey::Set(hk);
        m_id = hk.id;
    }
    
    bool operator < (const CPropHotKeyID& rhs) const
    {
        return m_vk < rhs.m_vk;
    }
};

struct CPropColor
{
    enum { eIndexBase = 200 };
    enum { eIndexDefault = 55 };
    enum { eIndexParent = 54 };
    enum { eClrDefault  = TMRGBA(eIndexBase + eIndexDefault,0,0,0) };
    enum { eClrParent   = TMRGBA(eIndexBase + eIndexParent,0,0,0) };

    TMColor m_clr;

    CPropColor(TMColor clr = eClrDefault)
        : m_clr(clr)
    {
    }

    bool operator == (const CPropColor& r) const { return m_clr == r.m_clr; }

    TMColor ToColor () const
    {
        return (IsColor() || IsDefault() || IsParent()) ? m_clr : ColorFromSys(GetIndex());
    }

    void SetColor(TMColor clr)
    {
        m_clr = clr;
    }

    void SetIndex(int index)
    {
        SetNativeIndex(index + eIndexBase);
    }

    void SetNativeIndex(int index)
    {
        m_clr = TMRGBA(index, 0, 0, 0);
    }

    int GetIndex() const
    {
        return GetNativeIndex() - eIndexBase;
    }

    int GetNativeIndex() const
    {
        DbgAssert(!IsColor());
        return ColorR(m_clr);
    }

    TMBool IsColor() const
    {
        return (m_clr == 0) || ColorA(m_clr);
    }

    TMBool IsParent() const
    {
        return m_clr == eClrParent;
    }

    TMBool IsDefault() const
    {
        return m_clr == eClrDefault;
    }

    void SetDefault() { m_clr = eClrDefault; }

    void FromText(const CXmlTag& xml)
    {
        if (xml[0] >= '0' && xml[0] <= '9')
        {
            int n = 0;
            XmlStrToLong(xml, n);
            this->SetNativeIndex(n);
        }
        else
            XmlStrToColor(xml, this->m_clr);
    }
    
    CString ToText() const
    {
        if (this->IsColor())
            return XmlStrFromColor(this->m_clr);
        int index = this->GetNativeIndex();
        return std::move(XmlStrFromLong(index));
    }
};



//////////////////////////////////////////////////////////////////////////////
// CProp_XXX

template <class T, class T_Val> inline bool _CProp_EqualString_val(T* __t, const T_Val* p, const CWeakStr& str)
{
    T_Val __v;
    __t->Str2T(0, str, &__v);
    return __v == *p;
}

template <class T_Prop> struct CPropInstX
{
    static T_Prop s_inst;
};
template <class T_Prop> T_Prop CPropInstX<T_Prop>::s_inst;

template <class T> class CProp_Class;


//////////////////////////////////////////////////////////////////////////////
// CProp_Long
template <class T> class CProp_Long : public IPropItemLoaderSaverEditT<T>
{
public:
    static T Str2Long(const CWeakStr& s) { T id = T(); XmlStrToLong(s, id); return id; }

    // loader
    virtual void Str2T(const ILoaderEnv* env, const CWeakStr& s, T* p) tm_override { XmlStrToLong(s, *p); }

#ifdef TUI_EDIT_MODE
    // saver
    virtual int  GetType() tm_override { return SS_INT; }
    virtual CString T2Str(const T* p) tm_override { return std::move(XmlStrFromLong(*p)); }
    virtual bool EqualString(const T* p, const CWeakStr& s) tm_override { return _CProp_EqualString_val(this, p, s); }

    // edit
    virtual bool Edit_Equal(const T* v1, const T* v2) tm_override { return *v1 == *v2; }
    virtual EPopable Edit_InitPopable(CPropBox2* pb, TMBool bSelect) { return ePopableNone; }
    virtual TMBool Edit_DoDialog(IProjMgr* mgr, TM::CString& str) { return false; }
#endif
};
template <class T>
inline IPropItemLoaderSaverEdit* PropMakeLong(const T&) { return CPropInstX<CProp_Long<T> >::s_inst.face(); }


//////////////////////////////////////////////////////////////////////////////
// CProp_Char; // "*" <-> 42; "" <-> 0
IPropItemLoaderSaverEdit* PropMakeChar(const wchar_t&);


//////////////////////////////////////////////////////////////////////////////
// CProp_hex; "#abcd0123" <-> long
#ifdef TUI_EDIT_MODE
template <class T> class CProp_Hex : public CProp_Long<T>
{
    virtual CString T2Str(const T* p) tm_override { return std::move(XmlStrFromHex(*p)); }
};
template <class T>
IPropItemSaver* PropMakeHex(const T&) { return CPropInstX<CProp_Hex<T> >::s_inst.face(); }
#endif


//////////////////////////////////////////////////////////////////////////////
// CProp_class<CString>, CProp_class<GXPText>, CProp_class<CPropText>
IPropItemLoaderSaverEdit* PropMake(const CString&);
IPropItemLoaderSaverEdit* PropMake(const GXPText&);
IPropItemLoaderSaverEdit* PropMake(const CPropText&);

#ifdef TUI_EDIT_MODE
IPropItemEdit* PropMakeFile(const CString&);
IPropItemEdit* PropMakeOle(const CString&);
#endif


//////////////////////////////////////////////////////////////////////////////
// CPropColor; GXFont; CPropHotKey
IPropItemLoaderSaverEdit* PropMake(const CPropHotKey&);
IPropItemLoaderSaverEdit* PropMake(const CPropColor&);
IPropItemLoaderSaverEdit* PropMake(const GXFont&);


//////////////////////////////////////////////////////////////////////////////
// CProp_id;
#ifdef TUI_EDIT_MODE
IPropItemEdit* PropMakeCID(const TMUInt32&);
IPropItemEdit* PropMakeDLG(const TMUInt32&);
IPropItemEdit* PropMakeWgt(const TMUInt32&);
#endif

IPropItemLoaderSaverEdit* PropMake(const CPicPtr&);
IPropItemLoaderSaverEdit* PropMake(const CFrmPtr&);
IPropItemLoaderSaverEdit* PropMake(const CPropMenu&);


//////////////////////////////////////////////////////////////////////////////
// CProp_ints;
IPropItemLoaderSaver* PropMake(const CTuiRect&);
IPropItemLoaderSaver* PropMake(const BORDER&);
template <class T> IPropItemLoaderSaver* PropMake(const TSIZE<T>&);

#ifdef TUI_EDIT_MODE
IPropItemEdit* PropMakeWidth(const CTuiRect&);
IPropItemEdit* PropMakeHeight(const CTuiRect&);
#endif





TM_END_NAMESPACE



#endif // PROPERTY_H
