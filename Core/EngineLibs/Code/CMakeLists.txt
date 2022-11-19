cmake_minimum_required(VERSION 3.13.4)
project(EngineCode)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_definitions(DLL_HEADER)
add_compile_definitions(DLL_CODE)
add_library(enginecode STATIC
"C:/Users/ellio/source/repos/FTHI/Core/Source/Class/Render/Camera/Camera.cpp"
)
target_compile_definitions(enginecode PRIVATE "BUILD_DLL")
set(installable_libs enginecode)
install(TARGETS ${installable_libs} DESTINATION lib)