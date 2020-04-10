#pragma once
#include <opc/nodes/Node.h>
#include <opc/NodeMetaInfo.h>

namespace opc
{
class Variant;
class VariableNode : public Node
{
  public:
    using Node::Node;
    bool write(const Variant &var);
    bool read(Variant &var) const;
    void connectCallback(std::unique_ptr<NodeMetaInfo> info);

  private:
};

} // namespace opc