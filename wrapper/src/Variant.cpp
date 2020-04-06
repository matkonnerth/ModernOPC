#include <opc/Variant.h>

namespace opc
{

Variant::Variant()
{
    variant = UA_Variant_new();
    owned = true;
}

Variant::Variant(UA_Variant *var, bool owner)
{
    variant = var;
    owned = owner;
}

Variant::~Variant()
{
    if (owned)
    {
        UA_Variant_delete(variant);
    }
}

Variant::Variant(Variant &&other) noexcept
{
    owned = other.owned;
    variant = other.variant;
    other.owned = false;
    other.variant = nullptr;
}

Variant &Variant::operator=(Variant &&other) noexcept
{
    if (owned)
    {
        UA_Variant_delete(variant);
    }
    owned = other.owned;
    variant = other.variant;
    other.owned = false;
    other.variant = nullptr;
    return *this;
}

void Variant::set(UA_Variant *var)
{
    if (variant && owned)
    {
        UA_Variant_delete(variant);
    }
    variant = var;
    owned = true;
}

void Variant::copyToUaVariant(UA_Variant *var)
{
    UA_Variant_copy(variant, var);
}

const UA_Variant *Variant::getUAVariant() const { return variant; }

bool Variant::isScalar()
{
    return UA_Variant_isScalar(variant);
}

} // namespace opc