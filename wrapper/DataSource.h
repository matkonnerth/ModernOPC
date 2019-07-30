#pragma once
#include <functional>
#include <open62541/server.h>
#include "TypeConverter.h"

template <typename T>
class DataSource
{
    public:
        DataSource(std::function<T(void)> read)
        {
            //src.read = std::bind(&internalRead, this, std::placeholders::_1);
            storedRead = read;
            src.read = &DataSource<T>::internalRead;
            src.write = nullptr;
        }

        UA_DataSource* getRawSource() const
        {
            return &src;
        }

    private:
        static std::function<T(void)> storedRead;
        static UA_DataSource src;


        static UA_StatusCode internalRead(
            UA_Server *server, const UA_NodeId *sessionId, void *sessionContext,
            const UA_NodeId *nodeId, void *nodeContext,
            UA_Boolean includeSourceTimeStamp, const UA_NumericRange *range,
            UA_DataValue *value) {
          auto val = storedRead();
          UA_Variant var = TypeConverter::toVariant(val);
        UA_Variant_copy(&var, &value->value);
          value->hasValue = true;
          value->status = UA_STATUSCODE_GOOD;
        }
};

template <typename T>
UA_DataSource DataSource<T>::src = {nullptr, nullptr};

template <typename T>
std::function<T(void)> DataSource<T>::storedRead = nullptr;