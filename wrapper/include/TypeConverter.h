#pragma once
#include <open62541/server.h>
#include <array>
#include <cassert>
#include <iostream>
#include <memory>
#include <typeinfo>
#include <vector>
#include <type_traits>

namespace TypeConverter {

template <typename T>
constexpr bool
isSupportedCppType() {
    if(std::is_arithmetic_v<T>) {
        return true;
    }
    return false;
}

template <typename T>
bool
isTypeMatching(const UA_DataType *uatype) {
    using type = typename std::remove_reference_t<T>;
    if(uatype->memSize == sizeof(type)) {
        return true;
    }
    return false;
}

template <typename T>
const UA_DataType *
getDataType() {
    if(std::is_same_v<T, bool>) {
        return &UA_TYPES[UA_TYPES_BOOLEAN];
    } else if(std::is_integral_v<T>) {
        if(std::is_signed_v<T>) {
            if(sizeof(T) == 1) {
                return &UA_TYPES[UA_TYPES_SBYTE];
            } else if(sizeof(T) == 2) {
                return &UA_TYPES[UA_TYPES_INT16];
            } else if(sizeof(T) == 4) {
                return &UA_TYPES[UA_TYPES_INT32];
            } else if(sizeof(T) == 8) {
                return &UA_TYPES[UA_TYPES_INT64];
            }
        } else {
            if(sizeof(T) == 1) {
                return &UA_TYPES[UA_TYPES_BYTE];
            } else if(sizeof(T) == 2) {
                return &UA_TYPES[UA_TYPES_UINT16];
            } else if(sizeof(T) == 4) {
                return &UA_TYPES[UA_TYPES_UINT32];
            } else if(sizeof(T) == 8) {
                return &UA_TYPES[UA_TYPES_UINT64];
            }
        }

    } else if(std::is_floating_point_v<T>) {
        if(sizeof(T) == 4) {
            return &UA_TYPES[UA_TYPES_FLOAT];
        } else if(sizeof(T) == 8) {
            return &UA_TYPES[UA_TYPES_DOUBLE];
        }

    } else if(std::is_same_v<T, std::string>) {
        return &UA_TYPES[UA_TYPES_STRING];
    }
}

template <typename T>
void
toUAVariant(T val, UA_Variant *var) {
    UA_Variant_init(var);
    UA_Variant_setScalarCopy(var, &val, getDataType<T>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

// specialize for std::array
template <typename T, size_t N>
void
toUAVariant(std::array<T, N> &arr, UA_Variant *var) {
    static_assert(isSupportedCppType<T>(), "This type is currently not supported!");
    UA_Variant_init(var);
    UA_Variant_setArrayCopy(var, arr.data(), N, getDataType<T>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

// specialize for std::vector
template <typename T>
void
toUAVariant(std::vector<T> &v, UA_Variant *var) {
    static_assert(isSupportedCppType<T>(), "This type is currently not supported!");
    UA_Variant_init(var);
    UA_Variant_setArrayCopy(var, v.data(), v.size(), getDataType<T>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

template <typename T>
UA_NodeId
uaTypeNodeIdFromCpp() {

    if(std::is_same_v<T, bool>) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_BOOLEAN);
    } else if(std::is_same_v<T, char>) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_SBYTE);
    } else if(std::is_same_v<T, unsigned char>) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_BYTE);
    } else if(std::is_same_v<T, int16_t>) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_INT16);
    } else if(std::is_same_v<T, uint16_t>) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_UINT16);
    } else if(std::is_same_v<T, int>) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_INT32);
    } else if(std::is_same_v<T, int32_t>) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_INT32);
    } else if(std::is_same_v<T, uint32_t>) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_UINT32);
    } else if(std::is_same_v<T, int64_t>) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_INT64);
    } else if(std::is_same_v<T, uint64_t>) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_UINT64);
    } else if(std::is_same_v<T, float>) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_FLOAT);
    } else if(std::is_same_v<T, double>) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_DOUBLE);
    } else if(std::is_same_v<T, std::string> || std::is_same_v<T, const char *>) {
        return UA_NODEID_NUMERIC(0, UA_NS0ID_STRING);
    }
    std::cout << typeid(T).name() << std::endl;
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
    attr.arrayDimensions = new UA_UInt32{static_cast<UA_UInt32>(arr.size())};
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
T
toStdType(UA_Variant *variant) {
    if constexpr(std::is_arithmetic_v<T>) {
        assert(TypeConverter::isTypeMatching<T>(variant->type));
        return *static_cast<T *>(variant->data);
    } else if constexpr(!std::is_same_v<T, std::vector<std::string>> &&
                        std::is_same_v<T, std::vector<typename T::value_type>>) {
        assert(TypeConverter::isTypeMatching<typename T::value_type>(variant->type));
        return std::vector<typename T::value_type>{
            static_cast<typename T::value_type *>(variant->data),
            static_cast<typename T::value_type *>(variant->data) + variant->arrayLength};
    } else if constexpr(std::is_same_v<typename std::remove_reference_t<T>,
                                       std::string>) {
        assert(variant->type->typeIndex == UA_TYPES_STRING);
        UA_String *s = (UA_String *)variant->data;
        if(s->length > 0) {
            std::string stdString{reinterpret_cast<char *>(s->data), s->length};
            return stdString;
        }
        return std::string{};
    } else if constexpr(std::is_same_v<T, std::vector<std::string>>) {
        assert(variant->type->typeIndex == UA_TYPES_STRING);

        std::vector<std::string> vec;
        for(size_t i = 0; i < variant->arrayLength; i++) {
            UA_String *s = ((UA_String *)variant->data) + i;
            if(s->length > 0) {
                std::string stdString{reinterpret_cast<char *>(s->data), s->length};
                vec.push_back(stdString);
            } else {
                vec.push_back(std::string{});
            }
        }
        return vec;
    }
}
}  // namespace TypeConverter