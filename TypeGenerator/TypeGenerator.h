#pragma once
#include <ConversionFrom.h>
#include <NodeId.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <ostream>
#include <types/DataType.h>
#include <unordered_map>
#include <unordered_set>

namespace gen
{
using modernopc::NodeId;
class TypeGenerator
{
  public:
    TypeGenerator(
        std::string cppTargetNamespace,
        const std::unordered_map<modernopc::NodeId, std::unique_ptr<gen::DataType>>
            &types,
        std::ostream &hdr, std::ostream &impl)
        : cppTargetNamespace{cppTargetNamespace}, types{types}, header{hdr},
          impl{impl}
    {
        openNamespace();
    }

    void addConversionDecl(const StructureDataType &type)
    {
        header << "friend void toUAVariantImpl(const " << type.name
               << "& val, UA_Variant "
                  "*var);\n\n";

        header << "friend const UA_" << type.name << " from" << type.name
               << "(const " << type.name << "& val);"
               << "\n";
    }

    void addConversionDecl(const EnumDataType &type)
    {
        header << "void toUAVariantImpl(const " << type.name
               << "& val, UA_Variant "
                  "*var);\n\n";
        header << "const UA_" << type.name << " from" << type.name << "(const "
               << type.name << "& val);"
               << "\n";
    }

    void addFromConversionFunction(const gen::EnumDefinitionField &field)
    {
        // get type name
        auto t = types.find(field.dataType);

        if (t == types.end())
        {
            std::cout << "could not find type for " << field.name
                      << field.dataType << std::endl;
            return;
        }

        if (field.valueRank > 0)
        {
            impl << "uaval." << firstToLow(field.name) << "Size = val."
                 << firstToLow(field.name) << ".size();"
                 << "\n";
            impl << "uaval." << firstToLow(field.name) << " = static_cast<UA_"
                 << t->second->name << "*>(UA_Array_new(val."
                 << firstToLow(field.name) << ".size(), &UA_TYPES[UA_TYPES_"
                 << str_toupper(t->second->name) << "]));"
                 << "\n";
            impl << "for(auto i=0; i<"
                 << "val." << firstToLow(field.name) << ".size(); i++){";
            impl << "uaval." << firstToLow(field.name) << "[i] = ";

            t->second->getArrayConversion(impl, firstToLow(field.name));
            impl << "}"
                 << "\n";
        }
        else
        {

            t->second->getScalarConversion(impl, firstToLow(field.name));
        }
    }

    void addFromConversionFunction(const gen::StructureDefinitionField &field)
    {
        // get type name
        auto t = types.find(field.dataType);

        if (field.valueRank > 0)
        {
            impl << "uaval." << firstToLow(field.name) << "Size = val."
                 << firstToLow(field.name) << ".size();"
                 << "\n";
            impl << "uaval." << firstToLow(field.name) << " = static_cast<UA_"
                 << t->second->name << "*>(UA_Array_new(val."
                 << firstToLow(field.name) << ".size(), &UA_TYPES[UA_TYPES_"
                 << str_toupper(t->second->name) << "]));"
                 << "\n";
            impl << "for(auto i=0; i<"
                 << "val." << firstToLow(field.name) << ".size(); i++){";
            impl << "uaval." << firstToLow(field.name) << "[i] = ";

            t->second->getArrayConversion(impl, firstToLow(field.name));
            impl << "}"
                 << "\n";
        }
        else
        {
            impl << "uaval." << firstToLow(field.name) << " = ";
            t->second->getScalarConversion(impl, firstToLow(field.name));
        }
    }

    void addFromConversionImpl(const EnumDataType &type)
    {
        // friend const UA_ApplicationInstanceCertificate
        // fromApplicationInstanceCertificate(
        //    const ApplicationInstanceCertificate &val);
        impl << "const UA_" << type.name << " from" << type.name << "(const "
             << type.name << " &val) {"
             << "\n";
        impl << "UA_" << type.name << " uaval;\n";
        for (const auto &field : type.fields)
        {
            addFromConversionFunction(field);
        }
        std::string nameToUpper = str_toupper(type.name);
        // impl << "UA_Variant_setScalarCopy(var, &uaval, &UA_TYPES[UA_TYPES_"
        //     << nameToUpper << "]);\n";
        impl << "return uaval;"
             << "\n";
        impl << "}\n\n";
    }

