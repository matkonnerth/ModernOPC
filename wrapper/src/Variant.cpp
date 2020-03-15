#include "Variant.h"
#include "TypeConverter.h"



namespace opc {

Variant::Variant() {
    variant = UA_Variant_new();
    owned = true;
}

Variant::Variant(UA_Variant *var, bool owner) {
    variant = var;
    owned = owner;
}

Variant::~Variant() {
    if(owned) {
        UA_Variant_delete(variant);
    }
}

Variant::Variant(Variant &&other) {
    owned = other.owned;
    variant = other.variant;
    other.owned=false;
    other.variant=nullptr;
}

Variant &
Variant::operator=(Variant &&other) {
    if(owned) {
        UA_Variant_delete(variant);
    }
    owned = other.owned;
    variant = other.variant;
    other.owned=false;
    other.variant=nullptr;
    return *this;
}

/* setters */
template<>
void Variant::operator()(double val)
{
    toUAVariant(val, variant);
}

template <>
void
Variant::operator()(int val) {
    toUAVariant(val, variant);
}

template <>
void
Variant::operator()(std::vector<int> val) {
    toUAVariant(val, variant);
}

template <>
void
Variant::operator()(uintptr_t val) {
    toUAVariant(val, variant);
}

template <>
void
Variant::operator()(std::string val) {
    toUAVariant(val, variant);
}

/*getters*/
template <>
bool Variant::get<bool>() const
{
    return toStdType<bool>(variant);
}

template <>
int
Variant::get<int>() const {
    return toStdType<int>(variant);
}

template <>
double
Variant::get<double>() const {
    return toStdType<double>(variant);
}

template <>
std::string
Variant::get<std::string>() const {
    return toStdType<std::string>(variant);
}

template <>
std::vector<int>
Variant::get<std::vector<int>>() const {
    return toStdType<std::vector<int>>(variant);
}

template <>
std::vector<uint> Variant::get<std::vector<uint>>() const
{
    return toStdType<std::vector<uint>>(variant);
}

template <>
std::vector<std::string>
Variant::get<std::vector<std::string>>() const {
    return toStdType<std::vector<std::string>>(variant);
}

}  // namespace opc