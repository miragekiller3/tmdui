/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2017.1.5
 *
 *  Copyright (C) 2017 miragekiller
 */

#include "property.h"
#include "uiwidget.h"


TM_BGN_NAMESPACE


/////////////////////////////////////////////////////////////////////////////
// CPropMenu
CPropMenu::CPropMenu()
    : hMenu(0)
    , hRes(0)
    , nID (0)
{}

CPropMenu::~CPropMenu()
{
#ifdef TM_WIN
    if (hMenu)
        DestroyMenu(hMenu);
#endif
}

void CPropMenu::Set(TMHMenu h)
{
    hMenu = h;
    hRes = 0;
    nID = 0;
}

void CPropMenu::Set(TMHModule h, int n)
{
    hRes = h;
    nID = n;
    ReLoad();
}

void CPropMenu::ReLoad()
{
#ifdef TM_WIN
    if (hMenu)
        DestroyMenu(hMenu);
    hMenu = _tuiApp.LoadMenu(hRes, MAKEINTRESOURCE(nID));
#endif
}


/////////////////////////////////////////////////////////////////////////////
// CPropText
void CPropText::PropSet(const CString& xml)
{
#ifdef TUI_EDIT_MODE
    CString strTmp(xml);
#endif
    CTuiWgt* p = TM_OTHIS(CTuiWgt, m_gxText);
    p->DoSetText(xml, TUISETTEXT::eDefault);
#ifdef TUI_EDIT_MODE
    DbgAssert(m_str == CString(strTmp));
#endif
}

void CPropText::PropSetByEditor(const CXmlTag& xml)
{
    PropSet(xml);
    CTuiWgt* p = TM_OTHIS(CTuiWgt, m_gxText);
    p->SetDirtyAndInvalidate();
}


#ifdef TUI_EDIT_MODE

CString CPropText::PropGet() const
{
    CTuiWgt* pw = TM_OTHIS(CTuiWgt, m_gxText);
    if (Edit_IsObjectInSpyMode(pw))
        return p ? p->GetHtml() : CString();
    return m_str;
}


#endif // TUI_EDIT_MODE




//////////////////////////////////////////////////////////////////////////////
// CProp_Char; // "*" <-> 42; "" <-> 0
class CProp_Char : public CProp_Long<wchar_t>
{
    virtual void Str2T(const ILoaderEnv* env, const CWeakStr& s, wchar_t* p) tm_final { XmlStrToChar(s, *p); }
    
#ifdef TUI_EDIT_MODE
    virtual int  GetType() tm_override { return SS_TEXT; }
    virtual CString T2Str(const wchar_t* p) tm_override { return std::move(XmlStrFromChar(*p)); }
#endif
};
IPropItemLoaderSaverEdit* PropMakeChar(const wchar_t&) { return CPropInstX<CProp_Char>::s_inst.face(); }


//////////////////////////////////////////////////////////////////////////////
// CProp_class<CString>, CProp_class<GXPText>, CProp_class<CPropText>
template <class T> class CProp_TextBase: public IPropItemLoaderSaverEditT<T>
{
#ifdef TUI_EDIT_MODE
    // saver
    virtual int  GetType() tm_override { return SS_TEXT; }
    virtual bool EqualString(const T* p, const CWeakStr& s) tm_override { return this->T2Str(p) == s; }
    
    // edit
    virtual bool Edit_Equal(const T* v1, const T* v2) tm_override { return this->T2Str(v1) == this->T2Str(v2); }
    virtual EPopable Edit_InitPopable(CPropBox2* pb, TMBool bSelect) tm_final { return ePopableButton; }
    virtual TMBool Edit_DoDialog(IProjMgr* mgr, TM::CString& s) tm_override { return mgr->PropShowPopupString(s); }
#endif
};

