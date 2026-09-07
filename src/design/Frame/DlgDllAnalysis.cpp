/**
 *  @file
 *  @brief
 *  CDlgDllAnalysis
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2025-7-10
 *
 *  compatibility: c++
 *
 *  Copyright (C) 2025 miragekiller                             
 *  All rights reserved  
 */

#include "stdafx.h"
#include "DlgDllAnalysis.h"
#include "tmfilename.h"
#include "MemHook.h"

using namespace TM;

// header 1
#define IND_DLL                 0
#define IND_SYMBOL              1   //not load; pdb; export
#define IND_ALLSIZE             2

// header 2
#define IND2_NAME               0
#define IND2_SIZE               1
#define IND2_ADDR               2
#define IND2_TYPE               3

static HTUI s_lb;


struct CColumn2 : public IColumnProxy
{
public:
    virtual void GetRenderText(TMLParam nTargetItemData, TMLParam nHeaderItemData, CText& text) tm_final
    {
        CDlgDllAnalysis::SECDATA* sd = (CDlgDllAnalysis::SECDATA*)nTargetItemData;
        text = sd->txt[nHeaderItemData];
    }

    virtual CString GetHInt(TMLParam nTargetItemData, TMLParam nHeaderItemData) tm_final { return CString(); }

    virtual bool Sort(TMLParam nTargetItemData1, TMLParam nTargetItemData2, TMLParam nHeaderItemData, TMBool bSortUp) tm_final
    {
        if (nHeaderItemData < 0)
            return false;
        CDlgDllAnalysis::SECDATA* sd1 = (CDlgDllAnalysis::SECDATA*)nTargetItemData1;
        CDlgDllAnalysis::SECDATA* sd2 = (CDlgDllAnalysis::SECDATA*)nTargetItemData2;
        if (nHeaderItemData == IND2_SIZE || nHeaderItemData == IND2_ADDR || nHeaderItemData == IND2_TYPE)
            return sd1->n[nHeaderItemData] < sd2->n[nHeaderItemData];
        return sd1->txt[nHeaderItemData].GetString() < sd2->txt[nHeaderItemData].GetString();
    }

    void Reclaim(TMLParam nTargetItemData) tm_final
    {
        CDlgDllAnalysis::SECDATA* sd = (CDlgDllAnalysis::SECDATA*)nTargetItemData;
        sd->txt[0].Reclaim();
        sd->txt[1].Reclaim();
        sd->txt[2].Reclaim();
        sd->txt[3].Reclaim();
    };
};
static CColumn2 s_column2;


///////////////////////////////////////////////////////////////////////////////
// CDlgDllAnalysis::SECS
CDlgDllAnalysis::SECS::SEC* CDlgDllAnalysis::SECS::Add(int idStr, int nSize)
{
    SECS::SEC sec;
    sec.nSize = nSize;
    sec.txt.ReCreate(MEM2T(nSize), false);
    SECS::SEC& s = aSec[idStr];
    s = sec;
    return &s;
}

int CDlgDllAnalysis::SECS::MakeTxt()
{
    int nSize = 0;
    for (CSecList::recorder rec(aSec); rec; ++rec)
    {
        SEC& sec = rec.get_data();
        if (sec.nSize)
            sec.txt.ReCreate(MEM2T(sec.nSize), false);
        nSize += sec.nSize;
    }
    return nSize;
}

