#pragma once
#include <open62541/server.h>
#include <array>
#include <iostream>
#include <memory>
#include <typeinfo>
#include <vector>


namespace TypeConverter {

template <typename T>
const UA_DataType *
getDataType() {
    if(std::is_same<T, int>::value) {
        return &UA_TYPES[UA_TYPES_INT32];
    } else if(std::is_same<T, float>::value) {
        return &UA_TYPES[UA_TYPES_FLOAT];
    }
}

template <typename T>
void
toVariant(T val, UA_Variant* var) {
    UA_Variant_init(var);
    UA_Variant_setScalarCopy(var, &val, getDataType<T>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

template <typename T>
void
toVariant(T *val, UA_Variant *var, bool zeroCopy = false) {
    UA_Variant_init(var);
    if(zeroCopy) {
        UA_Variant_setScalar(var, val, getDataType<T>());
        var->storageType = UA_VariantStorageType::UA_VARIANT_DATA_NODELETE;
    } else {
        UA_Variant_setScalarCopy(var, val, getDataType<T>());
        var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
    }
}

// specialize for std::array
template <typename T, size_t N>
void 
toVariant(std::array<T, N> &arr, UA_Variant* var) {
    UA_Variant_init(var);
    UA_Variant_setArrayCopy(var, arr.data(), N, getDataType<T>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

// specialize for std::vector
template <typename T>
void
toVariant(std::vector<T> &v, UA_Variant* var) {
    UA_Variant_init(var);
    UA_Variant_setArrayCopy(var, v.data(), v.size(), getDataType<T>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

// here it's clear: we have ownership, so there no need for copy, stack is responsible for
// cleaning up
template <typename T>
void
toVariant(std::unique_ptr<T> val, UA_Variant* var) {
    UA_Variant_init(var);
    UA_Variant_setScalar(var, val.release(), getDataType<T>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

template <typename T>
UA_NodeId
uaTypeNodeIdFromCpp() {

    if(std::is_same<T, bool>::value) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_BOOLEAN);
    } else if(std::is_same<T, char>::value) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_SBYTE);
    } else if(std::is_same<T, u_char>::value) {
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
    toVariant(val, &attr.value);
    return attr;
}

template <typename T, size_t N>
UA_VariableAttributes
getVariableAttributes(std::array<T, N> &arr) {
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    toVariant(arr, &attr.value);
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
    toVariant(v, &attr.value);
    attr.dataType = uaTypeNodeIdFromCpp<T>();
    attr.valueRank = 1;
    attr.arrayDimensionsSize = 1;
    attr.arrayDimensions = new UA_UInt32{static_cast<UA_UInt32>(v.size())};
    return attr;
}
}  // namespace TypeConverter