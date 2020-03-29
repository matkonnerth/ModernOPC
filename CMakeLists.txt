cmake_minimum_required(VERSION 3.0)
project(open62541Wrapper)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

include(CTest)
enable_testing()

option(ENABLE_ASAN on "enable address sanitizer")

set(DO_CLANG_TIDY clang-tidy -checks=-*,modernize-*,performance-*,boost-*)
set(COMPILER_OPTIONS -Wall -Wextra -pedantic-errors -Wconversion -Wsign-conversion -Wpedantic -Weffc++ -Wno-unused-parameter)
if(ENABLE_ASAN)
    set(COMPILER_OPTIONS -fsanitize=address -fsanitize-address-use-after-scope)
    set(LIB_ASAN asan)
endif()

add_subdirectory(open62541)
add_subdirectory(nodesetLoader)

add_library(wrapper SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/Server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/import/import.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/import/value.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/import/Extension.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/Variant.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/Variant_is_a.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/Variant_get.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/Variant_set.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/TypeConverter.cpp)
target_include_directories(wrapper
    PUBLIC 
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/wrapper/include>
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/import
    )

target_link_libraries(wrapper PRIVATE open62541 xmlImport)
target_compile_features(wrapper PRIVATE cxx_std_17)
target_compile_options(wrapper PRIVATE ${COMPILER_OPTIONS})
set_target_properties(wrapper PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")

add_subdirectory(tests)
add_subdirectory(tutorials)
add_subdirectory(benchmarks)

add_executable(functions function.cpp)
target_link_libraries(functions PRIVATE ${LIB_ASAN})