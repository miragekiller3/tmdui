/**
 *    @file
 *    @brief
 *    CLASS
 *
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2021-3-29
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2021 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
//#include "resource.h"
#include "tuires.h"
#include "DlgMenubar.h"



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
    _tuiApp.LoadXml(_Module.GetResourceInstance(), TM_T(":/tui/MenuBar") TUI_RES_XML_NAME);

    CDlgMenubar dlg;
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

    
//     at_app_dpi_aware_set();

    CDlgMenubar dlg;
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

