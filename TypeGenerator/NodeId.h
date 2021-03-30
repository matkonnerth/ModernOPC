#pragma once
#include <variant>
#include <string>
namespace modernopc
{

class NodeId
{

  public:
    enum class IdentifierType
    {
        NUMERIC,
        STRING
    };
    NodeId()
        : nsIdx(0), type(NodeId::IdentifierType::NUMERIC), i(0)
    {
    }

    NodeId(uint16_t nsIdx, int id)
        : nsIdx(nsIdx), type(NodeId::IdentifierType::NUMERIC), i(id)
    {
    }
    NodeId(uint16_t nsIdx, const std::string &id)
        : nsIdx(nsIdx), type(NodeId::IdentifierType::STRING), i(id)
    {
    }

    uint16_t getNsIdx() const { return nsIdx; }
    auto getIdType() const { return type; }
    const std::variant<int, std::string> &getIdentifier() const { return i; }
    bool operator==(const NodeId &other) const;
    friend std::size_t getHash(const NodeId &id);

    friend std::ostream &operator<<(std::ostream &os, const NodeId &id);

  private:
    uint16_t nsIdx{0};
    IdentifierType type{IdentifierType::NUMERIC};
    std::variant<int, std::string> i{0};
};

} // namespace modernopc

namespace std
{
template <>
struct hash<modernopc::NodeId>
{
    inline size_t operator()(const modernopc::NodeId &id) const
    {
        return getHash(id);
    }
};
} // namespace std