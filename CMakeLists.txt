cmake_minimum_required(VERSION 3.0)
project(open62541Wrapper)

add_subdirectory(open62541)

add_library(wrapper ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/Server.cpp ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/DataSource.cpp)
target_include_directories(wrapper
    PUBLIC 
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/wrapper/include>
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/open62541/include
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper
    )

target_link_libraries(wrapper PRIVATE open62541)
target_compile_features(wrapper PRIVATE cxx_std_17)

add_executable(appserver main.cpp)
target_link_libraries(appserver PRIVATE wrapper)
target_compile_features(appserver PRIVATE cxx_std_17)


add_subdirectory(tests)

add_executable(delegates ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/quickstart.cpp)
target_compile_features(delegates PRIVATE cxx_std_17)