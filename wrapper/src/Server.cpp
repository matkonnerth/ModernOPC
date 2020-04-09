#include <import/Extension.h>
#include <import/import.h>
#include <import/value.h>
#include <nodesetLoader/nodesetLoader.h>
#include <opc/NodeMetaInfo.h>
#include <opc/Server.h>
#include <opc/Variant.h>
#include <opc/events/BaseEventType.h>
#include <opc/nodes/MethodNode.h>
#include <opc/nodes/ObjectNode.h>
#include <opc/types/NodeId.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <opc/nodes/VariableNode.h>

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

bool Server::call(void *objectContext, const NodeId &id,
                  const std::vector<Variant> &inputArgs,
                  std::vector<Variant> &outputArgs)
{

    auto it = methods.find(id);
    if (it != methods.end())
    {
        return it->second->invoke(objectContext, inputArgs, outputArgs);
    }
    return false;
}

UA_StatusCode Server::internalMethodCallback(
    UA_Server *server, const UA_NodeId *sessionId, void *sessionContext,
    const UA_NodeId *methodId, void *methodContext, const UA_NodeId *objectId,
    void *objectContext, size_t inputSize, const UA_Variant *input,
    size_t outputSize, UA_Variant *output)
{
    auto s = getServerFromContext(server);

    if (s)
    {
        std::vector<Variant> inputArgs;
        std::vector<Variant> outputArgs;
        for (size_t i = 0; i < inputSize; i++)
        {
            Variant v{const_cast<UA_Variant *>(&input[i])};
            inputArgs.push_back(std::move(v));
        }
        if (s->call(objectContext, fromUaNodeId(*methodId), inputArgs,
                    outputArgs))
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

    Server *s = getServerFromContext(server);
    for (auto &ds : s->getDataSources())
    {
        if (info->getDataSourceKey().compare(ds->getKey()) == 0)
        {
            Variant var{&value->value};
            ds->read(fromUaNodeId(*nodeId), var);
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
    // TODO: !!!
    auto &ds = getServerFromContext(server)->getDataSources().at(0);
    // TODO: can we avoid this copy?
    // we can avoid it at the moment, because it's copied in Variant.get<>()
    Variant var{const_cast<UA_Variant *>(&value->value)};
    ds->write(fromUaNodeId(*nodeId), var);
    return UA_STATUSCODE_GOOD;
}

bool Server::readValue(const NodeId &id, Variant &var) const
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

bool Server::writeValue(const NodeId &id, const Variant &var)
{
    if (UA_STATUSCODE_GOOD ==
        UA_Server_writeValue(server, fromNodeId(id), *var.getUAVariant()))
        ;
    {
        return true;
    }
    return false;
}

UA_Server *Server::getUAServer() { return server; }

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

UA_StatusCode Server::getNodeIdForPath(const UA_NodeId objectId,
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
    bp.startingNode = objectId;
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

UA_StatusCode Server::setUpEvent(UA_NodeId *outId, const BaseEventType &event)
{
    UA_StatusCode retval =
        UA_Server_createEvent(server, fromNodeId(event.getEventType()), outId);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                       "createEvent failed. StatusCode %s",
                       UA_StatusCode_name(retval));
        return retval;
    }

    /* Set the Event Attributes */
    /* Setting the Time is required or else the event will not show up in
     * UAExpert! */
    UA_DateTime eventTime = UA_DateTime_now();
    UA_Server_writeObjectProperty_scalar(
        server, *outId, UA_QUALIFIEDNAME(0, (char *)"Time"), &eventTime,
        &UA_TYPES[UA_TYPES_DATETIME]);

    UA_UInt16 eventSeverity = 100;
    UA_Server_writeObjectProperty_scalar(
        server, *outId, UA_QUALIFIEDNAME(0, (char *)"Severity"), &eventSeverity,
        &UA_TYPES[UA_TYPES_UINT16]);

    UA_LocalizedText eventMessage =
        UA_LOCALIZEDTEXT(nullptr, (char *)"An event has been generated.");
    UA_Server_writeObjectProperty_scalar(
        server, *outId, UA_QUALIFIEDNAME(0, (char *)"Message"), &eventMessage,
        &UA_TYPES[UA_TYPES_LOCALIZEDTEXT]);

    UA_String eventSourceName = UA_STRING((char *)"Server");
    UA_Server_writeObjectProperty_scalar(
        server, *outId, UA_QUALIFIEDNAME(0, (char *)"SourceName"),
        &eventSourceName, &UA_TYPES[UA_TYPES_STRING]);

    for (const auto &field : event.getEventFields())
    {
        UA_NodeId pathId;
        auto status = getNodeIdForPath(*outId, field.first, &pathId);
        if (status != UA_STATUSCODE_GOOD)
        {
            UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                           "Cannot resolve browsepath. StatusCode %s",
                           UA_StatusCode_name(retval));
            continue;
        }

        status =
            UA_Server_writeValue(server, pathId, *field.second.getUAVariant());
        if (status != UA_STATUSCODE_GOOD)
        {
            UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                           "Writing Event property failed. StatusCode %s",
                           UA_StatusCode_name(retval));
        }
    }

    return UA_STATUSCODE_GOOD;
}

void Server::setEvent(const BaseEventType &event, const opc::NodeId &sourceNode)
{
    UA_NodeId eventNodeId;
    UA_StatusCode retval = setUpEvent(&eventNodeId, event);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                       "Creating event failed. StatusCode %s",
                       UA_StatusCode_name(retval));
    }

    retval = UA_Server_triggerEvent(server, eventNodeId, fromNodeId(sourceNode),
                                    nullptr, UA_TRUE);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                       "Trigger event failed. StatusCode %s",
                       UA_StatusCode_name(retval));
    }
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
              Server::createVariable(const NodeId &parentId, const NodeId &requestedId, const NodeId &typeId,
                             const QualifiedName &browseName,
                             const UA_VariableAttributes &attr)
{
    auto status = UA_Server_addVariableNode(
        server, fromNodeId(requestedId), fromNodeId(parentId),
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        fromQualifiedName(browseName),
        fromNodeId(typeId), attr, nullptr,
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


void Server::connectVariableDataSource(const NodeId& id, std::unique_ptr<NodeMetaInfo> info)
{
    UA_Server_setVariableNode_dataSource(server, fromNodeId(id),
                                         internalSrc);
    UA_Server_setNodeContext(server, fromNodeId(id), info.get());
    nodeMetaInfos.emplace_back(std::move(info));
}

void Server::connectMethodCallback(const NodeId &id)
{
    UA_Server_setMethodNode_callback(server, fromNodeId(id),
                                     &internalMethodCallback);
}

} // namespace opc