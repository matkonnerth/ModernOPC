#include <gtest/gtest.h>
#include <opc/types/NodeId.h>
#include <string>
#include <opc/Server.h>
#include <opc/nodes/Node.h>
#include <optional>
#include <opc/nodes/ObjectNode.h>

using opc::NodeId;
using opc::QualifiedName;
using namespace std::string_literals;

TEST(Object, add)
{
    opc::Server s;

    ASSERT_TRUE(s.getObjectsFolder()->addObject(NodeId(1,234),
                   QualifiedName(1,"test"),
                   nullptr));
    ASSERT_FALSE(s.getObject(NodeId(0, 0)));
}

TEST(Object, get) 
{ 
    opc::Server s; 
    auto obj = s.getObject(NodeId(0,85));
    ASSERT_EQ(obj->displayName().text(), "Objects"s);
    ASSERT_EQ(obj->browseName().name(), "Objects"s);
}

TEST(Object, get_noObjectFound)
{
    opc::Server s;
    auto obj = s.getObject(NodeId(233, 85));
}

TEST(Object, get_EventNotifier_Server)
{
    opc::Server s;
    auto obj = s.getObject(NodeId(0, 2253));
    std::byte val;
    ASSERT_TRUE(obj->eventNotifier(val)==UA_STATUSCODE_GOOD);
    ASSERT_EQ(val, std::byte{1});
}

TEST(Object, get_EventNotifier_ObjcetsFolder)
{
    opc::Server s;
    auto obj = s.getObject(NodeId(0, 85));
    std::byte val;
    ASSERT_TRUE(obj->eventNotifier(val) == UA_STATUSCODE_GOOD);
    ASSERT_EQ(val, std::byte{0});
}

TEST(Object, get_EventNotifier_Variable)
{
    opc::Server s;
    auto obj = s.getObject(NodeId(0, 2256));
    ASSERT_FALSE(obj);
}
