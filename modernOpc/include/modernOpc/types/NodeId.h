#pragma once
#include <modernOpc/DataType.h>
#include <open62541/types.h>
#include <variant>
#include <functional>
namespace modernopc
{
struct Guid
{
    uint32_t data1;
    uint16_t data2;
    uint16_t data3;
    uint8_t data4[8];
    friend std::size_t getHash(const Guid &id);
    bool operator==(const Guid &other) const;
};
class NodeId
{

  public:
    enum class IdentifierType
    {
        NUMERIC,
        STRING,
        GUID, 
        BYTESTRING
    };
    NodeId() : nsIdx(0), type(NodeId::IdentifierType::NUMERIC), i(0) {}

    NodeId(uint16_t nsIdx, int id)
        : nsIdx(nsIdx), type(NodeId::IdentifierType::NUMERIC), i(id)
    {
    }
    NodeId(uint16_t nsIdx, const std::string &id, NodeId::IdentifierType t = NodeId::IdentifierType::STRING)
        : nsIdx(nsIdx), type(t), i(id)
    {
    }

    NodeId(uint16_t nsIdx, Guid id)
        : nsIdx(nsIdx), type(NodeId::IdentifierType::GUID), i{id}
    {
    }

    uint16_t getNsIdx() const { return nsIdx; }
    auto getIdType() const { return type; }
    const std::variant<int, std::string, Guid> &getIdentifier() const { return i; }
    bool operator==(const NodeId &other) const;
    friend std::size_t getHash(const NodeId &id);

    friend std::ostream &operator<<(std::ostream &os, const NodeId &id);
    friend void toUAVariantImpl(const NodeId &qn, UA_Variant *var);
    friend UA_NodeId fromNodeId(NodeId &&nodeId);
    friend const UA_NodeId fromNodeId(const NodeId &nodeId);

  private:
    uint16_t nsIdx{0};
    IdentifierType type{IdentifierType::NUMERIC};
    std::variant<int, std::string, Guid> i{0};
};

const NodeId fromUaNodeId(const UA_NodeId &id);

template <>
inline const UA_DataType *getDataType<NodeId>()
{
    return &UA_TYPES[UA_TYPES_NODEID];
}

} // namespace modernopc

namespace std
{
template <>
struct hash<modernopc::NodeId>
{
    inline size_t operator()(const modernopc::NodeId &id) const
    {
        return getHash(id);
    }
};
} // namespace std

namespace std
{
template <>
struct hash<modernopc::Guid>
{
    inline size_t operator()(const modernopc::Guid &id) const
    {
        return getHash(id);
    }
};
} // namespace std