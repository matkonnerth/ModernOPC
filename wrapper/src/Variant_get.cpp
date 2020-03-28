#include <TypeConverter.h>
#include <Variant.h>

namespace opc
{
template <>
bool Variant::get<bool>() const
{
    return toStdType<bool>(variant);
}

template <>
int Variant::get<int>() const
{
    return toStdType<int>(variant);
}

template <>
double Variant::get<double>() const
{
    return toStdType<double>(variant);
}

template <>
std::string Variant::get<std::string>() const
{
    return toStdType<std::string>(variant);
}

template <>
std::vector<int> Variant::get<std::vector<int>>() const
{
    return toStdType<std::vector<int>>(variant);
}

template <>
std::vector<uint> Variant::get<std::vector<uint>>() const
{
    return toStdType<std::vector<uint>>(variant);
}

template <>
std::vector<std::string> Variant::get<std::vector<std::string>>() const
{
    return toStdType<std::vector<std::string>>(variant);
}

template <>
types::LocalizedText Variant::get<types::LocalizedText>() const
{
    return toStdType<types::LocalizedText>(variant);
}

template <>
std::vector<types::LocalizedText>
Variant::get<std::vector<types::LocalizedText>>() const
{
    return toStdType<std::vector<types::LocalizedText>>(variant);
}

template <>
types::QualifiedName Variant::get<types::QualifiedName>() const
{
    return toStdType<types::QualifiedName>(variant);
}
} // namespace opc