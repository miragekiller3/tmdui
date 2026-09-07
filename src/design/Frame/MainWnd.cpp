//-----------------------------------------------------------
//MainWnd.cpp

//  Copyright (C) 2004 miragekiller                               
//  All rights reserved                                                     
//
//
//  Author: 
//      miragekiller    <3916345933@qq.com>
//
//    Histroy :
//      9/29/2006       create
//-------------------------------------------------------------

#include "StdAfx.h"
#include "MainWnd.h"
#include "Tester.h"
#include "ResetIddlg.h"
#include "Propertydlg.h"
// #include "DlgZoom.h"
#include "Wizard.h"
#include "Tools.h"
#include "WndControlsView.h"
#include "ProjMgr.h"
#include "DlgImportWindow.h"
#include "DlgAbout.h"
#include "DlgSaveLang.h"
#include "DlgSysColor.h"
#include "DlgGDIDraw.h"
#include "DlgAnimateWindow.h"
#include "DlgCheckKeyword.h"
#include "DlgTrackWin.h"
#include "DlgStressTesting.h"
#include "DlgMemAnalysis.h"
#include "DlgDllAnalysis.h"
#include "DlgAutoTranslate.h"
#include "cfg.h"
#include "DlgTesterSetting.h"
#include "DlgAllocGpuMemory.h"
#include "DlgSpyMsg.h"
#include "PropertyDlg.h"
#include "DlgBlockInject.h"
#include "tmarchivemenu.h"
#include "tmkeyboard.h"
#include "tmdrag.h"
#include "tmwnd.h"
#include "tmformatguid.h"
#include "memhook.h"
//#include "DlgPdb.h"
#include <devguid.h>
#include <setupapi.h>

#pragma comment(lib, "Setupapi.lib")

using namespace TM;


TM::CString GetModulePath()
{
    TM::CFileName  strModule;
    strModule.GetModuleFilePath();
    return strModule;
}

TM::CCfg g_cfg(GetModulePath() + _T("\\TMDuiDesign.ini"));


CCfgDebug::CCfgDebug()
    : m_bPauseApp(false)
    , m_bEnalbeMemAna(false)
    , m_bKeepMenuPop(false)
    , bDisableGhost(false)
    , bUIDark(false)
    , bUIHC(false)
    , nBk(IDC_TILE_BK_SLASH)
    , nDevGra(0)
    , nDevTxt(0)
//    , m_nTestHC(0)
//    , m_nTestColor(0)
{
    bShowHideWindow = false;
    bLogBkBuffer = false;
    bLogFlip = false;
    bLogMsg = false;
    this->DoSerializeInput(g_cfg);
}

void CCfgDebug::InitMenu(HMENU hMenu)
{
    TM::CMenuArchive ma(hMenu);
    this->DoSerializeMenuOutput(ma);
}

void CCfgDebug::CheckPauseApp()
{
    if (m_bPauseApp || CKeyboard::IsKeyDown(VK_CONTROL))
    {
        m_bPauseApp = false;
        SaveCfg();
        ::MessageBox(0, L"click button to continue", L"wait app", MB_OK | MB_TOPMOST | MB_SETFOREGROUND | MB_TASKMODAL);
    }

    if (m_bEnalbeMemAna)
    {
        m_bEnalbeMemAna = false;
        SaveCfg();
        MemHookInit();
    }
}

void CCfgDebug::SaveCfg()
{
    this->DoSerializeOutput(g_cfg);
}

void CCfgDebug::ExecDark()
{
    struct CDarkStyle : public TM::CColorTransformDark
    {
        virtual TMColor ConvertColor(TMLParam context, TMColor clr) tm_override
        {
            if (context == IDC_CLR_TOOLBAR || context == IDP_GRADIENT_10003)
                return TMRGB(0,0,0);
            else if (context == IDC_CLR_CAPTION)
                return TMRGB(255,255,255);
            else if (context == IDO_TUI_MENUBAR_10004 && clr != TMRGB(255,255,255))
                return clr;
            return CColorTransformDark::ConvertColor(context, clr);
        }
    };
        
    static CDarkStyle style;
    AppGetEditor()->SetColorTransform(_Module.m_hInstResource, bUIDark ? &style : 0);
}

void CCfgDebug::ExecHC()
{
    AppGetEditor()->SetHighContrast(bUIHC);
}

void CCfgDebug::ExecBk(CMainWnd& wnd)
{
    TM::CTuiButton but(wnd.Item(IDC_SELBK));
    but.SetForePic(nBk);
    wnd.Item(IDC_STATIC_30334).SetBkPic(nBk);
}


CMainWnd::CMainWnd()
    : m_bIsControlsDlgVisibale(TMFalse) 
    , m_bIsCtrlPropertyDlgVisibale(TMFalse)
    , m_eMode (ePlugInModeUnknown)
{
//     _Module.m_pMainWnd = this;
//     _Module.m_pIMainWnd = this;

//    m_pPropDlg = new CPropertyDlg();

}

CMainWnd::~CMainWnd()
{
    DbgTrace("CMainWnd::~CMainWnd()");
//     if (m_pCtrlPropertyDlg)
//     {
//         delete m_pCtrlPropertyDlg;
//         m_pCtrlPropertyDlg = NULL;
//     }
//     if (m_pPropDlg)
//         delete m_pPropDlg;

}

TMResult CMainWnd::OnInitDialog (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;

    DoSerializeInput(g_cfg);

    m_curPanel.SubclassWindow(Item(m_curPanel.IDD));
    m_dlgProperty.SubclassWindow(Item(m_dlgProperty.IDD));
    m_dlgProperty.Init();
    m_wndSearchObj.SubclassWindow(Item(IDO_TUI_OBJ));
    m_wndPerformance.SubclassWindow(Item(m_wndPerformance.IDD));
    m_wndObjInfo.SubclassWindow(Item(m_wndObjInfo.IDD));
    m_wndMemRecorder.SubclassWindow(Item(m_wndMemRecorder.IDD));
    m_wndSelector.SubclassWindow(Item(m_wndSelector.IDD));
    m_wndAcc.SubclassWindow(Item(m_wndAcc.IDD));
    m_wndThumb.SubclassWindow(Item(m_wndThumb.IDD));

    TM::CTuiListBox lb(Item(IDC_LISTBOX_30191));
    int x = lb.AddString(TM_CT("sesame"));
    lb.SetItemData(x, IDP_TILE_BK_1P);
    x = lb.AddString(TM_CT("noise"));
    lb.SetItemData(x, IDC_TILE_BK_NOISE);
    x = lb.AddString(TM_CT("floral"));
    lb.SetItemData(x, IDC_TILE_BK_FLORAL);
    x = lb.AddString(TM_CT("slash"));
    lb.SetItemData(x, IDC_TILE_BK_SLASH);
    x = lb.AddString(TM_CT("grid"));
    lb.SetItemData(x, IDC_TILE_BK_GRID);
    x = lb.AddString(TM_CT("none"));
    lb.SetItemData(x, 0);

    CTuiMenuBar m_wndMB(Item(IDC_MENUBAR));
    m_wndMB.SetMenu(_Module.m_hInst, IDR_MENU_FRAME);
    m_hMenu = m_wndMB.GetMenu();

    return 0;
}


static bool AutoCheckMenuItem(HMENU hMenu, UINT id)
{
    MENUITEMINFO mInfo = { sizeof(mInfo) };  
    mInfo.fMask = MIIM_STATE;  
    BOOL b = GetMenuItemInfo(hMenu, id, false, &mInfo);
    DbgAssert(b);
    b = mInfo.fState & MFS_CHECKED;
    if (b)
        mInfo.fState &= ~MFS_CHECKED;
    else
        mInfo.fState |= MFS_CHECKED;
    SetMenuItemInfo(hMenu, id, false, &mInfo);
    return !b;
}

