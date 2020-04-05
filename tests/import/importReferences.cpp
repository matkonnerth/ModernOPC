#include <opc/Server.h>
#include <gtest/gtest.h>
#include <iostream>
#include <open62541/server.h>

std::string path = "";

TEST(import, reference)
{
    opc::Server server;
    ASSERT_TRUE(server.loadNodeset(path + "/" + "importreferences.xml"));

    UA_QualifiedName objectName;
    UA_Server_readBrowseName(server.getUAServer(), UA_NODEID_NUMERIC(2,5002), &objectName);
    ASSERT_TRUE(objectName.namespaceIndex==2);
    UA_QualifiedName_clear(&objectName);

    UA_QualifiedName refName;
    UA_Server_readBrowseName(server.getUAServer(), UA_NODEID_NUMERIC(2, 4002),
                             &refName);
    ASSERT_TRUE(refName.namespaceIndex == 2);
    UA_QualifiedName_clear(&refName);
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