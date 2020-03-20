#include <Server.h>
#include <Variant.h>
#include <gtest/gtest.h>
#include <Types.h>

std::string path = "";

TEST(import, namespaceZeroValues) {

    opc::Server server;
    ASSERT_TRUE(server.loadNodeset(path + "/" + "namespaceZeroValues.xml"));

    opc::Variant var;
    ASSERT_TRUE(server.readValue(opc::NodeId(2,1003), var));
    ASSERT_TRUE(var.get<double>()-3.14 < 0.01);
    ASSERT_TRUE(server.readValue(opc::NodeId(2, 1004), var));
    ASSERT_TRUE(var.get<std::vector<uint>>()[2]==140);
    ASSERT_TRUE(server.readValue(opc::NodeId(2, 1005), var));
    ASSERT_TRUE(var.getUAVariant()->type->typeIndex ==
                    UA_TYPES_SERVERSTATUSDATATYPE);
    ASSERT_TRUE(static_cast<UA_ServerStatusDataType*>(var.getUAVariant()->data)->state == 5);
    ASSERT_TRUE(server.readValue(opc::NodeId(2, 1006), var));
    ASSERT_TRUE(static_cast<UA_ServerStatusDataType *>(var.getUAVariant()->data)[1].state == 3);
    ASSERT_TRUE(server.readValue(opc::NodeId(2, 1007), var));
    ASSERT_TRUE(var.get<opc::types::LocalizedText>().text() == "someText@42");
    ASSERT_TRUE(var.get<opc::types::LocalizedText>().locale() == "en");
    ASSERT_TRUE(server.readValue(opc::NodeId(2, 1008), var));
    ASSERT_TRUE(var.get<std::vector<opc::types::LocalizedText>>()[0].text() == "griasEich!");
    ASSERT_TRUE(var.get<std::vector<opc::types::LocalizedText>>()[1].text() ==
                "Hi!");
    ASSERT_TRUE(server.readValue(opc::NodeId(2, 1009), var));
    ASSERT_TRUE(var.get<opc::types::QualifiedName>().namespaceIndex() == 2);
    ASSERT_TRUE(var.get<opc::types::QualifiedName>().name() == "qualifiedName");
}

int
main(int argc, char **argv) {

    testing::InitGoogleTest(&argc, argv);

    if(!(argc > 1))
        return 1;
    std::cout << argv[1] << std::endl;
    path = argv[1];

    return RUN_ALL_TESTS();
}