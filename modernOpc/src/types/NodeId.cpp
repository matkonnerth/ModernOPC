#include <cassert>
#include <modernOpc/Variant.h>
#include <modernOpc/types/NodeId.h>
#include <open62541/types.h>
#include <open62541/types_generated_handling.h>
#include <ostream>
#include <variant>

namespace modernopc
{

const NodeId fromUaNodeId(const UA_NodeId &id)
{

    auto nsIdx = id.namespaceIndex;
    switch (id.identifierType)
    {
    case UA_NODEIDTYPE_NUMERIC:
        return NodeId(nsIdx, static_cast<int>(id.identifier.numeric));
        break;
    case UA_NODEIDTYPE_STRING:
        return NodeId(nsIdx, std::string(reinterpret_cast<char *>(
                                             id.identifier.string.data),
                                         id.identifier.string.length));
        break;
    case UA_NODEIDTYPE_BYTESTRING:
    case UA_NODEIDTYPE_GUID:
        assert(false);
        break;
    }
    return NodeId(0, 0);
}

UA_NodeId fromNodeId(NodeId &nodeId)
{
    UA_NodeId id;
    UA_NodeId_init(&id);
    id.namespaceIndex = static_cast<UA_UInt16>(nodeId.getNsIdx());
    switch (nodeId.getIdType())
    {
    case NodeId::IdentifierType::NUMERIC:
        id.identifierType = UA_NodeIdType::UA_NODEIDTYPE_NUMERIC;
        id.identifier.numeric =
            static_cast<UA_UInt32>(std::get<int>(nodeId.getIdentifier()));
        break;
    case NodeId::IdentifierType::STRING:
        id.identifierType = UA_NodeIdType::UA_NODEIDTYPE_STRING;
        id.identifier.string = UA_STRING_ALLOC(
            std::get<std::string>(nodeId.getIdentifier()).c_str());
        break;
    }
    return id;
}

const UA_NodeId fromNodeId(const NodeId &nodeId)
{
    UA_NodeId id;
    UA_NodeId_init(&id);
    id.namespaceIndex = static_cast<UA_UInt16>(nodeId.getNsIdx());
    switch (nodeId.getIdType())
    {
    case NodeId::IdentifierType::NUMERIC:
        id.identifierType = UA_NodeIdType::UA_NODEIDTYPE_NUMERIC;
        id.identifier.numeric =
            static_cast<UA_UInt32>(std::get<int>(nodeId.getIdentifier()));
        break;
    case NodeId::IdentifierType::STRING:
        id.identifierType = UA_NodeIdType::UA_NODEIDTYPE_STRING;
        id.identifier.string = UA_STRING(const_cast<char *>(
            std::get<std::string>(nodeId.getIdentifier()).c_str()));
        break;
    }
    return id;
}

void toUAVariantImpl(const NodeId &id, UA_Variant *var)
{
    const UA_NodeId uaId = fromNodeId(id);
    UA_Variant_setScalarCopy(var, &uaId, modernopc::getDataType<NodeId>());
}

template <>
NodeId Variant::get<NodeId>() const
{
    return fromUaNodeId(*static_cast<UA_NodeId *>(variant->data));
}

std::ostream &operator<<(std::ostream &os, const NodeId &id)
{
    os << "ns=" << id.getNsIdx();

    switch (id.getIdType())
    {
    case NodeId::IdentifierType::NUMERIC:
        os << ";i=" << std::get<int>(id.getIdentifier());
        break;
    case NodeId::IdentifierType::STRING:
        os << ";s=" << std::get<std::string>(id.getIdentifier());
        break;
    }

    return os;
}

bool NodeId::operator==(const NodeId &other) const
{
    if (nsIdx != other.nsIdx)
        return false;
    if (type != other.type)
    {
        return false;
    }
    else
    {
        switch (type)
        {
        case NodeId::IdentifierType::NUMERIC:
            return std::get<int>(i) == std::get<int>(other.i);
            break;
        case NodeId::IdentifierType::STRING:
            return std::get<std::string>(i) == std::get<std::string>(other.i);
            break;
        }
    }
    return false;
}

std::size_t getHash(const NodeId &id)
{
    size_t h1 = std::hash<uint16_t>()(id.nsIdx);
    size_t h2 = std::hash<std::variant<int, std::string>>()(id.i);
    return h1 ^ (h2 << 1);
}

} // namespace modernopc
