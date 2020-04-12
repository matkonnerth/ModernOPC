#include <gtest/gtest.h>
#include <opc/Server.h>
#include <opc/Variant.h>
#include <opc/nodes/VariableNode.h>
#include <opc/types/LocalizedText.h>
#include <opc/types/QualifiedName.h>

std::string path = "";
using opc::NodeId;

TEST(import, namespaceZeroValues)
{

    opc::Server server;
    ASSERT_TRUE(server.loadNodeset(path + "/" + "namespaceZeroValues.xml"));

    auto nsIdx = server.getNamespaceIndex(
        "http://open62541.com/nodesetimport/tests/namespaceZeroValues");
    ASSERT_TRUE(nsIdx == 2);
    opc::Variant var;
    ASSERT_TRUE(server.getVariable(NodeId(2, 1003))->read(var));
    ASSERT_TRUE(var.get<double>() - 3.14 < 0.01);
    ASSERT_TRUE(server.getVariable(opc::NodeId(2, 1004))->read(var));
    ASSERT_TRUE(var.get<std::vector<uint>>()[2] == 140);
    ASSERT_TRUE(server.getVariable(opc::NodeId(2, 1005))->read(var));

    ASSERT_TRUE(var.getUAVariant()->type->typeIndex ==
                UA_TYPES_SERVERSTATUSDATATYPE);
    ASSERT_TRUE(static_cast<UA_ServerStatusDataType *>(var.getUAVariant()->data)
                    ->state == 5);
    ASSERT_TRUE(server.getVariable(opc::NodeId(2, 1006))->read(var));
    ASSERT_TRUE(
        static_cast<UA_ServerStatusDataType *>(var.getUAVariant()->data)[1]
            .state == 3);
    ASSERT_TRUE(server.getVariable(opc::NodeId(2, 1007))->read(var));
    ASSERT_TRUE(var.get<opc::LocalizedText>().text() == "someText@42");
    ASSERT_TRUE(var.get<opc::LocalizedText>().locale() == "en");
    ASSERT_TRUE(server.getVariable(opc::NodeId(2, 1008))->read(var));
    ASSERT_TRUE(var.get<std::vector<opc::LocalizedText>>()[0].text() ==
                "griasEich!");
    ASSERT_TRUE(var.get<std::vector<opc::LocalizedText>>()[1].text() == "Hi!");
    ASSERT_TRUE(server.getVariable(opc::NodeId(2, 1009))->read(var));
    ASSERT_TRUE(var.get<opc::QualifiedName>().namespaceIndex() == 2);
    ASSERT_TRUE(var.get<opc::QualifiedName>().name() == "qualifiedName");
}

TEST(import, primitiveValues)
{

    opc::Server server;
    ASSERT_TRUE(server.loadNodeset(path + "/" + "primitivevalues.xml"));

    auto nsIdx = server.getNamespaceIndex(
        "http://yourorganisation.org/primitiveValues/");
    ASSERT_TRUE(nsIdx == 2);
    opc::Variant var;
    ASSERT_TRUE(server.getVariable(opc::NodeId(2, 6005))->read(var));
    ASSERT_EQ(var.get<int32_t>(), -3434);
    ASSERT_TRUE(server.getVariable(opc::NodeId(2, 6012))->read(var));
    ASSERT_EQ(var.get<int8_t>(), -128);
    ASSERT_TRUE(server.getVariable(opc::NodeId(2, 6011))->read(var));
    ASSERT_EQ(var.get<double>(), -1.0);
    ASSERT_TRUE(server.getVariable(opc::NodeId(2, 6010))->read(var));
    ASSERT_EQ(var.get<float>(), -1.0f);
}

TEST(import, unknownDataType)
{

    opc::Server server;
    ASSERT_TRUE(
        server.loadNodeset(path + "/" + "valuesWithUnknownDataType.xml"));

    auto nsIdx = server.getNamespaceIndex(
        "http://yourorganisation.org/primitiveValues/");
    ASSERT_TRUE(nsIdx == 2);
    opc::Variant var;
    ASSERT_TRUE(server.getVariable(NodeId(2, 6005))->read(var));
    ASSERT_EQ(var.get<int32_t>(), -3434);
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