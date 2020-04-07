#include <gtest/gtest.h>
#include <opc/types/NodeId.h>
#include <string>
#include <opc/Server.h>

using opc::NodeId;
using namespace std::string_literals;

TEST(Object, add)
{
    opc::Server s;
    ASSERT_TRUE(s.addObject(NodeId(0, 85), NodeId(1,234),
                   NodeId(0,0), "test",
                   nullptr));
    ASSERT_FALSE(s.addObject(NodeId(0, 0), NodeId(1, 234), NodeId(0, 0), "test",
                            nullptr));
}
