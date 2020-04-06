#pragma once
#include <vector>
#include <opc/Conversion.h>


namespace opc {

template <typename T>
T toStdType(UA_Variant *variant);

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