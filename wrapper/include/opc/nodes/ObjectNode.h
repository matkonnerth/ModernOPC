#pragma once
#include <memory>
#include <opc/Server.h>
#include <opc/nodes/MethodNode.h>
#include <opc/nodes/Node.h>
#include <opc/types/Types.h>
#include <opc/methods/Method.h>

namespace opc
{

class ObjectNode : public Node
{
  public:
    using Node::Node;

    template <typename M>
    std::shared_ptr<MethodNode> addMethod(const NodeId &methodId,
                                          const QualifiedName &browseName,
                                          const M &memberFn)
    {

        auto method = server->createMethod(
            id, methodId, browseName, MethodTraits<M>::getInputArguments(),
            MethodTraits<M>::getOutputArguments());
        if (!method)
        {
            return nullptr;
        }
        method->bindCallable(memberFn);
        return method;
    }

    std::shared_ptr<ObjectNode> addObject(const NodeId &objId,
                                          const QualifiedName &browseName,
                                          void *context, const NodeId &typeId=NodeId(0,0));
};

} // namespace opc