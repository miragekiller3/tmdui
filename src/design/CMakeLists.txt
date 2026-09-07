cmake_minimum_required(VERSION 3.0)

file(GLOB Design_File1 frame/*.*)
file(GLOB Design_File2 Panel/*.*)
file(GLOB Design_File3 ./*.h ./*.cpp ./*.def res/*.h res/*.rc)

include_directories(.)

#add_library(TMDuiDesignLib ${Design_File1} ${Design_File2} ${Design_File3})
#target_precompile_headers(TMDuiDesignLib ./StdAfx.h)

source_group("Frame" FILES ${Design_File1})
source_group("Panel" FILES ${Design_File1})
