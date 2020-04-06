#include <opc/Variant.h>
#include <opc/types/Types.h>
#include <string>
#include <vector>
#include <opc/types/NodeId.h>

namespace opc
{
template <>
bool Variant::get<bool>() const
{
    return toStdType<bool>(variant);
}

template <>
uint8_t Variant::get<uint8_t>() const
{
    return toStdType<uint8_t>(variant);
}

template <>
int8_t Variant::get<int8_t>() const
{
    return toStdType<int8_t>(variant);
}

template <>
int16_t Variant::get<int16_t>() const
{
    return toStdType<int16_t>(variant);
}

template <>
uint16_t Variant::get<uint16_t>() const
{
    return toStdType<uint16_t>(variant);
}

template <>
int32_t Variant::get<int32_t>() const
{
    return toStdType<int32_t>(variant);
}

template <>
uint32_t Variant::get<uint32_t>() const
{
    return toStdType<uint32_t>(variant);
}

template <>
int64_t Variant::get<int64_t>() const
{
    return toStdType<int64_t>(variant);
}

template <>
uint64_t Variant::get<uint64_t>() const
{
    return toStdType<uint64_t>(variant);
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

template <>
types::NodeId Variant::get<types::NodeId>() const
{
    return toStdType<types::NodeId>(variant);
}
} // namespace opc