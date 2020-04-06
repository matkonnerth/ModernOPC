#include <opc/Conversion.h>
#include <open62541/server.h>

namespace std
{
template <>
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
    UA_Variant_setArrayCopy(var, strings, v.size(), opc::getDataType<std::string>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
    UA_free(strings);
}

template <>
void convertToUAVariantImpl(std::string &v, UA_Variant *var)
{
    UA_Variant_init(var);
    UA_String s;
    s.length = v.length();
    s.data = (UA_Byte *)(v.data());
    UA_Variant_setScalarCopy(var, &s, opc::getDataType<std::string>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

template <>
void convertToUAVariantImpl(std::string &&v, UA_Variant *var)
{
    UA_Variant_init(var);
    UA_String s;
    s.length = v.length();
    s.data = (UA_Byte *)(v.data());
    UA_Variant_setScalarCopy(var, &s, opc::getDataType<std::string>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

template <>
void convertToUAVariantImpl(const std::string &v, UA_Variant *var)
{
    UA_Variant_init(var);
    UA_String s;
    s.length = v.length();
    s.data = (UA_Byte *)(v.data());
    UA_Variant_setScalarCopy(var, &s, opc::getDataType<std::string>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}
}

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

std::string fromUAString(const UA_String *s)
{
    if (s->length > 0)
    {
        return std::string{reinterpret_cast<char *>(s->data), s->length};
    }
    return std::string{};
}

template <>
std::string toStdType(UA_Variant *variant)
{
    return fromUAString(static_cast<UA_String *>(variant->data));
}

template <>
std::vector<std::string> toStdType(UA_Variant *variant)
{
    std::vector<std::string> vec;
    for (size_t i = 0; i < variant->arrayLength; i++)
    {
        UA_String *s = ((UA_String *)variant->data) + i;
        vec.emplace_back(fromUAString(s));
    }
    return vec;
}

template <>
types::LocalizedText toStdType(UA_Variant *variant)
{
    return types::fromUALocalizedText(static_cast<UA_LocalizedText *>(variant->data));
}

template <>
types::QualifiedName toStdType(UA_Variant *variant)
{
    return types::fromUAQualifiedName(static_cast<UA_QualifiedName *>(variant->data));
}

template <>
std::vector<types::LocalizedText> toStdType(UA_Variant *var)
{
    std::vector<types::LocalizedText> vec;
    for (size_t i = 0; i < var->arrayLength; i++)
    {
        vec.emplace_back(types::fromUALocalizedText(
            &static_cast<UA_LocalizedText *>(var->data)[i]));
    }
    return vec;
}





} // namespace opc