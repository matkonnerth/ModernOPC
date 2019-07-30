#include "Server.h"
#include "NodeId.h"
#include <iostream>
#include <array>
#include "DataSource.h"
#include <functional>

void add(int a, int b, double c)
{
    std::cout << a + b << std::endl;
}

int getValue()
{
    static int test = 0;
    return test++;
}

int main() 
{
    opc::Server s;
    s.addMethod("addMethod", &add);

    NodeId id {3, 1234};

    std::array<int,3> test{12, 13, 14};

    std::vector<float> fVector{1.1f, 2.2f, 3.3f};

    std::function<int(void)> f = &getValue;
    DataSource<int> ds{f};

    s.addVariableNode(NodeId(0, 85), NodeId(1, "demoVector"), "demoVector", fVector);
    s.addVariableNode(NodeId(0, 85), NodeId(1, "demoArray"), "demoArray", test);
    s.addVariableNode(NodeId(0, 85), NodeId(1, "demoInt"), "demoInt", 23);

    s.setDataSource(NodeId(1, "demoInt"), ds);

    s.addVariableNode(NodeId(0, 85), "demoFloat", 23.0f);
    s.addVariableNode(NodeId(0, 85), NodeId(1, "myStringId"), "demoFloat", 23.0f);
    s.run();
}