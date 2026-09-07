/**
 *    @file
 *    @brief
 *    CWndPerformance
 *
 *    @author miragekiller <3916345933@qq.com>
 *    @date    2018-6-27
 *
 *    compatibility: c++
 *
 *    Copyright (C) 2018 miragekiller                               
 *    All rights reserved  
 */

#include "stdafx.h"
#include "WndPerformance.h"
#include "MainWnd.h"
#include "ProjMgr.h"
#include "DlgStressTesting.h"
#include "../../tmdebugext.h"
#include "../../tmclipboard.h"


using namespace TM;


///////////////////////////////////////////////////////////////////////////////
// CPU
static TMUInt64 _CWndPerformance_FileTime2UTC(const FILETIME* ftime)  
{  
    LARGE_INTEGER li;  
  
    DbgAssert(ftime);  
    li.LowPart = ftime->dwLowDateTime;  
    li.HighPart = ftime->dwHighDateTime;  
    return li.QuadPart;  
}  
  
  
static int _CWndPerformance_GetProcessorNumber()  
{  
    SYSTEM_INFO info;  
    GetSystemInfo(&info);  
    return (int)info.dwNumberOfProcessors;  
}

class CCpuRatio
{
private:
    static TMInt64 m_tmLastSys;
    TMInt64 m_tmLast;

public:
    CCpuRatio()
        : m_tmLast(0)
    {
    }

    static float Elapse() // return time data
    {
       FILETIME now;  
       GetSystemTimeAsFileTime(&now);  
       TMInt64 time = _CWndPerformance_FileTime2UTC(&now);
       float dtime = time - m_tmLastSys;
       m_tmLastSys = time;
       return dtime;
    }

    float Calc(FILETIME tmKernel, FILETIME tmUser, float dSys)
    {
        static int s_nProcessor = _CWndPerformance_GetProcessorNumber();

        TMInt64 nTime = (_CWndPerformance_FileTime2UTC(&tmKernel) + _CWndPerformance_FileTime2UTC(&tmUser)) / s_nProcessor;  
        float dTime = nTime - m_tmLast;  
        
        // We add time_delta / 2 so the result is rounded.  
        float cpu = dTime * 100.0f / dSys;
        m_tmLast = nTime;
        return cpu;  
    }
};
TMInt64 CCpuRatio::m_tmLastSys = 0;

struct CCPUThread : public CCpuRatio
{
    DWORD tid;
    float fCpu;
    void* address;
    TCHAR cName[2048];

    CCPUThread()
        : address(0)
    {
    }

    void SetCPU(float f, HANDLE hThread, DWORD _tid)
    {
        fCpu = f;
        if (address)
            return;

        tid = _tid;
        typedef LONG NTSTATUS;
        DECLARE_DLL_FUNC(dll_ntdll, NTSTATUS, WINAPI, NtQueryInformationThread,
            (HANDLE ThreadHandle, ULONG ThreadInformationClass, PVOID ThreadInformation, ULONG ThreadInformationLength, PULONG ReturnLength));
#       define ThreadQuerySetWin32StartAddress 9

        DWORD dwReturnLength = NULL;
        NTSTATUS x = _NtQueryInformationThread(hThread, ThreadQuerySetWin32StartAddress, &address, sizeof(address), &dwReturnLength);

        MEMORY_BASIC_INFORMATION mbi;
        ::VirtualQuery(address, &mbi, sizeof(mbi));
        HMODULE hm = (HMODULE)mbi.AllocationBase;
        GetModuleFileName(hm, cName, tm_countof(cName));
    }
};

typedef CMap<DWORD, CCPUThread> CThreadCPUList;
static  CThreadCPUList s_aThread;

CString _CWndPerformance_GetThreadCPUString(CThreadCPUList& aThread)
{
    CString str;
    for (CThreadCPUList::recorder rec(aThread); rec; ++rec)
    {
        CString strTmp;
        CCPUThread& th = rec.get_data();
        strTmp.Format(L"  tid:%d; cpu:%.2f%%; address:%p; module:%s\r\n",
            th.tid, th.fCpu, th.address, th.cName);
        str += strTmp;
    }
    return str;
}

CString _CWndPerformance_GetThreadCPUString()
{
    CString str(L"Threads:\r\n");
    return str + _CWndPerformance_GetThreadCPUString(s_aThread);
}

