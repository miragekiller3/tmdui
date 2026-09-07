/**
 *    @file
 *    @brief
 *    CDlgSysColor
 *
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2017-1-18
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2017 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "DlgSysColor.h"
#include "Tester.h"
#include "tmclipboard.h"
#include "../Design.h"
#include "tmwnd.h"

using namespace TM;


//static COLORREF cs_clrNormal[]    = {0xc8c8c8, 0x0, 0xd1b499, 0xdbcdbf, 0xf0f0f0, 0xffffff, 0x646464, 0x0, 0x0, 0x0, 0xb4b4b4, 0xfcf7f4, 0xababab, 0xd77800, 0xffffff, 0xf0f0f0, 0xa0a0a0, 0x6d6d6d, 0x0, 0x0, 0xffffff, 0x696969, 0xe3e3e3, 0x0, 0xe1ffff, 0x0, 0xcc6600, 0xead1b9, 0xf2e4d7, 0xd77800, 0xf0f0f0 };
//static COLORREF cs_clrNormal[]    = {0xc8c8c8, 0x0, 0xd1b499, 0xdbcdbf, 0xf0f0f0, 0xffffff, 0x646464, 0x0, 0x0, 0x0, 0xb4b4b4, 0xfcf7f4, 0xababab, 0xd77800, 0xffffff, 0xf0f0f0, 0xa0a0a0, 0x6d6d6d, 0x0, 0x0, 0xffffff, 0x696969, 0xe3e3e3, 0x0, 0xe1ffff, 0x0, 0xcc6600, 0xead1b9, 0xf2e4d7, 0xd77800, 0xf0f0f0, };
static const COLORREF cs_clrNormal[]    = {0xc8d0d4, 0xa56e3a, 0x6a240a, 0x808080, 0xc8d0d4, 0xffffff, 0x0, 0x0, 0x0, 0xffffff, 0xc8d0d4, 0xc8d0d4, 0x808080, 0x6a240a, 0xffffff, 0xc8d0d4, 0x808080, 0x808080, 0x0, 0xc8d0d4, 0xffffff, 0x404040, 0xc8d0d4, 0x0, 0xe1ffff, 0x0, 0xcc6600, 0xf0caa6, 0xc0c0c0, 0xd77800, 0xf0f0f0, };
static const COLORREF cs_clrWin10[]     = {0xc8c8c8, 0x0, 0xd1b499, 0xdbcdbf, 0xf0f0f0, 0xffffff, 0x646464, 0x0, 0x0, 0x0, 0xb4b4b4, 0xfcf7f4, 0xababab, 0xd77800, 0xffffff, 0xf0f0f0, 0xa0a0a0, 0x6d6d6d, 0x0, 0x0, 0xffffff, 0x696969, 0xe3e3e3, 0x0, 0xe1ffff, 0x0, 0xcc6600, 0xead1b9, 0xf2e4d7, 0xd77800, 0xf0f0f0, };
static const COLORREF cs_clrWin10_hc1[] = {0x0, 0x0, 0xff0000, 0xffff00, 0x0, 0x0, 0xffffff, 0xffffff, 0xffff, 0xffffff, 0xff0000, 0xffff00, 0x0, 0x8000, 0xffffff, 0x0, 0x808080, 0xff00, 0xffffff, 0x0, 0xc0c0c0, 0xffffff, 0xffffff, 0xffff, 0x0, 0xc0c0c0, 0xff8080, 0xff0000, 0xffff00, 0x8000, 0x0 };
static const COLORREF cs_clrWin10_hc2[] = {0x0, 0x0, 0xffff00, 0x400000, 0x0, 0x0, 0xffffff, 0xff00, 0xff00, 0x0, 0xffff00, 0xff0000, 0xffffff, 0xff0000, 0xffffff, 0x0, 0x808080, 0xc0c0c0, 0xffffff, 0xffffff, 0xc0c0c0, 0xffffff, 0xffffff, 0x0, 0xffff, 0xc0c0c0, 0xff8080, 0xffff00, 0xff0000, 0xff0000, 0x0 };
static const COLORREF cs_clrWin10_hc3[] = {0x0, 0x0, 0x6e0037, 0x2f0000, 0x0, 0x0, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffff, 0x8000, 0x0, 0xffeb1a, 0x0, 0x0, 0x808080, 0x3ff23f, 0xffffff, 0xffffff, 0xc0c0c0, 0xffffff, 0xffffff, 0xffffff, 0x0, 0xc0c0c0, 0xffff, 0x800080, 0x8000, 0x800080, 0x0 };
static const COLORREF cs_clrWin10_hc4[] = {0xffffff, 0xffffff, 0x0, 0xffffff, 0xffffff, 0xffffff, 0x0, 0x0, 0x0, 0xffffff, 0x808080, 0xc0c0c0, 0x808080, 0x6e0037, 0xffffff, 0xffffff, 0x808080, 0x60, 0x0, 0x0, 0xc0c0c0, 0x0, 0xc0c0c0, 0x0, 0xffffff, 0xc0c0c0, 0x9f0000, 0x0, 0xffffff, 0x0, 0xffffff };

enum { eCount = 31 };
static COLORREF s_clrDefault[eCount];


static void _SetSysColors(const COLORREF* clrs)
{
    for (int id = 0; id < eCount; ++id)
        SetSysColors(1, &id, &(clrs[id]));
}

static void _GetSysColors(COLORREF* clrs)
{
    for (int i = 0; i < eCount; ++i)
        clrs[i] = GetSysColor(i);
}

static TMBool _InitSysColors()
{
    _GetSysColors(s_clrDefault);
    return true;
}
static TMBool b = _InitSysColors();

static bool s_bDark = false;
void TesterSetHighContrast(IProj* proj, int nCmd)
{
    // dark
    s_bDark = (nCmd == IDC_RADIOBUTTON_33067);
    static TM::CColorTransformDark style;
    IProj::CModuleListH aModule;
    proj->GetModuleList(&aModule, 0);
    for (IProj::CModuleListH::recorder rec(aModule); rec; ++rec)
        AppGetTester()->SetColorTransform(rec.get_data(), s_bDark ? &style : 0);

    // HC
    TMBool bHC = (nCmd >= IDC_RADIOBUTTON_33061 && nCmd <= IDC_RADIOBUTTON_33066);
    HIGHCONTRAST hc = { sizeof(HIGHCONTRAST) };
    SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &hc, FALSE);
    if (bHC)
        hc.dwFlags |= HCF_HIGHCONTRASTON;
    else
        hc.dwFlags &= ~HCF_HIGHCONTRASTON;
    DbgVerify(SystemParametersInfo(SPI_SETHIGHCONTRAST, sizeof(HIGHCONTRAST), &hc, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE));
    AppGetTester()->SetHighContrast(bHC);
    AppGetEditor()->SetHighContrast(bHC);

    // color
    switch (nCmd)
    {
        case IDC_RADIOBUTTON_33054:
        case IDC_RADIOBUTTON_33061:
            _SetSysColors(cs_clrNormal);
            break;
        
        case IDC_RADIOBUTTON_33055:
        case IDC_RADIOBUTTON_33062:
            _SetSysColors(cs_clrWin10);
            break;

        case IDC_RADIOBUTTON_33063:
            _SetSysColors(cs_clrWin10_hc1);
            break;

        case IDC_RADIOBUTTON_33064:
            _SetSysColors(cs_clrWin10_hc2);
            break;

        case IDC_RADIOBUTTON_33065:
            _SetSysColors(cs_clrWin10_hc3);
            break;

        case IDC_RADIOBUTTON_33066:
            _SetSysColors(cs_clrWin10_hc4);
            break;
    }
}

int TesterGetHighContrast(IProj* proj)
{
    // dark
    if (s_bDark)
        return IDC_RADIOBUTTON_33067;

    TMBool bHC = AppGetTester()->IsHighContrast();

    int nClr;
    COLORREF clr[eCount];
    _GetSysColors(clr);
    if (MemCmp(clr, cs_clrNormal, sizeof(cs_clrNormal)) == 0)
        nClr = 0;
    else if (MemCmp(clr, cs_clrWin10, sizeof(cs_clrWin10)) == 0)
        nClr = 1;
    else if (MemCmp(clr, cs_clrWin10_hc1, sizeof(cs_clrWin10_hc1)) == 0)
        nClr = 2;
    else if (MemCmp(clr, cs_clrWin10_hc2, sizeof(cs_clrWin10_hc2)) == 0)
        nClr = 3;
    else if (MemCmp(clr, cs_clrWin10_hc3, sizeof(cs_clrWin10_hc3)) == 0)
        nClr = 4;
    else if (MemCmp(clr, cs_clrWin10_hc4, sizeof(cs_clrWin10_hc4)) == 0)
        nClr = 5;
    else
        return 0;

    return ((bHC || nClr > 1)  ? IDC_RADIOBUTTON_33061 : IDC_RADIOBUTTON_33054) + nClr;
}


CDlgSysColor::CDlgSysColor()
    : m_pClr(s_clrDefault)
    , m_bDark(false)
{
}

CDlgSysColor::~CDlgSysColor()
{
}

// TMResult CDlgSysColor::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     return 0;
// }

TMResult CDlgSysColor::OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//     EndDialog(wID);
   DestroyWindow();
    return 0;
}

TMResult CDlgSysColor::OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//     EndDialog(wID);
   DestroyWindow();
    return 0;
}

// TMResult CDlgSysColor::OnSys(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
// {
// //     CStringA str("static COLORREF g_clrHC[] = {");
// //     for (int i = 0; i < eCount; ++i)
// //     {
// //         COLORREF clr = GetSysColor(i);
// //         CStringA strx;
// //         strx.Format("0x%x, ", clr);
// //         str += strx;
// //     }
// //     str += L"};\r\n";
// //     TxtFileSave(L"color_sys.h", str);
// 
//     return 0;
// }

TMResult CDlgSysColor::OnSet (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    if (wID == IDC_PUSHBUTTON_SYS)
        m_pClr = 0;
    else
    {
        if (wID == IDC_PUSHBUTTON_30381)
        {
            wID = IDC_PUSHBUTTON_31250;
            m_bDark =  true;
        }
        else
            m_bDark = false;
        
        int n = wID - IDC_PUSHBUTTON_31250;
        const COLORREF* p[] = {cs_clrNormal, cs_clrWin10, cs_clrWin10_hc1, cs_clrWin10_hc2, cs_clrWin10_hc3, cs_clrWin10_hc4 };
        //_SetSysColors(p[n]);
        m_pClr = p[n];
    }
    Invalidate();
    return 0;
}

#define COLOR_MENUHILIGHT       29
#define COLOR_MENUBAR           30

#define INSERT1(x) this->insert2(L###x, eColorWeb##x);
#define INSERT2(x) this->insert2(L###x, (EColorIndex)x);
struct sys_color_CDecodeTable : public TM::CMap<TM::CStrViewW, EColorIndex>
{
    sys_color_CDecodeTable()
    {
        INSERT2(COLOR_SCROLLBAR              );
        INSERT2(COLOR_BACKGROUND             );
        INSERT2(COLOR_ACTIVECAPTION          );
        INSERT2(COLOR_INACTIVECAPTION        );
        INSERT2(COLOR_MENU                   );
        INSERT2(COLOR_WINDOW                 );
        INSERT2(COLOR_WINDOWFRAME            );
        INSERT2(COLOR_MENUTEXT               );
        INSERT2(COLOR_WINDOWTEXT             );
        INSERT2(COLOR_CAPTIONTEXT            );
        INSERT2(COLOR_ACTIVEBORDER           );
        INSERT2(COLOR_INACTIVEBORDER         );
        INSERT2(COLOR_APPWORKSPACE           );
        INSERT2(COLOR_HIGHLIGHT              );
        INSERT2(COLOR_HIGHLIGHTTEXT          );
        INSERT2(COLOR_BTNFACE                );
        INSERT2(COLOR_BTNSHADOW              );
        INSERT2(COLOR_GRAYTEXT               );
        INSERT2(COLOR_BTNTEXT                );
        INSERT2(COLOR_INACTIVECAPTIONTEXT    );
        INSERT2(COLOR_BTNHIGHLIGHT           );
        INSERT2(COLOR_3DDKSHADOW             );
        INSERT2(COLOR_3DLIGHT                );
        INSERT2(COLOR_INFOTEXT               );
        INSERT2(COLOR_INFOBK                 );
        INSERT2(COLOR_HOTLIGHT               );
        INSERT2(COLOR_GRADIENTACTIVECAPTION  );
        INSERT2(COLOR_GRADIENTINACTIVECAPTION);
        INSERT2(COLOR_MENUHILIGHT            );
        INSERT2(COLOR_MENUBAR                );

        INSERT1(AccentColor);
        INSERT1(AccentColorText);
        INSERT1(ActiveText);
        INSERT1(ButtonBorder);
        INSERT1(ButtonFace);
        INSERT1(ButtonText);
        INSERT1(Canvas);
        INSERT1(CanvasText);
        INSERT1(Field);
        INSERT1(FieldText);
        INSERT1(GrayText);
        INSERT1(Highlight);
        INSERT1(HighlightText);
        INSERT1(LinkText);
        INSERT1(Mark);
        INSERT1(MarkText);
        INSERT1(SelectedItem);
        INSERT1(SelectedItemText);
        INSERT1(VisitedText);
    }
};
#undef INSERT1
#undef INSERT2




TMResult CDlgSysColor::OnColorPrintClient(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    CTuiRect rc;
    
    CTuiWidget wnd(pMsg->hWnd);
    CStrView strText = wnd.GetWindowText();
    static sys_color_CDecodeTable table;
    EColorIndex index = table[strText];
    TMColor clr;
    if (m_pClr)
        clr = ColorFromCOLORREF(m_pClr[index]);// ColorFromSys(pMsg->uTag);
    else
        clr = ColorFromSys(index);
    if (m_bDark)
        clr = CColorTransformDark().ConvertColor(0, clr);

    wnd.GetClientRect(&rc);
    rc.InflateRect(-1, -1);
    rc.right = (rc.left + 35);
    _gxApp->FillRect(rc, clr);
    DefRouteProc(pMsg);
    return 0;
}

TMResult CDlgSysColor::OnLButtonDown(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    CString str;
    CTuiWidget wnd(pMsg->hWnd);
    wnd.GetWindowText(str);
    CClipboard().AddString(str);
    bHandled = false;
    return 0;
}

TMResult CDlgSysColor::OnLButtonDblClk(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    CColorDialog clrDlg;
    int id = pMsg->uTag;
    COLORREF clr = GetSysColor(id);
    if (IDOK == clrDlg.DoModal(GetHWND(), clr))
    {
        clr = clrDlg.GetColor();
        SetSysColors(1, &id, &clr);
    }
    return 0;
}
