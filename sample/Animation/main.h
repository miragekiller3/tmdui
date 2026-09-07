#ifndef MAIN_H
#define MAIN_H

#include "Resource.h"
#include "tuires.h"
//#include "../h/zoom.h"

TM::CTuiListBox g_lb;

void OutPut (TMCStr pFmt, ...)
{
    TM::CString str;
    
    va_list va;
    va_start(va, pFmt);
    str.FormatV (pFmt, va);
    va_end(va);
    if (g_lb)
    {
        g_lb.InsertString (0, str);
    }
    DbgTrace (str);
}


// #include "AniCube.h"
#include "AniFade.h"
//#include "AniMove.h"
#include "AniCircle.h"


class CMain : public TM::CTuiWidgetImpl<CMain>
{
public:
    enum { IDD = IDD_TUI_MANAGE_10002 };
    
    TM::CTuiWidget  m_wndPic;
//     TM::CAniStar  m_wndStar[STARTNUM];
//     int m_nPos;
//     CCubeWnd        m_wndCube;
    CFadeWnd        m_wndFade;
//     CMoveWnd        m_wndMove;
    CCircleWnd      m_wndCircle;
//    CCircleWnd      m_wndCircle2;
//    CCircleWnd      m_wndCircle3;

//     CFpsWindow<IDC_FPS> m_wndFps;

public:
    BEGIN_TUIMSG_MAP        (CMain)
        MESSAGE_HANDLER     (WM_INITDIALOG,     OnInitDialog)
        COMMAND_ID_HANDLER  (IDOK,              OnCancel)
        COMMAND_ID_HANDLER  (IDCANCEL,          OnCancel)
        COMMAND_ID_HANDLER  (IDC_MOVEWINDOW,    OnMoveWindow)
        COMMAND_ID_HANDLER  (IDC_FADE,          OnFade)
        COMMAND_ID_HANDLER  (IDC_MOVE,          OnMove)
        COMMAND_ID_HANDLER  (IDC_SHOWBUBBLE,    OnShowBubble)
        ROUTE_OID_HANDLER   (IDC_LAYER_22007, WM_SHOWWINDOW, OnShowWindowLayer)
        

//         COMMAND_ID_HANDLER  (IDC_ANICUBE,       OnCubeAni)
        COMMAND_ID_HANDLER  (IDC_ANICIRCLE,     OnCircleAni)




//         MESSAGE_HANDLER     (WM_DESTROY,        OnDestroy)

        

//         HOOK_HANDLER        (WM_MOUSEMOVE,      OnMouseMove)
        
//         CHAIN_MSG_MAP       (root_class)
    END_MSG_MAP()


    TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
    {
//         m_wndFps.SubclassWindow(m_hWnd);

        ::SetWindowText(GetHWND(), TM_T("TMDui Animation Sameple"));
        srand (GetTickCount());
        m_wndPic = GetDlgItem(IDC_PIC);
        g_lb = Item (ID_LIST);
        //         bHandled = false;
//         for (int i = 0; i < STARTNUM; ++i)
//         {
//             m_wndStar[i].Create(m_hWnd, 0, IDO_TUI_WINDOW_10012);
//         }
//         m_nPos = 0;

//         TM::CTuiWidget wnd(Item(IDC_FADE));
//         wnd.DestroyWindow();
//         wnd = wnd.GetParent();

        return 0;
    }


//     TMResult OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
//     {
//         bHandled = true;
//         HWND hWnd = GetHWND();
//         ::DestroyWindow(hWnd);
//         return 0;
//     }


//     TMResult OnMouseMove(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
//     {
//         TM::CPoint pt(lParam);
// 
//         m_wndStar[m_nPos].SetWindowPos (HTUI_TOP, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
//         m_wndStar[m_nPos].CreateAni ();
// 
//         m_nPos++;
// 
//         if (m_nPos >= STARTNUM)
//         {
//             m_nPos = 0;
//         }
// 
//         bHandled = false;
//         return 0;
//     }

