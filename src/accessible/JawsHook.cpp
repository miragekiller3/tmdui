/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2024.5.15
 *
 *  Copyright (C) 2024 miragekiller
 */

#include "StdAfx.h"
#include "JawsHook.h"
#include "tmdll.h"


TM_BGN_NAMESPACE


///////////////////////////////////////////////////////////////////////////////
// IAT hook
template<class T2>
inline static DWORD_PTR MakePtr(void* __x, T2 __y) { return (DWORD_PTR)__x + (DWORD)__y; }

inline static BOOL WriteMem(LPVOID dest, LPVOID buf, size_t size)
{
//     OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
//     SetPrivilege(hToken, SE_DEBUG_NAME, TRUE);
//     CloseHandle(hToken);
//     
//     HANDLE pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
//     
//     DWORD h =  GetProcessBaseAddress(pID);
//     SetLastError(0);
//     DWORD oldProtect;
//     BOOL ret = VirtualProtectEx(pHandle, (LPVOID)h, writing_size, PAGE_READWRITE, &oldProtect);
//     //    VOID* pp = VirtualAllocEx(pHandle, (LPVOID)h, writing_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
//     DWORD error = GetLastError();
//     SetLastError(0);
//     ret = WriteProcessMemory(pHandle, (LPVOID)h, writing.c_str(), writing_size, 0);
//     error = GetLastError();
//     std::cout << "Error: " << error << std::endl;
//     CloseHandle(pHandle);

    HANDLE hProc = GetCurrentProcess();
    DWORD dwFlag = 0;
    BOOL b = VirtualProtectEx(hProc, dest, size, PAGE_READWRITE, &dwFlag);
    return WriteProcessMemory(GetCurrentProcess(), dest, buf, size, 0);
}

// get the import desriptor of the dll module which the captured functions locate in 
inline static PIMAGE_IMPORT_DESCRIPTOR GetImportDescriptor(HMODULE hModule, LPCSTR pProcModule)
{
    DbgAssert(hModule && pProcModule);
    
    // get Dos file header
    PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)hModule;
    
    // check if it is MZ file header 
    DbgAssert(!IsBadReadPtr(pDOSHeader, sizeof(IMAGE_DOS_HEADER)));
    DbgAssert(pDOSHeader->e_magic == IMAGE_DOS_SIGNATURE);
    
    //get PE file header  
    PIMAGE_NT_HEADERS pNTHeader = 
        (PIMAGE_NT_HEADERS)MakePtr(pDOSHeader, pDOSHeader->e_lfanew);
    
    // check if it is PE image file 
    DbgAssert(!IsBadReadPtr(pNTHeader, sizeof(IMAGE_NT_HEADERS)));
    DbgAssert(pNTHeader->Signature == IMAGE_NT_SIGNATURE);
    
    //check PE file's .data section
    DWORD dwVAddr = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    if ( dwVAddr == 0)
        return NULL;
    
    // get the pointer to .idata section 
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc = 
        (PIMAGE_IMPORT_DESCRIPTOR)MakePtr(pDOSHeader, dwVAddr);

    /*/ //get the pointer to .idata section 
    // because dbghlp.dll needed by mageDirectoryEntryToData does not exist on some platforms 
    // so the code above is for instead 
    ULONG    ulSize;
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc =
        ( PIMAGE_IMPORT_DESCRIPTOR ) ImageDirectoryEntryToData
        ( hModule, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize );
    /*/
    
    // enumerate PIMAGE_IMPORT_DESCRIPTOR array to find the specified module 
    for ( ; pImportDesc->Name; pImportDesc++ )
    {
        PSTR szCurrMod = (PSTR)MakePtr(pDOSHeader, pImportDesc->Name);
        if (lstrcmpiA(szCurrMod, pProcModule) == 0)
            return pImportDesc; // found
    }
    
    return 0;
}

