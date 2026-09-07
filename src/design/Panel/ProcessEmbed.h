/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2019.2.22
 *
 *  Copyright (C) 2019 miragekiller
 */

#ifndef PROCESSEMBED_H
#define PROCESSEMBED_H


#ifndef TMDEF_H
#   include "tmdef.h"
#endif


struct CProcessEmbed
{
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

    static void Embed1(HWND hWndWgt)
    {
        static HWND hWnd = 0;
        if (hWnd)
            hWnd = 0;
        else
            hWnd = CreateNotepad();
        ::SetParent(hWndWgt, hWnd);
    }

    static void Embed2(HWND hWndWgt)
    {
        HWND hWnd = CreateNotepad();
        ::SetParent(hWnd, hWndWgt);
        ::MoveWindow(hWnd, 10, 10, 300, 200, true);
    }
};



#endif //PROCESSEMBED_H
