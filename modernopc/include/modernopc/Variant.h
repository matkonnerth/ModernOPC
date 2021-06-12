#pragma once
#include <cstring>
#include <modernopc/DataType.h>
#include <modernopc/util.h>
#include <open62541/types.h>
#include <open62541/types_generated_handling.h>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace modernopc
{

template <typename T>
void toUAVariantImpl(const T &val, UA_Variant *var);

template <typename T>
void toUAVariantImpl(const std::vector<T> &val, UA_Variant *var)
{
    static_assert(std::is_arithmetic_v<removeConstRef_t<T>>,
                  "Type not integral");
    UA_Variant_init(var);
    UA_Variant_setArrayCopy(var, val.data(), val.size(),
                            modernopc::getDataType<removeConstRef_t<T>>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

void toUAVariantImpl(const std::string &v, UA_Variant *var);
void toUAVariantImpl(const std::vector<std::string> &v, UA_Variant *var);

template <typename T>
typename std::enable_if<!std::is_arithmetic_v<removeConstRef_t<T>>, void>::type
toUAVariant(const T &val, UA_Variant *var)
{
    toUAVariantImpl(val, var);
}

template <typename T>
typename std::enable_if<std::is_arithmetic_v<removeConstRef_t<T>>, void>::type
toUAVariant(const T &val, UA_Variant *var)
{
    static_assert(std::is_arithmetic_v<removeConstRef_t<T>>,
                  "Type not integral");
    UA_Variant_init(var);
    UA_Variant_setScalarCopy(var, &val, getDataType<removeConstRef_t<T>>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

class Variant
{
  public:
    Variant();
    Variant(UA_Variant *var, bool owner = false);
    template <typename T>
    Variant(T &&val)
    {
        variant = UA_Variant_new();
        owned = true;
        toUAVariant(std::forward<T>(val), variant);
    }
    template <typename T>
    Variant(T &&val, bool owned)
    {
        variant = UA_Variant_new();
        tUAVariant(std::forward<T>(val), variant);
    }
    ~Variant();
    Variant(const Variant &other);
    Variant &operator=(const Variant &other);
    Variant(Variant &&other) noexcept;
    Variant &operator=(Variant &&other) noexcept;

    template <typename T>
    void operator()(const T &val)
    {
        toUAVariant(val, variant);
    }

    template <typename T>
    T get() const;

    inline const UA_Variant *getUAVariant() const { return variant; }
    inline UA_Variant *getUAVariant() { return variant; }

    bool isScalar() const;
    bool isEmpty() const;
    template <typename T>
    bool is_a() const
    {
        if constexpr (modernopc::is_vector<T>::value)
        {
            return variant->type == getDataType<removeConstRef_t<T>>();
        }
        return variant->type == getDataType<removeConstRef_t<T>>();
    }

    bool operator==(const Variant &other) const;

    std::string toString() const;
    

  private:
    UA_Variant *variant{nullptr};
    bool owned = false;
};

void toUAVariantImpl(const Variant &v, UA_Variant *var);
} // namespace modernopc