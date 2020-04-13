#pragma once
#include "NodeId.h"
#include <nodesetLoader/nodesetLoader.h>
#include <ostream>
#include <string>
#include <vector>

namespace gen
{

struct DataTypeDefinitionField
{
    std::string name{};
    opc::NodeId dataType;
    int valueRank = -1;
    int value = 0;

    friend std::ostream &operator<<(std::ostream &os,
                                    const DataTypeDefinitionField &field)
    {
        os << "name=" << field.name << ";id=" << field.dataType
           << ";valueRank=" << field.valueRank;
        return os;
    }
};

struct DataType
{
    DataType() {}
    DataType(const std::string &name, const opc::NodeId &id, bool isEnum)
        : name{name}, id{id}, isEnum{isEnum}
    {
    }
    const std::string name{};
    const opc::NodeId id;
    std::vector<DataTypeDefinitionField> fields{};
    bool isEnum = false;
    friend std::ostream &operator<<(std::ostream &os, const DataType &type)
    {
        os << "name=" << type.name << ";id=" << type.id << "\n";
        for (const auto &f : type.fields)
        {
            os << "\t" << f << "\n";
        }
        return os;
    }
};
} // namespace gen