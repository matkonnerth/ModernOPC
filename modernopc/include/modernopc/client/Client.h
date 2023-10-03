#pragma once
#include "Subscription.h"
#include <functional>
#include <map>
#include <modernopc/BrowseResult.h>
#include <modernopc/Logger.h>
#include <modernopc/UnresolvedNodeId.h>
#include <modernopc/Variant.h>
#include <modernopc/types/NodeId.h>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

struct UA_Client;
namespace modernopc
{

class Client
{
  public:
    enum class ConnectionState
    {
        DISCONNECTED,
        CONNECTED
    };
    using ConnectionStateCallback =
        std::function<void(const Client *client, ConnectionState state)>;
    Client(const std::string &endpointUri);
    ~Client();
    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;
    void connect();
    void disconnect();
    void doComm();
    void registerConnectionCallback(ConnectionStateCallback fn);
    void notifyConnectionState(ConnectionState state);
    NodeId resolve(const UnresolvedNodeId &id);
    int resolveNamespaceUri(const std::string &uri);
    ConnectionState getConnectionState();

    Variant read(const NodeId &id);
    void write(const NodeId &id, const Variant &var);
    std::vector<Variant> call(const NodeId &objId, const NodeId &methodId,
                              const std::vector<Variant> &inputArgs);
    std::vector<BrowseResult> browse(const NodeId &id);

    /**
     * loads a nodeset xml into an temporary server namespace and add the
     * datatypes to the client \param path path to nodeset file \param
     * namespaceIndex, in which the nodes are on server side, can be retrieved
     * with resolveNamespaceUri \return true on success, otherwise false. */
    bool loadNodeset(const std::string &path, int namespaceIndex);

    Variant createVariantFromJson(const std::string &jsonString,
                                  const NodeId &dataType);

    const struct UA_DataType *findCustomDataType(const UA_NodeId &typeId);

    void createSubscription();

    void createMonitoredItem(const NodeId &id);
    void clearMonitoredItems();

    void activateSession(const std::string &locale);

  private:
    UA_Client *client{nullptr};
    UA_ClientConfig *m_config{nullptr};
    std::string uri{};
    std::vector<ConnectionStateCallback> connectionStateCallbacks{};
    ConnectionState connState{ConnectionState::DISCONNECTED};
    std::vector<std::string> namespaces{};
    Logger logger{"Client"};
    Subscription m_subscription{};
};

} // namespace modernopc