TM_TEMPLATE_NULL class CProp_Class<CString> : public CProp_TextBase<CString>
{
    virtual void Str2T(const ILoaderEnv* env, const CWeakStr& s, CString* p) tm_final { *p = s; }
#ifdef TUI_EDIT_MODE
    virtual CString T2Str(const CString* p) tm_override { return *p; }
#endif
};

TM_TEMPLATE_NULL class CProp_Class<GXPText> : public CProp_TextBase<GXPText>
{
    virtual void Str2T(const ILoaderEnv* env, const CWeakStr& s, GXPText* p) tm_final { if (!*p) gxTextCreate(p); (*p)->SetString(s); }
#ifdef TUI_EDIT_MODE
    virtual CString T2Str(const GXPText* p) tm_override { return *p ? (*p)->GetString() : CString(); }
#endif
};

TM_TEMPLATE_NULL class CProp_Class<CPropText> : public CProp_TextBase<CPropText>
{
    virtual void Str2T(const ILoaderEnv* env, const CWeakStr& s, CPropText* p) tm_final { p->PropSetByEditor(s); }
#ifdef TUI_EDIT_MODE
    virtual CString T2Str(const CPropText* p) tm_override { return p->PropGet(); }
#endif
};
IPropItemLoaderSaverEdit* PropMake(const CString&)   { return CPropInstX<CProp_Class<CString> >::s_inst.face(); }
IPropItemLoaderSaverEdit* PropMake(const GXPText&)   { return CPropInstX<CProp_Class<GXPText> >::s_inst.face(); }
IPropItemLoaderSaverEdit* PropMake(const CPropText&) { return CPropInstX<CProp_Class<CPropText> >::s_inst.face(); }


#ifdef TUI_EDIT_MODE

//////////////////////////////////////////////////////////////////////////////
// CProp_file; 
class CProp_File : public CProp_Class<CString>
{
    virtual int  GetType() tm_override { return SS_FILE; }
    virtual TMBool Edit_DoDialog(IProjMgr* mgr, TM::CString& s) tm_final { return mgr->PropShowPopupFile(s); }
};
IPropItemEdit* PropMakeFile(const CString&) { return CPropInstX<CProp_File>::s_inst.face(); }


//////////////////////////////////////////////////////////////////////////////
// CProp_ole; 
class CProp_Ole : public CProp_Class<CString>
{
    virtual int  GetType() tm_override { return SS_OLE; }
    virtual TMBool Edit_DoDialog(IProjMgr* mgr, TM::CString& s) tm_final { return mgr->PropShowPopupOLE(s); }
};
IPropItemEdit* PropMakeOle(const CString&) { return CPropInstX<CProp_Ole>::s_inst.face(); }

#endif


//////////////////////////////////////////////////////////////////////////////
// CProp_class<CPropHotKey>; 
TM_TEMPLATE_NULL class CProp_Class<CPropHotKey> : public IPropItemLoaderSaverEditT<CPropHotKey>
{
public:
    // loader
    virtual void Str2T(const ILoaderEnv* env, const CWeakStr& s, CPropHotKey* p) tm_override { XmlStrToLong(s, p->m_key); }
    
#ifdef TUI_EDIT_MODE
    // saver
    virtual int  GetType() tm_override { return SS_ACCEL; }
    virtual CString T2Str(const CPropHotKey* p) tm_override { return std::move(XmlStrFromHex(p->m_key)); }
    virtual bool EqualString(const CPropHotKey* p, const CWeakStr& s) tm_override { return _CProp_EqualString_val(this, p, s); }
    
    // edit
    virtual void Edit_String2T(CPropBox2* pb, const CWeakStr& s, CPropHotKey* p) tm_override 
    {
        XmlStrToChar(s, (wchar_t&)p->m_vk);
        if (!s.IsEmpty())
            CKeyboardDebugInfo::String2VK(CString(s), p->m_vk);
    }
    virtual CString Edit_T2String(CPropBox2* pb, const CPropHotKey* p) tm_override
    {
        CString str = XmlStrFromChar(p->m_vk);
        CKeyboardDebugInfo::VK2String(p->m_vk, str);
        return str;
    }
    
    virtual bool Edit_Equal(const CPropHotKey* v1, const CPropHotKey* v2) tm_override { return *v1 == *v2; }
    virtual EPopable Edit_InitPopable(CPropBox2* pb, TMBool bSelect)
    {
        if (bSelect)
            pb->SetHook();
        else
            pb->RemoveHook();
        return ePopableNone;
    }
#endif
};
IPropItemLoaderSaverEdit* PropMake(const CPropHotKey&) { return CPropInstX<CProp_Class<CPropHotKey> >::s_inst.face(); }


