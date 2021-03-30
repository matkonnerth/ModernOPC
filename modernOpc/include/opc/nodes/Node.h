#pragma once
#include <opc/types/LocalizedText.h>
#include <opc/types/NodeId.h>
#include <opc/types/QualifiedName.h>

namespace opc
{
class Server;
class Node
{
  public:
    Node(Server *server, const NodeId &id) : server{server}, id{id} {}
    virtual ~Node() = default;

    const NodeId &getId() const;
    LocalizedText displayName() const;
    QualifiedName browseName() const;

  protected:
    Server *server;
    const NodeId id;
};

} // namespace opc