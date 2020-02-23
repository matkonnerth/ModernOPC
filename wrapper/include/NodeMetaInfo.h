#pragma once
#include <string>

namespace opc
{


class NodeMetaInfo
{
public:
  NodeMetaInfo(const std::string &dataSourceKey)
      : dataSourceKey(dataSourceKey) {}

    const std::string& getDataSourceKey() const
    {
        return dataSourceKey;
    }

private:
  std::string dataSourceKey {};
};

}  // namespace