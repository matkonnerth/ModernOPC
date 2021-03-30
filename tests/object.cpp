#include <gtest/gtest.h>
#include <modernOpc/Server.h>
#include <modernOpc/nodes/Node.h>
#include <modernOpc/nodes/ObjectNode.h>
#include <modernOpc/types/NodeId.h>
#include <optional>
#include <string>
//#include <generated.h>

using modernopc::NodeId;
using modernopc::QualifiedName;
using namespace std::string_literals;

TEST(Object, add)
{
    modernopc::Server s;

    ASSERT_TRUE(s.getObjectsFolder()->addObject(NodeId(1,234),
                   QualifiedName(1,"test"),
                   nullptr));
    ASSERT_FALSE(s.getObject(NodeId(0, 0)));
}

TEST(Object, get) 
{ 
    modernopc::Server s; 
    auto obj = s.getObject(NodeId(0,85));
    ASSERT_EQ(obj->displayName().text(), "Objects"s);
    ASSERT_EQ(obj->browseName().name(), "Objects"s);
}

TEST(Object, get_noObjectFound)
{
    modernopc::Server s;
    auto obj = s.getObject(NodeId(233, 85));
}

TEST(Object, get_EventNotifier_Server)
{
    modernopc::Server s;
    auto obj = s.getObject(NodeId(0, 2253));
    UA_Byte val;
    ASSERT_TRUE(obj->eventNotifier(val)==UA_STATUSCODE_GOOD);
    ASSERT_EQ(val, 1);
}

TEST(Object, get_EventNotifier_ObjcetsFolder)
{
    modernopc::Server s;
    auto obj = s.getObject(NodeId(0, 85));
    UA_Byte val;
    ASSERT_TRUE(obj->eventNotifier(val) == UA_STATUSCODE_GOOD);
    ASSERT_EQ(val, 0);
}

TEST(Object, get_EventNotifier_Variable)
{
    modernopc::Server s;
    auto obj = s.getObject(NodeId(0, 2256));
    ASSERT_FALSE(obj);
}

TEST(Object, buildINfo)
{
    modernopc::Server s;
    auto obj = s.getObjectsFolder();
    //modernopc::BuildInfo info;
    //obj->addVariable(NodeId(1, "myBuildInfo"), NodeId(0,0), QualifiedName(1, "test"), info);
}
