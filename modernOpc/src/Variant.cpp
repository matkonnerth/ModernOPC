#include <modernOpc/Variant.h>

namespace modernopc
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

Variant::Variant(const Variant &other)
{
    variant = UA_Variant_new();
    owned = true;
    UA_Variant_copy(other.variant, variant);
}

Variant &Variant::operator=(const Variant &other)
{
    if (owned)
    {
        UA_Variant_clear(variant);
        UA_Variant_init(variant);
    }
    owned = true;
    UA_Variant_copy(other.variant, variant);
    return *this;
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

bool Variant::isScalar() const { return UA_Variant_isScalar(variant); }
bool Variant::isEmpty() const { return UA_Variant_isEmpty(variant); }

using CmpFn = bool (*)(const void *a, const void *b);

template <typename T>
bool UAPrimitive_equal(const void *a, const void *b)
{
    return (*reinterpret_cast<const T *>(a) == *reinterpret_cast<const T *>(b));
}

template <>
bool UAPrimitive_equal<UA_String>(const void *a, const void *b)
{
    return UA_String_equal(reinterpret_cast<const UA_String *>(a),
                           reinterpret_cast<const UA_String *>(b));
}

template <>
bool UAPrimitive_equal<UA_Guid>(const void *a, const void *b)
{
    return UA_Guid_equal(reinterpret_cast<const UA_Guid *>(a),
                         reinterpret_cast<const UA_Guid *>(b));
}

bool UAPrimitive_equal_ByteString(const void *a, const void *b)
{
    return UAPrimitive_equal<UA_String>(a, b);
}
bool UAPrimitive_equal_XmlElement(const void *a, const void *b)
{
    return UAPrimitive_equal<UA_String>(a, b);
}

template <>
bool UAPrimitive_equal<UA_NodeId>(const void *a, const void *b)
{
    return UA_NodeId_equal(reinterpret_cast<const UA_NodeId *>(a),
                           reinterpret_cast<const UA_NodeId *>(b));
}

template <>
bool UAPrimitive_equal<UA_ExpandedNodeId>(const void *a, const void *b)
{
    return UA_ExpandedNodeId_equal(
        reinterpret_cast<const UA_ExpandedNodeId *>(a),
        reinterpret_cast<const UA_ExpandedNodeId *>(b));
}

template <>
bool UAPrimitive_equal<UA_StatusCode>(const void *a, const void *b)
{
    return UAPrimitive_equal<UA_UInt32>(a, b);
}

template <>
bool UAPrimitive_equal<UA_QualifiedName>(const void *a, const void *b)
{
    return UA_QualifiedName_equal(
        reinterpret_cast<const UA_QualifiedName *>(a),
        reinterpret_cast<const UA_QualifiedName *>(b));
}

template <>
bool UAPrimitive_equal<UA_LocalizedText>(const void *a, const void *b)
{
    auto lta = reinterpret_cast<const UA_LocalizedText *>(a);
    auto ltb = reinterpret_cast<const UA_LocalizedText *>(b);
    return (UAPrimitive_equal<UA_String>(&lta->locale, &ltb->locale) &&
            UAPrimitive_equal<UA_String>(&lta->text, &ltb->text));
}

template <>
bool UAPrimitive_equal<UA_ExtensionObject>(const void *a, const void *b)
{
    assert(false && "not implemented");
}

template <>
bool UAPrimitive_equal<UA_DataValue>(const void *a, const void *b)
{
    assert(false && "not implemented");
}

template <>
bool UAPrimitive_equal<UA_Variant>(const void *a, const void *b)
{
    assert(false && "not implemented");
}

template <>
bool UAPrimitive_equal<UA_DiagnosticInfo>(const void *a, const void *b)
{
    assert(false && "not implemented");
}

bool UAPrimitive_equal_NOT_IMPLEMENTED(const void *a, const void *b)
{
    assert(false && "not implemented");
}

#define CMP_TABLE_SIZE 31
static const CmpFn CMP_TABLE[CMP_TABLE_SIZE] = {
    UAPrimitive_equal<UA_Boolean>, UAPrimitive_equal<UA_SByte>,
    UAPrimitive_equal<UA_Byte>, UAPrimitive_equal<UA_Int16>,
    UAPrimitive_equal<UA_UInt16>, UAPrimitive_equal<UA_Int32>,
    UAPrimitive_equal<UA_UInt32>, UAPrimitive_equal<UA_Int64>,
    UAPrimitive_equal<UA_UInt64>, UAPrimitive_equal<UA_Float>,
    UAPrimitive_equal<UA_Double>, UAPrimitive_equal<UA_String>,
    UAPrimitive_equal<UA_DateTime>, UAPrimitive_equal<UA_Guid>,
    UAPrimitive_equal_ByteString, UAPrimitive_equal_XmlElement,
    UAPrimitive_equal<UA_NodeId>, UAPrimitive_equal<UA_ExpandedNodeId>,
    UAPrimitive_equal<UA_StatusCode>, UAPrimitive_equal<UA_QualifiedName>,
    UAPrimitive_equal<UA_LocalizedText>,
    // not implemented
    UAPrimitive_equal<UA_ExtensionObject>, UAPrimitive_equal<UA_DataValue>,
    UAPrimitive_equal<UA_Variant>, UAPrimitive_equal<UA_DiagnosticInfo>,
    // decimal
    UAPrimitive_equal_NOT_IMPLEMENTED,
    // enum
    UAPrimitive_equal_NOT_IMPLEMENTED,
    // structure
    UAPrimitive_equal_NOT_IMPLEMENTED,
    // optstruct
    UAPrimitive_equal_NOT_IMPLEMENTED,
    // union
    UAPrimitive_equal_NOT_IMPLEMENTED,
    // bitfieldcluster
    UAPrimitive_equal_NOT_IMPLEMENTED};

bool Variant::operator==(const Variant &other) const
{
    if (isEmpty() != other.isEmpty())
    {
        return false;
    }
    if (isEmpty())
    {
        return true;
    }
    if (isScalar() != other.isScalar())
    {
        return false;
    }
    if (variant->type != other.getUAVariant()->type)
    {
        return false;
    }
    if (variant->arrayLength != other.getUAVariant()->arrayLength)
    {
        return false;
    }

    return CMP_TABLE[variant->type->typeKind](variant->data,
                                              other.getUAVariant()->data);
}

void toUAVariantImpl(const Variant &v, UA_Variant *var)
{
    UA_Variant_init(var);
    UA_Variant_copy(v.getUAVariant(), var);
}

} // namespace modernopc