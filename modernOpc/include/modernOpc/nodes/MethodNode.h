#pragma once
#include <memory>
#include <modernOpc/Variant.h>
#include <modernOpc/methods/Method.h>
#include <modernOpc/methods/MethodWrapper.h>
#include <modernOpc/nodes/Node.h>
#include <modernOpc/types/NodeId.h>
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
        server->connectMethodCallback(id, callable.get());
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