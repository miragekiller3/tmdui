@echo off
cls

@echo ***********************************************************
@echo * set project name and src dir
set PRJ_NAME=sample
set SRC_DIR=sample
@echo PRJ_NAME:%PRJ_NAME%
@echo SRC_DIR:%SRC_DIR%
@echo=

call _set_tool.bat

@echo ***********************************************************
@echo * run cmake: %PRJ_NAME% simple exe mode
set PRJ_DIR2=%PRJ_DIR%
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
@echo * run cmake: %PRJ_NAME% spy mode
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

	
cd %CUR_DIR%

pause
