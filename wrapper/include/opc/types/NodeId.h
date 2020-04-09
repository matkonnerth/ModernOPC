#pragma once
#include <opc/DataType.h>
#include <open62541/types.h>
#include <variant>
namespace opc
{

class NodeId
{

  public:
    enum class IdentifierType
    {
        NUMERIC,
        STRING
    };
    NodeId(uint16_t nsIdx, int id)
        : nsIdx(nsIdx), type(NodeId::IdentifierType::NUMERIC), i(id)
    {
    }
    NodeId(uint16_t nsIdx, const std::string &id)
        : nsIdx(nsIdx), type(NodeId::IdentifierType::STRING), i(id)
    {
    }

    uint16_t getNsIdx() const { return nsIdx; }
    auto getIdType() const { return type; }
    const std::variant<int, std::string> &getIdentifier() const { return i; }
    bool operator==(const NodeId &other) const;
    friend std::size_t getHash(const NodeId &id);

    friend std::ostream &operator<<(std::ostream &os, const NodeId &id);
    friend void convertToUAVariantImpl(const NodeId &qn, UA_Variant *var);
    friend UA_NodeId fromNodeId(NodeId &nodeId);
    friend const UA_NodeId fromNodeId(const NodeId &nodeId);

  private:
    uint16_t nsIdx{0};
    IdentifierType type{IdentifierType::NUMERIC};
    std::variant<int, std::string> i{0};
};

const NodeId fromUaNodeId(const UA_NodeId &id);

template <>
inline const UA_DataType *getDataType<NodeId>()
{
    return &UA_TYPES[UA_TYPES_NODEID];
}

} // namespace opc

namespace std
{
template <>
struct hash<opc::NodeId>
{
    inline size_t operator()(const opc::NodeId &id) const
    {
        return getHash(id);
    }
};
} // namespace std