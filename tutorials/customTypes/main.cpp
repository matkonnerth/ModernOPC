#include <opc/Server.h>
#include "AppTypes.h"
#include <opc/nodes/ObjectNode.h>
#include <opc/nodes/VariableNode.h>
#include <iostream>

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
    s.run();
}