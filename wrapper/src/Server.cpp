#include <import/Extension.h>
#include <import/import.h>
#include <import/value.h>
#include <nodesetLoader/nodesetLoader.h>
#include <opc/DataSource.h>
#include <opc/Server.h>
#include <opc/Variant.h>
#include <opc/nodes/MethodNode.h>
#include <opc/nodes/ObjectNode.h>
#include <opc/nodes/VariableNode.h>
#include <opc/types/NodeId.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>

namespace opc
{

Server::Server() { create(4840); }

Server::Server(uint16_t port) { create(port); }

Server::~Server() { UA_Server_delete(server); }

Server *getServerFromContext(UA_Server *server)
{
    Server *s = nullptr;
    UA_Server_getNodeContext(server, UA_NODEID_NUMERIC(0, UA_NS0ID_SERVER),
                             (void **)&s);
    return s;
}

void setServerContext(UA_Server *server, Server *s)
{
    UA_Server_setNodeContext(server, UA_NODEID_NUMERIC(0, UA_NS0ID_SERVER), s);
}

void Server::create(uint16_t port)
{
    server = UA_Server_new();
    auto config = UA_Server_getConfig(server);
    auto status = UA_ServerConfig_setMinimal(config, port, nullptr);
    if (status != UA_STATUSCODE_GOOD)
    {
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                       "Setting serer config failed with StatusCode %s",
                       UA_StatusCode_name(status));
    }
    // UA_ServerConfig_addNetworkLayerWS(UA_Server_getConfig(server), 7681, 0,
    // 0);

    setServerContext(server, this);
}

void Server::run()
{
    isRunning = true;
    UA_Server_run(server, &isRunning);
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

UA_StatusCode Server::internalMethodCallback(
    UA_Server *server, const UA_NodeId *sessionId, void *sessionContext,
    const UA_NodeId *methodId, void *methodContext, const UA_NodeId *objectId,
    void *objectContext, size_t inputSize, const UA_Variant *input,
    size_t outputSize, UA_Variant *output)
{
    ICallable *s = static_cast<ICallable *>(methodContext);
    if (s)
    {

        std::vector<Variant> inputArgs;
        std::vector<Variant> outputArgs;
        outputArgs.emplace_back(Variant(const_cast<UA_Variant *>(output)));
        for (size_t i = 0; i < inputSize; i++)
        {
            inputArgs.emplace_back(
                Variant(const_cast<UA_Variant *>(&input[i])));
        }
        if (s->call(objectContext, inputArgs, outputArgs))
        {
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

    auto ds = static_cast<DataSource *>(nodeContext);
    Variant var{&value->value, false};
    ds->read(fromUaNodeId(*nodeId), var);
    return UA_STATUSCODE_GOOD;
}

UA_StatusCode Server::internalWrite(UA_Server *server,
                                    const UA_NodeId *sessionId,
                                    void *sessionContext,
                                    const UA_NodeId *nodeId, void *nodeContext,
                                    const UA_NumericRange *range,
                                    const UA_DataValue *value)
{
    if (!nodeContext)
        return UA_STATUSCODE_BADNODATA;
    auto ds = static_cast<DataSource *>(nodeContext);
    Variant var{const_cast<UA_Variant*>(&value->value),false};
    ds->write(fromUaNodeId(*nodeId), var);
    return UA_STATUSCODE_GOOD;
}

UA_Server *Server::getUAServer() { return server; }

UA_StatusCode Server::getNodeIdForPath(const UA_NodeId &startId,
                                       const std::vector<QualifiedName> &qn,
                                       UA_NodeId *outId)
{
    auto *elements = static_cast<UA_RelativePathElement *>(
        calloc(qn.size(), sizeof(UA_RelativePathElement)));
    std::vector<UA_RelativePathElement> pathElements{elements,
                                                     elements + qn.size()};

    std::size_t cnt = 0;
    for (auto &path : pathElements)
    {
        UA_RelativePathElement_init(&path);
        path.referenceTypeId =
            UA_NODEID_NUMERIC(0, UA_NS0ID_HIERARCHICALREFERENCES);
        path.isInverse = false;
        path.includeSubtypes = true;
        path.targetName = fromQualifiedName(qn[cnt]);
        cnt++;
    }

    UA_BrowsePath bp;
    UA_BrowsePath_init(&bp);
    bp.startingNode = startId;
    bp.relativePath.elementsSize = qn.size();
    bp.relativePath.elements = pathElements.data();

    UA_StatusCode retval;
    UA_BrowsePathResult bpr =
        UA_Server_translateBrowsePathToNodeIds(server, &bp);
    if (bpr.statusCode != UA_STATUSCODE_GOOD || bpr.targetsSize < 1)
    {
        retval = bpr.statusCode;
        for (auto &path : pathElements)
        {
            cnt++;
        }
        UA_BrowsePathResult_clear(&bpr);
        free(elements);
        return retval;
    }
    retval = bpr.statusCode;
    *outId = bpr.targets[0].targetId.nodeId;
    cnt = 0;
    for (auto &path : pathElements)
    {

        cnt++;
    }
    UA_BrowsePathResult_clear(&bpr);
    free(elements);
    return retval;
}

std::shared_ptr<ObjectNode>
Server::createObject(const NodeId &parentId, const NodeId &requestedId,
                     const NodeId &typeId, const QualifiedName &browseName,
                     void *context)
{
    UA_ObjectAttributes attr = UA_ObjectAttributes_default;
    auto status = UA_Server_addObjectNode(
        server, fromNodeId(requestedId), fromNodeId(parentId),
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        fromQualifiedName(browseName), fromNodeId(typeId), attr, nullptr,
        nullptr);

    if (status != UA_STATUSCODE_GOOD)
    {
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                       "addObject failed. StatusCode %s",
                       UA_StatusCode_name(status));
        return nullptr;
    }
    UA_Server_setNodeContext(server, fromNodeId(requestedId), context);
    auto ptr = std::make_shared<ObjectNode>(this, requestedId);
    objects.emplace(requestedId, ptr);
    return ptr;
}

std::shared_ptr<ObjectNode> Server::getObject(const NodeId &id)
{
    auto it = objects.find(id);
    if (it != objects.end())
    {
        return it->second;
    }
    UA_NodeClass nodeClass;
    auto status = UA_Server_readNodeClass(server, fromNodeId(id), &nodeClass);
    if (status != UA_STATUSCODE_GOOD)
    {
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                       "getObject failed. StatusCode %s",
                       UA_StatusCode_name(status));
        return nullptr;
    }
    if (nodeClass != UA_NODECLASS_OBJECT)
    {
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                       "Nodeclass mismatch.");
        return nullptr;
    }
    auto ptr = std::make_shared<ObjectNode>(this, id);
    objects.emplace(id, ptr);
    return ptr;
}

