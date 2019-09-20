#pragma once
#include <open62541/types.h>
#include "TypeConverter.h"

namespace opc {

    class Variant {
      public:
        //constructs and initializes Variant with UA_Variant
        Variant(UA_Variant *var, bool careForMem)
            : variant{var}, responsibleForMem{careForMem} {}

        //constructs empty variant
        Variant()
        {
            variant = UA_Variant_new();
            responsibleForMem=true;
        }


        template <typename T>
        void
        operator()(T val) {
            TypeConverter::toVariant(val, variant);
        }

        template <typename T>
        typename std::enable_if_t<std::is_arithmetic<T>::value, T>
        get() const {
            static_assert(std::is_arithmetic<T>::value, "must be a arithmetic type");
            assert(TypeConverter::isTypeMatching<T>(variant->type));
            return *static_cast<T *>(variant->data);
        }

        // specialize for vector
        template <typename T>
        typename std::enable_if_t<
            std::is_same<T, std::vector<typename T::value_type>>::value, T>
        get() const {
            static_assert(std::is_arithmetic<typename T::value_type>::value,
                          "must be a arithmetic type");
            assert(TypeConverter::isTypeMatching<typename T::value_type>(variant->type));
            return std::vector<typename T::value_type>{
                static_cast<typename T::value_type *>(variant->data),
                static_cast<typename T::value_type *>(variant->data) +
                    variant->arrayLength};
        }

        ~Variant() {
            if(responsibleForMem) {
                UA_Variant_delete(variant);
                variant = nullptr;
            }
        }

      private:
        UA_Variant *variant;
        bool responsibleForMem;
    };
}