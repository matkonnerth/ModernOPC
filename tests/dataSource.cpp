#include <gtest/gtest.h>
#include <opc/Server.h>
#include <opc/nodes/ObjectNode.h>
#include <opc/nodes/VariableNode.h>


void getValue(const opc::NodeId &id, opc::Variant &var)
{
    var(42);
}

void setValue(const opc::NodeId &id, opc::Variant &var)
{
    ASSERT_TRUE(var.is_a<int32_t>());
    auto test = var.get<int32_t>();
    (void)test;
}

TEST(DataSource, simple)
{
    opc::Server s;
    s.registerDataSource("simpleVal", getValue, setValue);

    auto root = s.getObjectsFolder();
    auto var=root->addVariable(opc::NodeId(1, "demoInt"), opc::NodeId(0, UA_NS0ID_BASEDATAVARIABLETYPE), opc::QualifiedName(1,"demoInt"), 27);
    var->connectCallback(std::make_unique<opc::NodeMetaInfo>("simpleVal"));

    opc::Variant v;
    ASSERT_TRUE(s.getVariable(opc::NodeId(1, "demoInt"))->read(v));
    ASSERT_TRUE(v.is_a<int32_t>());
    ASSERT_EQ(v.get<int32_t>(), 42);

    ASSERT_TRUE(s.getVariable(opc::NodeId(1, "demoInt"))->write(v));
}