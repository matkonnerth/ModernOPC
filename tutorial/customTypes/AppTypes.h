#pragma once


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