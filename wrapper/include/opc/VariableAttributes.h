#pragma once
#include <opc/Variant.h>

namespace opc
{

template <typename T>
const UA_VariableAttributes getVariableAttributes(const T& val)
{
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.dataType =
        getDataType<std::remove_const_t<std::remove_reference_t<T>>>()->typeId;
    attr.valueRank = -1;
    return attr;
}

template <typename T>
const UA_VariableAttributes getVariableAttributes(const std::vector<T> &v)
{
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.dataType =
        getDataType<std::remove_const_t<std::remove_reference_t<T>>>()->typeId;
    attr.valueRank = 1;
    attr.arrayDimensionsSize = 1;
    attr.arrayDimensions = new UA_UInt32{static_cast<UA_UInt32>(v.size())};
    return attr;
}

} // namespace opc