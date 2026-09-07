//-----------------------------------------------------------
//treeview.cpp
//
//  Copyright (C) 2009 miragekiller                               
//  All rights reserved                                                     
//
//
//  Author: 
//      miragekiller    <3916345933@qq.com>
//
//    Histroy :
//      2009.02.04      create
//-------------------------------------------------------------



#include "stdafx.h"

//#include "../../../tmdui.cpp"
#include "main.h"


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

int APIENTRY WinMain(TMHModule hInstance,
                     TMHModule hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    _Module.Init(ObjectMap, hInstance);
    _tuiApp.Init (0);
    _tuiApp.LoadXml (_Module.m_hInst, _T("tuires.XML"));
//    _tuiApp.SetUseSystemFrame(true);
    TM::CAppAT::SetDPIAware();

    OleInitialize(0);

    CMain m;
    m.DoModal (0);
    
    _tuiApp.Term();
    _Module.Term();
    return 0;
}



