#include <types/EnumDataType.h>

namespace gen
{

void BaseDataType::generate(TypeGenerator &g) const
{

}

bool BaseDataType::buildOrder(TypeGenerator &g) const
{
    return true;
}

void BaseDataType::getScalarConversion(std::ostream &os,
                                       const std::string &n) const
{
    os << "static_cast<UA_" << name << ">(val." << n << ");"
       << "\n";
}

void BaseDataType::getArrayConversion(std::ostream &os,
                                      const std::string &n) const
{
    os << "static_cast<UA_" << name << ">(val." << n << "[i]);"
       << "\n";
}

}