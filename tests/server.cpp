#include <gtest/gtest.h>
#include <opc/Server.h>
#include <opc/types/NodeId.h>
#include <string>
#include <vector>
#include <opc/Variant.h>
#include <opc/nodes/VariableNode.h>

using opc::NodeId;
using namespace std::string_literals;

TEST(Server, read)
{
    opc::Server s;
    opc::Variant var {};
    ASSERT_TRUE(s.getVariable(NodeId(0, 2255))->read(var));
    ASSERT_TRUE(var.is_a<std::vector<std::string>>());
}
