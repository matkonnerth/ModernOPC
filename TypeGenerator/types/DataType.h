#pragma once
#include <NodeId.h>
#include <memory>
#include <nodesetLoader/nodesetLoader.h>
#include <ostream>
#include <string>
#include <vector>

namespace gen
{
class TypeGenerator;
struct DataTypeDefinitionField
{
    DataTypeDefinitionField(const std::string &name, int valueRank,
                            const modernopc::NodeId &id)
        : name{name}, valueRank{valueRank}, dataType{id}
    {
    }
    std::string name{};
    int valueRank{-1};
    modernopc::NodeId dataType{0, 0};
    virtual ~DataTypeDefinitionField() = default;
    virtual std::ostream &operator<<(std::ostream &os) const = 0;
};

struct DataType
{
    DataType() {}
    virtual ~DataType() = default;
    DataType(const std::string &name, const modernopc::NodeId &id)
        : name{name}, id{id}
    {
    }
    const std::string name{};
    const modernopc::NodeId id;
    virtual std::ostream &operator<<(std::ostream &os) const = 0;
    virtual void generate(TypeGenerator &g) const = 0;
    virtual void getScalarConversion(std::ostream &os,
                                     const std::string &name) const = 0;
    virtual void getArrayConversion(std::ostream &os,
                                    const std::string &name) const = 0;
    virtual bool buildOrder(TypeGenerator &g) const = 0;
};
} // namespace gen