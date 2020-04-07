#include <opc/Server.h>
#include <opc/types/Types.h>
#include <opc/Variant.h>
#include <gtest/gtest.h>

std::string path = "";

TEST(import, namespaceZeroValues)
{

    opc::Server server;
    ASSERT_TRUE(server.loadNodeset(path + "/" + "namespaceZeroValues.xml"));

    auto nsIdx = server.getNamespaceIndex(
        "http://open62541.com/nodesetimport/tests/namespaceZeroValues");
    ASSERT_TRUE(nsIdx==2);
    opc::Variant var;
    ASSERT_TRUE(server.readValue(opc::NodeId(2, 1003), var));
    ASSERT_TRUE(var.get<double>() - 3.14 < 0.01);
    ASSERT_TRUE(server.readValue(opc::NodeId(2, 1004), var));
    ASSERT_TRUE(var.get<std::vector<uint>>()[2] == 140);
    ASSERT_TRUE(server.readValue(opc::NodeId(2, 1005), var));
    ASSERT_TRUE(var.getUAVariant()->type->typeIndex ==
                UA_TYPES_SERVERSTATUSDATATYPE);
    ASSERT_TRUE(static_cast<UA_ServerStatusDataType *>(var.getUAVariant()->data)
                    ->state == 5);
    ASSERT_TRUE(server.readValue(opc::NodeId(2, 1006), var));
    ASSERT_TRUE(
        static_cast<UA_ServerStatusDataType *>(var.getUAVariant()->data)[1]
            .state == 3);
    ASSERT_TRUE(server.readValue(opc::NodeId(2, 1007), var));
    ASSERT_TRUE(var.get<opc::LocalizedText>().text() == "someText@42");
    ASSERT_TRUE(var.get<opc::LocalizedText>().locale() == "en");
    ASSERT_TRUE(server.readValue(opc::NodeId(2, 1008), var));
    ASSERT_TRUE(var.get<std::vector<opc::LocalizedText>>()[0].text() ==
                "griasEich!");
    ASSERT_TRUE(var.get<std::vector<opc::LocalizedText>>()[1].text() ==
                "Hi!");
    ASSERT_TRUE(server.readValue(opc::NodeId(2, 1009), var));
    ASSERT_TRUE(var.get<opc::QualifiedName>().namespaceIndex() == 2);
    ASSERT_TRUE(var.get<opc::QualifiedName>().name() == "qualifiedName");
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