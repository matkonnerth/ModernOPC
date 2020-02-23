#include <DataSource.h>
#include <Variant.h>
#include <array>
#include <functional>
#include <iostream>
#include <vector>
#include "MyDataSource.h"
#include "NodeId.h"
#include "Server.h"

void
add(int a, int b, double c) {
    std::cout << a + b << std::endl;
}

static int test = 0;

void
getValue(const opc::NodeId &id, opc::Variant &var) {
    test++;
    var(test);
}

void
setValue(const opc::NodeId &id, opc::Variant &var) {
    auto test = var.get<double>();
    (void)test;
}

void
getVectorValue(const opc::NodeId &id, opc::Variant &var) {
    std::vector<int> vec;
    vec.push_back(33);
    vec.push_back(423);
    vec.push_back(5323);
    var(std::move(vec));
}

void
setVectorValue(const opc::NodeId &id, opc::Variant &var) {
    auto vec = var.get<std::vector<int>>();
    for(auto &elem : vec) {
        std::cout << elem << std::endl;
    }
}

void
NodesetLoader_unload(std::vector<std::string> s1, const std::string &s2,
                     const std::string &s3, float f) {
    for(auto &s : s1) {
        std::cout << s;
        std::cout << std::endl;
    }
    std::cout << s2 << s3 << f << std::endl;
}

int
main() {

    opc::Server s;
    std::vector<float> fVector{1.1f, 2.2f, 3.3f};

    MyDataSource source1{"source1"};

    s.registerDataSource(
        "simpleVal", getValue, setValue);
    s.registerDataSource(
        "vectorDataSource", getVectorValue, [](const opc::NodeId &, opc::Variant &) {});

    s.registerDataSource(
        source1.getKey(),
        [&](const opc::NodeId &id, opc::Variant &var) { source1.read(id, var); },
        [&](const opc::NodeId &id, opc::Variant &var) { source1.write(id, var);});

    // adding of variable nodes
    s.addVariableNode(opc::NodeId(0, 85), opc::NodeId(1, "demoVector"), "demoVector",
                      fVector, std::make_unique<opc::NodeMetaInfo>("vectorDataSource"));
    s.addVariableNode(opc::NodeId(0, 85), opc::NodeId(1, "demoArray"), "demoArray",
                      std::array<int, 3>{12, 13, 14});
    s.addVariableNode(opc::NodeId(0, 85), opc::NodeId(1, "demoInt"), "demoInt", 23,
                      std::make_unique<opc::NodeMetaInfo>("simpleVal"));
    s.addVariableNode(opc::NodeId(0, 85), opc::NodeId(1, "IntVector"), "IntVector", test);
    s.addVariableNode(opc::NodeId(0,85), opc::NodeId(1, "source1Var"), "source1Var", 12, std::make_unique<opc::NodeMetaInfo>("source1"));

    // loading of a xml nodeset
    s.loadNodeset("../models/serviceobject.xml");
    std::function<void(std::string)> load = [&](std::string path){s.loadNodeset(path);};

    // bind opc ua methods to business logic
    s.bindMethodNode(opc::NodeId(2, 7003), load);
    s.bindMethodNode(
        opc::NodeId(2, 7004),
        std::function<void(std::vector<std::string>, std::string, std::string, float)>{
            NodesetLoader_unload});

    // not really useful now, lacks parent node id
    s.addMethod("addMethod", &add);

    s.run();
}