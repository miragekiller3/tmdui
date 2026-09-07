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
#include "DlgTooltip2.h"

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

int APIENTRY WinMain(TMHModule hInstance,
                     TMHModule hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
//     for (int n = INT_MIN; n <= INT_MAX; ++n)
//     {
//         for (int m = -2; m < INT_MAX; ++m)
//         {
//             if (n == 0 || m == 0)
//                 continue;
//             int x = n * m;
//             if (x == 0)
//             {
//                 int z = 0;
//             }
//         }
//     }


//     CWindowDC dc(0);
//     RECT rc;
//     dc.GetClipBox(&rc);

    _Module.Init(ObjectMap, hInstance);
    _tuiApp.Init (0);
    _tuiApp.LoadXml (_Module.m_hInst, _T("tuires.XML"));
//    _tuiApp.SetZoom(144);

    OleInitialize(0);

//     TM::CCallbackList<0>::Register(hInstance);

    CDlgTooltip2::RegisterAutoCreate();

    CMain m;
    m.DoModal (0);
//     CMain::DoModal (0);
    
    _tuiApp.Term();
    _Module.Term();
    return 0;
}



