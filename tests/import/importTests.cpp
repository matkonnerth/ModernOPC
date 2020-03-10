#include <gtest/gtest.h>
#include <Server.h>
#include <iostream>

std::string path = "";

TEST(import, values)
{
    opc::Server server;
    ASSERT_TRUE(server.loadNodeset(path + "/" + "namespaceZeroValues.xml"));
}

TEST(import, nodeclass) {
    opc::Server server;
    ASSERT_TRUE(server.loadNodeset(path + "/" + "basicNodeClassTest.xml"));
}

TEST(import, speed) {
    opc::Server server;
    ASSERT_TRUE(server.loadNodeset(path + "/" + "100kNodes.xml"));
}

TEST(import, notFound)
{
    opc::Server server;
    ASSERT_FALSE(server.loadNodeset("someNotExisting.xml"));
}

int main(int argc, char**argv)
{

    testing::InitGoogleTest(&argc, argv);
    
    if(!(argc>1))
        return 1;
    std::cout << argv[1] << std::endl;
    path = argv[1];


    return RUN_ALL_TESTS();
}