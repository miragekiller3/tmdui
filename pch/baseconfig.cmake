
add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

# enable program database on release mode.
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO}")

#RTTI
option(ENABLE_RTTI "Enable C++ RTTI" ON)
if (NOT ENABLE_RTTI)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/GR-)
  else()
    add_compile_options(-fno-rtti)
  endif()
endif()

# big obj; support XP; merge same func; remove unused func; no incremental;
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options(/bigobj)
  #add_link_options(/SUBSYSTEM:CONSOLE,5.01)
  #target_compile_options(TMDui INTERFACE "/Zc:__cplusplus")
  add_compile_options(/Zc:__cplusplus /permissive-)
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:ICF /OPT:REF /INCREMENTAL:NO") 					# /VERBOSE:REF /VERBOSE:ICF
  set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /OPT:ICF /OPT:REF /INCREMENTAL:NO")
  if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SUBSYSTEM:WINDOWS,5.01")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /SUBSYSTEM:CONSOLE,5.01")
	endif()
else()
  add_compile_options(-Wa,-mbig-obj)
endif()