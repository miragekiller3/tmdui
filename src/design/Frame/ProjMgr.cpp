/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2016.2.24
 *
 *  Copyright (C) 2016 miragekiller
 */

#include "stdafx.h"
#include "ProjMgr.h"
#include "MainWnd.h"
#include "Tools.h"
// #include "FileOpt.h"
#include "Wizard.h"
#include "WndTVSearch.h"
#include "Propertydlg.h"
#include "DlgSpyFocus.h"
#include "resource.h"

using namespace TM;


CWndPanel::CWndPanel()
    : m_pProj(0)
{
}

CWndPanel::~CWndPanel()
{
}

CMainWnd& CWndPanel::GetMainWnd()
{
    return *TM_OTHIS(CMainWnd, m_curPanel);
}

TMResult CWndPanel::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_ctrlTree.ResetContent();
    m_ctrlTree.DestroyWindow();

    if (m_pProj)
    {
        m_pProj->Release();
        m_pProj = 0;
    }

    bHandled = false;
    return 0;
}

TMResult CWndPanel::OnCommand(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    switch (LOWORD(wParam))
    {
    case ID_EDIT_CUT:
    case ID_EDIT_COPY:
        GetUndo().CopyBegin(LOWORD(wParam) == ID_EDIT_CUT);
        struct CAdd
        {
            static void Add(CUndo* undo, CObjItem itm)
            {
                if (!itm)
                    return;
                if (itm.IsSelected())
                    undo->CopyAdd(itm.GetData());
                else
                    Add(undo, itm.GetChild().m_hItem);
                Add(undo, itm.GetNext().m_hItem);
            }
        };
        CAdd::Add(&GetUndo(), m_ctrlTree.GetRootItem().m_hItem);
        break;
        
    case ID_EDIT_PASTE:
        GetUndo().Paste(ItemGetSelectedData(false));
        break;

    case IDM_MOVE_UP:
    case IDM_MOVE_DOWN:
        {
            enum { eWindowLevel = 3 };
            CObjItem ti (m_ctrlTree.GetSelectedItem());
            if (!ti || ti.GetLevel() < eWindowLevel)
                break;
            if (LOWORD(wParam) == IDM_MOVE_UP)
            {
                CObjItem tiPrev(ti.GetPrev());
                if (tiPrev)
                    GetUndo().MoveItem(ti.GetData(), tiPrev.GetData(), eZOrderUp);
            }
            else
            {
                CObjItem tiNext(ti.GetNext());
                if (tiNext)
                    GetUndo().MoveItem(ti.GetData(), tiNext.GetData(), eZOrderDown);
            }
        }
        break;

    case IDM_DIALOG_PROPERTY:
        {
            enum { eWindowRoot = 1 };
            CObjItem ti (m_ctrlTree.GetSelectedItem());
            if (!ti || ti.GetLevel() <= eWindowRoot)
                break;
            CWizardDlg dlg(ti);
            dlg.m_pMainWnd = &GetMainWnd();
            dlg.DoModal(*AppGetEditor(), GetHWND());
        }
        break;

    default:
        bHandled = false;
        break;
    }
    return 0;
}

void CWndPanel::Invalidate()
{
    CTuiWidget::Invalidate();
}

void CWndPanel::GetPadInfo(TM::CTuiPoint& pt, int& dpi)
{
    CTuiRect rc;
    GetPaddingRect(&rc);

    TUISCROLLINFO ww, wh;
    GetScrollInfo(&ww, &wh);
    rc.OffsetRect(-ww.nOrg, -wh.nOrg);

    pt.x = rc.left;
    pt.y = rc.top;
    dpi = GetDPI();
}

void CWndPanel::OnSelector(const CTuiPoint& pt)
{
    GetMainWnd().m_wndSelector.Selector(pt, this);//GetProjMgr());
}

