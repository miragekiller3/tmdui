#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers


#define _WIN32_WINNT 0x0500
#define _WIN32_IE    0x0500
#define WINVER 0x050a



//TM::DbgMemLeak dml;


#include <atlbase.h>

//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module

extern CComModule _Module;

#include <atlapp.h>