//////////////////////////////////////////////////////////////////////////////
// CProp_class<CPropColor>; 
TM_TEMPLATE_NULL class CProp_Class<CPropColor> : public IPropItemLoaderSaverEditT<CPropColor>
{
public:
    // loader
    virtual void Str2T(const ILoaderEnv* env, const CWeakStr& s, CPropColor* p) tm_override { p->FromText(s); }
    
#ifdef TUI_EDIT_MODE
    // saver
    virtual int  GetType() tm_override { return SS_COLOR; }
    virtual CString T2Str(const CPropColor* p) tm_override { return p->ToText(); }
    virtual bool EqualString(const CPropColor* p, const CWeakStr& s) tm_override { return _CProp_EqualString_val(this, p, s); }
    
    // edit
    virtual bool Edit_Equal(const CPropColor* v1, const CPropColor* v2) tm_override { return *v1 == *v2; }
    virtual EPopable Edit_InitPopable(CPropBox2* pb, TMBool bSelect) { return ePopableButton; }

    static void _Append(const CPropColor& clr, CMenu& mu, int index, LPCTSTR pStr)
    {
        TMULong dwFlag = MF_STRING;
        if (!clr.IsColor() && clr.GetIndex() == index)
            dwFlag |= MF_CHECKED;
        mu.AppendMenu(dwFlag, index + 1, pStr);
    }

    virtual TMBool Edit_DoDialog(IProjMgr* mgr, TM::CString& str) tm_final
    {
        CPropColor clr;
        this->Str2T(0, str, &clr);
        
        CMenu mu;
        mu.CreatePopupMenu();
        mu.AppendMenu(MF_STRING, -1, L"custom");
        
#define COLOR_MENUHILIGHT   29
#define COLOR_MENUBAR       30
        
        const TMUInt COLOR_Default      = CPropColor::eIndexDefault;
        const TMUInt COLOR_Inherited    = CPropColor::eIndexParent;
        
#define PBC_APPEND(x) _Append(clr, mu, x, TM_T(#x))
        PBC_APPEND(COLOR_Default);
        PBC_APPEND(COLOR_Inherited);
        PBC_APPEND(COLOR_SCROLLBAR); //         0
        PBC_APPEND(COLOR_BACKGROUND); //        1
        PBC_APPEND(COLOR_ACTIVECAPTION); //     2
        PBC_APPEND(COLOR_INACTIVECAPTION); //   3
        PBC_APPEND(COLOR_MENU); //              4
        PBC_APPEND(COLOR_WINDOW); //            5
        PBC_APPEND(COLOR_WINDOWFRAME); //       6
        PBC_APPEND(COLOR_MENUTEXT); //          7
        PBC_APPEND(COLOR_WINDOWTEXT); //        8
        PBC_APPEND(COLOR_CAPTIONTEXT); //       9
        PBC_APPEND(COLOR_ACTIVEBORDER); //      10
        PBC_APPEND(COLOR_INACTIVEBORDER); //    11
        PBC_APPEND(COLOR_APPWORKSPACE); //      12
        PBC_APPEND(COLOR_HIGHLIGHT); //         13
        PBC_APPEND(COLOR_HIGHLIGHTTEXT); //     14
        PBC_APPEND(COLOR_BTNFACE); //           15
        PBC_APPEND(COLOR_BTNSHADOW); //         16
        PBC_APPEND(COLOR_GRAYTEXT); //          17
        PBC_APPEND(COLOR_BTNTEXT); //           18
        PBC_APPEND(COLOR_INACTIVECAPTIONTEXT); // 19
        PBC_APPEND(COLOR_BTNHIGHLIGHT); //      20
        PBC_APPEND(COLOR_3DDKSHADOW); //        21
        PBC_APPEND(COLOR_3DLIGHT); //           22
        PBC_APPEND(COLOR_INFOTEXT); //          23
        PBC_APPEND(COLOR_INFOBK); //            24
        PBC_APPEND(COLOR_HOTLIGHT); //                  26
        PBC_APPEND(COLOR_GRADIENTACTIVECAPTION); //     27
        PBC_APPEND(COLOR_GRADIENTINACTIVECAPTION); //   28
        PBC_APPEND(COLOR_MENUHILIGHT); //       29
        PBC_APPEND(COLOR_MENUBAR); //           30)
#undef PBC_APPEND
        
        int n = mgr->PropPopupMenu(mu);
        if (n == 0)
            return false;
        
        if (n != -1)
        {
            clr.SetIndex(n - 1);
            str = T2Str(&clr);
            return true;
        }
        
        if (mgr->PropShowPopupColor(clr.m_clr))
        {
            str = T2Str(&clr);
            return true;
        }
        
        return false;
    }
#endif
};
IPropItemLoaderSaverEdit* PropMake(const CPropColor&) { return CPropInstX<CProp_Class<CPropColor> >::s_inst.face(); }


