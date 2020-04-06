# open62541 Wrapper [![Build Status](https://travis-ci.org/matkonnerth/openWrapper.svg?branch=master)](https://travis-ci.org/matkonnerth/openWrapper)

# todos
* support for statemachines
* support binding to specific interface / port
* support user defined datatypes:
  * structures
  * enumerations
  Solution 1) let open62541 generate the datatype, manually implement the conversion functions
  Solution 2) generate the conversion functions
* refactor Variant handling
  * hack in open library is necessary to forward declare UA_Variant in Variant.h
* calculate test coverage
* provide callback for reading Localized Text from database
* security
* Support for base node Ids, node Types (Objectsfolder)
* client
* run tests with valgrind/asan
* closer look on method template implementation, lots of tuples in binary

# tutorials
* suppport creating Objects from Object types
As a developer I want to load a nodeset with the ObjectType of a service.
When I create an instance of this ObjectType, I want to bind it to the service implementation.
There can be several instances of the service
-> take a look on tutorial->serviceObject

* add support for Events