CString _CWndPerformance_GetThreadMaxCPUString()
{
    CThreadCPUList aThread;
    tm_for (CThreadCPUList::recorder rec(s_aThread); rec; ++rec)
    {
        CCPUThread& th = rec.get_data();
        if (th.fCpu < 0.0001)
            continue;
        DWORD x = th.fCpu * 10000000;
        aThread[x] = th;
    }
    tm_for (; aThread.size() > 5;)
    {
        aThread.erase(aThread.begin());
    }
    CString str(L"Max cpu Threads:\r\n");
    return str + _CWndPerformance_GetThreadCPUString(aThread);
}

static float _CWndPerformance_GetCPU()  
{
    float dTime = CCpuRatio::Elapse();

    ///////////////////////////////////////////////////////////////////////////
    // thread CPU
    CTIDList aThread;
    ListThreads(GetCurrentProcessId(), aThread);
    
    // remove unused thread
    tm_for (CThreadCPUList::recorder rec(s_aThread); rec;)
    {
        DWORD id = rec.get_key();
        if (aThread.find(id) == aThread.end())
            rec.erase();
        else
            ++rec;
    }
    
    tm_for (CTIDList::recorder rec(aThread); rec; ++rec)
    {
        DWORD tid = rec.get_data();
        CCPUThread& th = s_aThread[tid];
        FILETIME creation_time, exit_time, kernel_time, user_time; 
        HANDLE hThread = _OpenThread(THREAD_QUERY_INFORMATION, false, tid);
        if (GetThreadTimes(hThread, &creation_time, &exit_time, &kernel_time, &user_time))
        {
            float fCpu = th.Calc(kernel_time, user_time, dTime);
            th.SetCPU(fCpu, hThread, tid);
        }
        CloseHandle(hThread);
    }

    ///////////////////////////////////////////////////////////////////////////
    // process CPU
    static CCpuRatio cpuProcess;

    FILETIME creation_time, exit_time, kernel_time, user_time;  
    if (!GetProcessTimes(GetCurrentProcess(), &creation_time, &exit_time,  
        &kernel_time, &user_time))  
    {  
        // We don't DbgAssert here because in some cases (such as in the Task Manager)  
        // we may call this function on a process that has just exited but  we have  
        // not yet received the notification.  
        return -1;  
    }

    float fCpu = cpuProcess.Calc(kernel_time, user_time, dTime);
    return fCpu;
}  