///////////////////////////////////////////////////////////////////////////////
// CDlgDllAnalysis::IColumnProxy
void CDlgDllAnalysis::GetRenderText(TMLParam lpTarget, TMLParam lpHeaderItem, CText& text)
{
    SECS* p = (SECS*)lpTarget;
    if (lpHeaderItem == IND_SYMBOL && lpTarget != (TMLParam)m_secAll)
    {
        SECS::SEC& sec = p->aSec[IND_SYMBOL];
        SYM_TYPE st = IsLoadSymbal(p->strCompDll);
        if (st != sec.nSize)
        {
            sec.nSize = st;
            switch (st)
            {
            case SymNone:       sec.txt.ReCreate(L"none", false); break;
            case SymCoff:       sec.txt.ReCreate(L"Coff", false); break;
            case SymCv:         sec.txt.ReCreate(L"Cv", false); break;
            case SymPdb:        sec.txt.ReCreate(L"Pdb", false); break;
            case SymExport:     sec.txt.ReCreate(L"Export", false); break;
            case SymDeferred:   sec.txt.ReCreate(L"Deferred", false); break;
            case SymSym:        sec.txt.ReCreate(L"Sym", false); break;
            case SymDia:        sec.txt.ReCreate(L"Dia", false); break;
            case SymVirtual:    sec.txt.ReCreate(L"Virtual", false); break;
            default:            sec.txt.ReCreate(L"Unknown", false); break;
            }
        }
    }
    text = p->aSec[lpHeaderItem].txt;
}

CString CDlgDllAnalysis::GetHInt(TMLParam lpTarget, TMLParam lpHeaderItem)
{
    SECS* p = (SECS*)lpTarget;
    return p->aSec[lpHeaderItem].strHInt;
}

bool CDlgDllAnalysis::Sort(TMLParam lpTarget1, TMLParam lpTarget2, TMLParam lpHeaderItem, TMBool bSortUp)
{
    SECS* p1 = (SECS*)lpTarget1;
    SECS* p2 = (SECS*)lpTarget2;
    if (p1 == m_secAll || p2 == m_secAll)
        return (bSortUp ? p1 : p2) == m_secAll;
    if (lpHeaderItem == IND_DLL)
        return p1->strCompDll < p2->strCompDll;
    return p1->aSec[lpHeaderItem].nSize < p2->aSec[lpHeaderItem].nSize;
}

void CDlgDllAnalysis::Reclaim(TMLParam nTargetItemData)
{
    SECS* p = (SECS*)nTargetItemData;
    for (SECS::CSecList::recorder rec(p->aSec); rec; ++rec)
        rec.get_data().txt.Reclaim();
};


///////////////////////////////////////////////////////////////////////////////
// CDlgDllAnalysis
CDlgDllAnalysis::CDlgDllAnalysis()
{
}

CDlgDllAnalysis::~CDlgDllAnalysis()
{
}

void CDlgDllAnalysis::InitModule(TM::CTuiListBox& lb, HMODULE* mdl, int cb)
{
//     typedef struct _MODULEINFO {
//         LPVOID lpBaseOfDll;
//         DWORD SizeOfImage;
//         LPVOID EntryPoint;
//     } MODULEINFO, *LPMODULEINFO;
//     
//     DECLARE_DLL_FUNC(dll_psapi, BOOL, WINAPI, GetModuleInformation,
//         (HANDLE hProcess, HMODULE hModule, LPMODULEINFO lpmodinfo, DWORD cb));
    
    CString str;
    TMUInt64 nSizeAll = 0;
    TMUInt64 nSizeVAll = 0;
    for (int i = 0; mdl[i]; ++i)
    {
        HMODULE hModule = mdl[i];
        int nVirtualSize = 0;
        PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hModule;
        PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hModule + dosHeader->e_lfanew);
        PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeaders);
        SECS* secs = new SECS();
