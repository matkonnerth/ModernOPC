#pragma once
#include "NodeId.h"
#include <cstring>

// todo: handle guid, bytestring
inline modernopc::NodeId extractNodeId(char *s)
{
    if (!s || strlen(s) < 3)
    {
        return modernopc::NodeId(0, 0);
    }
    char *idxSemi = strchr(s, ';');
    // namespaceindex zero?
    if (idxSemi == NULL)
    {
        switch (s[0])
        {
        // integer
        case 'i':
            return modernopc::NodeId(0, atoi(&s[2]));
        case 's':
            return modernopc::NodeId(0, &s[2]);
        }
    }
    else
    {
        switch (idxSemi[1])
        {
        // integer
        case 'i':
        {
            return modernopc::NodeId(atoi(&s[3]), atoi(&idxSemi[3]));
            break;
        }
        case 's':
        {
            return modernopc::NodeId(atoi(&s[3]), &idxSemi[3]);
            break;
        }
        default:
            return modernopc::NodeId(0, 0);
        }
    }
    return modernopc::NodeId(0, 0);
}