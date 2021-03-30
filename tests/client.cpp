#include <future>
#include <gtest/gtest.h>
#include <iostream>
#include <opc/Server.h>
#include <opc/Variant.h>
#include <opc/client/Client.h>
#include <opc/nodes/VariableNode.h>
#include <opc/types/NodeId.h>
#include <string>
#include <vector>
#include <opc/nodes/ObjectNode.h>

using opc::NodeId;
using opc::UnresolvedNodeId;
using namespace std::string_literals;
using opc::ObjectNode;

/*
TEST(Client, read)
{
    opc::Server s;
    auto hdl = std::async(std::launch::async, [&] { s.run(); });
    opc::Client c{"opc.tcp://localhost:4840"};
    c.connect();

    auto id =
        c.resolve(UnresolvedNodeId("http://opcfoundation.org/UA/", "i=2255"));
    auto var = c.read(id);
    ASSERT_TRUE(var.is_a<std::vector<std::string>>());
    s.stop();
}
*/

UA_StatusCode getValue(const opc::NodeId &id, opc::Variant &var)
{
    var(42);
    return UA_STATUSCODE_GOOD;
}

UA_StatusCode setValue(const opc::NodeId &id, const opc::Variant &var)
{
    auto test = var.get<int32_t>();
    std::cout << "setValue: " << test << "\n";
    return UA_STATUSCODE_GOOD;
}

TEST(Client, write)
{
    opc::Server s;
    /*
    auto root = s.getObjectsFolder();
    auto var = root->addVariable(opc::NodeId(1, "demoInt"),
                                 opc::NodeId(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                 opc::QualifiedName(1, "demoInt"), 27);
    var->connectCallback(
        std::make_unique<opc::DataSource>("simpleVal", getValue, setValue));*/
    auto hdl = std::async(std::launch::async, [&] { s.run(); });

    //opc::Client c{"opc.tcp://localhost:4840"};
    //c.connect();

    //c.write(opc::NodeId(2, "demoInt"), opc::Variant(20));
    s.stop();
}
