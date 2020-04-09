#pragma once
#include <opc/types/NodeId.h>
#include <opc/types/LocalizedText.h>
#include <opc/Server.h>
#include <vector>

namespace opc
{

class Node
{
  public:
    Node(Server *server, const NodeId &id) : server{server}, id{id} {}
    virtual ~Node() = default;

    const NodeId &getId() const;
    LocalizedText displayName() const;

  protected:
    Server *server;
    const NodeId id;
};

} // namespace opc