#include <opc/types/StdTypes.h>
#include <opc/DataType.h>
#include <vector>
#include <opc/Variant.h>

namespace opc
{
void convertToUAVariantImpl(std::vector<std::string> v, UA_Variant *var)
{
    UA_Variant_init(var);
    auto strings =
        static_cast<UA_String *>(UA_calloc(v.size(), sizeof(UA_String)));
    size_t i = 0;
    for (auto &s : v)
    {
        strings[i].length = s.length();
        strings[i].data = reinterpret_cast<UA_Byte *>(s.data());
        i++;
    }
    UA_Variant_setArrayCopy(var, strings, v.size(),
                            opc::getDataType<std::string>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
    UA_free(strings);
}

void convertToUAVariantImpl(std::string &v, UA_Variant *var)
{
    UA_Variant_init(var);
    UA_String s;
    s.length = v.length();
    s.data = (UA_Byte *)(v.data());
    UA_Variant_setScalarCopy(var, &s, opc::getDataType<std::string>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

void convertToUAVariantImpl(std::string &&v, UA_Variant *var)
{
    UA_Variant_init(var);
    UA_String s;
    s.length = v.length();
    s.data = (UA_Byte *)(v.data());
    UA_Variant_setScalarCopy(var, &s, opc::getDataType<std::string>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

void convertToUAVariantImpl(const std::string &v, UA_Variant *var)
{
    UA_Variant_init(var);
    UA_String s;
    s.length = v.length();
    s.data = (UA_Byte *)(v.data());
    UA_Variant_setScalarCopy(var, &s, opc::getDataType<std::string>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}
} // namespace std

namespace opc
{
template <>
std::vector<int> toStdType(UA_Variant *variant)
{
    return std::vector<int>{static_cast<int *>(variant->data),
                            static_cast<int *>(variant->data) +
                                variant->arrayLength};
}

template <>
std::vector<int64_t> toStdType(UA_Variant *variant)
{
    return std::vector<int64_t>{static_cast<int64_t *>(variant->data),
                                static_cast<int64_t *>(variant->data) +
                                    variant->arrayLength};
}

template <>
std::vector<uint64_t> toStdType(UA_Variant *variant)
{
    return std::vector<uint64_t>{static_cast<uint64_t *>(variant->data),
                                 static_cast<uint64_t *>(variant->data) +
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
    return opc::types::fromUAString(static_cast<UA_String *>(variant->data));
}

template <>
std::vector<std::string> toStdType(UA_Variant *variant)
{
    std::vector<std::string> vec;
    for (size_t i = 0; i < variant->arrayLength; i++)
    {
        UA_String *s = ((UA_String *)variant->data) + i;
        vec.emplace_back(opc::types::fromUAString(s));
    }
    return vec;
}

template <>
bool toStdType(UA_Variant *variant)
{
    return *static_cast<bool *>(variant->data);
}

template <>
int8_t toStdType(UA_Variant *variant)
{
    return *static_cast<int8_t *>(variant->data);
}

template <>
uint8_t toStdType(UA_Variant *variant)
{
    return *static_cast<uint8_t *>(variant->data);
}

template <>
int16_t toStdType(UA_Variant *variant)
{
    return *static_cast<int16_t *>(variant->data);
}

template <>
uint16_t toStdType(UA_Variant *variant)
{
    return *static_cast<uint16_t *>(variant->data);
}

template <>
int32_t toStdType(UA_Variant *variant)
{
    return *static_cast<int32_t *>(variant->data);
}

template <>
uint32_t toStdType(UA_Variant *variant)
{
    return *static_cast<uint32_t *>(variant->data);
}

template <>
int64_t toStdType(UA_Variant *variant)
{
    return *static_cast<int64_t *>(variant->data);
}

template <>
uint64_t toStdType(UA_Variant *variant)
{
    return *static_cast<uint64_t *>(variant->data);
}

template <>
float toStdType(UA_Variant *variant)
{
    return *static_cast<float*>(variant->data);
}

template <>
double toStdType(UA_Variant *variant)
{
    return *static_cast<double *>(variant->data);
}

}