#pragma once
#include <vector>
#include <open62541/server.h>
#include <opc/DataType.h>

namespace opc {

template <typename T>
T toStdType(UA_Variant *variant);

template<typename T>
void convertToUAVariantImpl(T val, UA_Variant* var);

template <typename T>
typename std::enable_if<!std::is_arithmetic_v<T>, void>::type
convertToUAVariant(T &&t, UA_Variant *var)
{
    convertToUAVariantImpl(std::forward<T>(t), var);
}

template <typename T>
typename std::enable_if<std::is_arithmetic_v<T>, void>::type
convertToUAVariant(T &&val, UA_Variant *var)
{
    static_assert(std::is_arithmetic_v<std::remove_reference_t<T>>,
                  "Type not integral");
    UA_Variant_init(var);
    UA_Variant_setScalarCopy(var, &val,
                             getDataType<std::remove_reference_t<T>>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

class Variant {
  public:
    Variant();
    Variant(UA_Variant *var, bool owner = false);
    template<typename T>
    Variant(T&& val)
    {
        variant = UA_Variant_new();
        owned = true;
        convertToUAVariant(std::forward<T>(val), variant);
    }
    ~Variant();

    Variant(const Variant &other) = delete;
    Variant &
    operator=(const Variant &other) = delete;
    Variant(Variant &&other) noexcept;
    Variant &
    operator=(Variant &&other) noexcept;

    template <typename T>
    void
    operator()(T&& val){convertToUAVariant(std::forward<T>(val), variant);};

    template <typename T>
    T
    get() const;

    void set(UA_Variant* var);

    void copyToUaVariant(UA_Variant* var);

    const UA_Variant* getUAVariant() const;

    bool isScalar();
    template <typename T>
    bool is_a();

  private:
    UA_Variant *variant{nullptr};
    bool owned = false;
};
}  // namespace opc