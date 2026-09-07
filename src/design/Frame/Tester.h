/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Mon Feb 12 10:46:16 2007
 */

//@@MIDL_FILE_HEADING()


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __TestDll_h__
#define __TestDll_h__

#ifdef TESTER_EXPORTS
#   define TESTDLL_API                           __declspec(dllexport)
#else
#   define TESTDLL_API                           //__declspec(dllimport)
#endif

class IProj;


#ifdef __cplusplus
extern "C"{
#endif 

TESTDLL_API void TesterLoad(IProj* proj, TMHWnd hWnd, LPCTSTR pFile, int nID);

TESTDLL_API void TesterZoom(int n);

// TESTDLL_API void TesterRTL(IProj* proj, TMBool bKeep);
// TESTDLL_API void TesterLanguage(IProj* proj, LPCTSTR p);

// TESTDLL_API void ShowSysFrame();

// TESTDLL_API void SetR2L();

TESTDLL_API void TesterSetHighContrast(IProj* proj, int nCmd); 
TESTDLL_API int TesterGetHighContrast(IProj* proj); 

#ifdef __cplusplus
}
#endif

#endif
