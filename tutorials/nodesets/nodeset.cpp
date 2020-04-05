#include <opc/Server.h>
#include <functional>

int main()
{
    opc::Server s;

    // loading of a xml nodeset
    s.loadNodeset("../models/serviceobject.xml");
    std::function load = [&](std::string path) { s.loadNodeset(path); };
    s.bindMethodNode(opc::NodeId(2, 7003), load);
    
    s.run();
}