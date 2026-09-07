/**
 *    @file
 *    @brief
 *    CDlgMain
 *
 *
 *    @author 
 *    @date    2014-4-14
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2014 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "DlgMain.h"

using namespace TM;



CDlgMain::CDlgMain()
{
}

CDlgMain::~CDlgMain()
{
}

TMResult CDlgMain::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{

    HWND hPop = CreateWindow(L"edit", L"edit", WS_TABSTOP | WS_CHILD | WS_VISIBLE, 20, 20, 80, 20, GetHWND(), 0, 0, 0);
    return 0;
}

TMResult CDlgMain::OnTest1 (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
//     CDlgMain::CPtr ptr;
//     ptr.CreateByObjectID(m_hWnd, 0);
    return 0;
}

TMResult CDlgMain::OnOk (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    EndDialog(wID);
    return 0;
}

TMResult CDlgMain::OnCancel (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    EndDialog(wID);
    return 0;
}

struct ProcessWindow
{
    DWORD dwTId;
    HWND hwndWindow;
};

static BOOL CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam)
{
    ProcessWindow *pProcessWindow = (ProcessWindow *)lParam;
    
    DWORD dwTId = GetWindowThreadProcessId(hWnd, 0);
    
    if (pProcessWindow->dwTId == dwTId && IsWindowVisible(hWnd) && GetParent(hWnd) == NULL)
    {
        pProcessWindow->hwndWindow = hWnd;
        
        return FALSE;
    }
    
    return TRUE;
}

static HWND CreateNotepad()
{
    PROCESS_INFORMATION pi;
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOW;
    
    TCHAR name[] = L"notepad.exe\0";
    if (CreateProcess(NULL, name, NULL, NULL, false, 0, NULL, NULL, &si, &pi))
    {
        ProcessWindow procwin;
        procwin.dwTId = pi.dwThreadId;
        procwin.hwndWindow = NULL;
        
        WaitForInputIdle(pi.hProcess, 5000);
        
        EnumWindows(EnumWindowCallBack, (LPARAM)&procwin);
        
        return procwin.hwndWindow;
    }
    return 0;
}

TMResult CDlgMain::OnEmbed1 (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    HWND hWnd = CreateNotepad();
    ::SetParent(GetHWND(), hWnd);
    return 0;
}

TMResult CDlgMain::OnEmbed2 (WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    HWND hWnd = CreateNotepad();
    ::SetParent(hWnd, GetHWND());
    ::MoveWindow(hWnd, 300, 250, 300, 200, true);
    return 0;
}


TMResult CDlgMain::OnDestroy2(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    TM::HTUI hWnd = 0;
    _tuiApp.GetMenuPopInfo(1, 0, &hWnd);
    if (hWnd)
        CTuiWidget(hWnd).DestroyWindow();
    return 0;
}

TMResult CDlgMain::OnDestroy30(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    TM::HTUI hWnd = 0;
    _tuiApp.GetMenuPopInfo(2, &hWnd, 0);
    if (hWnd)
        CTuiWidget(hWnd).DestroyWindow();
    return 0;
}
