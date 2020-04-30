#pragma once
#include <memory>
#include <opc/Server.h>
#include <opc/VariableAttributes.h>
#include <opc/methods/Method.h>
#include <opc/nodes/MethodNode.h>
#include <opc/nodes/Node.h>
#include <opc/types/QualifiedName.h>

namespace opc
{
class BaseEventType;

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
                                          void *context,
                                          const NodeId &typeId = NodeId(0, 0));

    template <typename T>
    std::shared_ptr<VariableNode>
    addVariable(const NodeId &requestedId, const NodeId &typeId,
                const QualifiedName &browseName, T &&initialValue)
    {
        UA_VariableAttributes attr = getVariableAttributes(initialValue);
        auto var =
            server->createVariable(id, requestedId, typeId, browseName, attr);
        UA_VariableAttributes_clear(&attr);
        return var;
    }

    template <typename T>
    std::shared_ptr<VariableNode>
    addBaseDataTypeVariable(const NodeId &requestedId,
                            const QualifiedName &browseName, T &&initialValue)
    {
        return addVariable(requestedId,
                           NodeId(0, UA_NS0ID_BASEDATAVARIABLETYPE), browseName,
                           std::forward<T>(initialValue));
    }

    // Statuscode Missing
    void setEvent(BaseEventType &event);
    UA_StatusCode eventNotifier(UA_Byte& value);

  private:
    UA_StatusCode setUpEvent(UA_NodeId *outId, const BaseEventType &event);
};

} // namespace opc