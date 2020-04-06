#include <opc/Variant.h>

using opc::Variant;

int main()
{
    Variant v {3};

    Variant v2 {
        std::vector<int> {}
    };
}