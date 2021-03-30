#include <gtest/gtest.h>
#include <modernOpc/DataSource.h>
#include <modernOpc/Server.h>
#include <modernOpc/nodes/ObjectNode.h>
#include <modernOpc/nodes/VariableNode.h>

UA_StatusCode getValue(const opc::NodeId &id, opc::Variant &var)
{
    var(42);
    return UA_STATUSCODE_GOOD;
}

UA_StatusCode setValue(const opc::NodeId &id, const opc::Variant &var)
{
    auto test = var.get<int32_t>();
    return UA_STATUSCODE_GOOD;
}

TEST(DataSource, simple)
{
    opc::Server s;

    auto root = s.getObjectsFolder();
    auto var=root->addVariable(opc::NodeId(1, "demoInt"), opc::NodeId(0, UA_NS0ID_BASEDATAVARIABLETYPE), opc::QualifiedName(1,"demoInt"), 27);
    var->connectCallback(std::make_unique<opc::DataSource>("simpleVal", getValue, setValue));

    opc::Variant v;
    ASSERT_TRUE(s.getVariable(opc::NodeId(1, "demoInt"))->read(v));
    ASSERT_TRUE(v.is_a<int32_t>());
    ASSERT_EQ(v.get<int32_t>(), 42);

    ASSERT_TRUE(s.getVariable(opc::NodeId(1, "demoInt"))->write(v));
}