#pragma once
#include <vector>
#include <type_traits>
#include <string>
#include <open62541/types.h>
#include <opc/DataType.h>

namespace opc
{
inline std::string fromUAString(const UA_String *s)
{
    if (s->length > 0)
    {
        return std::string{reinterpret_cast<char *>(s->data), s->length};
    }
    return std::string{};
}
} // namespace opc


