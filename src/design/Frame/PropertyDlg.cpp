/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2010.08.03
 *
 *  Copyright (C) 2010 miragekiller
 */

#include "StdAfx.h"
#include "Propertydlg.h"
#include "MainWnd.h"
#include "ProjMgr.h"
#include "WndTVSearch.h"
#include <atlctrls.h>
#include "resource.h"
#include "tmwnd.h"
#include "dui/property.h"


using namespace TM;


///////////////////////////////////////////////////////////////////////////////
// CPropertyTV
CPropertyTV::CPropertyTV()
    : m_picBorder(AppGetEditor()->GetPic(_Module.GetResourceInstance(), IDC_BMP_FRAME1P))
{
}

void CPropertyTV::InitDC(IGXApp* gapp, const TM::CTuiRect& rc, IPropBox* prop, TMBool bSelected)
{
    if (!prop->m_bSameVal)
        gapp->SetTextColor(TMRGB(128, 128, 196));
    m_picBorder->Render(0, 0, rc, 0, gapp);
}

TMResult CPropertyTV::OnDrawItem (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    TM::TUIDRAWITEM* pdi = (TM::TUIDRAWITEM*)(lParam);
    IGXApp* gapp = pdi->gxApp;
    
    bHandled = false;
    if (pdi->dwDrawStage == CDDS_PREPAINT)
    {
        pdi->tmpData = 0;
    }
    else if (pdi->dwDrawStage == CDDS_POSTPAINT)
    {
    }
    else if (pdi->dwDrawStage == CDDS_ITEMPREPAINT)
    {
        bHandled = true;
        TM::CTVItemT<IPropBox*> ti((TM::HTVITEM)pdi->hItem);
        if (ti)
        {
            gapp->Push(eGXPropTextColor);
            
            TMBool bIsSelected = ti.IsSelected();
            IPropBox* prop = ti.GetData();
            int nMiddle = prop->ItemMidle(pdi->rcItem);
            
            // draw text
            TM::CTuiRect rcName(pdi->rcItem);
            rcName.MoveRightTo(nMiddle);
            InitDC(gapp, rcName, prop, bIsSelected);
            if (gapp->ClipAndPush(rcName, true, eGXPropClip))
            {
                prop->DrawName(rcName, bIsSelected);
                gapp->Pop(); // restore old clip
            }
            if (bIsSelected)
                gapp->InvertRect(rcName);
            
            TM::CTuiRect rcValue(pdi->rcItem);
            rcValue.MoveLeftTo(nMiddle);
            InitDC(gapp, rcValue, prop, false);
            prop->DrawValue(rcValue);
            
            if (ti.IsSelected())
            {
                //                 m_edit.SetWindowPos(0, &rcValue, SWP_SHOWWINDOW | SWP_NOZORDER);
                pdi->tmpData = 1;
            }
            
            gapp->Pop();
        }
    }
    
    return bHandled;
}


///////////////////////////////////////////////////////////////////////////////
// CPropertyDlg
CMainWnd& CPropertyDlg::GetMainWnd()
{
    return *TM_OTHIS(CMainWnd, m_dlgProperty);
}

// TMResult CPropertyDlg::OnCreate(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     m_tv = GetDlgItem(IDC_PROP_TREE);
//     m_tv.RegisterEventSink (this);
//     m_edit = m_tv.GetDlgItem(IDC_TVEDIT);
//     m_wndSearchProp.SubclassWindow(m_hWnd);
//     m_wndSearchProp.SetTarget(m_tv);
//     bHandled = false;
//     return 0;
// }

void CPropertyDlg::Init()
{
//    m_tv = GetDlgItem(IDC_PROP_TREE);
    m_tv.SubclassWindow(GetDlgItem(IDC_PROP_TREE));
    m_tv.RegisterEventSink (this);
    m_edit = m_tv.GetDlgItem(IDC_TVEDIT);
    m_wndSearchProp.SubclassWindow(m_hWnd);
    m_wndSearchProp.SetTarget(m_tv);
}

// TMResult CPropertyDlg::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     bHandled = false;
//     return 0;
// }


TMResult CPropertyDlg::OnKeyDown(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    if (CTuiComboButton(Item(IDC_COMBOBUTTON_30188)).IsDropDown())
    {
        bHandled = false;
        return 0;
    }
    
    switch (pMsg->wParam)
    {
    case VK_UP:
    case VK_DOWN:
        m_tv.SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
        return true;

    case VK_RETURN:
    case VK_EXECUTE:
        UpdateCtrl();
        return true;

    case VK_ESCAPE:
        Sync_T2Editer();
        return true;

    default:
        bHandled = false;
    }

    return 0;
}

