/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.03.15
 *
 *  Copyright (C) 2006 miragekiller
 */

#ifndef EDITSTREAM_H
#define EDITSTREAM_H

#include <typeinfo>
#include "PropItem.h"
#include "frame.h"
#include "pic.h"



TM_BGN_NAMESPACE

////////////////////////////////////////////////////////////////////////////////
// CProp_SymbolT
template<class T, class U>
class TM_NO_VTABLE CProp_SymbolT : public IPropItemEditT<T>
{
public:
    typedef U TSymbol;

    // IPropItemEditT
    virtual EPopable Edit_InitPopable(CPropBox2* pb, TMBool bSelect) tm_final { return ePopableButton; }
    
    virtual bool Edit_Equal(const T* v1, const T* v2) tm_final
    {
        TMULong d1 = TSymbol::GetMask() & (TMULong)*v1;
        TMULong d2 = TSymbol::GetMask() & (TMULong)*v2;
        return d1 == d2;
    }
    
    virtual TMBool Edit_DoDialog(IProjMgr* mgr, TM::CString& str) tm_override
    {
        CMenu mu;
        mu.CreatePopupMenu ();
        T dw = T();
        this->Edit_String2T(0, str, &dw);

        for (int i = 0; ; ++i)
        {
            TMCStr p = TSymbol::GetNameByIndex(i);
            if (!p)
                break;
            
            if (p[0])
            {
                TMULong dwFlag = TSymbol::CheckVal(i, dw) ? MF_CHECKED : 0;
                mu.AppendMenu (dwFlag | MF_STRING, i+1, p);
            }
            else
            {
                mu.AppendMenu(MF_MENUBREAK, i+1);
            }
        }
        
        int n = mgr->PropPopupMenu(mu);
        if (!n)
            return false;

        SetT(str, dw, n - 1);
        return true;
    }

    
    // self
    virtual void SetT(TM::CString& str, T val, int n) = 0;
};

////////////////////////////////////////////////////////////////////////////////
// CProp_Symbol_Enum
template<class T, class U>
class CProp_Symbol_Enum : public CProp_SymbolT<T, CEnumSymbolT<U> >
{
    typedef CEnumSymbolT<U>    TSymbol;
    
    // IPropItemEditT
    virtual CString Edit_T2String(CPropBox2* pb, const T* p) tm_final
    {
        return std::move(TSymbol::GetName(*p));
    }
    
    virtual void Edit_String2T(CPropBox2* pb, const CWeakStr& s, T* p) tm_final
    {
        unsigned long l = 0;
        if (TSymbol::GetValByName(s, l))
            *p = (T)l;
    }

    // CProp_SymbolT
    virtual void SetT(TM::CString& str, T val, int n) tm_final
    {
        TMULong dw = val;
        TMULong v = TSymbol::GetValByIndex (n);
        TMULong dwMask = TSymbol::GetMaskByIndex (n);
        dw &= ~dwMask;
        dw |= v;
        val = (T)dw;
        str = Edit_T2String(0, &val);
    }

public:
    static IPropItemEdit* PropMake() { return CPropInstX<CProp_Symbol_Enum>::s_inst.face(); }
};

////////////////////////////////////////////////////////////////////////////////
// CProp_Symbol_Mask
template<class T, class U>
class CProp_Symbol_Mask : public CProp_SymbolT<T, CMaskSymbolT<U> >
{
    typedef CMaskSymbolT<U>    TSymbol;
    
    // IPropItemEditT
    virtual CString Edit_T2String(CPropBox2* pb, const T* p) tm_final
    {
        CString str;
        str.Format(_T("#%08X"), (TSymbol::GetMask() & (TMULong)*p));
        return std::move(str);
    }
    
    virtual void Edit_String2T(CPropBox2* pb, const CWeakStr& s, T* p) tm_final
    {
        TMULong dwMask = TSymbol::GetMask();
        TMULong d = dwMask & CProp_Long<T>::Str2Long(s);
        TMULong val = (*p) & ~dwMask;
        *p = (T)(val | d);
    }