TMResult CMainWnd::OnCommand(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    TMResult rt = m_curPanel.OnCommand(uMsg, wParam, lParam, bHandled);
    if (bHandled)
        return rt;

    TMUInt uFlag = MF_BYCOMMAND|MF_UNCHECKED;
    TMUShort uCmd = LOWORD(wParam);
    switch (uCmd)
    {
    case IDM_FRAME_SAVE:
    case IDM_FRAME_SAVEAS:
        return SaveResource(eSaveModeNormal, uCmd);

    case IDM_FRAME_SAVESEL:
        return SaveResource(eSaveModeSel, uCmd);
        
    case IDM_FRAME_OPEN:
        DbgTrace("CMainWnd::OnCommand----IDM_FRAME_OPEN");
        OpenDlgXml(false);
        break;

    case IDM_MERGE:
        OpenDlgXml(true);
        break;

    case IDM_FRAME_LOADLANG:
        OpenDlgLang (false);
        break;

    case IDM_FRAME_SAVELANG:
        SaveDlgSkin (true);
        break;

    case IDM_FRAME_SAVESKIN:
        SaveDlgSkin (false);
        break;

    case IDM_SHOW_PROPERTY:
        {
            m_bIsCtrlPropertyDlgVisibale = !m_bIsCtrlPropertyDlgVisibale;
            if (m_bIsCtrlPropertyDlgVisibale)
                uFlag = MF_BYCOMMAND|MF_CHECKED;
            ShowCtrlPropertyDlg(m_bIsCtrlPropertyDlgVisibale);
            CheckMenuItem(m_hMenu,IDM_SHOW_PROPERTY,uFlag);
        }        
        break;
    case IDM_SHOW_CONTROLS:
        m_bIsControlsDlgVisibale = !m_bIsControlsDlgVisibale;
        if (m_bIsControlsDlgVisibale)
             uFlag = MF_BYCOMMAND|MF_CHECKED;
        ShowControlsDlg(m_bIsControlsDlgVisibale);
        CheckMenuItem(m_hMenu,IDM_SHOW_CONTROLS,uFlag);
        break;

    case IDM_SHOW_HIDE:
        m_cfgDbg.bShowHideWindow = !m_cfgDbg.bShowHideWindow;
        if (m_cfgDbg.bShowHideWindow)
            uFlag = MF_BYCOMMAND|MF_CHECKED;
        CheckMenuItem(m_hMenu,IDM_SHOW_HIDE,uFlag);
//         m_CtrlPanel.Invalidate();
        break;

    case IDM_UI_DARK:
        m_cfgDbg.bUIDark = AutoCheckMenuItem(m_hMenu, IDM_UI_DARK);
        m_cfgDbg.SaveCfg();
        m_cfgDbg.ExecDark();
//         this->SetFrame(IDC_FORM_EDIT_OBJ);
        break;

    case IDM_UI_HIGHCONTRAST:
        m_cfgDbg.bUIHC = AutoCheckMenuItem(m_hMenu, IDM_UI_HIGHCONTRAST);
        m_cfgDbg.SaveCfg();
        m_cfgDbg.ExecHC();
        break;

    case IDM_TOOL_TEST:
        TestCurEditWindow();
        break;

    case IDM_ZOOMIN:
        TesterZoom(17);
        break;
        
    case IDM_ZOOMOUT:
        TesterZoom(-17);
        break;

//     case IDM_TEST_SHOWFRM:
//         ShowSysFrame();
//         break;

    case IDM_TEST_LOADPROXY:
        ::SendMessage(GetHWND(), WM_GETOBJECT, 0, -99);
        break;

    case IDM_R2L:
        GetProj()->TesterRTL(false);
        break;

    case IDM_SETTING:
        {
            CDlgTesterSetting::CPtr ptr;
            (ptr.CreateInstance()->m_panel = &GetPanel()),
                ptr->Create(*AppGetEditor(), (HWND)0);
        }
        break;

    case IDM_DEVICE:
    case IDM_DEVICE_TXT:
        if (HIWORD(wParam) == CBN_SELENDOK)
        {
            CTuiListBox lb(CTuiComboUnit((HTUI)lParam).GetPopList());
            int n = lb.GetCurSel();
            if (uCmd == IDM_DEVICE)
                m_cfgDbg.nDevGra = n;
            else
                m_cfgDbg.nDevTxt = n;
            m_cfgDbg.SaveCfg();
            if (n >= 0)
                GetProj()->TesterChangeDevice(uCmd == IDM_DEVICE, n);
        }
        break;

    case IDM_TEST_LANGUAGE:
        OpenDlgLang (true);
        break;

//     case IDM_CHECKUNMARCO:
//         CheckUnusedMarco (GetHWND(), m_strFullName);
//         break;

    case IDM_CHECKKEYWORD:
        {
            CDlgCheckKeyword::CPtr ptr;
            ptr.CreateInstance(),
                (ptr->m_panel = &m_curPanel),
                ptr->Create(*AppGetEditor(), (HWND)0);
        }
        break;

    case IDM_RESETCONTROLID:
        {
            CDlgResetId dlg;
            dlg.m_proj = GetProj();
            dlg.DoModal(*AppGetEditor(), GetHWND());
//             if (dlg.DoModal() == IDOK && dlg.m_nId > 100)
//                 GetProj()->ResetControlId(dlg.m_nId);
        }
        break;

    case IDM_IMPORT_DLG_WIN:
        {
            CDlgImportWindow dlg;
            dlg.m_pUndo = &GetPanel().GetUndo();
            dlg.DoModal(*GetAPP(), 0);
        }
        break;

    case IDM_TRANSLATE:
        {
            CDlgAutoTranslate dlg;
            dlg.DoModal(*GetAPP(), 0);
        }
        break;

//     case IDM_CONVERT_PRJ:
//         {
//             CDlgConvertProject dlg;
//             dlg.DoModal(*GetAPP(), 0);
//         }
//         break;

    case IDM_TRACK_FOCUS_WIN:
    case IDM_TRACK_CAPTURE_WIN:
    case IDM_TRACK_FG_WIN:
    case IDM_TRACK_IME:
    case IDM_TRACK_ACC:
        CDlgTrackWin::SetTrack(this, uCmd, AutoCheckMenuItem(m_hMenu, uCmd));
        break;

    case IDM_GPU_MEMORY:
        {
            static CDlgAllocGpuMemory::CPtr dlg;
            if (dlg.IsWindow())
                dlg->DestroyWindow();
            dlg.CreateInstance()->Create(*GetAPP(), GetHWND());
        }
        break;

    case IDM_GPU_LOST:
//         {
//             struct Dev
//             {
//                 static void ChangeDeviceState(HDEVINFO hDI, SP_DEVINFO_DATA& data, DWORD State)
//                 {
//                     DWORD len = 1024;
//                     wchar_t buffer[1024] = {0};
//                     SetupDiGetDeviceInstanceId(hDI, &data, buffer, len, &len);
//                     DbgTrace(L"SetupDiEnumDeviceInfo: %s, %d, %s", DbgFormatGuid(data.ClassGuid).c_str(), data.DevInst, buffer);
// 
//                     SP_PROPCHANGE_PARAMS propChange = { sizeof(SP_CLASSINSTALL_HEADER) };
//                     propChange.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
//                     propChange.Scope = (State == DICS_START || State == DICS_STOP )? DICS_FLAG_CONFIGSPECIFIC : DICS_FLAG_GLOBAL;
//                        propChange.StateChange = State;
//                     propChange.HwProfile = 0;
//                     
//                     DbgVerify(::SetupDiSetClassInstallParams(hDI, &data, (SP_CLASSINSTALL_HEADER*)&propChange, sizeof(propChange)));
//                     DbgVerify(::SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDI, &data));
//                     DbgVerify(::SetupDiChangeState(hDI, &data));
//                 }
//             };
// 
//             //GUID_DEVCLASS_DISPLAY
//             static GUID GUID_DEVCLASS_DISPLAYX   = {0x4d36e968L, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18};
//             SP_DEVINFO_DATA data = { sizeof(SP_DEVINFO_DATA) };
//             HDEVINFO hDI = SetupDiGetClassDevs(&GUID_DEVCLASS_DISPLAYX, 0, 0, DIGCF_PRESENT);//DIGCF_PRESENT | DIGCF_ALLCLASSES);
//             if (!hDI)
//                 break;
//             for (DWORD i = 0; SetupDiEnumDeviceInfo(hDI, i, &data); ++i)
//             {
//                 Dev::ChangeDeviceState(hDI, data, DICS_STOP);
//                 Dev::ChangeDeviceState(hDI, data, DICS_DISABLE);
//                 Sleep(3000);
//                 Dev::ChangeDeviceState(hDI, data, DICS_ENABLE);
//                 Dev::ChangeDeviceState(hDI, data, DICS_START);
//             }
//             SetupDiDestroyDeviceInfoList(hDI); 
//         }
        if (ShellExecute(GetHWND(), L"runas", L"dxcap.exe", L"-forcetdr", L"", 0) <= (HINSTANCE)32)
        {
            keybd_event(VK_CONTROL, 0, 0, 0);
            keybd_event(VK_SHIFT,   0, 0, 0);
            keybd_event(VK_LWIN,    0, 0, 0);
            keybd_event('B',        0, 0, 0);
            keybd_event('B',        0, KEYEVENTF_KEYUP, 0);
            keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
            keybd_event(VK_SHIFT,   0, KEYEVENTF_KEYUP, 0);
            keybd_event(VK_LWIN,    0, KEYEVENTF_KEYUP, 0);
        }
        break;

    case IDM_BLOCK_INJECT:
        {
            CDlgBlockInject dlg;
            dlg.DoModal(*GetAPP(), GetHWND());
        }
        break;


//     case IDM_PARSE_PDB:
//         {
//             CDlgPdb::CPtr dlg;
//             dlg.CreateInstance()->Create(*GetAPP(), GetHWND());
//         }
//         break;

//     case IDM_SAVE_DFT_XML:
// //         SaveDefaultXml(L"defaultxml.h");
//         break;

    case IDM_SEE_SYS_COLOR:
        CDlgSysColor::CPtr().CreateInstance()->Create(*GetAPP(), GetHWND());
        break;

    case IDM_VIEW_DRAWXXX:
        CDlgDrawXXX::CPtr().CreateInstance()->Create(*GetAPP(), GetHWND());
        break;

    case IDM_VIEW_ANIMATEWINDOW:
        CDlgAnimateWindow::CPtr().CreateInstance()->Create(*GetAPP(), GetHWND());
        break;

    ///////////////////////////////////////////////////////////////////////////
    // tools
    case IDM_HELP:
    case IDM_HELPCTRLS:
    case IDM_HELPCP:
        {
            TM::CResource res;
            LPCTSTR pName;
            switch (uCmd)
            {
            case IDM_HELPCP:    pName = L"help3.htm"; break;
            case IDM_HELPCTRLS: pName = L"help2.htm"; break;
            default:            pName = L"help1.htm"; break;
            }
            res.Load(_ModuleATL.GetModuleInstance(), pName, RT_HTML);
            CFileName fn;
            fn.GetSiblingModuleFileName(pName);
            CFile f;
            f.CreateAlways(fn, f.e_faWrite);
            if (f)
                f.Write(res.GetData(), res.GetSize());
            ::ShellExecute(0, _T("open"), fn, 0, 0, SW_SHOWNORMAL);
        }
        break;

    case IDM_ABOUT:
        {
            CDlgAbout abt;
            abt.DoModal(*GetAPP(), GetHWND());
//             if (!g_dlgAbout.m_hWnd)
//             {
//                 g_dlgAbout.Create(m_hWnd);
//                 g_dlgAbout.BringWindowToTop();
//             }
        }
        break;

//     case IDM_ADD_NEWLANGUAGE:
//         AddNewLanguage();
//         break;
        
    case IDM_BACK:
        m_goto.Back();
        break;
        
    case IDM_FORWARD:
        m_goto.Forword();
        break;
        
    // popup menu on right click item  
//     case IDM_SETDEFAULT:
//         break;
// 
//     case IDM_NEW_EFF:
//         NewEff();
//         break;

    case IDM_ITEM_DELETE:
    case IDM_ITEM_DETACH:
        m_curPanel.DeleteSelect();
        break;

    case IDM_ITEM_DETACH_ALL:
        m_curPanel.DeleteAll();
        break;

    case IDM_SETUSER:
        {
            CString strOld(m_dlgSetUser.strUser);
            if (IDOK == m_dlgSetUser.DoModal(*GetAPP(), GetHWND()))
                GetProj()->ModifyUser(strOld, m_dlgSetUser.strUser);
        }
        break;

    case ID_EDIT_UNDO:
        m_curPanel.GetUndo().Undo();
        break;
        
    case ID_EDIT_REDO:
        m_curPanel.GetUndo().Redo();
        break;

    case IDC_PUSHBUTTON_TXT_SAVE:
        {
            IObjEdit* p = m_curPanel.ItemGetSelectedData(true);
            if (p)
            {
                TM::CString str;
                Item(IDC_MEMO_TXT_EDIT).GetWindowText(str);
                m_curPanel.GetUndo().TxtEditSave(p, str);
//                GetUndo().TxtEditSave(str);
            }
        }
        break;

    case IDC_PUSHBUTTON_TXT_REFRESH:
        SyncTxtEdit(true);
        break;

    case IDC_PUSHBUTTON_TXT_WORDBREAK:
        {
            TM::CTuiWidget wnd(Item(IDC_MEMO_TXT_EDIT));
            TMUInt32 dwLayout = wnd.GetLayOut();
            if (dwLayout & TM::LAYOUT_AUTOHSCROLL)
                wnd.SetLayOut(dwLayout & ~(TM::LAYOUT_AUTOHSCROLL | TM::LAYOUT_HSCROLL));
            else
                wnd.SetLayOut(dwLayout | TM::LAYOUT_AUTOHSCROLL);
        }
        break;

    case IDM_SPY_REFRESH:
        {
//            m_curPanel.GetTree().LoadExistentResource();
            TM::CString str;
            Item(IDC_EDIT_31110).GetWindowText(str);
            if (str)
                m_curPanel.LoadFile(str, false, false);
//                GetProj()->LoadFromFile(str, false);
        }
        break;

    case IDM_TRACKFOCUS:
        GetProj()->TrackFocusObject(AutoCheckMenuItem(m_hMenu, IDM_TRACKFOCUS));
        break;

    case IDM_BLOCKNEXT:
        m_cfgDbg.m_bPauseApp = AutoCheckMenuItem(m_hMenu, IDM_BLOCKNEXT);
        m_cfgDbg.SaveCfg();
        break;

    case IDM_ENABLE_MEM_ANALYSIS:
        m_cfgDbg.m_bEnalbeMemAna = AutoCheckMenuItem(m_hMenu, IDM_ENABLE_MEM_ANALYSIS);
        m_cfgDbg.SaveCfg();
        break;

    case IDM_DISABLEGHOST:
        m_cfgDbg.bDisableGhost = AutoCheckMenuItem(m_hMenu, IDM_DISABLEGHOST);
        m_cfgDbg.SaveCfg();
        break;

    case IDM_DISABLEASSERT:
        {
            bool bDisableAssert = AutoCheckMenuItem(m_hMenu, IDM_DISABLEASSERT);
            _dbgApp.TraceSetAssert(!bDisableAssert);
        }
        break;

    case IDM_RECORDWIN:
        m_cfgDbg.bLogBkBuffer = AutoCheckMenuItem(m_hMenu, IDM_RECORDWIN);
        m_cfgDbg.SaveCfg();
        break;

    case IDM_RECORDFLIP:
        m_cfgDbg.bLogFlip = AutoCheckMenuItem(m_hMenu, IDM_RECORDFLIP);
        m_cfgDbg.SaveCfg();
        break;

    case IDM_RECORDMSG:
        m_cfgDbg.bLogMsg = AutoCheckMenuItem(m_hMenu, IDM_RECORDMSG);
        m_cfgDbg.SaveCfg();
        break;

    case IDM_DEL_HWND:
        {
            IObjEdit* pSel = m_curPanel.ItemGetSelectedData(false);
            if (pSel)
                pSel->DeleteAfter5s();
        }
        break;

    case IDM_STRESS_TESTING:
        CDlgStressTesting::CPtr().CreateInstance()->Create(*AppGetEditor(), GetHWND());
        break;

    case IDM_MEM_ANALYSIS:
        {
            static CDlgMemAnalysis::CPtr dlg;
            if (dlg.IsWindow())
                dlg->DestroyWindow();
            dlg.CreateInstance()->Create(*GetAPP(), GetHWND());
        }
        break;

    case IDM_DLL_ANALYSIS:
        {
            static CDlgDllAnalysis::CPtr dlg;
            if (dlg.IsWindow())
                dlg->DestroyWindow();
            dlg.CreateInstance()->Create(*GetAPP(), GetHWND());
        }
        break;

    case IDM_KEEPPOPWIN:
        m_cfgDbg.m_bKeepMenuPop = AutoCheckMenuItem(m_hMenu, IDM_KEEPPOPWIN);
        break;

    case IDM_SPY_MESSAGE:
        if (CDlgSpyMsg::s_pMSg.IsWindow())
        {
            CDlgSpyMsg::s_pMSg->DestroyWindow();
            break;
        }
        tm_fallthrough;
    case IDM_SPY_MSG_OBJ:
        if (!CDlgSpyMsg::s_pMSg.IsWindow())
        {
            CDlgSpyMsg::s_pMSg.Release();
            CDlgSpyMsg::s_pMSg.CreateInstance(),
                CDlgSpyMsg::s_pMSg->m_wndPanel = &GetPanel(),
                CDlgSpyMsg::s_pMSg->Create(*AppGetEditor(), GetHWND());
        }
        if (uCmd == IDM_SPY_MESSAGE)
            break;
        {
            IObjEdit* pSel = m_curPanel.ItemGetSelectedData(false);
            CDlgSpyMsg::s_pMSg->AddSpyID(pSel);
        }
        break;


    // tab button for treeview
//     case IDT_DIALOG:
//         SetTreeShowMode(TM::e_otWnd);
//         break;
// 
//     case IDT_FRAME:
//         SetTreeShowMode(TM::e_otFrm);
//         break;
// 
//     case IDT_PIC:
//         SetTreeShowMode(TM::e_otPic);
//         break;

    default:
//         SelectLanguage(m_hMenu,wParam);
        bHandled = false;
        break;
    }
    return TMFalse;
}

