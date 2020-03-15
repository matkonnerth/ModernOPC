#include <TypeConverter.h>
#include <AppTypes.h>

//more or less an extension to the wrapper
namespace TypeConverter
{
template <>
const UA_DataType *
getDataType<app::types::Range2>() {
    return &UA_TYPES[UA_TYPES_RANGE];
}
}
