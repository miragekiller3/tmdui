/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2025.06.03
 *
 *  Copyright (C) 2025 miragekiller
 */

#pragma once

#ifndef MEMHOOK_H
#define MEMHOOK_H

#include <dbghelp.h>


void MemHookInit();

///////////////////////////////////////////////////////////////////////////////
// allocate
// char* _MyAlloc(size_t n);
// void  _MyFree(void* p);
// 
// template<class T> struct _my_alloc
// {
//     TM_SELF_ALLOCATOR(_MyAlloc(n),_MyFree(p));
// };
// 
// #define my_alloc_map(k,v) tm_allocator_map(_my_alloc,k,v)
#define my_alloc_map  dbg_allocator_map


///////////////////////////////////////////////////////////////////////////////
// CCallBack
struct CCallBack
{
    UINT_PTR    m_hash;
    void*       m_callback[32];
    int         m_nCallBack;
    
    bool operator < (const CCallBack& r) const { return m_hash < r.m_hash; }
    
    void Init();
    void MackCallBack();
    void MakeHash();
};
struct CCallBackVal
{
    int  m_nSize;
    int  m_nCount;
    int  m_nSum;
    int  m_nDel;
    int  m_nSizeDel;
    int  m_nSumDel;
    CCallBackVal() : m_nSize(0), m_nCount(0), m_nSum(0), m_nDel(0), m_nSizeDel(0), m_nSumDel(0) {}
};
typedef TM::CMap<CCallBack, CCallBackVal, std::less<CCallBack>, my_alloc_map(CCallBack, CCallBackVal) > CCallBackMap; // callback; all mem
typedef std::pair<CCallBack, CCallBackVal> CallBackInfo;
CCallBackMap& BeginGetCallBackMap();
void EndGetCallBackMap();

TM::CString GetCallBackStr(void* f);
TM::CString GetSourceAndLine(void* f);
void JumpToSource(void* f);


void HookMainThreadEnable();
void HookMainThreadDisable();
struct CAutoHookMainThread
{
    CAutoHookMainThread() { HookMainThreadDisable(); }
    ~CAutoHookMainThread() { HookMainThreadEnable(); }
};

bool EnumSymbols(LPCSTR dllPath, PSYM_ENUMERATESYMBOLS_CALLBACK proc, void* param, DWORD64 pStart, DWORD64 pEnd);
bool EnumSymbols(HMODULE hModule, PSYM_ENUMERATESYMBOLS_CALLBACK proc, void* param, DWORD64 pStart, DWORD64 pEnd);

SYM_TYPE IsLoadSymbal(LPCSTR dllPath);
TM::CString SymbolsGetTypeNameFromIndex(DWORD64 modBase, DWORD typeIndex);

void SymbolsAnalyzePData(HMODULE hModule, PSYM_ENUMERATESYMBOLS_CALLBACK proc, void* param, DWORD64 pStart, DWORD64 pEnd);
void SymbolsAnalyzeResource(HMODULE hModule, PSYM_ENUMERATESYMBOLS_CALLBACK proc, void* param, DWORD64 pStart, DWORD64 pEnd);


// struct CMemByFunc
// {
//     size_t m_nSize;
//     TM::CVector<void*, _my_alloc<void*> > m_aMem;
// 
//     CMemByFunc();
// };
// typedef TM::CMap<void*, CMemByFunc, std::less<void*>, my_alloc_map(void*, CMemByFunc) > CMemByFuncMap;
// void ParseMemByFunc(CMemByFuncMap& aMap);


// QClassBase* AddObjectQArrayData(void* p, int sub, const TM::CString& str);
// void InitHookQArrayData();


#endif // MEMHOOK_H