// TMResult CMainWnd::OnRenderChanged (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     int n = _gxApp->GetDeviceType();
//     bHandled = false;
// 
//     CTuiButton but1(Item(IDC_PUSHBUTTON_31176));
//     but1.SetCheck((n & eGXGraDevMask) - 1);
// 
//     CTuiButton but2(Item(IDC_PUSHBUTTON_33096));
//     but2.SetCheck((n >> 4) - 1);
// 
//     return 0;
// }

TMResult CMainWnd::OnGetObject (TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    return 0;
}


static TM::CDataFormat s_df(TM_T("tui_copy"));

// TMResult CMainWnd::OnCopy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     if (m_ctrlTree.GetSelCount() != 1)
//     {
//         MessageBeep(-1);
//         return 0;
//     }
// 
// //     CObjItem rec(m_ctrlTree.GetSelectedItem());
// //     DbgAssert(rec);
// //     TM::CTuiObject* pObj = rec.GetObject();
// //     if (!pObj)
// //     {
// //         MessageBeep(-1);
// //         return 0;
// //     }
// // 
// //     TM::CString str;
// //     pObj->SaveAll(str, 0);
// // 
// //     if (CWindow(GetHWND()).OpenClipboard())
// //     {
// //         EmptyClipboard ();
// // 
// //         TM::CGlobalMemHandle m;
// //         m.Assign(str, (str.GetLength() + 1) * sizeof(TMCharT));
// //         SetClipboardData (TM_SW_MBS(CF_TEXT,CF_UNICODETEXT), m);
// //         
// //         TM::CGlobalMemHandle m2;
// //         m2.AssignT(pObj->m_pFactoryInfo->eType);
// //         SetClipboardData (s_df, m2);
// // 
// //         CloseClipboard ();
// //     }
// 
//     return 0;
// }
// 
// TMResult CMainWnd::OnPaste(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
// //     if (m_nSel != 1)
// //     {
// //         MessageBeep(-1);
// //         return 0;
// //     }
// // 
// //     OpenClipboard();
// //     TM::CGlobalMemHandle m2(GetClipboardData (s_df));
// //     if (m2)
// //     {
// //         TM::CGlobalMemHandle m(GetClipboardData (TM_SW_MBS(CF_TEXT,CF_UNICODETEXT)));
// //         int nSize = m.GlobalSize();
// //         TMStr pstr = (TMStr)alloca(nSize);
// //         m.CopyTo(pstr, nSize);
// //         TM::CXmlTag tag(pstr, nSize);
// // 
// //         TM::EObjType etype;
// //         m2.CopyToT(etype);
// // 
// //         switch (etype)
// //         {
// //         case TM::e_otWnd: CPopMenu<TM::e_otWnd>::CreateByXml(m_CtrlPanel, tag); break;
// //         case TM::e_otFrm: CPopMenu<TM::e_otFrm>::CreateByXml(m_CtrlPanel, tag); break;
// //         case TM::e_otPic: CPopMenu<TM::e_otPic>::CreateByXml(m_CtrlPanel, tag); break;
// //         case TM::e_otWgt:
// //             {
// //                 CObjItem rec = m_ctrlTree.GetSelectedItem();
// //                 if (rec)
// //                 {
// //                     EDIT_MODE eMode = rec.GetMode();
// //                     if (eMode == EM_WGT || eMode == EM_DIALOG)
// //                     {
// //                         TM::CString str = TM_T("<p>");
// //                         str += tag;
// //                         str += TM_T("</p>");
// //                         tag = str;
// //                         m_pEditor->CreateChild(rec.GetObject(), tag);
// //                     }
// //                 }
// //             }
// //             break;
// //         }
// //     }
// // 
// //     CloseClipboard();
// 
//     return 0;
// }


