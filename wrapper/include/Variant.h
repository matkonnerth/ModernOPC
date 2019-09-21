#pragma once
#include "TypeConverter.h"

namespace opc {

    class Variant {
      public:
        //constructs and initializes Variant with UA_Variant
        Variant(UA_Variant *var, bool careForMem)
            : variant{var}, responsibleForMem{careForMem} {}

        Variant();
        ~Variant();

        template <typename T>
        void
        operator()(T val) {
            TypeConverter::toUAVariant(val, variant);
        }

        template <typename T>
        T get() const {
            return TypeConverter::toStdType<T>(variant);
        }
        
      private:
        UA_Variant *variant;
        bool responsibleForMem;
    };
}