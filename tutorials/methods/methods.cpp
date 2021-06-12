/**
 * Using OPC UA Methods
 * -----------------------
 * This example shows how to add method nodes and connect it to the implementation.
 * Free Functions, member functions and lambdas are supported. */

#include <functional>
#include <iostream>
#include <modernopc/Server.h>
#include <modernopc/nodes/ObjectNode.h>
#include <tuple>

using modernopc::QualifiedName;
struct Callable
{
    Callable(const std::string& s) : state{s} {}
    int run(int test)
    {
        std::cout << state << std::endl;
        return 12;
    }

  private:
    std::string state;
};



int main()
{
    modernopc::Server s;
    auto root = s.getObjectsFolder();

    Callable c1{"Hello"};
    Callable c2{"world"};

    //add object with memberfunction and bind it to implementation
    root->addObject(modernopc::NodeId(1, "hello"), QualifiedName(1,"Hello"), &c1)
        ->addMethod(modernopc::NodeId(1, "doIt"), QualifiedName(1, "run"), &Callable::run);

    //same here
    root->addObject(modernopc::NodeId(1, "world"), QualifiedName(1, "World"), &c2)
        ->addMethod(modernopc::NodeId(1, "doIt2"), QualifiedName(1,"run"),
                    &Callable::run);

    //add a MethodNode and bind it to a lambda function
    std::function f = [](int a, int b) { return std::vector<int>{a, b}; };
    root->addMethod(modernopc::NodeId(1, "doIt3"), QualifiedName(1, "getVector"), f);

    // add a MethodNode and bind it to a lambda function
    std::function fVoidIntInt = [](int a, int b) { std::cout << a+b << std::endl;};
    root->addMethod(modernopc::NodeId(1, "doIt4"), QualifiedName(1, "getVector"), fVoidIntInt);

    // multiple output arguments via std::tuple
    using namespace std::string_literals;
    std::function multipleOutputs = []()
    {
        return std::make_tuple(42, "helloWorld"s);
    };
    root->addMethod(modernopc::NodeId(1, "multiOutputArgs"), QualifiedName(1, "multi"), multipleOutputs);

    s.run();
}