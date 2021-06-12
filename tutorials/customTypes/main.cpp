#include "AppTypes.h"
#include <iostream>
#include <modernopc/Server.h>
#include <modernopc/nodes/ObjectNode.h>
#include <modernopc/nodes/VariableNode.h>
//#include <generated.h>

using modernopc::NodeId;
using modernopc::QualifiedName;

int main()
{
    modernopc::Server s;

    app::types::Range2 r{0.1, 9.9};
    auto varNode = s.getObjectsFolder()->addBaseDataTypeVariable(NodeId(1, "range"), QualifiedName(1, "range"), r);

    modernopc::Variant var;
    varNode->read(var);

    auto r2 = var.get<app::types::Range2>();

    std::cout << r2.min << " " << r2.max << std::endl;

    auto obj = s.getObjectsFolder();
    //modernopc::BuildInfo info;
    //info.productUri = "matkonnerth - modernopc";
    //obj->addVariable(NodeId(1, "myBuildInfo"), NodeId(0, 0),
    //                 QualifiedName(1, "test"), info);
    s.run();
}