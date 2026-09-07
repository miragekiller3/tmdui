cmake_minimum_required(VERSION 3.13)



file(GLOB File_Acc  *.cpp *.h *.def *.rc)


include_directories(
  ../3rd
  .
)

#  ../3rd/win
#  ../3rd/wtl10


if (${TMDUI_LIB})
  message("*******************************************************")
  message("TMDUI_ACC_LIB: make TMDuiAcc lib")
  message("*******************************************************")
  add_library(TMDuiAcc  ${File_Acc})
else()
  message("*******************************************************")
  message("TMDUI_ACC_DLL: make TMDuiAcc dll")
  message("*******************************************************")
  add_definitions(-DTUI_ACC_EXPORTS)
  add_definitions(-D_USRDLL)
  add_library(TMDuiAcc SHARED ${File_Acc})
endif ()


#  add_library(TMDui SHARED ${File_TM} ${File_GRA} ${File_SVG} ${File_DUI})
#  add_subdirectory(design)



# Add all subdirectory
#add_subdirectory(accessible)


