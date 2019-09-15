#pragma once
#include <open62541/server.h>
#include <iostream>
#include <variant>

class NodeId {
    enum class IdentifierType { NUMERIC, STRING };

  public:
    NodeId(int nsIdx, int id)
        : nsIdx(nsIdx), type(NodeId::IdentifierType::NUMERIC), i(id) {}
    NodeId(int nsIdx, const std::string &id)
        : nsIdx(nsIdx), type(NodeId::IdentifierType::STRING), i(id) {}
    NodeId(UA_NodeId id) {

        nsIdx = id.namespaceIndex;
        switch(id.identifierType) {
            case UA_NODEIDTYPE_NUMERIC:
                type = IdentifierType::NUMERIC;
                i = id.identifier.numeric;
                break;
            case UA_NODEIDTYPE_STRING:
                type = IdentifierType::STRING;
                i = std::string(reinterpret_cast<char*>(id.identifier.string.data), id.identifier.string.length);
                break;
        }
    }

    inline UA_NodeId
    toUA_NodeId() const {
        UA_NodeId id;
        UA_NodeId_init(&id);
        id.namespaceIndex = nsIdx;
        switch(type) {
            case IdentifierType::NUMERIC:
                id.identifierType = UA_NodeIdType::UA_NODEIDTYPE_NUMERIC;
                id.identifier.numeric = std::get<int>(i);
                break;
            case IdentifierType::STRING:
                id.identifierType = UA_NodeIdType::UA_NODEIDTYPE_STRING;
                id.identifier.string = UA_STRING((char *)std::get<std::string>(i).c_str());
                break;
            default:
                break;
        }
        return id;
    }

    const std::variant<int, std::string>& getIdentifier() const 
    {
        return i;
    }

  private:
    int nsIdx;
    IdentifierType type;
    std::variant<int, std::string> i;
};