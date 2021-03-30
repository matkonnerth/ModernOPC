/**
 * Adding variables to objects
 * ---------------------------
 * This tutorial describes how to add variables and connect to Datasources.
 * Datasources are the glue between a read/write request on a variable, no
 * matter if triggered from a read request from a remote client or from a
 * MonitoredItem. Especiall for sampling with a low intervall (e.g. 50ms, means
 * a high frequency) this code must be really performant. */

#include <functional>
#include <iostream>
#include <modernOpc/DataSource.h>
#include <modernOpc/Server.h>
#include <modernOpc/Variant.h>
#include <modernOpc/nodes/ObjectNode.h>
#include <modernOpc/nodes/VariableNode.h>
#include <modernOpc/types/NodeId.h>
#include <modernOpc/types/QualifiedName.h>
#include <vector>

using modernopc::DataSource;
using modernopc::NodeId;
using modernopc::QualifiedName;

int val=27;

UA_StatusCode getValue(const modernopc::NodeId &id, modernopc::Variant &var)
{
    var(val);
    return UA_STATUSCODE_GOOD;
}

UA_StatusCode setValue(const modernopc::NodeId &id, const modernopc::Variant &var)
{
    val = var.get<int>();
    return UA_STATUSCODE_GOOD;
}

UA_StatusCode getVectorValue(const modernopc::NodeId &id, modernopc::Variant &var)
{
    std::vector<int> vec{33, 423, 5323};
    var(std::move(vec));
    return UA_STATUSCODE_GOOD;
}

UA_StatusCode setVectorValue(const modernopc::NodeId &id, const modernopc::Variant &var)
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
    modernopc::Server s;
    auto root = s.getObjectsFolder();
    root->addVariable(NodeId(1, 233), NodeId(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                      QualifiedName(1, "demoNode" + std::to_string(233)), 12);

    std::vector<float> fVector{1.1f, 2.2f, 3.3f};

    // adding of variable nodes
    auto var = root->addVariable(modernopc::NodeId(1, "demoVector"),
                                 NodeId(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                 QualifiedName(1, "demoVector"), fVector);
    var->connectCallback(std::make_unique<DataSource>(
        "vectorDataSource", getVectorValue, setVectorValue));

    var = root->addVariable(modernopc::NodeId(1, 42),
                            NodeId(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                            QualifiedName(1, "myVar"), 23);
    var->connectCallback(
        std::make_unique<DataSource>("simple", getValue, setValue));

    s.run();
}