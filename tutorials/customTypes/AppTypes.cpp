#include <AppTypes.h>
#include <modernOpc/Variant.h>

namespace opc
{

template <>
app::types::Range2 Variant::get<app::types::Range2>() const
{
    return *static_cast<app::types::Range2 *>(variant->data);
}
} // namespace opc

namespace app
{
namespace types
{

void toUAVariantImpl(const Range2 &m, UA_Variant *var)
{
    UA_Range range;
    range.high = m.max;
    range.low = m.min;
    UA_Variant_setScalarCopy(
        var, &range,
        opc::getDataType<
            std::remove_const_t<std::remove_reference_t<decltype(m)>>>());
}

} // namespace types
} // namespace app