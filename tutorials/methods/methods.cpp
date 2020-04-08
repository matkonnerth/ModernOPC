#include <opc/Server.h>
#include <functional>
#include <opc/nodes/ObjectNode.h>


using opc::QualifiedName;
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
    opc::Server s;
    auto root = s.getObjectsFolder();

    Callable c1{"Hello"};
    Callable c2{"world"};

    root->addObject(opc::NodeId(1, "hello"), QualifiedName(1,"Hello"), &c1)
        ->addMethod(opc::NodeId(1, "doIt"), QualifiedName(1, "run"), &Callable::run);
    root->addObject(opc::NodeId(1, "world"), QualifiedName(1, "World"), &c2)
        ->addMethod(opc::NodeId(1, "doIt2"), QualifiedName(1,"run"),
                    &Callable::run);

    std::function f = [](int a, int b) { return std::vector<int>{a, b}; };
    root->addMethod(opc::NodeId(1, "doIt3"), QualifiedName(1, "getVector"), f);

    s.run();
}