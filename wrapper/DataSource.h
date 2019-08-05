#pragma once
#include <open62541/server.h>
#include <functional>
#include "Variant.h"

class DataSource {
  public:
    DataSource(std::function<void(Variant &var)> read) {
        storedRead = read;
        src.read = &DataSource::internalRead;
        src.write = nullptr;
    }

    DataSource(std::function<void(Variant &var)> read,
               std::function<void(Variant &var)> write) {
        storedRead = read;
        storedWrite = write;
        src.read = &DataSource::internalRead;
        src.write = &DataSource::internalWrite;
    }

    UA_DataSource
    getRawSource() const {
        return src;
    }

    void
    read(Variant&var) {
        return storedRead(var);
    }

    void
    write(Variant &var) {
        return storedWrite(var);
    }

  private:
    UA_DataSource src;
    std::function<void(Variant&var)> storedRead;
    std::function<void(Variant &var)> storedWrite;

    static UA_StatusCode
    internalRead(UA_Server *server, const UA_NodeId *sessionId, void *sessionContext,
                 const UA_NodeId *nodeId, void *nodeContext,
                 UA_Boolean includeSourceTimeStamp, const UA_NumericRange *range,
                 UA_DataValue *value) {

        if(!nodeContext)
            return UA_STATUSCODE_BADNODATA;

        DataSource *ds = (DataSource *)nodeContext;
        Variant var{&value->value, false};
        ds->read(var);
        return UA_STATUSCODE_GOOD;
    }

    static UA_StatusCode internalWrite(UA_Server *server, const UA_NodeId *sessionId,
                           void *sessionContext, const UA_NodeId *nodeId,
                           void *nodeContext, const UA_NumericRange *range,
                           const UA_DataValue *value)
    {
        if(!nodeContext)
            return UA_STATUSCODE_BADNODATA;

        DataSource *ds = (DataSource *)nodeContext;
        UA_Variant* uavar = UA_Variant_new();
        //TODO: can we avoid this copy?
        UA_Variant_copy(&value->value, uavar);
        Variant var{uavar, true};
        ds->write(var);
        return UA_STATUSCODE_GOOD;
    }
};