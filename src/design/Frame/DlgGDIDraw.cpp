/**
 *    @file
 *    @brief
 *    CDlgDrawXXX
 *
 *
 *    @author 
 *    @date    2015-4-23
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2015 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "dlggdidraw.h"
#include "tmgdix.h"

using namespace TM;

CDlgDrawXXX::CDlgDrawXXX()
{
}

CDlgDrawXXX::~CDlgDrawXXX()
{
}

#define DFCS(x) { x, TM_T(#x)}

struct DFCSITEM
{
    int nid; TMCStrW pStr;
};

const DFCSITEM cs_di0[] = 
{
    DFCS(DFCS_CAPTIONCLOSE),
    DFCS(DFCS_CAPTIONMIN),
    DFCS(DFCS_CAPTIONMAX),
    DFCS(DFCS_CAPTIONRESTORE),
    DFCS(DFCS_CAPTIONHELP),
    {0, 0},
};

const DFCSITEM cs_di1[] = 
{
    DFCS(DFCS_MENUARROW),
    DFCS(DFCS_MENUCHECK),
    DFCS(DFCS_MENUBULLET),
    DFCS(DFCS_MENUARROWRIGHT),
    {0, 0},
};

const DFCSITEM cs_di2[] = 
{
    DFCS(DFCS_SCROLLUP),
    DFCS(DFCS_SCROLLDOWN),
    DFCS(DFCS_SCROLLLEFT),
    DFCS(DFCS_SCROLLRIGHT),
    DFCS(DFCS_SCROLLCOMBOBOX),
    DFCS(DFCS_SCROLLSIZEGRIP),
    DFCS(DFCS_SCROLLSIZEGRIPRIGHT),
    {0, 0},
};

const DFCSITEM cs_di3[] = 
{
    DFCS(DFCS_BUTTONCHECK),
    DFCS(DFCS_BUTTONRADIOIMAGE),
    DFCS(DFCS_BUTTONRADIOMASK),
    DFCS(DFCS_BUTTONRADIO),
    DFCS(DFCS_BUTTON3STATE),
    DFCS(DFCS_BUTTONPUSH),
    {0, 0},
};

const DFCSITEM cs_di[] = 
{
    DFCS(DFCS_INACTIVE),
    DFCS(DFCS_PUSHED),
    DFCS(DFCS_CHECKED),
    DFCS(DFCS_TRANSPARENT),
    DFCS(DFCS_HOT),
    DFCS(DFCS_FLAT),
    DFCS(DFCS_MONO),
};


const DFCSITEM cs_de[] = 
{
    DFCS(BDR_RAISEDOUTER),
    DFCS(BDR_SUNKENOUTER),
    DFCS(BDR_RAISEDINNER),
    DFCS(BDR_SUNKENINNER),
};

const DFCSITEM cs_de2[] = 
{
    DFCS(BF_LEFT),
    DFCS(BF_TOP),
    DFCS(BF_RIGHT),
    DFCS(BF_BOTTOM),
    DFCS(BF_DIAGONAL),
    DFCS(BF_MIDDLE),
    DFCS(BF_SOFT),
    DFCS(BF_ADJUST),
    DFCS(BF_FLAT),
    DFCS(BF_MONO),
};

template <class T>
void LBAddString(HTUI hWgt, T &di)
{
    CTuiListBox lst(hWgt);
    tm_for (int i = 0; i < sizeof(di) / sizeof(*di); ++i)
    {
        lst.AddString(di[i].pStr);
        lst.SetItemData(i, di[i].nid);
    }
}

TMResult CDlgDrawXXX::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    CTuiListBox lst(Item(IDC_LISTBOX_31141));
    lst.AddString(L"DFC_CAPTION"); //DFC_BUTTON;
    lst.AddString(L"DFC_MENU");
    lst.AddString(L"DFC_SCROLL");
    lst.AddString(L"DFC_BUTTON");
    lst.AddString(L"DFC_POPUPMENU");

    LBAddString(Item(IDC_LISTBOX_31146), cs_di);

    LBAddString(Item(IDC_LISTBOX_31149), cs_de);
    LBAddString(Item(IDC_LISTBOX_31150), cs_de2);

    return 0;
}

TMResult CDlgDrawXXX::OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgDrawXXX::OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgDrawXXX::OnSelChange (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    CTuiListBox wnd((HTUI)hWndCtl);
    if (wnd.GetDlgCtrlObjectID() == IDC_LISTBOX_31141)
    {
        int n = wnd.GetCurSel();
        const DFCSITEM* p[5] = { cs_di0, cs_di1, cs_di2, cs_di3, cs_di1 };
        CTuiListBox lst(Item(IDC_LISTBOX_31142));
        lst.ResetContent();
        for (int i = 0; ; ++i)
        {
            if (!p[n][i].pStr)
                return 0;
            lst.AddString(p[n][i].pStr);
            lst.SetItemData(i, p[n][i].nid);
        }
    }

    Item(IDC_WINDOW_31145).Invalidate();
    Item(IDC_WINDOW_31153).Invalidate();
    
    return 0;
}

UINT GetListFlag(CTuiListBox lst)
{
    UINT uState = 0;
    TMInt c[32];
    int n = lst.GetSelItems(32, c);
    for (int i = 0; i < n; ++i)
        uState += lst.GetItemData(c[i]);
    return uState;
}

struct CAutoBk : public CTexture
{
    CTuiRect rc;
    CRect rcd;
    CAutoBk(CTuiWidget wnd)
    {
        wnd.GetClientRect(&rc);
        wnd.GetDPI().LPtoDP(rc, rcd);
        this->CreateIndirect(rcd.Width(), rcd.Height());
        rcd.MoveToXY(0, 0);
    }

    ~CAutoBk()
    {
        (*this)->Blt(rc, CRect(0,0,0,0), GX_BLT_DFTSRCWH | GX_BLT_SRCCOPY);
    }
};

TMResult CDlgDrawXXX::OnPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    CTuiListBox lst(Item(IDC_LISTBOX_31141));
    int n = lst.GetCurSel();
    UINT uType = n + DFC_CAPTION;

    UINT uState = GetListFlag(Item(IDC_LISTBOX_31142).m_hWnd);
    uState |= GetListFlag(Item(IDC_LISTBOX_31146).m_hWnd);

    CAutoBk bk(pMsg->hWnd);
    {
        CAutoTextureDC dc(bk);
        dc.FillSolidRect(&bk.rcd, RGB(255,0,255));
        bk.rcd.InflateRect(-10,-10);
        DrawFrameControl(dc, &bk.rcd, uType, uState);
    }
    return 0;
}

TMResult CDlgDrawXXX::OnPrintClient2(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    UINT edge = GetListFlag(Item(IDC_LISTBOX_31149).m_hWnd);
    UINT grfFlags = GetListFlag(Item(IDC_LISTBOX_31150).m_hWnd);
    
    CAutoBk bk(pMsg->hWnd);
    {
        CAutoTextureDC dc(bk);
        dc.FillSolidRect(&bk.rcd, RGB(255,0,255));
        bk.rcd.InflateRect(-10,-10);
        DrawEdge(dc, &bk.rcd, edge, grfFlags);
    }
    return 0;
}

