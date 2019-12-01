#include "include/Server.h"
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <open62541/plugin/nodesetLoader.h>

namespace opc {
Server::Server() : isRunning{true} {
    server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));
}

Server::~Server() { UA_Server_delete(server); }

void
Server::run() {
    UA_Server_run(server, &isRunning);
}

void
Server::setDataSource(const NodeId &id, DataSource &src) {
    UA_Server_setNodeContext(server, id.toUA_NodeId(), &src);
    UA_Server_setVariableNode_dataSource(server, id.toUA_NodeId(), src.getRawSource());
}

void Server::loadNodeset(const std::string &path)
{
    FileHandler f;
    f.addNamespace = UA_Server_addNamespace;
    f.server = server;
    f.file = path.c_str();
    UA_XmlImport_loadFile(&f);
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
            return UA_STATUSCODE_GOOD;
        }
        return UA_STATUSCODE_BADMETHODINVALID;
    }
    return UA_STATUSCODE_BADMETHODINVALID;
}
}