#include "Server.h"
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

void Server::loadNodeset(const std::string &path)
{
    FileHandler f;
    f.addNamespace = UA_Server_addNamespace;
    f.server = server;
    f.file = path.c_str();
    UA_XmlImport_loadFile(&f);
}

UA_StatusCode
Server::internalMethodCallback(UA_Server *server, const UA_NodeId *sessionId,
                       void *sessionContext, const UA_NodeId *methodId,
                       void *methodContext, const UA_NodeId *objectId,
                       void *objectContext, size_t inputSize, const UA_Variant *input,
                       size_t outputSize, UA_Variant *output)
{
    opc::Server *s = nullptr;
    UA_Server_getNodeContext(
        server, *methodId, (void**)&s);

    if(s)
    {
        s->call(NodeId(*methodId));
    }
    return UA_STATUSCODE_GOOD;
}
}