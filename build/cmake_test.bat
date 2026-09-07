@echo off
cls

@echo ***********************************************************
@echo * set project name and src dir
set PRJ_NAME=TMTest
set SRC_DIR=test
@echo PRJ_NAME:%PRJ_NAME%
@echo SRC_DIR:%SRC_DIR%
@echo=

call _set_tool.bat

@echo ***********************************************************
@echo * run cmake: %PRJ_NAME%
cd %PRJ_DIR%
mkdir %PRJ_DIR% > nul
@echo on
%CMake_Exe% ^
  %TOOLSET% ^
  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY ^
  -DTMDUI_DESIGN=True ^
  -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="%PRJ_DIR%\bin" ^
  -DCMAKE_LIBRARY_OUTPUT_DIRECTORY="%PRJ_DIR%\bin" ^
  -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY="%PRJ_DIR%\lib" ^
  %CUR_DIR%..\%SRC_DIR% 

@echo off
rem	--debug-output ^
rem	-DCMAKE_C_COMPILER="E:/Program Files/Microsoft Visual Studio/VC98/Bin/cl.exe" ^
rem	-DCMAKE_CXX_COMPILER="E:/Program Files/Microsoft Visual Studio/VC98/Bin/cl.exe" ^
rem -DCMAKE_RC_COMPILER="E:/Program Files/Microsoft Visual Studio/Common/MSDev98/Bin/rc.exe" ^
rem	-DCMAKE_DISABLE_PRECOMPILE_HEADERS=False ^
rem	-DCMAKE_RUNTIME_OUTPUT_DIRECTORY="%CD%\..\output\i386\bin" ^
rem	-DCMAKE_LIBRARY_OUTPUT_DIRECTORY="%CD%\..\output\i386\bin" ^
rem	-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY="%CD%\..\output\i386\lib" ^
	
cd %CUR_DIR%

pause
