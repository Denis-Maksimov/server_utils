cmake_minimum_required(VERSION 3.5)

project(myproject)

set(CMAKE_CXX_STANDARD 17)

include_directories(inc)

# find_package(Threads REQUIRED)

# file(GLOB_RECURSE LIB_SOURCES src/*.cpp)
file(GLOB LIB_SOURCES)

# абстрактное синтаксическое дерево
list(APPEND LIB_SOURCES "src/AST.cpp")

# шина
list(APPEND LIB_SOURCES "src/bus.cpp")

# model view controller
list(APPEND LIB_SOURCES "src/MVC.cpp")

# http parser
list(APPEND LIB_SOURCES "src/parser.cpp")


# s7
# list(APPEND LIB_SOURCES "src/parser.cpp")



# list(REMOVE_ITEM LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/test.cpp)

add_library(parser SHARED ${LIB_SOURCES})
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
target_include_directories(parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
set_target_properties(parser PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
# set_target_properties(myexecutable PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
# set_target_properties(opcua PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)