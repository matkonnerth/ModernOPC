#pragma once
#include <open62541/types.h>
namespace opc
{
template <typename T>
const UA_DataType *getDataType();
} // namespace opc