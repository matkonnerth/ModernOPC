#include "Server.h"
#include "TypeConverter.h"
#include "import/import.h"
#include "nodesetLoader.h"
#include "import/value.h"
#include <NodeId.h>
#include <NodeMetaInfo.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include "import/Extension.h"

namespace opc
{
Server::Server() : isRunning{true}
{
    server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));
    UA_ServerConfig_addNetworkLayerWS(UA_Server_getConfig(server), 7681, 0, 0);
    sServer = this;
}

Server::~Server() { UA_Server_delete(server); }

void Server::run() { UA_Server_run(server, &isRunning); }

void Server::registerDataSource(
    const std::string &key,
    std::function<void(const NodeId &id, Variant &var)> read,
    std::function<void(const NodeId &id, Variant &var)> write)
{
    datasources.push_back(std::make_unique<DataSource>(key, read, write));
}

bool Server::loadNodeset(const std::string &path)
{

    FileContext handler;
    handler.callback = importNodesCallback;
    handler.addNamespace = addNamespaceCallback;
    handler.userContext = server;
    handler.file = path.c_str();

    ValueInterface valIf;
    valIf.userData = nullptr;
    valIf.newValue = Value_new;
    valIf.start = Value_start;
    valIf.end = Value_end;
    valIf.finish = Value_finish;
    valIf.deleteValue = Value_delete;
    handler.valueHandling = &valIf;

    ExtensionInterface extIf;
    extIf.userData = nullptr;
    extIf.newExtension = Extension_new;
    extIf.start = Extension_start;
    extIf.end = Extension_end;
    extIf.finish = Extension_finish;
    handler.extensionHandling = &extIf;
    return loadFile(&handler);
}

uint16_t Server::getNamespaceIndex(const std::string &uri)
{
    UA_Variant v;
    if (UA_STATUSCODE_GOOD !=
        UA_Server_readValue(
            server, UA_NODEID_NUMERIC(0, UA_NS0ID_SERVER_NAMESPACEARRAY), &v))
    {
        return 0;
    }
    std::vector<UA_String> namespaces{static_cast<UA_String *>(v.data),
                                        static_cast<UA_String *>(v.data) +
                                            v.arrayLength};
    for (size_t i = 0; i < namespaces.size(); i++)
    {
        std::string s{reinterpret_cast<char *>(namespaces[i].data),
                      namespaces[i].length};
        if (uri.compare(s) == 0)
        {
            UA_Variant_clear(&v);
            return static_cast<uint16_t>(i);
        }
    }
    UA_Variant_clear(&v);
    return 0;
}

bool Server::call(void* objectContext, const NodeId &id, const std::vector<Variant> &inputArgs,
                  std::vector<Variant> &outputArgs)
{
    ICallable *c = callbacks.at(id).get();
    if (c)
    {
        return c->call(objectContext, inputArgs, outputArgs);
    }
    return false;
}

UA_StatusCode Server::internalMethodCallback(
    UA_Server *server, const UA_NodeId *sessionId, void *sessionContext,
    const UA_NodeId *methodId, void *methodContext, const UA_NodeId *objectId,
    void *objectContext, size_t inputSize, const UA_Variant *input,
    size_t outputSize, UA_Variant *output)
{
    opc::Server *s{nullptr};
    UA_Server_getNodeContext(server, *methodId, (void **)&s);

    if (s)
    {
        std::vector<Variant> inputArgs;
        std::vector<Variant> outputArgs;
        for (size_t i = 0; i < inputSize; i++)
        {
            Variant v{const_cast<UA_Variant *>(&input[i])};
            inputArgs.push_back(std::move(v));
        }
        if (s->call(objectContext, opc::fromUaNodeId(*methodId), inputArgs, outputArgs))
        {
            outputSize = outputArgs.size();
            if (outputSize == 1)
            {
                outputArgs[0].copyToUaVariant(output);
            }
            return UA_STATUSCODE_GOOD;
        }
        return UA_STATUSCODE_BADMETHODINVALID;
    }
    return UA_STATUSCODE_BADMETHODINVALID;
}

UA_StatusCode
Server::internalRead(UA_Server *server, const UA_NodeId *sessionId,
                     void *sessionContext, const UA_NodeId *nodeId,
                     void *nodeContext, UA_Boolean includeSourceTimeStamp,
                     const UA_NumericRange *range, UA_DataValue *value)
{

    if (!nodeContext)
        return UA_STATUSCODE_BADNODATA;

    auto info = static_cast<NodeMetaInfo *>(nodeContext);
    for (auto &ds : static_cast<Server *>(sServer)->getDataSources())
    {
        if (info->getDataSourceKey().compare(ds->getKey()) == 0)
        {
            Variant var{&value->value};
            ds->read(opc::fromUaNodeId(*nodeId), var);
            return UA_STATUSCODE_GOOD;
        }
    }
    return UA_STATUSCODE_BADNODATA;
}

UA_StatusCode Server::internalWrite(UA_Server *server,
                                    const UA_NodeId *sessionId,
                                    void *sessionContext,
                                    const UA_NodeId *nodeId, void *nodeContext,
                                    const UA_NumericRange *range,
                                    const UA_DataValue *value)
{
    // if(!nodeContext)
    //    return UA_STATUSCODE_BADNODATA;
    auto &ds = static_cast<Server *>(sServer)->getDataSources().at(0);
    // TODO: can we avoid this copy?
    // we can avoid it at the moment, because it's copied in Variant.get<>()
    Variant var{const_cast<UA_Variant *>(&value->value)};
    ds->write(opc::fromUaNodeId(*nodeId), var);
    return UA_STATUSCODE_GOOD;
}

bool Server::readValue(const NodeId& id, Variant &var)
{
    UA_Variant *v = UA_Variant_new();
    if (UA_STATUSCODE_GOOD == UA_Server_readValue(server, fromNodeId(id), v))
    {
        var.set(v);
        return true;
    }
    UA_Variant_delete(v);
    return false;
}

types::LocalizedText Server::readDisplayName(const NodeId& id)
{
    UA_LocalizedText lt;
    UA_Server_readDisplayName(server, fromNodeId(id), &lt);
    types::LocalizedText localized = fromUALocalizedText(&lt);
    UA_LocalizedText_clear(&lt);
    return localized;
}

UA_Server *Server::getUAServer()
{
    return server;
}

} // namespace opc