TMResult CPropertyDlg::OnGetDlgCode(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    return DLGC_WANTALLKEYS;
}

TMResult CPropertyDlg::OnSetFocus(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    Sync_T2Editer();
    bHandled = false;
    return 0;
}

TMResult CPropertyDlg::OnKillFocus(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    if (m_pProp && !CTuiComboButton(Item(IDC_COMBOBUTTON_30188)).IsDropDown())
        UpdateCtrl();
    bHandled = false;
    return 0;
}

TMResult CPropertyDlg::OnWindowPosChanged(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    if (((TM::TUIWINDOWPOS*)(pMsg->lParam))->flags & SWP_SHOWWINDOW)
        Sync_T2Editer();
    bHandled = false;
    return 0;
}


// TMResult CPropertyDlg::OnHitTest (int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
// {
//     DefRouteProc(pMsg);
//     
//     TM::TUIHITINFO* phi = (TM::TUIHITINFO*)(pMsg->lParam);
//     CPropItem ti ((TM::HTVITEM)phi->pTaget);
//     if (ti)
//     {
//         IPropBox* prop = ti.GetData();
//         prop->HitTest(phi);
//     }
//     
//     return 0;
// }

TMResult CPropertyDlg::OnDrawItem (int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    bHandled = false;
    TM::TUIDRAWITEM* pdi = (TM::TUIDRAWITEM*)(pMsg->lParam);
    TM::CTVItem ti ((TM::HTVITEM)pdi->hItem);
    if (ti && ti.IsSelected())
    {
        TM::CTuiRect rc(pdi->rcItem);
        rc.OffsetLeft(rc.Width() * 2 / 5);
        m_edit.SetWindowPos(0, &rc, SWP_SHOWWINDOW | SWP_NOZORDER);
    }
    return 0;
}

TMResult CPropertyDlg::OnLButtonDblClk(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    TMUInt64 lid;
    if (m_pProp && (0 != (lid = m_pProp->GetLocateID())))
        GetMainWnd().GetPanel().Goto(lid);
    bHandled = false;
    return 0;
}

void CPropertyDlg::SetVisible(TM::CTVItem itm, IPropBox* prop)
{
//    int n = prop->Matching(m_strFilter);
    int n = prop->Matching(L"");
    itm.ShowItem(n && n == m_aObj.size(), false);
}

void CPropertyDlg::SetVisible()
{
    for (TM::CTVItem rec(m_tv.GetRootItem()); rec; rec = rec.GetGlobalNext())
    {
        IPropBox* prop = (IPropBox*)rec.GetData();
        if (prop)
            SetVisible (rec, prop);
    }
}

void CPropertyDlg::UpdateCtrl()
{
    DbgAssert(m_pProp);
    TM::CString str;
    m_edit.GetWindowText(str);
    m_pProp->Editer2T(str);
//     InvalidateView(0);
}

void CPropertyDlg::SetObj(IObjEdit* pObj)
{
    m_edit.ShowWindow(SW_HIDE);
    if (pObj)
    {
        m_aObj.insert(pObj);

        pObj->EditProp(true);

//         SetAddMode(true);
//         p->EditProp (*this);
        
        SetVisible();
        m_tv.Invalidate();

        if (m_pProp)
            Sync_T2Editer();
    }
//     else
//         m_tuiWindow.SetWindowText(TM_T(""));
}

void CPropertyDlg::RemoveObj(IObjEdit* pObj)
{
    m_edit.ShowWindow(SW_HIDE);
    if (pObj)
    {
        pObj->EditProp(false);

        m_aObj.erase (pObj);
        SetVisible();
        m_tv.Invalidate();

        if (m_pProp)
            Sync_T2Editer();
    }
}

void CPropertyDlg::OnSelChanged (TM::HTVITEM hOld, TM::HTVITEM hNew, TMUInt uKey)
{
    if (hOld)
    {
        TM::CTVItem itm(hOld);
//        IPropBox* prop = (IPropBox*)itm.GetData();
        m_edit.ShowWindow(false);
        m_pProp->InitPopable(0);
    }

    m_pProp = 0;
    if (hNew)
    {
        TM::CTVItem itm(hNew);
        m_pProp = (IPropBox*)itm.GetData();
        
        TM::CTuiRect rc;
        itm.GetRect(&rc);
        int nMiddle = m_pProp->ItemMidle(rc);
        rc.MoveLeftTo(nMiddle + 1);

        m_edit.SetWindowPos(0, &rc, SWP_SHOWWINDOW);
        m_edit.SetFocus();
        
        EPopable n = m_pProp->InitPopable(true);
        m_edit.Item(IDW_TUI_PUSHBUTTON_20066).ShowWindow(n == 1);
        CTuiWidget wnd(Item(IDC_COMBOBUTTON_30188));
        wnd.EnableWindow(n == ePopableComb);
        wnd.ShowWindow(n == ePopableComb);
    }

    GetMainWnd().SyncTxtEdit(false);
}

