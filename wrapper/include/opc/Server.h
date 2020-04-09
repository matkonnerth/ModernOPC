#pragma once
#include <memory>
#include <opc/DataSource.h>
#include <opc/NodeMetaInfo.h>
#include <opc/types/NodeId.h>
#include <opc/types/QualifiedName.h>
#include <opc/types/LocalizedText.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config.h>
#include <open62541/types.h>
#include <unordered_map>

struct UA_Server;

namespace opc
{
class BaseEventType;
class MethodNode;
class ObjectNode;
class VariableNode;

class Server
{

    friend VariableNode;
    friend MethodNode;

  public:
    Server();
    explicit Server(uint16_t port);
    ~Server();
    Server(const Server &) = delete;
    Server &operator=(const Server &) = delete;
    Server(Server &&) = delete;
    Server &operator=(Server &&) = delete;

    void run();
    bool loadNodeset(const std::string &path);

    std::shared_ptr<MethodNode>
    createMethod(const NodeId &objId, const NodeId &methodId,
                 const QualifiedName &browseName,
                 const std::vector<UA_Argument> &in,
                 const std::vector<UA_Argument> &outArgs);

    bool call(void *objectContext, const NodeId &id,
              const std::vector<Variant> &inputArgs,
              std::vector<Variant> &outputArgs);

    std::shared_ptr<VariableNode>
    createVariable(const NodeId &parentId, const NodeId &requestedId,
                   const NodeId &typeId, const QualifiedName &browseName,
                   const UA_VariableAttributes &attr);

    void registerDataSource(
        const std::string &key,
        std::function<void(const NodeId &id, Variant &var)> read,
        std::function<void(const NodeId &id, Variant &var)> write);

    auto &getDataSources() { return datasources; }
    bool writeValue(const NodeId &id, const Variant &var);
    bool readValue(const NodeId &id, Variant &var) const;
    uint16_t getNamespaceIndex(const std::string &uri);
    UA_Server *getUAServer();
    const UA_Server *getUAServer() const;
    void setEvent(const BaseEventType &event, const opc::NodeId &sourceNode);

    std::shared_ptr<ObjectNode> getObject(const NodeId &);
    std::shared_ptr<ObjectNode> createObject(const NodeId &parentId,
                                             const NodeId &requestedId,
                                             const NodeId &typeId,
                                             const QualifiedName &browseName,
                                             void *context);
    std::shared_ptr<ObjectNode> getObjectsFolder();
    std::shared_ptr<MethodNode> getMethod(const NodeId &id);
    std::shared_ptr<MethodNode> createMethod(const NodeId &objectId,
                                             const NodeId &methodId,
                                             QualifiedName browseName);

  private:
    void connectVariableDataSource(const NodeId &id,
                                   std::unique_ptr<NodeMetaInfo> info);
    void connectMethodCallback(const NodeId &id);
    UA_Server *server{nullptr};
    UA_DataSource internalSrc{internalRead, internalWrite};
    bool isRunning{false};
    static UA_StatusCode internalMethodCallback(
        UA_Server *server, const UA_NodeId *sessionId, void *sessionContext,
        const UA_NodeId *methodId, void *methodContext,
        const UA_NodeId *objectId, void *objectContext, size_t inputSize,
        const UA_Variant *input, size_t outputSize, UA_Variant *output);
    std::vector<std::unique_ptr<DataSource>> datasources{};
    std::vector<std::unique_ptr<NodeMetaInfo>> nodeMetaInfos{};
    void create(uint16_t port);
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
    UA_StatusCode setUpEvent(UA_NodeId *outId, const BaseEventType &event);

    UA_StatusCode getNodeIdForPath(const UA_NodeId objectId,
                                   const std::vector<QualifiedName> &qn,
                                   UA_NodeId *outId);

    std::unordered_map<NodeId, std::shared_ptr<ObjectNode>> objects{};
    std::unordered_map<NodeId, std::shared_ptr<MethodNode>> methods{};
    std::unordered_map<NodeId, std::shared_ptr<VariableNode>> variables{};
};
} // namespace opc