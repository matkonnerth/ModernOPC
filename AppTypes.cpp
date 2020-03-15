#include <AppTypes.h>
#include <TypeConverter.h>
#include <Variant.h>

// more or less an extension to the wrapper
namespace TypeConverter
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
    assert(TypeConverter::isTypeMatching<app::types::Range2>(variant->type));
    return *static_cast<app::types::Range2 *>(variant->data);
}
} // namespace TypeConverter

namespace opc
{
template <>
app::types::Range2 Variant::get<app::types::Range2>() const
{
    return TypeConverter::toStdType<app::types::Range2>(variant);
}
} // namespace opc
