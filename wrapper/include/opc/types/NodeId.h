#pragma once
#include <iostream>
#include <open62541/types.h>
#include <variant>
#include <opc/Variant.h>
namespace opc
{

class NodeId
{

  public:
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

    static std::size_t getIdentifierHash(const NodeId &id)
    {
        switch (id.type)
        {
        case IdentifierType::NUMERIC:
            return static_cast<std::size_t>(std::get<int>(id.i));
            break;
        case IdentifierType::STRING:
            return std::hash<std::string>()(std::get<std::string>(id.i));
            break;
        }
        return 0;
    }

    bool operator<(const NodeId &other) const
    {
        if (nsIdx < other.nsIdx)
            return true;
        if (other.nsIdx < nsIdx)
            return false;
        // nsIdx same
        return getIdentifierHash(*this) < getIdentifierHash(other);
    }

    enum class IdentifierType
    {
        NUMERIC,
        STRING
    };

    friend std::ostream &operator<<(std::ostream &os, const NodeId &id);
    friend void convertToUAVariantImpl(const NodeId &qn, UA_Variant *var);
    friend UA_NodeId fromNodeId(const NodeId &nodeId);

  private:
    uint16_t nsIdx{0};
    IdentifierType type{IdentifierType::NUMERIC};
    std::variant<int, std::string> i{0};
};

NodeId fromUaNodeId(const UA_NodeId &id);
template <>
NodeId toStdType(UA_Variant *variant);

} // namespace opc