/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2016.2.24
 *
 *  Copyright (C) 2016 miragekiller
 */

#pragma once

#ifndef OBJMGR_H
#define OBJMGR_H

#include "tuires.h"
#include "ObjectTreeView.h"
#include "Undo.h"
#include "PropertyDlg.h"
#include "TraceMT.h"


class CMainWnd;

class CWndPanel
    : public TM::CTuiWidgetImpl<CWndPanel>
    , public IProjMgr
    , public CTraceMT
{
private:
    CTVCtrl m_ctrlTree;
    IProj*  m_pProj;
    CUndo   m_undo;

    friend CUndo;
    friend CTVCtrl;

public:
    CWndPanel();
    ~CWndPanel();
    
public:
    enum { IDD = IDO_TUI_DUMMY_10007 };

    BEGIN_TUIMSG_MAP    (CWndPanel)
//         MESSAGE_HANDLER (WM_PRINTCLIENT, OnPrintClient)
//         MESSAGE_HANDLER (uMsg, OnGeneralMessage)
//         MESSAGE_HANDLER (WM_SHOWWINDOW, OnShowWindow)
        MESSAGE_HANDLER (WM_DESTROY, OnDestroy)
    END_TUIMSG_MAP      ()

//     TMResult OnShowWindow(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnGeneralMessage(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnPrintClient(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);

public:
    TMResult OnCommand(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnContextMenu();

public:
    CMainWnd& GetMainWnd();
    CTVCtrl& GetTree() { return m_ctrlTree; }

    IProj* GetProj() { return m_pProj; }
    IProj* _CreateProj(CMainWnd* pMainWnd, EPlugInMode eMode);
    CUndo& GetUndo() { return m_undo; }
    void LoadFile(LPCTSTR pFileName, TMBool bMerge, TMBool bClearUndo);
    TMBool Goto(TMUInt64 nID);

    void Destroy();
//     CObjItem GetMgrOwner() { return m_ctrlTree.GetObjOwner(TM::e_otWnd); }
    TMBool SelectByString(LPCTSTR pStr);
    TMBool SelectByAcc(IAccessible* acc);
    void OnSelector(const TM::CTuiPoint& pt);
    void SelectItem(IObjEdit* pItem, TMUInt32 dwOption = TM::CTVItem::e_soEnsureVisible);
    IObjEdit* GetDataByObjectID(TMUInt32 id);
    void DeleteSelect();
    void DeleteAll();
    CPropertyDlg& GetProp();
    CObjItem Obj2Item(IObjEdit* pItem);

public:
    // CTuiWidgetImpl
    void OnSubclass(TM::HTUI hWnd);

    // ITraceHook, CTraceMT; trace assert to alert window
    virtual void OnTrace(int tp, TMCStr pStr) tm_final; 
    virtual void DoOnTrace(TMCStr pStr) tm_final; 

    // IProjMgr
    virtual void Invalidate() tm_override;
    virtual void GetPadInfo(TM::CTuiPoint& pt, int& dpi) tm_override;
    virtual void Alert(TMCStr pFmt, ...) tm_override;
    virtual void OnTrackFocus(void* pNewObj, const TM::CRect& rc, const TM::CString& strAlert) tm_final;

    // item
    virtual IObjEdit* ItemCreate(IObjEdit* pParent, const CDomStrView& pClassName, TMRect* prc = 0) tm_final { return m_undo.CreateNewItem(pParent, pClassName, prc); }
    virtual void ItemAdd(IObjEdit* pPos, EZOrder mp, IObjEdit* pData, TM::EObjType eType) tm_final;
    virtual void ItemRemove(IObjEdit* pPos) tm_final;
    virtual void ItemMoveOrAdd(IObjEdit* pPos, EZOrder mp, IObjEdit* pData) tm_final;
    virtual IObjEdit* ItemGetParent(IObjEdit* pItem) tm_final;
    virtual IObjEdit* ItemGetChild(IObjEdit* pItem) tm_final;
    virtual IObjEdit* ItemGetNext(IObjEdit* pItem) tm_final;
    virtual IObjEdit* ItemGetSelectedData(TMBool bGetOwner) tm_final;
    virtual TMUInt ItemGetState(IObjEdit* pItem) tm_final;

    // prop
    virtual TMBool PropInitPopupID(const CPopIDList& aID) tm_override { return GetProp().InitPopupID(aID); }
    virtual TMBool PropShowPopupString(TM::CString& str) tm_override { return GetProp().ShowPopupString(str); };
    virtual TMBool PropShowPopupFont(LOGFONT& lf) tm_override { return GetProp().ShowPopupFont(lf); };
    virtual TMBool PropShowPopupColor(TMColor& clr) tm_override { return GetProp().ShowPopupColor(clr); };
    virtual TMBool PropShowPopupFile(TM::CString& str) tm_override { return GetProp().ShowPopupFile(str); };
    virtual TMBool PropShowPopupOLE(TM::CString& str) tm_override { return GetProp().ShowPopupOLE(str); };
    virtual int PropPopupMenu(HMENU hMenu) tm_override { return GetProp().PopupMenu(hMenu); };
    virtual void PropSetCurrentText(LPCTSTR pStr) tm_override { GetProp().SetCurrentPropertyText(pStr); };
    virtual TMBool PropIsFocus() tm_override { return GetProp().IsFocus(); };
    virtual void PropAdd(const TM::CStrView& pName, IPropBox* pBox) tm_override { GetProp().AddProp(pName, pBox); };
    virtual TM::CStringW PropBegin(IObjEdit* p) tm_override { return m_undo.BeginProp(p); }
    virtual void PropEnd(IObjEdit* p, const TM::CStringW& key) tm_override { m_undo.EndProp(p, key); }
};

// extern CWndPanel* g_curPanel;



#endif //OBJMGR_H
