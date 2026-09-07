@ECHO OFF&PUSHD %~DP0 &TITLE setup tool set 
rem mode con cols=45 lines=30
color 2F


:Menu
rem Cls
@ echo=
@ echo ***********************************************************
@ echo=select tool set:
@ echo=
@ echo=  0. codeblocks mingw win64
@ echo=  1. vs2008 win32
@ echo=  2. vs2008 win64
@ echo=  3. vs2017 win32
@ echo=  4. vs2017 win64
@ echo=  5. vs2019 win32
@ echo=  6. vs2019 win64
@ echo=  7. vs2022 win32
@ echo=  8. vs2022 win64
@ echo=  9. vs2026 win32
@ echo=  a. vs2026 win64
@ echo= 
set /p xj=enter the number and press Enter: 

@echo=
set TOOLSET=
set BUILDER=

if /i "%xj%"=="0" goto codeblocks_mingw_win64
if /i "%xj%"=="1" goto vs2008_w32
if /i "%xj%"=="2" goto vs2008_w64
if /i "%xj%"=="3" goto vs2017_w32
if /i "%xj%"=="4" goto vs2017_w64
if /i "%xj%"=="5" goto vs2019_w32
if /i "%xj%"=="6" goto vs2019_w64
if /i "%xj%"=="7" goto vs2022_w32
if /i "%xj%"=="8" goto vs2022_w64
if /i "%xj%"=="9" goto vs2026_w32
if /i "%xj%"=="a" goto vs2026_w64
@ echo=
echo  The selection is invalid... Please re-enter...
ping -n 2 127.0.0.1>nul
goto menu

:codeblocks_mingw_win64
set TOOLSET=-G "CodeBlocks - MinGW Makefiles"
set BUILDER=CB_MinGW_w64
@echo=
@echo **********************  FBI WARNING  **********************
@echo if get error "can't find libintl-8.dll"
@echo you need do this:
@echo -^> open path_codeblocks.txt
@echo -^> input MinGW bin path like "C:\Program Files\CodeBlocks\MinGW\bin"
@echo -^> save and close
set CB_PATH=
set /p CB_PATH=<./path_codeblocks.txt
set PATH=%PATH%;%CB_PATH%
@echo CB_PATH:%CB_PATH%
@echo=
goto end


:vs2008_w32
set TOOLSET=-G "Visual Studio 9 2008"
set BUILDER=vs2008_w32
goto end


:vs2008_w64
set TOOLSET=-G "Visual Studio 9 2008 Win64"
set BUILDER=vs2008_w64
goto end


:vs2017_w32
set TOOLSET=-G "Visual Studio 15 2017"
set BUILDER=vs2017_w32
goto end


:vs2017_w64
set TOOLSET=-G "Visual Studio 15 2017 Win64"
set BUILDER=vs2017_w64
goto end


:vs2019_w32
set TOOLSET=-G "Visual Studio 16 2019" -A Win32
set BUILDER=vs2019_w32
goto end


:vs2019_w64
set TOOLSET=-G "Visual Studio 16 2019" -A x64
set BUILDER=vs2019_w64
goto end


:vs2022_w32
set TOOLSET=-G "Visual Studio 17 2022" -A Win32
set BUILDER=vs2022_w32
goto end


:vs2022_w64
set TOOLSET=-G "Visual Studio 17 2022" -A x64
set BUILDER=vs2022_w64
goto end


:vs2026_w32
set TOOLSET=-G "Visual Studio 18 2026" -A Win32
set BUILDER=vs2026_w32
goto end


:vs2026_w64
set TOOLSET=-G "Visual Studio 18 2026" -A x64
set BUILDER=vs2026_w64
goto end


:end
@echo ***********************************************************
@echo * get toolset
rem set TOOLSET=
rem set /p TOOLSET=<%TEMP%\toolset.tmp
@echo TOOLSET:%TOOLSET%
@echo=

@echo ***********************************************************
@echo * get build platform
rem set BUILDER=
rem set /p BUILDER=<%TEMP%\builder.tmp
@echo BUILDER:%BUILDER%
@echo=

@echo ***********************************************************
@echo * get current dir and project dir
set CUR_DIR=%~dp0
set PRJ_DIR=%CUR_DIR%..\..\TMDui_builds\%BUILDER%\%PRJ_NAME%
@echo CUR_DIR:%CUR_DIR%
@echo PRJ_DIR:%PRJ_DIR%
@echo=

@echo ***********************************************************
@echo * get cmake dir and version
set CMAKE_EXE=%CUR_DIR%\CMake\bin\CMake.exe
%CMAKE_EXE% --version > nul || (set CMAKE_EXE=cmake.exe)
@echo CMAKE_EXE: %CMAKE_EXE%
%CMake_Exe% --version
@echo=


