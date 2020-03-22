
add_executable(customTypes main.cpp AppTypes.cpp)
target_include_directories(customTypes PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(customTypes PRIVATE wrapper open62541 asan ubsan)
target_compile_features(customTypes PRIVATE cxx_std_17)
target_compile_options(customTypes PRIVATE ${COMPILER_OPTIONS})