//////////////////////////////////////////////////////////////////////////////
// CProp_class<GXFont>; 
TM_TEMPLATE_NULL class CProp_Class<GXFont> : public IPropItemLoaderSaverEditT<GXFont>
{
public:
    // loader
    virtual void Str2T(const ILoaderEnv* env, const CWeakStr& s, GXFont* p) tm_override { p->FromText(s); }
    
#ifdef TUI_EDIT_MODE
    // saver
    virtual int  GetType() tm_override { return SS_FONT; }
    virtual CString T2Str(const GXFont* p) tm_override { return p->ToText(); }
    virtual bool EqualString(const GXFont* p, const CWeakStr& s) tm_override { return T2Str(p) == s; }
    
    // edit
    virtual bool Edit_Equal(const GXFont* v1, const GXFont* v2) tm_override { return T2Str(v1) == T2Str(v2); }
    virtual EPopable Edit_InitPopable(CPropBox2* pb, TMBool bSelect) { return ePopableButton; }
        
    virtual TMBool Edit_DoDialog(IProjMgr* mgr, TM::CString& str) tm_final
    {
        GXFont ft;
        Str2T(0, str, &ft);
        LOGFONT lf;
        ft.ToLOGFONT(lf);
        if (mgr->PropShowPopupFont(lf))
        {
            ft.FromLOGFONT(lf);
            str = T2Str(&ft);
            return true;
        }
        return false;
    }
#endif
};
IPropItemLoaderSaverEdit* PropMake(const GXFont&) { return CPropInstX<CProp_Class<GXFont> >::s_inst.face(); }


