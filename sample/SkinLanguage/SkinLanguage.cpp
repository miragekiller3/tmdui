#include "StdAfx.h"
#include "skinlanguage.h"
#include "tmgdix.h"



CComModule _Module;

BEGIN_OBJECT_MAP(ObjMap)
END_OBJECT_MAP()

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    _Module.Init(ObjMap, hInstance);
    _tuiApp.Init(0);
    _tuiApp.LoadXml(_Module.m_hInst, L"tuires.xml", L"Skin2.xml");
    //_tuiApp.SetUseSystemFrame(TMTrue);

    _tuiApp.SetAnimationButton(true);
    _tuiApp.SetAnimationScroll(true);
    _tuiApp.SetAnimationSysBut(true);

    OleInitialize(0);
//     TM::CCallbackList<0>::Register(hInstance);

//     EGXDevice dev[] = { eGXDeviceGDI, eGXDeviceD3D9, eGXDeviceD3D11, eGXDeviceGL };
//     gxSetDefaultDevices(&(dev[2]), 1, true);
//     Sleep(1000 * 10);


    CWclDemoDlg dlg;
    dlg.DoModal(0);

    _tuiApp.Term();
    _Module.Term();
    return 0;
}