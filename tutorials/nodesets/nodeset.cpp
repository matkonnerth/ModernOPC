/**
 * Importing a nodeset.xml
 * -----------------------
 * By providing a path a valid nodeset xml can be loaded into the addresspace of the server. */

#include <functional>
#include <modernOpc/Server.h>
#include <modernOpc/nodes/MethodNode.h>

int main()
{
    modernopc::Server s;


    if(!s.loadNodeset("../models/serviceobject.xml"))
    {
        return -1;
    }
    std::function load = [&](std::string path) { s.loadNodeset(path); };

    //s.getMethod(modernopc::NodeId(2, 7003))->bindCallable(load);

    s.run();
}

/**
 * Used methods
 *
 * .. doxygenfunction:: modernopc::Server::loadNodeset
 * .. doxygenfunction:: modernopc::Server::getMethod */