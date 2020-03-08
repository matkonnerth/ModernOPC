#include <gtest/gtest.h>
#include <Server.h>

TEST(import, values)
{
    opc::Server server;
    server.loadNodeset("../tests/import/namespaceZeroValues.xml");
}

TEST(import, nodeclass) {
    opc::Server server;
    server.loadNodeset("../tests/import/basicNodeClassTest.xml");
}

TEST(import, speed) {
    opc::Server server;
    server.loadNodeset("../tests/import/100kNodes.xml");
}