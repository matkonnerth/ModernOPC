cmake_minimum_required(VERSION 3.0)
project(open62541Wrapper)

add_subdirectory(open62541)

add_library(wrapper SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/Server.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/DataSource.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/Variant.cpp)
target_include_directories(wrapper
    PUBLIC 
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/wrapper/include>
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper
    )

#get rid of this public
target_link_libraries(wrapper PUBLIC open62541)
target_compile_features(wrapper PRIVATE cxx_std_17)

add_executable(appserver main.cpp)
target_link_libraries(appserver PRIVATE wrapper)
target_compile_features(appserver PRIVATE cxx_std_17)


add_subdirectory(tests)

add_executable(test_variant 
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/test_variant.cpp ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/Variant.cpp)
target_include_directories(test_variant PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/include)
target_link_libraries(test_variant PRIVATE open62541)
target_compile_features(test_variant PRIVATE cxx_std_17)