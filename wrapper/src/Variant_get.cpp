#include <opc/Variant.h>
#include <opc/types/LocalizedText.h>
#include <opc/types/NodeId.h>
#include <opc/types/QualifiedName.h>
#include <string>
#include <vector>
#include <opc/types/StdTypes.h>

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
    return *static_cast<uint16_t*>(variant->data);
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
    return fromUAString(static_cast<UA_String *>(variant->data));
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
    for (size_t i = 0; i < variant->arrayLength; i++)
    {
        UA_String *s = ((UA_String *)variant->data) + i;
        vec.emplace_back(fromUAString(s));
    }
    return vec;
}

} // namespace opc