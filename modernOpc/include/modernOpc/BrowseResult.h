#pragma once
#include <modernOpc/types/NodeId.h>
#include <modernOpc/types/QualifiedName.h>

namespace modernopc
{
class BrowseResult final
{
  public:
    BrowseResult(const NodeId &id, const QualifiedName &name)
        : m_id{id}, m_qn{name}
    {
    }

    ~BrowseResult()=default;
    BrowseResult(const BrowseResult&)=default;
    BrowseResult& operator=(const BrowseResult&)=default;
    BrowseResult(BrowseResult&&)=default;
    BrowseResult& operator=(BrowseResult&&)=default;

    const NodeId &Id() const { return m_id; }
    const QualifiedName &Name() const { return m_qn; }

  private:
    NodeId m_id;
    QualifiedName m_qn;
};
} // namespace modernOpc