#pragma once
#include <ostream>
#include <types/DataType.h>

namespace gen
{

struct StructureDefinitionField : public DataTypeDefinitionField
{
    StructureDefinitionField(const std::string &name, int valueRank,
                             const modernopc::NodeId &id)
        : DataTypeDefinitionField(name, valueRank, id)
    {
    }
    virtual std::ostream &operator<<(std::ostream &os) const override
    {
        os << "name=" << name << ";id=" << dataType
           << ";valueRank=" << valueRank;
        return os;
    }
};

struct EnumDefinitionField : public DataTypeDefinitionField
{
    EnumDefinitionField(const std::string &name, int valueRank,
                        const modernopc::NodeId &id, int value)
        : DataTypeDefinitionField(name, valueRank, id), value{value}
    {
    }
    int value{0};
    std::ostream &operator<<(std::ostream &os) const override
    {
        os << "name=" << name << ";id=" << dataType << "value=" << value;
        return os;
    }
};
class EnumDataType : public DataType
{
  public:
    EnumDataType(const std::string &name, const modernopc::NodeId &id)
        : DataType(name, id)
    {
    }
    void generate(TypeGenerator &g) const override;
    void getScalarConversion(std::ostream &os,
                             const std::string &name) const override;
    void getArrayConversion(std::ostream &os, const std::string &name) const;
    std::vector<EnumDefinitionField> fields{};
    bool buildOrder(TypeGenerator &g) const override;
    std::ostream &operator<<(std::ostream &os) const override
    {
        os << "name=" << name << ";id=" << id;
        return os;
    }
};

class StructureDataType : public DataType
{
  public:
    StructureDataType(const std::string &name, const modernopc::NodeId &id)
        : DataType(name, id)
    {
    }
    void generate(TypeGenerator &g) const override;
    void getScalarConversion(std::ostream &os,
                             const std::string &name) const override;
    void getArrayConversion(std::ostream &os, const std::string &name) const;
    std::vector<StructureDefinitionField> fields{};
    bool buildOrder(TypeGenerator &g) const override;
    std::ostream &operator<<(std::ostream &os) const override
    {
        os << "name=" << name << ";id=" << id;
        return os;
    }
};

class BaseDataType : public DataType
{
  public:
    BaseDataType(const std::string &name, const modernopc::NodeId &id)
        : DataType(name, id)
    {
    }
    void generate(TypeGenerator &g) const override;
    void getScalarConversion(std::ostream &os,
                             const std::string &name) const override;
    void getArrayConversion(std::ostream &os, const std::string &name) const;
    std::vector<StructureDefinitionField> fields{};
    bool buildOrder(TypeGenerator &g) const override;
    std::ostream &operator<<(std::ostream &os) const override
    {
        os << "name=" << name << ";id=" << id;
        return os;
    }
};
} // namespace gen
