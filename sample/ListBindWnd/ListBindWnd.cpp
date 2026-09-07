/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2009.02.04
 *
 *  Copyright (C) 2009 miragekiller
 */

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

    OleInitialize(0);

//     TM::CCallbackList<0>::Register(hInstance);

    CMain m;
    m.DoModal (0);
//     CMain::DoModal (0);
    
    _tuiApp.Term();
    _Module.Term();
    return 0;
}



