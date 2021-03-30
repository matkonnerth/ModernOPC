#pragma once
#include <memory>
#include <modernOpc/DataSource.h>
#include <modernOpc/nodes/Node.h>

namespace opc
{
class Variant;
class VariableNode : public Node
{
  public:
    using Node::Node;
    bool write(const Variant &var);
    bool read(Variant &var) const;
    void connectCallback(std::unique_ptr<DataSource> dataSource);

  private:
    std::unique_ptr<DataSource> ds {};
};

} // namespace opc