///////////////////////////////////////////////////////////////////////////////
// memory
static void _CWndPerformance_GetMemoryUsage(TMUInt64& mem, TMUInt64& vmem)
{
    typedef struct _PROCESS_MEMORY_COUNTERS {
        DWORD  cb;
        DWORD  PageFaultCount;
        SIZE_T PeakWorkingSetSize;
        SIZE_T WorkingSetSize;
        SIZE_T QuotaPeakPagedPoolUsage;
        SIZE_T QuotaPagedPoolUsage;
        SIZE_T QuotaPeakNonPagedPoolUsage;
        SIZE_T QuotaNonPagedPoolUsage;
        SIZE_T PagefileUsage;
        SIZE_T PeakPagefileUsage;
    } PROCESS_MEMORY_COUNTERS;
    
    DECLARE_DLL_FUNC(dll_psapi, BOOL, WINAPI, GetProcessMemoryInfo,
        (HANDLE Process, PROCESS_MEMORY_COUNTERS* ppsmemCounters, DWORD cb));

    PROCESS_MEMORY_COUNTERS pmc = { sizeof(pmc) };
    if (_GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
    {
        mem = pmc.WorkingSetSize;
        vmem = pmc.PagefileUsage;
    }
}

///////////////////////////////////////////////////////////////////////////////
// IO
#ifndef MAX_HW_COUNTERS

typedef struct _IO_COUNTERS {
    ULONGLONG  ReadOperationCount;
    ULONGLONG  WriteOperationCount;
    ULONGLONG  OtherOperationCount;
    ULONGLONG ReadTransferCount;
    ULONGLONG WriteTransferCount;
    ULONGLONG OtherTransferCount;
} IO_COUNTERS;

DECLARE_DLL_FUNC(dll_kernel32, BOOL, WINAPI, GetProcessIoCounters,
                 (HANDLE hProcess, IO_COUNTERS* lpIoCounters));
#   define GetProcessIoCounters _GetProcessIoCounters
#endif // MAX_HW_COUNTERS

static void _CWndPerformance_GetIO(TMUInt64& read_bytes, TMUInt64& write_bytes)  
{  
    IO_COUNTERS io_counter;
    if (GetProcessIoCounters(GetCurrentProcess(), &io_counter))  
    {  
        read_bytes = io_counter.ReadTransferCount;  
        write_bytes = io_counter.WriteTransferCount;  
    }
}  

///////////////////////////////////////////////////////////////////////////////
// handles count
static int _CWndPerformance_GetProcessHandleCount()  
{
    DECLARE_DLL_FUNC(dll_kernel32, BOOL, WINAPI, GetProcessHandleCount,
        (HANDLE hProcess, PDWORD pdwHandleCount));

    DWORD dwCount = 0;
    _GetProcessHandleCount(GetCurrentProcess(), &dwCount);
    return dwCount;
}  

///////////////////////////////////////////////////////////////////////////////
// heaps count
static int _CWndPerformance_GetHeapsCount()  
{
    return GetProcessHeaps(0, 0);
}  

///////////////////////////////////////////////////////////////////////////////
// GDI count
// defines a GDI CELL
template <class T>
struct _PEB
{
    UCHAR InheritedAddressSpace;                // 00h
    UCHAR ReadImageFileExecOptions;             // 01h
    UCHAR BeingDebugged;                        // 02h
    UCHAR Spare;                                // 03h
    T Mutant;                                   // 04h
    T ImageBaseAddress;                         // 08h
    T Ldr;                                      // 0Ch struct PEB_LDR_DATA*
    T ProcessParameters;                        // 10h struct PROCESS_PARAMETERS*
    T SubSystemData;                            // 14h
    T ProcessHeap;                              // 18h
    T FastPebLock;                              // 1Ch
    T FastPebLockRoutine;                       // 20h struct PEBLOCKROUTINE*
    T FastPebUnlockRoutine;                     // 24h struct PEBLOCKROUTINE*
    ULONG EnvironmentUpdateCount;               // 28h
    T KernelCallbackTable;                      // 2Ch
    T EventLogSection;                          // 30h
    T EventLog;                                 // 34h
    T FreeList;                                 // 38h struct PEB_FREE_BLOCK*
    ULONG TlsExpansionCounter;                  // 3Ch
    T TlsBitmap;                                // 40h
    ULONG TlsBitmapBits[0x2];                   // 44h
    T ReadOnlySharedMemoryBase;                 // 4Ch
    T ReadOnlySharedMemoryHeap;                 // 50h
    T ReadOnlyStaticServerData;                 // 54h PVOID*
    T AnsiCodePageData;                         // 58h
    T OemCodePageData;                          // 5Ch
    T UnicodeCaseTableData;                     // 60h
    ULONG NumberOfProcessors;                   // 64h
    ULONG NtGlobalFlag;                         // 68h
    UCHAR Spare2[0x4];                          // 6Ch
    LARGE_INTEGER CriticalSectionTimeout;       // 70h
    ULONG HeapSegmentReserve;                   // 78h
    ULONG HeapSegmentCommit;                    // 7Ch
    ULONG HeapDeCommitTotalFreeThreshold;       // 80h
    ULONG HeapDeCommitFreeBlockThreshold;       // 84h
    ULONG NumberOfHeaps;                        // 88h
    ULONG MaximumNumberOfHeaps;                 // 8Ch
    T ProcessHeaps;                             // 90h PVOID**
    T GdiSharedHandleTable;                     // 94h
    T ProcessStarterHelper;                     // 98h
    T GdiDCAttributeList;                       // 9Ch
    T LoaderLock;                               // A0h
    ULONG OSMajorVersion;                       // A4h
    ULONG OSMinorVersion;                       // A8h
    ULONG OSBuildNumber;                        // ACh
    ULONG OSPlatformId;                         // B0h
    ULONG ImageSubSystem;                       // B4h
    ULONG ImageSubSystemMajorVersion;           // B8h
    ULONG ImageSubSystemMinorVersion;           // C0h
    ULONG GdiHandleBuffer[0x22];                // C4h
    
    T ProcessWindowStation;                     // ???
};
typedef _PEB<ULONG>         PEB32;
typedef _PEB<TMUInt64>      PEB64;
typedef _PEB<TMULongPtr>    PEB;

template <class T>
struct _GDICELL
{
    T pKernelAddress;
    USHORT wProcessId;
    USHORT wCount;
    USHORT wUpper;
    USHORT wType;
    T pUserAddress;
};
typedef _GDICELL<ULONG>         GDICELL32;
typedef _GDICELL<TMUInt64>      GDICELL64;
typedef _GDICELL<TMULongPtr>    GDICELL;


// NtQueryInformationProcess for pure 32 and 64-bit processes
typedef HRESULT (NTAPI *_NtQueryInformationProcess)(
                                                     IN HANDLE ProcessHandle,
                                                     ULONG ProcessInformationClass,
                                                     OUT PVOID ProcessInformation,
                                                     IN ULONG ProcessInformationLength,
                                                     OUT PULONG ReturnLength OPTIONAL
                                                    );

// typedef HRESULT (NTAPI *_NtReadVirtualMemory)(
//                                                IN HANDLE ProcessHandle,
//                                                IN PVOID BaseAddress,
//                                                OUT PVOID Buffer,
//                                                IN SIZE_T Size,
//                                                OUT PSIZE_T NumberOfBytesRead);

// NtQueryInformationProcess for 32-bit process on WOW64
typedef HRESULT (NTAPI *_NtWow64ReadVirtualMemory64)(
                                                      IN HANDLE ProcessHandle,
                                                      IN PVOID64 BaseAddress,
                                                      OUT PVOID Buffer,
                                                      IN ULONG64 Size,
                                                      OUT PULONG64 NumberOfBytesRead);

// PROCESS_BASIC_INFORMATION for pure 32 and 64-bit processes
typedef struct _PROCESS_BASIC_INFORMATION {
    PVOID Reserved1;
    PVOID PebBaseAddress;
    PVOID Reserved2[2];
    ULONG_PTR UniqueProcessId;
    PVOID Reserved3;
} PROCESS_BASIC_INFORMATION;

// PROCESS_BASIC_INFORMATION for 32-bit process on WOW64
// The definition is quite funky, as we just lazily doubled sizes to match offsets...
typedef struct _PROCESS_BASIC_INFORMATION_WOW64 {
    PVOID Reserved1[2];
//    PVOID64 PebBaseAddress;
    TMUInt64 PebBaseAddress;
    PVOID Reserved2[4];
    ULONG_PTR UniqueProcessId[2];
    PVOID Reserved3[2];
} PROCESS_BASIC_INFORMATION_WOW64;

const DWORD tableCount = 16384; // count of GDI table cells

struct GDICOUNT
{
    int nAll;
    int nDC;
    int nRGN;
    int nBmp;
    int nPal;
    int nFont;
    int nBrush;
    int nPen;
    int nOther;
};

enum EGdiType
{
    eGdiType_DEF,
    eGdiType_DC,
    eGdiType_UNUSED2,
    eGdiType_UNUSED3,
    eGdiType_RGN,
    eGdiType_SURF,
    eGdiType_CLIENTOBJ,
    eGdiType_PATH,
    eGdiType_PAL,
    eGdiType_ICMLCS,
    eGdiType_LFONT,
    eGdiType_RFONT,
    eGdiType_UNUSED12,
    eGdiType_UNUSED13,
    eGdiType_ICMCXF,
    eGdiType_SPRITE,
    eGdiType_BRUSH,
    eGdiType_UMPD,
    eGdiType_HLSURF,
    eGdiType_UNUSED19,
    eGdiType_UNUSED20,
    eGdiType_META,
    eGdiType_UNUSED22,
    eGdiType_UNUSED23,
    eGdiType_UNUSED24,
    eGdiType_UNUSED25,
    eGdiType_UNUSED26,
    eGdiType_UNUSED27,
    eGdiType_DRVOBJ,
    eGdiType_UNUSED29,
    eGdiType_MAX,
};

template <class T>
void GetGDICountFromTable(int pid, T* table, GDICOUNT& ct)
{
    for (int i = 0; i < tableCount; ++i)
    {
        T& cell = table[i];
        if (cell.wProcessId != pid)
            continue;
    
//        HGDIOBJ gdiHandle = (HGDIOBJ)((cell.wUpper << 16) + i);
        WORD type = cell.wType & 0x7F;
        ++ct.nAll;
        switch (type)
        {
        case eGdiType_DC: ++ct.nDC; break;          // 1
        case eGdiType_RGN: ++ct.nRGN; break;        // 4
        case eGdiType_SURF: ++ct.nBmp; break;       // 5
        case eGdiType_PAL: ++ct.nPal; break;        // 8
        case eGdiType_LFONT: ++ct.nFont; break;     // 10
        case eGdiType_BRUSH: ++ct.nBrush; break;    // 16
        case 48: ++ct.nPen; break;
        default: ++ct.nOther; break;
        }
    }
}

//#if TM_MSC <= 1200


//#endif


// PPEB _CWndPerformance_GetCurrentPeb()
// {
//     DECLARE_DLL_FUNC("kernel32.dll", PPEB, WINAPI, RtlGetCurrentPeb, ());
//     return CDllFunc_RtlGetCurrentPeb().Call();
// }

int _CWndPerformance_GetGDICount(GDICOUNT& count)
{
//    int n = sizeof(PVOID64);
    // open the process
    HANDLE hProcess = GetCurrentProcess();
    int dwId = GetCurrentProcessId();

    // NOTE: as this is undocumented, it *may vary* depending on bitness (32/64) and on Windows version.
    // use WinDbg "dt ntdll!_PEB" command and search for GdiSharedHandleTable offset to find the truth out
//     DWORD GdiSharedHandleTableOffset  = _CWndPerformance_IsAMD64() ? 0xF8 : 0x94;
    DbgAssertS1(offsetof(PEB32, GdiSharedHandleTable) == 0x94);
    DbgAssertS2(offsetof(PEB64, GdiSharedHandleTable) == 0xF8);

    DECLARE_DLL_FUNC(dll_kernel32, BOOL, WINAPI, IsWow64Process,
        (HANDLE hProcess, PBOOL Wow64Process));

    // determine if this process is running on WOW64
    BOOL wow = false;
    _IsWow64Process(hProcess, &wow);
    if (wow)
    {
        // we're running as a 32-bit process in a 64-bit process
        DECLARE_DLL_FUNC(dll_ntdll, HRESULT, WINAPI, NtWow64QueryInformationProcess64,
            (HANDLE ProcessHandle,
            ULONG ProcessInformationClass,
            PVOID ProcessInformation,
            ULONG ProcessInformationLength,
            PULONG ReturnLength));
        DbgAssert(_NtWow64QueryInformationProcess64);

        PROCESS_BASIC_INFORMATION_WOW64 pbi = {0};
        _NtWow64QueryInformationProcess64(hProcess, 0, &pbi, sizeof(pbi), NULL);

        PEB64* ppeb = (PEB64*)pbi.PebBaseAddress;
        // read PEB from 64-bit address space
//         _NtWow64ReadVirtualMemory64 read = (_NtWow64ReadVirtualMemory64)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtWow64ReadVirtualMemory64");
//         err = read(hProcess, pbi.PebBaseAddress, peb, pebSize, NULL);
//         if (err != 0)
//         {
//             printf("NtWow64ReadVirtualMemory64 PEB failed\n");
//             CloseHandle(hProcess);
//             return -1;
//         }

        // get GDI table ptr from PEB
        GDICELL64* gdiTable = (GDICELL64*)(void*)(ppeb->GdiSharedHandleTable); //*(LPVOID*)(ppeb->GdiSharedHandleTable);// (peb + GdiSharedHandleTableOffset); // address in remote process adress space
//         if (gdiTable == NULL)
//         {
//             printf("GDI32.DLL is not loaded in the process\n");
//             CloseHandle(hProcess);
//             return -1;
//         }
//         free(peb);
        GDICELL64 table[tableCount];
        const int tableSize = sizeof(table);

        // copy GDI table
//         HRESULT err = read(hProcess, gdiTable, table, tableSize, NULL);
//         if (err != 0)
//         {
// //             printf("NtWow64ReadVirtualMemory64 GdiTable failed\n");
// //             free(table);
// //             CloseHandle(hProcess);
//             return -1;
//         }

        CopyMemory(table, gdiTable, tableSize);
        GetGDICountFromTable(dwId, table, count);
    }
    else
    {
        // we're running as a 32-bit process in a 32-bit OS, or as a 64-bit process in a 64-bit OS
        DECLARE_DLL_FUNC(dll_ntdll, HRESULT, WINAPI, NtQueryInformationProcess,
            (HANDLE ProcessHandle,
            ULONG ProcessInformationClass,
            PVOID ProcessInformation,
            ULONG ProcessInformationLength,
            PULONG ReturnLength));
        DbgAssert(_NtQueryInformationProcess);

        PROCESS_BASIC_INFORMATION pbi = {0};
         _NtQueryInformationProcess(hProcess, 0, &pbi, sizeof(pbi), NULL);

        PEB* ppeb = (PEB*)pbi.PebBaseAddress;

        // get GDI table ptr
        GDICELL* gdiTable = (GDICELL*)(void*)(ppeb->GdiSharedHandleTable); //*(LPVOID*) peb + GdiSharedHandleTableOffset); // address in remote process adress space

        GDICELL table[tableCount];
        const int tableSize = sizeof(table);
        CopyMemory(table, gdiTable, tableSize);

        GetGDICountFromTable(dwId, table, count);
    }

    DECLARE_DLL_FUNC(dll_user32, DWORD, WINAPI, GetGuiResources,
        (HANDLE hProcess, DWORD uiFlags));
    count.nAll = _GetGuiResources(hProcess, GR_GDIOBJECTS);
    return 0;
}

CString _CWndPerformanc_FormatModule(HMODULE* mdl, int cb)
{
    typedef struct _MODULEINFO {
        LPVOID lpBaseOfDll;
        DWORD SizeOfImage;
        LPVOID EntryPoint;
    } MODULEINFO, *LPMODULEINFO;

    DECLARE_DLL_FUNC(dll_psapi, BOOL, WINAPI, GetModuleInformation,
        (HANDLE hProcess, HMODULE hModule, LPMODULEINFO lpmodinfo, DWORD cb));

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
        for (WORD i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++, section++)
        {
//             if (strcmp((char*)section->Name, ".rsrc") == 0)
//                 continue;
//             if (strcmp((char*)section->Name, ".reloc") == 0)
//                 continue;
//             if (strcmp((char*)section->Name, ".pdata") == 0)
//                 continue;
//             if (strcmp((char*)section->Name, ".debug") == 0)
//                 continue;
//             if (strcmp((char*)section->Name, ".data") == 0)
//                 continue;
            if (strcmp((char*)section->Name, ".idata") != 0 &&  // data
                strcmp((char*)section->Name, ".text") != 0)     // code
                continue;

            MEMORY_BASIC_INFORMATION mbi; 
            BYTE* addr = (BYTE*)hModule + section->VirtualAddress;
            if (addr > (BYTE*)0x80000000)
            {
                int x = 0;
            }
            if (!VirtualQuery(addr, &mbi, sizeof(mbi)))
                continue;
            if ((mbi.State != MEM_COMMIT) || (mbi.Protect & PAGE_NOACCESS))
                continue;
            nVirtualSize += mbi.RegionSize;// (((section->Misc.VirtualSize + 4095) >> 12) << 12);
        }
        nSizeVAll += nVirtualSize;

        MODULEINFO moduleInfo = {0};
        _GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(moduleInfo));
        nSizeAll += moduleInfo.SizeOfImage;
        str.AppendFormat(L"  %10s - %10s : ", MEM2T(moduleInfo.SizeOfImage).c_str(), MEM2T(nVirtualSize).c_str());

        TCHAR c[2048];
        GetModuleFileName(hModule, c, tm_countof(c));
        str += c;
        str += L"\r\n";
    }

    CString str2;
    str2.Format(L"modules(%d): %s - %s\r\n", cb / sizeof(HMODULE), MEM2T(nSizeAll).c_str(), MEM2T(nSizeVAll).c_str());
    return str2 + str;
}

