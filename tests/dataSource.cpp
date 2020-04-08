#include <gtest/gtest.h>
#include <opc/Server.h>


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
    s.addVariableNode(opc::NodeId(0, 85), opc::NodeId(1, "demoInt"), "demoInt",
                      23, std::make_unique<opc::NodeMetaInfo>("simpleVal"));

    opc::Variant var;
    ASSERT_TRUE(s.readValue(opc::NodeId(1, "demoInt"), var));
    ASSERT_TRUE(var.is_a<int32_t>());
    ASSERT_EQ(var.get<int32_t>(), 42);

    ASSERT_TRUE(s.writeValue(opc::NodeId(1, "demoInt"), var));
}