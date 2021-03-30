#include <modernOpc/nodes/Node.h>
#include <modernOpc/types/NodeId.h>
#include <modernOpc/Server.h>

namespace modernopc
{

const NodeId &Node::getId() const
{
    return id;
}

LocalizedText Node::displayName() const
{
    UA_LocalizedText ualt;
    UA_LocalizedText_init(&ualt);
    UA_Server_readDisplayName(server->getUAServer(), fromNodeId(id), &ualt);
    LocalizedText lt = fromUALocalizedText(&ualt);
    UA_LocalizedText_clear(&ualt);
    return lt;
}

QualifiedName Node::browseName() const
{
    UA_QualifiedName uaqn;
    UA_QualifiedName_init(&uaqn);
    UA_Server_readBrowseName(server->getUAServer(), fromNodeId(id), &uaqn);
    QualifiedName qn = fromUAQualifiedName(&uaqn);
    UA_QualifiedName_clear(&uaqn);
    return qn;
}


} // namespace modernopc