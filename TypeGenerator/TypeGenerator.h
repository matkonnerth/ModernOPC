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

        // friend const UA_QualifiedName fromQualifiedName(
        //    const QualifiedName &qn);

        header << "friend const UA_" << type.name << " from" << type.name
               << "(const " << type.name << "& val);"
               << "\n";
    }

    void addEnumConversionHeader(const DataType &type)
    {
        header << "void toUAVariantImpl(const " << type.name
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

    std::string firstToLow(std::string s)
    {
        s[0] = std::tolower(s[0]);
        return s;
    }

    void addFromConversionFunction(const gen::DataTypeDefinitionField &field)
    {
        /*
        impl << "uaval." << firstToLow(field.name) << "=";
        if (field.dataType.getIdType() == NodeId::IdentifierType::NUMERIC &&
            std::get<int>(field.dataType.getIdentifier()) <= 11)
        {
            impl << " val." << firstToLow(field.name) << ";\n";
            return;
        }
        */
        // get type name
        auto t = types.find(field.dataType);

        if (field.valueRank > 0)
        {
            impl << "uaval." << firstToLow(field.name) << "Size = val."
                 << firstToLow(field.name) << ".size();"
                 << "\n";
            impl << "uaval." << firstToLow(field.name) << " = static_cast<UA_"
                 << t->second.name << "*>(UA_Array_new(val."
                 << firstToLow(field.name) << ".size(), &UA_TYPES[UA_TYPES_"
                 << str_toupper(t->second.name) << "]));"
                 << "\n";
            impl << "for(auto i=0; i< "
                 << "val." << firstToLow(field.name) << ".size(); i++){";
            impl << "uaval." << firstToLow(field.name) << "[i] = ";
            if (t->second.isEnum)
            {
                impl << "static_cast<UA_" << t->second.name << ">(val."
                     << firstToLow(field.name) << ");"
                     << "\n";
            }
            else
            {
                impl << "from" << t->second.name << "("
                     << "val." << firstToLow(field.name) << "[i]);\n";
            }
            impl << "}"
                 << "\n";
        }
        else
        {
            impl << "uaval." << firstToLow(field.name) << " = ";
            if (t->second.isEnum)
            {
                impl << "static_cast<UA_" << t->second.name << ">(val."
                     << firstToLow(field.name) << ");"
                     << "\n";
            }
            else
            {
                if (field.dataType.getIdType() ==
                        NodeId::IdentifierType::NUMERIC &&
                    std::get<int>(field.dataType.getIdentifier()) <= 11)
                {
                    impl << " static_cast<UA_" << t->second.name << ">(val."
                         << firstToLow(field.name) << ");\n";
                    return;
                }
                else
                {
                    impl << "from" << t->second.name << "("
                         << "val." << firstToLow(field.name) << ");\n";
                }
            }
        }
    }

    void addConversionImpl(const DataType &type)
    {
        impl << "void toUAVariantImpl(const " << type.name
             << "& val, UA_Variant "
                "*var)\n{\n";

        impl << "UA_" << type.name << " uaval;\n";
        for (const auto &field : type.fields)
        {
            addFromConversionFunction(field);
        }
        std::string nameToUpper = str_toupper(type.name);
        impl << "UA_Variant_setScalarCopy(var, &uaval, &UA_TYPES[UA_TYPES_"
             << nameToUpper << "]);\n";
        impl << "}\n\n";
    }

    void addFromConversionImpl(const DataType &type)
    {
        //friend const UA_ApplicationInstanceCertificate
        //fromApplicationInstanceCertificate(
        //    const ApplicationInstanceCertificate &val);
        impl << "const UA_"<<type.name << " from" << type.name << "(const " << type.name << " &val) {" << "\n";
        impl << "UA_" << type.name << " uaval;\n";
        for (const auto &field : type.fields)
        {
            addFromConversionFunction(field);
        }
        std::string nameToUpper = str_toupper(type.name);
        //impl << "UA_Variant_setScalarCopy(var, &uaval, &UA_TYPES[UA_TYPES_"
        //     << nameToUpper << "]);\n";
        impl << "return uaval;" << "\n";
        impl << "}\n\n";
    }

    void addEnumConversionImpl(const DataType &type)
    {
        impl << "void toUAVariantImpl(const " << type.name
             << "& val, UA_Variant "
                "*var)\n{\n";

        impl << "UA_Int32 uaval = static_cast<UA_" << type.name << ">(val);\n";
        std::string nameToUpper = str_toupper(type.name);
        impl << "UA_Variant_setScalarCopy(var, &uaval, "
                "&UA_TYPES[UA_TYPES_INT32]);\n";
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
                if (field.valueRank > -1)
                {
                    header << "std::vector<" << t->second << ">"
                           << " " << firstToLow(field.name) << ";\n";
                }
                else
                {
                    header << t->second << " " << firstToLow(field.name)
                           << ";\n";
                }
            }
            else
            {
                auto t2 = types.find(field.dataType);
                if (t2 != types.end())
                {
                    if (field.valueRank > -1)
                    {
                        header << "std::vector<" << t2->second.name << ">"
                               << " " << firstToLow(field.name) << ";\n";
                    }
                    else
                    {
                        header << t2->second.name << " "
                               << firstToLow(field.name) << ";\n";
                    }
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
                addFromConversionImpl(t->second);
                addPrivateMember(t->second);
                header << "};\n\n";
            }
            else
            {
                addEnumHeader(t->second);
                addEnumOrdinals(t->second);
                header << "};\n\n";
                addEnumConversionHeader(t->second);
                addEnumConversionImpl(t->second);
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
        {NodeId(0, 3), "unsigned char"},  {NodeId(0, 4), "int16_t"},
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