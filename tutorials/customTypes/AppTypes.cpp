#include <AppTypes.h>
#include <opc/Conversion.h>
#include <opc/Variant.h>

// more or less an extension to the wrapper
namespace opc
{

template <>
UA_NodeId getUADataTypeId<app::types::Range2>()
{
    return UA_NODEID_NUMERIC(0, UA_NS0ID_RANGE);
}

template <>
const UA_DataType *getDataType<app::types::Range2>()
{
    return &UA_TYPES[UA_TYPES_RANGE];
}

template <>
app::types::Range2 toStdType(UA_Variant *variant)
{
    return *static_cast<app::types::Range2 *>(variant->data);
}

template <>
app::types::Range2 Variant::get<app::types::Range2>() const
{
    return toStdType<app::types::Range2>(variant);
}
} // namespace opc
