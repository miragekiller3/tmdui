# Microsoft Developer Studio Project File - Name="TMDuiAcc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=TMDuiAcc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TMDuiAcc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TMDuiAcc.mak" CFG="TMDuiAcc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TMDuiAcc - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TMDuiAcc - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TMDuiAcc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "c:\temp\TMDuiAcc\Release"
# PROP Intermediate_Dir "c:\temp\TMDuiAcc\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TMDuiAcc_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /I "../../src" /I "../../3rd/wtl" /I "../../3rd" /I "../../3rd/win" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /D "TMDuiAcc_EXPORTS" /Yu"stdafx.h" /FD /Zm400 /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../../bin/uidesign/TMDuiAcc.dll"
# SUBTRACT LINK32 /profile /incremental:yes /debug

!ELSEIF  "$(CFG)" == "TMDuiAcc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "c:\temp\TMDuiAcc\Debug"
# PROP Intermediate_Dir "c:\temp\TMDuiAcc\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TMDuiAcc_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /I "../../src" /I "../../3rd/wtl" /I "../../3rd" /I "../../3rd/win" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /D "_USRDLL" /D "TMDuiAcc_EXPORTS" /Yu"stdafx.h" /FD /GZ /Zm400 /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /profile /debug /machine:I386 /out:"c:\temp\TMDuiDesign\Debug/TMDuiAcc.dll"

!ENDIF 

# Begin Target

# Name "TMDuiAcc - Win32 Release"
# Name "TMDuiAcc - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\accCheckBox.cpp
# End Source File
# Begin Source File

SOURCE=.\accComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\accDummy.cpp
# End Source File
# Begin Source File

SOURCE=.\accEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\accListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\accMenuBar.cpp
# End Source File
# Begin Source File

SOURCE=.\accMenuItem.cpp
# End Source File
# Begin Source File

SOURCE=.\accMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\accProgressBar.cpp
# End Source File
# Begin Source File

SOURCE=.\accPushButton.cpp
# End Source File
# Begin Source File

SOURCE=.\accRadioButton.cpp
# End Source File
# Begin Source File

SOURCE=.\accScrollBar.cpp
# End Source File
# Begin Source File

SOURCE=.\accSpeak.cpp
# End Source File
# Begin Source File

SOURCE=.\accSpin.cpp
# End Source File
# Begin Source File

SOURCE=.\accSpin.h
# End Source File
# Begin Source File

SOURCE=.\accSplitter.cpp
# End Source File
# Begin Source File

SOURCE=.\accStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\accTabButton.cpp
# End Source File
# Begin Source File

SOURCE=.\accTreeView.cpp
# End Source File
# Begin Source File

SOURCE=.\accWidget.cpp
# End Source File
# Begin Source File

SOURCE=.\Hash.cpp
# End Source File
# Begin Source File

SOURCE=.\JawsHook.cpp
# End Source File
# Begin Source File

SOURCE=.\Proxy.cpp
# End Source File
# Begin Source File

SOURCE=.\Speak.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc
# End Source File
# Begin Source File

SOURCE=.\TMDuiAcc.cpp
# End Source File
# Begin Source File

SOURCE=.\TMDuiAcc.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\accCheckBox.h
# End Source File
# Begin Source File

SOURCE=.\accComboBox.h
# End Source File
# Begin Source File

SOURCE=.\accDummy.h
# End Source File
# Begin Source File

SOURCE=.\accEdit.h
# End Source File
# Begin Source File

SOURCE=.\accListBox.h
# End Source File
# Begin Source File

SOURCE=.\accMenuBar.h
# End Source File
# Begin Source File

SOURCE=.\accMenuItem.h
# End Source File
# Begin Source File

SOURCE=.\accMgr.h
# End Source File
# Begin Source File

SOURCE=.\accProgressBar.h
# End Source File
# Begin Source File

SOURCE=.\accPushButton.h
# End Source File
# Begin Source File

SOURCE=.\accRadioButton.h
# End Source File
# Begin Source File

SOURCE=.\accScrollBar.h
# End Source File
# Begin Source File

SOURCE=.\AccSpeak.h
# End Source File
# Begin Source File

SOURCE=.\accSplitter.h
# End Source File
# Begin Source File

SOURCE=.\accStatic.h
# End Source File
# Begin Source File

SOURCE=.\accTabButton.h
# End Source File
# Begin Source File

SOURCE=.\accTreeView.h
# End Source File
# Begin Source File

SOURCE=.\accWidget.h
# End Source File
# Begin Source File

SOURCE=.\Hash.h
# End Source File
# Begin Source File

SOURCE=.\Helper.h
# End Source File
# Begin Source File

SOURCE=.\JawsHook.h
# End Source File
# Begin Source File

SOURCE=.\Proxy.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Speak.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Proxy.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
