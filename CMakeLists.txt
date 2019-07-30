cmake_minimum_required(VERSION 3.0)
set (CMAKE_CXX_STANDARD 14)
project(open62541Wrapper)

add_subdirectory(open62541)

add_executable(server main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/Server.cpp)
target_include_directories(server 
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/open62541/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/wrapper)
target_link_libraries(server open62541)