//         secs->nImageBase = ntHeaders->OptionalHeader.ImageBase;
        int nDllAll = 0;
        for (WORD i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++, section++)
        {
            BYTE* addr = (BYTE*)hModule + section->VirtualAddress;
            if (addr > (BYTE*)0x80000000)
            {
                int x = 0;
            }

            CNameList::recorder rec(m_aName);
            char c[IMAGE_SIZEOF_SHORT_NAME + 1];
            *(TMUInt64*)c = *(TMUInt64*)section->Name;
            c[IMAGE_SIZEOF_SHORT_NAME] = 0;
            CString strSec(c);
            rec.find(strSec);
            if (!rec)
                rec.insert(strSec);
            int n = &*rec.get_iter() - &*m_aName.begin();
//             MEMORY_BASIC_INFORMATION mbi; 
//             VirtualQuery(addr, &mbi, sizeof(mbi));
            int vsize = section->Misc.VirtualSize; //(((section->Misc.VirtualSize + 4095) >> 12) << 12);
            SECS::SEC* s = secs->Add(n, vsize);
            s->secImageHeader = section;
            s->strHInt.Format(
                L"hModule:0x%p\r\n"
                L"StartAddress:0x%p\r\n"
                L"EndAddress:0x%p\r\n"
                L"VirtualSize:%d\r\n"
                L"VirtualAddress:0x%x\r\n"
                L"SizeOfRawData:%d\r\n"
                L"PointerToRawData:0x%x\r\n"
                L"PointerToRelocations:0x%x\r\n"
//                 L"PointerToLinenumbers:0x%x\r\n"
                L"NumberOfRelocations:%d\r\n"
//                 L"NumberOfLinenumbers:%d\r\n"
                L"Characteristics:0x%x",
                (TMLParam)hModule, addr, addr + vsize,
                vsize,
                section->VirtualAddress, 
                section->SizeOfRawData, 
                section->PointerToRawData, 
                section->PointerToRelocations, 
//                 section->PointerToLinenumbers, 
                section->NumberOfRelocations, 
//                 section->NumberOfLinenumbers, 
                section->Characteristics
                );
            nDllAll += vsize;
            m_secAll->aSec[n].nSize += vsize;


// 
// 
// 
//             if (!VirtualQuery(addr, &mbi, sizeof(mbi)))
//                 continue;
//             if ((mbi.State != MEM_COMMIT) || (mbi.Protect & PAGE_NOACCESS))
//                 continue;
//             nVirtualSize += mbi.RegionSize;// (((section->Misc.VirtualSize + 4095) >> 12) << 12);
        }
        nSizeVAll += nVirtualSize;
        secs->Add(IND_ALLSIZE, nDllAll);

        TCHAR c[2048];
        GetModuleFileName(hModule, c, tm_countof(c));
        SECS::SEC& secDll = secs->aSec[IND_DLL];
        secDll.strHInt = c;
        secDll.txt.ReCreate(CFileName(secDll.strHInt).GetName(), false);
        secDll.txt->SetFormat(DT_WORD_ELLIPSIS);
        secs->strCompDll = secDll.txt.GetString();
        secs->strCompDll.MakeLower();
        int nid = lb.AddString(secDll.strHInt);
        lb.SetItemDataPtr(nid, secs);
        
//         MODULEINFO moduleInfo = {0};
//         _GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(moduleInfo));
//         nSizeAll += moduleInfo.SizeOfImage;
//         str.AppendFormat(L"  %10s - %10s : ", MEM2T(moduleInfo.SizeOfImage).c_str(), MEM2T(nVirtualSize).c_str());
//         
//         TCHAR c[2048];
//         GetModuleFileName(hModule, c, tm_countof(c));
//         str += c;
//         str += L"\r\n";
    }
    
//     CString str2;
//     str2.Format(L"modules(%d): %s - %s\r\n", cb / sizeof(HMODULE), MEM2T(nSizeAll).c_str(), MEM2T(nSizeVAll).c_str());
//     return str2 + str;
}

void CDlgDllAnalysis::InitModules(TM::CTuiListBox& lb)
{
    DECLARE_DLL_FUNC(dll_psapi, BOOL, WINAPI, EnumProcessModules,
        (HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded));
    
    DECLARE_DLL_FUNC(dll_psapi, BOOL, WINAPI, EnumProcessModulesEx,
        (HANDLE  hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded, DWORD dwFilterFlag));
    
    DWORD cb = 0;
    HMODULE mdl[2048] = {0};
    if (_EnumProcessModulesEx)
        _EnumProcessModulesEx(GetCurrentProcess(), mdl, tm_countof(mdl) - 1, &cb, 0x03);
    else
        _EnumProcessModules(GetCurrentProcess(), mdl, tm_countof(mdl) - 1, &cb);
    InitModule(lb, mdl, cb);
}


