#pragma once
#include "TypeConverter.h"
#include <open62541/types.h>




class Variant
{
    public:
      Variant(UA_Variant* var) : variant{var} {}

      template<typename T>
      void operator()(T val)
      {
          ::TypeConverter::toVariant(val, variant);
          //TypeConverter::toVariant(val, variant);
      }

    private : 
      UA_Variant * variant;
};