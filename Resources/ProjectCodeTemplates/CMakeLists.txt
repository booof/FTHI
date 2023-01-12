cmake_minimum_required(VERSION 3.13.4)
project(
)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
add_library(project_lib SHARED Main.cpp ScriptHandler.cpp
)
target_precompile_headers(project_lib PRIVATE 
"Globals.h"
"Constants.h"
)
target_link_libraries(project_lib 
)
target_compile_definitions(project_lib PRIVATE "BUILD_DLL")
set(installable_libs project_lib)
install(TARGETS ${installable_libs} DESTINATION lib)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")