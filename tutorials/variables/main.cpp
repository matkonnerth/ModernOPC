#include "MyDataSource.h"
#include <chrono>
#include <functional>
#include <iostream>
#include <opc/DataSource.h>
#include <opc/NodeId.h>
#include <opc/Server.h>
#include <opc/Variant.h>
#include <thread>
#include <vector>

auto add(int a, int b, double c) { return a + b + c; }

static int test = 0;

void getValue(const opc::NodeId &id, opc::Variant &var)
{
    test++;
    var(test);
}

void setValue(const opc::NodeId &id, opc::Variant &var)
{
    auto test = var.get<double>();
    (void)test;
}

void getVectorValue(const opc::NodeId &id, opc::Variant &var)
{
    std::vector<int> vec;
    vec.push_back(33);
    vec.push_back(423);
    vec.push_back(5323);
    var(std::move(vec));
}

void setVectorValue(const opc::NodeId &id, opc::Variant &var)
{
    auto vec = var.get<std::vector<int>>();
    for (auto &elem : vec)
    {
        std::cout << elem << std::endl;
    }
}

int main()
{
    opc::Server s;
    s.addVariableNode(opc::NodeId(0, 85), opc::NodeId(1, 233),
                      "demoNode" + std::to_string(233), 42);

    std::vector<float> fVector{1.1f, 2.2f, 3.3f};

    MyDataSource source1{"source1"};

    s.registerDataSource("simpleVal", getValue, setValue);
    s.registerDataSource("vectorDataSource", getVectorValue,
                         [](const opc::NodeId &, opc::Variant &) {});

    s.registerDataSource(source1.getKey(),
                         [&](const opc::NodeId &id, opc::Variant &var) {
                             source1.read(id, var);
                         },
                         [&](const opc::NodeId &id, opc::Variant &var) {
                             source1.write(id, var);
                         });

    // adding of variable nodes
    s.addVariableNode(opc::NodeId(0, 85), opc::NodeId(1, "demoVector"),
                      "demoVector", fVector,
                      std::make_unique<opc::NodeMetaInfo>("vectorDataSource"));
    s.addVariableNode(opc::NodeId(0, 85), opc::NodeId(1, "demoInt"), "demoInt",
                      23, std::make_unique<opc::NodeMetaInfo>("simpleVal"));
    s.addVariableNode(opc::NodeId(0, 85), opc::NodeId(1, "IntVector"),
                      "IntVector", test);
    s.addVariableNode(opc::NodeId(0, 85), opc::NodeId(1, "source1Var"),
                      "source1Var", 12,
                      std::make_unique<opc::NodeMetaInfo>("source1"));
}