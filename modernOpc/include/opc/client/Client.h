#pragma once
#include <opc/UnresolvedNodeId.h>
#include <opc/types/NodeId.h>
#include <opc/Variant.h>
#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include <variant>
#include <opc/Logger.h>

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