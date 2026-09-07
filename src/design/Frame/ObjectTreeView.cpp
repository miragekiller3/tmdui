
//-----------------------------------------------------------
//CtrlPanel.cpp

//  Copyright (C) 2004 miragekiller
//  All rights reserved      
//
//
//  Author: 
//      miragekiller    <3916345933@qq.com>
//
//  Histroy :
//      9/30/2006       create
//-------------------------------------------------------------

#include "StdAfx.h"
#include "ObjectTreeView.h"
#include "MainWnd.h"
#include "PropertyDlg.h"
#include "ProjMgr.h"
#include "tmdrag.h"
#include "tmkeyboard.h"

using namespace TM;



#define INDENT 14

static TM::HTVITEM CheckItem (TM::TUIHITINFO& hi, int nLevel = 2)
{
    if (hi.pTaget && !hi.bNC)
    {
        CObjItem itm ((TM::HTVITEM)hi.pTaget);
        int i = itm.GetLevel();
        if (i > nLevel)
        {
            if (i == 1 && nLevel == 1)
            {
                if (!itm.GetPrev())
                {
                    return itm;
                }
            }
            else
            {
                return itm;
            }
        }
    }

    return 0;
}

CTVCtrl::CTVCtrl()
    : m_nSel(0)
{
}

void CTVCtrl::Init(TM::HTUI hTree, IProj* proj)
{
    SubclassWindow(hTree);
    RegisterEventSink(this);
    LoadExistentResource(proj);

//     m_ptrEditWnd.CreateInstance()->Init(&GetPanel());
}

CWndPanel& CTVCtrl::GetPanel()
{
    return *TM_OTHIS(CWndPanel, m_ctrlTree);
}

TMResult CTVCtrl::OnMouseMove(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    TM::TUIHITINFO hi;
    if (GetAPP()->GetMouseLDownState(hi) && hi.hWnd == m_hWnd)
    {
        TM::HTVITEM hItem = CheckItem (hi);
        if (hItem)
        {
            TM::CDragData dg;
            dg.AddMem(0, 0, DRAG_TV_ITEM);
            DoDrag(dg, eDragEffMove | eDragEffLink);
        }
    }
    return 0;
}

TMResult CTVCtrl::OnDragEntry(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    TM::DROPEVENT* pEvent = (TM::DROPEVENT*)lParam;
    if (pEvent->pData->Query(DRAG_TV_ITEM))
    {
        return eDragEffMove;
    }

    return 0;
}

TMResult CTVCtrl::OnDragXXX(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    TM::DROPEVENT* pEvent = (TM::DROPEVENT*)lParam;

//    if (pEvent->pData->Query(DRAG_TV_ITEM))
    {
        TM::TUIHITINFO hi = {0};
        hi.hWnd = m_hWnd;
        hi.pt = pEvent->ptWindow;
        SendMessage (CM_HITTEST, 0, (TMLParam)&hi);

        CObjItem itm (CheckItem (hi, 1));

        if (itm) //itmSel && itm && itm != itmSel && !itmSel.IsChild(itm))
        {
            IObjEdit* p1 = itm.GetData();
            DbgAssert(p1);

            typedef TM::CVector<CObjItem> CItmList;
            CItmList aItem;
            CObjItem itmp = GetObjOwner(TM::e_otWnd);
            
            CKeyInput key = _tuiApp.GetKeyInput();
            TMBool shift = key.IsModifiersShift();
            TMBool control = key.IsModifiersControl();
            if ((shift || control) && (itm.GetLevel() < 2)) // append mode not support manager
                return 0;

            int nMode = (shift || control) ? eDragEffLink : eDragEffMove;

            for (CObjItem itmc(itmp.GetChild()); itmc; itmc = itmc.GetGlobalNext(itmp))
            {
                if (itmc.IsSelected())
                {
                    if (itmc == itm)
                        return 0;
                    if (!itmc.GetData())
                        return 0;
                    if (uMsg == CM_DRAGDROP)
                        aItem.push_back(itmc);
                }
            }

            if (uMsg == CM_DRAGDROP)
            {
                for (CItmList::reverse_iterator it = aItem.rbegin(); it != aItem.rend(); ++it)
                {
                    CObjItem itmc = *it;
//                    IObjEdit* p2 = itmc.GetData();
                    if (shift || control)
                        GetPanel().GetUndo().MoveItem(itmc.GetData(), itm.GetData(), eZOrderDown);
                    else
                        GetPanel().GetUndo().MoveItem(itmc.GetData(), itm.GetData(), eZOrderChildFirst);
                }
            }

            return nMode;
        }
    }

    return 0;
}

CObjItem CTVCtrl::GetObjOwner(TM::EObjType tp)
{
    DbgAssert(tp != e_otWgt);
    DbgAssert(tp >= 0 && tp < tm_countof(m_ItemArray));
    return m_ItemArray[tp < e_otWgt ? tp : tp - 1];
}