//////////////////////////////////////////////////////////////////////////////
// CProp_id_base;
template <class T, class T_Base, EIDTYPE nType, EIDTYPE nCreateType> class TM_NO_VTABLE CProp_ID_base : public T_Base
{
public:
    TMUInt32 Str2ID(const CWeakStr& s) const { return CProp_Long<TMUInt32>::Str2Long(s); }

#ifdef TUI_EDIT_MODE
    // saver
    virtual bool EqualString(const T* p, const CWeakStr& s) tm_final { return Str2ID(s) == this->Obj2ID(p); }

    // edit
    virtual bool Edit_Equal(const T* v1, const T* v2) tm_final { return this->Obj2ID(v1) == this->Obj2ID(v2); }

    virtual void Edit_String2T(CPropBox2* pb, const CWeakStr& str, T* p) tm_final
    {
        TMUInt32 id = pb->Text2ID(str, nCreateType);
        this->ID2Obj(pb, p, id);
    }
    virtual CString Edit_T2String(CPropBox2* pb, const T* p) tm_final
    {
        TMUInt32 id = this->Obj2ID(p);
        return std::move(pb->ID2Text(id));
    }
    virtual EPopable Edit_InitPopable(CPropBox2* pb, TMBool bSelect) tm_final
    {
        if (bSelect)
            pb->InitPop(nType);
        return ePopableComb;
    }

//     virtual TMUInt32 Obj2ID(const T* p) = 0;
    virtual void ID2Obj(CPropBox2* pb, T* p, TMUInt32 id) = 0;
#endif
};

#ifdef TUI_EDIT_MODE

template <EIDTYPE nType, EIDTYPE nCreateType> class CProp_Id : public CProp_ID_base<TMUInt32, CProp_Long<TMUInt32>, nType, nCreateType>
{
    virtual int  GetType() tm_final { return SS_ID; }
    virtual TMUInt32 Obj2ID(const TMUInt32* p) tm_final { return *p; }
    virtual void ID2Obj(CPropBox2* pb, TMUInt32* p, TMUInt32 id) tm_final { *p = id; }
};

IPropItemEdit* PropMakeCID(const TMUInt32&) { return CPropInstX<CProp_Id<EM_CONTROL, EM_CONTROL> >::s_inst.face(); }
IPropItemEdit* PropMakeDLG(const TMUInt32&) { return CPropInstX<CProp_Id<EM_DIALOG, EM_NULL> >::s_inst.face(); }
IPropItemEdit* PropMakeWgt(const TMUInt32&) { return CPropInstX<CProp_Id<EM_WGT, EM_NULL> >::s_inst.face(); }

#endif


//////////////////////////////////////////////////////////////////////////////
// CProp_class<CPicPtr>; not for editor
template <class T, EIDTYPE nType, int ss_type>
class CProp_PicFrm : public CProp_ID_base<T, IPropItemLoaderSaverEditT<T>, nType, EM_NULL>
{
    void _ID2Obj(const ILoaderEnv* env, T* p, TMUInt32 id)
    {
        p->Create(env->GetModule(), id, env->GetDPI());
    }
    virtual void Str2T(const ILoaderEnv* env, const CWeakStr& s, T* p) tm_final
    {
        DbgAssert(env);
        _ID2Obj(env, p, this->Str2ID(s));
    }
    
#ifdef TUI_EDIT_MODE
    // saver
    virtual int  GetType() tm_final { return ss_type; }
    virtual CString T2Str(const T* p) tm_final { DbgAssert(0); return CString(); }


    virtual TMUInt32 Obj2ID(const T* p) tm_final
    {
        return *p ? (*p)->GetObjectID() : 0;
    }
    virtual void ID2Obj(CPropBox2* pb, T* p, TMUInt32 id) tm_final
    {
        _ID2Obj(pb, p, id);
    }
#endif
};

IPropItemLoaderSaverEdit* PropMake(const CPicPtr&) { return CPropInstX<CProp_PicFrm<CPicPtr, EM_PIC, SS_PIC> >::s_inst.face(); }
IPropItemLoaderSaverEdit* PropMake(const CFrmPtr&) { return CPropInstX<CProp_PicFrm<CFrmPtr, EM_FRAME, SS_FRAME> >::s_inst.face(); }


