// dui.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f duips.mk in the project directory.

//#include "stdafx.h"
//#include "resource.h"

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers

#if _MSC_VER < 1300 // vc6:120X
#   define WINVER       0x050a
#   define _WIN32_WINNT 0x0500
#   define _WIN32_IE    0x0600
#else
#   define WINVER       0x050a
#   define _WIN32_WINNT 0x0501 // 0x0501:xp; 0x0600:vista; 0x601:win7;
#   define _WIN32_IE    0x0600
#endif

#include "tmdef.h"
#include "tmstring.h"
#include "tmfilename.h"
#include "tmcfg.h"


#include <atlbase.h>

CComModule _Module;


#include <atlapp.h>
#include <atlcom.h>
#include <atlwin.h>

#include "tmdui.cpp"
#include "tm.cpp"
#include "gx.cpp"


/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(0, hInstance);
        DisableThreadLibraryCalls(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
        _Module.Term();
    return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

// STDAPI DllCanUnloadNow(void)
// {
//     return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
// }
// 
// /////////////////////////////////////////////////////////////////////////////
// // Returns a class factory to create an object of the requested type
// 
// STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
// {
//     return _Module.GetClassObject(rclsid, riid, ppv);
// }
// 
// /////////////////////////////////////////////////////////////////////////////
// // DllRegisterServer - Adds entries to the system registry
// 
// STDAPI DllRegisterServer(void)
// {
//     // registers object, typelib and all interfaces in typelib
//     return _Module.RegisterServer(TRUE);
// }
// 
// /////////////////////////////////////////////////////////////////////////////
// // DllUnregisterServer - Removes entries from the system registry
// 
// STDAPI DllUnregisterServer(void)
// {
//     return _Module.UnregisterServer(TRUE);
// }


