//-----------------------------------------------------------
//Sample.cpp

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
#include "IEMainDlg.h"

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

int APIENTRY WinMain(TMHModule hInstance,
                     TMHModule hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    OleInitialize(0);

    TM::CAppAT::SetDPIAware();

    _Module.Init(ObjectMap, hInstance);
    _tuiApp.Init(0);
    _tuiApp.SetWebview2SSOUsingOSA(true);
//    _tuiApp.SetWebview2Arguments(_T("-autoplay-policy=no-user-gesture-required"));
    _tuiApp.LoadXml(_Module.m_hInst, _T("tuires.XML"));

//     SetR2L(hInstance, true);

//     _tuiApp.SetUseSystemFrame(true);


    //CIEMainDlg m;
    //m.DoModal(0);
    CIEMainDlg::CPtr ptr;
    ptr.CreateInstance()->DoModal(0);

    
    _tuiApp.Term();
    _Module.Term();
    return 0;
}



