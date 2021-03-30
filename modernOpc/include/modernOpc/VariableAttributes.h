#pragma once
#include <modernOpc/Variant.h>
#include <modernOpc/util.h>
#include <type_traits>

namespace opc
{

template <typename T>
const UA_VariableAttributes getVariableAttributes(const T &val)
{
    Variant var(val);
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    attr.dataType =
        getDataType<removeConstRef_t<T>>()->typeId;
    attr.valueRank = -1;
    UA_Variant_copy(var.getUAVariant(), &attr.value);
    return attr;
}


template <typename T>
const UA_VariableAttributes getVariableAttributes(const std::vector<T> &v)
{
    Variant var(v);
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    attr.dataType = getDataType<removeConstRef_t<T>>()->typeId;
    attr.valueRank = 1;
    attr.arrayDimensionsSize = 1;
    attr.arrayDimensions =
        static_cast<UA_UInt32 *>(UA_Array_new(1, &UA_TYPES[UA_TYPES_UINT32]));
    attr.arrayDimensions[0] = static_cast<UA_UInt32>(v.size());
    UA_Variant_copy(var.getUAVariant(), &attr.value);
    return attr;
}

} // namespace opc