// TMResult CMainWnd::OnResChanged(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     if (m_bModified != (TMBool)wParam)
//     {
//         m_bModified = (TMBool)wParam;
//         SetWndTitle();
//     }
//     
//     return S_OK;
// }

TMResult CMainWnd::OnDestroy(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
//     CRcInfo pi(_T("mainwnd"));
//     WINDOWPLACEMENT wp = { sizeof(wp) };
//     GetWindowPlacement(GetHWND(), &wp);
//     ((RECT&)pi) = wp.rcNormalPosition;
//     pi.ProfileExchangeSave();

    DoSerializeOutput(g_cfg);

    m_dlgProperty.Clean();
    m_curPanel.Destroy();

//     m_ctrlTree.DestroyWindow();

//     m_ControlsDlg.DestroyWindow();

    DbgTrace("CMainWnd::OnDestroy");
    ::PostQuitMessage(0);
    bHandled = TMFalse;

    //if (CMainWnd::OnClose)
    //{
    //    int x = 0;
    //}
    return 0;
}

TMResult CMainWnd::OnCancel(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
//TMResult CMainWnd::OnClose(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    if (m_eMode != ePlugInModeSpy)
    {
        if (m_curPanel.GetUndo().IsModified())
        {
            TMUInt uResult = MessageBox(GetHWND(), L"The file have been modified, exit always?", _T("Warning"), MB_YESNO | MB_ICONWARNING);
            if (IDYES != uResult)
                return 0;
        }
        DestroyWindow();
    }
    else
        ::ShowWindow(GetHWND(), SW_SHOWMINIMIZED);

//    bHandled = false;
    return 0;
}

