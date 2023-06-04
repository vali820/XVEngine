cmake_minimum_required(VERSION 3.26)
project(XVEngine)

set(CMAKE_CXX_STANDARD 20)

# Binaries directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/${CMAKE_BUILD_TYPE}/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/${CMAKE_BUILD_TYPE}/)

include_directories(${CMAKE_SOURCE_DIR}/Source/)

function(add_project dir)
    add_subdirectory(Source/${dir}/)
endfunction()

# Engine libraries
add_project(Core)
add_project(App)
add_project(GpuApi)
add_project(Render)
add_project(UI)
add_project(Shaders)

# Programs
add_project(Programs/Sandbox)
add_dependencies(Sandbox Shaders)
