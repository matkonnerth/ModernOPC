#include <opc/Variant.h>

namespace opc
{
    template <>
    bool Variant::is_a<bool>()
    {
        return variant->type == getDataType<bool>();
    }

    template <>
    bool Variant::is_a<double>()
    {
        return variant->type == getDataType<double>();
    }

    template <>
    bool Variant::is_a<int32_t>()
    {
        return variant->type == getDataType<int32_t>();
    }

    template<>
    bool Variant::is_a<std::vector<int32_t>>()
    {
        return !isScalar() && is_a<int32_t>();
    }
}
