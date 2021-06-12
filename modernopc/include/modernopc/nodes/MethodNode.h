#pragma once
#include <memory>
#include <modernopc/Variant.h>
#include <modernopc/methods/Method.h>
#include <modernopc/methods/MethodWrapper.h>
#include <modernopc/nodes/Node.h>
#include <modernopc/types/NodeId.h>
#include <vector>

namespace modernopc
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

} // namespace modernopc