    void addFromConversionImpl(const StructureDataType &type)
    {
        // friend const UA_ApplicationInstanceCertificate
        // fromApplicationInstanceCertificate(
        //    const ApplicationInstanceCertificate &val);
        impl << "const UA_" << type.name << " from" << type.name << "(const "
             << type.name << " &val) {"
             << "\n";
        impl << "UA_" << type.name << " uaval;\n";
        for (const auto &field : type.fields)
        {
            addFromConversionFunction(field);
        }
        std::string nameToUpper = str_toupper(type.name);
        // impl << "UA_Variant_setScalarCopy(var, &uaval, &UA_TYPES[UA_TYPES_"
        //     << nameToUpper << "]);\n";
        impl << "return uaval;"
             << "\n";
        impl << "}\n\n";
    }

    void addToUAVariantImpl(const StructureDataType &type)
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

    void addToUAVariantImpl(const EnumDataType &type)
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

    void addPrivateMember(const StructureDataType &type)
    {
        header << "public:\n";
        for (const auto &field : type.fields)
        {

            auto t2 = types.find(field.dataType);
            if (t2 != types.end())
            {
                if (field.valueRank > -1)
                {
                    header << "std::vector<" << t2->second->name << ">"
                           << " " << firstToLow(field.name) << ";\n";
                }
                else
                {
                    header << t2->second->name << " " << firstToLow(field.name)
                           << ";\n";
                }
            }
            else
            {
                header << "unknown type"
                       << "\n";
            }
        }
    }

    void addEnumOrdinals(const EnumDataType &type)
    {
        // header << "private:\n";
        for (const auto &field : type.fields)
        {
            header << str_toupper(type.name) << "_" << str_toupper(field.name)
                   << " = " << field.value << ","
                   << "\n";
        }
    }

    bool buildOrder(const EnumDataType &type)
    {
        order.push_back(type.id);
        return true;
    }

    bool buildOrder(const StructureDataType &type)
    {
        for (const auto &field : type.fields)
        {

            auto t2 = types.find(field.dataType);
            if (t2 != types.end())
            {
                if (!t2->second->buildOrder(*this))
                {
                    return false;
                }
            }
            else
            {
                std::cout << "unknown type -> exit";
                return false;
            }
        }
        order.push_back(type.id);
        return true;
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

    void addDataTypeTemplate(const DataType &type)
    {
        header << "template<>"
               << "\n";
        header << "inline const UA_DataType* getDataType<" << type.name
               << ">() {"
               << "\n";
        header << "return &UA_TYPES[UA_TYPES_" << str_toupper(type.name) << "];"
               << "\n";
        header << "}"
               << "\n";
    }

    void addImpl(const StructureDataType &type)
    {
        addToUAVariantImpl(type);
        addFromConversionImpl(type);
    }

    void addImpl(const EnumDataType &type)
    {
        addToUAVariantImpl(type);
        addFromConversionImpl(type);
    }

    void addHeader(const StructureDataType &type)
    {
        addConversionDecl(type);
        addPrivateMember(type);
    }

    void afterHeader(const StructureDataType &type)
    {
        addDataTypeTemplate(type);
    }

    void afterHeader(const EnumDataType &type) { addConversionDecl(type); }

    void addHeader(const EnumDataType &type)
    {
        // addEnumConversionHeader(t->second);
        addEnumOrdinals(type);
    }

    void closeHeader() { header << "};\n\n"; }

    void generateClass(const DataType &type)
    {

        order.clear();
        if (!type.buildOrder(*this))
        {
            std::cout << "could build ordering for data " << type.name << " "
                      << type.id << "\n";
            return;
        }
        addIncludes();

        for (const auto &id : order)
        {
            if (generated.find(id) != generated.end())
            {
                continue;
            }
            auto t = types.find(id);
            t->second->generate(*this);
            generated.emplace(id);
        }
    }

    void finish() { closeNamespace(); }

    void openHeader(const StructureDataType &type)
    {
        header << "class " << type.name << "\n";
        header << "{\npublic:\n";
    }

    void openHeader(const EnumDataType &type)
    {
        header << "enum class " << type.name << "\n";
        header << "{\n";
    }

  private:
    std::string cppTargetNamespace{};

    const std::unordered_map<modernopc::NodeId, std::unique_ptr<gen::DataType>>
        &types{};

    std::vector<NodeId> order{};
    std::unordered_set<NodeId> generated{};

    std::ostream &header;
    std::ostream &impl;
};

}; // namespace gen