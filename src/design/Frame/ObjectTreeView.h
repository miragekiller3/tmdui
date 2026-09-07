//-----------------------------------------------------------
//CtrlPanel.h

//  Copyright (C) 2004 miragekiller                               
//  All rights reserved                                                     
//
//
//  Author: 
//      miragekiller    <3916345933@qq.com>
//
//    Histroy :
//      9/29/2006       create
//        10/20 ~ now         modify                Evan Zhu
//-------------------------------------------------------------

#pragma once 

#include "TVRender.h"
#include "WndObjEdit.h"
#include "../Design.h"


class CObjItem : public TM::CTVItem
{
public:
    typedef TM::CTVItem base_class;
    struct _FindById { FIND_OP(->GetId()) };
    
    CObjItem(TM::HTVITEM itm = 0) : base_class (itm) {}
    
    operator TM::HTVITEM() const { return m_hItem; }

//     HOBJITEM ToObjItem() { return (HOBJITEM)m_hItem; }
    
    CObjItem operator = (TM::HTVITEM hItem) { m_hItem = hItem; return *this; }
    
    IObjEdit* operator *() { return GetData(); }
    
    CObjItem& operator ++()
    {
        m_hItem = GetGlobalNext();
        return *this;
    }
    
    template < class _F, class _K >
        CObjItem& find(_F __func, const _K& __key)
    {
        for (; *this; ++*this)
        {
            IObjEdit* p = **this;
            
            if (p && __func(p, __key))
            {
                break;
            }
        }
        
        return *this;
    }
    
    CObjItem GetLevelRoot(int n)
    {
        int n1 = GetLevel();
        if (n1 < n)
            return 0;
        
        CObjItem itm(m_hItem);
        for (; n1 != n; --n1)
            itm = itm.GetParent();
        
        return itm;
    }

    void SetData(IObjEdit* pData, TMBool bRedraw = true)
    {
        DbgAssert(pData->GetTvData() == m_hItem);
        CTVItem::SetData((void*)pData, bRedraw);
    }

    IObjEdit* GetData()
    {
        IObjEdit* p = (IObjEdit*)CTVItem::GetData();
        if (p)
            DbgAssert(p->GetTvData() == m_hItem);
        return p;
    }
    
    CObjItem GetOwner() { return GetLevelRoot(2); }
//     CObjItem GetTypeRoot() { return GetLevelRoot(1); }
    
};

class CWndPanel;

class CTVCtrl
    : public TM::CTuiWidgetImpl<CTVCtrl, CTVCtrlRender>
    , public TM::ITVEvent
{
public:
    typedef TM::CTuiWidgetImpl<CTVCtrl, CTVCtrlRender> root_class;
    CObjItem            m_ItemArray[10];
    int                 m_nSel;
    CWndPicEdit::CPtr   m_ptrEditPic;
    CWndFrmEdit::CPtr   m_ptrEditFrm;
    CWndWinEdit::CPtr   m_ptrEditWnd;

public:
    BEGIN_TUIMSG_MAP(CTVCtrl)
//         MESSAGE_HANDLER     (CM_HITTEST,        OnHitTest)
//         MESSAGE_HANDLER     (CM_DRAWITEM,       OnDrawItem)
        MESSAGE_HANDLER     (WM_MOUSEMOVE,      OnMouseMove)
//         MESSAGE_HANDLER     (CM_MOUSESWITCH,    OnMouseSwitch)
        MESSAGE_HANDLER     (CM_DRAGENTRY,      OnDragEntry)
        MESSAGE_HANDLER     (CM_DRAGOVER,       OnDragXXX)
        MESSAGE_HANDLER     (CM_DRAGDROP,       OnDragXXX)
//         MESSAGE_HANDLER     (WM_NCMOUSEMOVE,    OnNcMouseMove)
        CHAIN_MSG_MAP       (CTVCtrlRender)
    END_TUIMSG_MAP()
    
//     TMResult OnHitTest (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnDrawItem (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnMouseMove (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDragEntry (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDragXXX (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

public:
    CTVCtrl();
    void Init(TM::HTUI hTree, IProj* proj);
    CWndPanel& GetPanel();
    using root_class::GetSelectedItem;
    CObjItem GetSelectedItem(TMBool bGetOwner);
    CObjItem GetValidItem(TM::CTVItem ti, TMBool bGetOwner);
//     int GetSelCount() const { return m_nSel; }

public:
    CObjItem GetObjOwner(TM::EObjType tp);
    CObjItem GetObjOwner(IObjEdit* p) { DbgAssert(p >= objEditMgr && p < objEditMax); return m_ItemArray[(INT_PTR)p - 1]; }
    void LoadExistentResource(IProj* proj);

    // ITVEvent
    virtual void OnDeleteItem (TM::HTVITEM hItem);
    virtual void OnSelChanged (TM::HTVITEM hOld, TM::HTVITEM hNew, TMUInt nKey);
    virtual void OnStateChanged (TM::HTVITEM hItem, TMUInt uOld, TMUInt uNew);
};



