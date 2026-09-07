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
    _tuiApp.Init(0);
    _tuiApp.LoadXml(_Module.m_hInst, _T("tuires.XML"));

    OleInitialize(0);

    TM::CAppAT::SetDPIAware();// at_app_dpi_aware_set();

    CMain m;
    m.DoModal (0);
    
    _tuiApp.UnLoadXml(_Module.m_hInst);
    _Module.Term();
    _tuiApp.Term();
    return 0;
}



