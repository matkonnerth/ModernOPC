.. _building:

Building ModernOPC
==================

ModernOPC uses cmake and conan to build the library. conan is a python based C/C++ package manager.
Installation guidelines for conan: https://docs.conan.io/en/latest/installation.html

Building the Library
--------------------

Setup conan for using prebuilt binary packages
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
add remote for open62541 prebuilt binaries

.. code-block:: bash

    conan remote add openRepo https://api.bintray.com/conan/matkonnerth/cpprepo 

Building with CMake on Ubuntu or Debian
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

    git clone git@github.com:matkonnerth/ModernOPC.git
    cd modernOPC
    git submodule init
    git submodule update

    cd modernOPC
    mkdir build
    cd build
    conan install ..
    cmake ..
    make
    make test
    

    




