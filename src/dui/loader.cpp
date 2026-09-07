/*
 *  @file
 *  @brief  xml configuration file management
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.09.19
 *
 *  Copyright (C) 2006 miragekiller
 */

#include "loader.h"
#include "pic.h"
#include "frame.h"
#include "../tmlist.h"
// #include "menu.h"

#include <string>
#include <map>


TM_BGN_NAMESPACE


///////////////////////////////////////////////////////////////////////////////
// CLoadStream
template<class L>
void AddMap(L& lst, IDomElement* tag)
{
    CComPtr<IDomNodeList> pList(tag->get_attributes());
    tm_for (int i = 0; ; ++i)
    {
        CComQIPtr<IDomAttr> node(pList->item(i));
        if (!node)
            break;
        lst[node->get_name()] = node->get_value();
    }
}

CLoadStream::CLoadStream(CModule& mdl, IDomElement* tag, TMUInt32 dwFlag, int dpi)
    : m_mdl     (mdl)
    , m_nDPI    (dpi)
    , m_xml     (tag)
    , m_dwFlag  (dwFlag)
{
    AddMap(m_map, tag);
}

void CLoadStream::ExecChilds(HTUI hParent) const
{
    for (IDomNode* itm = m_xml->get_firstChild(); itm; itm = itm->get_nextSibling())
    {
        int nId = GetDomObjectID(itm);
        if (m_dwFlag & SS_CHECKCHILD)
        {
            CTuiWgt* pc = CWidgetHelper::FindChild(hParent, nId);
            if (pc)
            {
                CDomElementPtr ptr(itm);
                pc->LoadAll(ptr, m_dwFlag);
                continue;
            }
        }
        
        if (m_dwFlag & SS_CHILD)
            CWinPtr::CreateNative(hParent->GetModule(), hParent, nId, m_nDPI);
    }
}

void CLoadStream::GetProp(const CWeakStr& name, void* pMem, IPropItemLoader* loader) const
{
    CValMap::const_iterator iter = m_map.find(name);
    if (iter == m_map.end())
        return;
    loader->Str2T(this, iter->second, pMem);
//    return true;
}


TM_END_NAMESPACE


