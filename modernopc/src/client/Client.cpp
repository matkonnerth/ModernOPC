#include <NodesetLoader/backendOpen62541.h>
#include <modernopc/OpcException.h>
#include <modernopc/client/Client.h>
#include <modernopc/nodes/Node.h>
#include <modernopc/types/String.h>
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
Client::Client(const std::string &endpointUri)
{
    uri = endpointUri;
    client = UA_Client_new();
    m_config = UA_Client_getConfig(client);
    UA_ClientConfig_setDefault(m_config);
    m_config->clientContext = this;
    m_config->logger = logger.getUALogger();
}

Client::~Client()
{
    disconnect();
    UA_Client_delete(client);
}

void Client::connect()
{
    UA_StatusCode retval = UA_Client_connect(client, uri.c_str());
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "Could not connect client %s with %s", uri.c_str(),
                     UA_StatusCode_name(retval));
        throw OpcException("could not connect");
    }

    UA_NodeId namespaceArrayId =
        UA_NODEID_NUMERIC(0, UA_NS0ID_SERVER_NAMESPACEARRAY);
    UA_Variant var;
    UA_Variant_init(&var);
    auto status = UA_Client_readValueAttribute(client, namespaceArrayId, &var);
    if (status != UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "could not read namespaceArray with %s",
                     UA_StatusCode_name(retval));
        throw OpcException("could not read namespace array");
    }
    for (auto i = 0u; i < var.arrayLength; i++)
    {
        auto s = static_cast<UA_String *>(var.data)[i];
        std::string str;
        namespaces.push_back(
            str.assign(reinterpret_cast<char *>(s.data), s.length));
    }
    UA_Variant_clear(&var);
    connState = ConnectionState::CONNECTED;
}

void Client::disconnect() { UA_Client_disconnect(client); }

void Client::registerConnectionCallback(ConnectionStateCallback fn)
{
    connectionStateCallbacks.emplace_back(fn);
}

void Client::notifyConnectionState(ConnectionState state)
{
    for (auto &cb : connectionStateCallbacks)
    {
        cb(this, state);
    }
}

int Client::resolveNamespaceUri(const std::string &uri)
{
    bool found = false;
    int idx = 0;
    for (const auto &nsUri : namespaces)
    {
        if (!uri.compare(nsUri))
        {
            found = true;
            break;
        }
        idx++;
    }
    if (!found)
    {
        throw OpcException("namespaceUri of nodeId not found");
    }
    return idx;
}

NodeId Client::resolve(const UnresolvedNodeId &id)
{
    auto idx = static_cast<uint16_t>(resolveNamespaceUri(id.namespaceUri));

    switch (id.identifier[0])
    {
    case 'i':
        return NodeId(idx, std::stoi(id.identifier.substr(2)));
        break;
    case 's':
        return NodeId(idx, id.identifier.substr(2));
        break;
    default:
        throw OpcException("could not parse nodeId");
    }
}

Client::ConnectionState Client::getConnectionState() { return connState; }

Variant Client::read(const NodeId &id)
{
    UA_Variant *v = UA_Variant_new();
    modernopc::Variant var{v, true};
    auto status = UA_Client_readValueAttribute(client, fromNodeId(id), v);
    if (status != UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "could not read with NodeId %s",
                     UA_StatusCode_name(status));
        throw OpcException("Error on readValueAttribute");
    }

    return var;
}

void Client::write(const NodeId &id, const Variant &var)
{
    auto status = UA_Client_writeValueAttribute(client, fromNodeId(id),
                                                var.getUAVariant());
    if (status != UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "could not write with NodeId %s",
                     UA_StatusCode_name(status));
        throw OpcException("Error on writeValueAttribute");
    }
}

