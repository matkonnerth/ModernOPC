#include "MyDataSource.h"
#include <chrono>
#include <functional>
#include <iostream>
#include <opc/DataSource.h>
#include <opc/Server.h>
#include <opc/Variant.h>
#include <opc/nodes/ObjectNode.h>
#include <opc/nodes/VariableNode.h>
#include <opc/types/NodeId.h>
#include <opc/types/QualifiedName.h>
#include <thread>
#include <vector>

using opc::NodeId;
using opc::QualifiedName;

void getValue(const opc::NodeId &id, opc::Variant &var)
{
    var(27.23);
}

void setValue(const opc::NodeId &id, opc::Variant &var)
{
    auto test = var.get<double>();
}

void getVectorValue(const opc::NodeId &id, opc::Variant &var)
{
    std::vector<int> vec {33,423,5323};
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
    auto root = s.getObjectsFolder();
    root->addVariable(NodeId(1, 233), NodeId(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                      QualifiedName(1, "demoNode" + std::to_string(233)),
                      12);

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
    auto var = root->addVariable(opc::NodeId(1, "demoVector"),
                                 NodeId(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                 QualifiedName(1, "demoVector"), fVector);
    var->connectCallback(
        std::make_unique<opc::NodeMetaInfo>("vectorDataSource"));

    var = root->addVariable(opc::NodeId(1, "demoInt"),
                            NodeId(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                            QualifiedName(1, "demoInt"), 23.12);
    var->connectCallback(std::make_unique<opc::NodeMetaInfo>("simpleVal"));

    s.run();
}