std::shared_ptr<MethodNode> Server::getMethod(const NodeId &id)
{
    auto it = methods.find(id);
    if (it != methods.end())
    {
        return it->second;
    }
    UA_NodeClass nodeClass;
    auto status = UA_Server_readNodeClass(server, fromNodeId(id), &nodeClass);
    if (status != UA_STATUSCODE_GOOD)
    {
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                       "getObject failed. StatusCode %s",
                       UA_StatusCode_name(status));
        return nullptr;
    }
    if (nodeClass != UA_NODECLASS_METHOD)
    {
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                       "Nodeclass mismatch.");
        return nullptr;
    }
    auto ptr = std::make_shared<MethodNode>(this, id);
    methods.emplace(id, ptr);
    return ptr;
}

// template<typename M>
std::shared_ptr<MethodNode>
Server::createMethod(const NodeId &objId, const NodeId &methodId,
                     const QualifiedName &browseName,
                     const std::vector<UA_Argument> &inArgs,
                     const std::vector<UA_Argument> &outArgs)
{
    UA_MethodAttributes methAttr = UA_MethodAttributes_default;
    methAttr.executable = true;
    methAttr.userExecutable = true;

    const UA_Argument *out = nullptr;
    if (outArgs.size() > 0)
    {
        out = outArgs.data();
    }
    auto status = UA_Server_addMethodNode(
        server, fromNodeId(methodId), fromNodeId(objId),
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        fromQualifiedName(browseName), methAttr, &internalMethodCallback,
        inArgs.size(), inArgs.data(), outArgs.size(), out, nullptr, nullptr);

    if (status != UA_STATUSCODE_GOOD)
    {
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                       "Creating method node failed %s",
                       UA_StatusCode_name(status));
        return nullptr;
    }
    auto ptr = std::make_shared<MethodNode>(this, methodId);
    methods.emplace(methodId, ptr);
    return ptr;
}

std::shared_ptr<ObjectNode> Server::getObjectsFolder()
{
    return getObject(NodeId(0, 85));
}

std::shared_ptr<VariableNode>
Server::createVariable(const NodeId &parentId, const NodeId &requestedId,
                       const NodeId &typeId, const QualifiedName &browseName,
                       const UA_VariableAttributes &attr)
{
    auto status = UA_Server_addVariableNode(
        server, fromNodeId(requestedId), fromNodeId(parentId),
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        fromQualifiedName(browseName), fromNodeId(typeId), attr, nullptr,
        nullptr);
    if (status != UA_STATUSCODE_GOOD)
    {
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                       "Creating variable node failed %s",
                       UA_StatusCode_name(status));
        return nullptr;
    }
    auto ptr = std::make_shared<VariableNode>(this, requestedId);
    variables.emplace(requestedId, ptr);
    return ptr;
}

std::shared_ptr<VariableNode> Server::getVariable(const NodeId &id)
{
    auto it = variables.find(id);
    if (it != variables.end())
    {
        return it->second;
    }
    UA_NodeClass nodeClass;
    auto status = UA_Server_readNodeClass(server, fromNodeId(id), &nodeClass);
    if (status != UA_STATUSCODE_GOOD)
    {
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                       "getObject failed. StatusCode %s",
                       UA_StatusCode_name(status));
        return nullptr;
    }
    if (nodeClass != UA_NODECLASS_VARIABLE)
    {
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                       "Nodeclass mismatch.");
        return nullptr;
    }
    auto ptr = std::make_shared<VariableNode>(this, id);
    variables.emplace(id, ptr);
    return ptr;
}

void Server::connectVariableDataSource(const NodeId &id,
                                       DataSource* src)
{
    UA_Server_setVariableNode_dataSource(server, fromNodeId(id), internalSrc);
    UA_Server_setNodeContext(server, fromNodeId(id), src);
}

void Server::connectMethodCallback(const NodeId &id, ICallable *callable)
{
    UA_Server_setMethodNode_callback(server, fromNodeId(id),
                                     &internalMethodCallback);
    UA_Server_setNodeContext(server, fromNodeId(id), callable);
}

} // namespace opc