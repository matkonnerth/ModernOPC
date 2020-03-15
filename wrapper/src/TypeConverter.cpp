#include <TypeConverter.h>
#include <Types.h>
#include <open62541/server.h>

namespace opc
{

template <>
UA_NodeId getUADataTypeId<bool>()
{
    return UA_NODEID_NUMERIC(0, UA_NS0ID_BOOLEAN);
}

template <>
UA_NodeId getUADataTypeId<int>()
{
    return UA_NODEID_NUMERIC(0, UA_NS0ID_INT32);
}

template <>
UA_NodeId getUADataTypeId<float>()
{
    return UA_NODEID_NUMERIC(0, UA_NS0ID_FLOAT);
}

template <>
UA_NodeId getUADataTypeId<double>()
{
    return UA_NODEID_NUMERIC(0, UA_NS0ID_DOUBLE);
}

template <>
UA_NodeId getUADataTypeId<std::vector<int>>()
{
    return getUADataTypeId<int>();
}

template <>
UA_NodeId getUADataTypeId<void>()
{
    return UA_NODEID_NULL;
}

template <>
UA_NodeId getUADataTypeId<unsigned long>()
{
    return UA_NODEID_NUMERIC(0, UA_NS0ID_UINT64);
}

template <>
UA_NodeId getUADataTypeId<std::string>()
{
    return UA_NODEID_NUMERIC(0, UA_NS0ID_STRING);
}

template <>
bool toStdType(UA_Variant *variant)
{
    return *static_cast<bool *>(variant->data);
}

template <>
double toStdType(UA_Variant *variant)
{
    return *static_cast<double *>(variant->data);
}

template <>
int toStdType(UA_Variant *variant)
{
    return *static_cast<int *>(variant->data);
}

template <>
std::vector<int> toStdType(UA_Variant *variant)
{
    return std::vector<int>{static_cast<int *>(variant->data),
                            static_cast<int *>(variant->data) +
                                variant->arrayLength};
}

template <>
std::vector<uint> toStdType(UA_Variant *variant)
{
    return std::vector<uint>{static_cast<uint *>(variant->data),
                            static_cast<uint *>(variant->data) +
                                variant->arrayLength};
}

template <>
std::string toStdType(UA_Variant *variant)
{
    UA_String *s = (UA_String *)variant->data;
    if (s->length > 0)
    {
        std::string stdString{reinterpret_cast<char *>(s->data), s->length};
        return stdString;
    }
    return std::string{};
}

template <>
std::vector<std::string> toStdType(UA_Variant *variant)
{
    std::vector<std::string> vec;
    for (size_t i = 0; i < variant->arrayLength; i++)
    {
        UA_String *s = ((UA_String *)variant->data) + i;
        if (s->length > 0)
        {
            std::string stdString{reinterpret_cast<char *>(s->data), s->length};
            vec.push_back(stdString);
        }
        else
        {
            vec.push_back(std::string{});
        }
    }
    return vec;
}

opc::NodeId fromUaNodeId(const UA_NodeId &id)
{

    auto nsIdx = id.namespaceIndex;
    switch (id.identifierType)
    {
    case UA_NODEIDTYPE_NUMERIC:
        return opc::NodeId(nsIdx, static_cast<int>(id.identifier.numeric));
        break;
    case UA_NODEIDTYPE_STRING:
        return opc::NodeId(nsIdx, std::string(reinterpret_cast<char *>(
                                                  id.identifier.string.data),
                                              id.identifier.string.length));
        break;
    case UA_NODEIDTYPE_BYTESTRING:
    case UA_NODEIDTYPE_GUID:
        assert(false);
        break;
    }
    return opc::NodeId(0, 0);
}

UA_NodeId fromNodeId(const opc::NodeId &nodeId)
{
    UA_NodeId id;
    UA_NodeId_init(&id);
    id.namespaceIndex = static_cast<UA_UInt16>(nodeId.getNsIdx());
    switch (nodeId.getIdType())
    {
    case opc::NodeId::IdentifierType::NUMERIC:
        id.identifierType = UA_NodeIdType::UA_NODEIDTYPE_NUMERIC;
        id.identifier.numeric =
            static_cast<UA_UInt32>(std::get<int>(nodeId.getIdentifier()));
        break;
    case opc::NodeId::IdentifierType::STRING:
        id.identifierType = UA_NodeIdType::UA_NODEIDTYPE_STRING;
        id.identifier.string = UA_STRING(
            (char *)std::get<std::string>(nodeId.getIdentifier()).c_str());
        break;
    default:
        break;
    }
    return id;
}

}