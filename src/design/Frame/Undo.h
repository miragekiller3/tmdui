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

#ifndef UNDO_H
#define UNDO_H

#include "../Design.h"

TM_BGN_NAMESPACE
class CTuiObject;
TM_END_NAMESPACE


// class CPanelApp;
class CWndPanel;

// 
// // move: m_mp && m_pOldElement
// // create: m_mp && m_pNewElement
// // modify: m_pOldElement && m_pNewElement
// // delete: m_pOldElement
// struct CEditOpt
// {
// public:
//     TMUInt32            m_idPos;
//     EMoveParam          m_mp;
//     TM::CDomElement*    m_pOldElement;
//     TM::CDomElement*    m_pNewElement;
// 
//     CEditOpt(TM::CDomElement* pOld, TM::CDomElement* pNew, TMUInt32 idPos = 0, EMoveParam mp = e_mpNull)
//         : m_idPos (idPos)
//         , m_mp (mp)
//         , m_pOldElement(pOld)
//         , m_pNewElement(pNew)
//     {
//     }
// 
//     void MakePos(CObjItem itm);
//     void MakePos(CObjItem itm, EMoveParam mp);
// };


struct CEditOpt
{
//     CComQIPtr<TM::IDomElement>  m_pOldElement;
//     CComQIPtr<TM::IDomElement>  m_pNewElement;
    TM::CString m_strOldElement;
    TM::CString m_strNewElement;

private:
    //void BeginNewObject(TM::CDomDocument& doc, IObjEdit* obj);
    void SetDelObj(bool bNew, IObjEdit* obj);
    void SaveObj(bool bNew, IObjEdit* obj);

public:
    void SaveObjOld(IObjEdit* obj) { SaveObj(false, obj); }
    void SaveObjNew(IObjEdit* obj) { SaveObj(true, obj); }
    void SetDelObjOld(IObjEdit* obj) { SetDelObj(false, obj); }
    void SetDelObjNew(IObjEdit* obj) { SetDelObj(true, obj); }
};


class CUndo
{
private:
    std::vector<CEditOpt>       m_aOpt;
    int                         m_nOptPos;
    int                         m_nSavePos;

    TM::CDomDocument            m_doc;
    CComQIPtr<TM::IDomElement>  m_pCopy;
    TMBool                      m_bCut;

private:
    void AddModify(CEditOpt& opt);
    TMBool DoModify(CEditOpt& opt);
    CWndPanel* GetApp();

public:
    CUndo();

    void Clear();
    void Undo();
    void Redo();
    TMBool IsModified () const { return m_nSavePos != m_nOptPos; }
    void ClearModifyed() { m_nSavePos = m_nOptPos; }

    // call after set parent or move z_order
    // after call, user need call SetCurrentObject.
    TMBool MoveItem(IObjEdit* itmInsert, IObjEdit* itmPos, EMoveParam mp);
    TMBool PasteItem(TM::IDomNode* itmInsert, IObjEdit* pParent);
    
    // call after create new object
    IObjEdit* CreateNewItem(IObjEdit* pParent, const CDomStrView& pClassName, TMRect* prc = 0);

    // call before delete object
    void RemoveItem(IObjEdit* itm);

    // for update prop
    TM::CStringW BeginProp(IObjEdit* pObj);
    void EndProp(IObjEdit* pObj, const TM::CStringW& key);

    void CopyBegin(TMBool bCut);
    void CopyAdd(IObjEdit* itm);
    void Paste(IObjEdit* pParent);

    // use XML string update object.
    void TxtEditSave(IObjEdit* pObj, const TM::CString& str);
};


#endif // UNDO_H