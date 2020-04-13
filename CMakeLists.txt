cmake_minimum_required(VERSION 3.0)
project(ModernOPC)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_run(CONANFILE conanfile.txt
                BASIC_SETUP CMAKE_TARGETS
                BUILD missing)

include(CTest)
enable_testing()

option(ENABLE_ASAN off "enable address sanitizer")
option(CALC_COVERAGE off "calculate code coverage")
option(BUILD_Benchmark "Build benchmark tests" OFF)

if(CALC_COVERAGE)
    set(COMPILER_OPTIONS -g -O0 -fno-inline -fno-inline-small-functions -fno-default-inline -fprofile-arcs -ftest-coverage)
    set(LIB_GCOV gcov)
endif()

#set(DO_CLANG_TIDY clang-tidy -checks=-*,modernize-*,performance-*,boost-*)
#set(CMAKE_CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
set(${COMPILER_OPTIONS} -Wall -Wextra -pedantic-errors -Wconversion -Wsign-conversion -Wpedantic -Weffc++ -Wno-unused-parameter)
if(ENABLE_ASAN)
    set(COMPILER_OPTIONS -fsanitize=address -fsanitize-address-use-after-scope)
endif()

add_subdirectory(nodesetLoader)
add_subdirectory(TypeGenerator)

add_library(wrapper SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/Server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/import/import.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/import/value.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/import/Extension.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/Variant.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/Variant_get.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/types/LocalizedText.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/types/QualifiedName.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/types/StdTypes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/types/NodeId.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/nodes/ObjectNode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/nodes/VariableNode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/nodes/Node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/generated.cpp)
target_include_directories(wrapper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/include)
target_include_directories(wrapper PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src)
target_include_directories(wrapper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/generated)
target_link_libraries(wrapper PRIVATE ${LIB_GCOV} CONAN_PKG::open62541 xmlImport)
target_link_libraries(wrapper PUBLIC CONAN_PKG::open62541)
target_compile_features(wrapper PRIVATE cxx_std_17)
target_compile_options(wrapper PRIVATE ${COMPILER_OPTIONS})
if(${ENABLE_ASAN})
    target_link_libraries(wrapper INTERFACE "-fsanitize=address")
endif()

add_subdirectory(tests)
add_subdirectory(tutorials)
if(${BUILD_Benchmark})
    if(CMAKE_BUILD_TYPE MATCHES "^(Debug)$")
        message(ERROR "It makes no sense to build benchmarks with build type DEBUG")
    endif()
    add_subdirectory(benchmarks)
endif()

add_executable(demo test.cpp) 
target_link_libraries(demo PRIVATE CONAN_PKG::open62541)
#target_link_libraries(demo INTERFACE "-fsanitize=address")

option(BUILD_DOC "Build documentation" OFF)

if(${BUILD_DOC})
    add_subdirectory(docs)
endif()