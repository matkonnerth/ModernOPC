#!/bin/bash
set -e

# gcc, test, memcheck
if ! [ -z ${GCC_MEMCHECK+x} ]; then
    mkdir -p build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Debug .. 
    make -j
    make test
    ctest --overwrite MemoryCheckCommandOptions="--leak-check=full --error-exitcode=100" -T memcheck
fi

# gcc, test
if ! [ -z ${GCC_RELEASE+x} ]; then
    mkdir -p build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release .. 
    make -j
    make test
fi

# gcc + asan, test
if ! [ -z ${GCC_ASAN+x} ]; then
    mkdir -p build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_ASAN=ON .. 
    make -j
    make test
fi

# clang, test
if ! [ -z ${CLANG_RELEASE+x} ]; then
    mkdir -p build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make
    make test
fi

# coverage
if ! [ -z ${COVERAGE+x} ]; then
    mkdir -p build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Debug -DCALC_COVERAGE=ON .. 
    make -j
    make test
fi