void CPropertyDlg::AddProp(const CStrView& pName, IPropBox* pBox)
{
    TM::CTVItemT<IPropBox*> ti = m_tv.GetRootItem();
    ti = ti.AppendChild(pName);
    ti.SetData(pBox);
    ti.SetHint(pBox->GetPropHint());
}

TMResult CPropertyDlg::OnShowPop(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    DbgAssert(m_pProp);
    m_tv.SetFocus();
    
    TM::CString str;
    m_edit.GetWindowText(str);
    if (m_pProp->ShowPopup(str))
    {
        m_edit.SetWindowText(str);
        m_pProp->Editer2T(str);
        m_edit.SetFocus();
    }
    return 0;
}

TMResult CPropertyDlg::OnSelEndOK (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    UpdateCtrl();
    return 0;
}

TMResult CPropertyDlg::OnSelEndCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    Sync_T2Editer();
    return 0;
}



TM::CString CPropertyDlg::GetSelectedPropName()
{
    TM::CString str;
    TM::CTVItemT<IPropBox*> ti = m_tv.GetSelectedItem();
    if (ti && !(ti.GetState() & TVIS_HIDE))
        ti.GetText(str);
    return str;
}

void CPropertyDlg::Sync_T2Editer()
{
    DbgAssert(m_pProp);
    CString str;
    TMBool b = m_pProp->T2Editer(str);
    if (b)
    {
        m_edit.SetBkPic(IDP_CLR_0xFFFFFF);
        m_edit.SetWindowText(str);
    }
    else
    {
        m_edit.SetBkPic(IDP_CLR_0x9FC9D7);
        m_edit.SetWindowText(TM_T(""));
    }
}

TMBool CPropertyDlg::InitPopupID(const CPopIDList& aID)
{
    TM::CTuiComboButton but(m_edit.Item(IDC_COMBOBUTTON_30188));
    CTuiTree tree(but.GetPopList());
    CTVItem itm(tree.GetRootItem());
    itm.RemoveChild();
    itm.AppendChild(_T(" 0"));
    for (CPopIDList::const_iterator iter = aID.cbegin(); iter != aID.cend(); ++iter)
    {
        CPopIDList::const_reference ref = *iter;
        CTVItem itmc = itm.AppendChild(ref.first);
        itmc.SetData((void*)ref.second, false);
    }
    return false;
}

class CInputTextDlg : public CDialogImpl<CInputTextDlg>
{
public:
    CEdit    m_ctrlText;
    CString    m_strText;
    
public:
    enum { IDD = IDD_INPUT_TEXT };
    
    BEGIN_MSG_MAP(CInputTextDlg)
        MESSAGE_HANDLER    (WM_INITDIALOG,    OnInitDialog);
    COMMAND_ID_HANDLER (IDOK,            OnClose)
        COMMAND_ID_HANDLER (IDCANCEL,        OnClose)
        END_MSG_MAP()
        
        TMResult OnClose(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        int n = m_ctrlText.GetWindowText (m_strText.GetBuffer(10240), 10240);
        m_strText.SetLength (n);
        EndDialog (wID);
        return 0;
    }
    
    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        m_ctrlText = GetDlgItem(IDC_EDIT_INPUT);
        m_ctrlText.SetWindowText(m_strText);
        bHandled =false;
        return 0;
    }
};

TMBool CPropertyDlg::ShowPopupString(TM::CString& str)
{
    CInputTextDlg dlg;
    dlg.m_strText = str;
    if (dlg.DoModal (GetHWND()) == IDOK)
    {
        str = dlg.m_strText;
        return true;
    }
    return false;
}

TMBool CPropertyDlg::ShowPopupFont(LOGFONT& lf)
{
    CFontDialog dlg (&lf, CF_EFFECTS | CF_SCREENFONTS | CF_TTONLY);
    return IDOK == dlg.DoModal();
}

