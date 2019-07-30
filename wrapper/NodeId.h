#pragma once
#include <iostream>
#include <open62541/server.h>

class NodeId
{
    enum class IdentifierType
    {
        NUMERIC,
        STRING
    };

    union identifier
    {
        identifier(int id): numeric(id){}
        identifier(const std::string& id): stringIdentifier(id){}
        const std::string& stringIdentifier;
        int numeric;
    };

    public:
        NodeId(int nsIdx, int id):nsIdx(nsIdx), type(NodeId::IdentifierType::NUMERIC), i(id){}
        NodeId(int nsIdx, const std::string& id):nsIdx(nsIdx), type(NodeId::IdentifierType::STRING), i(id){}
        inline UA_NodeId toUA_NodeId() const
        {
            UA_NodeId id;
            UA_NodeId_init(&id);
            id.namespaceIndex = nsIdx;
            switch(type)
            {
                case IdentifierType::NUMERIC:
                    id.identifierType = UA_NodeIdType::UA_NODEIDTYPE_NUMERIC;
                    id.identifier.numeric = i.numeric;
                    break;
                case IdentifierType::STRING:
                    id.identifierType = UA_NodeIdType::UA_NODEIDTYPE_STRING;
                    id.identifier.string = UA_STRING((char*)i.stringIdentifier.c_str());
                    break;
                default:
                    break;
            }
            return id;
        }
    private:
        const int nsIdx;
        const IdentifierType type;
        NodeId::identifier i;
};