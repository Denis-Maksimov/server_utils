cmake_minimum_required(VERSION 3.5)

project(snap7)

set(CMAKE_CXX_STANDARD 17)

include_directories(inc/core inc/lib inc/sys .)

find_package(Threads REQUIRED)

file(GLOB_RECURSE LIB_SOURCES 
    "src/core/*.cpp"
    "src/lib/*.cpp" 
    "src/sys/*.cpp"
    "snap7.cpp"
)


add_library(snap7 SHARED ${LIB_SOURCES})
target_include_directories(snap7 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/.)

set_target_properties(snap7 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})