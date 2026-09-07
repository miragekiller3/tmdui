/**
 *    @file
 *    @brief
 *    CDlg1000
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2018-5-22
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2018 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "Dlg1000.h"

using namespace TM;


CDlg1000::CDlg1000()
    : m_dwStyle(0)
{
}

CDlg1000::~CDlg1000()
{
}

TMResult CDlg1000::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
//     _tuiApp.AddMessageFilter(this);
    OnSetType(0, 0, 0, bHandled);
    return 0;
}

TMResult CDlg1000::OnNcCreate(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    if (m_dwStyle)
        CTuiWidget(pMsg->hWnd).SetStyle(m_dwStyle);
    bHandled = false;
    return 0;
}


// TMResult CDlg1000::OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     bHandled = false;
// //     _tuiApp.RemoveMessageFilter(this);
//     return 0;
// }

TMResult CDlg1000::OnCommand(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;

    int id = LOWORD(wParam);
    if (id != IDC_COMBOBUTTON_1003 || id != IDC_LISTBOX_1004)
        return 0;
    
    if (!CTuiButton(Item(IDC_LOG)).GetCheck())
        return 0;

    CTuiWidget wnd((HTUI)lParam);
    int code = HIWORD(wParam);
    CString strCode;
    if (id == IDC_COMBOBUTTON_1003)
    {
        if (code == BN_CLICKED)
            strCode = L"BN_CLICKED";
        else if (code == CBN_SELCHANGE)
            strCode = L"CBN_SELCHANGE";
        else if (code == CBN_DROPDOWN)
            strCode = L"CBN_DROPDOWN";
        else if (code == CBN_CLOSEUP)
            strCode = L"CBN_CLOSEUP";
        else if (code == CBN_SELENDOK)
            strCode = L"CBN_SELENDOK";
        else if (code == CBN_SELENDCANCEL)
            strCode = L"CBN_SELENDCANCEL";
        else
            strCode.Format(L"%d", code);
    }
    else if (id == IDC_LISTBOX_1004)
    {
        if (code == BN_CLICKED)
            strCode = L"BN_CLICKED";
        else if (code == LBN_SELCANCEL)
            strCode = L"LBN_SELCANCEL";
        else if (code == LBN_SELCHANGE)
            strCode = L"LBN_SELCHANGE";
        else if (code == LBN_SETFOCUS)
            strCode = L"LBN_SETFOCUS";
        else if (code == LBN_KILLFOCUS)
            strCode = L"LBN_KILLFOCUS";
        else
            strCode.Format(L"%d", code);
    }

    CTuiListBox lb(CTuiComboButton(Item(IDC_COMBOBUTTON_1003)).GetPopList());
    int sel = lb.GetCurSel();
    CString strC;
    wnd.GetClassName(strC);
    CString str;
    str += strC;
    str.Format(L"WM_COMMAND: %s; id:%d; code:%s; sel:%d\r\n", strC.c_str(), id, strCode.c_str(), sel);
    CTuiMemo(Item(IDC_MEMO_1005)).AppendText(str);

    return 0;
}

TMResult CDlg1000::OnSetType(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiWidget wndComb(Item(IDC_COMBOBUTTON_1003));
    m_dwStyle = wndComb ? wndComb.GetStyle() : 0;
    CTuiWidget wndc(Item(IDC_CONT));
    if (wndc)
        wndc.DestroyWindow();

    enum { eBut = 0x01, eDlg = 0x02, eTree = 0x04 };

    int bBut  = CTuiCheckBox(Item(IDC_SETTYPE_BUT)).GetCheck() ? eBut : 0;
    int bDlg  = CTuiCheckBox(Item(IDC_SETTYPE_DLG)).GetCheck() ? eDlg : 0;
    int bTree = CTuiCheckBox(Item(IDC_SETTYPE_TREE)).GetCheck() ? eTree : 0;
    int nid = IDC_WINDOW_EDIT_LIST;
    switch (bBut | bDlg | bTree)
    {
    case eBut:                  nid = IDC_WINDOW_BUT_LIST; break;
    case eBut | eDlg:           nid = IDC_WINDOW_BUT_LIST_DLG; break;
    case eBut | eTree:          nid = IDC_WINDOW_BUT_TREE; break;
    case eBut | eDlg | eTree:   nid = IDC_WINDOW_BUT_TREE_DLG; break;

    case eDlg:                  nid = IDC_WINDOW_EDIT_LIST_DLG; break;
    case eDlg | eTree:          nid = IDC_WINDOW_EDIT_TREE_DLG; break;

    case eTree:                 nid = IDC_WINDOW_EDIT_TREE; break;
    }

    CTuiWidget wnd;
    wnd.Create(Item(IDC_WINDOW_1009), nid);
    
    return 0;
}

TMResult CDlg1000::OnDropDown(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    bHandled = false;
    CTuiWidget wpop(CTuiComboButton((HTUI)hWndCtl).GetPopList());
    CString strClass;
    wpop.GetClassName(strClass);
    if (strClass == TUI_CLASS_LISTBOX)
    {
        CTuiListBox lb(wpop);
        //for (int i = 0; ;++i)
        //{
        //    //CString str;
        //    //str.Format(L"listbox hint %d", i);
        //    //lb.SetHint()

        //}
    }
    else if (strClass == TUI_CLASS_TREE)
    {
        CTuiTree tree(wpop);
        CTVItem itm(tree.GetRootItem());
        for (int i = 0; itm = itm.GetGlobalNext(); ++i)
        {
            CString str;
            str.Format(L"tree hint %d", i);
            itm.SetHint(str);
        }
    }
    return 0;
}


TMResult CDlg1000::OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlg1000::OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

//CTuiMsgFilter
// BOOL CDlg1000::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }
