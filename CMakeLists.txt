cmake_minimum_required(VERSION 3.0)
set (CMAKE_CXX_STANDARD 17)
project(open62541Wrapper)

add_subdirectory(open62541)
#set_target_properties(open62541 PROPERTIES UA_ENABLE_XML_IMPORT ON)

add_library(wrapper ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/Server.cpp ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/DataSource.cpp)
target_include_directories(wrapper 
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/open62541/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/wrapper
    )

target_link_libraries(wrapper PRIVATE open62541)

add_executable(appserver main.cpp)
target_link_libraries(appserver PRIVATE wrapper)


add_subdirectory(tests)

add_executable(delegates ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/quickstart.cpp)
target_include_directories(wrapper PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/wrapper)