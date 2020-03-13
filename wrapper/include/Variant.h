#pragma once
#include "TypeConverter.h"

namespace opc {

class Variant {
  public:
    Variant()
    {
      variant = UA_Variant_new();
      owned = true;
    }

    Variant(UA_Variant *var, bool owner= false)
    {
      variant=var;
      owned = owner;
    }
    ~Variant()
    {
      if(owned)
      {
        UA_Variant_delete(variant);
      }
    }

    Variant(const Variant& other) = delete;
    Variant& operator=(const Variant& other) = delete;

    Variant(Variant&& other)
    {
      owned = other.owned;
      variant = other.variant;
    }

    Variant& operator=(Variant&& other)
    {
      if(owned)
      {
        UA_Variant_delete(variant);
      }
      owned = other.owned;
      variant = other.variant;
      return *this;
    }

    template <typename T>
    void
    operator()(T val) {
        TypeConverter::toUAVariant(val, variant);
    }

    template <typename T>
    T
    get() const {
        return TypeConverter::toStdType<T>(variant);
    }

    UA_Variant* data()
    {
      return variant;
    }

    const UA_Variant* data() const
    {
      return variant;
    }

  private:
    UA_Variant* variant {nullptr};
    bool owned = false;
};
}  // namespace opc