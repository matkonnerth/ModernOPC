#pragma once
#include <functional>

/*

NodeInfo in nodeContext: pair<datasourceKey> for attributes



Server
  registerDataSource(DataSource)
  deregisterDataSource(DataSource)


Datasource
  getKey()
  read(Variant var)
  write(Variant var)


UA_Server_Read(NodeInfo, NodeId, AttributeId, ) ->
  if NodeInfo[attribute]!=null
  -> getDatasource -> read()
  -> else getDatasource(NodeId)
  -> else getDataSource(namespaceIndex)
  -> return noDataSource
*/

namespace opc {
class Variant;

class NodeId;

class DataSource {
  public:
    DataSource(const std::string &key, std::function<void(const NodeId&, Variant &var)> read,
               std::function<void(const NodeId&, Variant &var)> write)
        : key(key), m_read(read), m_write(write) {}

    void
    read(const NodeId& id, Variant &var) {
        m_read(id, var);
    }

    void
    write(const NodeId& id, Variant &var) {
        m_write(id, var);
    }

    const std::string &
    getKey() const {
        return key;
    }

  private:
    std::string key;
    std::function<void(const NodeId&, Variant &)> m_read;
    std::function<void(const NodeId&, Variant &)> m_write;
};
}  // namespace opc