std::vector<Variant> Client::call(const NodeId &objId, const NodeId &methodId,
                                  const std::vector<Variant> &inputArgs)
{
    UA_Variant input[inputArgs.size()];
    size_t cnt = 0;
    for (const auto &inputVar : inputArgs)
    {
        input[cnt] = *inputVar.getUAVariant();
        cnt++;
    }

    size_t outputSize;
    UA_Variant *output;

    auto status =
        UA_Client_call(client, fromNodeId(objId), fromNodeId(methodId),
                       inputArgs.size(), input, &outputSize, &output);

    if (status != UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "error on call %s",
                     UA_StatusCode_name(status));
        throw OpcException("error on call");
    }

    std::vector<Variant> out;
    for (auto var = output; var < output + outputSize; var++)
    {
        out.emplace_back(Variant(var, true));
    }
    return out;
}

std::vector<BrowseResult> Client::browse(const NodeId &id)
{
    UA_BrowseRequest bReq;
    UA_BrowseRequest_init(&bReq);
    bReq.requestedMaxReferencesPerNode = 0;
    bReq.nodesToBrowse = UA_BrowseDescription_new();
    bReq.nodesToBrowseSize = 1;
    const UA_NodeId startId = fromNodeId(id);
    UA_NodeId_copy(&startId, &bReq.nodesToBrowse[0].nodeId);
    bReq.nodesToBrowse[0].resultMask =
        UA_BROWSERESULTMASK_ALL; /* return everything */
    bReq.nodesToBrowse->referenceTypeId =
        UA_NODEID_NUMERIC(0, UA_NS0ID_HIERARCHICALREFERENCES);
    bReq.nodesToBrowse->includeSubtypes = UA_TRUE;
    UA_BrowseResponse bResp = UA_Client_Service_browse(client, bReq);

    if (bResp.responseHeader.serviceResult != UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "error on browse %s",
                     UA_StatusCode_name(bResp.responseHeader.serviceResult));
        throw OpcException("error on browse");
    }

    std::vector<BrowseResult> results;

    for (size_t i = 0; i < bResp.resultsSize; ++i)
    {
        for (size_t j = 0; j < bResp.results[i].referencesSize; ++j)
        {
            UA_ReferenceDescription *ref = &(bResp.results[i].references[j]);

            results.emplace_back(
                BrowseResult(fromUaNodeId(ref->nodeId.nodeId),
                             fromUAQualifiedName(&ref->browseName),
                             getNodeType(ref->nodeClass)));
        }
    }
    UA_BrowseRequest_clear(&bReq);
    UA_BrowseResponse_clear(&bResp);
    return results;
}

bool Client::loadNodeset(const std::string &path, int namespaceIndex)
{
    auto *server = UA_Server_new();
    if (!NodesetLoader_loadFile(server, path.c_str(), nullptr))
    {
        UA_Server_delete(server);
        return false;
    }

    auto *customDataTypes = UA_Server_getConfig(server)->customDataTypes;
    UA_Server_getConfig(server)->customDataTypes = nullptr;
    UA_Client_getConfig(client)->customDataTypes = customDataTypes;

    for (auto *t = (UA_DataType *)customDataTypes->types;
         t != customDataTypes->types + customDataTypes->typesSize; t++)
    {
        t->binaryEncodingId.namespaceIndex = namespaceIndex;
        t->typeId.namespaceIndex = namespaceIndex;
    }

    UA_Server_delete(server);
    return true;
}

void Client::doComm() { UA_Client_run_iterate(client, 50); }

void Client::createSubscription() { m_subscription.create(client); }

void Client::createMonitoredItem(const NodeId &id)
{
    m_subscription.createMonitoredItemAsyncBegin(client, id);
}

void Client::clearMonitoredItems()
{
    m_subscription.clearAllMonitoredItems(client);
}

void Client::activateSession(const std::string& locale)
{
    m_config->sessionLocaleIdsSize = 1;
    m_config->sessionLocaleIds =
        (UA_LocaleId *)UA_Array_new(1, &UA_TYPES[UA_TYPES_LOCALEID]);

    UA_String s{};
    s.data = (UA_Byte*)(locale.c_str());
    s.length = locale.length();
    UA_String_copy(&s, &m_config->sessionLocaleIds[0]);

    UA_Client_activateCurrentSession(client);
}
}
