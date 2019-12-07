#pragma once
#include <string>

namespace opc
{


class NodeInfo
{
public:
  NodeInfo(const std::string &dataSourceKey, const std::string &nativeKey)
      : dataSourceKey(dataSourceKey), nativeKey(nativeKey) {}

    const std::string& getDataSourceKey() const
    {
        return dataSourceKey;
    }

    const std::string& getNativeKey() const
    {
        return nativeKey;
    }

private:
  std::string dataSourceKey {};
  std::string nativeKey {};
};

}  // namespace