cmake_minimum_required(VERSION 3.13.4)
project(EngineCode)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_definitions(DLL_HEADER)
add_compile_definitions(DLL_CODE)
add_library(enginecode STATIC
"C:/Users/ellio/OneDrive/Documents/Visual Studio 2019 - Copy/projects/Game Engine/FTHI/Core/Source/Class/Render/Camera/Camera.cpp"
)
target_compile_definitions(enginecode PRIVATE "BUILD_DLL")
set(installable_libs enginecode)
install(TARGETS ${installable_libs} DESTINATION lib)