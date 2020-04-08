#include <gtest/gtest.h>
#include <opc/types/NodeId.h>
#include <string>
#include <opc/Server.h>
#include <opc/nodes/Node.h>
#include <iostream>
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
    std::cout << obj->displayName() << std::endl;
}

TEST(Object, get_noObjectFound)
{
    opc::Server s;
    auto obj = s.getObject(NodeId(233, 85));
    if(obj)
    {
        std::cout << obj->displayName() << std::endl;
    }    
}
