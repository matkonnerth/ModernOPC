#include <modernopc/Variant.h>
#include <modernopc/types/String.h>
#include <open62541/types.h>

namespace modernopc
{
Variant fromJson(const std::string &jsonString, const UA_DataType *dataType)
{
    void *data = calloc(1, dataType->memSize);

    const UA_ByteString buf = {jsonString.length(), (UA_Byte*)jsonString.c_str()};

    auto status = UA_decodeJson(&buf, data, dataType, NULL);

    if (!status == UA_STATUSCODE_GOOD)
    {
        free(data);
        return Variant();
    }

    auto var = UA_Variant_new();
    UA_Variant_init(var);
    UA_Variant_setScalar(var, data, dataType);

    return Variant(var, true);
}

} // namespace modernopc