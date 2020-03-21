#pragma once
#include <DataSource.h>
#include "Method.h"
#include <MethodWrapper.h>
#include <NodeId.h>
#include <NodeMetaInfo.h>
#include <map>
#include <open62541/server.h>
#include <open62541/server_config.h>

struct UA_Server;
namespace opc
{

class Server
{
  public:
    Server();
    ~Server();
    Server(const Server &) = delete;
    Server &operator=(const Server &) = delete;
    Server(Server &&) = delete;
    Server &operator=(Server &&) = delete;

    void run();

    bool loadNodeset(const std::string &path);
    template <typename R, typename... ARGS>
    void addMethod(const std::string &name, std::function<R(ARGS...)> fn)
    {
        std::vector<UA_Argument> inputArgs =
            MethodTraits<decltype(fn)>::getInputArguments();
        std::vector<UA_Argument> outputArgs =
            MethodTraits<decltype(fn)>::getOutputArguments();

        UA_MethodAttributes methAttr = UA_MethodAttributes_default;
        methAttr.executable = true;
        methAttr.userExecutable = true;

        UA_NodeId newId;
        UA_Server_addMethodNode(
            server, UA_NODEID_NULL,
            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
            UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
            UA_QUALIFIEDNAME(1, const_cast<char *>(name.c_str())), methAttr,
            nullptr, MethodTraits<decltype(fn)>::getNumArgs(), inputArgs.data(),
            1, outputArgs.data(), nullptr, &newId);

        UA_Server_setMethodNode_callback(server, newId, internalMethodCallback);
        UA_Server_setNodeContext(server, newId, this);

        callbacks.insert(std::pair<const NodeId, std::unique_ptr<ICallable>>(
            fromUaNodeId(newId),
            std::make_unique<Call<R, ARGS...>>(fn)));
    }

    template <typename R, typename... ARGS>
    void bindMethodNode(const NodeId &id, std::function<R(ARGS...)> fn)
    {
        UA_Server_setMethodNode_callback(server, fromNodeId(id),
                                         internalMethodCallback);
        callbacks.insert(std::pair<const NodeId, std::unique_ptr<ICallable>>(
            id, std::make_unique<Call<R, ARGS...>>(fn)));
        UA_Server_setNodeContext(server, fromNodeId(id), this);
    }

    bool call(const NodeId &id, const std::vector<Variant> &inputArgs,
              std::vector<Variant> &outputArgs);

    template <typename T>
    void addVariableNode(const NodeId &parentId, const NodeId &requestedId,
                         const std::string &browseName, T initialValue,
                         std::unique_ptr<NodeMetaInfo> info)
    {
        // memleak with info.release()!!
        UA_VariableAttributes attr =
            getVariableAttributes(initialValue);
        attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
        UA_Server_addVariableNode(
            server, fromNodeId(requestedId),
            fromNodeId(parentId),
            UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
            UA_QUALIFIEDNAME(1, (char *)browseName.c_str()),
            UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr,
            info.release(), nullptr);

        UA_Server_setVariableNode_dataSource(
            server, fromNodeId(requestedId), internalSrc);
    }

    template <typename T>
    void addVariableNode(const NodeId &parentId, const NodeId &requestedId,
                         const std::string &browseName, T initialValue)
    {
        UA_VariableAttributes attr =
            getVariableAttributes(initialValue);
        attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
        UA_Server_addVariableNode(
            server, fromNodeId(requestedId),
            fromNodeId(parentId),
            UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
            UA_QUALIFIEDNAME(1, (char *)browseName.c_str()),
            UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr, nullptr,
            nullptr);
    }

    void registerDataSource(
        const std::string &key,
        std::function<void(const NodeId &id, Variant &var)> read,
        std::function<void(const NodeId &id, Variant &var)> write);

    auto &getDataSources() { return datasources; }

    bool readValue(const NodeId id, Variant &var);

    uint16_t getNamespaceIndex(const std::string& uri);

  private:
    UA_Server *server{nullptr};
    UA_DataSource internalSrc{internalRead, internalWrite};
    bool isRunning{false};
    static UA_StatusCode internalMethodCallback(
        UA_Server *server, const UA_NodeId *sessionId, void *sessionContext,
        const UA_NodeId *methodId, void *methodContext,
        const UA_NodeId *objectId, void *objectContext, size_t inputSize,
        const UA_Variant *input, size_t outputSize, UA_Variant *output);
    std::map<const NodeId, std::unique_ptr<ICallable>> callbacks{};
    std::vector<std::unique_ptr<DataSource>> datasources{};

    inline static void *sServer{nullptr};

    static UA_StatusCode
    internalRead(UA_Server *server, const UA_NodeId *sessionId,
                 void *sessionContext, const UA_NodeId *nodeId,
                 void *nodeContext, UA_Boolean includeSourceTimeStamp,
                 const UA_NumericRange *range, UA_DataValue *value);

    static UA_StatusCode
    internalWrite(UA_Server *server, const UA_NodeId *sessionId,
                  void *sessionContext, const UA_NodeId *nodeId,
                  void *nodeContext, const UA_NumericRange *range,
                  const UA_DataValue *value);
};
} // namespace opc