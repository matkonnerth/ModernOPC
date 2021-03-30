#include <algorithm>
#include <modernOpc/Variant.h>
#include <modernOpc/types/LocalizedText.h>
#include <modernOpc/types/NodeId.h>
#include <modernOpc/types/QualifiedName.h>
#include <modernOpc/types/String.h>
#include <string>
#include <vector>

namespace opc
{

template <>
std::vector<uint64_t> Variant::get<std::vector<uint64_t>>() const
{
    return std::vector<uint64_t>{static_cast<uint64_t *>(variant->data),
                                 static_cast<uint64_t *>(variant->data) +
                                     variant->arrayLength};
}

template <>
bool Variant::get<bool>() const
{
    return *static_cast<bool *>(variant->data);
}

template <>
uint8_t Variant::get<uint8_t>() const
{
    return *static_cast<uint8_t *>(variant->data);
}

template <>
int8_t Variant::get<int8_t>() const
{
    return *static_cast<int8_t *>(variant->data);
}

template <>
int16_t Variant::get<int16_t>() const
{
    return *static_cast<int16_t *>(variant->data);
}

template <>
uint16_t Variant::get<uint16_t>() const
{
    return *static_cast<uint16_t *>(variant->data);
}

template <>
int32_t Variant::get<int32_t>() const
{
    return *static_cast<int32_t *>(variant->data);
}

template <>
uint32_t Variant::get<uint32_t>() const
{
    return *static_cast<uint32_t *>(variant->data);
}

template <>
int64_t Variant::get<int64_t>() const
{
    return *static_cast<int64_t *>(variant->data);
}

template <>
uint64_t Variant::get<uint64_t>() const
{
    return *static_cast<uint64_t *>(variant->data);
}

template <>
float Variant::get<float>() const
{
    return *static_cast<float *>(variant->data);
}

template <>
double Variant::get<double>() const
{
    return *static_cast<double *>(variant->data);
}

template <>
std::string Variant::get<std::string>() const
{
    return fromUAString(*static_cast<UA_String *>(variant->data));
}

template <>
std::vector<int> Variant::get<std::vector<int>>() const
{
    return std::vector<int>{static_cast<int *>(variant->data),
                            static_cast<int *>(variant->data) +
                                variant->arrayLength};
}

template <>
std::vector<uint> Variant::get<std::vector<uint>>() const
{
    return std::vector<uint>{static_cast<uint *>(variant->data),
                             static_cast<uint *>(variant->data) +
                                 variant->arrayLength};
}

template <>
std::vector<std::string> Variant::get<std::vector<std::string>>() const
{
    std::vector<std::string> vec;
    std::transform(
        static_cast<UA_String *>(variant->data),
        static_cast<UA_String *>(variant->data) + variant->arrayLength,
        std::back_inserter(vec), [](const UA_String &s) { return fromUAString(s); });

    return vec;
}

} // namespace opc