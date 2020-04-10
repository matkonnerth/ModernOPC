#include <functional>
#include <iostream>
#include <opc/DataSource.h>
#include <opc/Server.h>
#include <opc/Variant.h>
#include <opc/nodes/ObjectNode.h>
#include <opc/nodes/VariableNode.h>
#include <opc/types/NodeId.h>
#include <opc/types/QualifiedName.h>
#include <vector>

using opc::DataSource;
using opc::NodeId;
using opc::QualifiedName;

UA_StatusCode getValue(const opc::NodeId &id, opc::Variant &var) { var(27.23); return UA_STATUSCODE_GOOD; }

UA_StatusCode setValue(const opc::NodeId &id, const opc::Variant &var)
{
    auto test = var.get<double>();
    return UA_STATUSCODE_GOOD;
}

UA_StatusCode getVectorValue(const opc::NodeId &id, opc::Variant &var)
{
    std::vector<int> vec{33, 423, 5323};
    var(std::move(vec));
    return UA_STATUSCODE_GOOD;
}

UA_StatusCode setVectorValue(const opc::NodeId &id, const opc::Variant &var)
{
    auto vec = var.get<std::vector<int>>();
    for (auto &elem : vec)
    {
        std::cout << elem << std::endl;
    }
    return UA_STATUSCODE_GOOD;
}

int main()
{
    opc::Server s;
    auto root = s.getObjectsFolder();
    root->addVariable(NodeId(1, 233), NodeId(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                      QualifiedName(1, "demoNode" + std::to_string(233)), 12);

    std::vector<float> fVector{1.1f, 2.2f, 3.3f};

    // adding of variable nodes
    auto var = root->addVariable(opc::NodeId(1, "demoVector"),
                                 NodeId(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                 QualifiedName(1, "demoVector"), fVector);
    var->connectCallback(std::make_unique<DataSource>(
        "vectorDataSource", getVectorValue, setVectorValue));

    var = root->addVariable(opc::NodeId(1, "demoInt"),
                            NodeId(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                            QualifiedName(1, "demoInt"), 23.12);
    var->connectCallback(
        std::make_unique<DataSource>("simple", getValue, setValue));

    s.run();
}