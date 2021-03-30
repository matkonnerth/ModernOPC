#pragma once
#include <functional>
#include <map>
#include <modernOpc/Logger.h>
#include <modernOpc/UnresolvedNodeId.h>
#include <modernOpc/Variant.h>
#include <modernOpc/types/NodeId.h>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

struct UA_Client;
namespace opc {

class Client
{
public:
   enum class ConnectionState
   {
      DISCONNECTED,
      CONNECTED
   };
   using ConnectionStateCallback = std::function<void(const Client* client, ConnectionState state)>;
   Client(const std::string& endpointUri);
   ~Client();
   Client(const Client&) = delete;
   Client& operator=(const Client&) = delete;
   void connect();
   void disconnect();
   //void doComm();
   void registerConnectionCallback(ConnectionStateCallback fn);
   void notifyConnectionState(ConnectionState state);
   NodeId resolve(const UnresolvedNodeId& id);
   ConnectionState getConnectionState();


   Variant read(const NodeId& id);
   void write(const NodeId& id, const Variant& var);

private:
   UA_Client* client{ nullptr };
   std::string uri{};
   std::vector<ConnectionStateCallback> connectionStateCallbacks{};
   ConnectionState connState{ ConnectionState::DISCONNECTED };
   std::vector<std::string> namespaces{};
   Logger logger{"Client"};
};

} // namespace tt