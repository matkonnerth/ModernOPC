#include "Server.h"
#include "NodeId.h"
#include <iostream>



void add(int a, int b, double c)
{
    std::cout << a + b << std::endl;
}

int main() 
{
    opc::Server s;
    s.addMethod("addMethod", &add);

    NodeId id {3, 1234};

    s.addVariableNode(NodeId(0, 85), "demoInt", 23);
    s.addVariableNode(NodeId(0, 85), "demoFloat", 23.0f);
    s.addVariableNode(NodeId(0, 85), NodeId(1, "myStringId"), "demoFloat", 23.0f);
    s.run();
}