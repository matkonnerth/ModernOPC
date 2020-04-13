#include <cassert>
#include "NodeId.h"
#include <ostream>
#include <variant>
#include <ostream>

namespace opc
{

std::ostream &operator<<(std::ostream &os, const NodeId &id)
{
    os << "ns=" << id.getNsIdx();

    switch (id.getIdType())
    {
    case NodeId::IdentifierType::NUMERIC:
        os << ";i=" << std::get<int>(id.getIdentifier());
        break;
    case NodeId::IdentifierType::STRING:
        os << ";s=" << std::get<std::string>(id.getIdentifier());
        break;
    }

    return os;
}

bool NodeId::operator==(const NodeId &other) const
{
    if (nsIdx != other.nsIdx)
        return false;
    if (type != other.type)
    {
        return false;
    }
    else
    {
        switch (type)
        {
        case NodeId::IdentifierType::NUMERIC:
            return std::get<int>(i) == std::get<int>(other.i);
            break;
        case NodeId::IdentifierType::STRING:
            return std::get<std::string>(i) == std::get<std::string>(other.i);
            break;
        }
    }
    return false;
}

std::size_t getHash(const NodeId &id)
{
    size_t h1 = std::hash<uint16_t>()(id.nsIdx);
    size_t h2 = std::hash<std::variant<int, std::string>>()(id.i);
    return h1 ^ (h2 << 1);
}

} // namespace opc
