#pragma once
#include "TypeConverter.h"
#include <open62541/types.h>

class Variant
{
    public:
      Variant(UA_Variant* var, bool careForMem) : variant{var}, responsibleForMem{careForMem}  {}

      template<typename T>
      void operator()(T val)
      {
          TypeConverter::toVariant(val, variant);
      }

      template <typename T>
      T get()
      {
          assert(TypeConverter::isTypeMatching<T>(variant->type));
          return static_cast<T>(*(T *)variant->data);
      }

      ~Variant()
      {
        if(responsibleForMem)
        {
            UA_Variant_delete(variant);
        }
      }

    private : 
      UA_Variant * variant;
      bool responsibleForMem;
};