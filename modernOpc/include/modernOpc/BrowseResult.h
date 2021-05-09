#pragma once
#include <modernOpc/types/NodeId.h>
#include <modernOpc/types/QualifiedName.h>
#include <modernOpc/nodes/Node.h>

namespace modernopc
{

class BrowseResult final
{
  public:
    BrowseResult() = default;
    BrowseResult(const NodeId &id, const QualifiedName &name, NodeType type)
        : m_id{id}, m_qn{name}, m_type{type}  
    {
    }

    ~BrowseResult() = default;
    BrowseResult(const BrowseResult &) = default;
    BrowseResult &operator=(const BrowseResult &) = default;
    BrowseResult(BrowseResult &&) = default;
    BrowseResult &operator=(BrowseResult &&) = default;

    const NodeId &Id() const { return m_id; }
    const QualifiedName &Name() const { return m_qn; }
    NodeType Type() const {return m_type;};

  private:
    NodeId m_id{};
    QualifiedName m_qn{};
    NodeType m_type{NodeType::UNKNOWN};
};
} // namespace modernopc