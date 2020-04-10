# ModernOPC
[![Build Status](https://travis-ci.org/matkonnerth/openWrapper.svg?branch=master)](https://travis-ci.org/matkonnerth/openWrapper)
[![codecov](https://codecov.io/gh/matkonnerth/openWrapper/branch/master/graph/badge.svg)](https://codecov.io/gh/matkonnerth/openWrapper)

## a lightweight c++ opc ua library
which uses open62541 (https://github.com/open62541/open62541) for the heavy lifting

# goals
* make use of opc ua in c++ projects as easy as possible
* Support for import of xml nodesets
* Extended support for methods and events
* Extended support for statemachines

# not a goal
* completely wrapping away the open62541 library
* duplicate functionality, push as much as possible towards the open62541 implementation

# dependencies
* open62541 (https://github.com/open62541/open62541)
* libXml2 for parsinger nodesets (http://xmlsoft.org/)
* cmake for makefile generation (https://cmake.org/)
* gtest for testing (https://github.com/google/googletest)
* conan for pulling dependencies (conan.io)
* C++ Standard 17

# CI
[![Build Status](https://travis-ci.org/matkonnerth/openWrapper.svg?branch=master)](https://travis-ci.org/matkonnerth/openWrapper)
[![codecov](https://codecov.io/gh/matkonnerth/openWrapper/branch/master/graph/badge.svg)](https://codecov.io/gh/matkonnerth/openWrapper)

# license
??

# Quick start

# Build
* git clone https://github.com/matkonnerth/ModernOPC.git
* git submodule init && git submodule update
* mkdir build && cd build
* conan install ..
* make -j
* make test

# tutorials
* Nodeset import
* Methods
* Events
* Adding custom types
* Adding variables and connecting to Datasources

