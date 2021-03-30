#pragma once
#include <NodeId.h>
#include <algorithm>
#include <types/EnumDataType.h>

namespace gen
{
using modernopc::NodeId;
inline std::string str_toupper(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   // static_cast<int(*)(int)>(std::toupper)         // wrong
                   // [](int c){ return std::toupper(c); }           // wrong
                   // [](char c){ return std::toupper(c); }          // wrong
                   [](unsigned char c) { return std::toupper(c); } // correct
    );
    return s;
}

inline std::string firstToLow(std::string s)
{
    s[0] = std::tolower(s[0]);
    return s;
}

} // namespace gen
