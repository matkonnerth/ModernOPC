#include "DataType.h"
#include "TypeGenerator.h"
#include "backend.h"
#include <iostream>
#include <nodesetLoader/nodesetLoader.h>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <unordered_map>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("specify nodesetfile as argument. E.g. parserDemo text.xml\n");
        return 1;
    }

    std::unordered_map<opc::NodeId, gen::DataType> types{};

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

    //std::ostringstream header, impl;
    std::ofstream header("/home/matzy/git/modernOPC/generated/generated.h",
                         std::ios::out);
    std::ofstream impl("/home/matzy/git/modernOPC/generated/generated.cpp",
                       std::ios::out);
    gen::TypeGenerator g{"opc", types, header, impl};
    g.generateClass(types[opc::NodeId(0, 312)]);

    //std::cout << "header: \n" << header.str() << "\n";
    //std::cout << "impl\n" << impl.str() << "\n";
    //std::cout << impl.str() << "\n";
    header.close();
    impl.close();
    

    return 0;
}
