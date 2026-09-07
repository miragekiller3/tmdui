/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021.4.29
 *
 *  Copyright (C) 2021 miragekiller
 */

#include "StdAfx.h"
#include "Hash.h"
#include "Proxy.h"

using namespace TM;
using namespace TM::PRI;

struct CHashList
{
    TM_SELF_ALLOC_F(CHashList);

#ifdef TM_DBG
    int IC_GetDebugInfo(TMDbgClassInfo* cls, char* pbuf) // CStrPtrData
    {
        if (pbuf)
        {
            TM::CStringA str;
            str.Format("count: %d", m_aWidget.size());
            str.CopyTo(pbuf);
        }
        return IC_Size(m_aWarpper) + IC_Size(m_aWidget);
    }
#endif // TM_DBG

    typedef FLATSET(_HashObject, AccHash32, ->m_nHash)      CWarpperList;
    typedef TM::CMap<HTUI, _HashObject>                     CWidgetList;

    CWarpperList            m_aWarpper;
    CWidgetList                m_aWidget;
    int                     m_nIndex;

    static CHashList*       s_pAccMap;
    static CAccessibleObj   s_objItemSel;   // tree, list
    static CAccessibleObj   s_objItemFocus; // tree, list

private:
    CHashList()
        : m_nIndex(hash_min)
    {
    }

public:
    ~CHashList()
    {
        ACC_TRACE("CHashList::~CHashList()");
    }

    static CHashList* GetInst()
    {
        if (!s_pAccMap)
            s_pAccMap = new CHashList();
        return s_pAccMap;
    }

    _HashObject CreateWarp(HTUI hWgt)
    {
        DbgAssert(!Find(hWgt));
        _HashObject warp = new CRefCAccessibleObj(1); // the ref release when WM_NCDESTROY
        m_aWidget[hWgt] = warp;
        warp->m_hWnd = hWgt;
        warp->m_nHash = m_nIndex;
        m_aWarpper.insert(warp);
        do
        {
            ++m_nIndex;
            if (m_nIndex >= hash_max)
                m_nIndex = hash_min;
        }
        while(Get(m_nIndex));
        ACC_TRACE(L"CHashList::CreateWarp(%s)", _acc_dinfo(*warp).c_str());
        return warp;
    }

    _HashObject Add(HTUI hWgt)
    {
        DbgAssert(hWgt);
        _HashObject pair = Find(hWgt);
        if (pair)
            return pair;
        
        if (!tuiIsValid(hWgt))
        {
            ACC_TRACE("CHashList::Add(HTUI:%p, hash:0x%X) !tuiIsValid()", hWgt, 0);
            return 0;
        }

        _HashObject warp = CreateWarp(hWgt);
        ACC_TRACE("CHashList::Add(HTUI:%p, hash:0x%X)", hWgt, warp->m_nHash);
        return warp;
    }

    _HashObject Get(AccHash32 hash)
    {
        if (hash == hash_sel_item_0 || hash == hash_sel_item_1)
            return &s_objItemSel;
        if (hash == hash_focus_item)
            return &s_objItemFocus;

        CWarpperList::iterator iter = m_aWarpper.find(hash);
        if (iter == m_aWarpper.end())
            return 0;
        return *iter;
    }

    _HashObject Find(HTUI hWgt)
    {
        DbgAssert(hWgt);
        CWidgetList::recorder rec(m_aWidget);
        rec.find(hWgt);
        if (!rec)
            return 0;
        _HashObject p = rec.get_data();
        return p;
    }

    void Erase(HTUI hWgt)
    {
        DbgAssert(hWgt);
        if (s_objItemSel.m_hWnd == hWgt)
            s_objItemSel.m_hWnd = 0;
        if (s_objItemFocus.m_hWnd == hWgt)
            s_objItemFocus.m_hWnd = 0;

        CWidgetList::recorder rec(m_aWidget);
        rec.find(hWgt);
        if (!rec)
        {
            ACC_TRACE("CHashList::erase(HTUI:%p) fail", hWgt);
            return;
        }
        
        _HashObject p = rec.get_data();
        ACC_TRACE("CHashList::erase(HTUI:%p, hash:0x%X)", hWgt, p->m_nHash);
        CWarpperList::iterator iter = m_aWarpper.find(p->m_nHash);
        m_aWarpper.erase(iter);
        rec.erase();
        p->m_hWnd = 0;
        p->GetUnknown()->Release();
    }

    TMBool IsClean() const
    {
        return (0 == m_aWidget.size());// && m_aUnusedAccList.empty();
    }

    static _HashObject MakeItem(CAccessibleStyle* src, CAccessibleObj& p, HTUI hWnd, long sel, AccHash32 hash, CAccessibleStyleItemBase& style)
    {
        p.m_hWnd = hWnd;
        p.m_nHash = hash;
        p.m_ptr = &style;
        style.SelSel(src, sel);
        return &p;
    }
};

CHashList* CHashList::s_pAccMap;
CAccessibleObj CHashList::s_objItemSel; // tree, list
CAccessibleObj CHashList::s_objItemFocus; // tree, list


_HashObject CHash::Find(_HashInt id)
{
    if (CHashList::s_pAccMap)
        return CHashList::GetInst()->Get(id);
    return 0;
}

_HashObject CHash::Find(HTUI hWnd)
{
    if (CHashList::s_pAccMap)
        return CHashList::s_pAccMap->Find(hWnd);
    return 0;
}

_HashObject CHash::Make(HTUI hWnd)
{
    DbgAssert(hWnd);
    return CHashList::GetInst()->Add(hWnd);
}

void CHash::Erase(HTUI p)
{
    if (CHashList::s_pAccMap)
    {
        CHashList::s_pAccMap->Erase(p);
        if (CHashList::s_pAccMap->IsClean())
        {
            delete CHashList::s_pAccMap;
            CHashList::s_pAccMap = 0;
        }
    }
}

TMBool CHash::IsClean()
{
    return 0 == CHashList::s_pAccMap;
}

//_HashObject CHash::MakeSelItem(HTUI hWnd, long sel, CAccessibleStyleItemBase& style)
//{
//    static int n = 0;
//    n = 1 - n;
//    return CHashList::MakeItem(CHashList::s_objItemSel, hWnd, sel, hash_sel_item_0 + n, style);
//}

_HashObject CHash::MakeFocusItem(CAccessibleStyle* src, HTUI hWnd, long sel, CAccessibleStyleItemBase& style)
{
    static int n = 0;
    n = 1 - n;
    return CHashList::MakeItem(src, CHashList::s_objItemSel, hWnd, sel, hash_sel_item_0 + n, style);
//    return CHashList::MakeItem(src, CHashList::s_objItemFocus, hWnd, sel, hash_focus_item, style);
}

_HashObject CHash::GetFocusItem()
{
    return &CHashList::s_objItemSel;
}


