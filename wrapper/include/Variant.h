#pragma once
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
    Variant(Variant &&other);
    Variant &
    operator=(Variant &&other);

    template <typename T>
    void
    operator()(T val);

    template <typename T>
    T
    get() const;

    UA_Variant *
    data() {
        return variant;
    }

    const UA_Variant *
    data() const {
        return variant;
    }

  private:
    UA_Variant *variant{nullptr};
    bool owned = false;
};
}  // namespace opc