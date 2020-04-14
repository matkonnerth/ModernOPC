cmake_minimum_required(VERSION 3.0)
project(Typegenerator)

add_executable(typegenerator main.cpp backend.cpp NodeId.cpp types/EnumDataType.cpp types/BaseDataType.cpp)
target_include_directories(typegenerator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(typegenerator PRIVATE xmlImport)
target_compile_features(typegenerator PRIVATE cxx_std_17)
target_compile_options(typegenerator  PRIVATE ${COMPILER_OPTIONS})