    // CProp_SymbolT
    virtual void SetT(TM::CString& str, T val, int n) tm_final
    {
        TMULong dw = val;
        TMULong v = TSymbol::GetValByIndex(n);
        TMULong msk = TSymbol::GetMaskByIndex(n);
        TMBool b = (val & msk) == v;
        dw &= ~msk;
        if (!b)
            dw |= v;
        val = (T)dw;
        str = Edit_T2String(0, &val);
    }

public:
    static IPropItemEdit* PropMake() { return CPropInstX<CProp_Symbol_Mask>::s_inst.face(); }
};



////////////////////////////////////////////////////////////////////////////////
// CEditStream
class CEditStream
{
protected:
    struct PROPKEY
    {
        CStrPtr m_key;
        const char* m_pT;
        const char* m_pU;

        bool operator < (const PROPKEY& r) const
        {
            if (m_pT != r.m_pT)
                return m_pT < r.m_pT;
            if (m_pU != r.m_pU)
                return m_pU < r.m_pU;
            return m_key < r.m_key;
        }
    };
    typedef TM::CMap<PROPKEY, IPropBox*>    CPropList;
    typedef CPropList::recorder             CPropRec;

    CPropList m_aProp;
    TM::CSet<TM::CString> m_aString;
    IObjEdit* m_pCurrent;

private:
    CPanelApp* _GetApp();

    void _AddProp(const CWeakStr& pName, IPropBox* pBox);

public:
    CEditStream() : m_pCurrent (0) {}
    ~CEditStream();

    template <class T, class U>
    TMBool ExecX(const CWeakStr& name, T* pVal, const U*)
    {
        PROPKEY key;
        key.m_key = name;
        key.m_pT = typeid(T*).name();
        key.m_pU = typeid(U).name();
        CPropRec rec(m_aProp);
        rec.find(key);
        U* p;
        if (!rec)
        {
            p = new U();
            m_aProp[key] = p;
            p->m_strName = name;
            p->m_bDisable = false;
            p->m_pApp = _GetApp();
            _AddProp(name, p);
            p->FinalConstruct();
        }
        else
        {
            p = (U*)rec.get_data();
            DbgAssert(p->m_strName == name);
        }
        
        DbgAssert(m_pCurrent);
        if (m_aString.find(name) == m_aString.end())
        {
            p->AddVal(pVal, m_pCurrent);
            m_aString.insert(name);
        }
        
        return false;
    }

    template <class T, class U>
    TMBool ExecZ(const CWeakStr& name, T* pVal, const U*, IPropItemEdit* prop)
    {
        PROPKEY key;
        key.m_key = name;
        key.m_pT = typeid(T*).name();
        key.m_pU = typeid(U).name();
        CPropRec rec(m_aProp);
        rec.find(key);
        CPropBox2* p;
        if (!rec)
        {
            p = new CPropBox2();
            m_aProp[key] = p;
            p->m_strName = name;
            p->m_bDisable = false;
            p->m_pApp = _GetApp();
            p->m_prop = prop;
            _AddProp(name, p);
            p->FinalConstruct();
        }
        else
        {
            p = (CPropBox2*)rec.get_data();
            p->m_prop = prop;
            DbgAssert(p->m_strName == name);
        }
        
        DbgAssert(m_pCurrent);
        if (m_aString.find(name) == m_aString.end())
        {
            p->AddVal(pVal, m_pCurrent);
            m_aString.insert(name);
        }
        
        return false;
    }

    template <class T>
    TMBool ExecZ(const CWeakStr& name, T* pVal, IPropItemEdit* prop)
    {
        return ExecZ(name, pVal, pVal, prop);
    }


    void BeginSerialize(const void*) const {}
    void EndSerialize() const {}
    void ExecChilds(HTUI hParent) {}

    void SetCurrentObj(IObjEdit* p, TMBool bAddMode);
    
    template<class T>
    TMBool ExecInt(const CWeakStr& pName, T& __val)
    {
        return ExecZ(pName, &__val, PropMakeLong(__val));
    }
    
    template<class T>
    TMBool ExecID(const CWeakStr& pName, T& __val)
    {
        return ExecZ(pName, &__val, PropMakeCID(__val));
    }

    template<class T>
    TMBool ExecDlg(const CWeakStr& pName, T& __val)
    {
        return ExecZ(pName, &__val, PropMakeDLG(__val));
    }

