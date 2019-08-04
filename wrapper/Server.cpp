#include "Server.h"
#include <open62541/server.h>
#include <open62541/server_config_default.h>

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
}