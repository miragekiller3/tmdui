/**
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.08.25
 *
 *  Copyright (C) 2009-2018 miragekiller
 */

#include "stdafx.h"

#include "Proxy.h"
#include "accMgr.h"
//#include "Speak.h"

CComModule _Module;
bool s_bInLresultFromObject;


#ifdef _USRDLL
TMBool APIENTRY DllMain(TMHModule hModule,
                       TMULong  ul_reason_for_call,
                       LPVOID lpReserved
                    )
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            {
                ACC_TRACE("TMDuiAcc::DllMain(DLL_PROCESS_ATTACH)");
                 // TODO: Place code here.
                TMResult hRes = _Module.Init(NULL, hModule);
                DisableThreadLibraryCalls(hModule);
            }
            break;

        case DLL_PROCESS_DETACH:
            {
                ACC_TRACE("TMDuiAcc::DllMain(DLL_PROCESS_DETACH)");
                _Module.Term();                
            }
            break;
    }
    return TMTrue;
}
#endif //with _USRDLL

TMBool GetAccObject(TM::HTUI hWgt, TMWParam wParam, TMLParam lParam, TMResult& lr)
{
    DbgAssert(hWgt);
   
    ACC_TRACE(L"GetAccObject(%s, wParam:%X, lParam:%s)", _acc_dinfo2(hWgt).c_str(), wParam, DbgFormatObjId(lParam));

    IAccessible *pIAccessible = 0;
    AccHash32 hash = (AccHash32)lParam;

    switch (hash)
    {
    case (AccHash32)OBJID_WINDOW:
        return false;

    case (AccHash32)OBJID_CLIENT:
        pIAccessible = CAccessibleObj::MakeAccClient(hWgt);
        break;

//    case (AccHash32)OBJID_SOUND:
//        pIAccessible = &g_accSpeak;
//        break;

//     case (TMLParam)OBJID_MENU:
//         return false;
//         hWgt = TM::CTuiWindow(TM::CTuiWidget(hWgt).GetWindow(GW_OWNER)).GetMenuBar();
//         pIAccessible = CAccessibleObj::MakeAccClient(hWgt);
//         break;
// 
//     case (TMLParam)OBJID_CARET:
//         {
//             CAccessibleWrapper* px = CAccessibleWrapper::GetAccProxy(hWgt);
//             pIAccessible = px ? px->GetObjCaret() : 0;
//         }
//         break;
//     
//     case (TMLParam)OBJID_HSCROLL:
//         return false;
//         {
//             CAccessibleWrapper* px = CAccessibleWrapper::GetAccProxy(hWgt);
//             pIAccessible = px ? px->GetObjHScrollBar() : 0;
//         }
//         break;
// 
//     case (TMLParam)OBJID_VSCROLL:
//         return false;
//         {
//             CAccessibleWrapper* px = CAccessibleWrapper::GetAccProxy(hWgt);
//             pIAccessible = px ? px->GetObjVScrollBar() : 0;
//         }
//         break;

    default:
        if (hash >= (AccHash32)OBJID_UiaRootObjectId && hash <= (AccHash32)OBJID_SYSMENU)
            return false;
        pIAccessible = CAccessibleObj::GetAccProxyBy(hash);
        break;
    }
    
    if (pIAccessible)
    {
        s_bInLresultFromObject = true;
        lr = LresultFromObject(IID_IAccessible, wParam, pIAccessible);
        s_bInLresultFromObject = false;
        pIAccessible->Release();

//         HRESULT lr3 = LresultFromObject(IID_IAccessible, wParam, pIAccessible);
// 
//         CComPtr<IAccessible> ptrx;
//         HRESULT lr1 = ObjectFromLresult(lr3, IID_IAccessible, 0, (void**)&ptrx);
//         CComPtr<IAccessible> ptrxx;
//         HRESULT lr2 = ObjectFromLresult(lr, IID_IAccessible, 0, (void**)&ptrxx);
//         ptrx.Release();
    }

    return true;
}

