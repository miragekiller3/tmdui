@echo off
cls

@echo ***********************************************************
@echo * set project name and src dir
set PRJ_NAME=tmdui
set SRC_DIR=src
@echo PRJ_NAME:%PRJ_NAME%
@echo SRC_DIR:%SRC_DIR%
@echo=

call _set_tool.bat

@echo **********************  FBI WARNING  **********************
@echo if need support QT, you need do this:
@echo -^> open path_qt.txt
@echo -^> input QT path like "D:/dev/qt/qt6_64"
@echo -^> save and close
set QTPATH=
set /p QTPATH=<./path_qt.txt
@echo QTPATH:%QTPATH%
@echo=

@echo ***********************************************************
@echo * run cmake: %PRJ_NAME% lib mode
set PRJ_DIR2=%PRJ_DIR%_lib
mkdir %PRJ_DIR2% > nul
cd %PRJ_DIR2%
@echo on
%CMake_Exe% ^
  %TOOLSET% ^
  -DENABLE_RTTI=OFF ^
  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY ^
  -DTMDUI_LIB=True ^
  -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="%PRJ_DIR2%\bin" ^
  -DCMAKE_LIBRARY_OUTPUT_DIRECTORY="%PRJ_DIR2%\bin" ^
  -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY="%PRJ_DIR2%\lib" ^
  %CUR_DIR%..\%SRC_DIR% 
@echo off

@echo ***********************************************************
@echo * run cmake: %PRJ_NAME% dll mode
set PRJ_DIR2=%PRJ_DIR%_dll
mkdir %PRJ_DIR2% > nul
cd %PRJ_DIR2%
@echo on
%CMake_Exe% ^
  %TOOLSET% ^
  -DENABLE_RTTI=OFF ^
  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY ^
  -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="%PRJ_DIR2%\bin" ^
  -DCMAKE_LIBRARY_OUTPUT_DIRECTORY="%PRJ_DIR2%\bin" ^
  -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY="%PRJ_DIR2%\lib" ^
  %CUR_DIR%..\%SRC_DIR% 
@echo off

@echo ***********************************************************
@echo * run cmake: %PRJ_NAME% dll(spy) moode
set PRJ_DIR2=%PRJ_DIR%_spy
mkdir %PRJ_DIR2% > nul
cd %PRJ_DIR2%
@echo on
%CMake_Exe% ^
  %TOOLSET% ^
  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY ^
  -DTMDUI_DESIGN=True ^
  -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="%PRJ_DIR2%\bin" ^
  -DCMAKE_LIBRARY_OUTPUT_DIRECTORY="%PRJ_DIR2%\bin" ^
  -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY="%PRJ_DIR2%\lib" ^
  %CUR_DIR%..\%SRC_DIR% 
@echo off


@echo ***********************************************************
@echo * run cmake: %PRJ_NAME% dll(QT spy) moode
rem    set PRJ_DIR2=%PRJ_DIR%_qtspy
rem    mkdir %PRJ_DIR2% > nul
rem    cd %PRJ_DIR2%
rem    @echo on
rem    %CMake_Exe% ^
  %TOOLSET% ^
  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY ^
  -DTMDUI_DESIGN=True ^
  -DTMDUI_QTSPY=True ^
  -DCMAKE_CXX_STANDARD=17 ^
  -DQTPATH=%QTPATH% ^
  -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="%PRJ_DIR2%\bin" ^
  -DCMAKE_LIBRARY_OUTPUT_DIRECTORY="%PRJ_DIR2%\bin" ^
  -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY="%PRJ_DIR2%\lib" ^
  %CUR_DIR%..\%SRC_DIR% 
@echo off


cd %CUR_DIR%

pause
