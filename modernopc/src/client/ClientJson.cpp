#include <NodesetLoader/backendOpen62541.h>
#include <modernopc/OpcException.h>
#include <modernopc/client/Client.h>
#include <modernopc/nodes/Node.h>
#include <open62541/client.h>
#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/client_highlevel_async.h>
#include <open62541/client_subscriptions.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>

namespace modernopc
{

const struct UA_DataType *Client::findCustomDataType(const UA_NodeId &typeId)
{
    UA_ClientConfig *config = UA_Client_getConfig(client);
    const UA_DataTypeArray *types = config->customDataTypes;
    while (types)
    {
        const UA_DataTypeArray *next = types->next;
        if (types->types)
        {
            for (const UA_DataType *type = types->types;
                 type != types->types + types->typesSize; type++)
            {
                if (UA_NodeId_equal(&type->typeId, &typeId))
                {
                    return type;
                }
            }
        }
        types = next;
    }
    return NULL;
}

Variant Client::createVariantFromJson(const std::string &jsonString,
                                      const NodeId &dataType)
{
    const auto *type = findCustomDataType(fromNodeId(dataType));

    if (!type)
    {
        return Variant();
    }

    return fromJson(jsonString, type);
}

} // namespace modernopc