#include <TypeConverter.h>
#include <Variant.h>

namespace opc {

/* setters */
template<>
void Variant::operator()(double val)
{
    TypeConverter::toUAVariant(val, variant);
}

template <>
void
Variant::operator()(int val) {
    TypeConverter::toUAVariant(val, variant);
}

template <>
void
Variant::operator()(std::vector<int> val) {
    TypeConverter::toUAVariant(val, variant);
}

template <>
void
Variant::operator()(std::string val) {
    TypeConverter::toUAVariant(val, variant);
}

/*getters*/
template <>
bool Variant::get<bool>() const
{
    return TypeConverter::toStdType<bool>(variant);
}

template <>
int
Variant::get<int>() const {
    return TypeConverter::toStdType<int>(variant);
}

template <>
double
Variant::get<double>() const {
    return TypeConverter::toStdType<double>(variant);
}

template <>
std::string
Variant::get<std::string>() const {
    return TypeConverter::toStdType<std::string>(variant);
}

template <>
std::vector<int>
Variant::get<std::vector<int>>() const {
    return TypeConverter::toStdType<std::vector<int>>(variant);
}

template <>
std::vector<std::string>
Variant::get<std::vector<std::string>>() const {
    return TypeConverter::toStdType<std::vector<std::string>>(variant);
}

}  // namespace opc