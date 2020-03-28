#include <Variant.h>
#include <TypeConverter.h>

namespace opc
{
template <>
void Variant::operator()(double val)
{
    toUAVariant(val, variant);
}

template <>
void Variant::operator()(int val)
{
    toUAVariant(val, variant);
}

template <>
void Variant::operator()(std::vector<int> val)
{
    toUAVariant(val, variant);
}

template <>
void Variant::operator()(uintptr_t val)
{
    toUAVariant(val, variant);
}

template <>
void Variant::operator()(std::string val)
{
    toUAVariant(std::move(val), variant);
}

template <>
void Variant::operator()(std::vector<std::string> val)
{
    toUAVariant(std::move(val), variant);
}
}