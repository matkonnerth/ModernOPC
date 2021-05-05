#include <cassert>
#include <modernOpc/Variant.h>
#include <modernOpc/types/NodeId.h>
#include <open62541/types.h>
#include <open62541/types_generated_handling.h>
#include <ostream>
#include <variant>
#include <cstring>

namespace modernopc
{

const NodeId fromUaNodeId(const UA_NodeId &id)
{

    auto nsIdx = id.namespaceIndex;
    switch (id.identifierType)
    {
    case UA_NODEIDTYPE_NUMERIC:
        return NodeId(nsIdx, static_cast<int>(id.identifier.numeric));
        break;
    case UA_NODEIDTYPE_STRING:
        return NodeId(nsIdx, std::string(reinterpret_cast<char *>(
                                             id.identifier.string.data),
                                         id.identifier.string.length));
        break;
    case UA_NODEIDTYPE_GUID: {
        auto uag = id.identifier.guid;
        Guid g;
        g.data1 = uag.data1;
        g.data2 = uag.data2;
        g.data3 = uag.data3;
        std::memcpy(&g.data4, uag.data4, 8);
        return NodeId(nsIdx, g);
        break;
    }
    case UA_NODEIDTYPE_BYTESTRING:
        return NodeId(nsIdx, std::string(reinterpret_cast<char *>(
                                             id.identifier.byteString.data),
                                         id.identifier.byteString.length), NodeId::IdentifierType::BYTESTRING);
        break;
    }
    return NodeId(0, 0);
}

const UA_NodeId fromNodeId(const NodeId &nodeId)
{
    UA_NodeId id;
    UA_NodeId_init(&id);
    id.namespaceIndex = static_cast<UA_UInt16>(nodeId.getNsIdx());
    switch (nodeId.getIdType())
    {
    case NodeId::IdentifierType::NUMERIC:
        id.identifierType = UA_NodeIdType::UA_NODEIDTYPE_NUMERIC;
        id.identifier.numeric =
            static_cast<UA_UInt32>(std::get<int>(nodeId.getIdentifier()));
        break;
    case NodeId::IdentifierType::STRING:
        id.identifierType = UA_NodeIdType::UA_NODEIDTYPE_STRING;
        id.identifier.string = UA_STRING(const_cast<char *>(
            std::get<std::string>(nodeId.getIdentifier()).c_str()));
        break;
    case NodeId::IdentifierType::GUID:
        {
        auto g = std::get<Guid>(nodeId.getIdentifier());
        id.identifierType = UA_NodeIdType::UA_NODEIDTYPE_GUID;
        id.identifier.guid.data1 = g.data1;
        id.identifier.guid.data2 = g.data2;
        id.identifier.guid.data3 = g.data3;
        std::memcpy(&id.identifier.guid.data4, &g.data4, 8);
        break;
        }
    case NodeId::IdentifierType::BYTESTRING:
        {
            id.identifierType = UA_NodeIdType::UA_NODEIDTYPE_BYTESTRING;
            id.identifier.byteString = UA_BYTESTRING(const_cast<char *>(
                std::get<std::string>(nodeId.getIdentifier()).c_str()));
            break;
        }
    }
    return id;
}

void toUAVariantImpl(const NodeId &id, UA_Variant *var)
{
    const UA_NodeId uaId = fromNodeId(id);
    UA_Variant_setScalarCopy(var, &uaId, modernopc::getDataType<NodeId>());
}

template <>
NodeId Variant::get<NodeId>() const
{
    return fromUaNodeId(*static_cast<UA_NodeId *>(variant->data));
}

std::ostream &operator<<(std::ostream &os, const NodeId &id)
{
    os << "ns=" << id.getNsIdx();

    switch (id.getIdType())
    {
    case NodeId::IdentifierType::NUMERIC:
        os << ";i=" << std::get<int>(id.getIdentifier());
        break;
    case NodeId::IdentifierType::BYTESTRING:
        os << ";b=" << std::get<std::string>(id.getIdentifier());
        break;
    case NodeId::IdentifierType::STRING:
        os << ";s=" << std::get<std::string>(id.getIdentifier());
        break;
    case NodeId::IdentifierType::GUID: {
        auto g = std::get<Guid>(id.getIdentifier());
        os << ";g=" + std::to_string(g.data1) + "-" + std::to_string(g.data2) +
                  "-" + std::to_string(g.data3) + "-" +
                  std::to_string(g.data4[0]) + "-" + std::to_string(g.data4[1]) +
                  "-" + std::to_string(g.data4[2]) + "-" +
                  std::to_string(g.data4[3]) + "-" + std::to_string(g.data4[4]) +
                  "-" + std::to_string(g.data4[5]) + "-" +
                  std::to_string(g.data4[6]) + "-" + std::to_string(g.data4[7]);
        break;
    }
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
        case NodeId::IdentifierType::BYTESTRING:
        case NodeId::IdentifierType::STRING:
            return std::get<std::string>(i) == std::get<std::string>(other.i);
            break;
        case NodeId::IdentifierType::GUID:
            return std::get<Guid>(i) == std::get<Guid>(other.i);
            break;
        }
    }
    return false;
}

std::size_t getHash(const NodeId &id)
{
    size_t h1 = std::hash<uint16_t>()(id.nsIdx);
    size_t h2 = std::hash<std::variant<int, std::string, Guid>>()(id.i);
    return h1 ^ (h2 << 1);
}

/* FNV non-cryptographic hash function. See
 * https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function 
   copied from open62541
 */
#define FNV_PRIME_32 16777619
size_t ByteString_hash(const uint8_t *buf, size_t size)
{
    size_t fnv = 0u;
    for (size_t i = 0; i < size; ++i)
    {
        fnv = fnv ^ (buf[i]);
        fnv = fnv * FNV_PRIME_32;
    }
    return fnv;
}

std::size_t getHash(const Guid &id)
{
    return ByteString_hash(reinterpret_cast<const uint8_t*>(&id), sizeof(Guid));
}

bool Guid::operator==(const Guid& other) const
{
    auto cmp = std::memcmp(this, &other, sizeof(Guid));
    return cmp==0;
}

} // namespace modernopc