CString _CWndPerformance_GetModulesString()
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
    return _CWndPerformanc_FormatModule(mdl, cb);
}


///////////////////////////////////////////////////////////////////////////////
// CWndPerformance
#define ID_RESTORE  1000
#define ID_DUMP     1001
#define ID_COPY1    1002
#define ID_COPY2    1003

CWndPerformance::CWndPerformance()
{
}

CWndPerformance::~CWndPerformance()
{
}

CWndPanel& CWndPerformance::GetPanel()
{
    return TM_OTHIS(CMainWnd, m_wndPerformance)->GetPanel();
}

TMResult CWndPerformance::OnPreCommand(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    while(HIWORD(wParam) == BN_CLICKED)
    {
        int id = LOWORD(wParam);
        if (id == ID_RESTORE)
        {
            for (CDebugLeakInfoRec rec(*_dbgApp.LeakGetBegin()); rec; ++rec)
            {
                TMDbgClassInfo* p = const_cast<TMDbgClassInfo*>(rec.get_data());
                p->m_bDumped = false;
            }
            _dbgApp.LeakGetEnd();
        }
        else if (id == ID_DUMP)
        {
            CDebugAnalysis::CDebugHotKey::Flip();
        }
        else if (id == ID_COPY1 || id == ID_COPY2)
        {
            int nATCheckMems = 0;
            CMemDump d;
            if (id == ID_COPY1)
                _dbgApp.LeakDump(&d, 0, nATCheckMems);
            else
                _dbgApp.LeakDump(0, &d, nATCheckMems);
            CString str;
            d.AppendToCSV(str);
            CClipboard cb;
            cb.AddString(str);
        }
        else
            break;
        return 0;
    }
    bHandled = false;
    return 0;
}


