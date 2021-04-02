# ModernOPC
![IntegrationTests](https://github.com/matkonnerth/ModernOpc/workflows/GccDebugMemcheck/badge.svg)
[![codecov](https://codecov.io/gh/matkonnerth/ModernOPC/branch/master/graph/badge.svg)](https://codecov.io/gh/matkonnerth/ModernOPC)

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
[![Build Status](https://travis-ci.org/matkonnerth/ModernOPC.svg?branch=master)](https://travis-ci.org/matkonnerth/ModernOPC)
[![codecov](https://codecov.io/gh/matkonnerth/ModernOPC/branch/master/graph/badge.svg)](https://codecov.io/gh/matkonnerth/ModernOPC)

# license
MPL 2.0, take a look on [license](https://github.com/matkonnerth/ModernOPC/blob/master/LICENSE.md) for further information.

# Quick start

# [Documentation](https://matkonnerth.github.io/ModernOPC-Docu/)

