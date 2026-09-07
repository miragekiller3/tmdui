cmake_minimum_required(VERSION 3.13)

project("pretest")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  project("sample64")
else()
  project("sample")
endif()

include("../pch/baseconfig.cmake")

include_directories(
  ../3rd/wtl10
  ../src
  ../3rd
)

#  ../pch
#  .


# Add all subdirectory
add_subdirectory(./../src dui)


function(MakePrj pname)
  message("make sample: ${pname}")
  file(GLOB File_${pname} ${pname}/*)
  add_executable(${pname} WIN32 ${File_${pname}})
  target_link_libraries(${pname} TMDui TMDuiAcc)
endfunction()

message("*******************************************************")
MakePrj(Animation)
MakePrj(Arrow)
MakePrj(AutoSizeDlg)
MakePrj(Canvas2D)
MakePrj(Combo)
MakePrj(CreateByHWND)
MakePrj(DragDrop)
MakePrj(Dummy)
MakePrj(Flash)
MakePrj(Float)
MakePrj(GXTest)
MakePrj(IEBrowser)
MakePrj(ListBindWnd)
MakePrj(MenuBar)
MakePrj(MenuModal)
MakePrj(RichEdit)
MakePrj(Rotate)
MakePrj(ScrollBox)
MakePrj(SkinLanguage)
MakePrj(Tooltip)
MakePrj(TreeBindWnd)
MakePrj(TreeView)
MakePrj(Video)
MakePrj(Win2Pic)

#file(GLOB File_Animation Animation/*)
#add_executable(Animation WIN32 ${File_Animation})
#target_link_libraries(Animation TMDui TMDuiAcc)





