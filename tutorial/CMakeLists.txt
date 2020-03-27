
add_executable(appserver main.cpp FileService.cpp)
target_include_directories(appserver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(appserver PRIVATE wrapper open62541 stdc++fs pthread)
target_compile_features(appserver PRIVATE cxx_std_17)
target_compile_options(appserver PRIVATE ${COMPILER_OPTIONS})
#set_target_properties(appserver PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")

add_subdirectory(customTypes)