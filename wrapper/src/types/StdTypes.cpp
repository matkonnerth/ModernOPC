#include <opc/types/StdTypes.h>
#include <vector>
#include <opc/Variant.h>

namespace opc
{
void toUAVariantImpl(std::vector<std::string> v, UA_Variant *var)
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

void toUAVariantImpl(std::string &v, UA_Variant *var)
{
    UA_Variant_init(var);
    UA_String s;
    s.length = v.length();
    s.data = (UA_Byte *)(v.data());
    UA_Variant_setScalarCopy(var, &s, opc::getDataType<std::string>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}


void toUAVariantImpl(std::string &&v, UA_Variant *var)
{
    UA_Variant_init(var);
    UA_String s;
    s.length = v.length();
    s.data = (UA_Byte *)(v.data());
    UA_Variant_setScalarCopy(var, &s, opc::getDataType<std::string>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

void toUAVariantImpl(const std::string &v, UA_Variant *var)
{
    UA_Variant_init(var);
    UA_String s;
    s.length = v.length();
    s.data = (UA_Byte *)(v.data());
    UA_Variant_setScalarCopy(var, &s, opc::getDataType<std::string>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

}