TMResult CMainWnd::OnKeyDown(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
{
    bHandled = false;
    switch (wParam)
    {
    case L'C':
        if (CKeyInput(wParam, lParam).IsModifiersControl())
        {
            SendMessage(WM_COPY, 0, 0);
            bHandled = true;
        }
        break;
        
    case L'V':
        if (CKeyInput(wParam, lParam).IsModifiersControl())
        {
            SendMessage(WM_PASTE, 0, 0);
            bHandled = true;
        }
        break;
        
        //     case VK_CONTROL:
        //         if (!m_bCtrl)
        //         {
        //             m_bCtrl = true;
        //             IObjEdit* p = _Module.GetMainWnd()->GetSelectedItemData(false);
        //             if (p)
        //                 p->InvalidateObj();
        //         }
        //         break;
    }
    
    return 0;
}

// TMResult CMainWnd::OnSize(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
// //     TM::CRect rc;
// //     HWND hWnd = GetHWND();
// //     ::GetWindowRect(hWnd, &rc);
// //     HRGN hrgn = CreateRoundRectRgn(0, 0, rc.Width() + 1, rc.Height() + 1, 10, 10);
// //     ::SetWindowRgn(hWnd, hrgn, true);
//     bHandled = false;
//     return 0;
// }


// TMResult CMainWnd::OnChangeCurCtrl(TM::EObjType eType/* TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled */)
// {
// //     DbgTrace("CMainWnd::OnChangeCurCtrl");
//     TMUInt nFlag = MF_BYCOMMAND|MF_ENABLED;
//     TMUInt nFlag1 = MF_BYCOMMAND|MF_GRAYED;
//     switch (eType)
//     {
//     case TM::e_otWnd:// EM_DIALOG:
//     case TM::e_otWgt: // EM_WGT:
// //        if (!m_bIsControlsDlgVisibale)
// //            ShowControlsDlg(TMTrue);
//         nFlag1 = nFlag;
//         break;
// //     case EM_FRAME:
// //         if (!m_bIsFrameCtrlDlg)
// //             ShowFrameCtrlDlg(TMTrue);
// //         break;
//     }
// 
//     EnableMenuItem(m_hMenu,IDM_TOOL_TEST,nFlag1);
// 
// //     if (!(TMBool)lParam)
//     {
//         nFlag = MF_BYCOMMAND|MF_GRAYED;
//     }
//     EnableMenuItem(m_hMenu,IDM_CONTROL_ALIGN_BOTTOM,nFlag);
//     EnableMenuItem(m_hMenu,IDM_CONTROL_ALIGN_BOTTOM,nFlag);
//     EnableMenuItem(m_hMenu,IDM_CONTROL_ALIGN_LEFT,nFlag);
//     EnableMenuItem(m_hMenu,IDM_CONTROL_ALIGN_TOP,nFlag);
//     EnableMenuItem(m_hMenu,IDM_CONTROL_ALIGN_RIGHT,nFlag);
//     if (!m_bIsCtrlPropertyDlgVisibale)
//         ShowCtrlPropertyDlg(TMTrue);
// 
//     return 0;
// }


// TMResult CMainWnd::OnCloseDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
// {
//     DbgTrace("CMainWnd::OnDeleteDialog");
//     switch (lParam)
//     {
//     case DT_CONTROL:
//         m_bIsControlsDlgVisibale = TMFalse;
//         break;
//     case DT_PROPERTY:
//         m_bIsCtrlPropertyDlgVisibale = TMFalse;
//         break;
//     case DT_FRAME:
//         m_bIsFrameCtrlDlg = TMFalse;
//         break;
//     }
// 
//     return 0;
// }

VOID CMainWnd::ShowControlsDlg(TMBool bShow)
{
    Item(IDO_TUI_OBJ).ShowWindow(bShow);
//     if (m_ControlsDlg.IsWindow())
//     {
//         m_bIsControlsDlgVisibale = bShow;
//         m_ControlsDlg.ShowWindow(bShow ? SW_NORMAL : SW_HIDE);
//     }
}

void CMainWnd::SetWndTitle()
{
    TM::CString strTitle(TM_CS(MAINWNDNAME));
    
    if (!m_strFullName.IsEmpty())
    {
        strTitle += _T(" - ");
        strTitle += m_strFullName.GetName();
    }
    else
    {
        strTitle += _T(" - ");
        strTitle += _T("Untitled.xml");
    }

    if (m_curPanel.GetUndo().IsModified())
        strTitle += _T("*");

    IObjEdit* pObj = m_curPanel.ItemGetSelectedData(false);
    if (pObj)
    {
        TM::CString strTmp;
        strTmp.Format(L" - %s(%lld)", pObj->GetObjClassName().c_str(), pObj->GetId());
        strTitle += strTmp;
//         strTitle += _T(" - ");
//         strTitle += pObj->GetClassName();
    }

//     m_tuiWindow.SetWindowText(strTitle);
    SetWindowText(strTitle);
}

// TMBool CMainWnd::ProcessKeyDown(TMWParam wParam)
// {
//     switch (wParam)
//     {
//     case L'C':
//         if (GetKeyState(VK_CONTROL) & 0x8000)
//         {
//             _Module.GetMainWnd()->SendMessage(WM_COPY, 0, 0);
//             return true;
//         }
//         break;
//         
//     case L'V':
//         if (GetKeyState(VK_CONTROL) & 0x8000)
//         {
//             _Module.GetMainWnd()->SendMessage(WM_PASTE, 0, 0);
//             return true;
//         }
//         break;
//     }
// 
//     return false;
// }

void CMainWnd::OpenDlgXml(TMBool bMerge)
{
    DbgTrace("CMainWnd::OpenDlgXml");
    TMCStr lpcstrFilter = 
     _T("All Resource Files (*.rc;*.qrc)\0*.rc;*.qrc\0")
     _T("RC Files (*.rc)\0*.rc\0")
     _T("QT resouce Files (*.qrc)\0*.qrc\0")
     _T("XML Files (*.xml)\0*.xml\0")
     _T("All Files (*.*)\0*.*\0")
     _T("");

    TM::CFileOpenDialog dlg;
    int nRet = dlg.DoModal (GetHWND(), lpcstrFilter, 0, 0, 0,
        OFN_SHOWHELP | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST); 
    if (nRet == IDOK)
    {
//         m_undo.Clear();
        m_strFullName  = dlg.GetPathName();
//         m_strShortName = dlg.GetFileName();
//         m_ctrlTree.LoadExistentResource();
//         m_fnXml = LoadFromFile(GetProj(), m_strFullName, bMerge);
        CCycle tm;
        m_curPanel.LoadFile(m_strFullName, bMerge, true);
        GetPanel().Alert(L"load file %s success, use time %d ms", m_strFullName.c_str(), tm.ElapseMS());

//         m_undo.ClearModifyed();
        m_goto.Clear();
        SetWndTitle();
        SetMenuBarState (false);
    }
}

void CMainWnd::SetMenuBarState(TMBool bLoadLang)
{
    CMenuHandle mh (m_hMenu);// m_wndMB.GetMenu());
    CMenuHandle mp (mh.GetSubMenu(0));

    int n[2] = {MF_BYCOMMAND | MF_DISABLED | MF_GRAYED, MF_BYCOMMAND | MF_ENABLED};

    //mp.EnableMenuItem (IDM_FRAME_SAVE, n[!bLoadLang]);
//     mp.EnableMenuItem (IDM_FRAME_SAVEAS, n[!bLoadLang]);
    mp.EnableMenuItem (IDM_FRAME_LOADLANG, n[!bLoadLang]);
}



void CMainWnd::OpenDlgLang(TMBool bTest)
{
    TMCStr lpcstrFilter = 
        _T("XML Files (*.xml)\0*.xml\0")
        _T("All Files (*.*)\0*.*\0")
        _T("");

    if (m_curPanel.GetUndo().IsModified() && !bTest)
    {
        MessageBox (GetHWND(), _T("You need save file before load language/skin file"), _T("Warning"), MB_OK);
        return;
    }
    
    TM::CFileOpenDialog dlg;
    int nRet = dlg.DoModal(GetHWND(), lpcstrFilter); 
    if (nRet == IDOK)
    {
        // backup base object
//         m_fnLang = dlg.GetPathName();

        if (bTest)
        {
            GetProj()->TesterLanguage(dlg.GetPathName());
        }
        else
        {
            GetProj()->LoadLanguage(dlg.GetPathName());
//             TM::_tuiApp.LoadXml (HEDITOR, 0, dlg.GetPathName());
            
            // disable main menu::Save & Save As
            SetMenuBarState (true);
        }
    }
}

TMBool CMainWnd::SaveResource(ESaveMode eMode, int nCmd)
{
    if (IProj::GetFileVersion() > TUI_FILE_VER_CURRENT)
    {
        MessageBox(GetHWND(), L"you can't save a new file use old editor", L"error", MB_OK);
        return false;
    }

    if (nCmd != IDM_FRAME_SAVE || m_strFullName.IsEmpty())
    {
        DbgTrace("CMainWnd::SaveResource");
        TMCStr lpcstrFilter = 
            _T("All Resource Files (*.rc;*.qrc)\0*.rc;*.qrc\0")
            _T("RC Files (*.rc)\0*.rc\0")
            _T("QRC Files (*.qrc)\0*.qrc\0")
            _T("All Files (*.*)\0*.*\0")
            _T("");
        //     CFileDialog dlg(TMFalse, _T("rc"), _T("*.rc"), OFN_NOCHANGEDIR, 
        //         lpcstrFilter);
        TM::CFileSaveDialog dlg;
        int nRet = dlg.DoModal(GetHWND(), lpcstrFilter, _T("Save As"), 0, m_strFullName, OFN_NOCHANGEDIR);
        
        if (nRet == IDOK)
            m_strFullName  = dlg.GetPathName();
        else
            return false;
    }

    GetProj()->SaveFile(m_strFullName, eMode);
    m_curPanel.GetUndo().ClearModifyed();
    SetWndTitle();

    return true;
}

void CMainWnd::SaveDlgSkin(TMBool bLang)
{
    TMCStr lpcstrFilter = 
        _T("XML Files (*.xml)\0*.xml\0")
        _T("All Files (*.*)\0*.*\0")
        _T("");
    
    TM::CFileSaveDialog dlg;
    int nRet = dlg.DoModal (GetHWND(), lpcstrFilter); 
    if (nRet == IDOK)
    {
        if (bLang)
        {
            CDlgSaveLang dlgs(*this, dlg.GetPathName());
            dlgs.DoModal(*GetAPP(), GetHWND());
        }
        else
            GetProj()->SaveFile(dlg.GetPathName(), eSaveModeSkin);
    }
}


void CMainWnd::TestCurEditWindow()
{
//     IObjEdit* pData = GetCurrentObject();
//     if (!pData)
//         return;

    IObjEdit* p = m_curPanel.ItemGetSelectedData(true);
//    TM::CTuiWnd* p = (TM::CTuiWnd*)(pData->GetWindow());
    if (!p || p->GetType() != e_otWnd)
        return;

    int nID = (int)p->GetId(); // >GetObjectID();
    //SaveTestFile(strPath);
    
//    GetProj()->SaveFile(strPath, eSaveModeTest);

    TM::CString strPath = m_strFullName.GetPath(true);
    strPath +=  _T("test.tmp.xml");
    TesterLoad(GetProj(), GetHWND(), strPath, (int)nID);
}

// TMBool CMainWnd::SaveFile(TMCStr pFileName)
// {
//     GetProj()->SaveFile(pFileName, eSaveModeNormal);
//     m_curPanel.GetUndo().ClearModifyed();
// //     m_fnXml = fnXml;
// //     m_undo.ClearModifyed();
//     return true;
// }



////////////////////////////////////////////////////////////////////////////////
// CAddnewLanguageDlg
// class CNewlanguageDlg : public CDialogImpl<CNewlanguageDlg>
// {
// public:
//     CEdit   m_ctrlText;
//     TM ::CString    m_strText;
// 
// public:
//     enum { IDD = IDD_ADDLANGUAGE_DLG };
//     
//     BEGIN_MSG_MAP(CNewlanguageDlg)
//         MESSAGE_HANDLER (WM_INITDIALOG,    OnInitDialog);
//         COMMAND_ID_HANDLER (IDOK,          OnClose)
//         COMMAND_ID_HANDLER (IDCANCEL,      OnClose)
//     END_MSG_MAP()
// 
//     TMResult OnClose(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
//     {
//         int n = m_ctrlText.GetWindowText (m_strText.GetBuffer(CHAR_MAX), CHAR_MAX);
//         m_strText.SetLength(n);
//         EndDialog (wID);
//         return 0;
//     }
//     
//     TMResult OnInitDialog(TMUInt uMsg, TMWParam wParam, TMLParam lParam, TMBool& bHandled)
//     {
//         m_ctrlText = GetDlgItem(IDC_EDIT_LANGUAGE);
//         m_ctrlText.SetWindowText(m_strText.c_str());
//         bHandled =false;
//         return 0;
//     }
// };

void CMainWnd::SyncTxtEdit(TMBool bSyncText)
{
    TM::CTuiEdit wndEdit(Item(IDC_MEMO_TXT_EDIT));

    if (bSyncText)
    {
        IObjEdit* pNewMgr = m_curPanel.ItemGetSelectedData(true);
        TM::CString str;
        if (pNewMgr)
        {
            TM::CDomDocument doc;
            pNewMgr->SaveTo(doc);
            doc->transformNode(str);
        }
        wndEdit.SetWindowText(str);
    }

//    if (!g_curPanel)
//        return;
    IObjEdit* p = m_curPanel.ItemGetSelectedData(false);
    if (!p)
        return;
    TMUInt64 nid = p->GetId();
    if (!nid)
        return;
    TM::CString strTag;
    strTag.Format(TM_T("L%lld "), nid);
    TM::CString strTxt;
    wndEdit.GetWindowText(strTxt);
    int n = strTxt.Find(strTag);
    if (n <= 0)
        return;
    TM::CString strProp(m_dlgProperty.GetSelectedPropName());
    if (!strProp.IsEmpty())
    {
        strProp.Replace('.', 0);
        n = strTxt.Find(strProp, n);
    }
    wndEdit.SetSel(n, n + 5);
//     int n = strTxt.Find()
}

TMResult CMainWnd::OnShowProp(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TM::CTuiRect rc;
    m_dlgProperty.GetWindowRect(&rc);
    if (rc.Width() < 10)
    {
        rc.OffsetLeft(-240);
        m_dlgProperty.MoveWindow(&rc);
        m_dlgProperty.ShowWindow(true);
    }
    else
        m_dlgProperty.ShowWindow(!m_dlgProperty.IsWindowVisible());
    return 0;
}

TMResult CMainWnd::OnShowTree(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TM::CTuiWidget wnd(Item(IDO_TUI_OBJ));

    TM::CTuiRect rc;
    wnd.GetWindowRect(&rc);
    if (rc.Width() < 10)
    {
        rc.OffsetRight(240);
        wnd.MoveWindow(&rc);
        wnd.ShowWindow(true);
    }
    else
        wnd.ShowWindow(!wnd.IsWindowVisible());
    return 0;
}

TMResult CMainWnd::OnSelEndOK(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    TM::CTuiListBox lb(Item(IDC_LISTBOX_30191));
    int n = lb.GetCurSel();
    n = lb.GetItemData(n);
//     DbgAssert(n);
    m_cfgDbg.nBk = n;
    m_cfgDbg.ExecBk(*this);
    m_cfgDbg.SaveCfg();
    return 0;
}

TMResult CMainWnd::OnSpyDropDown(WORD wNotifyCode, WORD wID, TMHWnd hWndCtl, TMBool& bHandled)
{
    IProj::CModuleListS aList;
    GetProj()->GetModuleList(0, &aList);
    CTuiListBox lb(Item(IDC_LISTBOX_31114));
    lb.ResetContent();
    for (IProj::CModuleListS::recorder rec(aList); rec; ++rec)
        lb.AddString(rec.get_data());

    return 0;
}


// void CMainWnd::NewEff()
// {
//     IObjEdit* p = m_curPanel.GetObjMgr()->GetSelectedItemData(false);
//     if (!p)
//         return;
// 
//     TM::EObjType eType = p->GetType();
//     if (eType != e_otWnd && eType != e_otWgt)
//         return;
// 
//     m_dlgProperty.RemoveObj(p);
//     p->AddNewEff();
//     m_dlgProperty.SetObj(p);
// }



TMResult CMainWnd::OnContextMenu(int idCtrl, TM::TUIMSG* pMsg, TMBool& bHandled)
{
    TMResult nRet = m_curPanel.OnContextMenu();
    if (nRet)
        OnCommand(WM_COMMAND, nRet, 0, bHandled);
//     if (nRet)
//         ShowCtrlPropertyDlg(TMTrue);
    return nRet;
}

IProj* CMainWnd::GetProj()
{
    return m_curPanel.GetProj();
}

// TMBool CMainWnd::IsParentSelected(TM::CTVItem ti)
// {
//     DbgAssert(ti);
//     ti = ti.GetParent();
//     if (!ti)
//         return false;
//     if (ti.IsSelected())
//         return true;
//     return IsParentSelected(ti);
// }

void CMainWnd::ShowCtrlPropertyDlg(TMBool bShow)
{
    m_bIsCtrlPropertyDlgVisibale = bShow;
    m_dlgProperty.ShowWindow(bShow ? SW_SHOW :SW_HIDE);
}

// void CMainWnd::SelectLanguage(TMHMenu hMenu,TMUInt uIdItem)
// {
// #pragma message(FILE_AND_LINE "add the codes: get the selected language.")
// //     TM::CString strMenu;
// //     GetMenuString(hMenu, uIdItem, strMenu.GetBufferEx(CHAR_MAX),CHAR_MAX,MF_BYPOSITION);     // menu item identifier  
// }

// void CMainWnd::BindFile()
// {
// //     IObjEdit* pData = m_curPanel.GetObjMgr()->GetSelectedItemData(false);
// //     if (pData)
// //     {
// //         TMCStr lpcstrFilter = 
// //             _T("*.c;*.cpp;*.cxx;*.tli;*.h;*.tlh;*.inl;*.rc\0*.c;*.cpp;*.cxx;*.tli;*.h;*.tlh;*.inl;*.r\0")
// //             _T("All Files (*.*)\0*.*\0")
// //             _T("");
// //         TM::CFileOpenDialog dlg;
// //         int nRet = dlg.DoModal(GetHWND(), lpcstrFilter);
// //         if (nRet == IDOK)
// //         {
// //             TM::CString fn(dlg.GetFileName());
// //             pData->m_aFile.insert(fn);
// //             CTVItem itm((TM::HTVITEM) pData->m_hItemTree);
// //             itm.SetHeight(itm.GetDefaultHeight() * (1 + pData->m_aFile.size()));
// //         }
// //     }
// }
// 
// void CMainWnd::UnbindFile()
// {
// //     IObjEdit* pData = m_curPanel.GetObjMgr()->GetSelectedItemData(false);
// //     if (ti && (pData = ti.GetData()))
// //     {
// // //         pData->UnbindFile(0);
// //     }
// }

void CMainWnd::SetMode(EPlugInMode eMode)
{
    if (m_eMode != ePlugInModeUnknown)
        return;

    m_eMode = eMode;

    IProj* proj = m_curPanel._CreateProj(this, m_eMode);

    {
        CDlgControls::CPtr ptr;
        ptr.CreateInstance()->SubclassWindow(Item(IDW_TUI_CONTROLS_VIEW));
        CDlgControls::Init(*ptr, proj);
    }
    
    m_wndSearchObj.SetTarget(m_curPanel.GetTree());

    SetWndTitle();

    if (m_eMode == ePlugInModeSpy)
    {
        Item(IDC_TOOLBAR_EDITOR).ShowWindow(false);
        Item(IDC_TOOLBAR_SPY).ShowWindow(true);
//        CDlgSpyFocus::SetShowFocus(*this, true);//Item(IDM_SPY_FOCUS).m_hWnd);
        m_cfgDbg.bShowHideWindow = false;
        m_cfgDbg.InitMenu(m_hMenu);
        proj->ModifyUser(0, m_dlgSetUser.strUser);
        ShowWindow(true);
//         GetProj()->TrackFocusObject(true);
        m_cfgDbg.CheckPauseApp();
    }
    else
    {
        m_cfgDbg.bShowHideWindow = true;
        DeleteMenu(m_hMenu, 6, MF_BYPOSITION);
        Item(IDC_TOOLBAR_EDITOR).ShowWindow(true);
        Item(IDC_TOOLBAR_SPY).ShowWindow(false);
        m_cfgDbg.InitMenu(m_hMenu);
        ShowWindow(true);

        m_cfgDbg.CheckPauseApp();

        if (m_eMode == ePlugInModeEditor && m_dlgSetUser.strUser.IsEmpty())
            m_dlgSetUser.DoModal(*GetAPP(), GetHWND());
        GetProj()->ModifyUser(0, m_dlgSetUser.strUser);

        this->DoDialogBox(0);
    }
}


// CUndo& CMainModule::GetUndo()
// {
//     return m_pMainWnd->GetUndo();
// }


class CMainInstance
//     : public TM::CTuiMsgFilter
{
public:
    CMainWnd::CPtr m_ptrWnd;

    ~CMainInstance()
    {
        int x = 0;
    }

    void Init()
    {
        HMODULE hInstance = _Module.m_hInstResource;
        TM::CTuiApp* app = AppGetEditor();
        app->Init(0);
        app->LoadXml(hInstance, TUI_RES_XML_NAME);
//         app->AddMessageFilter(this);
        
        app->SetAnimationButton(true);
        app->SetAnimationScroll(true);
        app->SetAnimationSysBut(true);
        app->SetShowHintOnEllipsis(true);

        m_ptrWnd.CreateInstance()->Create(*app, (HWND)0, 0, IDD_TUI_MANAGE_10001, hInstance);

//         m_ptrWnd->m_cfgDbg.ExecHC();
        m_ptrWnd->m_cfgDbg.ExecDark();
        m_ptrWnd->m_cfgDbg.ExecBk(*m_ptrWnd);
    }

    CMainWnd* operator -> () { DbgAssert(m_ptrWnd); return m_ptrWnd; }

    // CTuiMsgFilter
//     virtual TMBool PreTranslateMessage(TUIMSG* pMsg, TMResult& lResult) tm_override
//     {
//         if (CM_APPMESSAGE == pMsg->message && WM_GETOBJECT == ((TUIMSG*)pMsg->lParam)->message)
//             return true;
//         return false;
//     }
// 
//     void EnableProxy()
//     {
//         TM::CTuiApp* app = AppGetEditor();
//         app->RemoveMessageFilter(this);
//     }

};

CMainInstance s_instMain;


TM_BGN_NAMESPACE


void tuiPlugInCreate(TMBool bSpyMode)
{
    static TMBool bCreated = false;
    if (bCreated)
        return;

    bCreated = true;

//     if (s_instMain.m_ptrWnd)
//         return;

    DbgAssert(!s_instMain.m_ptrWnd.IsWindow());
    s_instMain.Init();
//     s_instMain.EnableProxy();
    s_instMain.m_ptrWnd->SetMode(bSpyMode ? ePlugInModeSpy : ePlugInModeEditor);
}



///////////////////////////////////////////////////////////////////////////////
// edit function
TMBool Edit_IsShowHide(TMBool bEdit)
{
    return bEdit && s_instMain->m_cfgDbg.bShowHideWindow;
}

void Edit_SaveTexture(TMBool bEdit, HTUI hWnd, GXPTexture texture, int nAlpha)
{
    if (!bEdit || !s_instMain.m_ptrWnd.IsWindow() || !s_instMain->m_cfgDbg.bLogBkBuffer)
        return;
    SYSTEMTIME stm;
    GetLocalTime(&stm);
    CString str;
    str.Format(L"c:\\temp\\TMDui Analysis %4d-%02d-%02d %02d.%02d.%02d.%03d [%d] %03d.png",
        stm.wYear, stm.wMonth, stm.wDay,
        stm.wHour, stm.wMinute, stm.wSecond, stm.wMilliseconds,
        CTuiWidget(hWnd).GetDlgCtrlObjectID(), nAlpha);
    texture->SaveFile(str, true);
}


static void _DBG_SaveWndAndBk(int nObjid, HWND hWnd, CTexture& texture)
{
    static int  i = 0;

}

// msg: save screen mode
// msg: save flip mode
void Edit_SaveFlip(TMBool bEdit, int nObjid, HWND hWnd, GXPTexture texture, const TMRect& rcFlip, UINT* msg)
{
    if (!bEdit || !s_instMain.m_ptrWnd.IsWindow())
        return;
    if (!msg)
    {
        if (!s_instMain->m_cfgDbg.bLogFlip)
            return;
    }
    else if (!s_instMain->m_cfgDbg.bLogMsg)
            return;

    SYSTEMTIME stm;
    GetLocalTime(&stm);

    CRect rcW;
    ::GetWindowRect(hWnd, &rcW);
    
#define FMT_FN \
    L"c:\\temp\\TMDui %4d-%02d-%02d %02d.%02d.%02d.%03d [%d]" \
    L" rcWin-" TM_L(TM_FMT_RCI_STR) \
    L" rcUpdate-" TM_L(TM_FMT_RCI_STR)

    // save window
    {
        CImage img;
        img.CreateIndirect(rcW.Width(), rcW.Height(), eGXFormat32A);
        {
            CImageDC dc2(img);
            DWORD dwRop = SRCCOPY;
            if (CWindow(hWnd).GetExStyle() & WS_EX_LAYOUTRTL)
                SetLayout(dc2, LAYOUT_RTL);
            ::BitBlt(dc2, 0, 0, rcW.Width(), rcW.Height(), CWindowDC(hWnd), 0, 0, SRCCOPY | CAPTUREBLT);
        }
        CString str;
        str.Format(FMT_FN L" window.png",
            stm.wYear, stm.wMonth, stm.wDay,
            stm.wHour, stm.wMinute, stm.wSecond, stm.wMilliseconds,
            nObjid,
            TM_FMT_RCI_VAL(rcW),
            TM_FMT_RCI_VAL(rcFlip));
        ImageSaveFile(img, str, false);
    }

    // save screen
    {
        CImage img;
        img.CreateIndirect(rcW.Width(), rcW.Height(), eGXFormat32A);
        {
            CImageDC dc2(img);
            ::BitBlt(dc2, 0, 0, rcW.Width(), rcW.Height(), CWindowDC(0), rcW.Left(), rcW.Top(), SRCCOPY | CAPTUREBLT);
        }
        CString str;
        str.Format(FMT_FN L" screen.png",
            stm.wYear, stm.wMonth, stm.wDay,
            stm.wHour, stm.wMinute, stm.wSecond, stm.wMilliseconds,
            nObjid,
            TM_FMT_RCI_VAL(rcW),
            TM_FMT_RCI_VAL(rcFlip));
        ImageSaveFile(img, str, false);
    }
    
    // save texture
    {
        CString str;
        str.Format(FMT_FN L" bk_gnd.png",
            stm.wYear, stm.wMonth, stm.wDay,
            stm.wHour, stm.wMinute, stm.wSecond, stm.wMilliseconds,
            nObjid,
            TM_FMT_RCI_VAL(rcW),
            TM_FMT_RCI_VAL(rcFlip));
        texture->SaveFile(str, true);
    }
}

TMBool Edit_IsKeepMenuPop(TMBool bEdit)
{
    return bEdit && s_instMain->m_cfgDbg.m_bKeepMenuPop;
}

TMBool Edit_IsDisableGhost()
{
    if (s_instMain.m_ptrWnd)
        return s_instMain->m_cfgDbg.bDisableGhost;
    return false;
}

TMBool Edit_IsPlugInModeEditor()
{
    if (s_instMain.m_ptrWnd)
        return s_instMain.m_ptrWnd->GetMode() == ePlugInModeEditor;
    else
        return true;
};


TM_END_NAMESPACE