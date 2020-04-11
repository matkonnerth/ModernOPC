#include <gtest/gtest.h>
#include <opc/Server.h>
#include <opc/nodes/Node.h>
#include <opc/nodes/ObjectNode.h>

std::string path = "";

TEST(import, values)
{
    opc::Server server;
    ASSERT_TRUE(server.loadNodeset(path + "/" + "namespaceZeroValues.xml"));
}

TEST(import, nodeclass)
{
    opc::Server server;
    ASSERT_TRUE(server.loadNodeset(path + "/" + "basicNodeClassTest.xml"));
}

TEST(import, notFound)
{
    opc::Server server;
    ASSERT_FALSE(server.loadNodeset("someNotExisting.xml"));
}

TEST(import, displayname)
{
    opc::Server server;
    ASSERT_TRUE(server.loadNodeset(path + "/" + "Displayname.xml"));
    ASSERT_TRUE(server.getObject(opc::NodeId(2, 6567))->displayName().text() ==
                "<GroupIdentifier>");
}

int main(int argc, char **argv)
{

    testing::InitGoogleTest(&argc, argv);

    if (!(argc > 1))
        return 1;
    path = argv[1];

    return RUN_ALL_TESTS();
}