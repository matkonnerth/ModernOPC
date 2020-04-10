#include <opc/Server.h>
#include <opc/Variant.h>
#include <opc/nodes/VariableNode.h>

namespace opc
{
void VariableNode::connectCallback(std::unique_ptr<NodeMetaInfo> info)
{
    server->connectVariableDataSource(id, std::move(info));
}

bool VariableNode::read(Variant &var) const
{
    UA_Variant *v = var.getUAVariant();
    UA_Variant_clear(v);
    if (UA_STATUSCODE_GOOD ==
        UA_Server_readValue(server->getUAServer(), fromNodeId(id), v))
    {
        return true;
    }
    return false;
}

bool VariableNode::write(const Variant &var)
{
    if (UA_STATUSCODE_GOOD == UA_Server_writeValue(server->getUAServer(),
                                                   fromNodeId(id),
                                                   *var.getUAVariant()))
        ;
    {
        return true;
    }
    return false;
}

} // namespace opc