TMBool CPropertyDlg::ShowPopupColor(TMColor& clr)
{
    CColorDialog dlg;
    if (IDOK == dlg.DoModal(0, ColorToCOLORREF(clr)))
    {
        TMUInt8 a = ColorA(clr);
        clr = ColorFromCOLORREF(dlg.GetColor());
        if (a != 0)
            ColorSetA(clr, a);
//         if (clr == 0)
//             clr = TMRGB(0,0,0);
        return true;
    }
    return false;
}

TMBool CPropertyDlg::ShowPopupFile(TM::CString& str)
{
    TMCStr lpcsFileName = _T("*.*");
    TMCStr lpcstrFilter =_T("All Files (*.*)\0*.*\0") _T(""); 
    CFileOpenDialog dlg; //(TMTrue, NULL, lpcsFileName,
        //OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, lpcstrFilter);
    if (dlg.DoModal(0, lpcstrFilter, 0, 0, lpcsFileName,
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR) == IDOK)
    {
        str = dlg.GetPathName();
        return true;
    }
    return false;
}


class CCatInformation : public CComPtr<ICatInformation>
{
public:
    CCatInformation() 
    {
        CoCreateInstance(CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_INPROC);
        DbgAssert(p);
    }
};

class CCatIterator
{
protected:
    CComPtr<IEnumCATEGORYINFO> spEnumCatInfo;        // IEnumCATEGORYINFO 
    CCatInformation spCatInfo;                            // ICatInformation

public:
    CCatIterator(LCID lcid = GetUserDefaultLCID()) 
    {
        TMResult hr = spCatInfo->EnumCategories(lcid, &spEnumCatInfo);
        DbgAssert(SUCCEEDED(hr));
    }

    TMBool Next(CATEGORYINFO& catinfo) 
    {
        ULONG nRet=0;
        return SUCCEEDED(spEnumCatInfo->Next(1, &catinfo, &nRet)) && nRet==1;
    }
};

class CCatClassIterator
{
protected:
    CComPtr<IEnumCLSID> spEnumCLSID;                    // IEnumCLSID
    CCatInformation spCatInfo;                            // ICatInformation

public:
    CCatClassIterator(CATID* arImplCatids, ULONG nImpl,
        CATID* arReqdCatids=NULL, ULONG nReqd=0) 
    {
        
        TMResult hr = spCatInfo->EnumClassesOfCategories(
            nImpl,            // num implemented cats in array
            arImplCatids,    // array of cats to look for (implement)
            nReqd,            // num required categories in array
            arReqdCatids,    // array of required categories to look for
            &spEnumCLSID);    // IEnum returned
        DbgAssert(SUCCEEDED(hr));
    }

    TMBool Next(CLSID& clsid) 
    {
        ULONG nRet=0;
        return SUCCEEDED(spEnumCLSID->Next(1, &clsid, &nRet)) && nRet==1;
    }
};


class COleList : public CDialogImpl<COleList>
{
public:
    CListViewCtrl m_lvLeft;
    CListViewCtrl m_lvRight;
    CString m_strCur;
    
public:
    enum { IDD = IDD_OLELIST };
    
public:
    BEGIN_MSG_MAP(COleList)
        MESSAGE_HANDLER (WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER (ID_PROP, OnProp)
        COMMAND_ID_HANDLER (ID_RESET, OnReset)
        
        COMMAND_ID_HANDLER (IDOK, OnOK)
        COMMAND_ID_HANDLER (IDCANCEL, OnCancel)
        NOTIFY_HANDLER (IDC_LIST1, NM_DBLCLK, OnOK);
        NOTIFY_HANDLER (IDC_LIST2, LVN_ITEMCHANGED,OnItemChanged)
    END_MSG_MAP()
public:
    COleList ()
    {
    }

    ~COleList()
    {
    }

    int DoModal(TMHWnd hWndParent, const CString& strCur)
    {
        m_strCur = strCur;
        return CDialogImpl<COleList>::DoModal(hWndParent);
    }
    
    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
        m_lvLeft = GetDlgItem(IDC_LIST2);
        m_lvLeft.InsertColumn (0, _T("type"), 0, 250, -1);
        m_lvLeft.InsertColumn (1, _T("guid"), 0, 350, -1);

        m_lvRight = GetDlgItem(IDC_LIST1);
        m_lvRight.InsertColumn (0, _T("name"), 0, 250, -1);
        m_lvRight.InsertColumn (1, _T("prog"), 0, 250, -1);
        m_lvRight.InsertColumn (2, _T("guid"), 0, 350, -1);

        CATEGORYINFO catinfo;
        CCatIterator it;
        for (int i = 0; it.Next(catinfo); ++i) 
        {
            // add category name to list 
            CString sName = catinfo.szDescription;
            if (sName.IsEmpty()) 
            {
                sName = _T("<unkown>");
            }
            int iItem = m_lvLeft.InsertItem(i,sName);
            
            // Add CATID as 1st subitem
            LPOLESTR pstr=NULL;
            StringFromCLSID(catinfo.catid, &pstr);
            m_lvLeft.SetItemText(iItem,1,TM_W2T(pstr));
            CoTaskMemFree(pstr);

            if (catinfo.catid == CATID_Control)
                m_lvLeft.SelectItem(iItem);
        }

        return 0;
    }

