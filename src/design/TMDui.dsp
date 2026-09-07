# Microsoft Developer Studio Project File - Name="TMDui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=TMDui - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TMDui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TMDui.mak" CFG="TMDui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TMDui - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TMDui - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TMDui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "c:\temp\TMDui\debug"
# PROP Intermediate_Dir "c:\temp\TMDui\debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "../../src" /I "../../3rd/wtl" /I "../../3rd" /I "../../3rd/win" /I "../design/res" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "UNICODE" /D "TUI_EDIT_MODE" /D "TM_EXPORTS" /D "TUI_EXPORTS" /Yu"stdafx.h" /FD /GZ /Zm600 /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /i "res\reswhite" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Shlwapi.lib /nologo /subsystem:windows /dll /profile /debug /machine:I386 /out:"c:\temp\TMDuiDesign\Debug/TMDui.dll"

!ELSEIF  "$(CFG)" == "TMDui - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TMDui___Win32_Release"
# PROP BASE Intermediate_Dir "TMDui___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "c:\temp\TMDui\Release"
# PROP Intermediate_Dir "c:\temp\TMDui\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE CPP /nologo /MT /W3 /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /I "../../src" /I "../../3rd/wtl" /I "../../3rd" /I "../../3rd/win" /I "../design/res" /D "NDEBUG" /D "TM_DBG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "UNICODE" /D "TUI_EDIT_MODE" /D "TM_EXPORTS" /D "TUI_EXPORTS" /Yu"stdafx.h" /FD /Zm600 /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /i "res/reswhite" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Shlwapi.lib /nologo /subsystem:windows /dll /map /machine:I386 /out:"../../bin/uidesign/TMDui.dll"
# SUBTRACT LINK32 /profile /incremental:yes /debug

!ENDIF 

# Begin Target

# Name "TMDui - Win32 Debug"
# Name "TMDui - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\pch\tm.cpp
# End Source File
# Begin Source File

SOURCE=.\res\Dui.rc
# End Source File
# Begin Source File

SOURCE=..\..\pch\gx.cpp
# End Source File
# Begin Source File

SOURCE=.\Panel\panel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\pch\stdafx.cpp
# ADD CPP /Yc
# End Source File
# Begin Source File

SOURCE=..\..\pch\tmdui.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Design.h
# End Source File
# Begin Source File

SOURCE=.\Panel\panel.h
# End Source File
# Begin Source File

SOURCE=..\..\pch\stdafx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\about.gif
# End Source File
# Begin Source File

SOURCE=.\res\bk_1pixel.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bk_floral.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bk_grid.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bk_slash.bmp
# End Source File
# Begin Source File

SOURCE=.\res\controls.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cross.gif
# End Source File
# Begin Source File

SOURCE=..\graphic\hlsl\d11.hlsl
# End Source File
# Begin Source File

SOURCE=..\graphic\hlsl\d9.hlsl
# End Source File
# Begin Source File

SOURCE=.\res\ResEditor.ico
# End Source File
# Begin Source File

SOURCE=.\res\select.bmp
# End Source File
# End Group
# Begin Group "Panel"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Panel\CtrlEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\Panel\CtrlEditor.h
# End Source File
# Begin Source File

SOURCE=.\Panel\EditStream.cpp
# End Source File
# Begin Source File

SOURCE=.\Panel\EditStream.h
# End Source File
# Begin Source File

SOURCE=.\Panel\FactoryData.h
# End Source File
# Begin Source File

SOURCE=.\Panel\FileOpt.cpp
# End Source File
# Begin Source File

SOURCE=.\Panel\FileOpt.h
# End Source File
# Begin Source File

SOURCE=.\Panel\IdStream.cpp
# End Source File
# Begin Source File

SOURCE=.\Panel\IdStream.h
# End Source File
# Begin Source File

SOURCE=.\Panel\PropItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Panel\PropItem.h
# End Source File
# Begin Source File

SOURCE=.\Panel\ResFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Panel\ResFile.h
# End Source File
# Begin Source File

SOURCE=.\Panel\resource.h
# End Source File
# Begin Source File

SOURCE=.\Panel\ResourceID.cpp
# End Source File
# Begin Source File

SOURCE=.\Panel\ResourceID.h
# End Source File
# Begin Source File

SOURCE=.\Panel\SaveStream.cpp
# End Source File
# Begin Source File

