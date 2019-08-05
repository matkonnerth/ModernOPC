#pragma once
#include <open62541/server.h>
#include <functional>
#include "Variant.h"

class DataSource {
  public:
    DataSource(std::function<void(Variant&var)> read) {
        storedRead = read;
        src.read = &DataSource::internalRead;
        src.write = nullptr;
    }

    UA_DataSource
    getRawSource() const {
        return src;
    }

    void
    read(Variant&var) {
        return storedRead(var);
    }

  private:
    UA_DataSource src;
    std::function<void(Variant&var)> storedRead;

    static UA_StatusCode
    internalRead(UA_Server *server, const UA_NodeId *sessionId, void *sessionContext,
                 const UA_NodeId *nodeId, void *nodeContext,
                 UA_Boolean includeSourceTimeStamp, const UA_NumericRange *range,
                 UA_DataValue *value) {

        if(!nodeContext)
            return UA_STATUSCODE_BADNODATA;

        DataSource *ds = (DataSource *)nodeContext;
        Variant var{&value->value};
        ds->read(var);
        return UA_STATUSCODE_GOOD;
    }
};