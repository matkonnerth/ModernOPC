cmake_minimum_required(VERSION 3.0)
project(open62541Wrapper)


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

include(CTest)
enable_testing()

set(DO_CLANG_TIDY clang-tidy -checks=-*,modernize-*,performance-*,boost-*)
set(COMPILER_OPTIONS -Werror -Wall -Wextra -pedantic-errors -Wconversion -Wsign-conversion -Wpedantic -Weffc++ -Wno-unused-parameter)
add_subdirectory(open62541)
add_subdirectory(nodesetLoader)

add_library(wrapper SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/Server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/import.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/value.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/Variant.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/src/TypeConverter.cpp)
target_include_directories(wrapper
    PUBLIC 
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/wrapper/include>
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

target_link_libraries(wrapper PRIVATE open62541 xmlImport)
target_compile_features(wrapper PRIVATE cxx_std_17)
target_compile_options(wrapper PRIVATE ${COMPILER_OPTIONS})
#set_target_properties(wrapper PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")

add_executable(appserver main.cpp FileService.cpp AppTypes.cpp)
target_include_directories(appserver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(appserver PRIVATE wrapper open62541 pthread)
target_compile_features(appserver PRIVATE cxx_std_17)
target_compile_options(appserver PRIVATE ${COMPILER_OPTIONS})
#set_target_properties(appserver PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")


add_subdirectory(tests)

add_executable(statemachine statemachine.cpp)
target_compile_features(statemachine PRIVATE cxx_std_17)
target_compile_options(statemachine PRIVATE ${COMPILER_OPTIONS})
#set_target_properties(statemachine PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")