void CWndPerformance::OnTimer(TMHTimer, ...)
{
    CString str;

    // dpi mode
    DECLARE_DLL_FUNC(dll_shcore, HRESULT, WINAPI, GetProcessDpiAwareness,
        (HANDLE hprocess, int *value));
    int dpiMode = -1;
    if (_GetProcessDpiAwareness)
        _GetProcessDpiAwareness(GetCurrentProcess(), &dpiMode);
    static TMCStr const cs_pDPIString[4] = {
        L"not support",
        L"PROCESS_DPI_UNAWARE",
        L"PROCESS_SYSTEM_DPI_AWARE",
        L"PROCESS_PER_MONITOR_DPI_AWARE" };
    str.AppendFormat(L"DPI mode: %s\r\n", cs_pDPIString[dpiMode + 1]);

    // cpu
    float cpu = _CWndPerformance_GetCPU();  
    str.AppendFormat(L"CPU: %.2f%%\r\n", cpu);

    // render adapter
    GetPanel().GetProj()->AppendSysInfo(str, eSysInfoRender);

    // i/o
    TMUInt64 r, w;
    _CWndPerformance_GetIO(r, w);
    str.AppendFormat(L"I/O Read : %I64u byte\r\n", r);
    str.AppendFormat(L"I/O Write: %I64u byte\r\n", w);

    // handles
    int nHandle = _CWndPerformance_GetProcessHandleCount();
    str.AppendFormat(L"Handle Count: %d\r\n", nHandle);

    // heaps
    int nHeaps = _CWndPerformance_GetHeapsCount();
    str.AppendFormat(L"Heaps  Count: %d\r\n", nHeaps);

    // directory
    TCHAR cDir[2048];
    GetCurrentDirectory(2048, cDir);
    str.AppendFormat(L"Current Directory: %s\r\n", cDir);

    // GDI
    GDICOUNT gc = {0};
    _CWndPerformance_GetGDICount(gc);
    str.AppendFormat( L"GDI Object Count: %d\r\n  dc:%d\r\n  rgn:%d\r\n  bmp:%d\r\n  pal:%d\r\n  font:%d\r\n  brush:%d\r\n  pen:%d\r\n  other:%d\r\n",
        gc.nAll, gc.nDC, gc.nRGN, gc.nBmp, gc.nPal, gc.nFont, gc.nBrush, gc.nPen, gc.nOther);

    // mem
    {
        CMemDump dold, dnew;
//         int nATObjs;
//         int nATMems;
        int nATCheckMems = 0;
        _dbgApp.LeakDump(&dold, &dnew, nATCheckMems);
        TMUInt64 mem, vmem;
        _CWndPerformance_GetMemoryUsage(mem, vmem);
        mem /= 1024; vmem /= 1024; nATCheckMems /= 1024; 
        str.AppendFormat(L"Memory Working Set Size: %sK(%sK + debug %sK)\r\n",
            L2T(mem, ',').c_str(), L2T(mem - nATCheckMems, ',').c_str(), L2T(nATCheckMems, ',').c_str());
        str.AppendFormat(L"Memory Page File Usage : %sK\r\n", L2T(vmem, ',').c_str());

        TM::CString strATObjs;
        int nATMems = dold.AppendToString(strATObjs);
        nATMems /= 1024;
        str.AppendFormat(L"TM Object Count(dumped): %d(%sK):    ___UNDUMP___\r\n",
            dold.nAllCount, L2T(nATMems, ',').c_str());
        str += strATObjs;

        strATObjs.clear();
        nATMems = dnew.AppendToString(strATObjs);
        nATMems /= 1024;
        str.AppendFormat(L"TM Object Count(undumped): %d(%sK): (press F4: ___DUMP___)\r\n",
            dnew.nAllCount, L2T(nATMems, ',').c_str());
        str += strATObjs;
    }

    str += _CWndPerformance_GetModulesString();
    str += _CWndPerformance_GetThreadCPUString();
    str += _CWndPerformance_GetThreadMaxCPUString();
    str += L"\r\n\r\n\r\n";

    Str2Html(str);
    str.Replace(L"___UNDUMP___", L"<a id=\"" TM_T(TM_NUMBER_OF(ID_RESTORE)) L"\">undump</a>; " L"<a id=\"" TM_T(TM_NUMBER_OF(ID_COPY1)) L"\">copy</a>" );
    str.Replace(L"___DUMP___", L"<a id=\"" TM_T(TM_NUMBER_OF(ID_DUMP)) L"\">dump</a> to c:\\temp; " L"<a id=\"" TM_T(TM_NUMBER_OF(ID_COPY2)) L"\">copy</a>");
    SetWindowText(str, TUISETTEXT::eHTML);
}


