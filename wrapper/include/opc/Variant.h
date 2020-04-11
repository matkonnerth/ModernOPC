#pragma once
#include <opc/DataType.h>
#include <opc/util.h>
#include <open62541/types.h>
#include <open62541/types_generated_handling.h>
#include <string>
#include <vector>
#include <utility>
#include <type_traits>
#include "util.h"

namespace opc
{

template <typename T>
T toStdType(UA_Variant *variant);

template <typename T>
void convertToUAVariantImpl(T val, UA_Variant *var);

template <typename T>
void convertToUAVariantImpl(std::vector<T> val, UA_Variant *var)
{
    static_assert(std::is_arithmetic_v<removeConstRef_t<T>>,
                  "Type not integral");
    UA_Variant_init(var);
    UA_Variant_setArrayCopy(var, val.data(), val.size(),
                            opc::getDataType<removeConstRef_t<T>>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

void convertToUAVariantImpl(std::vector<std::string> v, UA_Variant *var);
void convertToUAVariantImpl(std::string &v, UA_Variant *var);
void convertToUAVariantImpl(std::string &&v, UA_Variant *var);
void convertToUAVariantImpl(const std::string &v, UA_Variant *var);

template <typename T>
typename std::enable_if<!std::is_arithmetic_v<removeConstRef_t<T>>, void>::type
convertToUAVariant(T &&t, UA_Variant *var)
{
    convertToUAVariantImpl(std::forward<T>(t), var);
}

template <typename T>
typename std::enable_if<std::is_arithmetic_v<removeConstRef_t<T>>, void>::type
convertToUAVariant(T &&val, UA_Variant *var)
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
        convertToUAVariant(std::forward<T>(val), variant);
    }
    template<typename T>
    Variant(T &&val, bool owned)
    {
        variant = UA_Variant_new();
        convertToUAVariant(std::forward<T>(val), variant);
    }
    ~Variant();
    Variant(const Variant &other) = delete;
    Variant &operator=(const Variant &other) = delete;
    Variant(Variant &&other) noexcept;
    Variant &operator=(Variant &&other) noexcept;

    template <typename T>
    void operator()(T &&val)
    {
        convertToUAVariant(std::forward<T>(val), variant);
    }

    template <typename T>
    T get() const;

    void set(UA_Variant *var);

    void copy(UA_Variant *var) const;

    inline const UA_Variant *getUAVariant() const
    {
        return variant;
    }
    inline UA_Variant *getUAVariant()
    {
        return variant;
    }

    bool isScalar();
    bool isEmpty() const;
    template <typename T>
    bool is_a() const
    {
        if constexpr (opc::is_vector<T>::value)
        {
            return variant->type == getDataType<removeConstRef_t<T>>();
        }
        return variant->type == getDataType<removeConstRef_t<T>>();
    }

  private:
    UA_Variant *variant{nullptr};
    bool owned = false;
};
} // namespace opc