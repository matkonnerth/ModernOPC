add_executable(variant_bench variant.cpp)
target_link_libraries(variant_bench PRIVATE ${LIB_GCOV} wrapper CONAN_PKG::benchmark open62541)
target_compile_features(variant_bench PRIVATE cxx_std_17)

add_executable(method_bench method.cpp)
target_link_libraries(method_bench PRIVATE ${LIB_GCOV} wrapper CONAN_PKG::benchmark open62541)
target_compile_features(method_bench PRIVATE cxx_std_17)