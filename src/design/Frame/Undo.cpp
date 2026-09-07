/*
 *  @file
 *  @brief
 *  undo manage
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2010.12.17
 *
 *  Copyright (C) 2010 miragekiller
 *  All rights reserved.
 */

#include "StdAfx.h"
#include "Undo.h"
#include "MainWnd.h"
#include "dui/object.h"
#include "ProjMgr.h"

using namespace TM;


void CEditOpt::SetDelObj(bool bNew, IObjEdit* obj)
{
    DbgAssert(obj);
    TMUInt64 nid = obj->GetId();
    (&m_strOldElement)[bNew].Format(TM_T("<L%lld />"), nid);
}

void CEditOpt::SaveObj(bool bNew, IObjEdit* obj)
{
    TM::CDomDocument doc;
    TM::IDomElement* p = obj->SaveTo(doc);
    if(p)
        p->transformNode((&m_strOldElement)[bNew]);
}

IObjEdit* _CUndo_CreateNewObject(CWndPanel* app, IObjEdit* pParent, const CDomStrView& pClass, TMRect* prc)
{
    CDomDocument doc;
    CComPtr<IDomElement> pElement = doc->createElement(TM_CL("L0"));
    pElement->setAttribute(TM_CL("class"), pClass);

    if (prc)
    {
        CString str;
        str.Format(L"%d,%d,%d,%d", prc->left, prc->top, prc->right, prc->bottom);
        pElement->setAttribute(TM_CL("Rect"), str);
    }
    
    IObjEdit* p = app->GetProj()->PasteTo(pParent, pElement);
    app->SelectItem(p);
    
    return p;
}

CUndo::CUndo()
    : m_nOptPos (0)
    , m_nSavePos(0)
    , m_bCut (false)
{
}

void CUndo::Clear()
{
    m_aOpt.clear();
    m_nOptPos = 0;
    DbgAssert(!m_doc->hasChildNodes());
}

TMBool CUndo::DoModify(CEditOpt& op)
{
    CDomDocument docOld;
    CDomDocument docNew;
    docOld->LoadXml(op.m_strOldElement);
    docNew->LoadXml(op.m_strNewElement);
    GetApp()->GetProj()->SyncAncestor(docNew->get_firstChild(), docOld->get_firstChild());
    op.m_strNewElement.swap(op.m_strOldElement);

    return true;
}

CWndPanel* CUndo::GetApp()
{
    return TM_OTHIS(CWndPanel, m_undo);
}


void CUndo::Undo()
{
    if (m_nOptPos<1)
        return;

    DbgAssert(m_nOptPos <= m_aOpt.size());
    --m_nOptPos;
    CEditOpt& op = m_aOpt[m_nOptPos];
    DoModify(op);
    GetApp()->GetMainWnd().SetWndTitle();
}

void CUndo::Redo()
{
    if (m_nOptPos >= m_aOpt.size())
        return;
    CEditOpt& op = m_aOpt[m_nOptPos];
    DoModify(op);
    ++m_nOptPos;
    GetApp()->GetMainWnd().SetWndTitle();
}

void CUndo::AddModify(CEditOpt& opt)
{
    // remove optpos - end
    m_aOpt.erase(m_aOpt.begin() + m_nOptPos, m_aOpt.end());

    ++m_nOptPos;
    m_aOpt.push_back(opt);
    GetApp()->GetMainWnd().SetWndTitle();
}

TMBool CUndo::MoveItem(IObjEdit* pInsert, IObjEdit* pPos, EMoveParam mp)
{
    if (!pInsert)
        return false;

    CEditOpt op;

    IObjEdit* itmAncestor = pPos->GetAncestorItem();
    op.SaveObjOld(itmAncestor);
    if (!pInsert->MoveItem(pPos, mp))
        return false;
    
    IObjEdit* itmNewAncestor = pInsert->GetAncestorItem();
    DbgAssert(itmNewAncestor);
    op.SaveObjNew(itmNewAncestor);

    AddModify(op);
    return true;
}

