/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2015.01.04
 *
 *  Copyright (C) 2015 miragekiller
 */

#ifndef PROPITEM_H
#define PROPITEM_H


#include "tmhook.h"
#include "../../dui/property.h"


class CPanelApp;


TM_BGN_NAMESPACE


////////////////////////////////////////////////////////////////////////////////
// CPropBoxBase
// one property style <-> one instance
class CPropBoxBase
    : public IPropBox
    , public TM::CHookKeyboardLLImpl<CPropBoxBase> // for hotkey
{
public:
    CPanelApp* m_pApp;
    CString m_strName;
    CString m_strHint;
    CText m_txtName;
    CText m_txtValue;

    CPropBoxBase();
    virtual ~CPropBoxBase() { RemoveHook(); }
    void FinalConstruct() {}

    void Draw2(TMCStr pText, const CRectF& rc, TMBool bSelected);

    static long T2L(const CString& str) { long x = 0; XmlStrToLong(str, x); return x; }
    TM::CString ID2Text(TMUInt32 dwID);
    TMUInt32 Text2ID(const TM::CString& str, EDIT_MODE eAutoCreateFlag);
    void InitPop(EDIT_MODE nType);
    IProjMgr* GetProjMgr();

    // self
    virtual CStringW BeginProp(IObjEdit* p);
    virtual void EndProp(IObjEdit* p, const CStringW& key);
    virtual TMBool _Match(const TM::CStrView& strItemTxt, const TM::CStrView& strFilter, int& nRootLevel) tm_override
    {
        nRootLevel = 1;
        int n = this->Matching(strFilter);
        return n;
    }

    // CHookKeyboardLLImpl
    TMBool OnKeyboardLL(UINT uMsg, KBDLLHOOKSTRUCT* ps);

    // IPropBox
    virtual void DrawName(const CRectF& rc, TMBool bSelected) tm_final;    
    virtual TM::CStrView GetPropHint() tm_final { return m_strHint; }
    virtual TMUInt32 GetLocateID() { return 0; }
    virtual EPopable InitPopable(TMBool bSelect) { return ePopableNone; }
    virtual void Release() tm_final { delete this; }
};


////////////////////////////////////////////////////////////////////////////////
// CPropBox
class CPropBox2
    : public CPropBoxBase
    , public IEditEnv
{
public:
    TM_SELF_ALLOC(CPropBox2);

protected:
    typedef CMap<void*, IObjEdit*>   CValList;
    typedef CValList::recorder      CValRec;
    CValList                        m_aVal;

public:
    IPropItemEdit*                  m_prop;

    typedef CPropBoxBase TBase;

private:
    void CheckSameVal()
    {
        if (m_aVal.size())
        {
            CValRec rec(m_aVal);
            void* p = rec.get_key();
            for (; ++rec;)
            {
                if (!m_prop->Edit_Equal(rec.get_key(), p))
                {
                    m_bSameVal = false;
                    return;
                }
            }
            m_bSameVal = true;
        }
        else
            m_bSameVal = false;
    }

public:
    ~CPropBox2()
    {
    }

    // IEditEnv
    virtual CModule& GetModule() const;

    // IPropBox
    virtual TMUInt32 GetLocateID() tm_final
    {
        for (CValRec rec(this->m_aVal); rec; ++rec)
        {
            void* p = rec.get_key();
            TMUInt32 id = m_prop->Obj2ID(p);
            if (id)
                return id;
        }
        return 0;
    }

    virtual void DrawValue (const CRectF& rc) tm_final
    {
        CString str;
        if (m_bSameVal)
        {
            void* p = (*m_aVal.begin()).first;
            T2Text(p, str);
        }
        Draw2(str, rc, false);
    }

    virtual int Matching(const TM::CStrView& str)
    {
        return (str.IsEmpty() || -1 != m_strName.FindI(str)) ? m_aVal.size() : 0;
    }

    virtual TMBool T2Editer(TM::CString& str)
    {
        if (m_bSameVal)
        {
            void* val = (*m_aVal.begin()).first;
            T2Text(val, str);
        }
        return m_bSameVal;
    }

    virtual TMBool Editer2T(const TM::CString& str)
    {
        if (!m_bSameVal && str.IsEmpty())
            return false;

        TMBool b = false;
        for (CValRec rec(m_aVal); rec; ++rec)
        {
            TM::CString str2;
            T2Text(rec.get_key(), str2);
            if (str2 != str)
            {
                b = true;
                IObjEdit* pObj = rec.get_data();
                CStringW pKey(BeginProp(pObj)); // _Module.GetIMainWnd()->BeginProp(pObj);

                TM::CDomDocument doc;
                CDomElementPtr pOld = pObj->SaveTo(doc);
                Text2T(rec.get_key(), str, pObj);
                CDomElementPtr pNew = pObj->SaveTo(doc);
                pObj->OnLoad(pOld, pNew);

                EndProp(pObj, pKey); // _Module.GetIMainWnd()->EndProp(pObj, pKey);
                pObj->InvalidateObj();
//                 Editer2T
            }
        }

//         if (b)
//             ChangeUI();

        CheckSameVal();
        return b;
    }

    virtual TMBool ShowPopup(TM::CString& str) tm_override
    {
        return m_prop->Edit_DoDialog(this->GetProjMgr(), str);
    }

    virtual EPopable InitPopable(TMBool bSelect) tm_override
    {
        return m_prop->Edit_InitPopable(this, bSelect);
    }

    // 
    virtual void Text2T(void* p, const TM::CString& str, IObjEdit* pObj)
    {
        m_prop->Edit_String2T(this, str, p);
    }

    virtual void T2Text(void* p, CString& str)
    {
        str = m_prop->Edit_T2String(this, p);
    }

    void AddVal(void* p, IObjEdit* pOwner)
    {
        m_aVal.insert2(p, pOwner);
        CheckSameVal();
    }

    virtual int RemoveOwner(IObjEdit* pOwner) tm_final
    {
        for (CValRec rec(m_aVal); rec; )
        {
            if (rec.get_data() == pOwner)
                rec.erase();
            else
                ++rec;
        }
        //if (m_aVal.size() == 0)
        //    m_bSameVal = false;
        CheckSameVal();
        return m_aVal.size();
    }
};


