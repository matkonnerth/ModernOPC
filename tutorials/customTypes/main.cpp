#include "AppTypes.h"
#include <iostream>
#include <modernOpc/Server.h>
#include <modernOpc/nodes/ObjectNode.h>
#include <modernOpc/nodes/VariableNode.h>
//#include <generated.h>

using opc::NodeId;
using opc::QualifiedName;

int main()
{
    opc::Server s;

    app::types::Range2 r{0.1, 9.9};
    auto varNode = s.getObjectsFolder()->addBaseDataTypeVariable(NodeId(1, "range"), QualifiedName(1, "range"), r);

    opc::Variant var;
    varNode->read(var);

    auto r2 = var.get<app::types::Range2>();

    std::cout << r2.min << " " << r2.max << std::endl;

    auto obj = s.getObjectsFolder();
    //opc::BuildInfo info;
    //info.productUri = "matkonnerth - modernOPC";
    //obj->addVariable(NodeId(1, "myBuildInfo"), NodeId(0, 0),
    //                 QualifiedName(1, "test"), info);
    s.run();
}