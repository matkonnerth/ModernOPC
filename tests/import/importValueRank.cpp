#include <gtest/gtest.h>
#include <opc/Server.h>
#include <iostream>
#include <opc/Variant.h>

std::string path = "";

TEST(import, valueRank)
{
    opc::Server server;
    ASSERT_TRUE(server.loadNodeset(path + "/" + "valuerank.xml"));


    opc::Variant var;
    ASSERT_TRUE(server.readValue(opc::NodeId(2, 6002), var));
    ASSERT_TRUE(var.get<int>()==1);
    ASSERT_TRUE(server.readValue(opc::NodeId(2, 6003), var));
    ASSERT_TRUE(var.get<std::vector<int>>()[1] == 13);
    ASSERT_TRUE(server.readValue(opc::NodeId(2, 6004), var));
    ASSERT_TRUE(var.get<std::vector<int>>()[2] == 300);
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