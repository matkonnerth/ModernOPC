#include <AppTypes.h>
#include <opc/Variant.h>

// more or less an extension to the wrapper
namespace opc
{

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

namespace app
{
    namespace types
    {
    void convertToUAVariantImpl(app::types::Range2 m, UA_Variant *var)
    {
        UA_Range range;
        range.high = m.max;
        range.low = m.min;
        UA_Variant_setScalarCopy(var, &range, opc::getDataType<decltype(m)>());
    }
    }
}