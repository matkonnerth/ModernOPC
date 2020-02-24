cmake_minimum_required(VERSION 3.0)
project(open62541Wrapper)


set(DO_CLANG_TIDY clang-tidy -checks=-*,modernize-*,performance-*,boost-*)
set(COMPILER_OPTIONS -Werror -Wall -Wextra -pedantic-errors -Wconversion -Wsign-conversion -Wpedantic -Weffc++ -Wno-unused-parameter)
add_subdirectory(open62541)

add_library(wrapper SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/Server.cpp)
target_include_directories(wrapper
    PUBLIC 
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/wrapper/include>
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/wrapper
    )

#get rid of this public
target_link_libraries(wrapper PRIVATE open62541)
target_compile_features(wrapper PRIVATE cxx_std_17)
target_compile_options(wrapper PRIVATE ${COMPILER_OPTIONS})
set_target_properties(wrapper PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")

add_executable(appserver main.cpp)
target_link_libraries(appserver PRIVATE wrapper pthread)
target_compile_features(appserver PRIVATE cxx_std_17)
target_compile_options(appserver PRIVATE ${COMPILER_OPTIONS})
set_target_properties(appserver PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")


add_subdirectory(tests)

add_executable(test_variant 
    ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/test_variant.cpp)
target_include_directories(test_variant PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/wrapper/include)
target_link_libraries(test_variant PRIVATE open62541)
target_compile_features(test_variant PRIVATE cxx_std_17)

add_executable(statemachine statemachine.cpp)
target_compile_features(statemachine PRIVATE cxx_std_17)
target_compile_options(statemachine PRIVATE ${COMPILER_OPTIONS})
set_target_properties(statemachine PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")