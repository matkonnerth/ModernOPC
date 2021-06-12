#include <gtest/gtest.h>
#include <modernopc/DataSource.h>
#include <modernopc/Server.h>
#include <modernopc/nodes/ObjectNode.h>
#include <modernopc/nodes/VariableNode.h>

UA_StatusCode getValue(const modernopc::NodeId &id, modernopc::Variant &var)
{
    var(42);
    return UA_STATUSCODE_GOOD;
}

UA_StatusCode setValue(const modernopc::NodeId &id, const modernopc::Variant &var)
{
    auto test = var.get<int32_t>();
    return UA_STATUSCODE_GOOD;
}

TEST(DataSource, simple)
{
    modernopc::Server s;

    auto root = s.getObjectsFolder();
    auto var=root->addVariable(modernopc::NodeId(1, "demoInt"), modernopc::NodeId(0, UA_NS0ID_BASEDATAVARIABLETYPE), modernopc::QualifiedName(1,"demoInt"), 27);
    var->connectCallback(std::make_unique<modernopc::DataSource>("simpleVal", getValue, setValue));

    modernopc::Variant v;
    ASSERT_TRUE(s.getVariable(modernopc::NodeId(1, "demoInt"))->read(v));
    ASSERT_TRUE(v.is_a<int32_t>());
    ASSERT_EQ(v.get<int32_t>(), 42);

    ASSERT_TRUE(s.getVariable(modernopc::NodeId(1, "demoInt"))->write(v));
}