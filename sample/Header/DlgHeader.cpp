/**
 *  @file
 *  @brief
 *  CDlgHeader
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2025-6-27
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2025 miragekiller                             
 *  All rights reserved  
 */

#include "stdafx.h"
#include "DlgHeader.h"

using namespace TM;


CDlgHeader::CDlgHeader()
{
}

CDlgHeader::~CDlgHeader()
{
}

TMResult CDlgHeader::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    HDITEM itm = { HDI_TEXT | HDI_WIDTH | HDI_FORMAT, 80, L"left" };
    CTuiHeader head(Item(IDC_HEADER_1003));
    head.InsertItem(0, &itm);

    itm.pszText = L"center";
    itm.fmt = HDF_CENTER;
    head.InsertItem(1, &itm);
    
    itm.pszText = L"right";
    itm.fmt = HDF_RIGHT | HDF_SORTDOWN;
    head.InsertItem(2, &itm);

    itm.pszText = L"r + r";
    itm.fmt = HDF_RIGHT | HDF_BITMAP_ON_RIGHT | HDF_SORTUP;
    head.InsertItem(3, &itm);

    itm.pszText = L"c + r";
    itm.fmt = HDF_RIGHT | HDF_BITMAP_ON_RIGHT | HDF_SORTDOWN;
    head.InsertItem(4, &itm);

//     _tuiApp.AddMessageFilter(this);
    return 0;
}

TMResult CDlgHeader::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
//     _tuiApp.RemoveMessageFilter(this);
    return 0;
}

TMResult CDlgHeader::OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgHeader::OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

//CTuiMsgFilter
// BOOL CDlgHeader::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }
