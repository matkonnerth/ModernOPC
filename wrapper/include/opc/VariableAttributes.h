#pragma once

namespace opc
{

template <typename T>
UA_VariableAttributes getVariableAttributes(T val)
{
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    attr.dataType = getUADataTypeId<T>();
    attr.valueRank = -1;
    convertToUAVariant(val, &attr.value);
    return attr;
}

template <typename T>
UA_VariableAttributes getVariableAttributes(std::vector<T> &v)
{
    UA_VariableAttributes attr = UA_VariableAttributes_default;
    convertToUAVariant(v, &attr.value);
    attr.dataType = getUADataTypeId<T>();
    attr.valueRank = 1;
    attr.arrayDimensionsSize = 1;
    attr.arrayDimensions = new UA_UInt32{static_cast<UA_UInt32>(v.size())};
    return attr;
}

} // namespace opc