void CTVCtrl::LoadExistentResource(IProj* proj)
{
    CObjItem tv(GetRootItem());
    tv.RemoveChild();
    TMCStr* names = proj->GetObjRootNames();
    DbgAssert(names);
    for (int i = 0; names[i]; ++i)
        m_ItemArray[i] = tv.AppendChild(names[i]);
}


CObjItem CTVCtrl::GetSelectedItem(TMBool bGetOwner)
{
    return GetValidItem((*this ? GetSelectedItem() : 0), bGetOwner);
}

CObjItem CTVCtrl::GetValidItem(TM::CTVItem ti, TMBool bGetOwner)
{
    if (!ti)
        return 0;
    int nLevel = ti.GetLevel();
    if (nLevel < 2)
        return 0;
    if (bGetOwner)
    {
        for (; nLevel > 2; --nLevel)
            ti = ti.GetParent();
    }
    return CObjItem(ti);
}



//////////////////////////////////////////////////////////////////////////////
// ITVEvent
void CTVCtrl::OnDeleteItem(TM::HTVITEM hItem)
{
    CObjItem itm(hItem);
    if (itm.IsSelected())
        --m_nSel;

    IObjEdit* pData = itm.GetData();
    if (pData)
    {
        GetPanel().GetMainWnd().GetProperty().RemoveObj(pData);
        pData->Release();
    }
}

void CTVCtrl::OnSelChanged(TM::HTVITEM hOld, TM::HTVITEM hNew, TMUInt nKey)
{
//    TMHWnd hOldW = 0, hNewW = 0;
//    IObjEdit* pNewF = 0;

    CObjItem itmOld(hOld);
    IObjEdit* pOldMgr = 0;
    if (itmOld)
    {
        CObjItem itmOldMgr = itmOld.GetOwner();
        if (itmOldMgr)
            pOldMgr = itmOldMgr.GetData();
    }

    CObjItem itmNew(hNew);
    IObjEdit* pNew = 0;
    IObjEdit* pNewMgr = 0;
    if (itmNew)
    {
        pNew = itmNew.GetData();
        CObjItem itmNewMgr = itmNew.GetOwner();
        if (itmNewMgr)
            pNewMgr = itmNewMgr.GetData();
    }

//     GetPanel().GetProj()->SetCurrentObject(pNew);

    if (pNew)
    {
//         GetPanel().GetMainWnd().OnChangeCurCtrl(pNew->GetType());
        GetPanel().GetMainWnd().AddGoto(pNew->GetId());
        CTuiRect rc;
        if (pNewMgr->GetObjRect(rc))
            GetPanel().SetScrollRange(rc.Width() + 10, rc.Height() + 10);
    }

    // edited object container window
    if ((pOldMgr != pNewMgr) || !pNew)
    {
        if (m_ptrEditPic.IsWindow())
            m_ptrEditPic->DestroyWindow();
        m_ptrEditPic.Release();
        
        if (m_ptrEditFrm.IsWindow())
            m_ptrEditFrm->DestroyWindow();
        m_ptrEditFrm.Release();
        
        if (m_ptrEditWnd.IsWindow())
            m_ptrEditWnd->DestroyWindow();

        if (pNew)
        {
            TM::EObjType eType = pNew->GetType();
            if (eType == e_otFrm)
            {
                m_ptrEditFrm.CreateInstance()
                    ->Init(&GetPanel())
                    ->Create(GetPanel());
            }
            else if (eType == e_otWgt || eType == e_otWnd)
            {
                m_ptrEditWnd.CreateInstance()
                    ->Init(&GetPanel())
                    ->Create(GetPanel());
            }
            else // pic or other
            {
                m_ptrEditPic.CreateInstance()
                    ->Init(&GetPanel())
                    ->Create(GetPanel());
            }
        }
    }
    
    GetPanel().GetMainWnd().SyncTxtEdit(pOldMgr!=pNewMgr);
    GetPanel().GetMainWnd().SetWndTitle();
}

void CTVCtrl::OnStateChanged(TM::HTVITEM hItem, TMUInt uOld, TMUInt uNew)
{
    if ((uOld & TVIS_SELECTED) == (uNew & TVIS_SELECTED))
        return;

    if (uOld & TVIS_SELECTED) // remove
        --m_nSel;
    else
        ++m_nSel;
    
    CObjItem itm(hItem);
    IObjEdit* pT = itm.GetData();
    if (pT)
    {
        if (uOld & TVIS_SELECTED) // remove
            GetPanel().GetMainWnd().GetProperty().RemoveObj(pT);
        else
            GetPanel().GetMainWnd().GetProperty().SetObj(pT);
    }
}