#include <opc/DataType.h>
#include <opc/Variant.h>
#include <opc/types/Types.h>
#include <open62541/nodeids.h>
#include <open62541/types.h>
#include <vector>

namespace opc
{
// datatype Ids
template <>
UA_NodeId getUADataTypeId<bool>()
{
    return UA_NODEID_NUMERIC(0, UA_NS0ID_BOOLEAN);
}

template <>
UA_NodeId getUADataTypeId<int8_t>()
{
    return UA_NODEID_NUMERIC(0, UA_NS0ID_SBYTE);
}

template <>
UA_NodeId getUADataTypeId<uint8_t>()
{
    return UA_NODEID_NUMERIC(0, UA_NS0ID_BYTE);
}

template <>
UA_NodeId getUADataTypeId<int16_t>()
{
    return UA_NODEID_NUMERIC(0, UA_NS0ID_INT16);
}

template <>
UA_NodeId getUADataTypeId<uint16_t>()
{
    return UA_NODEID_NUMERIC(0, UA_NS0ID_UINT16);
}

template <>
UA_NodeId getUADataTypeId<int32_t>()
{
    return UA_NODEID_NUMERIC(0, UA_NS0ID_INT32);
}

template <>
UA_NodeId getUADataTypeId<uint32_t>()
{
    return UA_NODEID_NUMERIC(0, UA_NS0ID_UINT32);
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
UA_NodeId getUADataTypeId<opc::types::LocalizedText>()
{
    return UA_NODEID_NUMERIC(0, UA_NS0ID_LOCALIZEDTEXT);
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
UA_NodeId getUADataTypeId<std::vector<std::string>>()
{
    return getUADataTypeId<std::string>();
}

// datatypes

template <>
const UA_DataType *getDataType<bool>()
{
    return &UA_TYPES[UA_TYPES_BOOLEAN];
}

template <>
const UA_DataType *getDataType<int8_t>()
{
    return &UA_TYPES[UA_TYPES_SBYTE];
}

template <>
const UA_DataType *getDataType<uint8_t>()
{
    return &UA_TYPES[UA_TYPES_BYTE];
}

template <>
const UA_DataType *getDataType<char>()
{
    return &UA_TYPES[UA_TYPES_SBYTE];
}

template <>
const UA_DataType *getDataType<int16_t>()
{
    return &UA_TYPES[UA_TYPES_INT16];
}

template <>
const UA_DataType *getDataType<uint16_t>()
{
    return &UA_TYPES[UA_TYPES_UINT16];
}

template <>
const UA_DataType *getDataType<int32_t>()
{
    return &UA_TYPES[UA_TYPES_INT32];
}

template <>
const UA_DataType *getDataType<uint32_t>()
{
    return &UA_TYPES[UA_TYPES_UINT32];
}

template <>
const UA_DataType *getDataType<long>()
{
    if (sizeof(long) == 4)
    {
        return &UA_TYPES[UA_TYPES_INT32];
    }
    else if (sizeof(long) == 8)
    {
        return &UA_TYPES[UA_TYPES_INT64];
    }
}

template <>
const UA_DataType *getDataType<unsigned long>()
{
    if (sizeof(unsigned long) == 4)
    {
        return &UA_TYPES[UA_TYPES_UINT32];
    }
    else if (sizeof(unsigned long) == 8)
    {
        return &UA_TYPES[UA_TYPES_UINT64];
    }
}

template <>
const UA_DataType *getDataType<float>()
{
    return &UA_TYPES[UA_TYPES_FLOAT];
}

template <>
const UA_DataType *getDataType<double>()
{
    return &UA_TYPES[UA_TYPES_DOUBLE];
}

template <>
const UA_DataType *getDataType<std::string>()
{
    return &UA_TYPES[UA_TYPES_STRING];
}

template <>
const UA_DataType *getDataType<std::vector<std::string>>()
{
    return &UA_TYPES[UA_TYPES_STRING];
}

} // namespace opc