#pragma once
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

              friend void convertToUAVariantImpl(const app::types::Range2& m,
                                          UA_Variant *var);
        };


        
    }
}