TMResult CDlgDllAnalysis::OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    m_aName.insert(TM_CT("dll"));
    m_aName.insert(TM_CT("symbol"));
    m_aName.insert(TM_CT("all"));
    CTuiListBox lb(Item(IDC_LISTBOX_30407));
    lb.SetPfnAutoDeleteData(CTuiProcDelete<SECS>::done);

    // init list box1
    lb.AddString(L"");
    m_secAll = new SECS();
    lb.SetItemDataPtr(0, m_secAll);
    InitModules(lb);
    int xAll = m_secAll->MakeTxt();
    m_secAll->Add(IND_ALLSIZE, xAll);

    // init header1
    CTuiHeader header(Item(IDC_HEADER_30408));
    HDITEM itm = { HDI_TEXT | HDI_WIDTH | HDI_FORMAT | HDI_LPARAM, 200, (TMStrW)L"dll name", 0, 0, HDF_AUTOSORTUP | HDF_AUTOSORTDOWN | HDF_BITMAP_ON_RIGHT };
    itm.lParam = IND_DLL;
    header.InsertItem(itm.lParam, &itm);
    itm.cxy = 60;
    for (int i = 1; i < m_aName.size(); ++i)
    {
        itm.pszText = (TMStrW)m_aName[i].c_str();
        itm.lParam = i;
        int n = header.InsertItem(itm.lParam, &itm);
        CString strHInt;
        if (m_aName[i] == TM_CT(".text"))
            strHInt = TM_CT("code section");
        else if (m_aName[i] == TM_CT(".rdata"))
            strHInt = TM_CT("read only data section");
        else if (m_aName[i] == TM_CT(".data"))
            strHInt = TM_CT("data section");
        else if (m_aName[i] == TM_CT(".pdata"))
            strHInt = TM_CT("exception table");
        else if (m_aName[i] == TM_CT(".rsrc"))
            strHInt = TM_CT("resource table");
        else if (m_aName[i] == TM_CT(".reloc"))
            strHInt = TM_CT("base relocation table");
        else if (m_aName[i] == TM_CT(".idata"))
            strHInt = TM_CT("import table");
        else if (m_aName[i] == TM_CT(".tls"))
            strHInt = TM_CT("TLS table");
        if (!strHInt.IsEmpty())
            header.SetItemHint(n, strHInt);
    }
    
    // bind list1 & header1
    lb.SetHeaderProxy(header, this);


    // list box2
    CTuiListBox lb2(Item(IDC_LISTBOX_30410));
    lb2.SetPfnAutoDeleteData(SECDATA::CAutoDel::done);

    // header2
    CTuiHeader header2(Item(IDC_HEADER_30413));
    itm.lParam = IND2_NAME; itm.pszText = (TMStrW)L"name";    header2.InsertItem(itm.lParam, &itm);
    itm.lParam = IND2_SIZE; itm.pszText = (TMStrW)L"size";    header2.InsertItem(itm.lParam, &itm);
    itm.lParam = IND2_ADDR; itm.pszText = (TMStrW)L"address"; header2.InsertItem(itm.lParam, &itm);
    itm.lParam = IND2_TYPE; itm.pszText = (TMStrW)L"type";    header2.InsertItem(itm.lParam, &itm);

    // bind list2 & header2
    lb2.SetHeaderProxy(header2, &s_column2);

//     _tuiApp.AddMessageFilter(this);
    return 0;
}

TMResult CDlgDllAnalysis::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    s_lb = 0;
    bHandled = false;
//     _tuiApp.RemoveMessageFilter(this);
    return 0;
}

static void _CDlgDllAnalysis_Exprot(HTUI hList, TMCStr strFn)
{
    CAutoHookMainThread autoHook;

    CTuiListBox lb(hList);
    TM::CStreamT<TM::CFile> f1;
    f1.CreateAlways(strFn, f1.e_faWrite);
    lb.ExportCSV(&f1);
}

TMResult CDlgDllAnalysis::OnExport(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    _CDlgDllAnalysis_Exprot(Item(IDC_LISTBOX_30407), L"c:\\temp\\Dll_Analysis_dlls.csv");
    _CDlgDllAnalysis_Exprot(Item(IDC_LISTBOX_30410), L"c:\\temp\\Dll_Analysis_dll_cur.csv");
    return 0;
}

