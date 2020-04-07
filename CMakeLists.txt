cmake_minimum_required(VERSION 3.0)
project(open62541Wrapper)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

include(CTest)
enable_testing()

option(ENABLE_ASAN off "enable address sanitizer")

set(DO_CLANG_TIDY clang-tidy -checks=-*,modernize-*,performance-*,boost-*)
set(CMAKE_CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
set(COMPILER_OPTIONS -Wall -Wextra -pedantic-errors -Wconversion -Wsign-conversion -Wpedantic -Weffc++ -Wno-unused-parameter)
if(ENABLE_ASAN)
    set(COMPILER_OPTIONS -fsanitize=address -fsanitize-address-use-after-scope)
    set(LIB_ASAN asan)
endif()

#open62541 configuration
SET(UA_ENABLE_SUBSCRIPTIONS_EVENTS ON CACHE BOOL "Enable event support")
SET(BUILD_SHARED_LIBS ON CACHE BOOL "build open shared")
SET(UA_NAMESPACE_ZERO FULL CACHE STRING "generate full namespace zero")

add_subdirectory(open62541)
add_subdirectory(nodesetLoader)

add_library(wrapper SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/Server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/import/import.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/import/value.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/import/Extension.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/Variant.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/Variant_get.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/types/Types.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/types/StdTypes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/types/NodeId.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/DataType.cpp)
target_include_directories(wrapper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/include)
target_include_directories(wrapper PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src)
target_link_libraries(wrapper PRIVATE open62541 xmlImport)
target_compile_features(wrapper PRIVATE cxx_std_17)
target_compile_options(wrapper PRIVATE ${COMPILER_OPTIONS})

add_subdirectory(tests)
add_subdirectory(tutorials)
add_subdirectory(benchmarks)

add_executable(demo test.cpp) 
#target_include_directories(demo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/include)
target_link_libraries(demo PRIVATE wrapper open62541)
target_compile_features(demo PRIVATE cxx_std_17)