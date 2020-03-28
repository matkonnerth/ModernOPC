#pragma once
#include <NodeId.h>
#include <array>
#include <memory>
#include <open62541/server.h>
#include <vector>
#include <cassert>
#include <Types.h>

namespace opc
{

template <class T>
struct is_vector : public std::false_type
{
};

template <class T, class Alloc>
struct is_vector<std::vector<T, Alloc>> : public std::true_type
{
};

template <typename T>
const UA_DataType *getDataType();

template <>
inline const UA_DataType *getDataType<bool>()
{
    return &UA_TYPES[UA_TYPES_BOOLEAN];
}

template <>
inline const UA_DataType *getDataType<char>()
{
    return &UA_TYPES[UA_TYPES_SBYTE];
}

template <>
inline const UA_DataType *getDataType<long>()
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
inline const UA_DataType *getDataType<unsigned long>()
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
inline const UA_DataType *getDataType<int>()
{
    if (sizeof(int) == 4)
    {
        return &UA_TYPES[UA_TYPES_INT32];
    }
    else if (sizeof(int) == 8)
    {
        return &UA_TYPES[UA_TYPES_INT64];
    }
}

template <>
inline const UA_DataType *getDataType<float>()
{
    return &UA_TYPES[UA_TYPES_FLOAT];
}

template <>
inline const UA_DataType *getDataType<double>()
{
    return &UA_TYPES[UA_TYPES_DOUBLE];
}

template <>
inline const UA_DataType *getDataType<std::string>()
{
    return &UA_TYPES[UA_TYPES_STRING];
}

template <>
inline const UA_DataType *getDataType<std::vector<std::string>>()
{
    return &UA_TYPES[UA_TYPES_STRING];
}


template <typename T>
void toUAVariant(T val, UA_Variant *var)
{
    UA_Variant_init(var);
    UA_Variant_setScalarCopy(var, &val, getDataType<T>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

template <typename T>
void toUAVariant(std::vector<T> v, UA_Variant *var)
{
    UA_Variant_init(var);
    UA_Variant_setArrayCopy(var, v.data(), v.size(), getDataType<T>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}


template <typename T>
UA_NodeId getUADataTypeId();

template <typename T>
UA_VariableAttributes getVariableAttributes(T val)
{
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.dataType = getUADataTypeId<T>();
    attr.valueRank = -1;
    toUAVariant(val, &attr.value);
    return attr;
}

template <typename T>
UA_VariableAttributes getVariableAttributes(std::vector<T> &v)
{
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    toUAVariant(v, &attr.value);
    attr.dataType = getUADataTypeId<T>();
    attr.valueRank = 1;
    attr.arrayDimensionsSize = 1;
    attr.arrayDimensions = new UA_UInt32{static_cast<UA_UInt32>(v.size())};
    return attr;
}

template <typename T>
T toStdType(UA_Variant *variant);

opc::NodeId fromUaNodeId(const UA_NodeId &id);
UA_NodeId fromNodeId(const opc::NodeId &nodeId);

types::LocalizedText fromUALocalizedText(const UA_LocalizedText *lt);

} // namespace TypeConverter