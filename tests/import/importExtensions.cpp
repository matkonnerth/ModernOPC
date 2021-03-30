#include <gtest/gtest.h>
#include <modernOpc/Server.h>
#include <open62541/server.h>

std::string path = "";

TEST(import, nodeclass)
{
    modernopc::Server server;
    ASSERT_TRUE(server.loadNodeset(path + "/" + "extension.xml"));
}

int main(int argc, char **argv)
{

    testing::InitGoogleTest(&argc, argv);

    if (!(argc > 1))
        return 1;
    path = argv[1];

    return RUN_ALL_TESTS();
}