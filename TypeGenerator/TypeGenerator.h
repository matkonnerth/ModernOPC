#include "DataType.h"
#include "NodeId.h"
#include <algorithm>
#include <iostream>
#include <ostream>
#include <unordered_map>

namespace gen
{
using opc::NodeId;
class TypeGenerator
{
  public:
    TypeGenerator(std::string cppTargetNamespace,
                  const std::unordered_map<opc::NodeId, gen::DataType> &types,
                  std::ostream &hdr, std::ostream &impl)
        : cppTargetNamespace{cppTargetNamespace}, types{types}, header{hdr},
          impl{impl}
    {
    }

    void addConversionHeader(const DataType &type)
    {
        header << "friend void toUAVariantImpl(const " << type.name
               << "& val, UA_Variant "
                  "*var);\n\n";
    }

    std::string str_toupper(std::string s)
    {
        std::transform(
            s.begin(), s.end(), s.begin(),
            // static_cast<int(*)(int)>(std::toupper)         // wrong
            // [](int c){ return std::toupper(c); }           // wrong
            // [](char c){ return std::toupper(c); }          // wrong
            [](unsigned char c) { return std::toupper(c); } // correct
        );
        return s;
    }

    void addFromConversionFunction(const NodeId &type,
                                   const std::string fieldName)
    {
        impl << "uaval." << fieldName << "=";
        if (type.getIdType() == NodeId::IdentifierType::NUMERIC &&
            std::get<int>(type.getIdentifier()) <= 11)
        {
            impl << "val." << fieldName << ";\n";
            return;
        }
        // get type name
        auto t = types.find(type);
        impl << "from" << t->second.name << "("
             << "val." << fieldName << ");\n";
    }

    void addConversionImpl(const DataType &type)
    {
        impl << "void " << type.name << "::toUAVariantImpl(const " << type.name
             << "& val, UA_Variant "
                "*var)\n{\n";

        impl << "UA_" << type.name << " uaval;\n";
        for (const auto &field : type.fields)
        {
            addFromConversionFunction(field.dataType, field.name);
        }
        std::string nameToUpper = str_toupper(type.name);
        impl << "UA_Variant_SetScalarCopy(var, &uavar, &UA_TYPES[UA_TYPES_"
             << nameToUpper << "]);\n";
        impl << "}\n\n";
    }

    void addEnumConversionImpl(const DataType &type)
    {
        impl << "void " << type.name << "::toUAVariantImpl(const " << type.name
             << "& val, UA_Variant "
                "*var)\n{\n";

        impl << "UA_Int32" << type.name
             << " uaval = static_cast<UA_Int32>(val);\n";
        std::string nameToUpper = str_toupper(type.name);
        impl << "UA_Variant_SetScalarCopy(var, &uavar, &UA_TYPES[UA_INT32]);\n";
        impl << "}\n\n";
    }

    void addPrivateMember(const DataType &type)
    {
        header << "private:\n";
        for (const auto &field : type.fields)
        {
            auto t = baseDataTypes.find(field.dataType);
            if (t != baseDataTypes.end())
            {
                header << t->second << " " << field.name << ";\n";
            }
            else
            {
                auto t2 = types.find(field.dataType);
                if (t2 != types.end())
                {
                    header << t2->second.name << " " << field.name << ";\n";
                }
                else
                {
                    header << "unknown type"
                           << "\n";
                }
            }
        }
    }

    void addEnumOrdinals(const DataType &type)
    {
        // header << "private:\n";
        for (const auto &field : type.fields)
        {
            header << str_toupper(type.name) << "_" << str_toupper(field.name)
                   << " = " << field.value << ","
                   << "\n";
        }
    }

    void buildOrder(const DataType &type)
    {
        if (type.isEnum)
        {
            order.push_back(type.id);
            return;
        }
        for (const auto &field : type.fields)
        {
            auto t = baseDataTypes.find(field.dataType);
            if (t == baseDataTypes.end())
            {
                auto t2 = types.find(field.dataType);
                if (t2 != types.end())
                {
                    buildOrder(t2->second);
                }
                else
                {
                    std::cout << "unknown type -> exit";
                    std::exit(2);
                }
            }
        }
        order.push_back(type.id);
    }

    void openNamespace()
    {
        header << "namespace " << cppTargetNamespace << " {\n";
        impl << "namespace " << cppTargetNamespace << " {";
    }

    void closeNamespace()
    {
        header << "}\n";
        impl << "}";
    }

    void newLine(std::ostream &os, const std::string &s) { os << s << "\n"; }

    void addIncludes()
    {
        newLine(header, "#pragma once");
        newLine(header, "#include <string>");
        newLine(header, "#include <opc/types/LocalizedText.h>");
        //
        newLine(impl, "#include <generated.h>");
        newLine(impl, "#include <opc/Variant.h>");
        newLine(impl, "#include <opc/types/StdTypes.h>");
        newLine(impl, "#include <opc/types/NodeId.h>");
        newLine(impl, "#include <opc/types/QualifiedName.h>");
    }

    void generateClass(const DataType &type)
    {
        buildOrder(type);
        addIncludes();
        openNamespace();
        for (const auto &id : order)
        {
            auto t = types.find(id);
            if (!t->second.isEnum)
            {
                addHeader(t->second);
                addConversionHeader(t->second);
                addConversionImpl(t->second);
                addPrivateMember(t->second);
                header << "};\n\n";
            }
            else
            {
                addEnumHeader(t->second);
                addConversionHeader(t->second);
                addEnumConversionImpl(t->second);
                addEnumOrdinals(t->second);
                header << "};\n\n";
            }
        }
        closeNamespace();
    }

    void addHeader(const DataType &type)
    {
        header << "class " << type.name << "\n";
        header << "{\n";
    }

    void addEnumHeader(const DataType &type)
    {
        header << "enum class " << type.name << "\n";
        header << "{\n";
    }

  private:
    std::string cppTargetNamespace{};
    const std::unordered_map<opc::NodeId, std::string> baseDataTypes{
        {NodeId(0, 1), "bool"},           {NodeId(0, 2), "char"},
        {NodeId(0, 3), "std::byte"},      {NodeId(0, 4), "int16_t"},
        {NodeId(0, 5), "uint16_t"},       {NodeId(0, 6), "int32_t"},
        {NodeId(0, 7), "uint32_t"},       {NodeId(0, 8), "int64_t"},
        {NodeId(0, 9), "uint64_t"},       {NodeId(0, 10), "float"},
        {NodeId(0, 11), "double"},        {NodeId(0, 12), "std::string"},
        {NodeId(0, 13), "uint64_t"},      {NodeId(0, 17), "NodeId"},
        {NodeId(0, 20), "QualifiedName"}, {NodeId(0, 21), "LocalizedText"}};

    const std::unordered_map<opc::NodeId, gen::DataType> &types{};

    std::vector<NodeId> order{};

    std::ostream &header;
    std::ostream &impl;
};

}; // namespace gen