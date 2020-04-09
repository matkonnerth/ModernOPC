#include <AppTypes.h>
#include <opc/Variant.h>

namespace opc
{

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

void convertToUAVariantImpl(const Range2 &m, UA_Variant *var) {}

} // namespace types
} // namespace app