/**
 *    @file
 *    @brief
 *    CLASS
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
//#include "resource.h"
#include "tuires.h"

#include "DlgMain.h"
#include "DlgPop.h"
//#include "DlgPopDlg.h"




CComModule _Module;

// BEGIN_OBJECT_MAP(ObjectMap)
// END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

int APIENTRY WinMain(TMHModule hInstance, TMHModule hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    OleInitialize(0);
    
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(icex);
    icex.dwICC  = ICC_WIN95_CLASSES
//         | ICC_BAR_CLASSES | ICC_COOL_CLASSES | ICC_DATE_CLASSES 
//         | ICC_HOTKEY_CLASS | ICC_INTERNET_CLASSES | ICC_LISTVIEW_CLASSES
//         | ICC_PAGESCROLLER_CLASS | ICC_PROGRESS_CLASS | ICC_TAB_CLASSES
//         | ICC_TREEVIEW_CLASSES | ICC_UPDOWN_CLASS | ICC_USEREX_CLASSES
//         | ICC_ANIMATE_CLASS
        ; 
    InitCommonControlsEx(&icex);

    _Module.Init (NULL, hInstance);
//     _Module.m_hInstResource = LoadLibrary(TM_T("res.dll"));

    _tuiApp.Init(0);
    _tuiApp.LoadXml(_Module.GetResourceInstance(), TUI_RES_XML_NAME);
    
    CDlgMain::RegisterAutoCreate();
    CDlgPop::RegisterAutoCreate();
    CDlgPop::RegisterAutoCreate(IDD_MANAGE_POP_DLG);
//    CDlgPopDlg::RegisterAutoCreate();

    _tuiApp.DlgBox(_Module.GetResourceInstance(), IDD_TUI_MAIN, 0);
    
    OleUninitialize();
    _tuiApp.Term();
    _Module.Term();

    
    return 0;
}



