#include <gtest/gtest.h>
#include <opc/Server.h>
#include <opc/types/NodeId.h>
#include <string>
#include <vector>

using opc::NodeId;
using namespace std::string_literals;

TEST(Server, read)
{
    opc::Server s;
    opc::Variant var {};
    ASSERT_TRUE(s.readValue(NodeId(0, 2255), var));
    ASSERT_TRUE(var.is_a<std::vector<std::string>>());
}
