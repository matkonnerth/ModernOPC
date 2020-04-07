#include <import/Extension.h>
#include <import/import.h>
#include <import/value.h>
#include <nodesetLoader/nodesetLoader.h>
#include <opc/NodeMetaInfo.h>
#include <opc/Server.h>
#include <opc/Variant.h>
#include <opc/events/BaseEventType.h>
#include <opc/types/NodeId.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>

namespace opc
{

Server::Server() { create(4840); }

Server::Server(uint16_t port) { create(port); }

Server::~Server() { UA_Server_delete(server); }

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
    sServer = this;
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
        if (s->call(objectContext, fromUaNodeId(*methodId),
                    inputArgs, outputArgs))
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
    // if(!nodeContext)
    //    return UA_STATUSCODE_BADNODATA;
    auto &ds = static_cast<Server *>(sServer)->getDataSources().at(0);
    // TODO: can we avoid this copy?
    // we can avoid it at the moment, because it's copied in Variant.get<>()
    Variant var{const_cast<UA_Variant *>(&value->value)};
    ds->write(fromUaNodeId(*nodeId), var);
    return UA_STATUSCODE_GOOD;
}

bool Server::readValue(const NodeId &id, Variant &var)
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

LocalizedText Server::readDisplayName(const NodeId &id)
{
    UA_LocalizedText lt;
    UA_Server_readDisplayName(server, fromNodeId(id), &lt);
    LocalizedText localized = fromUALocalizedText(&lt);
    UA_LocalizedText_clear(&lt);
    return localized;
}

UA_Server *Server::getUAServer() { return server; }

bool Server::addObject(const NodeId &parentId, const NodeId &requestedId,
                       const NodeId &typeId, const std::string &browseName,
                       void *context)
{
    UA_ObjectAttributes attr = UA_ObjectAttributes_default;
    UA_StatusCode status = UA_Server_addObjectNode(
        server, fromNodeId(requestedId), fromNodeId(parentId),
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(requestedId.getNsIdx(), (char *)browseName.c_str()),
        fromNodeId(typeId), attr, nullptr, nullptr);
    UA_Server_setNodeContext(server, fromNodeId(requestedId), context);
    return UA_STATUSCODE_GOOD == status;
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
        UA_BrowsePathResult_clear(&bpr);
        free(elements);
        return retval;
    }
    retval = bpr.statusCode;
    *outId = bpr.targets[0].targetId.nodeId;
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

    retval = UA_Server_triggerEvent(
        server, eventNodeId, fromNodeId(sourceNode), nullptr, UA_TRUE);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                       "Trigger event failed. StatusCode %s",
                       UA_StatusCode_name(retval));
    }
}

} // namespace opc