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

#open62541 configuration
SET(UA_ENABLE_SUBSCRIPTIONS_EVENTS ON CACHE BOOL "Enable event support")
SET(BUILD_SHARED_LIBS ON CACHE BOOL "build open shared")
SET(UA_NAMESPACE_ZERO REDUCED CACHE STRING "reduced namespace zero")

#add_subdirectory(open62541)
add_subdirectory(nodesetLoader)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/nodes/Node.cpp)
target_include_directories(wrapper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/include)
target_include_directories(wrapper PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src)
target_link_libraries(wrapper PRIVATE ${LIB_GCOV} CONAN_PKG::open62541 xmlImport)
target_link_libraries(wrapper PUBLIC CONAN_PKG::open62541)
target_compile_features(wrapper PRIVATE cxx_std_17)
target_compile_options(wrapper PRIVATE ${COMPILER_OPTIONS})
if(${ENABLE_ASAN})
    target_link_libraries(wrapper INTERFACE "-fsanitize=address")
endif()

add_subdirectory(tests)
add_subdirectory(tutorials)
add_subdirectory(benchmarks)

add_executable(demo test.cpp) 
#target_include_directories(demo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/include)
target_link_libraries(demo PRIVATE CONAN_PKG::open62541)
#target_compile_features(demo PRIVATE cxx_std_17)

option(BUILD_DOC "Build documentation" OFF)

if(${BUILD_DOC})
    add_subdirectory(docs)
endif()