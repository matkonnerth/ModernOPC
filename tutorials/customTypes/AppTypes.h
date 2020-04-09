#pragma once
#include <opc/DataType.h>
#include <opc/Variant.h>

namespace app
{
namespace types
{
struct Range2
{
    Range2(double min, double max) : min{min}, max{max} {}
    double min;
    double max;

    friend void convertToUAVariantImpl(const Range2 &m,
                                       UA_Variant *var);
};

} // namespace types
} // namespace app

namespace opc
{
template <>
inline const UA_DataType *getDataType<app::types::Range2>()
{
    return &UA_TYPES[UA_TYPES_RANGE];
}
} // namespace opc