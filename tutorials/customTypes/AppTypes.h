#pragma once
#include <opc/Conversion.h>


namespace app
{
    namespace types
    {
        struct Range2
        {
              Range2(double min, double max) : min{min}, max{max} {}
              double min;
              double max;
        };
    }
}
struct UA_Variant;
namespace opc
{
    template <>
    void toUAVariant(app::types::Range2 m, UA_Variant *var);
}