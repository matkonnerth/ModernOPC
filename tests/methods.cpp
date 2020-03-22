#include <Server.h>
#include <gtest/gtest.h>

TEST(Methods, std_function)
{
    using namespace std::string_literals;
    std::function test = []() { return "hello"s; };
    opc::Server s;
    s.addMethod(opc::NodeId(0, 85), "open", test);
}

TEST(Methods, memberFunction)
{
    using namespace std::string_literals;
    struct Callable
    {
        int run(){return 12;}
    };
    Callable c;
    opc::Server s;
    std::function fn = [&]() { return c.run(); };
    s.addMethod(opc::NodeId(0, 85), "open", fn);

    //std::function<int(Callable*)> memberFn = &Callable::run;
    //s.addMethod(opc::NodeId(0,85), "run", &Callable::run);
}