#include <opc/Server.h>
#include <functional>
#include <opc/nodes/MethodNode.h>

int main()
{
    opc::Server s;

    // loading of a xml nodeset
    s.loadNodeset("../models/serviceobject.xml");
    std::function load = [&](std::string path) { s.loadNodeset(path); };

    s.getMethod(opc::NodeId(2, 7003))->bindCallable(load);

    s.run();
}