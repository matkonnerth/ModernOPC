#include <gtest/gtest.h>
#include <modernOpc/Server.h>
#include <modernOpc/nodes/Node.h>
#include <modernOpc/nodes/ObjectNode.h>
#include <modernOpc/types/NodeId.h>
#include <optional>
#include <string>
//#include <generated.h>

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
    UA_Byte val;
    ASSERT_TRUE(obj->eventNotifier(val)==UA_STATUSCODE_GOOD);
    ASSERT_EQ(val, 1);
}

TEST(Object, get_EventNotifier_ObjcetsFolder)
{
    opc::Server s;
    auto obj = s.getObject(NodeId(0, 85));
    UA_Byte val;
    ASSERT_TRUE(obj->eventNotifier(val) == UA_STATUSCODE_GOOD);
    ASSERT_EQ(val, 0);
}

TEST(Object, get_EventNotifier_Variable)
{
    opc::Server s;
    auto obj = s.getObject(NodeId(0, 2256));
    ASSERT_FALSE(obj);
}

TEST(Object, buildINfo)
{
    opc::Server s;
    auto obj = s.getObjectsFolder();
    //opc::BuildInfo info;
    //obj->addVariable(NodeId(1, "myBuildInfo"), NodeId(0,0), QualifiedName(1, "test"), info);
}
