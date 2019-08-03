#pragma once
#include <functional>
#include <open62541/server.h>
#include "TypeConverter.h"

template <typename T>
class DataSource
{
    public:
        DataSource(std::function<T()> read)
        {
            storedRead = read;
            src.read = &DataSource<T>::internalRead;
            src.write = nullptr;
        }

        UA_DataSource getRawSource() const
        {
            return src;
        }

        T read()
        {
            return storedRead();
        }

    private:
        UA_DataSource src;
        std::function<T()> storedRead;

        static UA_StatusCode internalRead(
            UA_Server *server, const UA_NodeId *sessionId, void *sessionContext,
            const UA_NodeId *nodeId, void *nodeContext,
            UA_Boolean includeSourceTimeStamp, const UA_NumericRange *range,
            UA_DataValue *value) {

                if(!nodeContext)
                    return UA_STATUSCODE_BADNODATA;

                DataSource *ds = (DataSource *)nodeContext;
                auto val = ds->read();
                UA_Variant var = TypeConverter::toVariant(val);
                UA_Variant_copy(&var, &value->value);
                value->hasValue = true;
                value->status = UA_STATUSCODE_GOOD;
                return UA_STATUSCODE_GOOD;
        }
};