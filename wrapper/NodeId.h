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
        identifier(): numeric(0){}
        identifier(int id): numeric(id){}
        identifier(const std::string& id): stringIdentifier(id){}
        identifier(const char* id): stringIdentifier(id){}
        const std::string& stringIdentifier;
        int numeric;
    };

    public:
        NodeId(int nsIdx, int id):nsIdx(nsIdx), type(NodeId::IdentifierType::NUMERIC), i(id){}
        NodeId(int nsIdx, const std::string& id):nsIdx(nsIdx), type(NodeId::IdentifierType::STRING), i(id){}
        NodeId(UA_NodeId id):i("test")
        {
            
            nsIdx = id.namespaceIndex;
            switch (id.identifierType)
            {
            case UA_NODEIDTYPE_NUMERIC:
                type = IdentifierType::NUMERIC;
                i.numeric = id.identifier.numeric;
                break;
            case UA_NODEIDTYPE_STRING:
                type = IdentifierType::STRING;
                //i.stringIdentifier = "test";
                break;
            }
        }

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
        int nsIdx;
        IdentifierType type;
        NodeId::identifier i;
};