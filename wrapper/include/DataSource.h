#pragma once
#include <open62541/server.h>
#include <functional>

/*

NodeInfo in nodeContext: pair<datasourceKey, nativekey> for attributes



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
class DataSource {
  public:
    DataSource(const std::string &key, std::function<void(Variant &var)> read)
        : key(key), storedRead(read), storedWrite(nullptr){}

    DataSource(const std::string &key, std::function<void(Variant &var)> read,
               std::function<void(Variant &var)> write)
        : key(key), storedRead(read), storedWrite(write) {}

    void
    read(Variant &var)
    {
      storedRead(var);
    }

    void
    write(Variant &var)
    {
      storedRead(var);
    }

    const std::string& getKey() const
    {
      return key;
    }

  private:
    std::string key;
    std::function<void(Variant &var)> storedRead;
    std::function<void(Variant &var)> storedWrite;
};
}