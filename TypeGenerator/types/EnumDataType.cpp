#include <TypeGenerator.h>
#include <types/EnumDataType.h>

namespace gen
{

void EnumDataType::generate(TypeGenerator &g) const
{
    g.openHeader(*this);
    g.addHeader(*this);
    g.closeHeader();
    g.afterHeader(*this);
    //
    g.addImpl(*this);
}

void StructureDataType::generate(TypeGenerator &g) const
{
    g.openHeader(*this);
    g.addHeader(*this);
    g.closeHeader();
    g.afterHeader(*this);
    //
    g.addImpl(*this);
}

bool StructureDataType::buildOrder(TypeGenerator &g) const
{
    return g.buildOrder(*this);
}

bool EnumDataType::buildOrder(TypeGenerator &g) const
{
    return g.buildOrder(*this);
}

void EnumDataType::getScalarConversion(std::ostream &os,
                                       const std::string &n) const
{
    os << "static_cast<UA_" << name << ">(val." << n << ");"
       << "\n";
}

void EnumDataType::getArrayConversion(std::ostream &os,
                                      const std::string &n) const
{
    os << "static_cast<UA_" << name << ">(val." << n << "[i]);"
       << "\n";
}

void StructureDataType::getScalarConversion(std::ostream &os,
                                            const std::string &n) const
{
    if (id.getIdType() == NodeId::IdentifierType::NUMERIC &&
        std::get<int>(id.getIdentifier()) <= 11)
    {
        os << " static_cast<UA_" << name << ">(val." << n << ");\n";
        return;
    }
    else
    {
        os << "from" << name << "("
           << "val." << n << ");\n";
    }
}

void StructureDataType::getArrayConversion(std::ostream &os,
                                           const std::string &n) const
{
    os << "from" << name << "("
       << "val." << n << "[i]);\n";
}

} // namespace gen