// bind single module
static BOOL HookAssign(HMODULE hModule, LPCSTR pProcModule, PROC pOldProc, PROC pNewProc)
{
    DbgAssert(hModule && pProcModule && pOldProc && pNewProc);

// #ifdef AT_DBG
//     TCHAR c[MAX_PATH];
//     GetModuleFileName ( hModule, c, MAX_PATH );
// #endif
    
    // monitor if the current module is above the 2GB virtual memory space
    // this part of memory address is shared by Win32 processes in win95
//     if ( !IsNT() && ( ( DWORD ) hModule >= 0x80000000 ) )
//         return ATFalse;
    
    // call GetNamedImportDescriptor to get hModule --which is the 
    // import descriptor of the dll module which the captured functions locate in 
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc = GetImportDescriptor(hModule, pProcModule);
    if (!pImportDesc)
        return false; // module is not be imported in by current process
    
    // get the pointer to IMAGE_THUNK_DATA array from pImportDesc->FirstThunk,
    // because all import information has been filled in when dll is imported here
    // so the real capture occurs here 
    PIMAGE_THUNK_DATA pRealThunk =
        (PIMAGE_THUNK_DATA)MakePtr(hModule, pImportDesc->FirstThunk);

    PIMAGE_THUNK_DATA pOrigThunk =
        (PIMAGE_THUNK_DATA)MakePtr(hModule, pImportDesc->OriginalFirstThunk);

    
    // enumerate IMAGE_THUNK_DATA array to find the specified captured function
    while (pRealThunk->u1.Function)
    {
        PIMAGE_IMPORT_BY_NAME pByName = (PIMAGE_IMPORT_BY_NAME)MakePtr(hModule, pOrigThunk->u1.AddressOfData);
        if (pOldProc == (PROC)pRealThunk->u1.Function )
            return WriteMem(&(pRealThunk->u1.Function), &pNewProc, sizeof(pNewProc));

        // access the next element of MAGE_THUNK_DATA array 
        pRealThunk++;
        pOrigThunk++;
    }
    return false;
}

DECLARE_DLL_FUNC(dll_user32, int, WINAPI, GetClassNameW, (HWND hWnd, LPWSTR lpClassName, int nMaxCount));
DECLARE_DLL_FUNC(dll_user32, UINT, WINAPI, RealGetWindowClassW, (HWND hWnd, LPWSTR lpClassName, UINT nMaxCount));


//BOOL CALLBACK EnumChildProc(_In_ HWND hwnd, _In_ LPARAM lParam) //tianfu
//{
//    if (!::IsWindowVisible(hwnd))
//        return TRUE;
//
//    WCHAR clsName[MAX_PATH] = { 0 };
//    GetClassName(hwnd, clsName, MAX_PATH);
//    if (lstrcmpW(clsName, L"CefBrowserWindow") != 0)
//        return TRUE;
//
//    BOOL* bFind = (BOOL*)lParam;
//    *bFind = TRUE;
//    return FALSE;
//}
//
//BOOL FindCefBrowserWindow(HWND hWnd)
//{
//    BOOL bFind = FALSE;
//    EnumChildWindows(hWnd, EnumChildProc, (LPARAM)&bFind);
//    return bFind;
//}


BOOL g_bDisableJawsHook;

int WINAPI HookGetClassNameW(HWND hWnd, LPWSTR lpClassName, int nMaxCount)
{
    int n =  _GetClassNameW(hWnd, lpClassName, nMaxCount);
    if (!g_bDisableJawsHook &&
        (0 == lstrcmpi(TUI_CLASS_WIN_MAIN, lpClassName)))
    {
        ACC_TRACE(L"HookGetClassNameW(%p) %s" L"-> %s", hWnd, lpClassName, s_cClassDlg);
        lstrcpy(lpClassName, s_cClassDlg);
        n = tm_countof(s_cClassDlg) - 1;
    }
    return n;
}

//int WINAPI HookGetClassNameW(HWND hWnd, LPWSTR lpClassName, int nMaxCount)
//{
//    TCHAR d[1024];
//    int xx;
//    xx = GetWindowText(hWnd, d, 1024);
//
//    int n = _GetClassNameW(hWnd, lpClassName, nMaxCount);
//    if (0 == lstrcmpi(d, lpClassName))
//    {
//        ACC_TRACE(L"HookGetClassNameW(%p) %s" L"-> %s", hWnd, lpClassName, d);
//        lstrcpy(lpClassName, d);
//        n = xx;
//    }
//    return n;
//}

BOOL _JawsHookInstall(LPCTSTR pModule)
{
    HMODULE hJaws = GetModuleHandle(pModule);  // jaws 2021
    if (!hJaws)
        return false;
    BOOL b1 = HookAssign(hJaws, "user32.dll", (PROC)_GetClassNameW, (PROC)HookGetClassNameW);
    BOOL b2 = HookAssign(hJaws, "user32.dll", (PROC)_RealGetWindowClassW, (PROC)HookGetClassNameW);
    if (!b1 && !b2)
        ACC_TRACE(L"_JawsHookInstall(%s) fail", pModule);
    return b1 || b2;
}

BOOL JawsHookInstall()
{
    _JawsHookInstall(L"acceventcache.dll");  // jaws 2021
    _JawsHookInstall(L"jhook.dll");             // jaws 2020
    _JawsHookInstall(L"GlobalHooksDispatcher.dll");
    _JawsHookInstall(L"HookManager.dll");
    _JawsHookInstall(L"AccEventCache.dll");
    _JawsHookInstall(L"uiahooks.dll");
    _JawsHookInstall(L"FsDomSrv.dll");
    _JawsHookInstall(L"FSDomNodeMSAA.dll");

    return true;
}



TM_END_NAMESPACE



