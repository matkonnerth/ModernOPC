#pragma once
#include <array>
#include <cassert>
#include <memory>
#include <opc/types/NodeId.h>
#include <opc/types/Types.h>
#include <open62541/server.h>
#include <vector>
#include <string>

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

//void convertToUAvariantImpl(std::string val, UA_Variant *var);
//void convertToUAvariantImpl(std::string& val, UA_Variant *var);
//void convertToUAvariantImpl(const std::string &val, UA_Variant *var);
//void convertToUAvariantImpl(std::string &&val, UA_Variant *var);

//void convertToUAVariantImpl(std::string v, UA_Variant *var);
//void convertToUAVariantImpl(std::string &v, UA_Variant *var);
//void convertToUAVariantImpl(const std::string &v, UA_Variant *var);
//void convertToUAVariantImpl(std::string &&v, UA_Variant *var);

template <class T>
struct HasconvertToUAVariantImpl_
{
    template <typename C>
    static std::true_type
    test(decltype(convertToUAVariantImpl(std::declval<C>(),
                                         std::declval<UA_Variant *>())) *);
    template <typename C>
    static std::false_type test(...);
    typedef decltype(test<T>(0)) type;
};

template <typename T>
using HasconvertToUAVariantImpl = typename HasconvertToUAVariantImpl_<T>::type;

template <typename T>
typename std::enable_if<HasconvertToUAVariantImpl<T>::value, void>::type
convertToUAVariant(T &&t, UA_Variant *var)
{
    convertToUAVariantImpl(std::forward<T>(t), var);
}

template <typename T>
typename std::enable_if<!HasconvertToUAVariantImpl<T>::value, void>::type
convertToUAVariant(T &&val, UA_Variant *var)
{
    static_assert(std::is_arithmetic_v<std::remove_reference_t<T>>, "Type not integral");
    UA_Variant_init(var);
    UA_Variant_setScalarCopy(var, &val, getDataType<std::remove_reference_t<T>>());
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
    convertToUAVariant(val, &attr.value);
    return attr;
}

template <typename T>
UA_VariableAttributes getVariableAttributes(std::vector<T> &v)
{
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    convertToUAVariant(v, &attr.value);
    attr.dataType = getUADataTypeId<T>();
    attr.valueRank = 1;
    attr.arrayDimensionsSize = 1;
    attr.arrayDimensions = new UA_UInt32{static_cast<UA_UInt32>(v.size())};
    return attr;
}

template <typename T>
T toStdType(UA_Variant *variant);

std::string fromUAString(const UA_String *s);

} // namespace opc

namespace std
{
template <typename T>
void convertToUAVariantImpl(T&& val, UA_Variant *var);

// void convertToUAVariantImpl(std::string v, UA_Variant *var);
//template <>
//void convertToUAVariantImpl(std::string &v, UA_Variant *var);
// void convertToUAVariantImpl(const std::string &v, UA_Variant *var);
// void convertToUAVariantImpl(std::string &&v, UA_Variant *var);

//template <>


//template <>



template <typename T>
void convertToUAVariantImpl(std::vector<T> val, UA_Variant *var)
{
    static_assert(std::is_arithmetic_v<T>, "Type not integral");
    UA_Variant_init((UA_Variant *)var);
    UA_Variant_setArrayCopy(var, val.data(), val.size(), opc::getDataType<T>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

template <>
void convertToUAVariantImpl(std::vector<std::string> v, UA_Variant *var);




}