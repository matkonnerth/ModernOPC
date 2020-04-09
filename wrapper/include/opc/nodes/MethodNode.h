#pragma once
#include <memory>
#include <opc/Variant.h>
#include <opc/methods/Method.h>
#include <opc/methods/MethodWrapper.h>
#include <opc/nodes/Node.h>
#include <opc/types/NodeId.h>
#include <vector>

namespace opc
{

class MethodNode : public Node
{
  public:
    using Node::Node;
    template <typename M>
    void bindCallable(const M &memberFn)
    {
        typename MethodTraits<M>::type fn{memberFn};
        callable = std::make_unique<Call<decltype(fn)>>(fn);
        server->connectMethodCallback(id);
    }

    inline bool invoke(void *objectContext,
                       const std::vector<Variant> &inputArgs,
                       std::vector<Variant> &outputArgs)
    {
        if (callable)
        {
            return callable->call(objectContext, inputArgs, outputArgs);
        }
        return false;
    }

  private:
    std::unique_ptr<ICallable> callable{};
};

} // namespace opc