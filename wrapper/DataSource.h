#pragma once
#include <open62541/server.h>
#include <functional>
#include "DataValue.h"
#include "TypeConverter.h"

class DataSource {
  public:
    DataSource(std::function<void(DataValue &val)> read) {
        storedRead = read;
        src.read = &DataSource::internalRead;
        src.write = nullptr;
    }

    UA_DataSource
    getRawSource() const {
        return src;
    }

    void
    read(DataValue &val) {
        return storedRead(val);
    }

  private:
    UA_DataSource src;
    std::function<void(DataValue &val)> storedRead;

    static UA_StatusCode
    internalRead(UA_Server *server, const UA_NodeId *sessionId, void *sessionContext,
                 const UA_NodeId *nodeId, void *nodeContext,
                 UA_Boolean includeSourceTimeStamp, const UA_NumericRange *range,
                 UA_DataValue *value) {

        if(!nodeContext)
            return UA_STATUSCODE_BADNODATA;

        DataSource *ds = (DataSource *)nodeContext;
        DataValue val{value};
        ds->read(val);
        return UA_STATUSCODE_GOOD;
    }
};