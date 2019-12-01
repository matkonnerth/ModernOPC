#pragma once
#include "TypeConverter.h"

namespace opc {

    class Variant {
      public:
        Variant() : variant{UA_Variant_new(), UA_Variant_delete} {}
        Variant(UA_Variant *var) : variant{var, [](UA_Variant*){}} {}

        template <typename T>
        void
        operator()(T val) {
            TypeConverter::toUAVariant(val, variant.get());
        }

        template <typename T>
        T get() const {
            return TypeConverter::toStdType<T>(variant.get());
        }
       
      private:
        std::shared_ptr<UA_Variant> variant{nullptr};
    };
}