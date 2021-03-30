#include <gtest/gtest.h>
#include <modernOpc/Server.h>
#include <modernOpc/nodes/Node.h>
#include <modernOpc/nodes/ObjectNode.h>

std::string path = "";

TEST(import, values)
{
    modernopc::Server server;
    ASSERT_TRUE(server.loadNodeset(path + "/" + "namespaceZeroValues.xml"));
}

TEST(import, nodeclass)
{
    modernopc::Server server;
    ASSERT_TRUE(server.loadNodeset(path + "/" + "basicNodeClassTest.xml"));
}

TEST(import, notFound)
{
    modernopc::Server server;
    ASSERT_FALSE(server.loadNodeset("someNotExisting.xml"));
}

TEST(import, displayname)
{
    modernopc::Server server;
    ASSERT_TRUE(server.loadNodeset(path + "/" + "Displayname.xml"));
    ASSERT_TRUE(server.getObject(modernopc::NodeId(2, 6567))->displayName().text() ==
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