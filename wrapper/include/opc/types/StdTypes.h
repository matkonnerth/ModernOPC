#pragma once
#include <vector>
#include <type_traits>
#include <string>
//#include <opc/Conversion.h>
#include <open62541/types.h>
#include <open62541/server.h>
#include <opc/DataType.h>

namespace opc
{
namespace types
{
inline std::string fromUAString(const UA_String *s)
{
    if (s->length > 0)
    {
        return std::string{reinterpret_cast<char *>(s->data), s->length};
    }
    return std::string{};
}
} // namespace types
} // namespace opc

namespace std
{

template <typename T>
void convertToUAVariantImpl(std::vector<T> val, UA_Variant *var)
{
    static_assert(std::is_arithmetic_v<T>, "Type not integral");
    UA_Variant_init(var);
    UA_Variant_setArrayCopy(var, val.data(), val.size(), opc::getDataType<T>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

void convertToUAVariantImpl(std::vector<std::string> v, UA_Variant *var);
void convertToUAVariantImpl(std::string &v, UA_Variant *var);
void convertToUAVariantImpl(std::string &&v, UA_Variant *var);
void convertToUAVariantImpl(const std::string &v, UA_Variant *var);
} // namespace std



