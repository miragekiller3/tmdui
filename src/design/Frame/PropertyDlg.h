//-----------------------------------------------------------
//Property.h

//  Copyright (C) 2004 miragekiller                               
//  All rights reserved                                                     
//
//
//  Author: 
//      miragekiller    <3916345933@qq.com>
//
//    Histroy :
//      2010.08.03          create

//-------------------------------------------------------------

#pragma once 

// #include "StdAfx.h"
#include "tuires.h"
#include "WndTVSearch.h"
#include "../Design.h"

class CMainWnd;
class IObjEdit;


///////////////////////////////////////////////////////////////////////////////
// CPropertyTV
class CPropertyTV : public TM::CTuiWidgetImpl<CPropertyTV, TM::CTuiTree>
{
public:
    enum { IDD = IDO_TUI_TREE_20013 };
    
public:
    TM::CTuiPicPtr          m_picBorder;
    
public:
    BEGIN_TUIMSG_MAP        (CPropertyTV)
        MESSAGE_HANDLER     (CM_DRAWITEM,    OnDrawItem);
    END_TUIMSG_MAP()
        
    TMResult OnDrawItem (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    
    CPropertyTV();
    void InitDC(IGXApp* gapp, const TM::CTuiRect& rc, IPropBox* prop, TMBool bSelected);
};


///////////////////////////////////////////////////////////////////////////////
// CPropertyDlg
class CPropertyDlg
    : public TM::CTuiWidgetImpl<CPropertyDlg>
    , public TM::ITVEvent
{
private:
    typedef TM::CSet<IObjEdit*>          CObjList;
    typedef CObjList::recorder          CObjRec;


    CPropertyTV     m_tv;
    TM::CTuiEdit    m_edit;
//    TM::CString     m_strFilter;
    CObjList        m_aObj;
    class IPropBox* m_pProp;
    CWndTVSearch    m_wndSearchProp;

public:
    enum { IDD = IDD_TUI_PROP }; 

    BEGIN_TUIMSG_MAP        (CPropertyDlg)
//         MESSAGE_HANDLER        (WM_CREATE,                    OnCreate);
//         MESSAGE_HANDLER        (WM_DESTROY,                OnDestroy);
//         MESSAGE_HANDLER        (WM_CLOSE,              OnClose);
//         MESSAGE_HANDLER        (CM_VSCROLL,            OnVScroll);

        ROUTE_ID_HANDLER    (IDC_TVEDIT, WM_GETDLGCODE, OnGetDlgCode);
        ROUTE_ID_HANDLER    (IDC_TVEDIT, WM_KEYDOWN,    OnKeyDown);
        ROUTE_ID_HANDLER    (IDC_TVEDIT, WM_SETFOCUS,   OnSetFocus);
        ROUTE_ID_HANDLER    (IDC_TVEDIT, WM_KILLFOCUS,  OnKillFocus);
        ROUTE_ID_HANDLER    (IDC_TVEDIT, CM_WINDOWPOSCHANGED,  OnWindowPosChanged);

//         ROUTE_ID_HANDLER    (IDC_PROP_TREE,  CM_HITTEST,     OnHitTest);
        ROUTE_ID_HANDLER    (IDC_PROP_TREE,  CM_DRAWITEM,    OnDrawItem);
        ROUTE_ID_HANDLER    (IDC_PROP_TREE,  WM_LBUTTONDBLCLK,  OnLButtonDblClk);
//        ROUTE_ID_HANDLER    (IDC_PROP_TREE,  CM_GETSCROLLINFO,  OnGetScrollInfo);

//         COMMAND_HANDLER     (IDC_FILTER, EN_CHANGE, OnEnChange)
//         COMMAND_ID_HANDLER  (IDC_EDIT_CLEAR,        OnEditClear)
        COMMAND_ID_HANDLER  (IDC_POP,               OnShowPop)
        COMMAND_CODE_HANDLER(CBN_SELENDOK,          OnSelEndOK)
        COMMAND_CODE_HANDLER(CBN_SELENDCANCEL,      OnSelEndCancel)
        

            

        CHAIN_MSG_MAP   (root_class)
    END_TUIMSG_MAP()

//     TMResult OnCreate(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
    TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnSize(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled); 
//     TMResult OnClose(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnVScroll(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled);
//     TMResult OnListItemChanged(int , LPNMHDR pnmh, TMBool&);
//     TMResult OnTreeCtrlClick(int , LPNMHDR pnmh, TMBool&);

    TMResult OnGetDlgCode(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnKeyDown(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnSetFocus(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnKillFocus(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnWindowPosChanged(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);

    TMResult OnLButtonDblClk(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
//     TMResult OnHitTest (int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
//    TMResult OnGetScrollInfo (int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);
    TMResult OnDrawItem (int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled);

//     TMResult OnEnChange(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
//     TMResult OnEditClear(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnShowPop (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnSelEndOK (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);
    TMResult OnSelEndCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled);

private:
    void SetVisible(TM::CTVItem itm, IPropBox* p);
    void SetVisible();
//     void InvalidateView(TM::CDomElement* pXml);
    void UpdateCtrl();

    virtual void OnSelChanged (TM::HTVITEM hOld, TM::HTVITEM hNew, TMUInt uKey);

public:
    CPropertyDlg() : m_pProp(0) {}
    CMainWnd& GetMainWnd();
    void Clean() { m_tv.ResetContent(); }
    void Init();
    void SetObj(IObjEdit* ppObj);
    void RemoveObj(IObjEdit* pPorp);
//     int GetSelectedCount() { return m_aObj.size(); }
    TM::CString GetSelectedPropName();

    void Sync_T2Editer();
//    TM::CTuiObject* GetPorp(){return m_pCurrentProp;}

    // for IProjMgr::PropXXX
    TMBool InitPopupID(const CPopIDList& aID);
    TMBool ShowPopupString(TM::CString& str);
    TMBool ShowPopupFont(LOGFONT& lf);
    TMBool ShowPopupColor(TMColor& clr);
    TMBool ShowPopupFile(TM::CString& str);
    TMBool ShowPopupOLE(TM::CString& str);
    int PopupMenu(HMENU hMenu);
    void SetCurrentPropertyText(LPCTSTR pStr);
    TMBool IsFocus();
    void AddProp(const TM::CStrView& pName, IPropBox* pBox);
};

/*extern TM::CStaticWindowT<CPropertyDlg> g_dlgProperty;*/