//////////////////////////////////////////////////////////////////////////////
// CProp_class<CPropMenu>; not for editor
TM_TEMPLATE_NULL class CProp_Class<CPropMenu> : public CProp_ID_base<CPropMenu, IPropItemLoaderSaverEditT<CPropMenu>, EM_MENU, EM_NULL>
{
    virtual void Str2T(const ILoaderEnv* env, const CWeakStr& s, CPropMenu* p) tm_final { XmlStrToLong(s, p->nID); }
    
#ifdef TUI_EDIT_MODE
    // saver
    virtual int  GetType() tm_final { return SS_MENU; }
    virtual CString T2Str(const CPropMenu* p) tm_final { return std::move(XmlStrFromLong(p->nID)); }
    virtual TMUInt32 Obj2ID(const CPropMenu* p) tm_final { return p->nID; }
    virtual void ID2Obj(CPropBox2* pb, CPropMenu* p, TMUInt32 id) tm_final { p->Set(HEDITOR, id); }
#endif
};
IPropItemLoaderSaverEdit* PropMake(const CPropMenu&) { return CPropInstX<CProp_Class<CPropMenu> >::s_inst.face(); }


//////////////////////////////////////////////////////////////////////////////
// CProp_ints<rect>; CProp_ints<size>
template <class T>
class CProp_Ints : public IPropItemLoaderSaverT<T>
{
public:
    // loader
    virtual void Str2T(const ILoaderEnv* env, const CWeakStr& s, T* p) { XmlStrToInts(s, *p); }
    
    // saver
#ifdef TUI_EDIT_MODE
    int  GetType() tm_override  { return SS_SIZE; }
    CString T2Str(const T* p) tm_override { return std::move(XmlStrFromInts(*p)); }
    bool EqualString(const T* p, const CWeakStr& s) tm_override { return _CProp_EqualString_val(this, p, s); }
#endif
};

IPropItemLoaderSaver* PropMake(const CTuiRect&) { return CPropInstX<CProp_Ints<CTuiRect> >::s_inst.face(); }
IPropItemLoaderSaver* PropMake(const BORDER&) { return CPropInstX<CProp_Ints<BORDER> >::s_inst.face(); }
template <class T> IPropItemLoaderSaver* PropMake(const TSIZE<T>&) { return CPropInstX<CProp_Ints<TSIZE<T> > >::s_inst.face(); }


//////////////////////////////////////////////////////////////////////////////
// CProp_Width
#ifdef TUI_EDIT_MODE

class TM_NO_VTABLE CProp_WH : public IPropItemEditT<CTuiRect>
{
    virtual EPopable Edit_InitPopable(CPropBox2* pb, TMBool bSelect) { return ePopableNone; }
    virtual TMBool Edit_DoDialog(IProjMgr* mgr, TM::CString& str) { return false; }
};

class CProp_Width : public CProp_WH
{
public:
    virtual void Edit_String2T(CPropBox2* pb, const CWeakStr& s, CTuiRect* p) tm_final { p->KeepLSetW(CProp_Long<int>::Str2Long(s)); }
    virtual CString Edit_T2String(CPropBox2* pb, const CTuiRect* p) tm_final { return std::move(XmlStrFromLong(p->Width())); }
    virtual bool Edit_Equal(const CTuiRect* v1, const CTuiRect* v2) tm_final { return v1->Width() == v2->Width(); };
};

class CProp_Height : public CProp_WH
{
public:
    virtual void Edit_String2T(CPropBox2* pb, const CWeakStr& s, CTuiRect* p) tm_final { p->KeepTSetH(CProp_Long<int>::Str2Long(s)); }
    virtual CString Edit_T2String(CPropBox2* pb, const CTuiRect* p) tm_final { return std::move(XmlStrFromLong(p->Height())); }
    virtual bool Edit_Equal(const CTuiRect* v1, const CTuiRect* v2) tm_final { return v1->Height() == v2->Height(); };
};

IPropItemEdit* PropMakeWidth(const CTuiRect&) { return CPropInstX<CProp_Width>::s_inst.face(); }
IPropItemEdit* PropMakeHeight(const CTuiRect&) { return CPropInstX<CProp_Height>::s_inst.face(); }

#endif


TM_END_NAMESPACE



