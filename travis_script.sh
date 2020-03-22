#!/bin/bash
set -e

# gcc, test, memcheck
if ! [ -z ${GCC_MEMCHECK+x} ]; then
    mkdir -p build
    cd build
    conan install -s compiler.libcxx=libstdc++11 ..
    cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DBUILD_SHARED_LIBS=ON .. 
    make 
    make test
    ctest --overwrite MemoryCheckCommandOptions="--leak-check=full --error-exitcode=100" -T memcheck
fi

# clang, test
if ! [ -z ${USE_CLANG+x} ]; then
    mkdir -p build
    cd build
    conan install -s compiler.libcxx=libstdc++11 .. --build gtest
    cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DBUILD_SHARED_LIBS=ON .. 
    make 
    make test
fi

