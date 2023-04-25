# add_executable(mylib-example mylib_example.cpp)
cmake_minimum_required(VERSION 3.5)
# get_directory_property(IS_SUBPROJECT PARENT_DIRECTORY)

# set(CMAKE_C_COMPILER "gcc")
# set(CMAKE_CXX_COMPILER "g++")
# set(NM )
project(Server)				# Название проекта

# set (${CMAKE_PROJECT_NAME}_VERSION_MAJOR 0)
# set (${CMAKE_PROJECT_NAME}_VERSION_MINOR 1)
# set (${CMAKE_PROJECT_NAME}_VERSION "${${CMAKE_PROJECT_NAME}_VERSION_MAJOR}.${${CMAKE_PROJECT_NAME}_VERSION_MINOR}")
# set(PRNAME ${NM}_${${CMAKE_PROJECT_NAME}_VERSION})
# set(CMAKE_CXX_STANDARD 17)

#--
file(GLOB LIB_SOURCES)
LIST(APPEND LIB_SOURCES src/network.cpp)
LIST(APPEND LIB_SOURCES src/secureLayer.cpp)
LIST(APPEND LIB_SOURCES src/httpLayer.cpp)
# LIST(APPEND SOURCES src/test.cpp)
#--
# LIST(APPEND SOURCES src/UIManager.cpp)
# LIST(APPEND SOURCES src/MainMenuUI.cpp)
#--
# LIST(APPEND SOURCES src/main.cpp)
add_library(Server SHARED ${LIB_SOURCES})


include_directories(inc)
# include_directories(Crypto_0.1)
# include_directories(Protocol)

# add_executable(${PRNAME} ${SOURCES})	


target_link_libraries(
    Server 
    pthread 
    ssl 
    crypto 
    stdc++fs
)
target_include_directories(loaderLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
set_target_properties(Server PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