    template<class T>
    TMBool ExecWin(const CWeakStr& pName, T& __val)
    {
        return ExecZ(pName, &__val, PropMakeWgt(__val));
    }

    TMBool ExecPic(const CWeakStr& pName, CPicPtr& __val)
    {
        return ExecZ(pName, tm_addressof(__val), PropMake(__val));
    }
    
    TMBool ExecFrame(const CWeakStr& pName, CFrmPtr& __val)
    {
        return ExecZ(pName, tm_addressof(__val), PropMake(__val));
    }

    template<class T, class U>
    TMBool ExecEnum(const CWeakStr& pName, T& __val, const U* p)
    {
        return ExecZ(pName, &__val, p, CProp_Symbol_Enum<T,U>::PropMake());
//        return ExecX(pName, &__val, (CPropBoxEnumSymbol<T, U>*)0);
    }
    
    template<class T, class U>
    TMBool ExecMask(const CWeakStr& pName, T& __val, const U* p)
    {
        return ExecZ(pName, &__val, p, CProp_Symbol_Mask<T,U>::PropMake());
//        return ExecX(pName, &__val, (CPropBoxMaskSymbol<T, U>*)0);
    }

    TMBool ExecChar(const CWeakStr& pName, wchar_t& __val)
    {
        return ExecZ(pName, &__val, PropMakeChar(__val));
    }

    TMBool ExecText(const CWeakStr& pName, CString& __val)
    {
        return ExecZ(pName, &__val, PropMake(__val));
    }
    
    TMBool ExecFile(const CWeakStr& pName, CString& __val)
    {
        return ExecZ(pName, &__val, PropMakeFile(__val));
    }
    
//     TMBool ExecClass(const CWeakStr& pName, CPropMenu& __val)
//     {
//         return ExecZ(pName, &__val, PropMake(__val));
//     }
    template <class T>
    TMBool ExecClass(const CWeakStr& pName, T& __val)
    {
        return ExecZ(pName, tm_addressof(__val), PropMake(__val));
    }

    TMBool ExecOle(const CWeakStr& pName, CString& __val)
    {
        return ExecZ(pName, &__val, PropMakeOle(__val));
    }

    TMBool ExecClass(const CString& str, TSIZE<TMInt16>& __val)
    {
        ExecInt(str + ".cx", __val.cx);
        ExecInt(str + ".cy", __val.cy);
        return false;
    }

    TMBool ExecClass(const CString& str, BORDER& __val)
    {
        ExecInt(str + ".left", __val.left);
        ExecInt(str + ".top", __val.top);
        ExecInt(str + ".right", __val.right);
        ExecInt(str + ".bottom", __val.bottom);
        return false;
    }

    TMBool ExecClass(const CString& str, CTuiRect& __val)
    {
        ExecInt(str + ".left", __val.left);
        ExecInt(str + ".top", __val.top);
        ExecInt(str + ".right", __val.right);
        ExecInt(str + ".bottom", __val.bottom);
        ExecZ(str + ".width", &__val, PropMakeWidth(__val));
        ExecZ(str + ".height", &__val, PropMakeHeight(__val));
        return false;
    }

//     TMBool ExecClass(const CWeakStr& pName, CPropText& __val)
//     {
//         return ExecZ(pName, &__val, PropMake(__val));
//     }
//     
//     TMBool ExecClass(const CWeakStr& pName, GXPText& __val)
//     {
//         return ExecZ(pName, &__val, PropMake(__val));
//     }

    TMBool ExecClass(const CWeakStr& pName, CPropHotKey& __val)
    {
        CString str(pName);
        ExecMask(str + ".mask", __val.m_modifiers, (const CPropHotKey::CSymbol_Modifiers*)0);
        return ExecZ(str + ".key", &__val, PropMake(__val));
    }

//     TMBool ExecClass(const CWeakStr& pName, CPropColor& __val)
//     {
//         return ExecZ(pName, &__val, PropMake(__val));
//     }
// 
//     TMBool ExecClass(const CWeakStr& pName, GXFont& __val)
//     {
//         return ExecZ(pName, &__val, PropMake(__val));
//     }
};



TM_END_NAMESPACE

// TM::CEditStream& GetEditStream();

#endif // EDITSTREAM_H
