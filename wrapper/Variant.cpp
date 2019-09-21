#include "include/Variant.h"
#include <open62541/types.h>

namespace opc
{


// constructs empty variant
Variant::Variant() {
    variant = UA_Variant_new();
    responsibleForMem = true;
}

Variant::~Variant() {
    if(responsibleForMem) {
        UA_Variant_delete(variant);
        variant = nullptr;
    }
}
}