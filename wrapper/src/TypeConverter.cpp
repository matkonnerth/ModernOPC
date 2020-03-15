#include <open62541/server.h>
#include <TypeConverter.h>
#include <Types.h>

namespace TypeConverter {

template <>
const UA_DataType *
getDataType<bool>() {
    return &UA_TYPES[UA_TYPES_BOOLEAN];
}

template <>
const UA_DataType *
getDataType<char>() {
    return &UA_TYPES[UA_TYPES_SBYTE];
}

template <>
const UA_DataType *
getDataType<long>() {
    if(sizeof(long) == 4) {
        return &UA_TYPES[UA_TYPES_INT32];
    } else if(sizeof(long) == 8) {
        return &UA_TYPES[UA_TYPES_INT64];
    }
}

template <>
const UA_DataType *
getDataType<int>() {
    if(sizeof(int) == 4) {
        return &UA_TYPES[UA_TYPES_INT32];
    } else if(sizeof(int) == 8) {
        return &UA_TYPES[UA_TYPES_INT64];
    }
}

template <>
const UA_DataType *
getDataType<float>() {
    return &UA_TYPES[UA_TYPES_FLOAT];
}

template <>
const UA_DataType *
getDataType<double>() {
    return &UA_TYPES[UA_TYPES_DOUBLE];
}

template <>
const UA_DataType *
getDataType<std::string>() {
    return &UA_TYPES[UA_TYPES_STRING];
}

template <>
const UA_DataType *
getDataType<opc::types::Range>() {
    return &UA_TYPES[UA_TYPES_RANGE];
}

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