IProj* CWndPanel::_CreateProj(CMainWnd* pMainWnd, EPlugInMode eMode)
{
    HWND hWnd = pMainWnd->GetHWND();
    HINSTANCE hInst = _Module.GetResourceInstance();
//    m_rpic.Load(IDP_BMP_SELECT, hWgt);
//     m_rpic = (ITuiPic*)GetAPP()->GetPic(hWgt, IDP_BMP_SELECT, TUI_DPI_SYSTEM_DEFAULT);
    PANELINFO info = { hWnd, _Module.GetModuleInstance(), this };//m_rpic };
    if (eMode == ePlugInModeSpy)
#ifdef TM_QTSPY
        m_pProj = CreateQtProj(info);
#else
        m_pProj = CreateSpyProj(info);
#endif
    else
        m_pProj = CreateProj(info);
    m_ctrlTree.Init(pMainWnd->Item(IDC_TREE), m_pProj);

    TMCStr* ppDisplay;
    TMCStr* ppText;
    m_pProj->GetDeviceNames(&ppDisplay, &ppText);

    CTuiComboButtonT<CTuiComboUnit> but1(pMainWnd->Item(IDM_DEVICE));
    CTuiListBox lb1 = but1.GetPopList();
    lb1.ResetContent();
    tm_for (int i = 0; ppDisplay[i]; ++i)
        lb1.AddString(ppDisplay[i]);
    lb1.SetCurSel(pMainWnd->m_cfgDbg.nDevGra); but1.SetEditSel(); m_pProj->TesterChangeDevice(true, pMainWnd->m_cfgDbg.nDevGra);
    
    CTuiComboButtonT<CTuiComboUnit> but2(pMainWnd->Item(IDM_DEVICE_TXT));
    CTuiListBox lb2 = but2.GetPopList();
    lb2.ResetContent();
    tm_for (int i = 0; ppText[i]; ++i)
        lb2.AddString(ppText[i]);
    lb2.SetCurSel(pMainWnd->m_cfgDbg.nDevTxt); but2.SetEditSel(); m_pProj->TesterChangeDevice(false, pMainWnd->m_cfgDbg.nDevTxt);

    return m_pProj;
}

void CWndPanel::LoadFile(LPCTSTR pFileName, TMBool bMerge, TMBool bClearUndo)
{
    if (bClearUndo)
    {
        m_undo.Clear();
        m_undo.ClearModifyed();
    }
    m_ctrlTree.LoadExistentResource(GetProj());
    GetProj()->LoadFromFile(pFileName, bMerge);
}

TMBool CWndPanel::Goto(TMUInt64 nID)
{
    CObjItem rec(m_ctrlTree.GetRootItem());
    rec.find(CObjItem::_FindById(), nID);
    if (!rec)
        return false;
    rec.SelectItem();
    return true;
}

void CWndPanel::Destroy()
{
    DestroyWindow();
}

TMBool CWndPanel::SelectByString(LPCTSTR pStr)
{
    for (CTVItem itm = m_ctrlTree.GetRootItem(); itm = itm.GetGlobalNext();)
    {
        CString str;
        itm.GetText(str);
        if (str.Find(pStr) >= 0)
        {
            itm.SelectItem();
            return true;
        }
    }
    return false;
}

TMBool CWndPanel::SelectByAcc(IAccessible* acc)
{
    for (CObjItem itm(m_ctrlTree.GetRootItem()); itm = itm.GetGlobalNext();)
    {
        IObjEdit* p = *itm;
        if (!p)
            continue;
        if (p->IsSameAcc(acc))
        {
            itm.SelectItem();
            return true;
        }
    }
    return false;
}