    TMResult OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        g_lb = 0;
        //EndDialog(wID);
        m_wndCircle.EnabelAni(0, false);
        DestroyWindow();
        return 0;
    }

    TMResult OnMoveWindow(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        TM::CTuiRect rc;
        GetClientRect(&rc);
        int w = MulDiv (100, rand(), RAND_MAX) + 50;
        int h = MulDiv (90, w, 74);
        int l = 24 + MulDiv (rc.right - w - 24, rand(), RAND_MAX); // 24ÊÇcaption¸ß¶È
        int t = MulDiv(rc.bottom - h, rand(), RAND_MAX);
        TM::CTuiRect rc2(l, t, l + w, h + t);

        DbgAssert(m_wndPic);
        //CAniMove::CreateInstance()->CreateAni (m_wndPic, rc2);
//         m_wndMove.CreateAni (m_wndPic, rc2, 0);
        m_wndPic.AnimateWindow(3000, 0, 0, &rc2);

        return 0;
    }

    TMResult OnFade(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        DbgAssert(m_wndPic);

//         TM::CTuiRect rc;
//         GetWindowRect(&rc);
//         SetWindowPos(0, -20, -20, rc.Width() + 40, rc.Height() + 40, 0);


        //*/
        m_wndFade.CreateAni(m_wndPic, m_wndPic.IsWindowVisible());
        /*/
        CFadeWnd* pwin = new CFadeWnd();
        pwin->CreateAni(m_wndPic, m_wndPic.IsWindowVisible());
        delete pwin;
        //*/
        return 0;
    }

    TMResult OnMove(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        DbgAssert(m_wndPic);

        TM::CTuiRect rc;
        GetClientRect (&rc);
        rc.left = rc.right;
        rc.top = rc.bottom;


        TMUInt32 nflag = 0;
        m_wndPic.StopAnimateWindow();
        if (m_wndPic.IsWindowVisible())
            m_wndPic.AnimateWindow(3000, AW_HIDE, 0, &rc);
        else
            m_wndPic.AnimateWindow(3000, 0, &rc, 0);
//        m_wndMove.CreateAni (m_wndPic, rc, 1 + m_wndPic.IsWindowVisible());


//         if (m_wndPic.IsWindowVisible())
//         {
//             /*TM::CRefWindow<CAniMoveOut>*/(new CAniMoveOut())->CreateAni (m_wndPic, rc);
//         }
//         else
//         {
//             /*TM::CRefWindow<CAniMoveIn>*/(new CAniMoveIn())->CreateAni (m_wndPic, rc);
//         }

        return 0;
    }

    TMResult OnShowBubble(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        //CTuiWidget wndFloat(Item(IDC_FLOAT_22006));
        TM::CTuiWidget wndItem;
        wndItem.Create(Item(IDC_FLOAT_22006), IDC_LAYER_22007);
        TM::CString str;
        Item(IDC_FPS).GetWindowText(str);
        wndItem.Item(IDC_PUSHBUTTON_22008).SetWindowText(str);
        wndItem.ShowWindow(SWP_SHOWWINDOW);
        wndItem.AnimateWindow(5000, AW_BLEND | AW_HIDE | AW_ERASEMODEIN);
        return 0;
    }

    TMResult OnShowWindowLayer(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
    {
        bHandled = false;
        if (!pMsg->wParam)
            CTuiWidget(pMsg->hWnd).DestroyWindow();
        return 0;
    }


//     TMResult OnCubeAni(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
//     {
// //         INPUT ipt[3] = {0};
// // //         ipt[0].type = ipt[1].type = ipt[2].type = INPUT_HARDWARE;
// // //         ipt[0].hi.uMsg = WM_LBUTTONDOWN;
// // //         ipt[1].hi.uMsg = WM_MOUSEMOVE;
// // //         ipt[2].hi.uMsg = WM_LBUTTONUP;
// // //         ipt[1].hi.wParamH = MK_LBUTTON;
// //         ipt[0].type = ipt[1].type = ipt[2].type = INPUT_MOUSE;
// //         ipt[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
// //         ipt[1].mi.dwFlags = MOUSEEVENTF_MOVE;
// //         ipt[1].mi.dx = 10;
// //         ipt[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;
// //         SendInput(1, ipt, sizeof(INPUT));
// //         SendInput(1, &ipt[1], sizeof(INPUT));
// // //         SendInput(1, &ipt[2], sizeof(INPUT));
// //         x;
// 
// 
// //         TM::CTuiButton but((TM::HTUI)hWndCtl);
// //         if (but.GetCheck())
// //         {
// //             if (!m_wndCube.m_hWnd)
// //                 m_wndCube.SubclassWindow(Item(IDW_WINDOW_20016));
// //         }
// //         else
// //         {
// //             if (m_wndCube.m_hWnd)
// //             {
// //                 m_wndCube.KillAni();
// //                 m_wndCube.UnsubclassWindow();
// //             }
// //         }
//         return 0;
//     }

    TMResult OnCircleAni(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
    {
        TM::CTuiButton but((TM::HTUI)hWndCtl);
        m_wndCircle.EnabelAni(Item(IDW_WINDOW_22002), but.GetCheck());

//         CCircleWnd wndCircle[10];
//         for (int i = 0; i < 10000; ++i)
//         {
//             wndCircle[i % 10].Create(m_hWnd, 0, IDW_WINDOW_22002);
//             wndCircle[i % 10].DestroyWindow();
//         }
        return 0;
    }
};    

//static CMain::Register<0> s_callback_CMain;




#endif // MAIN_H