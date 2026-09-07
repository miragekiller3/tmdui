cmake_minimum_required(VERSION 3.13)

project("pretest")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  project("TMDuiDesign64")
else()
  project("TMDuiDesign")
endif()

include("../pch/baseconfig.cmake")

message("*******************************************************")
message("make ${PROJECT_NAME}.exe")
message("*******************************************************")

add_subdirectory(./../src dui)

file(GLOB File_Source
  ../pch/StdAfx.cpp
  ../pch/StdAfx.h
  TMDuiDesign.cpp
  TMDuiDesign.rc
)

include_directories(
  ../3rd
  ../3rd/wtl10
  ../src
  ../pch
)

add_executable(TMDuiDesign WIN32 ${File_Source})
target_link_libraries(TMDuiDesign TMDui TMDuiAcc)

#target_precompile_headers(TMDuiDesign stdafx.h)


