#pragma once
#include <memory>
#include <modernopc/DataSource.h>
#include <modernopc/nodes/Node.h>

namespace modernopc
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

} // namespace modernopc