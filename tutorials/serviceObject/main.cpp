#include "FileService.h"
#include <opc/Server.h>
#include <functional>

std::vector<std::string> freeBrowse(const std::string& path)
{
    std::vector<std::string> paths {"2", "1", "3"};
    return paths;
}

int main()
{
    opc::Server s;
    FileService fs1;
    FileService fs2;

    s.loadNodeset("../models/serviceobject.xml");

    //bind member functions
    s.bindMethodNode(opc::NodeId(2,7001), &FileService::browse);
    s.bindMethodNode(opc::NodeId(2, 7002), &FileService::getBrowsedPaths);
    //bind free function
    s.bindMethodNode(opc::NodeId(2, 7005), &freeBrowse);

    s.addObject(opc::NodeId(0,85), opc::NodeId(2, "stringId"), opc::NodeId(2,1002), "MyServiceObject", &fs1);
    s.addObject(opc::NodeId(0, 85), opc::NodeId(2, "stringId2"),
                opc::NodeId(2, 1002), "Fileservice2", &fs2);
    s.run();
}