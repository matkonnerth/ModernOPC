#include <gtest/gtest.h>
#include <opc/Server.h>
#include <opc/Variant.h>
#include <opc/nodes/VariableNode.h>
#include <opc/types/NodeId.h>
#include <string>
#include <vector>
#include <opc/client/Client.h>
#include <future>

using opc::NodeId;
using opc::UnresolvedNodeId;
using namespace std::string_literals;

TEST(Client, read)
{
    opc::Server s;
    auto hdl = std::async(std::launch::async, [&]{s.run();});
    opc::Client c{"opc.tcp://localhost:4840"};
    c.connect();

    auto id = c.resolve(UnresolvedNodeId("http://opcfoundation.org/UA/", "i=2255"));
    auto var = c.read(id);
    ASSERT_TRUE(var.is_a<std::vector<std::string>>());
    s.stop();
}