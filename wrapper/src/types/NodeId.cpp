#include <opc/Conversion.h>
#include <opc/types/NodeId.h>

namespace opc
{
namespace types
{
opc::types::NodeId fromUaNodeId(const UA_NodeId &id)
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
        id.identifier.string = UA_STRING(
            (char *)std::get<std::string>(nodeId.getIdentifier()).c_str());
        break;
    default:
        break;
    }
    return id;
}
} // namespace types
} // namespace opc
