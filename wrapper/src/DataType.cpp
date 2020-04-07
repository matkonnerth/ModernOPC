#include <opc/DataType.h>
#include <opc/Variant.h>
#include <opc/types/Types.h>
#include <open62541/nodeids.h>
#include <open62541/types.h>
#include <vector>

namespace opc
{
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
    return getDataType<std::string>();
}

template <>
const UA_DataType *getDataType<std::vector<int>>()
{
    return getDataType<int>();
}

} // namespace opc