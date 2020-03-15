#pragma once


namespace app
{
    namespace types
    {
        class Range2
        {
            public:
              Range2(double min, double max) : min{min}, max{max} {}
              double min;
              double max;
            private:
              int test{0};
        };
    }
}