// static TM::CString CheckPopupMenu(IProj* pProj, TMPoint pt, HWND hWnd, TM::EObjType eType)
// {
//     CMenu muMain = LoadMenu(_Module.m_hInst, MAKEINTRESOURCE(IDR_MENU_POP));
//     CMenuHandle muPop = muMain.GetSubMenu(0);
//     int n = 0;
//     muPop.CreatePopupMenu();
//     TMCStr strClass;
//     for (n = 0; strClass = pProj->GetClassByIndex(eType, n); ++n)
//     {
//         int nNewMenu = 30000;
//         nNewMenu += n;
//         muPop.AppendMenu (MF_STRING,nNewMenu,TM_W2T(strClass));
//     }
//     
//     n =  muPop.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN, pt.x, pt.y, hWnd);
//     if (!n)
//         return TM::CString();
//     TMCharT strTemp[CHAR_MAX + 1] = {0};
//     int nTmp = muPop.GetMenuString(n,strTemp,CHAR_MAX,MF_BYCOMMAND);
//     return strTemp;
// }
// 
// static int TrackDlgPopupMenu(CWndPanel& panel, TMPoint pt, TMHWnd hWnd, TM::EObjType eObjType)
// {
//     TM::CString str(CheckPopupMenu(panel.GetProj(), pt, hWnd, eObjType));
//     if (!str.IsEmpty())
//     {
//         panel.ItemCreate(0, str);
//         //        CreateNewObject((IObjEdit*)eObjType, str);
//         return true;
//     }
//     return false;
// }
// 
TMResult CWndPanel::OnContextMenu()
{
    int nRet = 0;
    TMPoint pt;
    if (!GetCursorPos(&pt))
        return 0;
    
    //     TVITEM vItem={0};
    CObjItem ti(m_ctrlTree.GetSelectedItem());
    if (!ti)
    {
        return 0;
    }

    CMenu muMain = LoadMenu(_Module.m_hInst, MAKEINTRESOURCE(IDR_MENU_POP));
    IObjEdit* pCur;
    if (ti == m_ctrlTree.GetObjOwner(TM::e_otWnd))
        pCur = objEditMgr; 
    else if (ti == m_ctrlTree.GetObjOwner(TM::e_otFrm))
        pCur = objEditFrm;
    else if (ti == m_ctrlTree.GetObjOwner(TM::e_otPic))
        pCur = objEditPic;
    else
        pCur = ti.GetData();
    return m_pProj->TrackMenuPop(GetHWND(), pt, muMain, pCur);
 
//     
//     if (ti == m_ctrlTree.GetObjOwner(TM::e_otWnd))//m_ctrlTree.GetParentItem(hItem) == m_ItemArray[0])
//         nRet = TrackDlgPopupMenu(*this, pt, GetHWND(), TM::e_otWnd);// CPopMenu<TM::e_otWnd> ::Exec(pt, m_ctrlTree,this);        
//     else if (ti == m_ctrlTree.GetObjOwner(TM::e_otFrm))
//         nRet = TrackDlgPopupMenu(*this, pt, GetHWND(), TM::e_otFrm); //CPopMenu<TM::e_otFrm> ::Exec(pt, m_ctrlTree,this);
//     else if (ti == m_ctrlTree.GetObjOwner(TM::e_otPic))
//         nRet = TrackDlgPopupMenu(*this, pt, GetHWND(), TM::e_otPic); //CPopMenu<TM::e_otPic> ::Exec(pt, m_ctrlTree,this);
//     else 
//     {
//         CMenu muMain = LoadMenu(_Module.m_hInst, MAKEINTRESOURCE(IDR_MENU_POP));
//         CMenuHandle muPop = muMain.GetSubMenu(1);
//         CObjItem rec(ti);
//         //        rec.find(_FindByItem(), (TM::HTVITEM)ti);
//         TM::EObjType eType = (*rec)->GetType(); //  EDIT_MODE eMode = (*rec)->GetMode();
//         if (eType != TM::e_otWgt) // EM_WGT)
//         {
//             muPop.DeleteMenu(IDM_MOVE_UP, MF_BYCOMMAND);
//             muPop.DeleteMenu(IDM_MOVE_DOWN, MF_BYCOMMAND);
//         }
//         
//         if (eType != TM::e_otWnd) // EM_DIALOG)
//             muPop.DeleteMenu(IDM_TOOL_TEST, MF_BYCOMMAND);
//         
//         if (eType != TM::e_otWnd && eType != TM::e_otWgt)// EM_DIALOG && eMode != EM_WGT)
//         {
//             muPop.DeleteMenu(IDM_TOOL_ADDFILE, MF_BYCOMMAND);
//             muPop.DeleteMenu(IDM_TOOL_DELFILE, MF_BYCOMMAND);
//         }
//         
//         muPop.TrackPopupMenu(/*TPM_RETURNCMD |*/ TPM_LEFTALIGN,pt.x,pt.y,GetHWND());
//         return 0;
//     }
// //     if (nRet)
// //     {
// //         //ShowCurDlg(ti);
// //         ShowCtrlPropertyDlg(TMTrue);
// //     }
//     return nRet;
}

CPropertyDlg& CWndPanel::GetProp()
{
    return GetMainWnd().m_dlgProperty;
}

CObjItem CWndPanel::Obj2Item(IObjEdit* pItem)
{
    if (!pItem)
        return 0;
    return pItem < objEditMax
        ? m_ctrlTree.GetObjOwner((EObjType)((TMLParam)pItem -  1))
        : CObjItem(pItem->GetTvData());
}

void CWndPanel::OnSubclass(HTUI hWnd)
{
    root_class::OnSubclass(hWnd);
    if (hWnd)
        _dbgApp.TraceAddHook(this);
    else
        _dbgApp.TraceDelHook(this);
}

void CWndPanel::OnTrace(int tp, TMCStr pStr)
{
    if (e_tpAssert == (tp & e_tpAssert))
        CTraceMT::OnTrace(tp, pStr);
}

void CWndPanel::DoOnTrace(TMCStr pStr)
{
    TM::CTuiMemo(GetMainWnd().Item(IDC_MEMO_ALERT)).AppendText(pStr);
}

void CWndPanel::Alert(TMCStr pFmt, ...)
{
    va_list va;
    va_start (va, pFmt);
    TM::VL2T vl2t(pFmt, va);
    va_end (va);
    TM::CTuiMemo(GetMainWnd().Item(IDC_MEMO_ALERT)).AppendText(vl2t);
}

void CWndPanel::OnTrackFocus(void* pNewObj, const TM::CRect& rc, const TM::CString& strAlert)
{
    if (!strAlert.IsEmpty())
        Alert(strAlert);
    CDlgSpyFocus::OnTrackFocus(GetMainWnd().GetHWND(), pNewObj, rc);
}