    TMResult OnItemChanged(int idCtrl, LPNMHDR pnmh, TMBool& bHandled)
    {
        NMLISTVIEW nm = *(NMLISTVIEW*)pnmh;
        if (nm.iItem>=0 && (nm.uNewState & LVIS_SELECTED)) 
        {
            TMCharT c[256];
            CString sguid;
            int n = m_lvLeft.GetItemText(nm.iItem,1, c, 255);

            CATID catid;
            USES_CONVERSION;
            CLSIDFromString(T2OLE(c),&catid);

            m_lvRight.DeleteAllItems();

            CLSID clsid;
            CCatClassIterator it(&catid, 1);
            while (it.Next(clsid)) 
            {
                LPOLESTR pszName;
                TMResult h = OleRegGetUserType(clsid, USERCLASSTYPE_FULL, &pszName);
                if (SUCCEEDED(h))
                {
                    int iItem = m_lvRight.InsertItem(0, OLE2T(pszName));
                    CoTaskMemFree(pszName);

                    ProgIDFromCLSID(clsid, &pszName);
                    m_lvRight.SetItemText(iItem,1,OLE2T(pszName));
                    CoTaskMemFree(pszName);

                    StringFromCLSID(clsid, &pszName);
                    m_lvRight.SetItemText(iItem,2,OLE2T(pszName));
                    if (m_strCur == OLE2T(pszName))
                    {
                        m_lvRight.SelectItem(iItem);
                    }
                    CoTaskMemFree(pszName);
                }
            }
        }

        return 0;
    }

    TMResult OnProp(...)
    {
//         CComQIPtr< ISpecifyPropertyPages > spSpecify(m_ax.GetControlUnknown());
//         if (!spSpecify)    return;
//         
//         CAUUID pages;
//         TMResult hResult = spSpecify->GetPages(&pages);
//         if (FAILED(hResult))    return;
//         
//         CLSID * pclsidPages = (CLSID*)_alloca(pages.cElems * sizeof(CLSID));
//         for (int i=0; i<(int)pages.cElems; i++)
//             pclsidPages[i] = pages.pElems[i];
//         
//         ::CoTaskMemFree(pages.pElems);
//         
//         IUnknown * pObject = m_ax.GetControlUnknown();
//         
//         OleCreatePropertyFrame(m_hWnd, 0, 0, L"ÊôÐÔ",
//             1, &pObject,
//             pages.cElems, pclsidPages,
//             GetUserDefaultLCID(), 0, NULL);

        return 0;
    }

    TMResult OnReset(...)
    {
        m_strCur.clear();
        EndDialog(IDOK);
        return 0;
    }
    
    TMResult OnOK(...)
    {
        int iItem = m_lvRight.GetSelectedIndex();
        if (iItem >= 0)
        {
            int n = m_lvRight.GetItemText(iItem, 2, m_strCur.GetBuffer(255), 255);
            m_strCur.SetLength(n);
        }
        else
            m_strCur.clear();

        EndDialog(IDOK);
        return 0;
    }
    
    TMResult OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        EndDialog(0);
        return 0;
    }
};

TMBool CPropertyDlg::ShowPopupOLE(TM::CString& str)
{
    COleList ol;
    if (IDOK == ol.DoModal(GetHWND(), str))
    {
        str = ol.m_strCur;
        return true;
    }
    else
        return false;
}

int CPropertyDlg::PopupMenu(HMENU hMenu)
{
    CTuiRect rc;
    RECT rcScreen;
    m_edit.GetWindowRect(&rc);
    m_edit.ClientToScreen(rc, rcScreen);
    int n = TrackPopupMenu(hMenu, TPM_RETURNCMD, rcScreen.left, rcScreen.bottom, 0, GetHWND(), 0);
    return n;
}

void CPropertyDlg::SetCurrentPropertyText(LPCTSTR pStr)
{
    m_edit.SetWindowText(pStr);
}

TMBool CPropertyDlg::IsFocus()
{
    return m_edit.IsFocus();
};