struct SYMPARAM
{
    CDlgDllAnalysis::SECS::SEC* sec;
    DWORD64 addrBgn;
    DWORD64 addrEnd;
    CDlgDllAnalysis* dlg;
    const CString* strTitle;
    int nProgress;
    CDlgDllAnalysis::SECDATA* lastData;
};

static BOOL CALLBACK EnumSymbolsProc(
     PSYMBOL_INFO pSymInfo,
     ULONG SymbolSize,
     PVOID UserContext)
{
    if (!s_lb)
        return false;

    SYMPARAM* sp = (SYMPARAM*)UserContext;
    if (!sp->sec->secImageHeader)
        return true;
    if (pSymInfo->Address < sp->addrBgn || pSymInfo->Address >= sp->addrEnd)
        return true;
    
    CAutoHookMainThread autoHook;

    CDlgDllAnalysis::SECDATA* sd = new CDlgDllAnalysis::SECDATA();
    sd->hModule = pSymInfo->ModBase;
    sd->txt[IND2_NAME].ReCreate(pSymInfo->Name, false);
    sd->txt[IND2_NAME]->SetFormat(DT_WORD_ELLIPSIS);
    sd->txt[IND2_SIZE].ReCreate(MEM2T(pSymInfo->Size), false);
    sd->txt[IND2_ADDR].ReCreate(TM_H2T(pSymInfo->Address), false);
    sd->txt[IND2_TYPE].ReCreate(TM_L2T(pSymInfo->TypeIndex), false);
    sd->n[IND2_SIZE] = pSymInfo->Size;
    sd->n[IND2_ADDR] = pSymInfo->Address;
    sd->n[IND2_TYPE] = pSymInfo->TypeIndex;

    // calc size
    if (sp->lastData && !sp->lastData->n[IND2_SIZE])
    {
        sp->lastData->n[IND2_SIZE] = abs((long)(pSymInfo->Address - sp->lastData->n[IND2_ADDR]));
        sp->lastData->txt[IND2_SIZE].ReCreate(MEM2T(sp->lastData->n[IND2_SIZE]) + L"?", false);
    }
    sp->lastData = sd;

    // set progress text
    if (sp->dlg)
    {
        int pg = 1000 * (sp->addrEnd - pSymInfo->Address) / (sp->addrEnd - sp->addrBgn);
        if (pg != sp->nProgress)
        {
            sp->nProgress = pg;
            CString str(*sp->strTitle);
            str.AppendFormat(L" %g%%", (float)pg / 10.0f);
            sp->dlg->SetWindowText(str);
        }
    }

    // `string'
//     if (sd->txt[IND2_NAME].GetString()[1] == 's')
//     {
//         int x = 0;
//     }
    
    CString str(pSymInfo->Name);
    str.AppendFormat(L" %d", pSymInfo->Size);
    CTuiListBox lb(s_lb);
    int n = lb.AddString(str);
    lb.SetItemDataPtr(n, sd);
    
    return TRUE;
}

