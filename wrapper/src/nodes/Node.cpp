#include <opc/nodes/Node.h>
#include <opc/types/NodeId.h>
#include <opc/Server.h>

namespace opc
{

const NodeId &Node::getId() const
{
    return id;
}

LocalizedText Node::displayName() const
{
    UA_LocalizedText ualt;
    UA_Server_readDisplayName(server->getUAServer(), fromNodeId(id), &ualt);
    LocalizedText lt = fromUALocalizedText(&ualt);
    UA_LocalizedText_clear(&ualt);
    return lt;
}


} // namespace opc