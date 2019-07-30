#include "Server.h"
#include "NodeId.h"
#include <iostream>
#include <array>



void add(int a, int b, double c)
{
    std::cout << a + b << std::endl;
}

int main() 
{
    opc::Server s;
    s.addMethod("addMethod", &add);

    NodeId id {3, 1234};

    std::array<int,3> test{12, 13, 14};

    std::vector<float> fVector{1.1f, 2.2f, 3.3f};

    s.addVariableNode(NodeId(0, 85), NodeId(1, "demoVector"), "demoVector", fVector);
    s.addVariableNode(NodeId(0, 85), NodeId(1, "demoArray"), "demoArray", test);
    s.addVariableNode(NodeId(0, 85), "demoInt", 23);
    s.addVariableNode(NodeId(0, 85), "demoFloat", 23.0f);
    s.addVariableNode(NodeId(0, 85), NodeId(1, "myStringId"), "demoFloat", 23.0f);
    s.run();
}