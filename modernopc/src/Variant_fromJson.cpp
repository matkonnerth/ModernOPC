#include <modernopc/Variant.h>
#include <modernopc/types/String.h>

#ifdef __cplusplus
extern "C" {
#endif
UA_StatusCode
UA_decodeJson(const UA_ByteString *src, void *dst,
              const UA_DataType *type);

#ifdef __cplusplus
}
#endif

namespace modernopc
{
Variant fromJson(const std::string &jsonString, const UA_DataType *dataType)
{
    //TODO: check if it's an array
    void *data = calloc(1, dataType->memSize);

    const UA_ByteString buf = {jsonString.length(), (UA_Byte*)jsonString.c_str()};

    auto status = UA_decodeJson(&buf, data, dataType);

    if (!status == UA_STATUSCODE_GOOD)
    {
        free(data);
        Variant();
    }

    auto var = UA_Variant_new();
    UA_Variant_init(var);
    UA_Variant_setScalar(var, data, dataType);

    return Variant(var, true);
}

} // namespace modernopc