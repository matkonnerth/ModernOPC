#include <gtest/gtest.h>
#include <modernOpc/Server.h>
#include <modernOpc/Variant.h>
#include <modernOpc/nodes/VariableNode.h>
#include <open62541/server.h>

std::string path = "";
using opc::NodeId;

TEST(import, nodeclass)
{
    opc::Server server;
    ASSERT_TRUE(server.loadNodeset(path + "/" + "basicNodeClassTest.xml"));

    opc::Variant var;
    ASSERT_TRUE(server.getVariable(opc::NodeId(2, 6005))->read(var));

    UA_Byte accessLevel=0;
    UA_Server_readAccessLevel(server.getUAServer(), UA_NODEID_NUMERIC(2,6005), &accessLevel);
    ASSERT_TRUE(accessLevel==3);
    UA_Boolean exec = false;
    UA_Server_readExecutable(server.getUAServer(), UA_NODEID_NUMERIC(2, 7005),
                              &exec);
    ASSERT_TRUE(exec);
}

int main(int argc, char **argv)
{

    testing::InitGoogleTest(&argc, argv);

    if (!(argc > 1))
        return 1;
    path = argv[1];

    return RUN_ALL_TESTS();
}