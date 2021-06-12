cmake_minimum_required(VERSION 3.0)
project(modernopc)

option(ENABLE_ASAN "enable address sanitizer" OFF)
option(CALC_COVERAGE "calculate code coverage" OFF)
option(BUILD_Benchmark "Build benchmark tests" OFF)

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

find_package(open62541 REQUIRED)
find_package(NodesetLoader REQUIRED)
#add_subdirectory(TypeGenerator)

add_library(modernopc SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/modernopc/src/Server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modernopc/src/client/Client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modernopc/src/import/Extension.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modernopc/src/Variant.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modernopc/src/Variant_get.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modernopc/src/types/LocalizedText.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modernopc/src/types/QualifiedName.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modernopc/src/types/String.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modernopc/src/types/NodeId.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modernopc/src/nodes/ObjectNode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modernopc/src/nodes/VariableNode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modernopc/src/nodes/Node.cpp)
target_include_directories(modernopc PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/modernopc/include>
    $<INSTALL_INTERFACE:include/modernopc>)
target_include_directories(modernopc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/modernopc/src)
target_link_libraries(modernopc PRIVATE ${LIB_GCOV} NodesetLoader)
target_link_libraries(modernopc PUBLIC CONAN_PKG::spdlog)
target_link_libraries(modernopc PUBLIC open62541::open62541)
target_compile_features(modernopc PRIVATE cxx_std_17)
target_compile_options(modernopc PRIVATE ${COMPILER_OPTIONS})
if(${ENABLE_ASAN})
    target_link_libraries(modernopc INTERFACE "-fsanitize=address")
endif()

add_subdirectory(tests)
add_subdirectory(tutorials)
if(${BUILD_Benchmark})
    if(CMAKE_BUILD_TYPE MATCHES "^(Debug)$")
        message(ERROR "It makes no sense to build benchmarks with build type DEBUG")
    endif()
    add_subdirectory(benchmarks)
endif()

option(BUILD_DOC "Build documentation" OFF)

if(${BUILD_DOC})
    add_subdirectory(docs)
endif()

#install
if(NOT ${CALC_COVERAGE})
    install(TARGETS modernopc
            EXPORT modernopc
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
            RUNTIME DESTINATION bin
            INCLUDES DESTINATION include
            PUBLIC_HEADER DESTINATION include)

    install(EXPORT modernopc DESTINATION lib/cmake/modernopc)
    install(FILES modernopcConfig.cmake DESTINATION lib/cmake/modernopc)

    install(DIRECTORY ${PROJECT_SOURCE_DIR}/modernopc/include/modernopc DESTINATION include)
endif()