///////////////////////////////////////////////////////////////////////////////
// class CWndObjInfo
CWndPanel& CWndObjInfo::GetPanel()
{
    return TM_OTHIS(CMainWnd, m_wndObjInfo)->GetPanel();
}

//  CAniProc
void CWndObjInfo::OnTimer(TMHTimer, ...)
{
    IObjEdit* pCur = GetPanel().ItemGetSelectedData(false);
    if (!pCur)
        return;

    CString str(pCur->GetDebugInfo());
    if (!str.IsEmpty())
        SetWindowText(str);
}


// 
// #include <vector>
// #include <iostream>
// 
// typedef struct {
//     DWORD   pid;
//     HANDLE  hProcess;
//     DWORD   count;
//     HANDLE  handles[1];
// } GDITable;
// 
// typedef DWORD(WINAPI* NtGdiGetProcessHandleTableFunc)(HANDLE, GDITable**, DWORD);
// 
// void EnumProcessGDIObjects() {
//     HMODULE hGdi32 = LoadLibrary(L"gdi32.dll");
//     NtGdiGetProcessHandleTableFunc pGetHandleTable =
//         (NtGdiGetProcessHandleTableFunc)GetProcAddress(hGdi32, "NtGdiGetProcessHandleTable");
// 
//     if (pGetHandleTable) {
//         GDITable* pTable = nullptr;
//         DWORD ret = pGetHandleTable(GetCurrentProcess(), &pTable, 0);
// 
//         if (ret > 0 && pTable) {
//             std::wcout << L"????GDI????: " << pTable->count << std::endl;
//             for (DWORD i = 0; i < pTable->count; ++i) {
//                 HGDIOBJ hObj = (HGDIOBJ)pTable->handles[i];
//                 std::wcout << L"??: 0x" << std::hex << (DWORD_PTR)hObj
//                     << L" ??: " << GetObjectType(hObj) << std::endl;
//             }
//             LocalFree(pTable);
//         }
//     }
//     FreeLibrary(hGdi32);
// }
// 
// 
// 
// 
// #include <wbemidl.h>
// #pragma comment(lib, "wbemuuid.lib")
// 
// void QueryGDIByWMI() {
//     IWbemServices* pSvc = nullptr;
//     IWbemLocator* pLoc = nullptr;
//     CoInitializeEx(0, COINIT_MULTITHREADED);
//     pLoc->ConnectServer(L"ROOT\\CIMV2", NULL, NULL, 0, NULL, 0, 0, &pSvc);
// 
//     IEnumWbemClassObject* pEnumerator = nullptr;
//     pSvc->ExecQuery(L"WQL", L"SELECT * FROM Win32_GDIObject", WBEM_FLAG_FORWARD_ONLY, NULL, &pEnumerator);
//     // ??????
// }