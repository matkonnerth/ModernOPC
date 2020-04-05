#pragma once
#include <vector>
struct UA_Variant;

namespace opc {

class Variant {
  public:
    Variant();
    Variant(UA_Variant *var, bool owner = false);
    ~Variant();

    Variant(const Variant &other) = delete;
    Variant &
    operator=(const Variant &other) = delete;
    Variant(Variant &&other) noexcept;
    Variant &
    operator=(Variant &&other) noexcept;

    template <typename T>
    void
    operator()(T val);

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