#include "Server.h"
#include "AppTypes.h"

int main()
{
    opc::Server s;

    app::types::Range2 r{0.1, 9.9};
    s.addVariableNode(opc::NodeId(0, 85), opc::NodeId(1, "range"), "range", r);

    opc::Variant var;
    s.readValue(opc::NodeId(1, "range"), var);

    auto r2 = var.get<app::types::Range2>();

    std::cout << r2.min << " " << r2.max << std::endl;
    s.run();
}