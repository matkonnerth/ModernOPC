#pragma once
#include <iostream>
#include <variant>

namespace opc {

class NodeId {
    

  public:
    NodeId(int nsIdx, int id)
        : nsIdx(nsIdx), type(NodeId::IdentifierType::NUMERIC), i(id) {}
    NodeId(int nsIdx, const std::string &id)
        : nsIdx(nsIdx), type(NodeId::IdentifierType::STRING), i(id) {}

    int
    getNsIdx() const {
        return nsIdx;
    }

    auto
    getIdType() const {
        return type;
    }

    const std::variant<int, std::string> &
    getIdentifier() const {
        return i;
    }

    static std::size_t
    getIdentifierHash(const NodeId &id) {
        switch(id.type) {
            case IdentifierType::NUMERIC:
                return static_cast<std::size_t>(std::get<int>(id.i));
                break;
            case IdentifierType::STRING:
                return std::hash<std::string>()(std::get<std::string>(id.i));
                break;
        }
        return 0;
    }

    bool
    operator<(const NodeId &other) const {
        if(nsIdx < other.nsIdx)
            return true;
        if(other.nsIdx < nsIdx)
            return false;
        // nsIdx same
        return getIdentifierHash(*this) < getIdentifierHash(other);
    }

    enum class IdentifierType { NUMERIC, STRING };

  private:
    int nsIdx{0};
    IdentifierType type{IdentifierType::NUMERIC};
    std::variant<int, std::string> i{0};
};

}  // namespace opc