////////////////////////////////////////////////////////////////////////////////
// CPropBoxT
template <class T, EPopable nShowPop = ePopableButton>
class CPropBoxT
    : public CPropBoxBase
{
public:
    TM_SELF_ALLOC(CPropBoxT);

protected:
    typedef CMap<T, IObjEdit*>           CValList;
    typedef typename CValList::recorder CValRec;
    CValList m_aVal;


    typedef CPropBoxBase TBase;

    CPropBoxT()
        : CPropBoxBase()
    {
    }

private:
    void CheckSameVal()
    {
        if (m_aVal.size())
        {
            CValRec rec(m_aVal);
            T p = rec.get_key();
            for (; ++rec;)
            {
                if (!MatchT(rec.get_key(), p))
                {
                    m_bSameVal = false;
                    return;
                }
            }
            m_bSameVal = true;
        }
        else
            m_bSameVal = false;
    }

public:
    // IPropBox
    virtual void DrawValue (const CRectF& rc) tm_final
    {
        CString str;
        if (m_bSameVal)
        {
            T p = (*m_aVal.begin()).first;
            T2Text (p, str);
        }
        Draw2 (str, rc, false);
    }

    virtual EPopable InitPopable(TMBool bSelect) tm_override
    {
        return nShowPop;
    }

    virtual int Matching(const TM::CStrView& str)
    {
        return (str.IsEmpty() || -1 != m_strName.FindI(str)) ? m_aVal.size() : 0;
    }

    virtual TMBool MatchT(T p1, T p2)
    {
        CString str1;
        CString str2;
        T2Text(p1, str1);
        T2Text(p2, str2);
        return str1 == str2;

//         return _MatchT(p1, p2);
    }

    virtual TMBool T2Editer(TM::CString& str)
    {
        if (m_bSameVal)
        {
            T val = (*m_aVal.begin()).first;
            T2Text(val, str);
        }
        return m_bSameVal;
    }

    virtual TMBool Editer2T(const TM::CString& str)
    {
        if (!m_bSameVal && str.IsEmpty())
            return false;

        TMBool b = false;
        for (CValRec rec(m_aVal); rec; ++rec)
        {
            TM::CString str2;
            T2Text(rec.get_key(), str2);
            if (str2 != str)
            {
                b = true;
                IObjEdit* pObj = rec.get_data();
                CStringW pKey(BeginProp(pObj)); // _Module.GetIMainWnd()->BeginProp(pObj);

                TM::CDomDocument doc;
                CDomElementPtr pOld = pObj->SaveTo(doc);
                Text2T(rec.get_key(), str, pObj);
                CDomElementPtr pNew = pObj->SaveTo(doc);
                pObj->OnLoad(pOld, pNew);

                EndProp(pObj, pKey); // _Module.GetIMainWnd()->EndProp(pObj, pKey);
                pObj->InvalidateObj();
//                 Editer2T
            }
        }

//         if (b)
//             ChangeUI();

        CheckSameVal();
        return b;
    }

    virtual void Text2T(T p, const TM::CString& str, IObjEdit* pObj) = 0;
    virtual void T2Text(T p, CString& str) = 0;

    void AddVal(T p, IObjEdit* pOwner)
    {
        m_aVal.insert2(p, pOwner);
        CheckSameVal();
    }

    virtual int RemoveOwner(IObjEdit* pOwner) tm_final
    {
        for (CValRec rec(m_aVal); rec; )
        {
            if (rec.get_data() == pOwner)
            {
                rec.erase();
                CheckSameVal();
                break;
            }
            else
                ++rec;
        }
        //if (m_aVal.size() == 0)
        //    m_bSameVal = false;
        return m_aVal.size();
    }

};


TM_END_NAMESPACE

#endif // PROPITEM_H
