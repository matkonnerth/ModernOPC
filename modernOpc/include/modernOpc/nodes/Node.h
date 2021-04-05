#pragma once
#include <modernOpc/types/LocalizedText.h>
#include <modernOpc/types/NodeId.h>
#include <modernOpc/types/QualifiedName.h>
#include <open62541/types_generated.h>

namespace modernopc
{

enum class NodeType
{
    UNKNOWN,
    OBJECT,
    OBJECTTYPE,
    VARIABLE,
    VARIABLETYPE,
    DATATYPE,
    REFERENCETYPE,
    METHOD,
    VIEW
};

NodeType getNodeType(UA_NodeClass kind);

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

} // namespace modernopc