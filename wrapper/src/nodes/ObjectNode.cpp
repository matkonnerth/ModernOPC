#include <opc/nodes/ObjectNode.h>
#include <opc/nodes/MethodNode.h>
#include <opc/methods/Method.h>
#include <opc/Server.h>

namespace opc
{
std::shared_ptr<ObjectNode>
ObjectNode::addObject(const NodeId &objId, const QualifiedName &browseName,
                      void *context, const NodeId &typeId)
{
    return server->createObject(id, objId, typeId, browseName, context);
}

} // namespace opc