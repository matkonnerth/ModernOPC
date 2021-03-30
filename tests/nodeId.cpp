#include <gtest/gtest.h>
#include <modernOpc/types/NodeId.h>
#include <string>
#include <unordered_map>

using modernopc::NodeId;
using namespace std::string_literals;

TEST(NodeId, mem)
{
    NodeId numericId{1, 1};
    NodeId stringId{1, "test"s};
}

TEST(NodeId, hashmap)
{
    std::unordered_map<NodeId, int> map;
    map.emplace(NodeId{1,"test"s}, 1);
    map.emplace(NodeId{1,2222}, 2);
    auto it = map.find(NodeId{1, "test"s});
    ASSERT_TRUE(it!=map.end());
    ASSERT_EQ(it->second, 1);
    it = map.find(NodeId{1, 2222});
    ASSERT_TRUE(it != map.end());
    ASSERT_EQ(it->second, 2);
}

TEST(NodeId, conversion_constNodeId_to_constUANodeId)
{
    const NodeId cId(1, "test"s);
    const UA_NodeId& uacId = fromNodeId(cId);
    //cannot bind to non const lvalue -> good
    //UA_NodeId &uaId = fromNodeId(cId);
    // assignment not possible -> good
    //uaId.identifierType = UA_NODEIDTYPE_NUMERIC;
    //assign
    //uaId.identifier.string.length=2134;
}

TEST(NodeId, conversion_NodeId_to_UANodeId)
{
    NodeId cId(1, "test"s);
    UA_NodeId uaId = fromNodeId(cId);
    UA_clear(&uaId, &UA_TYPES[UA_TYPES_NODEID]);
}

TEST(NodeId, conversion_constUaNodeId_to_constNodeId)
{
    const UA_NodeId uacId = UA_NODEID_STRING(3, (char*)"string");
    const NodeId& cid = modernopc::fromUaNodeId(uacId);
    //cannot bind -> good
    //NodeId& id = modernopc::fromUaNodeId(uacId);
}
