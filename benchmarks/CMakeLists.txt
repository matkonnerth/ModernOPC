add_executable(variant_bench variant.cpp)
target_link_libraries(variant_bench PRIVATE wrapper CONAN_PKG::benchmark open62541)