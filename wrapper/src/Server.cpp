#include "Server.h"
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <NodeId.h>
#include <NodeMetaInfo.h>
#include "nodesetLoader.h"
#include "import.h"
#include "value.h"

namespace opc {
Server::Server() : isRunning{true} {
    server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));
    sServer = this;
}

Server::~Server() { UA_Server_delete(server); }

void
Server::run() {
    UA_Server_run(server, &isRunning);
}

void
Server::registerDataSource(const std::string &key,
                           std::function<void(const NodeId &id, Variant &var)> read,
                           std::function<void(const NodeId &id, Variant &var)> write) {
    datasources.push_back(std::make_unique<DataSource>(key, read, write));
}

void Server::loadNodeset(const std::string &path)
{

    FileHandler handler;
    handler.callback = importNodesCallback;
    handler.addNamespace = addNamespaceCallback;
    handler.userContext = server;
    handler.file = path.c_str();

    ValueInterface valIf;
    valIf.userData=nullptr;
    valIf.newValue = Value_new;
    valIf.start = Value_start;
    valIf.end = Value_end;
    valIf.finish = Value_finish;
    handler.valueHandling = &valIf;
    loadFile(&handler);
}

bool
Server::call(const NodeId& id, const std::vector<Variant>& inputArgs, std::vector<Variant>& outputArgs) {
    ICallable *c = callbacks.at(id).get();
    if(c) {
        return c->call(inputArgs, outputArgs);
    }
    return false;
}

UA_StatusCode
Server::internalMethodCallback(UA_Server *server, const UA_NodeId *sessionId,
                       void *sessionContext, const UA_NodeId *methodId,
                       void *methodContext, const UA_NodeId *objectId,
                       void *objectContext, size_t inputSize, const UA_Variant *input,
                       size_t outputSize, UA_Variant *output)
{
    opc::Server *s {nullptr};
    UA_Server_getNodeContext(
        server, *methodId, (void**)&s);

    if(s)
    {
        std::vector<Variant> inputArgs;
        std::vector<Variant> outputArgs;
        for(size_t i=0; i< inputSize; i++)
        {
            Variant v{const_cast<UA_Variant*>(&input[i])};
            inputArgs.push_back(v);
        }
        if (s->call(NodeId(*methodId), inputArgs, outputArgs))
        {
            outputSize=outputArgs.size();
            if(outputSize==1)
            {
                UA_Variant_copy(outputArgs[0].getImpl().get(), output);
            }
            return UA_STATUSCODE_GOOD;
        }
        return UA_STATUSCODE_BADMETHODINVALID;
    }
    return UA_STATUSCODE_BADMETHODINVALID;
}

UA_StatusCode
Server::internalRead(UA_Server *server, const UA_NodeId *sessionId,
                         void *sessionContext, const UA_NodeId *nodeId, void *nodeContext,
                         UA_Boolean includeSourceTimeStamp, const UA_NumericRange *range,
                         UA_DataValue *value) {

    if(!nodeContext)
        return UA_STATUSCODE_BADNODATA;

    auto info = static_cast<NodeMetaInfo*>(nodeContext);
    for(auto &ds : static_cast<Server *>(sServer)->getDataSources())
    {
        if(info->getDataSourceKey().compare(ds->getKey())==0)
        {
            Variant var{&value->value};
            ds->read(NodeId{*nodeId}, var);
            return UA_STATUSCODE_GOOD;
        }
    }
    return UA_STATUSCODE_BADNODATA;
}

UA_StatusCode
Server::internalWrite(UA_Server *server, const UA_NodeId *sessionId,
                          void *sessionContext, const UA_NodeId *nodeId,
                          void *nodeContext, const UA_NumericRange *range,
                          const UA_DataValue *value) {
    //if(!nodeContext)
    //    return UA_STATUSCODE_BADNODATA;
    auto &ds = static_cast<Server *>(sServer)->getDataSources().at(0);
    // TODO: can we avoid this copy?
    // we can avoid it at the moment, because it's copied in Variant.get<>()
    Variant var{const_cast<UA_Variant *>(&value->value)};
    ds->write(NodeId{*nodeId}, var);
    return UA_STATUSCODE_GOOD;
}
}