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

void
Server::call(NodeId id) {
    ICallable *c = callbacks.at(std::get<int>(id.getIdentifier())).get();
    if(c) {
        std::vector<std::variant<int, double>> vec;
        std::variant<int, double> val{10};
        vec.push_back(val);
        c->call(vec);
    }
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

void
Server::registerStructType() {
    struct DemoStruct {
        int x;
        int y;
    };

    UA_ServerConfig *config = UA_Server_getConfig(server);
    // setup custom type array
    UA_DataTypeArray *typeArray = (UA_DataTypeArray *)calloc(1, sizeof(UA_DataTypeArray));
    typeArray->next = NULL;
    // typeArray->typesSize = 1;
    UA_DataType *type = (UA_DataType *)(calloc(1, sizeof(UA_DataType)));

    // the datatpye
    type->typeKind = UA_DATATYPEKIND_STRUCTURE;
    type->binaryEncodingId = 100;
    type->typeId = UA_NODEID_NUMERIC(1, 100);

    type->typeIndex = 0;

    // need some sort of reflection for this
    type->pointerFree = true;
    type->overlayable = false;
    type->memSize = sizeof(DemoStruct);
    type->membersSize = 2;
    type->members = (UA_DataTypeMember *)UA_calloc(2, sizeof(UA_DataTypeMember));

    // X
    type->members[0].isArray = false;
    type->members[0].namespaceZero = true;
    type->members[0].memberTypeIndex = UA_TYPES_INT32;
    type->members[0].padding = 0;

    // Y
    UA_DataTypeMember *memberY =
        (UA_DataTypeMember *)UA_calloc(1, sizeof(UA_DataTypeMember));
    type->members[0].isArray = false;
    type->members[0].namespaceZero = true;
    type->members[0].memberTypeIndex = UA_TYPES_INT32;
    type->members[0].padding =
        offsetof(DemoStruct, y) - offsetof(DemoStruct, x) - sizeof(int);

    typeArray->types = type;
    config->customDataTypes = typeArray;
}
}