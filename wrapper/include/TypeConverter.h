#pragma once
#include <open62541/server.h>
#include <array>
#include <iostream>
#include <memory>
#include <typeinfo>
#include <vector>

namespace TypeConverter {

template <typename T>
constexpr bool isSupportedCppType()
{
    if(std::is_arithmetic<T>::value)
    {
        return true;
    }
    return false;
}

template<typename T>
bool isTypeMatching(const UA_DataType* uatype)
{
    using type = typename std::remove_reference<T>::type;
    if(uatype->memSize == sizeof(type))
    {
        return true;
    }    
    return false;
}

template <typename T>
const UA_DataType *
getDataType() {
    if(std::is_same<T, bool>::value)
    {
        return &UA_TYPES[UA_TYPES_BOOLEAN];
    }
    else if(std::is_same<T, int>::value) {
        return &UA_TYPES[UA_TYPES_INT32];
    } 
    else if(std::is_same<T, uint>::value)
    {
        return &UA_TYPES[UA_TYPES_UINT32];
    }
    else if(std::is_same<T, float>::value) 
    {
        return &UA_TYPES[UA_TYPES_FLOAT];
    } 
    else if(std::is_same<T, double>::value) 
    {
        return &UA_TYPES[UA_TYPES_DOUBLE];
    } 
    else if(std::is_same<T, std::string>::value) {
        return &UA_TYPES[UA_TYPES_STRING];
    }
}


template <typename T>
void
toUAVariant(T val, UA_Variant* var) {
    UA_Variant_init(var);
    UA_Variant_setScalarCopy(var, &val, getDataType<T>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

// specialize for std::array
template <typename T, size_t N>
void 
toUAVariant(std::array<T, N> &arr, UA_Variant* var) {
    static_assert(isSupportedCppType<T>(), "This type is currently not supported!");
    UA_Variant_init(var);
    UA_Variant_setArrayCopy(var, arr.data(), N, getDataType<T>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

// specialize for std::vector
template <typename T>
void
toUAVariant(std::vector<T> &v, UA_Variant* var) {
    static_assert(isSupportedCppType<T>(), "This type is currently not supported!");
    UA_Variant_init(var);
    UA_Variant_setArrayCopy(var, v.data(), v.size(), getDataType<T>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

template <typename T>
UA_NodeId
uaTypeNodeIdFromCpp() {

    if(std::is_same<T, bool>::value) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_BOOLEAN);
    } else if(std::is_same<T, char>::value) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_SBYTE);
    } else if(std::is_same<T, unsigned char>::value) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_BYTE);
    } else if(std::is_same<T, int16_t>::value) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_INT16);
    } else if(std::is_same<T, uint16_t>::value) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_UINT16);
    } else if(std::is_same<T, int>::value) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_INT32);
    } else if(std::is_same<T, int32_t>::value) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_INT32);
    } else if(std::is_same<T, uint32_t>::value) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_UINT32);
    } else if(std::is_same<T, int64_t>::value) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_INT64);
    } else if(std::is_same<T, uint64_t>::value) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_UINT64);
    } else if(std::is_same<T, float>::value) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_FLOAT);
    } else if(std::is_same<T, double>::value) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_DOUBLE);
    } else if(std::is_same<T, std::string>::value ||
              std::is_same<T, const char *>::value) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_STRING);
    }
    std::cout << typeid(T).name() << std::endl;
    // bool x = T::nothing;
    // static_assert(false, "uaTypeNodeIdFromCpp");
    return UA_NodeId();
}

template <typename T>
UA_VariableAttributes
getVariableAttributes(T val) {
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.dataType = uaTypeNodeIdFromCpp<T>();
    attr.valueRank = -1;
    toUAVariant(val, &attr.value);
    return attr;
}

template <typename T, size_t N>
UA_VariableAttributes
getVariableAttributes(std::array<T, N> &arr) {
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    toUAVariant(arr, &attr.value);
    attr.dataType = uaTypeNodeIdFromCpp<T>();
    attr.valueRank = 1;
    attr.arrayDimensionsSize = 1;
    attr.arrayDimensions = new UA_UInt32{arr.size()};
    return attr;
}

template <typename T>
UA_VariableAttributes
getVariableAttributes(std::vector<T> &v) {
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    toUAVariant(v, &attr.value);
    attr.dataType = uaTypeNodeIdFromCpp<T>();
    attr.valueRank = 1;
    attr.arrayDimensionsSize = 1;
    attr.arrayDimensions = new UA_UInt32{static_cast<UA_UInt32>(v.size())};
    return attr;
}

template <typename T>
typename std::enable_if_t<std::is_arithmetic<T>::value, T>
toStdType(UA_Variant* variant){
    static_assert(std::is_arithmetic<T>::value, "must be a arithmetic type");
    assert(TypeConverter::isTypeMatching<T>(variant->type));
    return *static_cast<T *>(variant->data);
}

// specialize for vector
template <typename T>
typename std::enable_if_t<std::is_same<T, std::vector<typename T::value_type>>::value, T>
toStdType(UA_Variant* variant){
    static_assert(std::is_arithmetic<typename T::value_type>::value,
                  "must be a arithmetic type");
    assert(TypeConverter::isTypeMatching<typename T::value_type>(variant->type));
    return std::vector<typename T::value_type>{
        static_cast<typename T::value_type *>(variant->data),
        static_cast<typename T::value_type *>(variant->data) + variant->arrayLength};
}

// specialize for string
template <typename T>
typename std::enable_if_t<
    std::is_same<typename std::remove_reference<T>::type, std::string>::value, T>
toStdType(UA_Variant* variant){
    UA_String *s = (UA_String *)variant->data;
    std::string stdString = (s->length, reinterpret_cast<char *>(s->data));
    return stdString;
}

}  // namespace TypeConverter