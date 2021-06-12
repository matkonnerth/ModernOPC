#include <modernopc/nodes/Node.h>
#include <modernopc/types/NodeId.h>
#include <modernopc/Server.h>

namespace modernopc
{

NodeType getNodeType(UA_NodeClass kind)
{
    switch(kind)
    {
        case UA_NODECLASS_VARIABLE:
            return NodeType::VARIABLE;
        case UA_NODECLASS_OBJECT:
            return NodeType::OBJECT;
        case UA_NODECLASS_METHOD:
            return NodeType::METHOD;
        case UA_NODECLASS_OBJECTTYPE:
            return NodeType::OBJECTTYPE;
        case UA_NODECLASS_REFERENCETYPE:
            return NodeType::REFERENCETYPE;
        case UA_NODECLASS_DATATYPE:
            return NodeType::DATATYPE;
        case UA_NODECLASS_VIEW:
            return NodeType::VIEW;
        case UA_NODECLASS_UNSPECIFIED:
            return NodeType::UNKNOWN;
    };
    return NodeType::UNKNOWN;
}

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