//TMResult CDlgDllAnalysis::OnSelChange1(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
TMResult CDlgDllAnalysis::OnSelChange1(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//     bHandled = false;

//    CTuiListBox lb(pMsg->hWnd);
    CTuiListBox lb((HTUI)hWndCtl);
    TUIHITINFO hi;
    GetAPP()->GetMouseLDownState(hi);
    int n = hi.dwCode;
    if (n <= 0)
        return 0;
    SECS* p = (SECS*)lb.GetItemDataPtr(n);
    if (!p)
        return 0;


    HITCODE_HEADER hih = hi.dwCode2;
    CTuiHeader header(Item(IDC_HEADER_30408));
    TCHAR c[256];
    HDITEM hdi = { HDI_LPARAM | HDI_TEXT };
    hdi.pszText = c;
    hdi.cchTextMax = 256;
    header.GetItem(hih.iItem, &hdi);
//    int lp = header.GetItemData(hih.iItem);
    SECS::SEC* sec = &p->aSec[hdi.lParam];
    Item(IDC_MEMO_30417).SetWindowText(sec->strHInt);
    if (hdi.lParam <= IND_ALLSIZE)
        return 0;

    if (!sec->secImageHeader)
        return 0;

    CTuiListBox lb2(Item(IDC_LISTBOX_30410));
    lb2.ResetContent();
    s_lb = lb2;
    HMODULE hModule = GetModuleHandleA(p->strCompDll);
    CStrView strProp(L"Text");
    CString strTitle = GetAPP()->LoadString(_ModuleATL.GetResourceInstance(), IDD, &strProp);
    strTitle += L" (";
    strTitle += c;
    strTitle += L")";

    DWORD64 b = (DWORD64)hModule + sec->secImageHeader->VirtualAddress;
    DWORD64 e = b + sec->secImageHeader->Misc.VirtualSize;

     //EnumSymbols(p->strCompDll, EnumSymbolsProc, &sp, b, e);
     //return 0;

    if (StrCompare(c, L".pdata") == 0)
    {
        SYMPARAM sp = { sec, 0, -1 };
        SymbolsAnalyzePData(hModule, EnumSymbolsProc, &sp, b, e);
    }
    if (StrCompare(c, L".rsrc") == 0)
    {
        SYMPARAM sp = { sec, 0, -1 };
        SymbolsAnalyzeResource(hModule, EnumSymbolsProc, &sp, b, e);
    }
    else
    {
        SYMPARAM sp = { sec, b, e, this, &strTitle };
        EnumSymbols(hModule, EnumSymbolsProc, &sp, b, e);
    }

    SetWindowText(strTitle);
    return 0;
}

static CDlgDllAnalysis::SECDATA* _CDlgDllAnalysis_GetSecData(HTUI hList2)
{
    CTuiListBox lb2(hList2);
    int n = lb2.GetCurSel();
    if (n < 0)
        return 0;
    CDlgDllAnalysis::SECDATA* ps = (CDlgDllAnalysis::SECDATA*)lb2.GetItemDataPtr(n);
    return ps;
}

TMResult CDlgDllAnalysis::OnSelChange2(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    SECDATA* ps = _CDlgDllAnalysis_GetSecData((HTUI)hWndCtl);
    if (!ps)
        return 0;

    CAutoHookMainThread autoHook;

    CString str(ps->txt[IND2_NAME].GetString());
    bool bstr = (str == L"`string\'");
    str += L"\r\n";
    str += ps->txt[IND2_ADDR].GetString();
    str += L"\r\n";
    str += GetSourceAndLine((void*)ps->n[IND2_ADDR]);
    if (bstr)
    {
        str += L"\r\n";
        TMCStrA p = (TMCStrA)ps->n[IND2_ADDR];
        bool bChar = !(p[0] != 0 && p[1] == 0);
        if (bChar)
            str += p;
        else
            str += (TMCStrW)p;
    }
    if (ps->n[IND2_TYPE])
    {
        str += L"\r\n";
        str += SymbolsGetTypeNameFromIndex(ps->hModule, ps->n[IND2_TYPE]);
    }
    Item(IDC_MEMO_30417).SetWindowText(str);
    return 0;
}

TMResult CDlgDllAnalysis::OnDblClk2(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
    SECDATA* ps = _CDlgDllAnalysis_GetSecData((HTUI)hWndCtl);
    if (ps)
        JumpToSource((void*)ps->n[IND2_ADDR]);
    return 0;
}


TMResult CDlgDllAnalysis::OnOk(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

TMResult CDlgDllAnalysis::OnCancel(TMUShort wNotifyCode, TMUShort wID, void* hWndCtl, TMBool& bHandled)
{
//    EndDialog(wID);
    DestroyWindow();
    return 0;
}

//CTuiMsgFilter
// BOOL CDlgDllAnalysis::PreTranslateMessage(TM::TUIMSG* pMsg, TMResult& lResult)
// {
//     return false;
// }


