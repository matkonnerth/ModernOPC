#pragma once
#include <open62541/server.h>
#include <functional>

namespace opc
{
class Variant;
class DataSource {
  public:
    DataSource(std::function<void(Variant &var)> read);

    DataSource(std::function<void(Variant &var)> read,
               std::function<void(Variant &var)> write);

    inline UA_DataSource
    getRawSource() const {
        return src;
    }

    void
    read(Variant &var);

    void
    write(Variant &var);

  private:
    UA_DataSource src;
    std::function<void(Variant &var)> storedRead;
    std::function<void(Variant &var)> storedWrite;

    static UA_StatusCode
    internalRead(UA_Server *server, const UA_NodeId *sessionId, void *sessionContext,
                 const UA_NodeId *nodeId, void *nodeContext,
                 UA_Boolean includeSourceTimeStamp, const UA_NumericRange *range,
                 UA_DataValue *value);

    static UA_StatusCode
    internalWrite(UA_Server *server, const UA_NodeId *sessionId, void *sessionContext,
                  const UA_NodeId *nodeId, void *nodeContext,
                  const UA_NumericRange *range, const UA_DataValue *value);
};
}