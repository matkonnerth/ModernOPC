#include <opc/types/StdTypes.h>
#include <opc/DataType.h>
#include <vector>
#include <opc/Variant.h>

namespace std
{
void convertToUAVariantImpl(std::vector<string> v, UA_Variant *var)
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
                            opc::getDataType<string>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
    UA_free(strings);
}

void convertToUAVariantImpl(string &v, UA_Variant *var)
{
    UA_Variant_init(var);
    UA_String s;
    s.length = v.length();
    s.data = (UA_Byte *)(v.data());
    UA_Variant_setScalarCopy(var, &s, opc::getDataType<string>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

void convertToUAVariantImpl(string &&v, UA_Variant *var)
{
    UA_Variant_init(var);
    UA_String s;
    s.length = v.length();
    s.data = (UA_Byte *)(v.data());
    UA_Variant_setScalarCopy(var, &s, opc::getDataType<string>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

void convertToUAVariantImpl(const string &v, UA_Variant *var)
{
    UA_Variant_init(var);
    UA_String s;
    s.length = v.length();
    s.data = (UA_Byte *)(v.data());
    UA_Variant_setScalarCopy(var, &s, opc::getDataType<string>());
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
}