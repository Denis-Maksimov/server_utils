cmake_minimum_required(VERSION 3.5)

project(loaderLib)

set(CMAKE_CXX_STANDARD 17)

include_directories(inc)

# find_package(Threads REQUIRED)

# file(GLOB_RECURSE LIB_SOURCES src/*.cpp)
file(GLOB LIB_SOURCES)

#
list(APPEND LIB_SOURCES "src/dem.cpp")

#
list(APPEND LIB_SOURCES "src/pass.cpp")

#
list(APPEND LIB_SOURCES "src/shared.cpp")

# http parser
# list(APPEND LIB_SOURCES "src/parser.cpp")


# s7
# list(APPEND LIB_SOURCES "src/parser.cpp")



# list(REMOVE_ITEM LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/test.cpp)

add_library(loaderLib SHARED ${LIB_SOURCES})
# add_subdirectory("./open62541")
# add_subdirectory("./snap7")

# add_executable(myexecutable src/test.cpp)
# add_executable(myexecutable src/testS7.cpp)

# add_executable(opcua src/sql.cpp)
# target_link_libraries(opcua 
#     # open62541
#     # sqlite3
#     ${CMAKE_THREAD_LIBS_INIT}
# )

# target_link_libraries(myexecutable mylib ${CMAKE_THREAD_LIBS_INIT})
# target_link_libraries(myexecutable 
#     snap7
#     ${CMAKE_THREAD_LIBS_INIT}
# )
target_link_libraries(
    loaderLib 
    dl
    ssl 
    crypto 
)
target_include_directories(loaderLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
set_target_properties(loaderLib PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
# set_target_properties(myexecutable PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
# set_target_properties(opcua PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)