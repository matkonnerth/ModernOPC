#pragma once
#include <string>

namespace modernopc {
class UnresolvedNodeId
{
public:
   UnresolvedNodeId(const std::string& ns, const std::string identifier)
   : namespaceUri{ ns }
   , identifier{ identifier }
   {}
   const std::string namespaceUri;
   const std::string identifier;
   friend bool operator<(const UnresolvedNodeId& lhs, const UnresolvedNodeId& rhs)
   {
      if (lhs.namespaceUri != rhs.namespaceUri)
      {
         return lhs.namespaceUri < rhs.namespaceUri;
      }
      return lhs.identifier < rhs.identifier;
}
};
} // namespace modernopctest::client
