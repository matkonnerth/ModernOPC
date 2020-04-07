#include <cassert>
#include <opc/DataType.h>
#include <opc/types/NodeId.h>
#include <open62541/types.h>
#include <open62541/types_generated_handling.h>
#include <ostream>
#include <variant>

namespace opc
{

template <>
const UA_DataType *getDataType<NodeId>()
{
    return &UA_TYPES[UA_TYPES_NODEID];
}

NodeId fromUaNodeId(const UA_NodeId &id)
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

UA_NodeId fromNodeId(const NodeId &nodeId)
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
    default:
        break;
    }
    return id;
}

void convertToUAVariantImpl(const NodeId &id, UA_Variant *var)
{
    UA_NodeId uaId = fromNodeId(id);
    UA_Variant_setScalarCopy(var, &uaId, opc::getDataType<NodeId>());
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

template <>
NodeId toStdType(UA_Variant *variant)
{
    return fromUaNodeId(*static_cast<UA_NodeId *>(variant->data));
}

} // namespace opc
