#include "FileService.h"
#include "Server.h"
#include <functional>

int main()
{
    opc::Server s;
    FileService fs1;
    FileService fs2;

    s.loadNodeset("../models/serviceobject.xml");

    s.bindMethodNode(opc::NodeId(2,7001), &FileService::browse);
    s.bindMethodNode(opc::NodeId(2, 7002), &FileService::getBrowsedPaths);

    s.addObject(opc::NodeId(0,85), opc::NodeId(2, "stringId"), opc::NodeId(2,1002), "MyServiceObject", &fs1);
    s.addObject(opc::NodeId(0, 85), opc::NodeId(2, "stringId2"),
                opc::NodeId(2, 1002), "Fileservice2", &fs2);
    s.run();
}