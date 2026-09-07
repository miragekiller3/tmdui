/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2024.5.15
 *
 *  Copyright (C) 2024 miragekiller
 */

#pragma once

#ifndef JAWSHACK_H
#define JAWSHACK_H


// #define s_cClassDlg L"Chrome"
// #define s_cAppName  s_cClassDlg

//#define s_cClassDlg L"MozillaDialogClass"
//#define s_cClassDlg L"MozillaContentWindowClass"
//#define s_cClassDlg L"MozillaWindowClass"
#define s_cClassDlg L"Mozilla"
//#define s_cClassDlg L"Chrome_WidgetWin_1"
//#define s_cClassDlg L"Chrome_"
//#define s_cClassDlg L"Chrome"
// 
//#define s_cClassDlg L"firefox"
//#define s_cClassDlg L"Qt5QWindowIcon"
//#define s_cClassDlg L"Qt5152QWindowIcon"
//#define s_cClassDlg L"QPopup"
//#define s_cClassDlg L"OperaWindowClass"
//#define s_cClassDlg L"#32770" // dialog box
//#define s_cClassDlg L"#32771" // task bar
//#define s_cClassDlg L"Internet Explorer_Server"
//#define s_cClassDlg L"Listbox"
//#define s_cClassDlg L"DirectUIHWND"
//#define s_cClassDlg L"MDIClient"
//#define s_cClassDlg L"#32769" // desktop
//#define s_cClassDlg L"ScrollBar"
//#define s_cClassDlg L"ApplicationFrameWindow"
//#define s_cClassDlg L"Windows.UI.Core."
//#define s_cClassDlg L"NetUIHWND"
//#define s_cClassDlg L"VsCompletorPane"
//#define s_cClassDlg L"WebViewWindowClass"
//#define s_cClassDlg L"iTunesWebViewControl"
//#define s_cClassDlg L"msctls_trackbar32"







//#define s_cAppName  s_cClassDlg
//#define s_cAppName      L"firefox"
//#define s_cAppName      L"Firefox"
//#define s_cAppName  L"LibreOffice"
//#define s_cAppName      L"WindowsForms10"
//#define s_cAppName      L"Ribbon"

//#define s_cAppName  L"browser" // unused
//#define s_cAppName  L"Desktop" // unused
#define s_cAppName  L"Chrome" // same as firefox
//#define s_cAppName  L"DV2ControlHost" // unused
//#define s_cAppName_LibreOffice  L"LibreOffice"
//#define s_cAppName_LibreOffice  L"Ribbon"  // slider unused




TM_BGN_NAMESPACE

BOOL JawsHookInstall();
extern BOOL g_bDisableJawsHook;

TM_END_NAMESPACE



#endif //JAWSHACK_H
