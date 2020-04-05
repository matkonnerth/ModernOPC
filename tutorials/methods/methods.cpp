#include <opc/Server.h>
#include <functional>

struct Callable
{
    Callable(const std::string s) : state{s} {}
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
    opc::Server s;

    Callable c1{"Hello"};
    Callable c2{"world"};

    s.addObject(opc::NodeId(0,85), opc::NodeId(1,"hello"), opc::NodeId(0,0), "Hello", &c1);
    s.addObject(opc::NodeId(0, 85), opc::NodeId(1, "world"), opc::NodeId(0, 0),
                "World", &c2);

    s.addMethod(opc::NodeId(1, "hello"), "run", &Callable::run);
    s.addMethod(opc::NodeId(1, "world"), "run", &Callable::run);

    std::function f = [](int a, int b) { return std::vector<int>{a, b}; };
    s.addMethod(opc::NodeId(0, 85), "addMethod2", f);

    s.run();
}