
add_executable(tutorial_customTypes main.cpp AppTypes.cpp)
target_include_directories(tutorial_customTypes PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(tutorial_customTypes PRIVATE ${LIB_ASAN} wrapper open62541)
target_compile_features(tutorial_customTypes PRIVATE cxx_std_17)
target_compile_options(tutorial_customTypes PRIVATE ${COMPILER_OPTIONS})