TMBool CUndo::PasteItem(TM::IDomNode* itmInsert, IObjEdit* pParent)
{
    CEditOpt eo;
    IObjEdit* itmAncestor;
    if (pParent)
    {
        itmAncestor = pParent->GetAncestorItem();
        DbgAssert(itmAncestor);
        eo.SaveObjOld(itmAncestor);
        if (!GetApp()->GetProj()->PasteTo(pParent, itmInsert))
            return false;
    }
    else
    {
        itmAncestor = GetApp()->GetProj()->PasteTo(0, itmInsert);
        if (!itmAncestor)
            return false;
        eo.SetDelObjOld(itmAncestor);
    }

    eo.SaveObjNew(itmAncestor);
    AddModify(eo);
    return true;
}


IObjEdit* CUndo::CreateNewItem(IObjEdit* pParent, const CDomStrView& pClassName, TMRect* prc)
{
    IObjEdit* itmAncestor;
    IObjEdit* pObj;
    CEditOpt eo;

    if (pParent < objEditMax) // create root object
    {
        pObj = _CUndo_CreateNewObject(GetApp(), pParent, pClassName, prc);
        itmAncestor = pObj;
        eo.SetDelObjOld(itmAncestor);
    }
    else
    {
        itmAncestor = pParent->GetAncestorItem();
        eo.SaveObjOld(itmAncestor);
        pObj = _CUndo_CreateNewObject(GetApp(), pParent, pClassName, prc);
    }

    DbgAssert(itmAncestor);
    eo.SaveObjNew(itmAncestor);
    AddModify(eo);

    return pObj;
}

void CUndo::RemoveItem(IObjEdit* itm)
{
    DbgAssert(itm);
    IObjEdit* itmAncestor = itm->GetAncestorItem();
    CEditOpt op;
    op.SaveObjOld(itmAncestor);
    if (itm == itmAncestor) // root item
    {
        op.SetDelObjNew(itmAncestor);
        GetApp()->ItemRemove(itm);// itm.Remove();
    }
    else
    {
        GetApp()->ItemRemove(itm);// itm.Remove();
        op.SaveObjNew(itmAncestor);
    }
    AddModify(op);
}

CStringW CUndo::BeginProp(IObjEdit* pObj)
{
    DbgAssert(pObj);
    IObjEdit* itmAncestor = pObj->GetAncestorItem();
    DbgAssert(itmAncestor);
    CEditOpt op;
    op.SaveObjOld(itmAncestor);
    return op.m_strOldElement;
}

void CUndo::EndProp(IObjEdit* pObj, const CStringW& key)
{
    DbgAssert(key);

    CEditOpt op;
    op.m_strOldElement = key;

    IObjEdit* itmAncestor = pObj->GetAncestorItem();
    DbgAssert(itmAncestor);
    op.SaveObjNew(itmAncestor);
    AddModify(op);
}

void CUndo::CopyBegin(TMBool bCut)
{
    m_bCut = bCut;
    if (m_pCopy)
        m_doc->removeChild(m_pCopy);
    m_pCopy = m_doc->createElement(L"copy");
}

void CUndo::CopyAdd(IObjEdit* itm)
{
    DbgAssert(m_pCopy);
    if (itm)
        itm->SaveTo(m_pCopy);
}

void CUndo::Paste(IObjEdit* pParent)
{
    if (!m_pCopy)
        return;

    for (CDomNodePtr ptr(m_pCopy->get_firstChild()); ptr; ptr = ptr->get_nextSibling())
    {
        if (m_bCut)
        {
            int nID = GetDomObjectID(ptr);
            IObjEdit* p = GetApp()->GetDataByObjectID(nID);
            if (!p) // the object have deleted, like: cut first, then delete, then paste.
                return;
            if (MoveItem(p, pParent, eZOrderChildLast))
                m_pCopy.Release();
        }
        else
        {
            PasteItem(ptr, pParent);
        }
    }
}

void CUndo::TxtEditSave(IObjEdit* p, const CString& str)
{
    TM::CDomDocument docOld, docNew;
    IDomElement* pOldElement = p->SaveTo(docOld);

    docNew->LoadXml(str);
    CDomElementPtr ptrNew(docNew->get_firstChild());

    GetApp()->GetProj()->SyncAncestor(pOldElement, ptrNew);

    CEditOpt op;
    pOldElement->transformNode(op.m_strOldElement);
    ptrNew->transformNode(op.m_strNewElement);
    AddModify(op);
}



