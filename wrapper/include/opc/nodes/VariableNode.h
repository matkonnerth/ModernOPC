#pragma once
#include <opc/nodes/Node.h>
#include <opc/NodeMetaInfo.h>

namespace opc
{

class VariableNode : public Node
{
  public:
    using Node::Node;
    void connectCallback(std::unique_ptr<NodeMetaInfo> info)
    {

        server->connectVariableDataSource(id, std::move(info));
    }

  private:
};

} // namespace opc