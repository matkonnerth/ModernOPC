#include <gtest/gtest.h>
#include <opc/types/NodeId.h>
#include <unordered_map>
#include <string>

using opc::NodeId;
using namespace std::string_literals;

TEST(NodeId, hash)
{
    NodeId numericId{1, 1};
    NodeId stringId{1, "test"s};
}

TEST(NodeId, hashmap)
{
    std::unordered_map<NodeId, int> map;
    map.emplace(NodeId{1,"test"s}, 1);
    map.emplace(NodeId{1,234}, 2);
    auto it = map.find(NodeId{1, "test"s});
    ASSERT_TRUE(it!=map.end());
    ASSERT_EQ(it->second, 1);
}
