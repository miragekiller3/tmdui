/**
 *  @file
 *  @brief  CLASS
 *
 *
 *  @author YourUserName <name@Email.com>
 *  @date   2026-4-3
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2026 miragekiller                             
 *  All rights reserved  
 */

#include "stdafx.h"
//#include "resource.h"
#include "tuires.h"
#include "DlgPlayHz.h"

// #include "tmdui.cpp"


CComModule _Module;

// BEGIN_OBJECT_MAP(ObjectMap)
// END_OBJECT_MAP()


#ifdef TM_QT

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    _Module.Init(NULL, DllHandle(0));
    _tuiApp.Init(0);
    _tuiApp.LoadXml(_Module.GetResourceInstance(), TM_T(":/tui/updown") TUI_RES_XML_NAME);

    CDlgPlayHz dlg;
    dlg.Create((TMHWnd)0);
    int nret = a.exec();

    _tuiApp.Term();
    _Module.Term();
    return nret;
}

#else 

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

int APIENTRY WinMain(TMHModule hInstance, TMHModule hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    OleInitialize(0);
    
//     INITCOMMONCONTROLSEX icex;
//     icex.dwSize = sizeof(icex);
//     icex.dwICC  = ICC_WIN95_CLASSES
// //         | ICC_BAR_CLASSES | ICC_COOL_CLASSES | ICC_DATE_CLASSES 
// //         | ICC_HOTKEY_CLASS | ICC_INTERNET_CLASSES | ICC_LISTVIEW_CLASSES
// //         | ICC_PAGESCROLLER_CLASS | ICC_PROGRESS_CLASS | ICC_TAB_CLASSES
// //         | ICC_TREEVIEW_CLASSES | ICC_UPDOWN_CLASS | ICC_USEREX_CLASSES
// //         | ICC_ANIMATE_CLASS
//         ; 
//     InitCommonControlsEx(&icex);

    _Module.Init (NULL, hInstance);
//     _Module.m_hInstResource = LoadLibrary(TM_T("res.dll"));

    _tuiApp.Init(0);
    _tuiApp.LoadXml(_Module.GetResourceInstance(), TUI_RES_XML_NAME);

//     _tuiApp.SetAnimationButton(true);
//     _tuiApp.SetAnimationScroll(true);
//     _tuiApp.SetAnimationSysBut(true);
    
//     TM::CAppAT::SetDPIAware();

    CDlgPlayHz dlg;
    dlg.DoModal(0);
    
//     MSG msg;
//     while(GetMessage(&msg,NULL,0,0))
//     {
//         TranslateMessage (&msg);
//         DispatchMessage (&msg) ;
//     }

    _tuiApp.Term();
    _Module.Term();
    OleUninitialize();
    
    return 0;
}

#endif // TM_QT