//////////////////////////////////////////////////////////////////////////////
// IProjMgr
void CWndPanel::ItemAdd(IObjEdit* pPos, EZOrder mp, IObjEdit* pData, EObjType eType)
{
    DbgAssert((pPos == 0 || pPos > objEditMax) && pData);
    DbgAssert(!pData->GetTvData());
    
    TM::CString str(pData->ID2Text());
    //    TM::CString str = pData->GetMacroName();
    
    CObjItem itm;
    if (!pPos)
    {
        DbgAssert(eType != TM::e_otWgt);
        itm = m_ctrlTree.GetObjOwner(eType);
    }
    else
        itm = (TM::HTVITEM)(pPos->GetTvData());
    
    CObjItem itmc(itm.InsertChild(str));
    pData->SetTvData(itmc);
    itmc.SetData(pData);
    itmc.SetZOrder(itm, mp);
    
    if (pPos)
        itmc.Expand();
}

void CWndPanel::ItemMoveOrAdd(IObjEdit* pPos, EZOrder mp, IObjEdit* pData)
{
    CObjItem itmPos;
    if (!pPos)
        itmPos = m_ctrlTree.GetObjOwner(pData->GetType());
    else if (pPos < objEditMax)
        itmPos = m_ctrlTree.GetObjOwner(pPos);
    else
        itmPos = (TM::HTVITEM)(pPos->GetTvData());
    
    CObjItem itmc = (TM::HTVITEM)(pData->GetTvData());
    if (itmc)
    {
        itmc.SetZOrder(itmPos, mp);
        return;
    }

    TM::CString str(pData->ID2Text());
    itmc = itmPos.InsertChild(str);
    pData->SetTvData(itmc);
    itmc.SetData(pData);
    itmc.SetZOrder(itmPos, mp);
    if (itmc.GetLevel() > objLevelRoot)
        itmc.Expand();
}

void CWndPanel::ItemRemove(IObjEdit* pPos)
{
    DbgAssert(pPos > objEditMax);
    CObjItem itm((TM::HTVITEM)pPos->GetTvData());
    itm.Remove();
}

void CWndPanel::SelectItem(IObjEdit* pItem, TMUInt32 dwOption)
{
    CObjItem itm((TM::HTVITEM)pItem->GetTvData());
    itm.SelectItem(dwOption);
    //     itm.GetData()->InvalidateObj();
}

IObjEdit* CWndPanel::ItemGetSelectedData(TMBool bGetOwner)
{
    CObjItem itm = m_ctrlTree.GetSelectedItem(bGetOwner);
    if (itm)
        return itm.GetData();
    return 0;
}

IObjEdit* CWndPanel::GetDataByObjectID(TMUInt32 id)
{
    CObjItem rec(m_ctrlTree.GetRootItem());
    rec.find (CObjItem::_FindById(), id);
    return rec ? rec.GetData() : 0;
}

void CWndPanel::DeleteSelect()
{
    CObjItem tiSel;
    for (CObjItem ti(m_ctrlTree.GetRootItem()); ;)
    {
        CObjItem tiNew;
        tiNew = ti.GetGlobalNext();
        if (!tiNew)
            break;
        
        if (tiNew.IsSelected() && tiNew.GetLevel() > 1)
        {
            tiSel = ti;
            GetUndo().RemoveItem(tiNew.GetData());
        }
        else
            ti = tiNew;
    }
    
    if (tiSel)
        tiSel.SelectItem();
}

void CWndPanel::DeleteAll()
{
    for (CObjItem ti(m_ctrlTree.GetRootItem()); ;)
    {
        CObjItem tiNew;
        tiNew = ti.GetGlobalNext();
        if (!tiNew)
            break;
        
        IObjEdit* data = tiNew.GetData();
        if (data)
            GetUndo().RemoveItem(data);
        else
            ti = tiNew;
    }
}


TMUInt CWndPanel::ItemGetState(IObjEdit* pItem)
{
    return CObjItem(pItem->GetTvData()).GetState();
}

IObjEdit* CWndPanel::ItemGetParent(IObjEdit* pItem)
{
    if (!pItem)
        return 0;
    CObjItem itmc(CObjItem(pItem->GetTvData()).GetParent());
    return itmc ? itmc.GetData() : 0;
};

IObjEdit* CWndPanel::ItemGetChild(IObjEdit* pItem)
{
    CObjItem itmp(Obj2Item(pItem));
    if (!itmp)
        return 0;

    CObjItem itmc(itmp.GetChild());
    return itmc ? itmc.GetData() : 0;
}

IObjEdit* CWndPanel::ItemGetNext(IObjEdit* pItem)
{
    if (!pItem)
        return 0;
    CObjItem itmc(CObjItem(pItem->GetTvData()).GetNext());
    return itmc ? itmc.GetData() : 0;
}