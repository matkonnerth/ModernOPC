#include "backend.h"
#include <TypeGenerator.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <nodesetLoader/nodesetLoader.h>
#include <stdio.h>
#include <string.h>
#include <types/DataType.h>
#include <unordered_map>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("specify nodesetfile as argument. E.g. parserDemo text.xml\n");
        return 1;
    }

    std::unordered_map<opc::NodeId, std::unique_ptr<gen::DataType>> types{};



    /*{
        {NodeId(0, 1), "bool"},           {NodeId(0, 2), "char"},
        {NodeId(0, 3), "unsigned char"},  {NodeId(0, 4), "int16_t"},
        {NodeId(0, 5), "uint16_t"},       {NodeId(0, 6), "int32_t"},
        {NodeId(0, 7), "uint32_t"},       {NodeId(0, 8), "int64_t"},
        {NodeId(0, 9), "uint64_t"},       {NodeId(0, 10), "float"},
        {NodeId(0, 11), "double"},        {NodeId(0, 12), "std::string"},
        {NodeId(0, 13), "uint64_t"},      {NodeId(0, 17), "NodeId"},
        {NodeId(0, 20), "QualifiedName"}, {NodeId(0, 21), "LocalizedText"}};*/
    types.insert(std::make_pair(
        opc::NodeId(0, 1),
        std::make_unique<gen::BaseDataType>("bool", opc::NodeId(0, 1))));
    types.insert(std::make_pair(
        opc::NodeId(0, 2),
        std::make_unique<gen::BaseDataType>("char", opc::NodeId(0, 2))));
    types.insert(std::make_pair(
        opc::NodeId(0, 3), std::make_unique<gen::BaseDataType>(
                               "unsigned char", opc::NodeId(0, 3))));
    types.insert(std::make_pair(
        opc::NodeId(0, 4),
        std::make_unique<gen::BaseDataType>("int16_t", opc::NodeId(0, 4))));
    types.insert(std::make_pair(
        opc::NodeId(0, 5),
        std::make_unique<gen::BaseDataType>("uint16_t", opc::NodeId(0, 5))));
    types.insert(std::make_pair(
        opc::NodeId(0, 6),
        std::make_unique<gen::BaseDataType>("int32_t", opc::NodeId(0, 6))));
    types.insert(std::make_pair(
        opc::NodeId(0, 7),
        std::make_unique<gen::BaseDataType>("uint32_t", opc::NodeId(0, 7))));
    types.insert(std::make_pair(
        opc::NodeId(0, 8),
        std::make_unique<gen::BaseDataType>("int64_t", opc::NodeId(0, 8))));
    types.insert(std::make_pair(
        opc::NodeId(0, 9),
        std::make_unique<gen::BaseDataType>("uint64_t", opc::NodeId(0, 9))));
    types.insert(std::make_pair(
        opc::NodeId(0, 10),
        std::make_unique<gen::BaseDataType>("float", opc::NodeId(0, 10))));
    types.insert(std::make_pair(
        opc::NodeId(0, 11),
        std::make_unique<gen::BaseDataType>("double", opc::NodeId(0, 11))));
    types.insert(std::make_pair(
        opc::NodeId(0, 12),
        std::make_unique<gen::BaseDataType>("std::string", opc::NodeId(0, 12))));
    types.insert(std::make_pair(
        opc::NodeId(0, 13), std::make_unique<gen::BaseDataType>(
                                "uint64_t", opc::NodeId(0, 13))));
    types.insert(std::make_pair(
        opc::NodeId(0, 17),
        std::make_unique<gen::BaseDataType>("NodeId", opc::NodeId(0, 17))));
    types.insert(std::make_pair(
        opc::NodeId(0, 20), std::make_unique<gen::BaseDataType>(
                                "QualifiedName", opc::NodeId(0, 20))));
    types.insert(std::make_pair(
        opc::NodeId(0, 21), std::make_unique<gen::BaseDataType>(
                                "LocalizedText", opc::NodeId(0, 21))));

    FileContext handler;
    handler.callback = addNode;
    handler.addNamespace = addNamespace;
    handler.userContext = &types;
    ValueInterface valIf;
    valIf.userData = NULL;
    valIf.newValue = Value_new;
    valIf.start = Value_start;
    valIf.end = Value_end;
    valIf.finish = Value_finish;
    valIf.deleteValue = Value_delete;
    handler.valueHandling = &valIf;

    for (int cnt = 1; cnt < argc; cnt++)
    {
        handler.file = argv[cnt];
        if (!loadFile(&handler))
        {
            printf("nodeset could not be loaded, exit\n");
            return 1;
        }
    }
    std::cout << "Types count: " << types.size() << std::endl;

    std::ofstream header("/home/matzy/git/modernOPC/generated/generated.h",
                         std::ios::out);
    std::ofstream impl("/home/matzy/git/modernOPC/generated/generated.cpp",
                       std::ios::out);
    gen::TypeGenerator g{"opc", types, header, impl};

    // for(const auto&p:types)
    //{
    g.generateClass(*types[opc::NodeId(0, 308)]);
    //}
    g.finish();
    header.close();
    impl.close();

    return 0;
}
