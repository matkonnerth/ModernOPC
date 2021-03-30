#pragma once
#include <memory>
#include <opc/types/NodeId.h>
#include <opc/types/QualifiedName.h>
#include <open62541/server.h>
#include <unordered_map>
#include <atomic>

struct UA_Server;

namespace opc
{
class BaseEventType;
class MethodNode;
class ObjectNode;
class VariableNode;
class ICallable;
class DataSource;
class Variant;

/**
  Main interface for the server implementation.
*/
class Server
{

    friend VariableNode;
    friend MethodNode;
    friend ObjectNode;

  public:
    Server();
    explicit Server(uint16_t port);
    ~Server();
    Server(const Server &) = delete;
    Server &operator=(const Server &) = delete;
    Server(Server &&) = delete;
    Server &operator=(Server &&) = delete;

    void run();

    void stop();

    /**
     * loads a nodeset xml into server address space.
     * \param path path to nodeset file
     * \return true on success, otherwise false. */
    bool loadNodeset(const std::string &path);

    uint16_t getNamespaceIndex(const std::string &uri);
    UA_Server *getUAServer();
    const UA_Server *getUAServer() const;

    std::shared_ptr<ObjectNode> getObject(const NodeId &);
    std::shared_ptr<ObjectNode> createObject(const NodeId &parentId,
                                             const NodeId &requestedId,
                                             const NodeId &typeId,
                                             const QualifiedName &browseName,
                                             void *context);
    std::shared_ptr<ObjectNode> getObjectsFolder();
    std::shared_ptr<MethodNode> getMethod(const NodeId &id);

    std::shared_ptr<VariableNode>
    createVariable(const NodeId &parentId, const NodeId &requestedId,
                   const NodeId &typeId, const QualifiedName &browseName,
                   const UA_VariableAttributes &attr);

    std::shared_ptr<MethodNode>
    createMethod(const NodeId &objId, const NodeId &methodId,
                 const QualifiedName &browseName,
                 const std::vector<UA_Argument> &in,
                 const std::vector<UA_Argument> &outArgs);

    std::shared_ptr<VariableNode> getVariable(const NodeId &);

    UA_StatusCode translatePathToNodeId(const NodeId &id,
                                        const std::vector<QualifiedName> &qn,
                                        NodeId &outId);

  private:
    void connectVariableDataSource(const NodeId &id, DataSource *info);
    void connectMethodCallback(const NodeId &id, ICallable *c);
    UA_Server *server{nullptr};
    UA_DataSource internalSrc{internalRead, internalWrite};
    bool isRunning{false};
    static UA_StatusCode internalMethodCallback(
        UA_Server *server, const UA_NodeId *sessionId, void *sessionContext,
        const UA_NodeId *methodId, void *methodContext,
        const UA_NodeId *objectId, void *objectContext, size_t inputSize,
        const UA_Variant *input, size_t outputSize, UA_Variant *output);
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

    std::unordered_map<NodeId, std::shared_ptr<ObjectNode>> objects{};
    std::unordered_map<NodeId, std::shared_ptr<MethodNode>> methods{};
    std::unordered_map<NodeId, std::shared_ptr<VariableNode>> variables{};
};
} // namespace opc