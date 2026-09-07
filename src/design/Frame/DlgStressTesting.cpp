/**
 *    @file
 *    @brief
 *    CDlgStressTesting
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2018-12-12
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2018 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "DlgStressTesting.h"
#include <TLHELP32.H>
#include "tmthread.h"
#include "tmkeyboard.h"
#include "tmtime.h"

// #define PMIB_ICMP_EX void*
// #include <Iphlpapi.h>
// #pragma comment (lib, "Iphlpapi.lib")

using namespace TM;


#define STEST_TRACE DbgTrace
// #define STEST_TRACE DbgTraceNull


static int _CDlgStressTesting_IsAppWindow(HWND hWnd)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId())
    {
        CTuiWidget wnd((HTUI)::SendMessage(hWnd, CM_GET_WIDGET, 0, 0));
        if (!wnd)
            return 1;
        if (wnd.GetAPP() == &_tuiApp)
            return 2;
    }
    return 0;
}

struct ENUMPARAM
{
    HTUI hFore;
    float sz;
};

TMBool CALLBACK _EnumDlg(HTUI hwnd, TMLParam lParam)
{
    ENUMPARAM* ep = (ENUMPARAM*)lParam;
    CTuiRect rc;
    CTuiWidget(hwnd).GetWindowRect(&rc);
    float sz = rc.Width() * rc.Height();
    if (sz > ep->sz)
    {
        ep->sz = sz;
        ep->hFore = hwnd;
    }
    return true;
}

TMBool _CDlgStressTesting_MakeFront(TMBool bMax = false)
{
    HWND hCur = GetForegroundWindow();
    if (_CDlgStressTesting_IsAppWindow(hCur))
        return true;
    
    ENUMPARAM ep = {0};
    _tuiApp.EnumDialogs(_EnumDlg, (TMLParam)&ep);
    if (!ep.hFore)
        return false;
    hCur = CTuiWidget(ep.hFore).GetHWND();
    ::ShowWindow(hCur, SW_SHOWNORMAL);
    ::BringWindowToTop(hCur);
    ::SetForegroundWindow(hCur);
    if (bMax)
    {
        ::ShowWindow(hCur, SW_SHOWMAXIMIZED);
        RECT rc;
        ::GetWindowRect(hCur, &rc);
        ::MoveWindow(hCur, rc.left, rc.top, rc.right - rc.left - 1, rc.bottom - rc.top, true);
    }
    return false;
}

CString Exec(LPTSTR pCmd)
{
    TM::CString str;
    
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    HANDLE hRead, hWrite;
    if (!CreatePipe(&hRead, &hWrite, &sa, 0))
        return str;
    
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdError = hWrite;
    si.hStdOutput = hWrite;
    
    PROCESS_INFORMATION pi;
    if (!CreateProcess(NULL, pCmd, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
        return str;
    
    CloseHandle(hWrite);
    
    char buff[1024] = { 0 };
    DWORD dwRead = 0;
    while (ReadFile(hRead, buff, 1024, &dwRead, NULL))
    {
        //strRet.append(buff, dwRead);
        str.Append(buff, dwRead);
    }
    CloseHandle(hRead);
    
    return str;
}

///////////////////////////////////////////////////////////////////////////////
// CTestThread

class CThreadTest
{
private:
    CThreadID       m_thread1;
    CThreadID       m_thread2;
    CPoint          m_ptOld;
    int             m_tmPause;

public:
    volatile HWND   m_hOwner;

    TMBool          m_bTestMouseL;
    TMBool          m_bTestMouseR;
    TMBool          m_bTestMouseMove;

    TMBool          m_bTestMouseWheel;
    TMBool          m_bTestKey;
    TMBool          m_bTestThread;
//    TMBool          m_bTestDisplayMode;
    TMBool          m_bTestDPI;
    TCHAR           m_chNet[1024];

    static TM_DECALRE_THREAD_PROC (_ThreadProc1)
    {
        CThreadTest* p = (CThreadTest*) (pArg);
        return (TM_THREAD_PROC_RET) p->ThreadProcMouse ();
    }
    
    static TM_DECALRE_THREAD_PROC (_ThreadProc2)
    {
        CThreadTest* p = (CThreadTest*) (pArg);
        return (TM_THREAD_PROC_RET) p->ThreadProcOther ();
    }
    
    unsigned ThreadProcMouse();
    unsigned ThreadProcOther();

    CString DbgInfoFrom(HWND hWnd)
    {
        CString str;
        if (hWnd)
        {
            TCHAR c[1024] = {0};
            TCHAR d[1024] = {0};
            GetWindowText(hWnd, c, 1024);
            GetClassName(hWnd, c, 1024);
            str.Format(L"hWnd:%p(%s - %s)", hWnd, c, d);
        }
        else
            str = L"hWnd:0";
        return str;
    }
    
    CString DbgInfoFrom(CPoint pt)
    {
        HWND hWnd = WindowFromPoint(pt);
        return DbgInfoFrom(hWnd);
    }
    
    CString DbgInfoFrom()
    {
        CPoint pt;
        GetCursorPos(&pt);
        return DbgInfoFrom(pt);
    }

    int IsAppWin(HWND hWnd)
    {
        DWORD_PTR lr = 0;
        ::SendMessageTimeout(m_hOwner, WM_ISAPPWIN, 0, (TMLParam)hWnd, SMTO_ABORTIFHUNG, 100, &lr);
        return lr;
    }
    
    TMBool MakeFront(TMBool bMax = false)
    {
        DWORD_PTR lr = 0;
        ::SendMessageTimeout(m_hOwner, WM_MAKEFRONT, 0, bMax, SMTO_ABORTIFHUNG, 100, &lr);
        return lr;
    }

    TMBool IsUserMoveCursor()
    {
        return m_tmPause;
    }

    TMBool IsUserMoveCursor(int decTime)
    {
        CPoint pt;
        GetCursorPos(&pt);
        TMBool b = m_ptOld != pt;
        if (b)
            m_tmPause = 2 * 1000;
        m_ptOld = pt;
        m_tmPause += decTime;
        tm_setmax(m_tmPause, 0);
        return IsUserMoveCursor();
    }

    void MoveCursor(CPoint pt)
    {
        SetCursorPos(pt.x, pt.y);
        m_ptOld = pt;
    }

    CPoint GetRadomPoint(TMBool bCurrentProcess, TMBool bGetCaption = false, CPoint ptCenter = CPoint(0,0), int dx = 30000)
    {
        for (int i = 0; i < 1000; ++i)
        {
            CPoint pt(rand() % dx - dx / 2, rand() % dx - dx / 2);
            pt += ptCenter;
            HWND hWnd = WindowFromPoint(pt);
            if (hWnd)
            {
                if (!bCurrentProcess)
                    return pt;
                if (IsAppWin(hWnd))
                {
                    if (bGetCaption)
                    {
                        RECT rcx;
                        ::GetWindowRect(hWnd, &rcx);
                        pt.y = rcx.top + 15;
                        if (WindowFromPoint(pt) != hWnd)
                            continue;
                    }
                    return pt;
                }
            }
        }
        
        MakeFront(true);
        return CPoint(0,0);
    }

    void VirtualMouseClick(CPoint pt, TMBool bDoubleClick, TMBool bLeft)
    {
        if (pt.x == 0 && pt.y == 0)
            return;
        
        STEST_TRACE(L"CDlgStressTesting::VirtualMouseClick(pt:%d,%d; bDoubleClick:%d, bLeft:%d; %s)",
            pt, bDoubleClick, bLeft, DbgInfoFrom(pt).c_str());
        
        MoveCursor(pt);
        DWORD dwFlags1 = bLeft ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN;
        DWORD dwFlags2 = bLeft ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP;
        
        mouse_event(dwFlags1 | dwFlags2, 0, 0, 0, 0);
        if (bDoubleClick)
            mouse_event(dwFlags1 | dwFlags2, 0, 0, 0, 0);
    }
    
    void VirtualMouseDrag(CPoint ptFrom, CPoint ptTo, TMBool bLeft)
    {
        if (ptFrom.x == 0 && ptFrom.y == 0)
            return;
        
        STEST_TRACE(L"CDlgStressTesting::VirtualMouseDrag(ptFrom:%d,%d; ptTo:%d,%d; bLeft:%d; %s)",
            ptFrom, ptTo, bLeft, DbgInfoFrom(ptFrom).c_str());
        
        MoveCursor(ptFrom);
        DWORD dwFlags1 = bLeft ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN;
        DWORD dwFlags2 = bLeft ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP;
        
        mouse_event(dwFlags1, 0, 0, 0, 0);
        
        enum { eCount = 20 };
        for (int i = 1; (i <= eCount) && m_hOwner; ++i, Sleep(5))
        {
            int l = (ptTo.x - ptFrom.x) * i / eCount + ptFrom.x;
            int t = (ptTo.y - ptFrom.y) * i / eCount + ptFrom.y;
            if (IsUserMoveCursor(-5))
                break;
            MoveCursor(CPoint(l,t));
        }
        mouse_event(dwFlags2, 0, 0, 0, 0);
    }

    typedef CVector<CRect> CRectList;
    static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
    {
        CRectList* p = (CRectList*)dwData;
        p->push_back(*lprcMonitor);
        return true;
    }

    void VirtualMouseScreen()
    {
        CRectList aRect;
        EnumDisplayMonitors(0, 0, MonitorEnumProc, (LPARAM)&aRect);
        int nCount = aRect.size();
        if (nCount == 1)
            return;
        int nNow = rand() % nCount;
        CPoint pt = GetRadomPoint(true, true);
        CRect rc(aRect[nNow]);
        CPoint pt2((rc.left + rc.right) / 2, rc.top + 15);
        VirtualMouseDrag(pt, pt2, true);
    }

    void EnableNet()
    {
        if (m_chNet[0])
        {
            CString str;
            str.Format(L"netsh interface set interface name=\"%s\" admin=enabled", m_chNet);
            CString strret(Exec(str.GetBuffer()));
            STEST_TRACE(L"CDlgStressTesting::Reconnect Net(%s)", m_chNet);
        }
    }
    
    TMBool Close()
    {
        TMBool b = (0 != m_hOwner);
        m_hOwner = 0;
        m_thread1.Join();
        m_thread2.Join();
        return b;
    }

    void Begin()
    {
        m_tmPause = 0;
        m_thread1.CreateBy(&_ThreadProc1, this);
        m_thread2.CreateBy(&_ThreadProc2, this);
    }
};

unsigned CThreadTest::ThreadProcMouse()
{
    srand(GetTickCount());

    for (; m_hOwner; Sleep(10))
    {
        if (IsUserMoveCursor(-10))
            continue;

        if (CKeyboard::IsKeyDown(VK_PAUSE))
        {
            m_hOwner = 0;
            continue;
        }
        
        if (!MakeFront())
            continue;
        
        int n = rand() % 1000;
        DbgTrace ("CDlgStressTesting::OnAni(tupe:%d)", n);
        
        if (n >= 0 && n < 200) // L button [double] click
        {
            if (m_bTestMouseL)
            {
                CPoint pt = GetRadomPoint(true);
                VirtualMouseClick(pt, n < 50, true);
            }
        }
        else if (n >= 200 && n < 400) // R button [double] click
        {
            if (m_bTestMouseR)
            {
                CPoint pt = GetRadomPoint(true);
                VirtualMouseClick(pt, n < 250, false);
            }
        }
        else if (n >= 400 && n < 500) // L/R button drag
        {
            if (m_bTestMouseMove)
            {
                CPoint pt = GetRadomPoint(true);
                CPoint pt2 = GetRadomPoint(false, false, pt, 200);
                VirtualMouseDrag(pt, pt2, n < 480);
            }
        }
        else if (n >= 500 && n < 510)
        {
            if (m_bTestMouseMove)
                VirtualMouseScreen();
        }
    }
    return 0;
}

unsigned CThreadTest::ThreadProcOther()
{
    srand(GetTickCount());

    for (; m_hOwner; Sleep(10))
    {
        if (IsUserMoveCursor())
            continue;

        int n = rand() % 1000;
        DbgTrace ("CDlgStressTesting::OnAni(tupe:%d)", n);
        
        if (n < 600)
            continue;

        if (n >= 600 && n < 700) // mouse wheel
        {
            if (m_bTestMouseWheel)
            {
                int nWheel = rand() % 2 ? WHEEL_DELTA : -WHEEL_DELTA;
                STEST_TRACE(L"CDlgStressTesting::VirtualMouseWheel(nWheel:%d, %s)", nWheel, DbgInfoFrom().c_str());
                mouse_event(MOUSEEVENTF_WHEEL, 0, 0, nWheel, 0);
            }
        }
        else if (n >= 700 && n < 800) // key input
        {
            if (m_bTestKey) // && !HaveKeyMessage())
            {
                HWND hWnd = GetFocus();
                if (!IsAppWin(hWnd))
                    continue;
                int n2 = rand() % 512;
                if (n2 > 255)
                    n2 = VK_TAB;
                int nShift = rand() % 5;
                int nCtrl = rand() % 5;
                int nAlt = rand() % 20;
                if (nShift == 1)
                    keybd_event(VK_SHIFT, 0, 0, 0);
                if (nCtrl == 1)
                    keybd_event(VK_CONTROL, 0, 0, 0);
                if (nAlt == 1)
                    keybd_event(VK_MENU, 0, 0, 0);
                keybd_event(n2, 0, 0, 0);
                keybd_event(n2, 0, KEYEVENTF_KEYUP, 0);
                if (nAlt == 1)
                    keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
                if (nCtrl == 1)
                    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
                if (nShift == 1)
                    keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
                STEST_TRACE(L"CDlgStressTesting::VirtualKeyDown(%d, %s)", n, DbgInfoFrom(hWnd).c_str());
            }
        }
        else if (n >= 800 && n < 900) // suspend thread
        {
            if (m_bTestThread)
            {
                CTIDList tList;
                ListThreads(GetCurrentProcessId(), tList);
                DbgAssert(tList.size());
                int n2 = rand() % tList.size();
                DWORD tid = tList[n2];
                if (tid != GetCurrentThreadId())
                {
                    HANDLE hThread = _OpenThread(0x02, false, tid);
                    if (!hThread)
                        continue;

                    int nMS = rand() % 10;
                    STEST_TRACE(L"CDlgStressTesting::SuspendThread(tid:%d, time:%dMS)", tid, nMS);
                    CClock cyc;
                    ::SuspendThread(hThread);
                    Sleep(nMS);
                    ::ResumeThread(hThread);
                    CloseHandle(hThread);
                }
            }
        }
        else if (n >= 900 && n < 905) // block network
        {
            if (m_chNet[0])
            {
                CString str;
                str.Format(L"netsh interface set interface name=\"%s\" admin=disabled", m_chNet);
                CString strret(Exec(str.GetBuffer()));
                STEST_TRACE(L"CDlgStressTesting::Disconnect Net(%s)", m_chNet);
            }
        }
        else if (n >= 905 && n < 1000)
            EnableNet();
    }

    EnableNet();

    return 0;
}

static CThreadTest s_threadTest;

///////////////////////////////////////////////////////////////////////////////
// CDlgStressTesting
CDlgStressTesting::CDlgStressTesting()
{
//     m_c[0] = 0;
}

CDlgStressTesting::~CDlgStressTesting()
{
}

TMResult CDlgStressTesting::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    TCHAR c[1024] = L"netsh interface show interface";
    CString str(Exec(c));
    CStrPtr strret;
    int nLine = 0;
    CTuiListBox lb(Item(IDC_LISTBOX_31232));
    for (size_t x = 0; 0 != (x = str.SplitLine(x, strret)); ++nLine)
    {
        if (nLine < 3)
            continue;
        strret = strret.Mid(40);
        strret = strret.Trim();
        if (!strret.IsEmpty())
        {
            CString strx(strret);
            lb.AddString(strx);
        }
    }
    lb.SetCurSel(0);

//     _tuiApp.AddMessageFilter(this);
    return 0;
}

TMResult CDlgStressTesting::OnIsAppWin (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    return _CDlgStressTesting_IsAppWindow((HWND)lParam);
}

TMResult CDlgStressTesting::OnMakeFront (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    return _CDlgStressTesting_MakeFront(lParam);
}

TMResult CDlgStressTesting::OnDestroy (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
    s_threadTest.Close();

//     _tuiApp.RemoveMessageFilter(this);
    return 0;
}

TMResult CDlgStressTesting::OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TMBool b = s_threadTest.Close();
    if (!b)
    {
        s_threadTest.m_hOwner = GetHWND();

        s_threadTest.m_bTestMouseL = CTuiButton(Item(IDC_CHECKBOX_31220)).GetCheck();
        s_threadTest.m_bTestMouseR = CTuiButton(Item(IDC_CHECKBOX_31221)).GetCheck();
        s_threadTest.m_bTestMouseMove = CTuiButton(Item(IDC_CHECKBOX_31222)).GetCheck();
        s_threadTest.m_bTestMouseWheel = CTuiButton(Item(IDC_CHECKBOX_31223)).GetCheck();
        s_threadTest.m_bTestKey = CTuiButton(Item(IDC_CHECKBOX_31224)).GetCheck();
        s_threadTest.m_bTestThread = CTuiButton(Item(IDC_CHECKBOX_31225)).GetCheck();
        TMBool bTestNet = CTuiButton(Item(IDC_CHECKBOX_31226)).GetCheck();
        if (bTestNet)
        {
            CTuiListBox lb(Item(IDC_LISTBOX_31232));
            int n = lb.GetCurSel();
            int x = lb.GetText(n, s_threadTest.m_chNet);
            s_threadTest.m_chNet[x] = 0;
        }
        else
            s_threadTest.m_chNet[0] = 0;

        s_threadTest.Begin();
    }
//    EndDialog(wID);
//     DestroyWindow();
    return 0;
}

TMResult CDlgStressTesting::OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

//CTuiMsgFilter
// BOOL CDlgStressTesting::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }




// static TMBool s_bMouse;

// void VirtualMouseClick(CPoint pt, TMBool bDoubleClick, TMBool bLeft)
// {
//     if (pt.x == 0 && pt.y == 0)
//         return;
// 
//     s_bMouse = true;
//     STEST_TRACE(L"CDlgStressTesting::VirtualMouseClick(pt:%d,%d; bDoubleClick:%d, bLeft:%d; %s)",
//         pt, bDoubleClick, bLeft, DbgInfoFrom(pt).c_str());
// 
//     SetCursorPos(pt.x, pt.y);
//     DWORD dwFlags1 = bLeft ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN;
//     DWORD dwFlags2 = bLeft ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP;
// 
//     mouse_event(dwFlags1 | dwFlags2, 0, 0, 0, 0);
//     if (bDoubleClick)
//         mouse_event(dwFlags1 | dwFlags2, 0, 0, 0, 0);
//     s_bMouse = false;
// }
// 
// TMBool Loop()
// {
//     MSG msg;
//     while(::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
//     {
//         if (msg.message == WM_QUIT)
//             return false;
//         ::TranslateMessage(&msg);
//         ::DispatchMessage(&msg);
//     }
//     return true;
// }
// 
// void VirtualMouseDrag(CPoint ptFrom, CPoint ptTo, TMBool bLeft)
// {
//     if (ptFrom.x == 0 && ptFrom.y == 0)
//         return;
// 
//     s_bMouse = true;
//     STEST_TRACE(L"CDlgStressTesting::VirtualMouseDrag(ptFrom:%d,%d; ptTo:%d,%d; bLeft:%d; %s)",
//         ptFrom, ptTo, bLeft, DbgInfoFrom(ptFrom).c_str());
// 
//     SetCursorPos(ptFrom.x, ptFrom.y);
//     DWORD dwFlags1 = bLeft ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN;
//     DWORD dwFlags2 = bLeft ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP;
// 
//     mouse_event(dwFlags1, 0, 0, 0, 0);
// 
//     for (int i = 0; i < 20; ++i)
//     {
//         if (!Loop())
//             return;
//         int l = (ptTo.x - ptFrom.x) / 100 + ptFrom.x;
//         int t = (ptTo.y - ptFrom.y) / 100 + ptFrom.y;
//         SetCursorPos(ptFrom.x, ptFrom.y);
//     }
//     mouse_event(dwFlags2, 0, 0, 0, 0);
//     s_bMouse = false;
// }




// TMBool HaveMessage(int nBgn, int nEnd)
// {
//     MSG msg;
//     return PeekMessage(&msg, 0, nBgn, nEnd, PM_NOREMOVE);
// }
// 
// TMBool HaveMouseMessage()
// {
//     return s_bMouse;
// //    return HaveMessage(WM_MOUSEFIRST, WM_MOUSELAST);
// }
// 
// TMBool HaveKeyMessage()
// {
//     return HaveMessage(WM_KEYFIRST, WM_KEYLAST);
// }

void ListThreads(DWORD pid, CTIDList& aTid)
{
    HANDLE hThreads = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pid);
    if (hThreads == INVALID_HANDLE_VALUE)
        return;
    
    THREADENTRY32 te32 = { sizeof(THREADENTRY32) };
    BOOL anyMore = Thread32First(hThreads, &te32);
    while (anyMore)
    {
        if (te32.th32OwnerProcessID == pid)
            aTid.push_back(te32.th32ThreadID);
        anyMore = Thread32Next(hThreads, &te32);
    }
    CloseHandle(hThreads);
}

