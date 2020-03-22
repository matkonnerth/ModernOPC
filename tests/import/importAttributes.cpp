#include <Server.h>
#include <gtest/gtest.h>
#include <iostream>

std::string path = "";

TEST(import, nodeclass)
{
    opc::Server server;
    ASSERT_TRUE(server.loadNodeset(path + "/" + "basicNodeClassTest.xml"));

    opc::Variant var;
    ASSERT_TRUE(server.readValue(opc::NodeId(2, 6005), var));
}

int main(int argc, char **argv)
{

    testing::InitGoogleTest(&argc, argv);

    if (!(argc > 1))
        return 1;
    std::cout << argv[1] << std::endl;
    path = argv[1];

    return RUN_ALL_TESTS();
}