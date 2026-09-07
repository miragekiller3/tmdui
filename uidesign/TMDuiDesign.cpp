/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2006.09.29
 *
 *  Copyright (C) 2006 miragekiller
 *  All rights reserved.
 */

#include "stdafx.h"
#include <atlbase.h>
#include "tmduiatl.h"


CComModule _Module;

using namespace TM;


int APIENTRY WinMain(TMHModule hInstance,
                     TMHModule hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
//     InitCommonControls();

    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    _Module.Init (NULL, hInstance);

//    TM::CTuiZoom::SetSystemZoom();
//     _tuiApp.SetUseSystemFrame(true);

//          TM::CAppAT::SetDPIAware();

    tuiPlugInCreate(false);//, ePlugInModeEditor);

    OleUninitialize();
    
    _tuiApp.Term();
    _Module.Term();
	return 0;
}



