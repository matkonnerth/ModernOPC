#include <array>
#include <functional>
#include <iostream>
#include <vector>
#include "DataSource.h"
#include "Variant.h"
#include "NodeId.h"
#include "Server.h"

void
add(int a, int b, double c) {
    std::cout << a + b << std::endl;
}

static int test = 0;

void
getValue(opc::Variant& var) {    
    test++;
    var(test);
}

void setValue(opc::Variant&var)
{
    double test = var.get<double>();
}


void
getVectorValue(opc::Variant &var) {
    std::vector<int> vec;
    vec.push_back(3);
    vec.push_back(4);
    vec.push_back(5);
    var(std::move(vec));
}


void
setVectorValue(opc::Variant &var) {
    std::vector<int> vec = var.get<std::vector<int>>();
    for(auto& elem : vec)
    {
        std::cout << elem << std::endl;
    }
}

int
main() {

    opc::Server s;
    s.addMethod("addMethod", &add);

    NodeId id{3, 1234};

    std::array<int, 3> test{12, 13, 14};

    std::vector<float> fVector{1.1f, 2.2f, 3.3f};

    opc::DataSource ds{getValue,setValue};
    opc::DataSource d2{getVectorValue, setVectorValue};

    s.addVariableNode(NodeId(0, 85), NodeId(1, "demoVector"), "demoVector", fVector);
    s.addVariableNode(NodeId(0, 85), NodeId(1, "demoArray"), "demoArray", test);
    s.addVariableNode(NodeId(0, 85), NodeId(1, "demoInt"), "demoInt", 23);

    s.addVariableNode(NodeId(0, 85), NodeId(1, "IntVector"), "IntVector", test);

    s.setDataSource(NodeId(1, "IntVector"), d2);
    s.setDataSource(NodeId(1, "demoInt"), ds);

    s.addVariableNode(NodeId(0, 85), "demoFloat", 23.0f);
    s.addVariableNode(NodeId(0, 85), NodeId(1, "myStringId"), "demoFloat", 23.0f);
    s.run();
}