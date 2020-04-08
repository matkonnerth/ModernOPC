#include "FileService.h"
#include <functional>
#include <opc/Server.h>
#include <opc/nodes/MethodNode.h>
#include <opc/nodes/ObjectNode.h>

std::vector<std::string> freeBrowse(const std::string &path)
{
    std::vector<std::string> paths{"2", "1", "3"};
    return paths;
}

int main()
{
    opc::Server s;
    FileService fs1;
    FileService fs2;

    s.loadNodeset("../models/serviceobject.xml");

    // bind member functions

    s.getMethod(opc::NodeId(2, 7001))->bindCallable(&FileService::browse);
    s.getMethod(opc::NodeId(2, 7002))
        ->bindCallable(&FileService::getBrowsedPaths);
    // bind free function
    s.getMethod(opc::NodeId(2, 7005))->bindCallable(&freeBrowse);

    s.getObjectsFolder()->addObject(opc::NodeId(2, "stringId"),
                                    opc::QualifiedName(1, "MyServiceObject"),
                                    &fs1, opc::NodeId(2, 1002));
    s.getObjectsFolder()->addObject(opc::NodeId(2, "stringId2"),
                                    opc::QualifiedName(1, "Fileservice2"), &fs2,
                                    opc::NodeId(2, 1002));
    s.run();
}