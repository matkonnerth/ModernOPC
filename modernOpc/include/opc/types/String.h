#pragma once
#include <vector>
#include <type_traits>
#include <string>
#include <open62541/types.h>

namespace opc
{
inline std::string fromUAString(const UA_String& s)
{
    if (s.length > 0)
    {
        return std::string{reinterpret_cast<char *>(s.data), s.length};
    }
    return std::string{};
}

inline UA_String fromString(const std::string &v)
{
    return UA_STRING(const_cast<char *>(v.c_str()));
}

} // namespace opc


