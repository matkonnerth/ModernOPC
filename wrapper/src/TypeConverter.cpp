#include <TypeConverter.h>

namespace TypeConverter
{


opc::NodeId
fromUaNodeId(const UA_NodeId &id) {

    auto nsIdx = id.namespaceIndex;
    switch(id.identifierType) {
        case UA_NODEIDTYPE_NUMERIC:
            return opc::NodeId(nsIdx, static_cast<int>(id.identifier.numeric));
            break;
        case UA_NODEIDTYPE_STRING:
            return opc::NodeId(
                nsIdx, std::string(reinterpret_cast<char *>(id.identifier.string.data),
                                   id.identifier.string.length));
            break;
        case UA_NODEIDTYPE_BYTESTRING:
        case UA_NODEIDTYPE_GUID:
            assert(false);
            break;
    }
    return opc::NodeId(0, 0);
}

UA_NodeId
fromNodeId(const opc::NodeId &nodeId) {
    UA_NodeId id;
    UA_NodeId_init(&id);
    id.namespaceIndex = static_cast<UA_UInt16>(nodeId.getNsIdx());
    switch(nodeId.getIdType()) {
        case opc::NodeId::IdentifierType::NUMERIC:
            id.identifierType = UA_NodeIdType::UA_NODEIDTYPE_NUMERIC;
            id.identifier.numeric =
                static_cast<UA_UInt32>(std::get<int>(nodeId.getIdentifier()));
            break;
        case opc::NodeId::IdentifierType::STRING:
            id.identifierType = UA_NodeIdType::UA_NODEIDTYPE_STRING;
            id.identifier.string =
                UA_STRING((char *)std::get<std::string>(nodeId.getIdentifier()).c_str());
            break;
        default:
            break;
    }
    return id;
}

}  // namespace TypeConverter