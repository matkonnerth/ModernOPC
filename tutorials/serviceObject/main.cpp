#include "FileService.h"
#include <functional>
#include <modernOpc/Server.h>
#include <modernOpc/nodes/MethodNode.h>
#include <modernOpc/nodes/ObjectNode.h>

int main()
{
    modernopc::Server s;
    FileService fs1;
    FileService fs2;

    s.loadNodeset("../models/serviceobject.xml");

    //bind the Methods to the member functions of the FileService Type
    s.getMethod(modernopc::NodeId(2, 7001))->bindCallable(&FileService::browse);
    s.getMethod(modernopc::NodeId(2, 7002))
        ->bindCallable(&FileService::getBrowsedPaths);

    //instantiate 2 FileService objects and set the context to the c++ objects
    s.getObjectsFolder()->addObject(modernopc::NodeId(2, "stringId"),
                                    modernopc::QualifiedName(1, "MyServiceObject"),
                                    &fs1, modernopc::NodeId(2, 1002));
    s.getObjectsFolder()->addObject(modernopc::NodeId(2, "stringId2"),
                                    modernopc::QualifiedName(1, "Fileservice2"), &fs2,
                                    modernopc::NodeId(2, 1002));
    s.run();
}