SOURCE=.\Panel\SaveStream.h
# End Source File
# Begin Source File

SOURCE=.\Panel\version.cpp
# End Source File
# Begin Source File

SOURCE=.\Panel\WndPad.cpp
# End Source File
# Begin Source File

SOURCE=.\Panel\WndPad.h
# End Source File
# Begin Source File

SOURCE=.\Panel\WndSpy.cpp
# End Source File
# Begin Source File

SOURCE=.\Panel\WndSpy.h
# End Source File
# End Group
# Begin Group "Frame"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Frame\demodlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\demodlg.h
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgAbout.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgAbout.h
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgAllocGpuMemory.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgAllocGpuMemory.h
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgAnimateWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgAnimateWindow.h
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgAutoTranslate.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgAutoTranslate.h
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgBlockInject.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgBlockInject.h
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgCheckKeyword.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgCheckKeyword.h
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgDllAnalysis.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgDllAnalysis.h
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgGDIDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgGDIDraw.h
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgImportWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgImportWindow.h
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgMemAnalysis.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgMemAnalysis.h
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgSaveLang.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgSaveLang.h
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgSetUser.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgSetUser.h
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgSpyFocus.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgSpyFocus.h
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgSpyMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgSpyMsg.h
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgSpyMsg_Event.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgSpyMsg_Event.h
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgStressTesting.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgStressTesting.h
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgSvgEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgSvgEdit.h
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgSysColor.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgSysColor.h
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgTesterSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgTesterSetting.h
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgTrackWin.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\DlgTrackWin.h
# End Source File
# Begin Source File

SOURCE=.\Frame\GoTo.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\GoTo.h
# End Source File
# Begin Source File

SOURCE=.\Frame\MainWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\MainWnd.h
# End Source File
# Begin Source File

SOURCE=.\Frame\MemHook.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\MemHook.h
# End Source File
# Begin Source File

SOURCE=.\Frame\ObjectTreeView.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\ObjectTreeView.h
# End Source File
# Begin Source File

SOURCE=.\Frame\ProjMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\ProjMgr.h
# End Source File
# Begin Source File

SOURCE=.\Frame\PropertyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\PropertyDlg.h
# End Source File
# Begin Source File

SOURCE=.\Frame\ResetIdDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\ResetIdDlg.h
# End Source File
# Begin Source File

SOURCE=.\Frame\Tester.h
# End Source File
# Begin Source File

SOURCE=.\Frame\Tools.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\Tools.h
# End Source File
# Begin Source File

SOURCE=.\Frame\TVRender.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\TVRender.h
# End Source File
# Begin Source File

SOURCE=.\Frame\Undo.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\Undo.h
# End Source File
# Begin Source File

SOURCE=.\Frame\Wizard.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\Wizard.h
# End Source File
# Begin Source File

SOURCE=.\Frame\WndAcc.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\WndAcc.h
# End Source File
# Begin Source File

SOURCE=.\Frame\WndControlsView.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\WndControlsView.h
# End Source File
# Begin Source File

SOURCE=.\Frame\WndMemRecorder.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\WndMemRecorder.h
# End Source File
# Begin Source File

SOURCE=.\Frame\WndObjEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\WndObjEdit.h
# End Source File
# Begin Source File

SOURCE=.\Frame\WndPerformance.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\WndPerformance.h
# End Source File
# Begin Source File

SOURCE=.\Frame\WndSelector.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\WndSelector.h
# End Source File
# Begin Source File

SOURCE=.\Frame\WndThumb.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\WndThumb.h
# End Source File
# Begin Source File

SOURCE=.\Frame\WndTVSearch.cpp
# End Source File
# Begin Source File

SOURCE=.\Frame\WndTVSearch.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\bk_noise.png
# End Source File
# Begin Source File

SOURCE=.\res\bkfloat.png
# End Source File
# Begin Source File

SOURCE=.\res\border1p.png
# End Source File
# Begin Source File

SOURCE=.\res\butclose.svg
# End Source File
# Begin Source File

SOURCE=.\res\butmax.svg
# End Source File
# Begin Source File

SOURCE=.\res\butmin.svg
# End Source File
# Begin Source File

SOURCE=.\res\button2.png
# End Source File
# Begin Source File

SOURCE=.\res\Button_Commmon.png
# End Source File
# Begin Source File

