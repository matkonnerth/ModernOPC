#include <array>
#include <functional>
#include <iostream>
#include <vector>
#include <DataSource.h>
#include <Variant.h>
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

void setValue(opc::Variant& var)
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

void
NodesetLoader_unload(std::vector<std::string> s1, std::string s2, std::string s3, float f) {
    for(auto&s : s1)
    {
        std::cout << s;
        std::cout << std::endl;
    }
    std::cout << s2 << s3 << f << std::endl;
}

int
main() {

    opc::Server s;
    std::vector<float> fVector{1.1f, 2.2f, 3.3f};

    opc::DataSource ds{getValue,setValue};
    opc::DataSource d2{getVectorValue, setVectorValue};

    //adding of variable nodes
    s.addVariableNode(NodeId(0, 85), NodeId(1, "demoVector"), "demoVector", fVector);
    s.addVariableNode(NodeId(0, 85), NodeId(1, "demoArray"), "demoArray", std::array<int, 3> {12, 13, 14});
    s.addVariableNode(NodeId(0, 85), NodeId(1, "demoInt"), "demoInt", 23);
    s.addVariableNode(NodeId(0, 85), NodeId(1, "IntVector"), "IntVector", test);

    s.setDataSource(NodeId(1, "IntVector"), d2);
    s.setDataSource(NodeId(1, "demoInt"), ds);

    s.addVariableNode(NodeId(0, 85), "demoFloat", 23.0f);
    s.addVariableNode(NodeId(0, 85), NodeId(1, "myStringId"), "demoFloat", 23.0f);

    //loading of a xml nodeset
    s.loadNodeset("../models/serviceobject.xml");
    std::function<void(std::string)> load = std::bind(&opc::Server::loadNodeset, &s, std::placeholders::_1);

    //bind opc ua methods to business logic
    s.bindMethodNode(NodeId(2,7003), load);
    s.bindMethodNode(NodeId(2, 7004),
                     std::function<void(std::vector<std::string>, std::string, std::string, float)>{NodesetLoader_unload});

    //not really useful now, lacks parent node id
    s.addMethod("addMethod", &add);

    s.run();
}