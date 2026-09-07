cmake_minimum_required(VERSION 3.13)

if(NOT PROJECT_NAME)
project("pretest")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  project("bin64")
else()
  project("bin")
endif() # CMAKE_SIZEOF_VOID_P
endif() #(NOT PROJECT_NAME)

include("../pch/baseconfig.cmake")

message("*******************************************************")
message("build type TMDUI_DESIGN:${TMDUI_DESIGN}")
message("build type TMDUI_LIB:${TMDUI_LIB}")
message("PROJECT_NAME: ${PROJECT_NAME}")
message("*******************************************************")

file(GLOB File_TM  ../pch/tm.cpp)
file(GLOB File_GRA ../pch/gx.cpp)
file(GLOB File_DUI ../pch/tmdui.cpp)
file(GLOB File_AFX ../pch/stdafx.*)

file(GLOB Design_File1 design/frame/*.*)
file(GLOB Design_File2 design/Panel/*.*)
file(GLOB Design_File3 design/*.h design/*.cpp design/*.def design/res/*.h design/res/*.rc)
file(GLOB Design_FileQ design/QtExt/*.*)

include_directories(
  ../3rd
  ../3rd/wtl10
  ../pch
  .
)

#  ../3rd/win


if(${TMDUI_DESIGN})
  message("*******************************************************")
  message("TMDUI_DESIGN: make TMDui dll in design(spy) mode")
  message("*******************************************************")
  add_definitions(-DTUI_EDIT_MODE)
  add_definitions(-DTUI_EXPORTS)
  add_definitions(-DTM_EXPORTS)
  add_definitions(-D_USRDLL)
  add_definitions(-DTM_DBG)  
  include_directories(design/res)
#  add_subdirectory(design)
  source_group("Frame" FILES ${Design_File1})
  source_group("Panel" FILES ${Design_File2})
#  target_precompile_headers(TMDui "./pch/StdAfx.cpp")
  if(${TMDUI_QTSPY})
    add_definitions(-DTM_QTSPY)
    #set(CMAKE_CXX_STANDARD 17)
    #target_compile_features(TMDui PUBLIC cxx_std_17)
    source_group("QtExt" FILES ${Design_FileQ})
  
####  search QT_VERSION_STR
    set(QT_VERSION_HEADER "${QTPATH}/include/QtCore/qconfig.h")
    file(READ "${QT_VERSION_HEADER}" QT_VERSION_H_CONTENT)
    string(REGEX MATCH "#define QT_VERSION_STR \"([^\"]*)" _ ${QT_VERSION_H_CONTENT})
    set(QT_VERSION_STR "${CMAKE_MATCH_1}")
    message("QTPATH: ${QTPATH}")
    message("QT_VERSION_STR: ${QT_VERSION_STR}")
    
####  include path
    include_directories(${QTPATH}/include)
	
    include_directories(${QTPATH}/include/QtCore)
    include_directories(${QTPATH}/include/QtCore/${QT_VERSION_STR})
    include_directories(${QTPATH}/include/QtCore/${QT_VERSION_STR}/QtCore)
	
    include_directories(${QTPATH}/include/QtQuick)
    include_directories(${QTPATH}/include/QtQuick/${QT_VERSION_STR})
    include_directories(${QTPATH}/include/QtQuick/${QT_VERSION_STR}/QtQuick)

    include_directories(${QTPATH}/include/QtGui)
    include_directories(${QTPATH}/include/QtGui/${QT_VERSION_STR})
    include_directories(${QTPATH}/include/QtGui/${QT_VERSION_STR}/QtGui)
	
    include_directories(${QTPATH}/include/QtQml)
    include_directories(${QTPATH}/include/QtQml/${QT_VERSION_STR})
	  include_directories(${QTPATH}/include/QtQml/${QT_VERSION_STR}/QtQml)
	
    include_directories(${QTPATH}/include/QtQmlModels)
    include_directories(${QTPATH}/include/QtQmlModels/${QT_VERSION_STR})
	  include_directories(${QTPATH}/include/QtQmlModels/${QT_VERSION_STR}/QtQmlModels)

    include_directories(${QTPATH}/include/QtWidgets)
    include_directories(${QTPATH}/include/QtWidgets/${QT_VERSION_STR})
    include_directories(${QTPATH}/include/QtWidgets/${QT_VERSION_STR}/QtWidgets)
    
    link_directories(${QTPATH}/lib)
    add_library(TMDui SHARED ${File_AFX} ${File_TM} ${File_GRA} ${File_DUI} ${Design_File1} ${Design_File2} ${Design_File3} ${Design_FileQ})
      #set_target_properties(TMDui PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
      #add_compile_options( $<$<CONFIG:Debug>:/MD>  $<$<CONFIG:Release>:/MD> )
      target_compile_options(TMDui PRIVATE $<$<CONFIG:Debug>:/MD>  $<$<CONFIG:Release>:/MD>)
    #add_definitions(-DTM_QTSPYD)
    add_library(TMDuiD SHARED ${File_AFX} ${File_TM} ${File_GRA} ${File_DUI} ${Design_File1} ${Design_File2} ${Design_File3} ${Design_FileQ})
      target_compile_definitions(TMDuiD PRIVATE -DTM_QTSPYD)
  else()
    add_library(TMDui SHARED ${File_AFX} ${File_TM} ${File_GRA} ${File_DUI} ${Design_File1} ${Design_File2} ${Design_File3})
  endif()
elseif(${TMDUI_LIB})
  message("*******************************************************")
  message("TMDUI_LIB: make TMDui lib")
  message("*******************************************************")
  add_library(TMDui ${File_AFX} ${File_TM} ${File_GRA} ${File_DUI})
else()
  message("*******************************************************")
  message("TMDUI_DLL: make TMDui dll in normal mode")
  message("*******************************************************")
  add_definitions(-DTUI_EXPORTS)
  add_definitions(-DTM_EXPORTS)
  add_definitions(-D_USRDLL)
  add_library(TMDui SHARED ${File_AFX} ${File_TM} ${File_GRA} ${File_DUI})
endif()


add_subdirectory(accessible)
#add_dependencies(TMDuiAcc TMDui)
target_link_libraries(TMDuiAcc TMDui)

#  add_library(TMDui SHARED ${File_TM} ${File_GRA} ${File_SVG} ${File_DUI})
#  add_subdirectory(design)



# Add all subdirectory
#add_subdirectory(accessible)