SOURCE=.\res\device.png
# End Source File
# Begin Source File

SOURCE=.\res\easing.png
# End Source File
# Begin Source File

SOURCE=.\res\erase.png
# End Source File
# Begin Source File

SOURCE=.\res\goto.png
# End Source File
# Begin Source File

SOURCE=..\..\doc\help.htm
# End Source File
# Begin Source File

SOURCE=.\res\help.htm
# End Source File
# Begin Source File

SOURCE=..\doc\help.htm
# End Source File
# Begin Source File

SOURCE=..\..\doc\help_controls.htm
# End Source File
# Begin Source File

SOURCE=.\res\help_controls.htm
# End Source File
# Begin Source File

SOURCE=..\doc\help_controls.htm
# End Source File
# Begin Source File

SOURCE=..\..\doc\help_cross.htm
# End Source File
# Begin Source File

SOURCE=.\res\help_cross.htm
# End Source File
# Begin Source File

SOURCE=..\doc\help_cross.htm
# End Source File
# Begin Source File

SOURCE=.\res\iborder.png
# End Source File
# Begin Source File

SOURCE=.\res\next.png
# End Source File
# Begin Source File

SOURCE=.\res\open.png
# End Source File
# Begin Source File

SOURCE=.\res\prev.png
# End Source File
# Begin Source File

SOURCE=.\res\r2l.png
# End Source File
# Begin Source File

SOURCE=.\res\save.png
# End Source File
# Begin Source File

SOURCE=.\res\shadow.png
# End Source File
# Begin Source File

SOURCE=.\res\spy_break.png
# End Source File
# Begin Source File

SOURCE=.\res\spy_clear.png
# End Source File
# Begin Source File

SOURCE=.\res\spy_copy.png
# End Source File
# Begin Source File

SOURCE=.\res\spy_delsel.png
# End Source File
# Begin Source File

SOURCE=.\res\spy_message.png
# End Source File
# Begin Source File

SOURCE=.\res\spy_pause.png
# End Source File
# Begin Source File

SOURCE=.\res\spy_refresh.png
# End Source File
# Begin Source File

SOURCE=.\res\spy_remote.png
# End Source File
# Begin Source File

SOURCE=.\res\T_Dlg.cpp.Templet
# End Source File
# Begin Source File

SOURCE=.\res\T_Dlg.h.Templet
# End Source File
# Begin Source File

SOURCE=.\res\T_global.h.Templet
# End Source File
# Begin Source File

SOURCE=.\res\T_prjdll.cpp.Templet
# End Source File
# Begin Source File

SOURCE=.\res\T_prjdll.dsp.Templet
# End Source File
# Begin Source File

SOURCE=.\res\T_prjdll.pro.Templet
# End Source File
# Begin Source File

SOURCE=.\res\T_prjexe.cpp.Templet
# End Source File
# Begin Source File

SOURCE=.\res\T_prjexe.dsp.Templet
# End Source File
# Begin Source File

SOURCE=.\res\T_prjexe.pro.Templet
# End Source File
# Begin Source File

SOURCE=.\res\T_rc.qrc.Templet
# End Source File
# Begin Source File

SOURCE=.\res\T_rc.rc.Templet
# End Source File
# Begin Source File

SOURCE=.\res\T_StdAfx.cpp.Templet
# End Source File
# Begin Source File

SOURCE=.\res\T_StdAfx.h.Templet
# End Source File
# Begin Source File

SOURCE=.\res\target.PNG
# End Source File
# Begin Source File

SOURCE=.\res\test.png
# End Source File
# Begin Source File

SOURCE=.\res\test_settings.png
# End Source File
# Begin Source File

SOURCE=.\res\txt_down.png
# End Source File
# Begin Source File

SOURCE=.\res\txt_up.png
# End Source File
# Begin Source File

SOURCE=.\res\txt_wordbreak.png
# End Source File
# Begin Source File

SOURCE=.\res\txtdev.png
# End Source File
# Begin Source File

SOURCE=.\res\vscroll.png
# End Source File
# Begin Source File

SOURCE=.\res\tuires.xml
# End Source File
# Begin Source File

SOURCE=.\res\zoomin.png
# End Source File
# Begin Source File

SOURCE=.\res\zoomout.png
# End Source File
# End Target
# End Project
